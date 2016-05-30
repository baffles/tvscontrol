#include(../include/QtSolutions/qtservice/src/qtservice.pri)

CONFIG  += debug
#CONFIG  += release
QT      += network 

TEMPLATE = app
CONFIG  += console

SOURCES += main.cpp tvscontrol.cpp
HEADERS += tvscontrol.h

unix:LIBS += -lqatemcontrol -lqxkey24

TARGET   = tvscontrol

isEmpty(PREFIX) {
 PREFIX = /usr/bin
}
inst.files = tvscontrol
inst.path  = $$PREFIX/

INSTALLS += inst
