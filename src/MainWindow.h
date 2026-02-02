#pragma once
#include <QMainWindow>

#include "core/graph/Graph.h"
#include "core/graph/NodeRegistry.h"
#include "core/eval/Cooker.h"

class QListWidget;
class ViewportWidget;
class ParamPanel;

class MainWindow final : public QMainWindow
{
    Q_OBJECT
  public:
    MainWindow();

private:
    Graph m_graph;
    NodeRegistry m_registry;
    Cooker m_cooker;

    NodeId m_displayNode = 0;
    NodeId m_selectedNode = 0;
    NodeId m_nextId = 1;

    QListWidget* m_nodeList = nullptr;
    ViewportWidget* m_viewport = nullptr;
    ParamPanel* m_params = nullptr;

    void setupRegistry();
    void buildInitialGraph();
    NodeId spawn(const std::string& type);

    void rebuildNodeList();
    void setSelected(NodeId id);
    void setDisplay(NodeId id);
};