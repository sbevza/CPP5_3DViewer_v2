#include "glwidget.h"

namespace s21 {

glWidget::glWidget(QWidget *parent) : QOpenGLWidget(parent) {
  setMouseTracking(true);
}

void glWidget::initializeGL() {
  initializeOpenGLFunctions();
  glEnable(GL_DEPTH_TEST);
  glEnableClientState(GL_VERTEX_ARRAY);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  loadBGColor();
}

void glWidget::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void glWidget::paintGL() {
  if (data.vertices.data()) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    projectionChange();
    glTranslatef(widgetdata.PosX_, widgetdata.PosY_, widgetdata.PosZ_);
    setScale();
    rotateModel();
    loadBGColor();

    setTypeViews();
    glColor3f(widgetdata.LineColor_.redF(), widgetdata.LineColor_.greenF(),
              widgetdata.LineColor_.blueF());
    typeViewsModel();
  } else
    loadBGColor();
}

void glWidget::rotateModel() const {
  glRotatef(widgetdata.RotX_, 1.0f, 0.0f, 0.0f);
  glRotatef(widgetdata.RotY_, 0.0f, 1.0f, 0.0f);
  glRotatef(widgetdata.RotZ_, 0.0f, 0.0f, 1.0f);
}

void glWidget::mousePressEvent(QMouseEvent *ma) {
  widgetdata.mPos_ = ma->pos();
}

void glWidget::mouseMoveEvent(QMouseEvent *ma) {
  if (ma->buttons() & Qt::RightButton) {
    GLfloat dx = static_cast<GLfloat>(ma->pos().x() - widgetdata.mPos_.x());
    GLfloat dy = static_cast<GLfloat>(ma->pos().y() - widgetdata.mPos_.y());

    widgetdata.PosX_ += dx / 10;
    widgetdata.PosY_ -= dy / 10;
    widgetdata.mPos_ = ma->pos();

  } else if (ma->buttons() & Qt::LeftButton) {
    GLfloat dx = static_cast<GLfloat>(ma->pos().x() - widgetdata.mPos_.x());
    GLfloat dy = static_cast<GLfloat>(ma->pos().y() - widgetdata.mPos_.y());

    widgetdata.RotX_ += static_cast<GLfloat>(1.0f / M_PI * dy);
    widgetdata.RotY_ += static_cast<GLfloat>(1.0f / M_PI * dx);

    widgetdata.mPos_ = ma->pos();
  }
  update();
}

void glWidget::wheelEvent(QWheelEvent *event) {
  int delta = event->angleDelta().y();
  if (widgetdata.Scale_ == 0) widgetdata.Scale_ = 1;
  if (delta > 0)
    widgetdata.Scale_ *= 1.1;
  else if (delta < 0)
    widgetdata.Scale_ /= 1.1;
}

void glWidget::projectionChange() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (widgetdata.ProjectionType_ == 0) setPerspectiveProjection();
  if (widgetdata.ProjectionType_ == 1) setOrthographicProjection();
}

void glWidget::setPerspectiveProjection() {
  GLfloat aspect = calculateAspect();
  GLfloat nearPlane = 0.001f;
  GLfloat farPlane = 5500.0f;
  GLfloat fov = 60.0f;

  GLfloat top = nearPlane * tanf(static_cast<float>(fov * M_PI / 360.0));
  GLfloat right = top * aspect;

  glFrustum(-right, right, -top, top, nearPlane, farPlane);
}

void glWidget::setOrthographicProjection() {
  GLfloat aspect = calculateAspect();
  auto [modelCenterX, modelCenterY, maxModelSize] = calculateModelDimensions();
  GLfloat zDistance = 3 * maxModelSize;
  GLfloat visibleSize = 1.1f * maxModelSize;

  glOrtho((modelCenterX - visibleSize) * aspect,
          (modelCenterX + visibleSize) * aspect, modelCenterY - visibleSize,
          modelCenterY + visibleSize, -zDistance, zDistance);
}

GLfloat glWidget::calculateAspect() {
  int width = this->width();
  int height = this->height();
  return static_cast<GLfloat>(width) / static_cast<GLfloat>(height);
}

std::tuple<float, float, float> glWidget::calculateModelDimensions() const {
  float modelCenterX = (data.minX + data.maxX) / 2.0f;
  float modelCenterY = (data.minY + data.maxY) / 2.0f;
  float modelDepth = data.maxZ - data.minZ;
  float maxModelSize = std::max(data.maxX - data.minX,
                                std::max(data.maxY - data.minY, modelDepth));
  return {modelCenterX, modelCenterY, maxModelSize};
}

