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

LIBS += -L$$PREFIX/lib -lxslt -lexslt -lxml2

TARGET = xsltproc

VERSION = 1.0.0

SOURCES += xsltproc.c

mac:QMAKE_CFLAGS += -fno-common
mac:QMAKE_CXXFLAGS += -fno-common