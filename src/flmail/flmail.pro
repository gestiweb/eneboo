exists(../../settings.pro) {
	include(../../settings.pro)
}
else {
	error("settings.pro missing, unable to build")
}

TEMPLATE = lib

CONFIG += warn_off createprl linkprl qt
!mac:CONFIG += plugin
unix:CONFIG += x11

win32 {
        CONFIG += dll
        DLLDESTDIR = $$PREFIX/bin
}
DESTDIR = $$PREFIX/lib

INCLUDEPATH += $$ROOT .

LIBS += -L$$PREFIX/lib

TARGET = flmail

VERSION = 1.0.0

HEADERS += addressclass.h \
           addresslistclass.h \
           charsets.h \
           dateclass.h \
           headerclass.h \
           indexclass.h \
           mailfolder.h \
           md5.h \
           messageclass.h \
           messagedescriptor.h \
           messagedevice.h \
           messagefactory.h \
           mimecodec.h \
           mimepart.h \
           smtp.h \
           localmailfolder.h
SOURCES += addressclass.cpp \
           addresslistclass.cpp \
           charsets.cpp \
           dateclass.cpp \
           headerclass.cpp \
           indexclass.cpp \
           mailfolder.cpp \
           md5.c \
           messageclass.cpp \
           messagedescriptor.cpp \
           messagedevice.cpp \
           messagefactory.cpp \
           mimecodec.cpp \
           mimepart.cpp \
           smtp.cpp \
           localmailfolder.cpp

mac:SOURCES += $$ROOT/AQConfig.cpp
win32:SOURCES += $$ROOT/AQConfig.cpp
