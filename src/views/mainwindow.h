#ifndef CPP4_3DVIEWER_V2_0_SRC_VIEWS_MAINWINDOW_H_
#define CPP4_3DVIEWER_V2_0_SRC_VIEWS_MAINWINDOW_H_

#include <ui_mainwindow.h>

#include <QColorDialog>
#include <QPainter>
#include <QSettings>

#include "controllers/controller.h"
#include "patterns/application_comands.h"
#include "patterns/application_strategy.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(s21::Controller* controller, QWidget* parent = nullptr);
  ~MainWindow() override;

 private slots:
  void on_openFile_clicked();
  void on_ScreenShot_clicked();
  void on_Gif_clicked();
  void on_Reset_clicked();
  void on_Background_color_clicked();
  void on_Vertex_color_clicked();
  void on_color_edge_clicked();

  void on_pushButton_clicked();

  void on_LoadTexture_button_clicked();

  void on_ClearTexture_button_clicked();

  void on_SaveUV_buttonn_clicked();

  void on_ViewType_box_currentIndexChanged(int index);

  void on_Light_button_clicked();

  private:
  Ui::MainWindow* ui_;
  QSettings settings_;
  s21::Controller* controller_;
  s21::CommandInvoker commandInvoker_;

  void SaveSettings();
  void LoadSettings();
  void initSliders();
  void initChangeBoxes();
  void initComboBox(QComboBox* comboBox, std::function<void(int)> updateFunc);
  void initSlider(QSlider* slider, QSpinBox* spinBox,
                  std::function<void(int)> updateFunc);
  static void resetComboBox(QComboBox* box, int index);
  static void resetSlider(QSlider* slider, int value);
  void rotateModelOverTime(QString rotationAxis);
};

#endif  // CPP4_3DVIEWER_V2_0_SRC_VIEWS_MAINWINDOW_H_
