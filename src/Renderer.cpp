#include "Renderer.hpp"
#include "Utils.hpp"
#include "TextureType.hpp"

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
    texDiffuse = nullptr;
    texSpecular = nullptr;

    ResetParams();
}

void Renderer::ResetParams()
{
    backfaceCulling = true;
    perspectiveCorrection = true;

    shading = None;

    FOV = 90.0f;
    camPos = glm::vec3(0.0f, 0.0f, 1.0f);
    modelPos = glm::vec3(0.0f);
    modelRot = glm::vec3(0.0f);
    modelScale = glm::vec3(1.0f);
    lightDir = glm::vec2(0.0f);

    ambientFactor = 0.1f;
    lambertFactor = 0.4f;
    spec1 = 20.0f;
    spec2 = 0.5f;
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
        buffer = new uint8_t[width * height * 3];
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

    glm::vec4 a = glm::vec4(va.v, 1.0f),
        b = glm::vec4(vb.v, 1.0f),
        c = glm::vec4(vc.v, 1.0f);

    glm::vec4 tangA(va.tangent, 0.0f),
        tangB(vb.tangent, 0.0f),
        tangC(vc.tangent, 0.0f);

    a = modelMat * a;
    b = modelMat * b;
    c = modelMat * c;

    glm::vec4 na = modelMat * glm::vec4(va.n, 0.0f),
            nb = modelMat * glm::vec4(vb.n, 0.0f),
            nc = modelMat * glm::vec4(vc.n, 0.0f);

    na = glm::normalize(na);
    nb = glm::normalize(nb);
    nc = glm::normalize(nc);

    tangA = glm::normalize(modelMat * tangA);
    tangB = glm::normalize(modelMat * tangB);
    tangC = glm::normalize(modelMat * tangC);

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

    tangA = glm::normalize(viewMat * tangA);
    tangB = glm::normalize(viewMat * tangB);
    tangC = glm::normalize(viewMat * tangC);

    va.tangent = tangA;
    vb.tangent = tangB;
    vc.tangent = tangC;

    a = projMat * a;
    b = projMat * b;
    c = projMat * c;

    if (a.z < Renderer::zNear || a.z > Renderer::zFar ||
        b.z < Renderer::zNear || b.z > Renderer::zFar ||
        c.z < Renderer::zNear || c.z > Renderer::zFar)
    {
        return;
    }

    if (backfaceCulling && canCull(a, b, c))
    {
        ++culledFaces;
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
        drawBottomTriangle(va, vb, vc);
    }
    else if (vb.v.y == vc.v.y)
    {
        // natural top triangle
        if (vb.v.x > vc.v.x)
        {
            swap(vb, vc);
        }
        drawTopTriangle(va, vb, vc);
    }
    else // general triangle
    {
        const float ratio = (vb.v.y - va.v.y) / (vc.v.y - va.v.y);
        const float newX = (1.0f - ratio) * va.v.x + ratio * vc.v.x;

        if (vb.v.x < newX)
        {
            // left triangle
            drawLeftTriangle(va, vb, vc);
        }
        else
        {
            // right triangle
            drawRightTriangle(va, vb, vc);
        }
    }
}

