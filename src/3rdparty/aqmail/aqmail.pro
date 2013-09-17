HEADERS = 
SOURCES = 
include(../../../settings.pri)
TEMPLATE = lib
TARGET = aqmail
VERSION = 1.0
CONFIG += dll create_prl link_prl
QT -= gui
DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
INSTALLS += target
include(../../../includes.pri)
