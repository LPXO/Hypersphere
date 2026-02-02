#include "core/ops/TransformSop.h"

TransformSop::TransformSop(NodeId id) : Node(id)
{
    setName("xform1");
}

Geometry TransformSop::cook(const CookContext&,
                            const std::vector<std::shared_ptr<const Geometry>>& inputs) const
{
    Geometry out;
    if (inputs.empty() || !inputs[0]) return out;

    out = *inputs[0]; // copy (MVP)
    for (auto& p : out.P)
    {
        p.x = p.x * uniformScale + translate.x;
        p.y = p.y * uniformScale + translate.y;
        p.z = p.z * uniformScale + translate.z;
    }
    return out;
}