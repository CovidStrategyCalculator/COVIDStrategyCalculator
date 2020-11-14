QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CovidStrategyCalculator
TEMPLATE = app

INCLUDEPATH += ../../eigen

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        simulation.cpp

HEADERS += \
        mainwindow.h \
        simulation.h

RESOURCES     = CovidStrategyCalculator.qrc

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13

VERSION  = 1.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
