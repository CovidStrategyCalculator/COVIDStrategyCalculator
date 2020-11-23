QT += testlib         #Include QtTest to use SignalSpy, QTest::mouseClick, etc

TARGET = Tests
TEMPLATE = app

CONFIG +=
       c++17 \
       cmdline \
       testcase    #Creates 'check' target in Makefile.
CONFIG -= debug_and_release

include(../GoogleTest/GoogleTest.pri)

INCLUDEPATH += ../../

HEADERS +=

SOURCES +=
        main.cpp \