void Renderer::drawLeftTriangle(Vertex va, Vertex vb, Vertex vc)
{
    const glm::vec3& a = va.v,
        b = vb.v,
        c = vc.v;

    float mLeft = (b.x - a.x) / (b.y - a.y),
        mRight = (c.x - a.x) / (c.y - a.y);

    glm::vec3 brLeft(1.0f, 0.0f, 0.0f),
        brRight(1.0f, 0.0f, 0.0f);

    glm::vec3 brStepLeft = glm::vec3(-1.0f, 1.0f, 0.0f) / (b.y - a.y);
    const glm::vec3 brStepRight = glm::vec3(-1.0f, 0.0f, 1.0f) / (c.y - a.y);

    int yStart = std::ceil(a.y - 0.5f),
        yEnd = std::ceil(b.y - 0.5f);

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
            drawFragment(br, x, y, va, vb, vc);

            br += brScanStep;
        }

        brLeft += brStepLeft;
        brRight += brStepRight;
    }

    brLeft = glm::vec3(0.0f, 1.0f, 0.0f);

    const float rt = (b.y - a.y) / (c.y - a.y);
    brRight = glm::vec3(1.0f - rt, 0.0f, rt);

    brStepLeft = glm::vec3(0.0f, -1.0f, 1.0f) / (c.y - b.y);

    yStart = yEnd;
    yEnd = std::ceil(c.y - 0.5f);

    mLeft = (c.x - b.x) / (c.y - b.y);

    brLeft += brStepLeft * (yStart - b.y + 0.5f);
    brRight += brStepRight * (yStart - b.y + 0.5f);

    for (int y = yStart; y < yEnd; ++y)
    {
        const float pxLeft = mLeft * (y - b.y + 0.5f) + b.x,
            pxRight = mRight * (y - a.y + 0.5f) + a.x;

        const int xStart = std::ceil(pxLeft - 0.5f),
            xEnd = std::ceil(pxRight - 0.5f);

        const glm::vec3 brScanStep = (brRight - brLeft) / (pxRight - pxLeft);

        glm::vec3 br = brLeft + brScanStep * (xStart - pxLeft + 0.5f);

        for (int x = xStart; x < xEnd; ++x)
        {
            drawFragment(br, x, y, va, vb, vc);

            br += brScanStep;
        }

        brLeft += brStepLeft;
        brRight += brStepRight;
    }
}

void Renderer::drawRightTriangle(Vertex va, Vertex vb, Vertex vc)
{
    const glm::vec3& a = va.v,
        b = vb.v,
        c = vc.v;

    const float mLeft = (c.x - a.x) / (c.y - a.y);
    float mRight = (b.x - a.x) / (b.y - a.y);

    glm::vec3 brLeft(1.0f, 0.0f, 0.0f),
        brRight(1.0f, 0.0f, 0.0f);

    const glm::vec3 brStepLeft = glm::vec3(-1.0f, 0.0f, 1.0f) / (c.y - a.y);
    glm::vec3 brStepRight = glm::vec3(-1.0f, 1.0f, 0.0f) / (b.y - a.y);

    int yStart = std::ceil(a.y - 0.5f),
        yEnd = std::ceil(b.y - 0.5f);

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
            drawFragment(br, x, y, va, vb, vc);

            br += brScanStep;
        }

        brLeft += brStepLeft;
        brRight += brStepRight;
    }

    brRight = glm::vec3(0.0f, 1.0f, 0.0f);

    const float rt = (b.y - a.y) / (c.y - a.y);
    brLeft = glm::vec3(1.0f - rt, 0.0f, rt);

    brStepRight = glm::vec3(0.0f, -1.0f, 1.0f) / (c.y - b.y);

    yStart = yEnd;
    yEnd = std::ceil(c.y - 0.5f);

    mRight = (c.x - b.x) / (c.y - b.y);

    brLeft += brStepLeft * (yStart - b.y + 0.5f);
    brRight += brStepRight * (yStart - b.y + 0.5f);

    for (int y = yStart; y < yEnd; ++y)
    {
        const float pxLeft = mLeft * (y - a.y + 0.5f) + a.x,
            pxRight = mRight * (y - b.y + 0.5f) + b.x;

        const int xStart = std::ceil(pxLeft - 0.5f),
            xEnd = std::ceil(pxRight - 0.5f);

        const glm::vec3 brScanStep = (brRight - brLeft) / (pxRight - pxLeft);

        glm::vec3 br = brLeft + brScanStep * (xStart - pxLeft + 0.5f);

        for (int x = xStart; x < xEnd; ++x)
        {
            drawFragment(br, x, y, va, vb, vc);

            br += brScanStep;
        }

        brLeft += brStepLeft;
        brRight += brStepRight;
    }
}

