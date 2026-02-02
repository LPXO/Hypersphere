#include "core/ops/NullSop.h"

NullSop::NullSop(NodeId id) : Node(id)
{
    setName("null1");
}

Geometry NullSop::cook(const CookContext&,
                       const std::vector<std::shared_ptr<const Geometry>>& inputs) const
{
    if (inputs.empty() || !inputs[0]) return {};
    return *inputs[0];
}