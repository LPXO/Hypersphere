#include "ui/NodeGraphView.h"
#include <QScrollBar>
#include <QKeyEvent>
#include <QWheelEvent>



NodeGraphView::NodeGraphView(QWidget* parent)
  : QGraphicsView(parent)
{
  setScene(&m_scene);
  setRenderHint(QPainter::Antialiasing, true);
  setDragMode(QGraphicsView::RubberBandDrag);
  setDragMode(QGraphicsView::NoDrag);
  setTransformationAnchor(QGraphicsView::NoAnchor);
  setViewportUpdateMode(FullViewportUpdate);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  m_scene.setSceneRect(-2000, -2000, 4000, 4000);
}

void NodeGraphView::setGraph(Graph* g)
{
  m_graph = g;
  rebuildFromGraph();
}

int NodeGraphView::inputCountForNode(const Node* n) const
{
  if (!n) return 0;
  // MVP SOP arity rules
  const std::string t = n->typeName();
  if (t == "Grid") return 0;
  if (t == "Merge") return 2;      // start with 2 inputs
  return 1;                        // Transform, Null
}

void NodeGraphView::rebuildFromGraph()
{
  m_scene.clear();
  m_nodeItems.clear();
  m_connections.clear();

  if (!m_graph) return;

  const auto ids = m_graph->allNodeIds();
  int i = 0;
  for (NodeId id : ids)
  {
    const Node* n = m_graph->get(id);
    if (!n) continue;

    const int inputs = inputCountForNode(n);
    auto* item = new NodeItem(id, QString::fromStdString(n->name()), inputs);
    m_scene.addItem(item);

    // Simple grid layout if no positions stored yet
    const int col = i % 3;
    const int row = i / 3;
    item->setPos(col * 220, row * 140);

    connect(item, &NodeItem::clicked, this, &NodeGraphView::onNodeClicked);
    connect(item, &NodeItem::doubleClicked, this, &NodeGraphView::onNodeDoubleClicked);

    m_nodeItems[id] = item;
    ++i;
  }

  rebuildConnections();
}

void NodeGraphView::rebuildConnections()
{
  // remove existing connection items (scene was cleared in rebuildFromGraph, so for now just recreate)
  for (auto& kv : m_connections)
    delete kv.second;
  m_connections.clear();

  if (!m_graph) return;

  for (NodeId dst : m_graph->allNodeIds())
  {
    const Node* dstNode = m_graph->get(dst);
    if (!dstNode) continue;

    const int inputs = inputCountForNode(dstNode);
    auto itConn = m_graph->inputsOf(dst); // ordered sources by input index ascending, but we need actual indices
    // We don't have indices from Graph::inputsOf, so we re-walk Graph internals would be needed.
    // MVP workaround: treat ordered list as inputs 0..N-1.
    for (int inputIndex = 0; inputIndex < (int)itConn.size() && inputIndex < inputs; ++inputIndex)
    {
      NodeId src = itConn[(size_t)inputIndex];
      auto* srcItem = m_nodeItems[src];
      auto* dstItem = m_nodeItems[dst];
      if (!srcItem || !dstItem) continue;

      auto* conn = new ConnectionItem();
      m_scene.addItem(conn);
      conn->setEndpoints(srcItem->outputSocketScenePos(),
                         dstItem->inputSocketScenePos(inputIndex));

      m_connections[{dst, inputIndex}] = conn;
    }
  }
}

NodeItem* NodeGraphView::itemAtScene(const QPointF& scenePos) const
{
  const auto items = m_scene.items(scenePos);
  for (QGraphicsItem* it : items)
    if (auto* ni = dynamic_cast<NodeItem*>(it))
      return ni;
  return nullptr;
}

void NodeGraphView::setDisplayNode(NodeId id)
{
  for (auto& kv : m_nodeItems)
    kv.second->setDisplay(kv.first == id);
}

void NodeGraphView::setSelectedNode(NodeId id)
{
  auto it = m_nodeItems.find(id);
  if (it != m_nodeItems.end())
  {
    it->second->setSelected(true);
    centerOn(it->second);
  }
}

void NodeGraphView::centerOnGraph()
{
  if (!scene() || scene()->items().isEmpty())
    return;

  QRectF bounds = scene()->itemsBoundingRect();

  // Optional padding so nodes aren't flush to the edge
  bounds.adjust(-100, -100, 100, 100);

  // Option A: Just centre (no zoom change)
  //centerOn(bounds.center());

  // Option B: Fit everything nicely (Houdini-like)
  fitInView(bounds, Qt::KeepAspectRatio);
}

void NodeGraphView::beginWireDrag(NodeId srcNode, const QPointF& scenePos)
{
  m_draggingWire = true;
  m_dragSrcNode = srcNode;

  m_preview = new ConnectionItem();
  m_scene.addItem(m_preview);

  auto* srcItem = m_nodeItems[srcNode];
  m_preview->setEndpoints(srcItem->outputSocketScenePos(), scenePos);
}