void glWidget::setTypeLine() {
  static int previousLineType = -1;

  if (widgetdata.LineType_ != previousLineType) {
    if (widgetdata.LineType_ == 1) {
      glEnable(GL_LINE_STIPPLE);
      glLineStipple(1, 0xF0F0);
    } else {
      glDisable(GL_LINE_STIPPLE);
    }
    previousLineType = widgetdata.LineType_;
  }
}

void glWidget::setEDGEType() {
  glPointSize(widgetdata.EDGEThick_ * 1.5f);

  if (widgetdata.EDGEType_) {
    if (widgetdata.EDGEType_ == 1) glEnable(GL_POINT_SMOOTH);
    if (widgetdata.EDGEType_ == 2) glDisable(GL_POINT_SMOOTH);
    glColor3f(widgetdata.EDGEColor_.redF(), widgetdata.EDGEColor_.greenF(),
              widgetdata.EDGEColor_.blueF());
    glDrawElements(GL_POINTS, static_cast<GLsizei>(data.faces.size()),
                   GL_UNSIGNED_INT, data.faces.data());
  }
}

void glWidget::loadBGColor() {
  if (widgetdata.BGColor_ != widgetdata.prevBGColor_) {
    glClearColor(widgetdata.BGColor_.redF(), widgetdata.BGColor_.greenF(),
                 widgetdata.BGColor_.blueF(), widgetdata.BGColor_.alphaF());
    widgetdata.prevBGColor_ = widgetdata.BGColor_;
    update();
  }
}

void glWidget::setScale() const {
  float baseScale = 1;

  if (widgetdata.Scale_ > 0)
    baseScale *= static_cast<float>(widgetdata.Scale_ * 1.1);
  else if (widgetdata.Scale_ < 0)
    baseScale = static_cast<float>(1.0 / (widgetdata.Scale_ * -1.1));

  glScalef(baseScale, baseScale, baseScale);
}

void glWidget::setTypeViews() const {
  if (widgetdata.ViewType_ == 1) glShadeModel(GL_FLAT);
  if (widgetdata.ViewType_ == 2) glShadeModel(GL_SMOOTH);
}

void glWidget::typeViewsModel() {
  if (widgetdata.ViewType_ == 0)
    paintWireFrame();
  else
    paintShading();
}

void glWidget::paintWireFrame() {
  glEnableClientState(GL_VERTEX_ARRAY);

  setTypeLine();
  glLineWidth(widgetdata.LineThick_);

  glVertexPointer(3, GL_FLOAT, 0, data.vertices.data());  // каркасный рисунок
  glDrawElements(GL_LINES, static_cast<GLsizei>(data.faces.size()),
                 GL_UNSIGNED_INT, data.faces.data());
  setEDGEType();
}

void glWidget::paintShading() {
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);
  glEnable(GL_TEXTURE_2D);
  setTypeViews();
  loadTexture();
  setLight();

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  glVertexPointer(3, GL_FLOAT, 0, data.verticesShade.data());
  glTexCoordPointer(2, GL_FLOAT, 0, data.vertexTextureShade.data());
  glNormalPointer(GL_FLOAT, 0, data.vertexNormalShade.data());

  glDrawArrays(GL_TRIANGLES, 0,
               static_cast<GLsizei>(data.verticesShade.size() / 3));

  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_NORMALIZE);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHTING);
}

void glWidget::SetCenterModel() {
  widgetdata.Scale_ = 1.0f;
  widgetdata.RotX_ = 0;
  widgetdata.RotY_ = 0;
  widgetdata.RotZ_ = 0;

  float modelWidth = data.maxX - data.minX;
  float modelHeight = data.maxY - data.minY;
  float zOffset = modelWidth + modelHeight;

  widgetdata.PosX_ = 0;
  widgetdata.PosY_ = 0;
  widgetdata.PosZ_ = -zOffset;

  glRotatef(90, 1.0f, 0.0f, 0.0f);

  projectionChange();
  update();
}

void glWidget::loadTexture() {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, GLsizei(widgetdata.Texture_.width()),
               GLsizei(widgetdata.Texture_.height()), 0, GL_RGBA,
               GL_UNSIGNED_BYTE, widgetdata.Texture_.bits());
}

void glWidget::setLight() {
  GLfloat lightPosition[4] = {widgetdata.posLight_X, widgetdata.posLight_Y,
                              widgetdata.posLight_Z, 0.0f};
  GLfloat lightColor[4] = {widgetdata.light_R, widgetdata.light_G,
                           widgetdata.light_B, 1.0f};  // 1.0f для альфа-канала

  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
}

}  // namespace s21
