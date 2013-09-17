AQMODULE = libhoard

include(../../../settings.pri)

TEMPLATE = lib
TARGET = hoard
VERSION = 3.8.0

QT -= gui core
CONFIG -= qt debug

DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
INSTALLS += target
DEFINES +=  NDEBUG _REENTRANT=1
CONFIG += warn_off release

LIBS = -ldl -lpthread

INCLUDEPATH += /usr/include/nptl ./src ./src/heaplayers ./src/heaplayers/util

SOURCES = ./src/gnuwrapper.cpp ./src/libhoard.cpp

linux-g++:QMAKE_CXXFLAGS = -pthread -malign-double -pipe -O3 -finline-limit=20000 -fomit-frame-pointer -finline-functions
linux-g++-64:QMAKE_CXXFLAGS = -pthread -pipe -O3 -finline-limit=20000 -fomit-frame-pointer -finline-functions
win32:QMAKE_CXXFLAGS = -malign-double -pipe -O3 -finline-limit=20000 -fomit-frame-pointer -finline-functions
QMAKE_LFLAGS += -Bsymbolic