#include "glwidget.h"

namespace s21 {

glWidget::glWidget(QWidget *parent) : QOpenGLWidget(parent) {
  setMouseTracking(true);
}

void glWidget::initializeGL() {
  initializeOpenGLFunctions();
  glEnable(GL_DEPTH_TEST);
  glEnableClientState(GL_VERTEX_ARRAY);
  LoadBGColor();
}

void glWidget::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void glWidget::paintGL() {
  if (data.vertices.data()) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ProjectionChange();
    glTranslatef(widgetdata.PosX, widgetdata.PosY, widgetdata.PosZ);
    SetScale();
    rotateModel();
    LoadBGColor();
    SetTypeLine();
    setTypeViews();
    glColor3f(widgetdata.LineColor.redF(), widgetdata.LineColor.greenF(),
              widgetdata.LineColor.blueF());
    TypeViewsModel();
  } else
    LoadBGColor();
}

void glWidget::rotateModel() {
  glRotatef(widgetdata.RotX, 1.0f, 0.0f, 0.0f);
  glRotatef(widgetdata.RotY, 0.0f, 1.0f, 0.0f);
  glRotatef(widgetdata.RotZ, 0.0f, 0.0f, 1.0f);
}

void glWidget::mousePressEvent(QMouseEvent *ma) {
  widgetdata.mPos_ = ma->pos();
}

void glWidget::mouseMoveEvent(QMouseEvent *ma) {
  if (ma->buttons() & Qt::RightButton) {
    GLfloat dx = static_cast<GLfloat>(ma->pos().x() - widgetdata.mPos_.x());
    GLfloat dy = static_cast<GLfloat>(ma->pos().y() - widgetdata.mPos_.y());

    widgetdata.PosX += dx / 10;
    widgetdata.PosY -= dy / 10;
    widgetdata.mPos_ = ma->pos();
    update();
  } else if (ma->buttons() & Qt::LeftButton) {
    GLfloat dx = static_cast<GLfloat>(ma->pos().x() - widgetdata.mPos_.x());
    GLfloat dy = static_cast<GLfloat>(ma->pos().y() - widgetdata.mPos_.y());

    widgetdata.RotX += 1 / M_PI * dy;
    widgetdata.RotY += 1 / M_PI * dx;

    widgetdata.mPos_ = ma->pos();
    update();
  }
}

void glWidget::wheelEvent(QWheelEvent *event) {
  int delta = event->angleDelta().y();
  if (widgetdata.Scale == 0) widgetdata.Scale = 1;
  if (delta > 0)
    widgetdata.Scale *= 1.1;
  else if (delta < 0)
    widgetdata.Scale /= 1.1;
}

void glWidget::ProjectionChange() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (widgetdata.ProjectionType == 0) {
    setPerspectiveProjection();
  } else if (widgetdata.ProjectionType == 1) {
    setOrthographicProjection();
  }
}

