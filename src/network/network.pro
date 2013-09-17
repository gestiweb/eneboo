AQMODULE = AQNetwork

HEADERS = AQSmtpClient.h
SOURCES = AQSmtpClient.cpp
include (../../settings.pri)
TEMPLATE = lib
TARGET = AQNetwork
VERSION = 3.0
CONFIG += dll create_prl
DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
QT -= gui
INSTALLS += target
include(../../includes.pri)
