#include "patterns/application_facade.h"

int main(int argc, char *argv[]) {
  s21::ApplicationFacade appFacade(argc, argv);
  appFacade.run();
  return QApplication::exec();
}
