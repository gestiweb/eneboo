exists(../../settings.pro) {
	include(../../settings.pro)
}
else {
        error("settings.pro missing, unable to build")
}

TEMPLATE = app

CONFIG += warn_off console
CONFIG -= dll

DESTDIR = $$PREFIX/bin

win32:RC_FILE = dbf.rc

TARGET = dbf

SOURCES += an_string.c \
           csv.c \
           statistic.c \
           congraph.c \
           iodbf.c \
           sql.c \
           codepages.c \
           dbf.c \
           endian.c

HEADERS += an_string.h \
           codepages.h \
           congraph.h \
           csv.h \
           dbf.h \
           endian.h \
           iodbf.h \
           sql.h \
           statistic.h \
           tables.h
