#include "Renderer.hpp"

#include <cstring>

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
        memset(buffer, 0, width * height * 3);
    }

    for (int i = 0; i < height / 2; ++i)
    {
        for (int j = 0; j < width / 2; ++j)
        {
            buffer[index(i, j)] = Color::red();
        }
    }

    return buffer;
}

int Renderer::index(int i, int j) const
{
    return i * width + j;
}
