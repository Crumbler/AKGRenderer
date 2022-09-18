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

    FOV = 90.0f;
    camPos = glm::vec3(0.0f, 0.0f, 1.0f);
    modelScale = glm::vec3(1.0f);
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

    viewProjMat = projMat * viewMat;

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

    renderModel();

    return buffer;
}

void Renderer::drawTriangle(glm::vec4 a, glm::vec4 b, glm::vec4 c, Color col)
{
    using std::swap;

    a = modelMat * a;
    b = modelMat * b;
    c = modelMat * c;

    a = viewProjMat * a;
    b = viewProjMat * b;
    c = viewProjMat * c;

    a /= a.w;
    b /= b.w;
    c /= c.w;

    a = viewportMat * a;
    b = viewportMat * b;
    c = viewportMat * c;

    glm::vec3 zs = glm::abs(glm::vec3(a.z, b.z, c.z));
    if (zs.x > 1.0f || zs.y > 1.0f || zs.z > 1.0f)
    {
        return;
    }

    if (a.y > b.y)
    {
        swap(a, b);
    }

    if (b.y > c.y)
    {
        swap(b, c);
    }

    if (a.y > b.y)
    {
        swap(a, b);
    }

    if (a.y == b.y)
    {
        // natural bottom triangle
        if (a.x > b.x)
        {
            swap(a, b);
        }
        drawBottomTriangle(a, b, c, col);
    }
    else if (b.y == c.y)
    {
        // natural top triangle
        if (b.x > c.x)
        {
            swap(b, c);
        }
        drawTopTriangle(a, b, c, col);
    }
    else // general triangle
    {
        const float ratio = (b.y - a.y) / (c.y - a.y);

        const glm::vec3 newV = a + (c - a) * ratio;

        if (b.x < newV.x)
        {
            // new vertex on the right edge
            drawTopTriangle(a, b, newV, col);
            drawBottomTriangle(b, newV, c, col);
        }
        else
        {
            // new vertex on the left edge
            drawTopTriangle(a, newV, b, col);
            drawBottomTriangle(newV, b, c, col);
        }
    }
}

void Renderer::drawTopTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, Color col)
{
    const float mLeft = (b.x - a.x) / (b.y - a.y),
        mRight = (c.x - a.x) / (c.y - a.y);

    glm::vec3 brLeft(0.0f, 1.0f, 0.0f),
        brTop(1.0f, 0.0f, 0.0f),
        brRight(0.0f, 0.0f, 1.0f);

    const glm::vec3 brStepLeft = (brLeft - brTop) / (b.y - a.y),
        brStepRight = (brRight - brTop) / (c.y - a.y);

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
            const float z = Interpolate(br, a.z, b.z, c.z);

            setPixel(x, y, z, col);

            br += brScanStep;
        }

        brLeft += brStepLeft;
        brRight += brStepRight;
    }
}

void Renderer::drawBottomTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, Color col)
{
    const float mLeft = (c.x - a.x) / (c.y - a.y),
        mRight = (c.x - b.x) / (c.y - a.y);

    glm::vec3 brLeft(1.0f, 0.0f, 0.0f),
        brBottom(0.0f, 0.0f, 1.0f),
        brRight(0.0f, 1.0f, 0.0f);

    const glm::vec3 brStepLeft = (brBottom - brLeft) / (c.y - a.y),
        brStepRight = (brBottom - brRight) / (c.y - b.y);

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
            const float z = Interpolate(br, a.z, b.z, c.z);

            setPixel(x, y, z, col);

            br += brScanStep;
        }

        brLeft += brStepLeft;
        brRight += brStepRight;
    }
}

void Renderer::drawLine(glm::vec4 a, glm::vec4 b)
{
    if (std::max(std::abs(a.z), std::abs(b.z)) > 1.0f)
    {
        return;
    }

    drawLine(a.x, a.y, b.x, b.y);
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
        znear = 0.1f, zfar = 100.0f;

    projMat = glm::mat4(1.0f / (aspect * t), 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f / t, 0.0f, 0.0f,
                        0.0f, 0.0f, zfar / (znear - zfar), -1.0f,
                        0.0f, 0.0f, (znear * zfar) / (znear - zfar), 0.0f);
}

void Renderer::genViewMatrix()
{
    using glm::vec3;

    const vec3 up = vec3(0.0f, 1.0f, 0.0f),
        eye = camPos,
        target = vec3(0.0f),
        zAxis = glm::normalize(eye - target),
        xAxis = glm::normalize(glm::cross(up, zAxis)),
        yAxis = up;

    viewMat = glm::mat4(xAxis.x, yAxis.x, zAxis.x, 0.0f,
                        xAxis.y, yAxis.y, zAxis.y, 0.0f,
                        xAxis.z, yAxis.z, zAxis.z, 0.0f,
                        -glm::dot(xAxis, eye), -glm::dot(yAxis, eye),
                        -glm::dot(zAxis, eye), 1.0f);
}

void Renderer::renderModel()
{
    if (model == nullptr)
    {
        return;
    }

    for (size_t i = 0; i < model->faces.size(); ++i)
    {
        const Face& f = model->faces[i];

        Color col(128 + i % 64);

        drawTriangle(model->vertices[f.vertices[0]],
                     model->vertices[f.vertices[1]],
                     model->vertices[f.vertices[2]],
                     col);
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

template<typename T>
T Renderer::Interpolate(const glm::vec3 br, const T a, const T b, const T c)
{
    return a * br.x + b * br.y + c * br.z;
}
