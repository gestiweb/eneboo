AQMODULE = AQXml

HEADERS = AQXbelReader.h \
    AQXbelWriter.h
SOURCES = AQXbelReader.cpp \
    AQXbelWriter.cpp
include (../../settings.pri)
TEMPLATE = lib
TARGET = AQXml
VERSION = 3.0
CONFIG += dll \
    create_prl
QT += xml sql
DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
INSTALLS += target
include(../../includes.pri)
