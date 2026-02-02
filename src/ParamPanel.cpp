#include "ParamPanel.h"

#include <QFormLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QVBoxLayout>

#include "core/ops/GridSop.h"
#include "core/ops/TransformSop.h"

ParamPanel::ParamPanel(QWidget* parent)
  : QWidget(parent)
{
  auto* root = new QVBoxLayout(this);
  m_form = new QFormLayout();
  root->addLayout(m_form);
  root->addStretch(1);
}

void ParamPanel::setGraphAndCooker(Graph* g, Cooker* c)
{
  m_graph = g;
  m_cooker = c;
  rebuild();
}

void ParamPanel::setSelectedNode(NodeId id)
{
  m_selected = id;
  rebuild();
}

void ParamPanel::clearForm()
{
  while (m_form->rowCount() > 0)
    m_form->removeRow(0);
}

void ParamPanel::rebuild()
{
  clearForm();

  if (!m_graph || m_selected == 0)
  {
    m_form->addRow(new QLabel("Select a node."));
    return;
  }

  Node* n = m_graph->get(m_selected);
  if (!n)
  {
    m_form->addRow(new QLabel("Invalid node."));
    return;
  }

  m_form->addRow(new QLabel(QString("Type: %1").arg(n->typeName())));

  // Grid
  if (auto* grid = dynamic_cast<GridSop*>(n))
  {
    auto* rows = new QSpinBox();
    rows->setRange(2, 400);
    rows->setValue(grid->rows);

    auto* cols = new QSpinBox();
    cols->setRange(2, 400);
    cols->setValue(grid->cols);

    auto* size = new QDoubleSpinBox();
    size->setRange(0.01, 1000.0);
    size->setDecimals(3);
    size->setValue(grid->size);

    auto apply = [this, grid, rows, cols, size]()
    {
      grid->rows = rows->value();
      grid->cols = cols->value();
      grid->size = float(size->value());
      grid->bumpParamRevision();
      if (m_cooker) m_cooker->clearCache();
      emit paramsChanged();
    };

    connect(rows, &QSpinBox::valueChanged, this, [apply](int){ apply(); });
    connect(cols, &QSpinBox::valueChanged, this, [apply](int){ apply(); });
    connect(size, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [apply](double){ apply(); });

    m_form->addRow("Rows", rows);
    m_form->addRow("Cols", cols);
    m_form->addRow("Size", size);
    return;
  }

  // Transform
  if (auto* xf = dynamic_cast<TransformSop*>(n))
  {
    auto* tx = new QDoubleSpinBox(); tx->setRange(-1000, 1000); tx->setDecimals(3); tx->setValue(xf->translate.x);
    auto* ty = new QDoubleSpinBox(); ty->setRange(-1000, 1000); ty->setDecimals(3); ty->setValue(xf->translate.y);
    auto* tz = new QDoubleSpinBox(); tz->setRange(-1000, 1000); tz->setDecimals(3); tz->setValue(xf->translate.z);

    auto* sc = new QDoubleSpinBox(); sc->setRange(0.001, 1000); sc->setDecimals(3); sc->setValue(xf->uniformScale);

    auto apply = [this, xf, tx, ty, tz, sc]()
    {
      xf->translate = { float(tx->value()), float(ty->value()), float(tz->value()) };
      xf->uniformScale = float(sc->value());
      xf->bumpParamRevision();
      if (m_cooker) m_cooker->clearCache();
      emit paramsChanged();
    };

    connect(tx, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [apply](double){ apply(); });
    connect(ty, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [apply](double){ apply(); });
    connect(tz, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [apply](double){ apply(); });
    connect(sc, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [apply](double){ apply(); });

    m_form->addRow("Translate X", tx);
    m_form->addRow("Translate Y", ty);
    m_form->addRow("Translate Z", tz);
    m_form->addRow("Uniform Scale", sc);
    return;
  }

  m_form->addRow(new QLabel("No editable parameters for this node yet."));
}