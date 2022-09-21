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
    int vCount = 0, fCount = 0, tCount = 0, nCount = 0;

    while (file >> s)
    {
        if (s == "v")
        {
            ++vCount;

            loadVertex(file);
        }
        else if (s == "vt")
        {
            ++tCount;

            loadTextureCoords(file);
        }
        else if (s == "vn")
        {
            ++nCount;

            loadNormal(file);
        }
        else if (s == "f")
        {
            ++fCount;

            loadFace(file);
        }

        if (!file && !file.eof())
        {
            printf("Model parsing failed\n");
            break;
        }

        std::getline(file, s);
    }

    file.close();

    adjustIndices();

    printf("Loaded model. Vertices: %d, faces: %d\n", vCount, fCount);
    printf("Texture coords: %d, normals: %d\n", tCount, nCount);
}

void Model::getVertices(const Face f, Vertex& a, Vertex& b, Vertex& c)
{
    a.v = vertices[f.vertices[0]];
    b.v = vertices[f.vertices[1]];
    c.v = vertices[f.vertices[2]];

    a.n = normals[f.normals[0]];
    b.n = normals[f.normals[1]];
    c.n = normals[f.normals[2]];
}

void Model::adjustIndices()
{
    const int szV = vertices.size(),
        szN = normals.size();

    for (Face& f : faces)
    {
        for (int i = 0; i < 3; ++i)
        {
            glm::ivec3& p = f.vertices;
            p[i] = p[i] > 0 ? p[i] - 1 : szV + p[i];

            glm::ivec3& n = f.normals;
            n[i] = n[i] > 0 ? n[i] - 1 : szN + n[i];
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

    faces.push_back(Face(v, t, n));

    bool hasFourthVertex = file.peek() == ' ';

    // quad face
    if (hasFourthVertex)
    {
        a = loadFaceVertex(file);

        v[1] = v[2];
        t[1] = t[2];
        n[1] = n[2];

        v[2] = a[0];
        t[2] = a[1];
        n[2] = a[2];

        faces.push_back(Face(v, t, n));
    }
}

glm::ivec3 Model::loadFaceVertex(std::ifstream& file)
{
    // vertex, texture, normal indices
    glm::ivec3 v(0);
    char c;

    file >> v[0];

    if (file.peek() != '/')
    {
        return v;
    }

    file >> c;

    // normal with no texture
    if (file.peek() == '/')
    {
        file >> c >> v[2];
    }
    else
    {
        // texture
        file >> v[1];
        if (file.peek() == '/')
        {
            // normal
            file >> c >> v[2];
        }
    }

    return v;
}

void Model::loadTextureCoords(std::ifstream& file)
{
    glm::vec2 t;

    file >> t.x >> t.y;

    uvs.push_back(t);
}

void Model::loadNormal(std::ifstream& file)
{
    glm::vec3 n;

    file >> n.x >> n.y >> n.z;

    normals.push_back(n);
}
