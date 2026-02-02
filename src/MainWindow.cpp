#include "MainWindow.h"

#include <QSplitter>
#include <QListWidget>
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>
#include <QMessageBox>

#include "ViewportWidget.h"
#include "ParamPanel.h"

#include "core/ops/GridSop.h"
#include "core/ops/TransformSop.h"
#include "core/ops/MergeSop.h"
#include "core/ops/NullSop.h"

MainWindow::MainWindow()
  : QMainWindow()
  , m_cooker(&m_graph)
{
  setupRegistry();

  auto* splitter = new QSplitter(this);

  // Left: node list
  m_nodeList = new QListWidget(splitter);
  m_nodeList->setMinimumWidth(220);

  // Center: viewport
  m_viewport = new ViewportWidget(splitter);
  m_viewport->setGraphAndCooker(&m_graph, &m_cooker);

  // Right: params
  m_params = new ParamPanel(splitter);
  m_params->setMinimumWidth(260);
  m_params->setGraphAndCooker(&m_graph, &m_cooker);

  splitter->setStretchFactor(0, 0);
  splitter->setStretchFactor(1, 1);
  splitter->setStretchFactor(2, 0);

  setCentralWidget(splitter);

  // Toolbar: add nodes
  auto* tb = addToolBar("Nodes");
  auto addActionFor = [&](const char* type)
  {
    QAction* a = tb->addAction(QString("Add %1").arg(type));
    connect(a, &QAction::triggered, this, [this, type]()
    {
      NodeId id = spawn(type);
      (void)id;
      rebuildNodeList();
    });
  };

  addActionFor("Grid");
  addActionFor("Transform");
  addActionFor("Merge");
  addActionFor("Null");

  tb->addSeparator();

  QAction* setDisplayAct = tb->addAction("Set Display");
  connect(setDisplayAct, &QAction::triggered, this, [this]()
  {
    if (m_selectedNode == 0) return;
    setDisplay(m_selectedNode);
  });

  // Selection handling
  connect(m_nodeList, &QListWidget::currentRowChanged, this, [this](int row)
  {
    const auto ids = m_graph.allNodeIds();
    if (row < 0 || row >= (int)ids.size()) return;
    setSelected(ids[size_t(row)]);
  });

  connect(m_params, &ParamPanel::paramsChanged, this, [this]()
  {
    m_viewport->update();
  });

  buildInitialGraph();
  rebuildNodeList();
  setSelected(m_displayNode);
  setDisplay(m_displayNode);

  setWindowTitle("Houdini-ish SOP Clone (Qt Skeleton)");
}

void MainWindow::setupRegistry()
{
  m_registry.registerType("Grid", [](NodeId id){ return std::make_unique<GridSop>(id); });
  m_registry.registerType("Transform", [](NodeId id){ return std::make_unique<TransformSop>(id); });
  m_registry.registerType("Merge", [](NodeId id){ return std::make_unique<MergeSop>(id); });
  m_registry.registerType("Null", [](NodeId id){ return std::make_unique<NullSop>(id); });
}

NodeId MainWindow::spawn(const std::string& type)
{
  NodeId id = m_nextId++;
  auto node = m_registry.create(type, id);
  if (!node)
  {
    QMessageBox::warning(this, "Error", "Unknown node type.");
    return 0;
  }

  // Make unique-ish names
  node->setName(std::string(node->typeName()) + std::to_string(id));

  m_graph.addNode(std::move(node));
  m_cooker.clearCache();
  return id;
}

void MainWindow::buildInitialGraph()
{
  // grid1 -> xform2 -> null3 (display)
  NodeId grid = spawn("Grid");
  NodeId xf   = spawn("Transform");
  NodeId out  = spawn("Null");

  // wire: grid -> xform input0
  m_graph.connect(grid, xf, 0);
  // wire: xform -> null input0
  m_graph.connect(xf, out, 0);

  m_displayNode = out;
}

void MainWindow::rebuildNodeList()
{
  m_nodeList->clear();

  const auto ids = m_graph.allNodeIds();
  for (NodeId id : ids)
  {
    const Node* n = m_graph.get(id);
    if (!n) continue;

    QString label = QString("%1  (%2)")
      .arg(QString::fromStdString(n->name()))
      .arg(n->typeName());

    if (id == m_displayNode)
      label += "   [DISPLAY]";

    m_nodeList->addItem(label);
  }
}

void MainWindow::setSelected(NodeId id)
{
  m_selectedNode = id;
  m_params->setSelectedNode(id);
}

void MainWindow::setDisplay(NodeId id)
{
  m_displayNode = id;
  m_viewport->setDisplayNode(id);
  rebuildNodeList();
}