#include "Vertex.hpp"

Vertex Vertex::Combine(const Vertex a, const Vertex b, const float ratio)
{
    Vertex res;
    res.v = a.v * (1.0f - ratio) + b.v * ratio;
    res.n = a.n * (1.0f - ratio) + b.n * ratio;
    res.posView = a.posView * (1.0f - ratio) + b.posView * ratio;

    return res;
}
