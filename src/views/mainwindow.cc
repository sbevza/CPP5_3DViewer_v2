#include "mainwindow.h"

MainWindow::MainWindow(s21::Controller *controller, QWidget *parent)
    : QMainWindow(parent), ui_(new Ui::MainWindow),
      settings_("School_21", "3D_Viewer_2.0"), controller_(controller) {
  ui_->setupUi(this);

  QIcon icon(":/icon.png");
  setWindowIcon(icon);
  setWindowTitle("3D_Viewer_2.0");

  LoadSettings();
  initChangeBoxes();
  initSliders();
  ui_->ClearTexture_button->setEnabled(false);
  ui_->LoadTexture_button->setEnabled(false);
  ui_->SaveUV_buttonn->setEnabled(false);

  statusBar()->showMessage("Для начала откройте файл модели!");
}

void MainWindow::initComboBox(QComboBox *comboBox,
                              std::function<void(int)> updateFunc) {
  connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          [this, updateFunc](int index) {
            updateFunc(index);
            SaveSettings();
          });
}

void MainWindow::initChangeBoxes() {
  initComboBox(ui_->type_line_box, [=](int index) {
    ui_->openGLWidget->setMember(&s21::WidgetData::LineType_, index);
  });
  initComboBox(ui_->Projections_box, [=](int index) {
    ui_->openGLWidget->setMember(&s21::WidgetData::ProjectionType_, index);
  });
  initComboBox(ui_->edge_box, [=](int index) {
    ui_->openGLWidget->setMember(&s21::WidgetData::EDGEType_, index);
  });
  initComboBox(ui_->ViewType_box, [=](int index) {
    ui_->openGLWidget->setMember(&s21::WidgetData::ViewType_, index);
  });
}

void MainWindow::initSlider(QSlider *slider, QSpinBox *spinBox,
                            std::function<void(int)> updateFunc) {
  auto weakSlider = QPointer<QSlider>(slider);
  connect(spinBox, qOverload<int>(&QSpinBox::valueChanged), this,
          [this, weakSlider, updateFunc](int value) {
            if (weakSlider) {
              updateFunc(value);
              weakSlider->setValue(value);
              SaveSettings();
            }
          });
  connect(slider, &QSlider::valueChanged, spinBox, &QSpinBox::setValue);
}

void MainWindow::initSliders() {
  initSlider(ui_->Slider_X, ui_->X_digit, [=](GLfloat value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::PosX_, value / 4.0f);
  });

  initSlider(ui_->Slider_Y, ui_->Y_digit, [=](GLfloat value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::PosY_, value);
  });
  initSlider(ui_->Slider_Z, ui_->Z_digit, [=](GLfloat value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::PosZ_, value / 0.8f);
  });

  initSlider(ui_->Slider_X_Rotate, ui_->X_Rotate_digit, [=](GLfloat value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::RotX_, value);
  });
  initSlider(ui_->Slider_Y_Rotate, ui_->Y_Rotate_digit, [=](GLfloat value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::RotY_, value);
  });
  initSlider(ui_->Slider_Z_Rotate, ui_->Z_Rotate_digit, [=](GLfloat value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::RotZ_, value);
  });
  initSlider(ui_->Slider_thickness, ui_->thickness_digit, [=](float value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::LineThick_, value);
  });
  initSlider(ui_->Slider_edge, ui_->Edge_digit, [=](float value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::EDGEThick_, value);
  });
  initSlider(ui_->Slider_size, ui_->Size, [=](float value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::Scale_, value);
  });
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::on_openFile_clicked() {
  s21::OpenCommand openCommand;
  QString fileName = commandInvoker_.runCommand(&openCommand);

  if (!fileName.isEmpty()) {
    statusBar()->showMessage("Открыт файл: " + fileName);
    std::string str = fileName.toStdString();
    controller_->parse_obj(ui_->openGLWidget->data, str);
    if (!controller_->hasError()) {
      ui_->openGLWidget->SetCenterModel();
      ui_->openGLWidget->update();
      statusBar()->showMessage(
          "Открыли файл: " + fileName + " Количество вершин: " +
          QString::number(ui_->openGLWidget->data.vertices.size() / 3) +
          " Количество граней: " +
          QString::number(ui_->openGLWidget->data.faces.size() / 2));
    } else {
      statusBar()->showMessage("Ошибка открытия файла");
    }
  }
}

void MainWindow::on_ScreenShot_clicked() {
  if (ui_->statusbar->currentMessage() != "Для начала откройте файл модели!") {
    s21::SaveScreenshotCommand saveScreenshot;
    QString filePath = commandInvoker_.runCommand(&saveScreenshot);

    if (!filePath.isEmpty()) {
      s21::MediaMaker mediaMaker;
      mediaMaker.SetMedia(makeImage, ui_);
      mediaMaker.MakeMedia(filePath);
    }
  }
}

