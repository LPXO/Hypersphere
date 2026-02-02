#pragma once
#include "core/graph/Node.h"

class TransformSop final : public Node
{
public:
    explicit TransformSop(NodeId id);

    const char* typeName() const override { return "Transform"; }

    Vec3 translate{0,0,0};
    float uniformScale = 1.0f;

    Geometry cook(const CookContext&,
                  const std::vector<std::shared_ptr<const Geometry>>& inputs) const override;
};