#include "controller.h"

namespace s21 {

Controller::Controller(s21::Parser &parser) : parser_(parser) {}

void Controller::parse_obj(s21::Attrib &data, std::string &filename) {
  parser_.parseObj(data, filename);
}

bool Controller::hasError() { return parser_.hasError(); }

}  // namespace s21