void NodeGraphView::updateWireDrag(const QPointF& scenePos)
{
  if (!m_draggingWire || !m_preview) return;
  auto* srcItem = m_nodeItems[m_dragSrcNode];
  m_preview->setEndpoints(srcItem->outputSocketScenePos(), scenePos);
}

void NodeGraphView::endWireDrag(const QPointF& scenePos)
{
  if (!m_draggingWire) return;

  // cleanup preview
  if (m_preview)
  {
    m_scene.removeItem(m_preview);
    delete m_preview;
    m_preview = nullptr;
  }

  // connect if we dropped on an input socket
  if (m_graph)
  {
    for (auto& kv : m_nodeItems)
    {
      NodeItem* target = kv.second;
      if (target->nodeId() == m_dragSrcNode)
        continue;

      const int inputIndex = target->hitInputSocket(scenePos);
      if (inputIndex >= 0)
      {
        m_graph->disconnect(target->nodeId(), inputIndex);
        m_graph->connect(m_dragSrcNode, target->nodeId(), inputIndex);
        rebuildConnections();
        emit graphChanged();
        break;
      }
    }
  }

  m_draggingWire = false;
  m_dragSrcNode = 0;
}

void NodeGraphView::onNodeClicked(NodeId id)
{
  emit nodeSelected(id);
}

void NodeGraphView::onNodeDoubleClicked(NodeId id)
{
  emit displayNodeRequested(id);
}

void NodeGraphView::mousePressEvent(QMouseEvent* e)
{
  if (e->button() == Qt::LeftButton && m_spaceDown)
  {
    m_spacePanning = true;
    m_lastPanPos = e->pos();
    setCursor(Qt::ClosedHandCursor);
    e->accept();
    return;
  }

  QGraphicsView::mousePressEvent(e);
}

void NodeGraphView::mouseMoveEvent(QMouseEvent* e)
{
  if (m_spacePanning)
  {
    const QPoint delta = e->pos() - m_lastPanPos;
    m_lastPanPos = e->pos();

    horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
    verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());

    e->accept();
    return;
  }

  QGraphicsView::mouseMoveEvent(e);
}

void NodeGraphView::mouseReleaseEvent(QMouseEvent* e)
{
  if (m_spacePanning && e->button() == Qt::LeftButton)
  {
    m_spacePanning = false;
    if (m_spaceDown) setCursor(Qt::OpenHandCursor);
    else unsetCursor();
    e->accept();
    return;
  }

  QGraphicsView::mouseReleaseEvent(e);
}
void NodeGraphView::wheelEvent(QWheelEvent* e)
{
  if (e->angleDelta().y() == 0)
  {
    e->ignore();
    return;
  }

  // Tweak to taste
  constexpr double zoomStep = 1.05;
  constexpr double minScale = 0.10;
  constexpr double maxScale = 6.00;

  const double factor = (e->angleDelta().y() > 0) ? zoomStep : (1.0 / zoomStep);

  const double currentScale = transform().m11();
  const double nextScale = currentScale * factor;
  if (nextScale < minScale || nextScale > maxScale)
  {
    e->accept();
    return;
  }

  // Sub-pixel accurate: use viewportTransform() inverse with QPointF
  const QPointF viewPos = e->position();
  const QTransform invBefore = viewportTransform().inverted();
  const QPointF scenePosBefore = invBefore.map(viewPos);

  // Apply scale around origin; we’ll compensate using scrollbars.
  scale(factor, factor);

  // After scaling, compute where the *same* scene point ends up in the viewport.
  const QPointF viewPosOfSceneAfter = viewportTransform().map(scenePosBefore);
  const QPointF deltaView = viewPosOfSceneAfter - viewPos;

  // Compensate by scrolling in viewport pixel space.
  horizontalScrollBar()->setValue(horizontalScrollBar()->value() + int(deltaView.x()));
  verticalScrollBar()->setValue(verticalScrollBar()->value() + int(deltaView.y()));

  e->accept();
}

void NodeGraphView::keyPressEvent(QKeyEvent* e)
{
  if (e->key() == Qt::Key_Space && !e->isAutoRepeat())
  {
    m_spaceDown = true;
    if (!m_spacePanning) setCursor(Qt::OpenHandCursor);
    e->accept();
    return;
  }
  QGraphicsView::keyPressEvent(e);
}

void NodeGraphView::keyReleaseEvent(QKeyEvent* e)
{
  if (e->key() == Qt::Key_Space && !e->isAutoRepeat())
  {
    m_spaceDown = false;
    if (!m_spacePanning) unsetCursor();
    e->accept();
    return;
  }
  QGraphicsView::keyReleaseEvent(e);
}