void glWidget::setPerspectiveProjection() {
  GLfloat aspect = calculateAspect();
  GLfloat nearPlane = 0.001f;
  GLfloat farPlane = 5500.0f;
  GLfloat fov = 60.0f;

  GLfloat top = nearPlane * static_cast<float>(tanf(fov * M_PI / 360.0));
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

void glWidget::SetTypeLine() {
  if (widgetdata.LineType == 1) {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xF0F0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(widgetdata.LineThick);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(widgetdata.LineThick);
  }
}

void glWidget::SetEDGEType() {
  glPointSize(widgetdata.EDGEThick * 1.5);

  if (widgetdata.EDGEType) {
    if (widgetdata.EDGEType == 1) glEnable(GL_POINT_SMOOTH);
    if (widgetdata.EDGEType == 2) glDisable(GL_POINT_SMOOTH);
    glColor3f(widgetdata.EDGEColor.redF(), widgetdata.EDGEColor.greenF(),
              widgetdata.EDGEColor.blueF());
    glDrawElements(GL_POINTS, data.numFaces, GL_UNSIGNED_INT,
                   data.faces.data());
  }
}

void glWidget::LoadBGColor() {
  if (widgetdata.BGColor_ != widgetdata.prevBGColor_) {
    glClearColor(widgetdata.BGColor_.redF(), widgetdata.BGColor_.greenF(),
                 widgetdata.BGColor_.blueF(), widgetdata.BGColor_.alphaF());
    widgetdata.prevBGColor_ = widgetdata.BGColor_;
    update();
  }
}

void glWidget::SetScale() {
  float baseScale = 1;

  if (widgetdata.Scale > 0) {
    baseScale *= widgetdata.Scale * 1.1;
  } else if (widgetdata.Scale < 0) {
    baseScale = 1 / (widgetdata.Scale * -1.1);
  }
  glScalef(baseScale, baseScale, baseScale);

  update();
}

void glWidget::setTypeViews() {
  if (widgetdata.ViewType == 1) {
    glShadeModel(GL_FLAT);
  }
  if (widgetdata.ViewType == 2) {
    glShadeModel(GL_SMOOTH);
  }
}

void glWidget::TypeViewsModel() {
  if (widgetdata.ViewType == 0) {
    PaintWireFrame();
  } else
    PaintShading();
}

void glWidget::PaintWireFrame() {
  glVertexPointer(3, GL_FLOAT, 0, data.vertices.data());  //каркасный рисунок
  glDrawElements(GL_LINES, data.numFaces, GL_UNSIGNED_INT, data.faces.data());
  SetEDGEType();
}

void glWidget::PaintShading() {
  glEnable(GL_LIGHTING);  //теневой и текстурный рисунок
  glEnable(GL_LIGHT0);  //теневой и текстурный рисунок
  glEnable(GL_COLOR_MATERIAL);  //теневой и текстурный рисунок
  glEnable(GL_NORMALIZE);  //теневой и текстурный рисунок
  glEnable(GL_TEXTURE_2D);  //текстурный рисунок
  setTypeViews();
  LoadTexture();

  glEnableClientState(GL_TEXTURE_COORD_ARRAY);  //текстурный рисунок
  glEnableClientState(GL_NORMAL_ARRAY);  //теневой и текстурный рисунок

  // Массив вершин
  std::vector<float> vertices = {
      0.0,  1.0, 0.0,  // Вершина пирамиды
      -1.0, 0.0, -1.0,  // Нижняя левая вершина основания
      1.0,  0.0, -1.0,  // Нижняя правая вершина основания
      1.0,  0.0, 1.0,  // Верхняя правая вершина основания
      -1.0, 0.0, 1.0  // Верхняя левая вершина основания
  };

  // Массив текстурных координат
  std::vector<float> vertexTexture = {0.5, 1.0, 0.0, 0.0, 1.0,
                                      0.0, 1.0, 0.0, 0.0, 0.0};

  // Массив нормалей
  std::vector<float> vertexNormal = {0.0, 1.0, 0.0, -1.0, 0.0, 0.0, 1.0, 0.0,
                                     0.0, 0.0, 0.0, 1.0,  0.0, 0.0, -1.0};

  // Индексный массив
  std::vector<unsigned int> faces = {0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1};

  glVertexPointer(3, GL_FLOAT, 0,
                  vertices.data());  //теневой и текстурный рисунок
  glNormalPointer(GL_FLOAT, 0,
                  vertexNormal.data());  //теневой и текстурный рисунок
  glTexCoordPointer(2, GL_FLOAT, 0,
                    vertexTexture.data());  //текстурный рисунок
  glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT,
                 faces.data());  // теневой и текстурный рисунок

  glDisable(GL_TEXTURE_2D);  //текстурный рисунок
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);  //текстурный рисунок
  glDisable(GL_LIGHTING);  //теневой и текстурный рисунок
  glDisable(GL_LIGHT0);  //теневой и текстурный рисунок
  glDisable(GL_COLOR_MATERIAL);  //теневой и текстурный рисунок
  glDisable(GL_NORMALIZE);  //теневой и текстурный рисунок
  glDisableClientState(GL_NORMAL_ARRAY);  //теневой и текстурный рисунок
}

void glWidget::SetCenterModel() {
  widgetdata.Scale = 1.0f;
  widgetdata.RotX = 0;
  widgetdata.RotY = 0;
  widgetdata.RotZ = 0;

  // Вычисляем размер модели по X и Y
  float modelWidth = data.maxX - data.minX;
  float modelHeight = data.maxY - data.minY;

  // Вычисляем смещение камеры по Z,
  // чтобы уместить модель по X и Y
  float zOffset = modelWidth + modelHeight;

  widgetdata.PosX = 0;
  widgetdata.PosY = 0;
  widgetdata.PosZ = -zOffset;

  glRotatef(90, 1.0f, 0.0f, 0.0f);

  ProjectionChange();
  update();
}

void glWidget::LoadTexture() {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, GLsizei(widgetdata.texture_.width()),
               GLsizei(widgetdata.texture_.height()), 0, GL_RGBA,
               GL_UNSIGNED_BYTE, widgetdata.texture_.bits());
}

void glWidget::ButtonLightAllFunc() {
  // double value_x = ui_->lineEditLightX->text().toDouble();
  // double value_y = ui_->lineEditLightY->text().toDouble();
  // double value_z = ui_->lineEditLightZ->text().toDouble();
  // ui_->widget->SetPositionX(value_x);
  // ui_->widget->SetPositionY(value_y);
  // ui_->widget->SetPositionZ(value_z);
  // ui_->widget->update();
}

void glWidget::setLight() {
  float ambient_light[3];
  widgetdata.LightColor.getRgbF(&ambient_light[0], &ambient_light[1],
                                &ambient_light[2]);
  glLightfv(GL_LIGHT0, GL_POSITION, widgetdata.position_light);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
}

}  // namespace s21
