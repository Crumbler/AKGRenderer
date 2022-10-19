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

    centerModel();

    calcTangents();

    printf("Loaded model. Vertices: %d, faces: %d\n", vCount, fCount);
    printf("Texture coords: %d, normals: %d\n", tCount, nCount);
}

void Model::getVertices(const Face f, Vertex& a, Vertex& b, Vertex& c)
{
    const int vInd0 = f.vertices[0],
        vInd1 = f.vertices[1],
        vInd2 = f.vertices[2];

    a.v = vertices[vInd0];
    b.v = vertices[vInd1];
    c.v = vertices[vInd2];

    a.n = normals[f.normals[0]];
    b.n = normals[f.normals[1]];
    c.n = normals[f.normals[2]];

    a.t = uvs[f.uvs[0]];
    b.t = uvs[f.uvs[1]];
    c.t = uvs[f.uvs[2]];

    a.tangent = tangents[vInd0];
    b.tangent = tangents[vInd1];
    c.tangent = tangents[vInd2];
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

            glm::ivec3& t = f.uvs;
            t[i] = t[i] > 0 ? t[i] - 1 : szN + t[i];
        }
    }
}

void Model::loadVertex(std::ifstream& file)
{
    glm::vec3 v;
    float tmp;

    file >> v.x >> v.y >> v.z;

    if (file.peek() == ' ')
    {
        // w
        file >> tmp;
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

void Model::centerModel()
{
    glm::vec3 center(0.0f);

    for (const glm::vec3 v : vertices)
    {
        center += v;
    }

    center /= (float)vertices.size();

    for (glm::vec3& v : vertices)
    {
        v -= center;
    }
}

void Model::calcTangents()
{
    tangents = std::vector<glm::vec3>(vertices.size(), glm::vec3(0.0f));

    for (const Face fc : faces)
    {
        const int vInd0 = fc.vertices[0],
            vInd1 = fc.vertices[1],
            vInd2 = fc.vertices[2];

        const glm::vec3 v0 = vertices[vInd0],
            v1 = vertices[vInd1],
            v2 = vertices[vInd2];

        const glm::vec2 uv0 = uvs[fc.uvs[0]],
            uv1 = uvs[fc.uvs[1]],
            uv2 = uvs[fc.uvs[2]];

        const glm::vec3 edge1 = v1 - v0,
            edge2 = v2 - v0;

        const float deltaU1 = uv1.x - uv0.x,
            deltaV1 = uv1.y - uv0.y,
            deltaU2 = uv2.x - uv0.x,
            deltaV2 = uv2.y - uv0.y;

        const float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

        glm::vec3 tangent, bitangent;

        tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
        tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
        tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);

        tangents[vInd0] += tangent;
        tangents[vInd1] += tangent;
        tangents[vInd2] += tangent;
    }

    for (glm::vec3& tangent : tangents)
    {
        tangent = glm::normalize(tangent);
    }
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
