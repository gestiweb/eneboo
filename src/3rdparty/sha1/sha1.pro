AQMODULE = AQCore

include(../../../settings.pri)

TEMPLATE = lib
TARGET = AQsha1
VERSION = 1.0

CONFIG += create_prl staticlib
CONFIG -= dll

QT -= gui

DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
INSTALLS += target

HEADERS = AQSha1.h
SOURCES = AQSha1.cpp

include(../../../includes.pri)

linux-g++:QMAKE_CXXFLAGS = -pthread -malign-double -pipe -O3 -finline-limit=20000 -fomit-frame-pointer -finline-functions
linux-g++-64:QMAKE_CXXFLAGS = -pthread -pipe -O3 -finline-limit=20000 -fomit-frame-pointer -finline-functions
win32:QMAKE_CXXFLAGS = -malign-double -pipe -O3 -finline-limit=20000 -fomit-frame-pointer -finline-functions
