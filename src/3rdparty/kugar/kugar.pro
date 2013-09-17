AQMODULE = AQkugar

HEADERS = AQReportEngine.h
SOURCES = AQReportEngine.cpp
include(../../../settings.pri)
TEMPLATE = lib
TARGET = AQkugar
VERSION = 1.0
CONFIG += release dll create_prl
QT -= gui
QT += xml
DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
INSTALLS += target
include(../../../includes.pri)
