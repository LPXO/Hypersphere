#pragma once
#include <QWidget>

#include "core/graph/Graph.h"
#include "core/eval/Cooker.h"

class QFormLayout;

class ParamPanel final : public QWidget
{
    Q_OBJECT
  public:
    explicit ParamPanel(QWidget* parent = nullptr);

    void setGraphAndCooker(Graph* g, Cooker* c);
    void setSelectedNode(NodeId id);

    signals:
      void paramsChanged(); // tell viewport to update

private:
    Graph* m_graph = nullptr;
    Cooker* m_cooker = nullptr;
    NodeId m_selected = 0;

    QFormLayout* m_form = nullptr;

    void rebuild();
    void clearForm();
};