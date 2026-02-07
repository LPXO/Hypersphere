#include "ui/ConnectionItem.h"
#include <QPainterPath>
#include <QPen>

ConnectionItem::ConnectionItem(QGraphicsItem* parent)
  : QGraphicsPathItem(parent)
{
    QPen p;
    p.setWidthF(2.0);
    setPen(p);
    setZValue(-1.0); // behind nodes
}

void ConnectionItem::setEndpoints(const QPointF& a, const QPointF& b)
{
    m_a = a;
    m_b = b;
    rebuild();
}

void ConnectionItem::rebuild()
{
    // cubic bezier
    const qreal dx = (m_b.x() - m_a.x());
    const QPointF c1(m_a.x() + dx * 0.5, m_a.y());
    const QPointF c2(m_b.x() - dx * 0.5, m_b.y());

    QPainterPath path;
    path.moveTo(m_a);
    path.cubicTo(c1, c2, m_b);
    setPath(path);
}