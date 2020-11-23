QT += testlib         #Include QtTest to use SignalSpy, QTest::mouseClick, etc

TARGET = Tests
TEMPLATE = app

CONFIG += \
       c++17 \
       cmdline \
       testcase
CONFIG -= debug_and_release

include(../GoogleTest/GoogleTest.pri)

INCLUDEPATH += \
            ../../CovidStrategyCalculator \
            ../../CovidStrategyCalculator/submodules/eigen

HEADERS +=

SOURCES += \
        core/base_model_test.cpp
