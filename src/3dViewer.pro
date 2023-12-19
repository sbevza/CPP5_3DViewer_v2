QT += core gui openglwidgets

#DEFINES += GL_SILENCE_DEPRECATION

CONFIG += c++17

SOURCES += \
    main.cc \ 
    models/parse.cc \
    models/glwidget.cc \
    views/mainwindow.cc \
    controllers/controller.cc \
    patterns/application_facade.cc \
    patterns/application_comands.cc \
    patterns/application_strategy.cc 

    
HEADERS += \
    views/gif.h \
    models/parse.h \
    models/glwidget.h \
    views/mainwindow.h \
    controllers/controller.h \
    patterns/application_facade.h \
    patterns/application_comands.h \
    patterns/application_strategy.h 


FORMS += \
    views/mainwindow.ui

ICON += views/icon.png
