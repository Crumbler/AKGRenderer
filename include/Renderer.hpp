#pragma once

#include "Color.hpp"
#include "Model.hpp"
#include "Vertex.hpp"
#include "Shading.hpp"
#include "Texture.hpp"
#include <string>

class Renderer
{
    public:
        Renderer();

        const void* Render(int width, int height, bool sizeChanged);
        void LoadModel(const std::string& filename);
        void LoadDiffuse(const std::string& filename);

        float FOV, ambientFactor, lambertFactor, spec1, spec2;
        bool backfaceCulling, perspectiveCorrection;
        glm::vec3 camPos, modelScale,
            modelPos, modelRot;

        glm::vec2 lightDir;
        Shading shading;

    private:
        int index(int i, int j) const;
        void drawTriangle(Vertex va, Vertex vb, Vertex vc);
        void drawTopTriangle(Vertex va, Vertex vb, Vertex vc, const float brightness);
        void drawBottomTriangle(Vertex va, Vertex vb, Vertex vc, const float brightness);
        void drawLeftTriangle(Vertex va, Vertex vb, Vertex vc, const float brightness);
        void drawRightTriangle(Vertex va, Vertex vb, Vertex vc, const float brightness);
        void drawFragment(const glm::vec3 br, const int x, const int y,
                           const Vertex va, const Vertex vb,
                          const Vertex vc, const float brightness);
        void renderModel();
        void setPixel(const int x, const int y, const float z, const Color c);
        template<typename T> static T Interpolate(const glm::vec3 br, const T a, const T b, const T c);
        static glm::vec3 InterpolateNormals(const glm::vec3 br, const glm::vec3 a, const glm::vec3 b, const glm::vec3 c);
        static bool canCull(const glm::vec3 a, const glm::vec3 b, const glm::vec3 c);
        void genProjectionMatrix();
        void genViewportMatrix();
        void genViewMatrix();
        void genModelMatrix();
        void genLightVec();
        float calcLighting(const glm::vec3 n);
        float calcPhongShading(const glm::vec3 p, const glm::vec3 n);

        glm::mat4 modelMat, viewMat, projMat, viewportMat;
        glm::vec3 lightVec, lightVecView;
        Color *buffer;
        float *zBuffer;
        int width, height, culledFaces;
        constexpr static float zNear = 0.1f, zFar = 100.0f;
        Model *model;
        Texture *texDiffuse;
};
