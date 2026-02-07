#include "ui/NodeItem.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

NodeItem::NodeItem(NodeId id, QString title, int inputCount, QGraphicsItem* parent)
  : QGraphicsObject(parent)
  , m_id(id)
  , m_title(std::move(title))
  , m_inputs(inputCount)
{
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
  setAcceptHoverEvents(true);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

QRectF NodeItem::boundingRect() const { return m_rect.adjusted(-2, -2, 2, 2); }

void NodeItem::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*)
{
  p->setRenderHint(QPainter::Antialiasing, true);

  // body
  QColor fill = isSelected() ? QColor(70, 70, 90) : QColor(55, 55, 70);
  p->setBrush(fill);
  p->setPen(QPen(QColor(25, 25, 35), 2));
  p->drawRoundedRect(m_rect, 10, 10);

  // title bar
  QRectF titleRect = m_rect.adjusted(8, 6, -8, -40);
  p->setPen(Qt::white);
  p->drawText(titleRect, Qt::AlignLeft | Qt::AlignTop, m_title);

  // display badge
  if (m_isDisplay)
  {
    p->setBrush(QColor(70, 160, 90));
    p->setPen(Qt::NoPen);
    p->drawEllipse(QPointF(m_rect.right() - 14, m_rect.top() + 14), 6, 6);
  }

  // sockets
  auto drawSocket = [&](QPointF c, QColor col)
  {
    p->setBrush(col);
    p->setPen(QPen(QColor(10, 10, 10), 1));
    p->drawEllipse(c, 6, 6);
  };

  // inputs (left)
  for (int i = 0; i < m_inputs; ++i)
    drawSocket(inputSocketLocalPos(i), QColor(180, 180, 200));

  // output (right)
  drawSocket(outputSocketLocalPos(), QColor(200, 160, 80));
}

QPointF NodeItem::outputSocketLocalPos() const
{
  return QPointF(m_rect.right() - 8, m_rect.center().y());
}

QPointF NodeItem::inputSocketLocalPos(int index) const
{
  if (m_inputs <= 1)
    return QPointF(m_rect.left() + 8, m_rect.center().y());

  const qreal top = m_rect.top() + 26;
  const qreal bot = m_rect.bottom() - 10;
  const qreal t = (m_inputs == 1) ? 0.5 : (qreal(index) / qreal(m_inputs - 1));
  return QPointF(m_rect.left() + 8, top + (bot - top) * t);
}

QPointF NodeItem::outputSocketScenePos() const { return mapToScene(outputSocketLocalPos()); }
QPointF NodeItem::inputSocketScenePos(int index) const { return mapToScene(inputSocketLocalPos(index)); }

int NodeItem::hitInputSocket(const QPointF& scenePos, qreal radiusPx) const
{
  const qreal r2 = radiusPx * radiusPx;
  for (int i = 0; i < m_inputs; ++i)
  {
    QPointF d = inputSocketScenePos(i) - scenePos;
    if (QPointF::dotProduct(d, d) <= r2) return i;
  }
  return -1;
}

bool NodeItem::hitOutputSocket(const QPointF& scenePos, qreal radiusPx) const
{
  const qreal r2 = radiusPx * radiusPx;
  QPointF d = outputSocketScenePos() - scenePos;
  return QPointF::dotProduct(d, d) <= r2;
}

void NodeItem::setDisplay(bool on)
{
  m_isDisplay = on;
  update();
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
  if (e->button() == Qt::LeftButton)
    emit clicked(m_id);

  QGraphicsObject::mousePressEvent(e);
}

void NodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e)
{
  e->accept();              // ⬅️ prevent move/drag
  emit doubleClicked(m_id);
}

QVariant NodeItem::itemChange(GraphicsItemChange change,
                              const QVariant& value)
{
  if (change == QGraphicsItem::ItemPositionHasChanged)
  {
    emit moved(m_id);
  }

  return QGraphicsObject::itemChange(change, value);
}

