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

    std::string s;
    int vCount = 0, fCount = 0;

    while (!file.eof())
    {
        file >> s;

        if (s == "v")
        {
            ++vCount;

            loadVertex(file);
        }
        else if (s == "f")
        {
            ++fCount;

            loadFace(file);
        }

        if (!file)
        {
            printf("Model parsing failed\n");
            break;
        }

        std::getline(file, s);
    }

    file.close();

    adjustIndices();

    printf("Loaded model. Vertices: %d, faces: %d\n", vCount, fCount);
}

void Model::adjustIndices()
{
    const int sz = vertices.size();

    for (glm::ivec3& p : polygons)
    {
        for (int i = 0; i < 3; ++i)
        {
            p[i] = p[i] > 0 ? p[i] - 1 : sz + p[i];
        }
    }
}

void Model::loadVertex(std::ifstream& file)
{
    glm::vec4 v;

    file >> v.x >> v.y >> v.z;

    if (file.peek() != ' ')
    {
        v.w = 1.0f;
    }
    else
    {
        file >> v.w;
    }

    vertices.push_back(v);
}

void Model::loadFace(std::ifstream& file)
{
    glm::ivec3 a, v, t, n;

    a = loadFaceVertex(file);

    v[0] = a[0];
    t[0] = a[1];
    n[0] = a[2];

    a = loadFaceVertex(file);

    v[1] = a[0];
    t[1] = a[1];
    n[1] = a[2];

    a = loadFaceVertex(file);

    v[2] = a[0];
    t[2] = a[1];
    n[2] = a[2];

    polygons.push_back(v);

    if (t[0] != 0)
    {
        uvs.push_back(t);
    }

    if (n[0] != 0)
    {
        normals.push_back(n);
    }

    bool hasFourthVertex = false;
    if (file.peek() == ' ')
    {
        char c;
        file.get(c);
    }

    if (file.peek() != '\n')
    {
        hasFourthVertex = true;
    }

    // quad face
    if (hasFourthVertex)
    {
        a = loadFaceVertex(file);

        v[0] = a[0];
        t[0] = a[1];
        n[0] = a[2];

        polygons.push_back(v);

        if (t[0] != 0)
        {
            uvs.push_back(t);
        }

        if (n[0] != 0)
        {
            normals.push_back(n);
        }
    }
}

glm::ivec3 Model::loadFaceVertex(std::ifstream& file)
{
    // vertex, texture, normal indices
    glm::ivec3 v(0);
    char c;

    file >> v[0];

    if (file.peek() == '/')
    {
        file >> c;
    }

    if (file.peek() == '/')
    {
        file >> c;
    }
    else
    {
        file >> v[1];
    }

    if (file.peek() == '/')
    {
        file >> c >> v[2];
    }

    return v;
}

glm::ivec3 Model::loadFaceVertex(std::ifstream& file, int n1)
{
    // vertex, texture, normal indices
    glm::ivec3 v(0);
    char c;

    v[0] = n1;

    if (file.peek() == '/')
    {
        file >> c;
    }

    if (file.peek() == '/')
    {
        file >> c;
    }
    else
    {
        file >> v[1];
    }

    if (file.peek() == '/')
    {
        file >> c >> v[2];
    }

    return v;
}
