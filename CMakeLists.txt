cmake_minimum_required(VERSION 3.23)
project(CPP5_3DViewer_v2.1-1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets OpenGLWidgets REQUIRED)
find_package(GTest REQUIRED)

include_directories(src)
include_directories(src/views)

add_executable(CPP5_3DViewer_v2.1-1
        src/models/parse.cc
        src/models/parse.h
        src/views/mainwindow.cc
        src/views/mainwindow.h
        src/views/mainwindow.ui
        src/models/glwidget.h
        src/models/glwidget.cc
        src/main.cc
        src/controllers/controller.cc
        src/3dViewer.pro
        src/patterns/application_facade.cc
        src/patterns/application_facade.h
        src/patterns/application_comands.cc
        src/patterns/application_strategy.cc
        src/tests/test.cc
)

target_link_libraries(CPP5_3DViewer_v2.1-1 m Qt6::Core Qt6::Widgets Qt6::OpenGLWidgets gtest gtest_main)
