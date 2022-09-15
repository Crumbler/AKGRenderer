#include "Renderer.hpp"

#include <cstring>
#include <cmath>
#include <algorithm>

#include <glm/ext.hpp>
#include <glm/gtx/euler_angles.hpp>

Renderer::Renderer()
{
    buffer = nullptr;
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
        delete [] ptr;
    }

    if (sizeChanged || buffer == nullptr)
    {
        buffer = (Color*)new char[width * height * 3];
    }

    memset((void*)buffer, 0, width * height * 3);

    renderModel();

    //drawTriangle(glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f),
    //             glm::vec4(0.0f, -1.0f, 0.0f, 1.0f),
    //             glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    return buffer;
}

void Renderer::drawTriangle(glm::vec4 a, glm::vec4 b, glm::vec4 c)
{
    using std::swap;

    a = viewProjMat * modelMat * a;
    b = viewProjMat * modelMat * b;
    c = viewProjMat * modelMat * c;

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
        drawBottomTriangle(a, b, c);
    }
    else if (b.y == c.y)
    {
        // natural top triangle
        if (b.x > c.x)
        {
            swap(b, c);
        }
        drawTopTriangle(a, b, c);
    }
    else // general triangle
    {
        const float ratio = (b.y - a.y) / (c.y - a.y);

        const glm::vec2 newV = a + (c - a) * ratio;

        if (b.x < newV.x)
        {
            // new vertex on the right edge
            drawTopTriangle(a, b, newV);
            drawBottomTriangle(b, newV, c);
        }
        else
        {
            // new vertex on the left edge
            drawTopTriangle(a, newV, b);
            drawBottomTriangle(newV, b, c);
        }
    }
}

void Renderer::drawTopTriangle(glm::vec2 a, glm::vec2 b, glm::vec2 c)
{
    const float mLeft = (b.x - a.x) / (b.y - a.y),
        mRight = (c.x - a.x) / (c.y - a.y);

    const int yStart = std::ceil(a.y - 0.5f),
        yEnd = std::ceil(c.y - 0.5f);

    for (int y = yStart; y < yEnd; ++y)
    {
        int xStart = mLeft * (y - a.y + 0.5f) + a.x,
            xEnd = mRight * (y - a.y + 0.5f) + a.x;

        xStart = std::ceil(xStart - 0.5f);
        xEnd = std::ceil(xEnd - 0.5f);

        for (int x = xStart; x < xEnd; ++x)
        {
            setPixel(x, y, Color::white());
        }
    }
}

void Renderer::drawBottomTriangle(glm::vec2 a, glm::vec2 b, glm::vec2 c)
{
    const float mLeft = (c.x - a.x) / (c.y - a.y),
        mRight = (c.x - b.x) / (c.y - b.y);

    const int yStart = std::ceil(a.y - 0.5f),
        yEnd = std::ceil(c.y - 0.5f);

    for (int y = yStart; y < yEnd; ++y)
    {
        int xStart = mLeft * (y - a.y + 0.5f) + a.x,
            xEnd = mRight * (y - a.y + 0.5f) + b.x;

        xStart = std::ceil(xStart - 0.5f);
        xEnd = std::ceil(xEnd - 0.5f);

        for (int x = xStart; x < xEnd; ++x)
        {
            setPixel(x, y, Color::white());
        }
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

void Renderer::setPixel(const int x, const int y, const Color c)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        return;
    }

    buffer[index(y, x)] = c;
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

    for (const auto p : model->polygons)
    {
        drawTriangle(model->vertices[p[0]],
                     model->vertices[p[1]],
                     model->vertices[p[2]]);
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

        setPixel(x0, y0, Color::white());
    }
}


int Renderer::index(int i, int j) const
{
    return i * width + j;
}
