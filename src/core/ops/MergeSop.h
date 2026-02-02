#pragma once
#include "core/graph/Node.h"

class MergeSop final : public Node
{
public:
    explicit MergeSop(NodeId id);

    const char* typeName() const override { return "Merge"; }

    Geometry cook(const CookContext&,
                  const std::vector<std::shared_ptr<const Geometry>>& inputs) const override;
};