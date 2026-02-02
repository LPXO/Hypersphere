#include "core/ops/MergeSop.h"

MergeSop::MergeSop(NodeId id) : Node(id)
{
    setName("merge1");
}

Geometry MergeSop::cook(const CookContext&,
                        const std::vector<std::shared_ptr<const Geometry>>& inputs) const
{
    Geometry out;

    uint32_t pointOffset = 0;
    for (auto& in : inputs)
    {
        if (!in) continue;
        // append points
        out.P.insert(out.P.end(), in->P.begin(), in->P.end());

        // append tris with offset
        for (auto t : in->Tris)
        {
            t.a += pointOffset;
            t.b += pointOffset;
            t.c += pointOffset;
            out.Tris.push_back(t);
        }

        pointOffset += uint32_t(in->P.size());
    }

    return out;
}