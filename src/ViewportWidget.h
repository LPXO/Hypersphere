#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QPoint>

#include "core/graph/Graph.h"
#include "core/eval/Cooker.h"

class ViewportWidget final : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
  public:
    explicit ViewportWidget(QWidget* parent = nullptr);

    void setGraphAndCooker(Graph* g, Cooker* c);
    void setDisplayNode(NodeId id);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;

private:
    Graph* m_graph = nullptr;
    Cooker* m_cooker = nullptr;
    NodeId m_displayNode = 0;

    // ultra-simple camera
    float m_yaw = 30.0f;
    float m_pitch = -25.0f;
    float m_dist = 3.0f;
    QPoint m_lastMouse;

    bool m_showGeoWireframe;
    bool m_showViewportGrid;

    void applySimpleCamera(int w, int h);
    void drawViewportGrid(float halfSize, float majorStep, float minorStep);

    void setShowViewportGrid(bool on);
    void setShowGeoWireframe(bool on);
    void keyPressEvent(QKeyEvent* e) override;
};