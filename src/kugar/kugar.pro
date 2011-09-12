exists(../../settings.pro) {
	include(../../settings.pro)
}
else {
	error("settings.pro missing, unable to build")
}

TEMPLATE = lib

CONFIG += warn_off createprl staticlib qt
CONFIG -= dll
unix:CONFIG += x11

INCLUDEPATH += $$ROOT $$ROOT/src/kugar $$ROOT/src/flbase $$ROOT/src/flmail $$ROOT/src/serialport
DEPENDPATH += $$ROOT/src/flbase

LIBS += -L$$PREFIX/lib

DESTDIR = $$PREFIX/lib

TARGET = kugar

VERSION = 1.0

INCLUDEPATH += $$ROOT/src/posprinter
include(../posprinter/posprinter.pri)

win32 {
	INCLUDEPATH += $$ROOT/src/psprinter
	include(../psprinter/psprinter.pri)
}

mac {
	INCLUDEPATH += $$ROOT/src/psprinter
	include(../psprinter/psprinter.pri)
}

SOURCES += mcalcobject.cpp \
           mfieldobject.cpp \
           mlabelobject.cpp \
           mlineobject.cpp \
           mpagecollection.cpp \
           mpagedisplay.cpp \
           mreportdetail.cpp \
           mreportengine.cpp \
           mreportobject.cpp \
           mreportsection.cpp \
           mreportviewer.cpp \
           mspecialobject.cpp \
           mutil.cpp

HEADERS += kugar.h \
           mcalcobject.h \
           mfieldobject.h \
           mlabelobject.h \
           mlineobject.h \
           mpagecollection.h \
           mpagedisplay.h \
           mreportdetail.h \
           mreportengine.h \
           mreportobject.h \
           mreportsection.h \
           mreportviewer.h \
           mspecialobject.h \
           mutil.h

unix {
        QMAKE_CFLAGS += $$QMAKE_CFLAGS_SHLIB
        QMAKE_CXXFLAGS += $$QMAKE_CXXFLAGS_SHLIB
}
