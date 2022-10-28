#pragma once

#include "Model.hpp"
#include "Vertex.hpp"
#include "Shading.hpp"
#include "Texture.hpp"
#include "NormalTexture.hpp"
#include "MonoTexture.hpp"
#include <string>

class Renderer
{
    public:
        Renderer();
        void ResetParams();

        const void* Render(int width, int height, bool sizeChanged);
        void LoadModel(const std::string& filename);
        void LoadDiffuse(const std::string& filename);
        void LoadSpecular(const std::string& filename);
        void LoadNormal(const std::string& filename);
        void LoadEmission(const std::string& filename);
        void LoadMetallic(const std::string& filename);
        void LoadRoughness(const std::string& filename);
        void LoadAO(const std::string& filename);

        float FOV, ambientFactor, lambertFactor, spec1, spec2;
        bool backfaceCulling, perspectiveCorrection;
        glm::vec3 camPos, modelScale,
            modelPos, modelRot;

        glm::vec2 lightDir;
        Shading shading;

    private:
        int index(int i, int j) const;
        void drawTriangle(Vertex va, Vertex vb, Vertex vc);
        void drawTopTriangle(Vertex va, Vertex vb, Vertex vc);
        void drawBottomTriangle(Vertex va, Vertex vb, Vertex vc);
        void drawLeftTriangle(Vertex va, Vertex vb, Vertex vc);
        void drawRightTriangle(Vertex va, Vertex vb, Vertex vc);
        void drawFragment(const glm::vec3 br, const int x, const int y,
                           const Vertex va, const Vertex vb,
                          const Vertex vc);
        void renderModel();
        void setPixel(const int x, const int y, const float z, glm::vec3 c);
        template<typename T> static T Interpolate(const glm::vec3 br, const T a, const T b, const T c);
        static glm::vec3 InterpolateNormals(const glm::vec3 br, const glm::vec3 a, const glm::vec3 b, const glm::vec3 c);
        static bool canCull(const glm::vec2 a, const glm::vec2 b, const glm::vec2 c);
        void genProjectionMatrix();
        void genViewportMatrix();
        void genViewMatrix();
        void genModelMatrix();
        void genLightVec();
        glm::vec3 calcNormal(const glm::vec3 n, glm::vec3 tangent, const glm::vec2 t);
        glm::vec3 calcBlinnPhongShading(const glm::vec3 p, const glm::vec3 n);
        glm::vec3 getPBR(const glm::vec3 n, const glm::vec3 pos, glm::vec3 albedo,
                         float metallic, float roughness, float ao, glm::vec3 emission);

        glm::mat4 modelMat, viewMat, projMat, viewportMat;
        glm::vec3 lightVec, lightVecView;
        uint8_t *buffer;
        float *zBuffer;
        int width, height, culledFaces;
        constexpr static float zNear = 0.1f, zFar = 100.0f;
        Model *model;
        Texture *texDiffuse, *texSpecular, *texEmission;
        NormalTexture *texNormal;
        MonoTexture *texMetallic, *texRoughness, *texAO;
};
