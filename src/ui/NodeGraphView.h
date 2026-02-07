#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPoint>
#include <unordered_map>
#include <optional>

#include "core/graph/Graph.h"
#include "core/graph/NodeRegistry.h"
#include "ui/ConnectionItem.h"
#include "ui/NodeItem.h"

class NodeGraphView final : public QGraphicsView
{
    Q_OBJECT
  public:
    explicit NodeGraphView(QWidget* parent = nullptr);

    void setGraph(Graph* g);
    void rebuildFromGraph();

    void setDisplayNode(NodeId id);
    void setSelectedNode(NodeId id);

    void centerOnGraph();

    signals:
    void nodeSelected(NodeId id);
    void displayNodeRequested(NodeId id);
    void graphChanged(); // connect/disconnect


protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;

private:
    QGraphicsScene m_scene;
    Graph* m_graph = nullptr;

    std::unordered_map<NodeId, NodeItem*> m_nodeItems;

    // Connection visuals keyed by (dst, inputIndex)
    struct ConnKey { NodeId dst; int input; };
    struct ConnKeyHash {
        size_t operator()(const ConnKey& k) const noexcept {
            return (size_t(k.dst) << 8) ^ size_t(k.input);
        }
    };
    struct ConnKeyEq {
        bool operator()(const ConnKey& a, const ConnKey& b) const noexcept {
            return a.dst == b.dst && a.input == b.input;
        }
    };
    std::unordered_map<ConnKey, ConnectionItem*, ConnKeyHash, ConnKeyEq> m_connections;

    // Drag-to-connect state
    bool m_draggingWire = false;
    NodeId m_dragSrcNode = 0;
    ConnectionItem* m_preview = nullptr;

    bool m_spaceDown = false;
    bool m_spacePanning = false;
    QPoint m_lastPanPos;

    int inputCountForNode(const Node* n) const;
    void rebuildConnections();
    NodeItem* itemAtScene(const QPointF& scenePos) const;

    void beginWireDrag(NodeId srcNode, const QPointF& scenePos);
    void updateWireDrag(const QPointF& scenePos);
    void endWireDrag(const QPointF& scenePos);

    void onNodeClicked(NodeId id);
    void onNodeDoubleClicked(NodeId id);

};