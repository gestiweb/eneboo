PRJ_MODULE      = FLFCgi
exists(../../settings.pro) {
	include(../../settings.pro)
}
else {
	error("settings.pro missing, unable to build")
}
TEMPLATE = app 

CONFIG += warn_off link_prl qt 
unix:CONFIG += x11
CONFIG -= dll

win32:RC_FILE = fllite.rc
mac:RC_FILE = fl.icns

DESTDIR = $$PREFIX/bin

TARGET = eneboo-fcgi


INCLUDEPATH += $$ROOT/src/qt/src/kernel
INCLUDEPATH += $$ROOT $$ROOT/src/flbase
DEPENDPATH += $$ROOT/src/flbase

QT -= gui
LIBS *= -L$$PREFIX/lib
enable_hoard:LIBS *= -lhoard
enable_hoard:!win32:LIBS *= -L$$PREFIX/lib -ldl
LIBS *= -L$$PREFIX/lib -lflbase -lfcgi
mac:LIBS *= -ladvance -lqui -lflmail -lqwt
enable_digidoc:mac:LIBS *= -llibdigidoc -lcrypto -lssl -lxml2

load(qsa)
HEADERS += fcgiobjectfactory.h
SOURCES += main.cpp


win32:QMAKE_LFLAGS += -mwindows

win32 { 
    INCLUDEPATH += $$ROOT/src/qsa/src/qsa
    INCLUDEPATH += $$ROOT/src/qsa/src/ide
}
mac { 
    INCLUDEPATH += $$ROOT/src/qsa/src/qsa
    INCLUDEPATH += $$ROOT/src/qsa/src/ide
}
