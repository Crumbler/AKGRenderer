#include "Vertex.hpp"
#include <cstdio>

Vertex Vertex::Combine(const Vertex a, const Vertex b, const float ratio)
{
    Vertex res;
    res.v = a.v * (1.0f - ratio) + b.v * ratio;
    res.n = a.n * (1.0f - ratio) + b.n * ratio;
    res.posView = a.posView * (1.0f - ratio) + b.posView * ratio;

    const float dv = (1.0f - ratio) / a.posView.z + ratio / b.posView.z;
    res.t = (a.t * (1.0f - ratio) / a.posView.z + b.t * ratio / b.posView.z) / dv;

    return res;
}
