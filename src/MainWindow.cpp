#include "MainWindow.h"

#include <QSplitter>
#include <QListWidget>
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTimer>

#include "ViewportWidget.h"
#include "ParamPanel.h"

#include "core/ops/GridSop.h"
#include "core/ops/TransformSop.h"
#include "core/ops/MergeSop.h"
#include "core/ops/NullSop.h"

#include "ui/NodeGraphView.h"

MainWindow::MainWindow()
  : QMainWindow()
  , m_cooker(&m_graph)
{
  setupRegistry();

  // Layout: viewport on the left, graph + params stacked on the right
  auto* splitter = new QSplitter(Qt::Horizontal, this);

  // left viewport
  m_viewport = new ViewportWidget(splitter);
  m_viewport->setMinimumWidth(250);
  m_viewport->setGraphAndCooker(&m_graph, &m_cooker);

  // right side: vertical stack (params on top)
  auto* rightSplitter = new QSplitter(Qt::Vertical, splitter);

  m_params = new ParamPanel(rightSplitter);
  m_params->setMinimumHeight(250);
  m_params->setGraphAndCooker(&m_graph, &m_cooker);

  m_graphView = new NodeGraphView(rightSplitter);
  m_params->setMinimumHeight(250);
  m_graphView->setGraph(&m_graph);



  // proportions
  splitter->setStretchFactor(0, 1); // viewport grows
  splitter->setStretchFactor(1, 0); // right stack keeps width

  rightSplitter->setStretchFactor(0, 0); // params grows vertically
  rightSplitter->setStretchFactor(1, 0); // graph grows vertically

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

  QAction* centerGraphAction = tb->addAction("Center Graph");
  connect(centerGraphAction, &QAction::triggered,
          m_graphView, &NodeGraphView::centerOnGraph);
  //centerGraphAction->setIcon(QIcon(":/icons/center.svg"));
  centerGraphAction->setShortcut(QKeySequence(Qt::Key_H));

  QToolBar* toolbar = addToolBar("Graph");

  connect(m_graphView, &NodeGraphView::nodeSelected, this, [this](NodeId id){
    setSelected(id);
  });

  connect(m_graphView, &NodeGraphView::displayNodeRequested, this, [this](NodeId id){
    setDisplay(id);
  });

  connect(m_graphView, &NodeGraphView::graphChanged, this, [this](){
    m_cooker.clearCache();
    m_viewport->update();
  });

  connect(m_params, &ParamPanel::paramsChanged, this, [this]()
  {
    m_viewport->update();
  });

  buildInitialGraph();
  setSelected(m_displayNode);
  setDisplay(m_displayNode);

  setWindowTitle("Hypersphere");
  // macOS: maximize after the window is actually shown (constructor is too early)
  QTimer::singleShot(0, this, [this]() {
    this->showMaximized();
  });
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
  if (m_graphView) m_graphView->rebuildFromGraph();
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

  // The view is rebuilt during spawn(), but connections are added *after* spawns.
  // Rebuild again so ConnectionItem visuals match the model graph.
  if (m_graphView)
  {
    m_graphView->rebuildFromGraph();
    m_graphView->centerOnGraph();
  }
  m_cooker.clearCache();
  if (m_viewport) m_viewport->update();


  m_displayNode = out;
}

void MainWindow::setSelected(NodeId id)
{
  m_selectedNode = id;
  m_params->setSelectedNode(id);
  if (m_graphView)
    m_graphView->setSelectedNode(id);
}

void MainWindow::setDisplay(NodeId id)
{
  m_displayNode = id;
  m_viewport->setDisplayNode(id);
  m_viewport->update();               // ⬅️ force redraw
  if (m_graphView)
    m_graphView->setDisplayNode(id);
}