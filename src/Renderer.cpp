#include "Renderer.hpp"

#include <cstring>
#include <cmath>
#include <algorithm>

#include <glm/ext.hpp>
#include <glm/gtx/euler_angles.hpp>

Renderer::Renderer()
{
    buffer = nullptr;
    zBuffer = nullptr;
    model = nullptr;

    backfaceCulling = true;

    FOV = 90.0f;
    camPos = glm::vec3(0.0f, 0.0f, 1.0f);
    modelScale = glm::vec3(1.0f);
    lightDir = glm::vec2(0.0f);

    ambientFactor = 0.1f;
    lambertFactor = 0.4f;
    spec1 = 20.0f;
    spec2 = 0.5f;

    shading = None;
}

const void* Renderer::Render(int width, int height, bool sizeChanged)
{
    using glm::vec4;

    this->width = width;
    this->height = height;

    genProjectionMatrix();
    genViewportMatrix();
    genViewMatrix();
    genModelMatrix();
    genLightVec();

    if (sizeChanged && buffer != nullptr)
    {
        char *ptr = (char*)buffer;
        delete[] ptr;
        delete[] zBuffer;
    }

    if (sizeChanged || buffer == nullptr)
    {
        buffer = (Color*)new char[width * height * 3];
        zBuffer = new float[width * height];
    }

    memset((void*)buffer, 0, width * height * 3);
    for (int i = 0; i < width * height; ++i)
    {
        zBuffer[i] = 1.0f;
    }

    culledFaces = 0;

    renderModel();

    if (backfaceCulling)
    {
        printf("Culled faces: %d\n", culledFaces);
    }

    return buffer;
}

void Renderer::drawTriangle(Vertex va, Vertex vb, Vertex vc)
{
    using std::swap;

    Color col = Color::white();

    glm::vec4 a = glm::vec4(va.v, 1.0f),
        b = glm::vec4(vb.v, 1.0f),
        c = glm::vec4(vc.v, 1.0f);

    a = modelMat * a;
    b = modelMat * b;
    c = modelMat * c;

    glm::vec4 na = modelMat * glm::vec4(va.n, 0.0f),
            nb = modelMat * glm::vec4(vb.n, 0.0f),
            nc = modelMat * glm::vec4(vc.n, 0.0f);

    na = glm::normalize(na);
    nb = glm::normalize(nb);
    nc = glm::normalize(nc);

    a = viewMat * a;
    b = viewMat * b;
    c = viewMat * c;

    na = viewMat * na;
    nb = viewMat * nb;
    nc = viewMat * nc;

    na = glm::normalize(na);
    nb = glm::normalize(nb);
    nc = glm::normalize(nc);

    va.n = na;
    vb.n = nb;
    vc.n = nc;

    va.posView = a;
    vb.posView = b;
    vc.posView = c;

    if (shading == Flat)
    {
        const float l1 = calcLighting(na),
            l2 = calcLighting(nb),
            l3 = calcLighting(nc);

        const float totalLighting = std::clamp((l1 + l2 + l3) / 3.0f + ambientFactor, 0.0f, 1.0f);

        col = Color::flat(totalLighting);
    }

    if (backfaceCulling && canCull(a, b, c))
    {
        ++culledFaces;
        return;
    }

    a = projMat * a;
    b = projMat * b;
    c = projMat * c;

    if (a.z < Renderer::zNear || a.z > Renderer::zFar ||
        b.z < Renderer::zNear || b.z > Renderer::zFar ||
        c.z < Renderer::zNear || c.z > Renderer::zFar)
    {
        return;
    }

    a /= a.w;
    b /= b.w;
    c /= c.w;

    a = viewportMat * a;
    b = viewportMat * b;
    c = viewportMat * c;

    va.v = a;
    vb.v = b;
    vc.v = c;

    if (va.v.y > vb.v.y)
    {
        swap(va, vb);
    }

    if (vb.v.y > vc.v.y)
    {
        swap(vb, vc);
    }

    if (va.v.y > vb.v.y)
    {
        swap(va, vb);
    }

    if (va.v.y == vb.v.y)
    {
        // natural bottom triangle
        if (va.v.x > vb.v.x)
        {
            swap(va, vb);
        }
        drawBottomTriangle(va, vb, vc, col);
    }
    else if (vb.v.y == vc.v.y)
    {
        // natural top triangle
        if (vb.v.x > vc.v.x)
        {
            swap(vb, vc);
        }
        drawTopTriangle(va, vb, vc, col);
    }
    else // general triangle
    {
        const float ratio = (vb.v.y - va.v.y) / (vc.v.y - va.v.y);

        Vertex newV = Vertex::Combine(va, vc, ratio);

        if (vb.v.x < newV.v.x)
        {
            // new vertex on the right edge
            drawTopTriangle(va, vb, newV, col);
            drawBottomTriangle(vb, newV, vc, col);
        }
        else
        {
            // new vertex on the left edge
            drawTopTriangle(va, newV, vb, col);
            drawBottomTriangle(newV, vb, vc, col);
        }
    }
}

