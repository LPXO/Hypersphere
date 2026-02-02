#pragma once
#include "core/graph/Node.h"

class GridSop final : public Node
{
public:
    explicit GridSop(NodeId id);

    const char* typeName() const override { return "Grid"; }

    int rows = 20;
    int cols = 20;
    float size = 1.0f;

    Geometry cook(const CookContext&,
                  const std::vector<std::shared_ptr<const Geometry>>&) const override;
};