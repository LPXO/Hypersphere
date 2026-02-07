#pragma once
#include <QGraphicsPathItem>
#include <QPointF>

class ConnectionItem final : public QGraphicsPathItem
{
public:
    ConnectionItem(QGraphicsItem* parent = nullptr);

    void setEndpoints(const QPointF& a, const QPointF& b);

private:
    QPointF m_a, m_b;
    void rebuild();
};