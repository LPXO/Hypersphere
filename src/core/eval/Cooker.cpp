#include "core/eval/Cooker.h"

std::shared_ptr<const Geometry> Cooker::evaluate(NodeId nodeId)
{
    return evaluateInternal(nodeId);
}

std::shared_ptr<const Geometry> Cooker::evaluateInternal(NodeId nodeId)
{
    if (!m_graph) return std::make_shared<Geometry>();

    const Node* node = m_graph->get(nodeId);
    if (!node) return std::make_shared<Geometry>();

    const uint64_t topoRev = m_graph->topologyRevision();
    const uint64_t paramRev = node->paramRevision();

    // Gather inputs
    const auto inputIds = m_graph->inputsOf(nodeId);
    std::vector<std::shared_ptr<const Geometry>> inputGeos;
    inputGeos.reserve(inputIds.size());

    std::vector<uint64_t> inputParamRevs;
    inputParamRevs.reserve(inputIds.size());

    for (NodeId inId : inputIds)
    {
        const Node* inNode = m_graph->get(inId);
        inputParamRevs.push_back(inNode ? inNode->paramRevision() : 0);
        inputGeos.push_back(evaluateInternal(inId));
    }

    // Cache check
    auto it = m_cache.find(nodeId);
    if (it != m_cache.end())
    {
        CacheEntry& e = it->second;
        const bool topoOk = (e.topoRev == topoRev);
        const bool paramOk = (e.paramRev == paramRev);
        const bool inputsOk = (e.inputIds == inputIds && e.inputParamRevs == inputParamRevs);

        if (topoOk && paramOk && inputsOk && e.geo)
            return e.geo;
    }

    // Cook
    CookContext ctx;
    Geometry out = node->cook(ctx, inputGeos);
    auto shared = std::make_shared<Geometry>(std::move(out));

    CacheEntry entry;
    entry.geo = shared;
    entry.topoRev = topoRev;
    entry.paramRev = paramRev;
    entry.inputIds = inputIds;
    entry.inputParamRevs = inputParamRevs;
    m_cache[nodeId] = std::move(entry);

    return shared;
}