void MainWindow::on_Gif_clicked() {
  if (ui_->statusbar->currentMessage() != "Для начала откройте файл модели!") {
    s21::SaveGifCommand saveGif;
    ui_->statusbar->showMessage("укажите папку для сохранения Gif");
    QString filePath = commandInvoker_.runCommand(&saveGif);

    if (!filePath.isEmpty()) {
      s21::MediaMaker mediaMaker;
      mediaMaker.SetMedia(makeGif, ui_);

      if (ui_->TypeGif_box->currentIndex() == 1) {
        rotateModelOverTime("RotX");
        // mediaMaker.MakeMedia(filePath);
      }
      if (ui_->TypeGif_box->currentIndex() == 2) {
        rotateModelOverTime("RotY");
        // mediaMaker.MakeMedia(filePath);
      }
      if (ui_->TypeGif_box->currentIndex() == 3) {
        rotateModelOverTime("RotZ");
        // mediaMaker.MakeMedia(filePath);
      }
      mediaMaker.MakeMedia(filePath);
    }
  }
}

void MainWindow::resetComboBox(QComboBox *box, int index = 0) {
  box->setCurrentIndex(index);
  box->currentIndexChanged(index);
}

void MainWindow::resetSlider(QSlider *slider, int value = 0) {
  slider->setValue(value);
}

void MainWindow::on_Reset_clicked() {
  resetComboBox(ui_->Projections_box);
  resetComboBox(ui_->type_line_box);
  resetComboBox(ui_->edge_box);

  resetSlider(ui_->Slider_thickness, 1);
  resetSlider(ui_->Slider_edge, 1);
  resetSlider(ui_->Slider_X);
  resetSlider(ui_->Slider_Y);
  resetSlider(ui_->Slider_Z);
  resetSlider(ui_->Slider_X_Rotate);
  resetSlider(ui_->Slider_Y_Rotate);
  resetSlider(ui_->Slider_Z_Rotate);
  resetSlider(ui_->Slider_size);

  ui_->openGLWidget->setMember(&s21::WidgetData::EDGEColor_, QColor(Qt::red));
  ui_->openGLWidget->setMember(&s21::WidgetData::BGColor_, QColor(Qt::black));
  ui_->openGLWidget->setMember(&s21::WidgetData::LineColor_, QColor(Qt::white));

  ui_->openGLWidget->update();

  SaveSettings();
}

void MainWindow::SaveSettings() {
  settings_.setValue("BGColor",
                     ui_->openGLWidget->getMember(&s21::WidgetData::BGColor_));
  settings_.setValue(
      "EDGEColor", ui_->openGLWidget->getMember(&s21::WidgetData::EDGEColor_));
  settings_.setValue(
      "EDGEThick", ui_->openGLWidget->getMember(&s21::WidgetData::EDGEThick_));
  settings_.setValue("EDGEType",
                     ui_->openGLWidget->getMember(&s21::WidgetData::EDGEType_));
  settings_.setValue(
      "LineColor", ui_->openGLWidget->getMember(&s21::WidgetData::LineColor_));
  settings_.setValue(
      "LineThick", ui_->openGLWidget->getMember(&s21::WidgetData::LineThick_));
  settings_.setValue("LineType",
                     ui_->openGLWidget->getMember(&s21::WidgetData::LineType_));
  settings_.setValue("ProjectionType", ui_->openGLWidget->getMember(
                                           &s21::WidgetData::ProjectionType_));

  ui_->openGLWidget->update();
}

void MainWindow::LoadSettings() {
  ui_->Projections_box->setCurrentIndex(
      settings_.value("ProjectionType", 0).toInt());

  ui_->Slider_thickness->setValue(settings_.value("LineThick", 1.0).toInt());

  ui_->Slider_edge->setValue(settings_.value("EDGEThick", 1.0).toInt());

  ui_->type_line_box->setCurrentIndex(settings_.value("LineType", 0).toInt());

  ui_->edge_box->setCurrentIndex(settings_.value("EDGEType", 0).toInt());

  ui_->thickness_digit->setValue(settings_.value("LineThick", 1).toInt());

  ui_->openGLWidget->setMember(&s21::WidgetData::EDGEThick_,
                               settings_.value("EDGEThick", 1.0).toFloat());
  ui_->Edge_digit->setValue(settings_.value("EDGEThick", 1).toInt());

  ui_->openGLWidget->setMember(
      &s21::WidgetData::BGColor_,
      settings_.value("BGColor", QColor(Qt::black)).value<QColor>());

  ui_->openGLWidget->setMember(
      &s21::WidgetData::LineColor_,
      settings_.value("LineColor", QColor(Qt::white)).value<QColor>());
  ui_->openGLWidget->setMember(
      &s21::WidgetData::EDGEColor_,
      settings_.value("EDGEColor", QColor(Qt::black)).value<QColor>());
  ui_->openGLWidget->setMember(&s21::WidgetData::ProjectionType_,
                               settings_.value("ProjectionType", 0).toInt());
  ui_->openGLWidget->setMember(&s21::WidgetData::LineType_,
                               settings_.value("LineType", 0).toInt());
  ui_->openGLWidget->setMember(&s21::WidgetData::EDGEType_,
                               settings_.value("EDGEType", 0).toInt());
  ui_->openGLWidget->setMember(&s21::WidgetData::LineThick_,
                               settings_.value("LineThick", 1.0).toFloat());

  ui_->openGLWidget->update();
  update();
}

