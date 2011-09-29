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

VERSION = 2.8.6

SOURCES += attach.c \
           auth.c \
           btree.c \
           btree_rb.c \
           build.c \
           copy.c \
	     date.c \
           delete.c \
           expr.c \
           func.c \
           hash.c \
           insert.c \
           main.c \
           opcodes.c \
           os.c \
           pager.c \
           parse.c \
           pragma.c \
           printf.c \
           random.c \
           select.c \
           shell.c \
           table.c \
           tokenize.c \
           trigger.c \
           update.c \
           util.c \
           vacuum.c \
           vdbe.c \
	   vdbeaux.c \
           where.c \
           dataset.cpp qry_dat.cpp  sqlitedataset.cpp

HEADERS += btree.h \
           config_sqlite.h \
           hash.h \
           opcodes.h \
           os.h \
           pager.h \
           parse.h \
           sqlite.h \
           sqliteInt.h \
           vdbe.h \
	   vdbeInt.h \
	   dataset.h  qry_dat.h  sqlitedataset.h
