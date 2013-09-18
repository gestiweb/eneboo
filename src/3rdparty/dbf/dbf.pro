AQMODULE = dbf

include(../../../settings.pri)

TEMPLATE = app
TARGET = dbf

DESTDIR = $$ABANQBLDDIR/bin
target.path = $$ABANQINSDIR/bin
INSTALLS += target

QT -= gui core

CONFIG -= app_bundle
CONFIG += warn_off

win32 {
	CONFIG += console
	RC_FILE = dbf.rc
}

SOURCES += an_string.c \
           csv.c \
           statistic.c \
           congraph.c \
           iodbf.c \
           sql.c \
           codepages.c \
           dbf.c \
           dbf_endian.c

HEADERS += an_string.h \
           codepages.h \
           congraph.h \
           csv.h \
           dbf.h \
           dbf_endian.h \
           iodbf.h \
           sql.h \
           statistic.h \
           tables.h
