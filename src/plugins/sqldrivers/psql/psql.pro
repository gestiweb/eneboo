AQMODULE = AQPSQLPlugin

HEADERS += ../../../sql/drivers/psql/AQSqlPSQL.h
SOURCES += ../../../sql/drivers/psql/AQSqlPSQL.cpp main.cpp

include (../../../../settings.pri)

TEMPLATE = lib

CONFIG += plugin
CONFIG += hide_symbols
TARGET = AQPSQLPlugin
VERSION = 3.0

QT += sql xml
QT -= gui
LIBS += -laqpq

DLLDESTDIR = $$ABANQBLDDIR/plugins/sqldrivers
DESTDIR = $$ABANQBLDDIR/plugins/sqldrivers
target.path = $$ABANQINSDIR/plugins/sqldrivers
INSTALLS += target

include(../../../../includes.pri)

