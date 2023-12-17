#ifndef CPP4_3DVIEWER_V2_0_SRC_PATTERNS_APPLICATION_COMANDS_H_
#define CPP4_3DVIEWER_V2_0_SRC_PATTERNS_APPLICATION_COMANDS_H_

#include <QFileDialog>
#include <QRegularExpression>

namespace s21 {

// Интерфейс команды
class PathCommand {
 public:
  virtual ~PathCommand() {}
  virtual QString execute() = 0;
};

// Команда для открытия файла модели
class OpenCommand : public PathCommand {
 public:
  QString execute() override;
};

// Команда для получения пути сохранения скриншота
class SaveScreenshotCommand : public PathCommand {
 public:
  QString execute() override;
};

// Команда для получения пути сохранения GIF
class SaveGifCommand : public PathCommand {
 public:
  QString execute() override;
};

// Инвокер
class CommandInvoker {
 public:
  QString runCommand(PathCommand* command);
};

}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_SRC_PATTERNS_APPLICATION_COMANDS_H_
