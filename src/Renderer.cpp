#include "Renderer.hpp"

#include <cstring>
#include <cmath>
#include <algorithm>

Renderer::Renderer()
{
    this->buffer = nullptr;
}

const void* Renderer::Render(int width, int height, bool sizeChanged)
{
    this->width = width;
    this->height = height;

    if (sizeChanged && buffer != nullptr)
    {
        char *ptr = (char*)buffer;
        delete [] ptr;
    }

    if (sizeChanged || buffer == nullptr)
    {
        buffer = (Color*)new char[width * height * 3];
        memset((void*)buffer, 0, width * height * 3);
    }

    drawLine(0.0f, 0.0f, width / 2.0f, height / 2.0f);

    return buffer;
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
