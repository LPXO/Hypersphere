#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>

#include "core/graph/Node.h"

struct Connection
{
    // input index on dst node -> src node id
    // (for MVP, we keep it simple: each input index has at most one source)
    std::unordered_map<int, NodeId> inputToSrc;
};

class Graph
{
public:
    Graph() = default;

    NodeId addNode(std::unique_ptr<Node> node);
    Node* get(NodeId id);
    const Node* get(NodeId id) const;

    std::vector<NodeId> allNodeIds() const;

    void connect(NodeId src, NodeId dst, int dstInputIndex);
    void disconnect(NodeId dst, int dstInputIndex);

    std::vector<NodeId> inputsOf(NodeId dst) const; // ordered by input index ascending

    uint64_t topologyRevision() const { return m_topologyRev; }

private:
    NodeId m_nextId = 1;
    std::unordered_map<NodeId, std::unique_ptr<Node>> m_nodes;
    std::unordered_map<NodeId, Connection> m_connections;

    uint64_t m_topologyRev = 1;
};