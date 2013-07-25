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
enable_win64:DEFINES += _UWIN

DESTDIR = $$PREFIX/lib

DEFINES += HAVE_CONFIG_H 

mac {
	DEFINES += _REENTRANT
}

INCLUDEPATH += $$ROOT/src/libdigidoc/libxml2 $$ROOT/src/libdigidoc/libxml2/include $$ROOT/src/libdigidoc/libxml2/libxml
INCLUDEPATH += $$ROOT/src/libxslt
QMAKE_LIBS_X11SM =

LIBS *= -L$$PREFIX/lib -lxml2

TARGET = xslt

VERSION = 1.1.26

HEADERS += attributes.h \
           config.h \
           documents.h \
           extensions.h \
           extra.h \
           functions.h \
           imports.h \
           keys.h \
           libxslt.h \
           namespaces.h \
           numbersInternals.h \
           pattern.h \
           preproc.h \
           security.h \
           templates.h \
           transform.h \
           trio.h \
           triodef.h \
           variables.h \
           win32config.h \
           xslt.h \
           xsltconfig.h \
           xsltexports.h \
           xsltInternals.h \
           xsltlocale.h \
           xsltutils.h \
           xsltwin32config.h
SOURCES += attributes.c \
           attrvt.c \
           documents.c \
           extensions.c \
           extra.c \
           imports.c \
           keys.c \
           namespaces.c \
           numbers.c \
           preproc.c \
           security.c \
           templates.c \
           transform.c \
           variables.c \
           xslt.c \
           xsltfunctions.c \
           xsltlocale.c \
           xsltpattern.c \
           xsltutils.c

mac:QMAKE_CFLAGS += -fno-common
mac:QMAKE_CXXFLAGS += -fno-common
