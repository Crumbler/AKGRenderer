#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

class Model
{
    public:
        Model(const std::string& filename);

        std::vector<glm::vec4> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> uvs;
        std::vector<glm::ivec3> polygons;

    private:
        glm::ivec3 loadFaceVertex(std::ifstream& file);
        glm::ivec3 loadFaceVertex(std::ifstream& file, int n1);
        void loadVertex(std::ifstream& file);
        void loadFace(std::ifstream& file);
        void adjustIndices();
};
