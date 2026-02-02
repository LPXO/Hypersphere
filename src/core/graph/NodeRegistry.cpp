#include "core/graph/NodeRegistry.h"
#include <algorithm>

void NodeRegistry::registerType(std::string typeName, Factory f)
{
    m_factories.emplace(std::move(typeName), std::move(f));
}

std::vector<std::string> NodeRegistry::types() const
{
    std::vector<std::string> out;
    out.reserve(m_factories.size());
    for (auto& kv : m_factories) out.push_back(kv.first);
    std::sort(out.begin(), out.end());
    return out;
}

std::unique_ptr<Node> NodeRegistry::create(const std::string& typeName, NodeId id) const
{
    auto it = m_factories.find(typeName);
    if (it == m_factories.end()) return {};
    return it->second(id);
}