void Renderer::drawTopTriangle(Vertex va, Vertex vb, Vertex vc)
{
    const glm::vec3& a = va.v,
        b = vb.v,
        c = vc.v;

    const float mLeft = (b.x - a.x) / (c.y - a.y),
        mRight = (c.x - a.x) / (c.y - a.y);

    glm::vec3 brLeft(1.0f, 0.0f, 0.0f),
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
            drawFragment(br, x, y, va, vb, vc);

            br += brScanStep;
        }

        brLeft += brStepLeft;
        brRight += brStepRight;
    }
}

void Renderer::drawBottomTriangle(Vertex va, Vertex vb, Vertex vc)
{
    const glm::vec3& a = va.v,
        b = vb.v,
        c = vc.v;

    const float mLeft = (c.x - a.x) / (c.y - a.y),
        mRight = (c.x - b.x) / (c.y - a.y);

    glm::vec3 brLeft(1.0f, 0.0f, 0.0f),
        brRight(0.0f, 1.0f, 0.0f);

    const glm::vec3 brStepLeft = glm::vec3(-1.0f, 0.0f, 1.0f) / (c.y - a.y),
        brStepRight = glm::vec3(0.0f, -1.0f, 1.0f) / (c.y - a.y);

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
            drawFragment(br, x, y, va, vb, vc);

            br += brScanStep;
        }

        brLeft += brStepLeft;
        brRight += brStepRight;
    }
}

void Renderer::drawFragment(const glm::vec3 br, const int x, const int y,
                            const Vertex va, const Vertex vb, const Vertex vc)
{
    const float z = Interpolate(br, va.v.z, vb.v.z, vc.v.z);
    glm::vec3 n = InterpolateNormals(br, va.n, vb.n, vc.n),
        tangent = InterpolateNormals(br, va.tangent, vb.tangent, vc.tangent);
    const glm::vec3 posView = Interpolate(br, va.posView, vb.posView, vc.posView);

    glm::vec2 t;

    if (perspectiveCorrection)
    {
        const float dv = Interpolate(br, 1.0f / va.posView.z, 1.0f / vb.posView.z, 1.0f / vc.posView.z);
        t = Interpolate(br, va.t / va.posView.z, vb.t / vb.posView.z, vc.t / vc.posView.z) / dv;
    }
    else
    {
        t = Interpolate(br, va.t, vb.t, vc.t);
    }

    glm::vec3 baseCol = texDiffuse->getCol(t.x, t.y),
        pCol;

    n = calcNormal(n, tangent, t);

    switch (shading)
    {
    case None:
        pCol = baseCol;
        break;

    case PBR:
        pCol = getPBR(n, posView, baseCol, texMetallic->getVal(t.x, t.y),
                      texRoughness->getVal(t.x, t.y), texAO->getVal(t.x, t.y),
                      texEmission->getCol(t.x, t.y));
        break;

    case Smooth:
        const glm::vec3 bps = calcBlinnPhongShading(posView, n);
        const glm::vec3 cSpec = texSpecular->getCol(t.x, t.y) * bps.z;

        pCol = baseCol * (bps.x + bps.y) + cSpec;
        break;
    }

    setPixel(x, y, z, pCol);
}

void Renderer::setPixel(const int x, const int y, const float z, glm::vec3 c)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        return;
    }

    const int ind = index(y, x);

    if (zBuffer[ind] > z)
    {
        zBuffer[ind] = z;

        c = glm::min(glm::vec3(1.0f), c);

        buffer[ind * 3] = std::round(c.x * 255.0f);
        buffer[ind * 3 + 1] = std::round(c.y * 255.0f);
        buffer[ind * 3 + 2] = std::round(c.z * 255.0f);
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
    if (model != nullptr)
    {
        delete model;
    }

    model = new Model("model" + filename + ".obj");
}

void Renderer::LoadDiffuse(const std::string& filename)
{
    if (texDiffuse != nullptr)
    {
        delete texDiffuse;
    }

    texDiffuse = new Texture("diffuse" + filename + ".png", Diffuse);
}

void Renderer::LoadSpecular(const std::string& filename)
{
    if (texSpecular != nullptr)
    {
        delete texSpecular;
    }

    texSpecular = new Texture("specular" + filename + ".png", Specular);
}

