#include "glwidget.h"

namespace s21 {

glWidget::glWidget(QWidget *parent) : QOpenGLWidget(parent), Scale(1.0f) {
  setMouseTracking(true);
}

void glWidget::initializeGL() {
  initializeOpenGLFunctions();
  glEnable(GL_DEPTH_TEST);
  glEnableClientState(GL_VERTEX_ARRAY);
  SetBGColor();
}

void glWidget::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void glWidget::paintGL() {
  if (data.vertices.data()) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    LoadTexture();
    ProjectionChange();
    glTranslatef(PosX, PosY, PosZ);
    glScalef(Scale, Scale, Scale);
    glRotatef(RotX, 1.0f, 0.0f, 0.0f);
    glRotatef(RotY, 0.0f, 1.0f, 0.0f);
    glRotatef(RotZ, 0.0f, 0.0f, 1.0f);
    SetBGColor();
    SetTypeLine();
    setTypeViews();
    glColor3f(LineColor.redF(), LineColor.greenF(), LineColor.blueF());
    TypeViewsModel();
  } else
    SetBGColor();
}

void glWidget::mousePressEvent(QMouseEvent *ma) { mPos_ = ma->pos(); }

void glWidget::mouseMoveEvent(QMouseEvent *ma) {
  if (ma->buttons() & Qt::RightButton) {
    GLfloat dx = static_cast<GLfloat>(ma->pos().x() - mPos_.x());
    GLfloat dy = static_cast<GLfloat>(ma->pos().y() - mPos_.y());

    PosX += dx / 10;
    PosY -= dy / 10;
    mPos_ = ma->pos();
    update();
  } else if (ma->buttons() & Qt::LeftButton) {
    RotX = 1 / M_PI * (ma->pos().y() - mPos_.y());
    RotY = 1 / M_PI * (ma->pos().x() - mPos_.x());
    update();
  }
}

void glWidget::wheelEvent(QWheelEvent *event) {
  int delta = event->angleDelta().y();

  if (delta > 0)
    Scale *= 1.1;
  else if (delta < 0)
    Scale /= 1.1;

  update();
}

void glWidget::ProjectionChange() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (ProjectionType == 0) {
    setPerspectiveProjection();
  } else if (ProjectionType == 1) {
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
  if (LineType == 1) {
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xF0F0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(LineThick);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(LineThick);
  }
}

void glWidget::SetEDGEType() {
  glPointSize(EDGEThick * 1.5);

  if (EDGEType) {
    if (EDGEType == 1)
      glEnable(GL_POINT_SMOOTH);
    if (EDGEType == 2)
      glDisable(GL_POINT_SMOOTH);
    glColor3f(EDGEColor.redF(), EDGEColor.greenF(), EDGEColor.blueF());
    glDrawElements(GL_POINTS, data.numFaces, GL_UNSIGNED_INT,
                   data.faces.data());
  }
}

void glWidget::SetBGColor() {
  // Проверяем, изменились ли значения цвета фона
  if (BGColor != prevBGColor_) {
    glClearColor(BGColor.redF(), BGColor.greenF(), BGColor.blueF(),
                 BGColor.alphaF());
    prevBGColor_ = BGColor;
    update();
  }
}

void glWidget::SetScaleFromSlider(int sliderValue) {
  // Нормализация значения слайдера в диапазоне от -10 до 10
  float normalizedValue = static_cast<float>(sliderValue + 10) / 20.0f;
  Scale = 0.5f + normalizedValue * 1.5f;
  update();
}

void glWidget::setTypeViews() {
  if (ViewType == 1) {
    glShadeModel(GL_FLAT);
  }
  if (ViewType == 2) {
    glShadeModel(GL_SMOOTH);
  }
}

void glWidget::TypeViewsModel() {
  if (ViewType == 0) {
    PaintWireFrame();
  } else
    PaintShading();
}

