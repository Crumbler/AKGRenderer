#include "Utils.hpp"

#include <windows.h>

float Utils::perpDotProduct(const glm::vec2 a, const glm::vec2 b)
{
    return a.x * b.y - a.y * b.x;
}

float Utils::DistributionGGX(glm::vec3 h, glm::vec3 n, float a)
{
    a = a * a;
    const float a2 = a * a,
        NdotH = std::max(glm::dot(n, h), 0.0f),
        NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = std::max(pi * denom * denom, 0.000001f);

    return a2 / denom;
}

float Utils::GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0f,
        k = r * r / 8.0f;
    float denom = NdotV * (1.0f - k) + k;
    denom = std::max(denom, 0.000001f);

    return NdotV / denom;
}

float Utils::GeometrySmith(glm::vec3 n, glm::vec3 v, glm::vec3 l, float roughness)
{
    float NdotV = std::max(glm::dot(n, v), 0.0f);
    float NdotL = std::max(glm::dot(n, l), 0.0f);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

glm::vec3 Utils::FresnelSchlick(float cosTheta, glm::vec3 F0)
{
    return F0 + (1.0f - F0) * std::pow(1.0f - cosTheta, 5.0f);
}
