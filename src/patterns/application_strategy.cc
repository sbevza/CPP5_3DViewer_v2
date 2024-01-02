#include "application_strategy.h"

namespace s21 {

void StrategyImage::make(QString filename) {
  s21::glWidget *widget = ui_->openGLWidget;

  QRect widgetGeometry = widget->geometry();

  QImage screenshot(widgetGeometry.size(), QImage::Format_RGBA8888);

  QPixmap pixmap(widgetGeometry.size());

  QPainter painter(&pixmap);
  widget->render(&painter);

  screenshot = pixmap.toImage();
  screenshot.save(filename);
  if (QFile::exists(filename)) {
    ui_->statusbar->showMessage("ScreenShot сохранен: " + filename);
  } else {
    ui_->statusbar->showMessage(
        "Ошибка создания файла в данной дериктории, попробуйте в другую!");
  }
}

void StrategyGif::make(QString filename) {
  Gif gif;
  QTime timer;
  Gif::GifWriter gifWriter = {};
  int targetWidth = 640;
  int targetHeight = 480;
  // Извлекаем путь к папке без названия файла и расширения
  QFileInfo fileInfo(filename);
  QDir dir = fileInfo.absoluteDir();
  QString gifFolderPath = dir.absolutePath() + "/gif/";

  QDir().mkpath(gifFolderPath);

  gif.GifBegin(&gifWriter, filename.toStdString().c_str(), targetWidth,
               targetHeight, 100);

  for (short int i = 0; i < 50; ++i) {
    QPixmap pixmap(ui_->openGLWidget->size());
    ui_->openGLWidget->render(&pixmap, QPoint(),
                              QRegion(ui_->openGLWidget->rect()));
    pixmap = pixmap.scaled(targetWidth, targetHeight, Qt::IgnoreAspectRatio);

    QImage image = pixmap.toImage();
    gif.GifWriteFrame(&gifWriter, image.bits(), targetWidth, targetHeight, 0);
    timer = QTime::currentTime().addMSecs(100);
    while (QTime::currentTime() < timer) {
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
  }
  gif.GifEnd(&gifWriter);
  if (QFile::exists(filename)) {
    ui_->statusbar->showMessage("Gif создана и лежит в папке:" + filename);
    QDir imageDir(gifFolderPath);
    imageDir.removeRecursively();
  } else {
    ui_->statusbar->showMessage(
        "Ошибка создания файла в данной дериктории, попробуйте в другую!");
  }
}

void StrategyUV::make(QString filename) {
  QImage tex = ui_->openGLWidget->getMember(&s21::WidgetData::Texture_);
  QPainter painter(&tex);
  painter.setPen(QPen(ui_->openGLWidget->getMember(&s21::WidgetData::LineColor_), 1, Qt::SolidLine));

  const std::vector<float> &vec = ui_->openGLWidget->data.vertexTextureShade;

  for (size_t i = 0; i < vec.size(); i += 6) {
    float u1 = vec[i], v1 = vec[i + 1];
    float u2 = vec[i + 2], v2 = vec[i + 3];
    float u3 = vec[i + 4], v3 = vec[i + 5];

    painter.drawLine(tex.width() * u1, tex.height() * v1,
                     tex.width() * u2, tex.height() * v2);
    painter.drawLine(tex.width() * u2, tex.height() * v2,
                     tex.width() * u3, tex.height() * v3);
    painter.drawLine(tex.width() * u3, tex.height() * v3,
                     tex.width() * u1, tex.height() * v1);
  }

  if (!filename.isEmpty() && tex.save(filename)) {
    ui_->statusbar->showMessage("UV создана и лежит в папке:" + filename);
  } else {
    ui_->statusbar->showMessage("Ошибка при сохранении UV карты.");
  }
}



MediaMaker::~MediaMaker() { delete media_; }

void MediaMaker::MakeMedia(QString filename) {
  if (media_) media_->make(filename);
}

void MediaMaker::SetMedia(strategy strategyType, Ui::MainWindow *ui) {
  if (strategyType == makeImage) {
    media_ = new StrategyImage(ui);
  } else if (strategyType == makeGif) {
    media_ = new StrategyGif(ui);
  } else if (strategyType == makeUV) {
    media_ = new StrategyUV(ui);
  }
}

}  // namespace s21
