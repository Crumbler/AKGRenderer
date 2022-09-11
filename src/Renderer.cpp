#include "Renderer.hpp"

#include <cstring>
#include <cmath>
#include <algorithm>

#include <glm/ext.hpp>

#include "Global.hpp"

Renderer::Renderer()
{
    buffer = nullptr;
    model = nullptr;

    FOV = 90.0f;
    camPos = glm::vec3(0.0f, 0.0f, 1.0f);
}

const void* Renderer::Render(int width, int height, bool sizeChanged)
{
    using glm::vec4;

    this->width = width;
    this->height = height;

    genProjectionMatrix();
    genViewportMatrix();
    genViewMatrix();

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

    return buffer;
}

void Renderer::drawTriangle(glm::vec4 a, glm::vec4 b, glm::vec4 c)
{
    a = projMat * viewMat * a;
    b = projMat * viewMat * b;
    c = projMat * viewMat * c;

    a /= a.w;
    b /= b.w;
    c /= c.w;

    a = viewportMat * a;
    b = viewportMat * b;
    c = viewportMat * c;

    drawLine(a, b);
    drawLine(a, c);
    drawLine(b, c);
}

void Renderer::drawLine(glm::vec4 a, glm::vec4 b)
{
    drawLine(a.x, a.y, b.x, b.y);
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
    viewMat = glm::lookAt(Global::camPos, glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Renderer::renderModel()
{
    if (model == nullptr)
    {
        return;
    }

    for (const auto p : model->polygons)
    {
        drawTriangle(model->vertices[p[0] - 1],
                     model->vertices[p[1] - 1],
                     model->vertices[p[2] - 1]);
    }
}

void Renderer::genViewportMatrix()
{
    using glm::vec4;

    viewportMat[0] = vec4(width / 2.0f, 0.0f, 0.0f, 0.0f);
    viewportMat[1] = vec4(0.0f, -height / 2.0f, 0.0f, 0.0f);
    viewportMat[2] = vec4(0.0f, 0.0f, 1.0f, 0.0f);
    viewportMat[3] = vec4(width / 2.0f, height / 2.0f, 0.0f, 1.0f);
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

        buffer[index(y0, x0)] = Color::white();
    }
}


int Renderer::index(int i, int j) const
{
    return i * width + j;
}
