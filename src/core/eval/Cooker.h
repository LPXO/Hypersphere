#pragma once
#include <unordered_map>
#include <memory>
#include <vector>

#include "core/graph/Graph.h"

struct CacheEntry
{
    std::shared_ptr<const Geometry> geo;
    uint64_t topoRev = 0;
    uint64_t paramRev = 0;
    std::vector<uint64_t> inputParamRevs; // simplistic (per input node param rev)
    std::vector<NodeId> inputIds;         // to match above
};

class Cooker
{
public:
    explicit Cooker(const Graph* g) : m_graph(g) {}

    std::shared_ptr<const Geometry> evaluate(NodeId nodeId);

    void clearCache() { m_cache.clear(); }

private:
    const Graph* m_graph = nullptr;
    std::unordered_map<NodeId, CacheEntry> m_cache;

    std::shared_ptr<const Geometry> evaluateInternal(NodeId nodeId);
};