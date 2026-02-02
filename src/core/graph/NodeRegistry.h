#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/graph/Node.h"

class NodeRegistry
{
public:
    using Factory = std::function<std::unique_ptr<Node>(NodeId)>;

    void registerType(std::string typeName, Factory f);
    std::vector<std::string> types() const;
    std::unique_ptr<Node> create(const std::string& typeName, NodeId id) const;

private:
    std::unordered_map<std::string, Factory> m_factories;
};