void Renderer::drawTopTriangle(Vertex va, Vertex vb, Vertex vc, Color col)
{
    const glm::vec3& a = va.v,
        b = vb.v,
        c = vc.v;

    const float mLeft = (b.x - a.x) / (c.y - a.y),
        mRight = (c.x - a.x) / (c.y - a.y);

    glm::vec3 brLeft(1.0f, 0.0f, 0.0f),
        brTop(1.0f, 0.0f, 0.0f),
        brRight(1.0f, 0.0f, 0.0f);

    const glm::vec3 brStepLeft = glm::vec3(-1.0f, 1.0f, 0.0f) / (c.y - a.y),
        brStepRight = glm::vec3(-1.0f, 0.0f, 1.0f) / (c.y - a.y);

    const int yStart = std::ceil(a.y - 0.5f),
        yEnd = std::ceil(c.y - 0.5f);

    brLeft += brStepLeft * (yStart - a.y + 0.5f);
    brRight += brStepRight * (yStart - a.y + 0.5f);

    for (int y = yStart; y < yEnd; ++y)
    {
        const float pxLeft = mLeft * (y - a.y + 0.5f) + a.x,
            pxRight = mRight * (y - a.y + 0.5f) + a.x;

        const int xStart = std::ceil(pxLeft - 0.5f),
            xEnd = std::ceil(pxRight - 0.5f);

        const glm::vec3 brScanStep = (brRight - brLeft) / (pxRight - pxLeft);

        glm::vec3 br = brLeft + brScanStep * (xStart - pxLeft + 0.5f);

        for (int x = xStart; x < xEnd; ++x)
        {
            drawFragment(br, x, y, va, vb, vc, col);

            br += brScanStep;
        }

        brLeft += brStepLeft;
        brRight += brStepRight;
    }
}

void Renderer::drawBottomTriangle(Vertex va, Vertex vb, Vertex vc, Color col)
{
    const glm::vec3& a = va.v,
        b = vb.v,
        c = vc.v;

    const float mLeft = (c.x - a.x) / (c.y - a.y),
        mRight = (c.x - b.x) / (c.y - a.y);

    glm::vec3 brLeft(1.0f, 0.0f, 0.0f),
        brBottom(0.0f, 0.0f, 1.0f),
        brRight(0.0f, 1.0f, 0.0f);

    const glm::vec3 brStepLeft = (brBottom - brLeft) / (c.y - a.y),
        brStepRight = (brBottom - brRight) / (c.y - a.y);

    const int yStart = std::ceil(a.y - 0.5f),
        yEnd = std::ceil(c.y - 0.5f);

    brLeft += brStepLeft * (yStart - a.y + 0.5f);
    brRight += brStepRight * (yStart - a.y + 0.5f);

    for (int y = yStart; y < yEnd; ++y)
    {
        const float pxLeft = mLeft * (y - a.y + 0.5f) + a.x,
            pxRight = mRight * (y - a.y + 0.5f) + b.x;

        const int xStart = std::ceil(pxLeft - 0.5f),
            xEnd = std::ceil(pxRight - 0.5f);

        const glm::vec3 brScanStep = (brRight - brLeft) / (pxRight - pxLeft);

        glm::vec3 br = brLeft + brScanStep * (xStart - pxLeft + 0.5f);

        for (int x = xStart; x < xEnd; ++x)
        {
            drawFragment(br, x, y, va, vb, vc, col);

            br += brScanStep;
        }

        brLeft += brStepLeft;
        brRight += brStepRight;
    }
}

void Renderer::drawFragment(const glm::vec3 br, const int x, const int y,
                            const Vertex va, const Vertex vb, const Vertex vc,
                            const Color col)
{
    const float z = Interpolate(br, va.v.z, vb.v.z, vc.v.z);

    Color pCol = col;

    if (shading == Smooth)
    {
        const glm::vec3 n = InterpolateNormals(br, va.n, vb.n, vc.n),
            posView = Interpolate(br, va.posView, vb.posView, vc.posView);

        const float l = calcPhongShading(posView, n);
        pCol = Color::mul(col, l);
    }

    setPixel(x, y, z, pCol);
}

