#pragma once
#include "core/graph/Node.h"

class NullSop final : public Node
{
public:
    explicit NullSop(NodeId id);

    const char* typeName() const override { return "Null"; }

    Geometry cook(const CookContext&,
                  const std::vector<std::shared_ptr<const Geometry>>& inputs) const override;
};