#include "NormalTexture.hpp"

#include "lodepng.h"
#include <algorithm>

NormalTexture::NormalTexture(const std::string& filename)
{
    width = 0;
    height = 0;

    std::vector<unsigned char> data;

    unsigned error = lodepng::decode(data, width, height, filename, LCT_RGB);

    if (!error)
    {
        printf("Loaded texture:\n");
    }
    else
    {
        printf("Failed to load texture:\nUsing default normal map\n");

        width = 1;
        height = 1;

        data.resize(3);
        data[0] = 0;
        data[1] = 0;
        data[2] = 255;
    }

    printf("width: %d height: %d\n\n", width, height);

    for (std::size_t i = 0; i < data.size(); i += 3)
    {
        glm::vec3 n(data[i], data[i + 1], data[i + 2]);
        n = glm::normalize(n * 2.0f / 255.0f - 1.0f);

        normals.push_back(n);
    }
}

glm::vec3 NormalTexture::getNormal(float x, float y)
{
    x = std::clamp(x, 0.0f, 1.0f);
    y = std::clamp(y, 0.0f, 1.0f);

    const int newX = round(x * (width - 1)),
        newY = round(y * (height - 1));

    const int ind = newY * width + newX;

    return normals[ind];
}
