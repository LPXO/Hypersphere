#include "ViewportWidget.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <cmath>

ViewportWidget::ViewportWidget(QWidget* parent)
  : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::StrongFocus);
}

void ViewportWidget::setGraphAndCooker(Graph* g, Cooker* c)
{
  m_graph = g;
  m_cooker = c;
  update();
}

void ViewportWidget::setDisplayNode(NodeId id)
{
  m_displayNode = id;
  update();
}

void ViewportWidget::initializeGL()
{
  initializeOpenGLFunctions();
  glEnable(GL_DEPTH_TEST);
}

void ViewportWidget::resizeGL(int w, int h)
{
  glViewport(0, 0, w, h);
}

static void matIdentity(float m[16])
{
  for (int i=0;i<16;i++) m[i] = 0.0f;
  m[0]=m[5]=m[10]=m[15]=1.0f;
}

// Very small fixed-function-like camera using legacy matrices.
// (Fine for MVP. Later replace with shader pipeline.)
void ViewportWidget::applySimpleCamera(int w, int h)
{
  const float aspect = (h == 0) ? 1.0f : float(w) / float(h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // simple perspective
  const float fov = 45.0f;
  const float n = 0.01f;
  const float f = 100.0f;

  const float top = std::tan(fov * 0.5f * 3.1415926f / 180.0f) * n;
  const float right = top * aspect;
  glFrustum(-right, right, -top, top, n, f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // orbit camera around origin
  const float yawR = m_yaw * 3.1415926f / 180.0f;
  const float pitchR = m_pitch * 3.1415926f / 180.0f;

  const float cx = m_dist * std::cos(pitchR) * std::sin(yawR);
  const float cy = m_dist * std::sin(pitchR);
  const float cz = m_dist * std::cos(pitchR) * std::cos(yawR);

  // lookAt (very basic)
  const float eye[3] = {cx, cy, cz};
  const float at[3]  = {0, 0, 0};
  const float up[3]  = {0, 1, 0};

  // compute forward, right, up
  float fx = at[0]-eye[0], fy = at[1]-eye[1], fz = at[2]-eye[2];
  const float fl = std::sqrt(fx*fx+fy*fy+fz*fz);
  fx/=fl; fy/=fl; fz/=fl;

  float rx = fy*up[2]-fz*up[1];
  float ry = fz*up[0]-fx*up[2];
  float rz = fx*up[1]-fy*up[0];
  const float rl = std::sqrt(rx*rx+ry*ry+rz*rz);
  rx/=rl; ry/=rl; rz/=rl;

  float ux = ry*fz-rz*fy;
  float uy = rz*fx-rx*fz;
  float uz = rx*fy-ry*fx;

  float M[16]; matIdentity(M);
  M[0]=rx; M[4]=ry; M[8]=rz;
  M[1]=ux; M[5]=uy; M[9]=uz;
  M[2]=-fx;M[6]=-fy;M[10]=-fz;

  glMultMatrixf(M);
  glTranslatef(-eye[0], -eye[1], -eye[2]);
}

void ViewportWidget::paintGL()
{
  glClearColor(0.08f, 0.08f, 0.09f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  applySimpleCamera(width(), height());

  // Draw simple ground axes
  glBegin(GL_LINES);
    glColor3f(1,0,0); glVertex3f(0,0,0); glVertex3f(1,0,0);
    glColor3f(0,1,0); glVertex3f(0,0,0); glVertex3f(0,1,0);
    glColor3f(0,0,1); glVertex3f(0,0,0); glVertex3f(0,0,1);
  glEnd();

  if (!m_graph || !m_cooker || m_displayNode == 0) return;

  auto geo = m_cooker->evaluate(m_displayNode);
  if (!geo || geo->empty()) return;

  glColor3f(0.85f, 0.85f, 0.9f);
  glBegin(GL_TRIANGLES);
  for (const auto& t : geo->Tris)
  {
    const auto& a = geo->P[t.a];
    const auto& b = geo->P[t.b];
    const auto& c = geo->P[t.c];
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glVertex3f(c.x, c.y, c.z);
  }
  glEnd();
}

void ViewportWidget::mousePressEvent(QMouseEvent* e)
{
  m_lastMouse = e->pos();
}

void ViewportWidget::mouseMoveEvent(QMouseEvent* e)
{
  const QPoint d = e->pos() - m_lastMouse;
  m_lastMouse = e->pos();

  if (e->buttons() & Qt::LeftButton)
  {
    m_yaw += d.x() * 0.4f;
    m_pitch += d.y() * 0.4f;
    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;
    update();
  }
}

void ViewportWidget::wheelEvent(QWheelEvent* e)
{
  const float delta = (e->angleDelta().y() / 120.0f);
  m_dist *= std::pow(0.9f, delta);
  if (m_dist < 0.2f) m_dist = 0.2f;
  if (m_dist > 50.0f) m_dist = 50.0f;
  update();
}