#include "core/graph/Graph.h"

#include <algorithm>


NodeId Graph::addNode(std::unique_ptr<Node> node)
{
    // if caller constructed Node with a placeholder id, overwrite not supported in MVP
    const NodeId id = node->id();
    m_nodes.emplace(id, std::move(node));
    ++m_topologyRev;
    return id;
}

Node* Graph::get(NodeId id)
{
    auto it = m_nodes.find(id);
    return (it == m_nodes.end()) ? nullptr : it->second.get();
}

const Node* Graph::get(NodeId id) const
{
    auto it = m_nodes.find(id);
    return (it == m_nodes.end()) ? nullptr : it->second.get();
}

std::vector<NodeId> Graph::allNodeIds() const
{
    std::vector<NodeId> ids;
    ids.reserve(m_nodes.size());
    for (auto& kv : m_nodes) ids.push_back(kv.first);
    std::sort(ids.begin(), ids.end());
    return ids;
}

void Graph::connect(NodeId src, NodeId dst, int dstInputIndex)
{
    m_connections[dst].inputToSrc[dstInputIndex] = src;
    ++m_topologyRev;
}

void Graph::disconnect(NodeId dst, int dstInputIndex)
{
    auto it = m_connections.find(dst);
    if (it == m_connections.end()) return;
    it->second.inputToSrc.erase(dstInputIndex);
    ++m_topologyRev;
}

std::vector<NodeId> Graph::inputsOf(NodeId dst) const
{
    std::vector<std::pair<int, NodeId>> tmp;
    auto it = m_connections.find(dst);
    if (it != m_connections.end())
    {
        tmp.reserve(it->second.inputToSrc.size());
        for (auto& kv : it->second.inputToSrc)
            tmp.push_back({kv.first, kv.second});
    }

    std::sort(tmp.begin(), tmp.end(),
              [](auto& a, auto& b){ return a.first < b.first; });

    std::vector<NodeId> ordered;
    ordered.reserve(tmp.size());
    for (auto& p : tmp) ordered.push_back(p.second);
    return ordered;
}