#pragma once
#include <QGraphicsObject>
#include <QVector>
#include <QRectF>

#include "core/graph/Node.h"

class NodeItem final : public QGraphicsObject
{
    Q_OBJECT
  public:
    NodeItem(NodeId id, QString title, int inputCount, QGraphicsItem* parent = nullptr);

    NodeId nodeId() const { return m_id; }
    int inputCount() const { return m_inputs; }

    QRectF boundingRect() const override;
    void paint(QPainter* p, const QStyleOptionGraphicsItem* opt, QWidget* w) override;

    QPointF outputSocketScenePos() const;
    QPointF inputSocketScenePos(int index) const;

    int hitInputSocket(const QPointF& scenePos, qreal radiusPx = 8.0) const;
    bool hitOutputSocket(const QPointF& scenePos, qreal radiusPx = 8.0) const;

    void setDisplay(bool on);
    bool isDisplay() const { return m_isDisplay; }

    signals:
      void clicked(NodeId id);
    void doubleClicked(NodeId id);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* e) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e) override;

private:
    NodeId m_id;
    QString m_title;
    int m_inputs;
    bool m_isDisplay = false;

    QRectF m_rect{0, 0, 160, 70};

    QPointF outputSocketLocalPos() const;
    QPointF inputSocketLocalPos(int index) const;
};