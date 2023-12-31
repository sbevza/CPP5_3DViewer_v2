#ifndef CPP4_3DVIEWER_V2_0_SRC_PATTERNS_STRATEGY_H_
#define CPP4_3DVIEWER_V2_0_SRC_PATTERNS_STRATEGY_H_

#include "../views/gif.h"
#include "views/mainwindow.h"
#include "../models/parse.h"


enum strategy { makeImage = 0, makeGif, makeUV };

namespace s21 {

class Strategy {
 public:
  virtual ~Strategy() {}
  virtual void make(QString filename) = 0;
};

class StrategyImage : public Strategy {
 public:
  StrategyImage(Ui::MainWindow *ui) : ui_(ui) {}

  void make(QString filename);

 private:
  Ui::MainWindow *ui_;
};

class StrategyGif : public Strategy {
 public:
  StrategyGif(Ui::MainWindow *ui) : ui_(ui) {}

  void make(QString filename);

 private:
  Ui::MainWindow *ui_;
};

class StrategyUV : public Strategy {
 public:
  StrategyUV(Ui::MainWindow *ui) : ui_(ui) {}

  void make(QString filename);

 private:
  Ui::MainWindow *ui_;


};

class MediaMaker {
 public:
  MediaMaker() : media_(nullptr) {}
  ~MediaMaker();

  void MakeMedia(QString filename);

  void SetMedia(strategy strategyType, Ui::MainWindow *ui);

 private:
  Strategy *media_;
};
}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_SRC_PATTERNS_STRATEGY_H_
