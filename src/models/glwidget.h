#ifndef CPP4_3DVIEWER_V2_0_SRC_MODELS_GLWIDGET_H_
#define CPP4_3DVIEWER_V2_0_SRC_MODELS_GLWIDGET_H_

#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QTimer>

#include "parse.h"

namespace s21 {

struct WidgetData {
  QImage Texture_;
  QColor LineColor_;
  QColor EDGEColor_;

  GLfloat PosX_, PosY_, PosZ_;
  GLfloat RotX_, RotY_, RotZ_;

  QPoint mPos_;
  QColor prevBGColor_;
  QColor BGColor_;

  GLfloat light_R, light_G, light_B;
  GLfloat posLight_X, posLight_Y, posLight_Z;

  int ProjectionType_, LineType_, EDGEType_, ViewType_ = 0;
  float LineThick_, EDGEThick_, Scale_;
};

class glWidget : public QOpenGLWidget, protected QOpenGLFunctions {

private:
  WidgetData widgetdata;

  void loadBGColor();
  void setScale() const;

  void mousePressEvent(QMouseEvent *) override;
  void mouseMoveEvent(QMouseEvent *) override;
  void wheelEvent(QWheelEvent *) override;
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;
  void projectionChange();
  void setTypeLine();
  void setEDGEType();
  void setPerspectiveProjection();
  void setOrthographicProjection();
  void setTypeViews() const;
  void paintWireFrame();
  void typeViewsModel();
  void paintShading();
  void loadTexture();
  void rotateModel() const;

  GLfloat calculateAspect();
  [[nodiscard]] std::tuple<float, float, float> calculateModelDimensions() const;

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

  s21::Attrib data = {0, 0, 0, 0, 0, 0, {}, {}, {}, {}, {}, {}, {}};

  void SetCenterModel();
  void setLight();
};
} // namespace s21

#endif // CPP4_3DVIEWER_V2_0_SRC_MODELS_GLWIDGET_H_