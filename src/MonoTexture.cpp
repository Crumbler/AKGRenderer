#include "MonoTexture.hpp"

#include "lodepng.h"
#include <algorithm>

MonoTexture::MonoTexture(const std::string& filename, TextureType type)
{
    width = 0;
    height = 0;

    unsigned error = lodepng::decode(data, width, height, filename, LCT_GREY);

    if (!error)
    {
        printf("Loaded texture:\n");
    }
    else
    {
        printf("Failed to load texture:\n");

        switch (type)
        {
        case Emission:
            printf("Using default emission map\n");

            width = 1;
            height = 1;

            data = std::vector<unsigned char>(1, 0);
            break;

        case Ambient:
            printf("Using default ambient map\n");

            width = 1;
            height = 1;

            data = std::vector<unsigned char>(1, 255);
            break;

        case Metallic:
            printf("Using default metallic map\n");

            width = 1;
            height = 1;

            data = std::vector<unsigned char>(1, 0);
            break;

        case Roughness:
            printf("Using default roughness map\n");

            width = 1;
            height = 1;

            data = std::vector<unsigned char>(1, 0);
            break;

        default:
            break;
        }
    }

    printf("width: %d height: %d\n\n", width, height);
}

float MonoTexture::getVal(float x, float y)
{
    using glm::vec2;
    using glm::clamp;

    x = std::clamp(x, 0.0f, 1.0f);
    y = std::clamp(y, 0.0f, 1.0f);

    const int newX = round(x * (width - 1)),
        newY = round(y * (height - 1));

    const int ind = newY * width + newX;

    return data[ind] / 255.0f;
}
