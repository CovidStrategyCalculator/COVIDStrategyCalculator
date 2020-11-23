QT += testlib         #Include QtTest to use SignalSpy, QTest::mouseClick, etc

TARGET = Tests
TEMPLATE = app

CONFIG += c++14
CONFIG += testcase    #Creates 'check' target in Makefile.
CONFIG -= debug_and_release
CONFIG += cmdline

INCLUDEPATH += ../MyCoolLibs/MyLib1 \
               ../MyCoolLibs/MyLib2

LIBS += -L../MyCoolLibs/MyLib1 -lMyLib1 \
        -L../MyCoolLibs/MyLib2 -lMyLib2

include(../GoogleTest/GoogleTest.pri)

HEADERS +=


SOURCES += main.cpp \
           tst_mylib1.cpp \
           tst_mylib2.cpp
