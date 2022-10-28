#pragma once

#include <glm/glm.hpp>

class Utils
{
public:
    static float perpDotProduct(const glm::vec2 a, const glm::vec2 b);
    static float DistributionGGX(glm::vec3 h, glm::vec3 n, float a);
    static float GeometrySchlickGGX(float NdotV, float roughness);
    static float GeometrySmith(glm::vec3 n, glm::vec3 v, glm::vec3 l, float roughness);
    static glm::vec3 FresnelSchlick(float cosTheta, glm::vec3 F0);

    static constexpr float pi = std::acos(-1.0f);
};
