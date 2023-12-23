#ifndef CPP4_3DVIEWER_V2_0_SRC_MODELS_GLWIDGET_H_
#define CPP4_3DVIEWER_V2_0_SRC_MODELS_GLWIDGET_H_

#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QTimer>

#include "parse.h"

namespace s21 {

struct WidgetData {
  QImage texture_;
  QColor LineColor;
  QColor EDGEColor;
  QColor LightColor;

  GLfloat PosX;
  GLfloat PosY;
  GLfloat PosZ;

  GLfloat RotX;
  GLfloat RotY;
  GLfloat RotZ;

  QPoint mPos_;
  QTimer tmr_;
  QColor prevBGColor_;
  QColor BGColor_;

  int ProjectionType, LineType, EDGEType, ViewType = 0;
  float LineThick, EDGEThick, Scale;
  float position_light[3];
};

class glWidget : public QOpenGLWidget, protected QOpenGLFunctions {

private:
  WidgetData widgetdata;

  void LoadBGColor();
  void SetScale();

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
  void LoadTexture();
  void rotateModel();

  GLfloat calculateAspect();
  std::tuple<float, float, float> calculateModelDimensions() const;

public:
  explicit glWidget(QWidget *parent = nullptr);

  // Универсальный геттер
  template <typename T> const T &getMember(T WidgetData::*member) const {
    return widgetdata.*member;
  }

  // Универсальный сеттер
  template <typename T> void setMember(T WidgetData::*member, const T &value) {
    widgetdata.*member = value;
  }

  s21::Attrib data = {0, 0, 0, 0, 0, 0, 0, 0, 0, {}, {}, {}, {}, {}, {}};

  void SetCenterModel();
  void ButtonLightAllFunc();
  void setLight();
};
} // namespace s21

#endif // CPP4_3DVIEWER_V2_0_SRC_MODELS_GLWIDGET_H_