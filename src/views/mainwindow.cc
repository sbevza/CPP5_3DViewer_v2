#include "mainwindow.h"

MainWindow::MainWindow(s21::Controller *controller, QWidget *parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      settings_("School_21", "3D_Viewer_2.0"),
      controller_(controller) {
  ui_->setupUi(this);

  QIcon icon(":/icon.png");
  setWindowIcon(icon);
  setWindowTitle("3D_Viewer_2.0");

  LoadSettings();
  initChangeBoxes();
  initSliders();
  ui_->ClearTexture_button->setEnabled(false);

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
    ui_->openGLWidget->setMember(&s21::WidgetData::LineType, index);
  });

  initComboBox(ui_->Projections_box, [=](int index) {
    ui_->openGLWidget->setMember(&s21::WidgetData::ProjectionType, index);
  });
  initComboBox(ui_->edge_box, [=](int index) {
    ui_->openGLWidget->setMember(&s21::WidgetData::EDGEType, index);
  });
  initComboBox(ui_->ViewType_box, [=](int index) {
    ui_->openGLWidget->setMember(&s21::WidgetData::ViewType, index);
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
    ui_->openGLWidget->setMember(&s21::WidgetData::PosX, value / 4.0f);
  });

  initSlider(ui_->Slider_Y, ui_->Y_digit, [=](GLfloat value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::PosY, value);
  });
  initSlider(ui_->Slider_Z, ui_->Z_digit, [=](GLfloat value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::PosZ, value / 0.8f);
  });

  initSlider(ui_->Slider_X_Rotate, ui_->X_Rotate_digit, [=](GLfloat value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::RotX, value);
  });
  initSlider(ui_->Slider_Y_Rotate, ui_->Y_Rotate_digit, [=](GLfloat value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::RotY, value);
  });
  initSlider(ui_->Slider_Z_Rotate, ui_->Z_Rotate_digit, [=](GLfloat value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::RotZ, value);
  });
  initSlider(ui_->Slider_thickness, ui_->thickness_digit, [=](float value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::LineThick, value);
  });
  initSlider(ui_->Slider_edge, ui_->Edge_digit, [=](float value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::EDGEThick, value);
  });
  initSlider(ui_->Slider_size, ui_->Size, [=](float value) {
    ui_->openGLWidget->setMember(&s21::WidgetData::Scale, value);
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
          QString::number(ui_->openGLWidget->data.numVertices) +
          " Количество граней: " +
          QString::number(ui_->openGLWidget->data.numFaceNumVerts));
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

  ui_->openGLWidget->setMember(&s21::WidgetData::EDGEColor, QColor(Qt::red));
  ui_->openGLWidget->setMember(&s21::WidgetData::BGColor_, QColor(Qt::black));
  ui_->openGLWidget->setMember(&s21::WidgetData::LineColor, QColor(Qt::white));

  ui_->openGLWidget->update();

  SaveSettings();
}

void MainWindow::SaveSettings() {
  settings_.setValue("BGColor",
                     ui_->openGLWidget->getMember(&s21::WidgetData::BGColor_));
  settings_.setValue("EDGEColor",
                     ui_->openGLWidget->getMember(&s21::WidgetData::EDGEColor));
  settings_.setValue("EDGEThick",
                     ui_->openGLWidget->getMember(&s21::WidgetData::EDGEThick));
  settings_.setValue("EDGEType",
                     ui_->openGLWidget->getMember(&s21::WidgetData::EDGEType));
  settings_.setValue("LineColor",
                     ui_->openGLWidget->getMember(&s21::WidgetData::LineColor));
  settings_.setValue("LineThick",
                     ui_->openGLWidget->getMember(&s21::WidgetData::LineThick));
  settings_.setValue("LineType",
                     ui_->openGLWidget->getMember(&s21::WidgetData::LineType));
  settings_.setValue("ProjectionType", ui_->openGLWidget->getMember(
                                           &s21::WidgetData::ProjectionType));

  ui_->openGLWidget->update();
}

