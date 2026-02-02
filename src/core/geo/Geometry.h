#pragma once
#include <vector>
#include <cstdint>

struct Vec3
{
    float x = 0, y = 0, z = 0;
};

struct Tri
{
    uint32_t a=0, b=0, c=0;
};

struct Geometry
{
    std::vector<Vec3> P;      // point positions
    std::vector<Tri>  Tris;   // triangle primitives

    bool empty() const { return P.empty() || Tris.empty(); }
    void clear() { P.clear(); Tris.clear(); }
};