void Renderer::LoadNormal(const std::string& filename)
{
    if (texNormal != nullptr)
    {
        delete texNormal;
    }

    texNormal = new NormalTexture("normal" + filename + ".png");
}

void Renderer::LoadEmission(const std::string& filename)
{
    if (texEmission != nullptr)
    {
        delete texEmission;
    }

    texEmission = new Texture("emission" + filename + ".png", Emission);
}

void Renderer::LoadMetallic(const std::string& filename)
{
    if (texMetallic != nullptr)
    {
        delete texMetallic;
    }

    texMetallic = new MonoTexture("metallic" + filename + ".png", Metallic);
}

void Renderer::LoadRoughness(const std::string& filename)
{
    if (texRoughness != nullptr)
    {
        delete texRoughness;
    }

    texRoughness = new MonoTexture("roughness" + filename + ".png", Roughness);
}

void Renderer::LoadAO(const std::string& filename)
{
    if (texAO != nullptr)
    {
        delete texAO;
    }

    texAO = new MonoTexture("ao" + filename + ".png", Ambient);
}

int Renderer::index(int i, int j) const
{
    return i * width + j;
}

glm::vec3 Renderer::calcNormal(const glm::vec3 n, glm::vec3 tangent, const glm::vec2 t)
{
    tangent = glm::normalize(tangent - glm::dot(tangent, n) * n);
    const glm::vec3 bitangent = glm::cross(tangent, n);
    glm::vec3 mapNormal = texNormal->getNormal(t.x, t.y);

    const glm::mat3 tbn(tangent, bitangent, n);

    mapNormal = glm::normalize(tbn * mapNormal);

    return mapNormal;
}

glm::vec3 Renderer::calcBlinnPhongShading(const glm::vec3 p, const glm::vec3 n)
{
    const float l1 = glm::dot(lightVecView, -n) * lambertFactor,
        l2 = ambientFactor;

    const glm::vec3 eyeDir = glm::normalize(p);

    float kSp = -glm::dot(n, glm::normalize(eyeDir + lightVecView));

    kSp = std::max(0.0f, kSp);

    kSp = std::pow(kSp, spec1) * spec2;

    return glm::vec3(l2, std::max(0.0f, l1), kSp);
}

glm::vec3 Renderer::getPBR(glm::vec3 n, glm::vec3 pos, glm::vec3 albedo,
                           float metallic, float roughness, float ao,
                           glm::vec3 emission)
{
    albedo = glm::pow(albedo, glm::vec3(2.2f));
    const glm::vec3 l = -lightVecView,
        v = glm::normalize(-pos),
        h = glm::normalize(v + l),
        F0 = glm::mix(glm::vec3(0.04f), albedo, metallic),
        F = Utils::FresnelSchlick(std::max(glm::dot(h, v), 0.0f), F0);

    const float NDF = Utils::DistributionGGX(h, n, roughness),
        G = Utils::GeometrySmith(n, v, l, roughness),
        NdotL = std::max(glm::dot(n, l), 0.0f);

    const glm::vec3 numerator = NDF * G * F;
    float denominator = 4.0f * std::max(glm::dot(n, v), 0.0f) * NdotL;

    denominator = std::max(denominator, 0.000001f);

    const glm::vec3 specular = numerator / denominator;

    const glm::vec3& kS = F;
    const glm::vec3 kD = (1.0f - kS) * (1.0f - metallic),
        Lo = emission + (kD * albedo / Utils::pi + specular) * NdotL,
        ambient = glm::vec3(0.03f) * albedo * ao;

    glm::vec3 color = ambient + Lo;

    color = color / (color + 1.0f);
    color = glm::pow(color, glm::vec3(1.0f / 2.2f));

    return color;
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

bool Renderer::canCull(const glm::vec2 a, const glm::vec2 b, const glm::vec2 c)
{
    const float cull = Utils::perpDotProduct(b - a, c - a);

    return cull <= 0.0f;
}
