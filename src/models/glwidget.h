#ifndef CPP4_3DVIEWER_V2_0_SRC_MODELS_GLWIDGET_H_
#define CPP4_3DVIEWER_V2_0_SRC_MODELS_GLWIDGET_H_

#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QTimer>

#include "parse.h"

namespace s21 {

class glWidget : public QOpenGLWidget, protected QOpenGLFunctions {
 public:
  explicit glWidget(QWidget *parent = nullptr);

  int ProjectionType, LineType, EDGEType, ViewType;
  float LineThick, EDGEThick, Scale;

  QColor LineColor;
  QColor BGColor;
  QColor EDGEColor;

  GLfloat PosX;  // Позиция модели по оси X
  GLfloat PosY;  // Позиция модели по оси Y
  GLfloat PosZ;  // Позиция модели по оси Z

  GLfloat RotX;  // Поле для хранения значения вращения по оси X
  GLfloat RotY;
  GLfloat RotZ;

  s21::Attrib data = {0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, {}, {}};

  void SetBGColor();
  void SetCenterModel();

 public slots:
  void SetScaleFromSlider(int sliderValue);

 private:
  QPoint mPos_;
  QTimer tmr_;
  QColor prevBGColor_;

  void mousePressEvent(QMouseEvent *) override;
  void mouseMoveEvent(QMouseEvent *) override;
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;
  void wheelEvent(QWheelEvent *) override;
  void ProjectionChange();
  void SetTypeLine();
  void SetEDGEType();
  void setPerspectiveProjection();
  void setOrthographicProjection();
  void setTypeViews();
  void PaintWireFrame();
  void TypeViewsModel();
  void PaintShading();
  GLfloat calculateAspect();
  std::tuple<float, float, float> calculateModelDimensions() const;
};
}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_SRC_MODELS_GLWIDGET_H_