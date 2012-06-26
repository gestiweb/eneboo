PRJ_MODULE      = LibXSLT

exists(../../../settings.pro) {
	include(../../../settings.pro)
}
else {
	error("settings.pro missing, unable to build")
}

TEMPLATE = lib
CONFIG += warn_off
!mac:CONFIG += plugin
CONFIG -= x11 qt

win32 {
	CONFIG += dll
	DLLDESTDIR = $$PREFIX/bin
	LIBS = -L$$PREFIX/lib -lm
}

DESTDIR = $$PREFIX/lib

DEFINES += HAVE_CONFIG_H 

mac {
	DEFINES += _REENTRANT
	LIBS += -ldl
}

INCLUDEPATH += $$ROOT/src/libdigidoc/libxml2 $$ROOT/src/libdigidoc/libxml2/include $$ROOT/src/libdigidoc/libxml2/libxml
INCLUDEPATH += $$ROOT/src/libxslt
QMAKE_LIBS_X11SM =

LIBS += -L$$PREFIX/lib -lxslt -lxml2

TARGET = exslt

VERSION = 0.8.15

HEADERS += config.h exslt.h exsltconfig.h exsltexports.h libexslt.h
SOURCES += common.c \
           crypto.c \
           date.c \
           dynamic.c \
           exslt.c \
           functions.c \
           math.c \
           saxon.c \
           sets.c \
           strings.c
           
mac:QMAKE_CFLAGS += -fno-common
mac:QMAKE_CXXFLAGS += -fno-common