void glWidget::PaintWireFrame() {
  glVertexPointer(3, GL_FLOAT, 0, data.vertices.data()); //каркасный рисунок
  glDrawElements(GL_LINES, data.numFaces, GL_UNSIGNED_INT, data.faces.data());
  SetEDGEType();
}

void glWidget::PaintShading() {
  glEnable(GL_LIGHTING); //теневой и текстурный рисунок
  glEnable(GL_LIGHT0);   //теневой и текстурный рисунок
  glEnable(GL_COLOR_MATERIAL); //теневой и текстурный рисунок
  glEnable(GL_NORMALIZE); //теневой и текстурный рисунок
  glEnable(GL_TEXTURE_2D); //текстурный рисунок
  setTypeViews();
  glEnableClientState(GL_TEXTURE_COORD_ARRAY); //текстурный рисунок
  glEnableClientState(GL_NORMAL_ARRAY); //теневой и текстурный рисунок

  // Массив вершин
  std::vector<float> vertices = {
      0.0, 1.0, 0.0,    // Вершина пирамиды
      -1.0, 0.0, -1.0,  // Нижняя левая вершина основания
      1.0, 0.0, -1.0,   // Нижняя правая вершина основания
      1.0, 0.0, 1.0,    // Верхняя правая вершина основания
      -1.0, 0.0, 1.0    // Верхняя левая вершина основания
  };

// Массив текстурных координат
  std::vector<float> vertexTexture = {
      0.5, 1.0,
      0.0, 0.0,
      1.0, 0.0,
      1.0, 0.0,
      0.0, 0.0
  };

// Массив нормалей
  std::vector<float> vertexNormal = {
      0.0, 1.0, 0.0,
      -1.0, 0.0, 0.0,
      1.0, 0.0, 0.0,
      0.0, 0.0, 1.0,
      0.0, 0.0, -1.0
  };

// Индексный массив
  std::vector<unsigned int> faces = {
      0, 1, 2,
      0, 2, 3,
      0, 3, 4,
      0, 4, 1
  };

  glVertexPointer(3, GL_FLOAT, 0,
                  vertices.data()); //теневой и текстурный рисунок
  glNormalPointer(GL_FLOAT, 0,
                  vertexNormal.data()); //теневой и текстурный рисунок
  glTexCoordPointer(2, GL_FLOAT, 0,
                    vertexTexture.data()); //текстурный рисунок
  glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT,
                 faces.data()); // теневой и текстурный рисунок

  glDisable(GL_TEXTURE_2D); //текстурный рисунок
  glDisableClientState(GL_TEXTURE_COORD_ARRAY); //текстурный рисунок
  glDisable(GL_LIGHTING); //теневой и текстурный рисунок
  glDisable(GL_LIGHT0); //теневой и текстурный рисунок
  glDisable(GL_COLOR_MATERIAL); //теневой и текстурный рисунок
  glDisable(GL_NORMALIZE); //теневой и текстурный рисунок
  glDisableClientState(GL_NORMAL_ARRAY); //теневой и текстурный рисунок
}

void glWidget::SetCenterModel() {
  Scale = 1.0f;
  RotX = 0;
  RotY = 0;
  RotZ = 0;

  // Вычисляем размер модели по X и Y
  float modelWidth = data.maxX - data.minX;
  float modelHeight = data.maxY - data.minY;

  // Вычисляем смещение камеры по Z,
  // чтобы уместить модель по X и Y
  float zOffset = modelWidth + modelHeight;

  PosX = 0;
  PosY = 0;
  PosZ = -zOffset;

  glRotatef(90, 1.0f, 0.0f, 0.0f);

  ProjectionChange();
  update();
}

void glWidget::LoadTexture() {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, GLsizei(texture_.width()),
               GLsizei(texture_.height()), 0, GL_RGBA, GL_UNSIGNED_BYTE,
               texture_.bits());
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
    LightColor.getRgbF(&ambient_light[0], &ambient_light[1],
                         &ambient_light[2]);
    glLightfv(GL_LIGHT0, GL_POSITION, position_light);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
}

} // namespace s21
