AQMODULE = AQUiPlugins

SOURCES += aq3mainwindow/AQ3MainWindowPlugin.cpp \
    aqtabledb/AQTableDBPlugin.cpp \
    aqfielddb/AQFieldDBPlugin.cpp \
    AQUiPlugins.cpp
HEADERS += aq3mainwindow/AQ3MainWindowPlugin.h \
    aqtabledb/AQTableDBPlugin.h \
    aqfielddb/AQFieldDBPlugin.h
DEPENDPATH += aqfielddb \
    aqtabledb
include (../../../settings.pri)
TEMPLATE = lib
CONFIG += plugin
CONFIG += qt \
    warn_on \
    qt_no_compat_warning
build_all:!build_pass { 
    CONFIG -= build_all
    CONFIG += release
}
TARGET = AQUiPlugins
VERSION = 3.0
LIBS += -lAQUi
QT += qt3support
DLLDESTDIR = $$ABANQBLDDIR/plugins/designer
DESTDIR = $$ABANQBLDDIR/plugins/designer
target.path = $$ABANQINSDIR/plugins/designer
INSTALLS += target
include(../../../includes.pri)