void MainWindow::on_Vertex_color_clicked() {
  QColor color = QColorDialog::getColor(Qt::white, this, "Выберите цвет");
  if (color.isValid()) {
    ui_->openGLWidget->setMember(&s21::WidgetData::LineColor_, color);
    SaveSettings();
  }
}

void MainWindow::on_color_edge_clicked() {
  QColor color = QColorDialog::getColor(Qt::white, this, "Выберите цвет");
  if (color.isValid()) {
    ui_->openGLWidget->setMember(&s21::WidgetData::EDGEColor_, color);
    SaveSettings();
  }
}

void MainWindow::rotateModelOverTime(QString rotationAxis) {
  float i = 0;
  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, [=]() mutable {
    if (rotationAxis == "RotX")
      ui_->openGLWidget->setMember(&s21::WidgetData::RotX_, i++);
    if (rotationAxis == "RotY")
      ui_->openGLWidget->setMember(&s21::WidgetData::RotY_, i++);
    if (rotationAxis == "RotZ")
      ui_->openGLWidget->setMember(&s21::WidgetData::RotZ_, i++);
    ui_->openGLWidget->update();
    if (i == 360)
      timer->stop();
  });
  timer->start(16);
}

void MainWindow::on_Background_color_clicked() {
  QColor color = QColorDialog::getColor(Qt::white, this, "Выберите цвет");

  if (color.isValid()) {
    ui_->openGLWidget->setMember(&s21::WidgetData::BGColor_, color);
    SaveSettings();
  }
}

void MainWindow::on_pushButton_clicked() {
  ui_->openGLWidget->SetCenterModel();
}

void MainWindow::on_LoadTexture_button_clicked() {
  s21::OpenBMPCommand openTexture;
  QString bmpFilePath = commandInvoker_.runCommand(&openTexture);

  if (!bmpFilePath.isEmpty()) {
    ui_->openGLWidget->setMember(&s21::WidgetData::Texture_,
                                 QImage(bmpFilePath));
    const QImage &texture =
        ui_->openGLWidget->getMember(&s21::WidgetData::Texture_);

    if (texture.width() <= 1024 && texture.height() <= 1024 &&
        texture.width() == texture.height()) {
      ui_->statusbar->showMessage("Текстура загружена успешно!");
      ui_->openGLWidget->update();
      ui_->ClearTexture_button->setEnabled(true);
      ui_->SaveUV_buttonn->setEnabled(true);

    } else {
      ui_->statusbar->showMessage("Ошибка загрузки текстуры");
    }
  }
}

void MainWindow::on_ClearTexture_button_clicked() {
  ui_->openGLWidget->setMember(&s21::WidgetData::Texture_, QImage());
  ui_->openGLWidget->update();
  ui_->ClearTexture_button->setEnabled(false);
  ui_->SaveUV_buttonn->setEnabled(false);
}

void MainWindow::on_SaveUV_buttonn_clicked() {
  s21::SaveBMPCommand saveUV;
  QString bmpFilePath = commandInvoker_.runCommand(&saveUV);
  s21::MediaMaker mediaMaker;
  mediaMaker.SetMedia(makeUV, ui_);
  mediaMaker.MakeMedia(bmpFilePath);
}

void MainWindow::on_ViewType_box_currentIndexChanged(int index) {
  if (index == 0)
    ui_->LoadTexture_button->setEnabled(false);
  else
    ui_->LoadTexture_button->setEnabled(true);
}

void MainWindow::on_Light_button_clicked() {
ui_->openGLWidget->setMember(&s21::WidgetData::posLight_X, ui_->X_light_line->text().toFloat());
ui_->openGLWidget->setMember(&s21::WidgetData::posLight_Y, ui_->Y_light_line->text().toFloat());
ui_->openGLWidget->setMember(&s21::WidgetData::posLight_Z, ui_->Z_light_line->text().toFloat());
ui_->openGLWidget->setMember(&s21::WidgetData::light_R, ui_->R_line->text().toFloat());
ui_->openGLWidget->setMember(&s21::WidgetData::light_G, ui_->G_line->text().toFloat());
ui_->openGLWidget->setMember(&s21::WidgetData::light_B, ui_->B_line->text().toFloat());



ui_->openGLWidget->update();  
}
