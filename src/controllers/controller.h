#ifndef CPP4_3DVIEWER_V2_0_SRC_CONTROLLERS_CONTROLLER_H_
#define CPP4_3DVIEWER_V2_0_SRC_CONTROLLERS_CONTROLLER_H_

#include "models/glwidget.h"

namespace s21 {
class Controller {
 public:
  explicit Controller(s21::Parser &parser);
  void parse_obj(s21::Attrib &data, std::string &objFilenameArray);
  bool hasError();

 private:
  s21::Parser &parser_;
};
}  // namespace s21
#endif  // CPP_3DVIEWER_V2_0_SRC_CONTROLLERS_CONTROLLER_H_
