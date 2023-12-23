#include "application_comands.h"

namespace s21 {

QString OpenCommand::execute() {
  return QFileDialog::getOpenFileName(nullptr, "Открыть модель", "",
                                      "Файлы моделей (*.obj)");
}

QString OpenBMPCommand::execute() {
    return QFileDialog::getOpenFileName(nullptr, "Открыть изображение BMP", "",
                                        "Файлы изображений BMP (*.bmp)");
}

QString saveFileWithExtension(const QString &dialogTitle, const QString &defaultExtension, const QStringList &fileFormats) {
    QFileDialog dialog(nullptr, dialogTitle);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDefaultSuffix(defaultExtension);
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

            // Добавляем расширение к пути файла, если оно отличается от выбранного фильтра
            if (!extension.isEmpty() &&
                !filePath.endsWith(extension, Qt::CaseInsensitive)) {
                filePath += "." + extension;
            }
        }
    }

    return filePath;
}

QString SaveScreenshotCommand::execute() {
    QStringList fileFormats = { "JPEG (*.jpg)", "Bitmap (*.bmp)" };
    return saveFileWithExtension("Save Screenshot", "bmp", fileFormats);
}

QString SaveGifCommand::execute() {
    QStringList fileFormats = { "GIF (*.gif)" };
    return saveFileWithExtension("Save GIF", "gif", fileFormats);
}

QString SaveBMPCommand::execute() {
    QStringList fileFormats = { "Bitmap (*.bmp)" };
    return saveFileWithExtension("Save UVmap", "bmp", fileFormats);
}

QString CommandInvoker::runCommand(PathCommand* command) {
  return command->execute();
}

}  // namespace s21
