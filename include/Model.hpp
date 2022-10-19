#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Face.hpp"
#include "Vertex.hpp"

class Model
{
    public:
        Model(const std::string& filename);

        std::vector<glm::vec3> vertices, normals, tangents;
        std::vector<glm::vec2> uvs;
        std::vector<Face> faces;

        void getVertices(const Face f, Vertex& a, Vertex& b, Vertex& c);

    private:
        glm::ivec3 loadFaceVertex(std::ifstream& file);
        void loadVertex(std::ifstream& file);
        void loadFace(std::ifstream& file);
        void loadTextureCoords(std::ifstream& file);
        void loadNormal(std::ifstream& file);
        void adjustIndices();
        void centerModel();
        void calcTangents();
};
