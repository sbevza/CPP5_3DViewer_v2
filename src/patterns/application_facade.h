//
// Created by sbevza on 12/13/23.
//

#ifndef CPP4_3DVIEWER_V2_0_SRC_PATTERNS_APPLICATION_FACADE_H_
#define CPP4_3DVIEWER_V2_0_SRC_PATTERNS_APPLICATION_FACADE_H_

#include "views/mainwindow.h"

namespace s21 {

class ApplicationFacade {
 public:
  explicit ApplicationFacade(int argc, char *argv[]);
  void run();

 private:
  QApplication app_;
  s21::Parser parser_;
  s21::Controller controller_;
  MainWindow mainWindow_;
};

}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_SRC_PATTERNS_APPLICATION_FACADE_H_
