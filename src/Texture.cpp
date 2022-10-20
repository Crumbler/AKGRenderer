#include "Texture.hpp"

#include <cstdio>
#include "lodepng.h"
#include <algorithm>
#include "Utils.hpp"

Texture::Texture(const std::string& filename)
{
    width = 0;
    height = 0;

    unsigned error = lodepng::decode(data, width, height, filename, LCT_RGB);

    if (!error)
    {
        printf("Loaded texture:\n");
    }
    else
    {
        printf("Failed to load texture:\n");
    }

    printf("width: %d height: %d\n", width, height);
}

Color Texture::getCol(float x, float y)
{
    using glm::vec2;
    using glm::clamp;

    x = std::clamp(x, 0.0f, 1.0f);
    y = std::clamp(y, 0.0f, 1.0f);

    const int newX = round(x * (width - 1)),
        newY = round(y * (height - 1));

    const int ind = (newY * width + newX) * 3;

    return Color(data[ind], data[ind + 1], data[ind + 2]);
}

glm::vec3 Texture::getNormal(float x, float y)
{
    x = std::clamp(x, 0.0f, 1.0f);
    y = std::clamp(y, 0.0f, 1.0f);

    const int newX = round(x * (width - 1)),
        newY = round(y * (height - 1));

    const int ind = (newY * width + newX) * 3;

    glm::vec3 n(data[ind], data[ind + 1], data[ind + 2]);

    n = (n * 2.0f) / 255.0f;

    n -= 1.0f;

    return glm::normalize(n);
}
