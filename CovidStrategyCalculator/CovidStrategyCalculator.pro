QT += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CovidStrategyCalculator
TEMPLATE = app

CONFIG += c++17
QMAKE_CXXFLAGS += "-Wno-deprecated-copy"

INCLUDEPATH += submodules/eigen

HEADERS += \
        include/core/base_model.h \
        include/core/model.h \
        include/core/prevalence_estimator.h \
        include/core/simulation.h \
        include/gui/efficacy_table.h \
        include/gui/main_window.h \
        include/gui/plot_area.h \
        include/gui/result_log.h \
        include/gui/user_input/input_container.h \
        include/gui/user_input/parameters_tab.h \
        include/gui/user_input/prevalence_tab.h \
        include/gui/user_input/strategy_tab.h \
        include/gui/utils.h

SOURCES += \
        main.cpp \
        src/core/base_model.cpp \
        src/core/model.cpp \
        src/core/prevalence_estimator.cpp \
        src/core/simulation.cpp \
        src/gui/efficacy_table.cpp \
        src/gui/main_window.cpp \
        src/gui/plot_area.cpp \
        src/gui/result_log.cpp \
        src/gui/user_input/input_container.cpp \
        src/gui/user_input/parameters_tab.cpp \
        src/gui/user_input/prevalence_tab.cpp \
        src/gui/user_input/strategy_tab.cpp \
        src/gui/utils.cpp

RESOURCES = CovidStrategyCalculator.qrc

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13

VERSION = 2.0
DEFINES += APP_VERSION=\\"$$VERSION\\"
