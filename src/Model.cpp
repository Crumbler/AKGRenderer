#include "Model.hpp"

#include <fstream>
#include <cstdio>

Model::Model(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        return;
    }

    int vCount = 0, fCount = 0;

    std::string s;

    while (!file.eof())
    {
        file >> s;

        if (s == "v")
        {
            ++vCount;

            glm::vec4 v;

            file >> v.x >> v.y >> v.z;

            v.w = 1.0f;

            vertices.push_back(v);
        }
        else if (s == "f")
        {
            ++fCount;

            glm::ivec3 p;
            int tmp;

            file >> p[0];

            file.get();
            file >> tmp;

            file >> p[1];

            file.get();
            file >> tmp;

            file >> p[2];

            file.get();
            file >> tmp;

            polygons.push_back(p);
        }

        std::getline(file, s);
    }

    printf("Model loaded. Vertices: %d Polygons: %d\n", vCount, fCount);

    file.close();
}