void Renderer::setPixel(const int x, const int y, const float z, const Color c)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        return;
    }

    const int ind = index(y, x);

    if (zBuffer[ind] > z)
    {
        zBuffer[ind] = z;
        buffer[ind] = c;
    }
}

void Renderer::genModelMatrix()
{
    using glm::mat4;

    const mat4 trMat(1.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f,
                     modelPos.x, modelPos.y, modelPos.z, 1.0f);

    const glm::vec3 rot = glm::radians(modelRot);
    const mat4 rotMat = glm::eulerAngleXYZ(rot.x, rot.y, rot.z);

    mat4 scaleMat(modelScale.x, 0.0f, 0.0f, 0.0f,
                  0.0f, modelScale.y, 0.0f, 0.0f,
                  0.0f, 0.0f, modelScale.z, 0.0f,
                  0.0f, 0.0f, 0.0f, 1.0f);

    modelMat = trMat * rotMat * scaleMat;
}

void Renderer::genProjectionMatrix()
{
    const float fov = glm::radians(this->FOV),
        aspect = (float)width / height,
        t = std::tan(fov / 2.0f),
        znear = Renderer::zNear, zfar = Renderer::zFar;

    projMat = glm::mat4(1.0f / (aspect * t), 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f / t, 0.0f, 0.0f,
                        0.0f, 0.0f, zfar / (znear - zfar), -1.0f,
                        0.0f, 0.0f, (znear * zfar) / (znear - zfar), 0.0f);
}

void Renderer::genViewMatrix()
{
    const glm::vec3 up(0.0f, 1.0f, 0.0f),
        target(0.0f);

    viewMat = glm::lookAt(camPos, target, up);
}

void Renderer::genLightVec()
{
    glm::vec2 angle = glm::radians(lightDir);
    glm::vec4 v = glm::eulerAngleYX(-angle.x, angle.y) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    lightVec = v;

    lightVecView = viewMat * glm::vec4(lightVec, 0.0f);
    lightVecView = glm::normalize(lightVecView);
}

void Renderer::renderModel()
{
    if (model == nullptr)
    {
        return;
    }

    for (size_t i = 0; i < model->faces.size(); ++i)
    {
        const Face f = model->faces[i];

        Vertex va, vb, vc;
        model->getVertices(f, va, vb, vc);

        drawTriangle(va, vb, vc);
    }
}

void Renderer::genViewportMatrix()
{
    using glm::vec4;

    viewportMat = glm::mat4(width / 2.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, -height / 2.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            width / 2.0f, height / 2.0f, 0.0f, 1.0f);
}

void Renderer::LoadModel(const std::string& filename)
{
    model = new Model(filename);
}

void Renderer::drawLine(float x0, float y0, float x1, float y1)
{
    float dx = x1 - x0,
        dy = y1 - y0;

    const float step = std::max(abs(dx), abs(dy));

    dx /= step;
    dy /= step;

    for (int i = 0, iStep = (int)step; i < iStep; ++i, x0 += dx, y0 += dy)
    {
        if (x0 < 0.0f || x0 >= width || y0 < 0.0f || y0 >= height)
        {
            continue;
        }

        setPixel(x0, y0, 0.0f, Color::white());
    }
}


int Renderer::index(int i, int j) const
{
    return i * width + j;
}

float Renderer::calcLighting(const glm::vec3 n)
{
    const float res = glm::dot(lightVecView, -n);

    return std::clamp(res, 0.0f, 1.0f);
}

float Renderer::calcPhongShading(const glm::vec3 p, const glm::vec3 n)
{
    const float l1 = glm::dot(lightVecView, -n) * lambertFactor,
        l2 = ambientFactor;

    const glm::vec3 eyeDir = glm::normalize(p),
        reflRay = glm::reflect(lightVecView, n);

    float kSp = -glm::dot(eyeDir, reflRay);

    kSp = std::max(0.0f, kSp);

    kSp = std::pow(kSp, spec1);

    kSp *= spec2;

    return std::clamp(l1 + l2 + kSp, 0.0f, 1.0f);
}

template<typename T>
T Renderer::Interpolate(const glm::vec3 br, const T a, const T b, const T c)
{
    return a * br.x + b * br.y + c * br.z;
}

glm::vec3 Renderer::InterpolateNormals(const glm::vec3 br, const glm::vec3 a, const glm::vec3 b, const glm::vec3 c)
{
    return glm::normalize(a * br.x + b * br.y + c * br.z);
}

bool Renderer::canCull(const glm::vec3 a, const glm::vec3 b, const glm::vec3 c)
{
    const float cull = glm::dot(glm::cross(b - a, c - a), a);

    return cull >= 0.0f;
}
