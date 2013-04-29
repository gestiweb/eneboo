PRJ_MODULE      = SQLite
exists(../../settings.pro) {
	include(../../settings.pro)
}
else {
	error("settings.pro missing, unable to build")
}
TEMPLATE = lib

CONFIG += warn_off create_prl link_prl
!mac:CONFIG += plugin

win32 {
	CONFIG += dll exceptions
	DLLDESTDIR = $$PREFIX/bin
	DEFINES += OS_WIN=1
}

unix {
	CONFIG -= x11
	DEFINES += OS_UNIX=1
}

DESTDIR = $$PREFIX/lib

TARGET = sqlite

LIBS += -L$$PREFIX/lib

INCLUDEPATH += $$ROOT/src/qt/include ./

VERSION = 3.7.16.2

SOURCES += sqlite3.c \
	   shell.c \
	   opcodes.c \         
           dataset.cpp qry_dat.cpp sqlitedataset.cpp

HEADERS += sqlite3ext.h \
           sqlite3.h \
           opcodes.h \
           config_sqlite.h \
	   dataset.h qry_dat.h sqlitedataset.h
