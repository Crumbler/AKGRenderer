#pragma once

#include "Color.hpp"

class Renderer
{
    public:
        Renderer();

        const void* Render(int width, int height, bool sizeChanged);

    private:
        int index(int i, int j) const;
        void drawLine(float x0, float y0, float x1, float y1);

        Color *buffer;
        int width, height;
};
