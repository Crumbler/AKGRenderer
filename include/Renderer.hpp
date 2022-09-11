#pragma once

#include "Color.hpp"
#include "Model.hpp"
#include <string>

class Renderer
{
    public:
        Renderer();

        const void* Render(int width, int height, bool sizeChanged);
        void LoadModel(const std::string& filename);

        float FOV;
        glm::vec3 camPos;

    private:
        int index(int i, int j) const;
        void drawLine(float x0, float y0, float x1, float y1);
        void drawLine(glm::vec4 a, glm::vec4 b);
        void drawTriangle(glm::vec4 a, glm::vec4 b, glm::vec4 c);
        void renderModel();

        void genProjectionMatrix();
        void genViewportMatrix();
        void genViewMatrix();

        glm::mat4 viewMat, projMat, viewportMat;
        Color *buffer;
        int width, height;
        Model *model;
};
