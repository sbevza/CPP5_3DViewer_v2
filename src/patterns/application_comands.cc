#include "application_comands.h"

namespace s21 {

QString OpenCommand::execute() {
  return QFileDialog::getOpenFileName(nullptr, "Открыть модель", "",
                                      "Файлы моделей (*.obj)");
}

QString SaveScreenshotCommand::execute() {
  QFileDialog dialog(nullptr, "Save Screenshot");
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setFileMode(QFileDialog::AnyFile);

  QStringList fileFormats;
  fileFormats << "JPEG (*.jpg)"
              << "Bitmap (*.bmp)";
  dialog.setNameFilters(fileFormats);
  QString filePath = "";

  if (dialog.exec() == QDialog::Accepted) {
    QString selectedFilter = dialog.selectedNameFilter();
    filePath = dialog.selectedFiles().first();

    // Извлекаем расширение из выбранного фильтра
    QRegularExpression filterRegex("\\(\\*\\.(\\w+)\\)");
    QRegularExpressionMatch match = filterRegex.match(selectedFilter);
    if (match.hasMatch()) {
      QString extension = match.captured(1);

      // Добавляем расширение к пути файла
      if (!extension.isEmpty() &&
          !filePath.endsWith(extension, Qt::CaseInsensitive)) {
        filePath += "." + extension;
      }
    }
  }
  return filePath;
}

QString SaveGifCommand::execute() {
  QFileDialog dialog(nullptr, "Save GIF");
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setFileMode(QFileDialog::AnyFile);

  QStringList fileFormats;
  fileFormats << "GIF (*.gif)";
  dialog.setNameFilters(fileFormats);
  QString filePath = "";

  if (dialog.exec() == QDialog::Accepted) {
    QString selectedFilter = dialog.selectedNameFilter();
    filePath = dialog.selectedFiles().first();

    // Извлекаем расширение из выбранного фильтра
    QRegularExpression filterRegex("\\(\\*\\.(\\w+)\\)");
    QRegularExpressionMatch match = filterRegex.match(selectedFilter);
    if (match.hasMatch()) {
      QString extension = match.captured(1);

      // Добавляем расширение к пути файла
      if (!extension.isEmpty() &&
          !filePath.endsWith(extension, Qt::CaseInsensitive)) {
        filePath += "." + extension;
      }
    }
  }
  return filePath;
}

QString CommandInvoker::runCommand(PathCommand* command) {
  return command->execute();
}

}  // namespace s21
