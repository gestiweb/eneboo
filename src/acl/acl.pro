AQMODULE = AQAcl
HEADERS = ../corelib/acl/AQAccessControlTable.h \
    AQAccessControl_p.h \
    AQAccessControl.h \
    AQAccessControlFactory.h \
    AQAccessControlLists.h
SOURCES = ../corelib/acl/AQAccessControlTable.cpp \
    AQAccessControl.cpp \
    AQAccessControlFactory.cpp \
    AQAccessControlLists.cpp
include (../../settings.pri)
TEMPLATE = lib
TARGET = AQAcl
VERSION = 3.0
CONFIG += dll \
    create_prl
CONFIG -= no_keywords
QT -= gui
QT += xml
DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
INSTALLS += target
include(../../includes.pri)

