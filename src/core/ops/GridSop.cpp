#include "core/ops/GridSop.h"

GridSop::GridSop(NodeId id) : Node(id)
{
    setName("grid1");
}

Geometry GridSop::cook(const CookContext&,
                       const std::vector<std::shared_ptr<const Geometry>>&) const
{
    Geometry g;
    const int r = (rows < 2) ? 2 : rows;
    const int c = (cols < 2) ? 2 : cols;

    g.P.reserve(r * c);

    const float half = size * 0.5f;
    for (int y = 0; y < r; ++y)
    {
        const float ty = float(y) / float(r - 1);
        const float py = -half + ty * size;

        for (int x = 0; x < c; ++x)
        {
            const float tx = float(x) / float(c - 1);
            const float px = -half + tx * size;
            g.P.push_back({px, 0.0f, py});
        }
    }

    // Triangulate grid
    for (int y = 0; y < r - 1; ++y)
    {
        for (int x = 0; x < c - 1; ++x)
        {
            const uint32_t i0 = uint32_t(y * c + x);
            const uint32_t i1 = uint32_t(y * c + (x + 1));
            const uint32_t i2 = uint32_t((y + 1) * c + x);
            const uint32_t i3 = uint32_t((y + 1) * c + (x + 1));

            g.Tris.push_back({i0, i2, i1});
            g.Tris.push_back({i1, i2, i3});
        }
    }

    return g;
}