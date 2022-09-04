#pragma once

#include "Color.hpp"

class Renderer
{
    public:
        Renderer();

        const void* Render(int width, int height, bool sizeChanged);

    private:
        int index(int i, int j) const;

        Color *buffer;
        int width, height;
};