void MainWindow::LoadSettings() {
  ui_->Projections_box->setCurrentIndex(
      settings_.value("ProjectionType", 0).toInt());

  ui_->Slider_thickness->setValue(settings_.value("LineThick", 1.0).toInt());

  ui_->Slider_edge->setValue(settings_.value("EDGEThick", 1.0).toInt());

  ui_->type_line_box->setCurrentIndex(settings_.value("LineType", 0).toInt());

  ui_->edge_box->setCurrentIndex(settings_.value("EDGEType", 0).toInt());

  ui_->openGLWidget->setMember(
      &s21::WidgetData::BGColor_,
      settings_.value("BGColor", QColor(Qt::black)).value<QColor>());

  ui_->openGLWidget->setMember(
      &s21::WidgetData::LineColor,
      settings_.value("LineColor", QColor(Qt::white)).value<QColor>());
  ui_->openGLWidget->setMember(
      &s21::WidgetData::EDGEColor,
      settings_.value("EDGEColor", QColor(Qt::black)).value<QColor>());
  ui_->openGLWidget->setMember(&s21::WidgetData::ProjectionType,
                               settings_.value("ProjectionType", 0).toInt());
  ui_->openGLWidget->setMember(&s21::WidgetData::LineType,
                               settings_.value("LineType", 0).toInt());
  ui_->openGLWidget->setMember(&s21::WidgetData::EDGEType,
                               settings_.value("EDGEType", 0).toInt());
  ui_->openGLWidget->setMember(&s21::WidgetData::LineThick,
                               settings_.value("LineThick", 1.0).toFloat());
  ui_->thickness_digit->setValue(settings_.value("LineThick", 1).toInt());
  ui_->openGLWidget->setMember(&s21::WidgetData::EDGEThick,
                               settings_.value("EDGEThick", 1.0).toFloat());
  ui_->Edge_digit->setValue(settings_.value("EDGEThick", 1).toInt());

  ui_->openGLWidget->update();
  update();
}

void MainWindow::on_Vertex_color_clicked() {
  QColor color = QColorDialog::getColor(Qt::white, this, "Выберите цвет");
  if (color.isValid()) {
    ui_->openGLWidget->setMember(&s21::WidgetData::LineColor, color);
    SaveSettings();
  }
}

void MainWindow::on_color_edge_clicked() {
  QColor color = QColorDialog::getColor(Qt::white, this, "Выберите цвет");
  if (color.isValid()) {
    ui_->openGLWidget->setMember(&s21::WidgetData::EDGEColor, color);
    SaveSettings();
  }
}

void MainWindow::rotateModelOverTime(QString rotationAxis) {
  // GLfloat *rotationPtr = nullptr;

  // if (rotationAxis == "RotX") {
  //   rotationPtr = &ui_->openGLWidget->getMember(&s21::WidgetData::RotX);
  // } else if (rotationAxis == "RotY") {
  //   rotationPtr = &ui_->openGLWidget->getMember(&s21::WidgetData::RotY);
  // } else if (rotationAxis == "RotZ") {
  //   rotationPtr = &ui_->openGLWidget->getMember(&s21::WidgetData::RotZ);
  // }

  // GLfloat startAngle = *rotationPtr;
  // int timerInterval = 16;
  // QTimer *timer = new QTimer(this);
  // connect(timer, &QTimer::timeout, [=]() {
  //   ui_->openGLWidget->setMember(rotationPtr, += 1.0);
  //   ui_->openGLWidget->update();
  //   if (*rotationPtr == (startAngle + 360.0f)) {
  //     timer->stop();
  //   }
  // });
  // timer->start(timerInterval);
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
    ui_->openGLWidget->setMember(&s21::WidgetData::texture_,
                                 QImage(bmpFilePath));
    const QImage &texture =
        ui_->openGLWidget->getMember(&s21::WidgetData::texture_);

    if (texture.width() <= 1024 && texture.height() <= 1024 &&
        texture.width() == texture.height()) {
      ui_->statusbar->showMessage("Текстура загружена успешно!");
      ui_->openGLWidget->update();
      ui_->ClearTexture_button->setEnabled(true);
    } else {
      ui_->statusbar->showMessage("Ошибка загрузки текстуры");
    }
  }
}

void MainWindow::on_ClearTexture_button_clicked() {
  ui_->openGLWidget->setMember(&s21::WidgetData::texture_, QImage());
  ui_->openGLWidget->update();
  ui_->ClearTexture_button->setEnabled(false);
}

void MainWindow::on_SaveUV_buttonn_clicked() {
  s21::SaveBMPCommand saveUV;
  QString bmpFilePath = commandInvoker_.runCommand(&saveUV);
  s21::MediaMaker mediaMaker;
  mediaMaker.SetMedia(makeUV, ui_);
  mediaMaker.MakeMedia(bmpFilePath);
}
