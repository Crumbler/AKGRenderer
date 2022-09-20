#pragma once

#include "Color.hpp"
#include "Model.hpp"
#include "Vertex.hpp"
#include <string>

class Renderer
{
    public:
        Renderer();

        const void* Render(int width, int height, bool sizeChanged);
        void LoadModel(const std::string& filename);

        float FOV;
        bool backfaceCulling;
        glm::vec3 camPos, modelScale,
            modelPos, modelRot;

    private:
        int index(int i, int j) const;
        void drawLine(float x0, float y0, float x1, float y1);
        void drawLine(glm::vec4 a, glm::vec4 b);
        void drawTriangle(Vertex va, Vertex vb, Vertex vc, Color col);
        void drawTopTriangle(Vertex va, Vertex vb, Vertex vc, Color col);
        void drawBottomTriangle(Vertex va, Vertex vb, Vertex vc, Color col);
        void renderModel();
        void setPixel(const int x, const int y, const float z, const Color c);
        template<typename T> static T Interpolate(const glm::vec3 br, const T a, const T b, const T c);
        static bool canCull(glm::vec4 a, glm::vec4 b, glm::vec4 c);
        void genProjectionMatrix();
        void genViewportMatrix();
        void genViewMatrix();
        void genModelMatrix();
        float calcLighting(const glm::vec3 n);

        glm::mat4 modelMat, viewMat, projMat, viewportMat;
        Color *buffer;
        float *zBuffer;
        int width, height, culledFaces;
        Model *model;
};
