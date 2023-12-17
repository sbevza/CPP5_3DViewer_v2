#include "application_facade.h"

namespace s21 {

ApplicationFacade::ApplicationFacade(int argc, char *argv[])
    : app_(argc, argv),
      parser_(),
      controller_(parser_),
      mainWindow_(&controller_) {}

void ApplicationFacade::run() { mainWindow_.show(); }

}  // namespace s21
