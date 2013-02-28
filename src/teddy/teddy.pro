PRJ_MODULE = teddy
exists(../../settings.pro) {
        include(../../settings.pro)
}
else {
        error("settings.pro missing, unable to build")
}

TEMPLATE = app

CONFIG += warn_off qt link_prl
unix:CONFIG += x11
CONFIG -= dll

DESTDIR = $$PREFIX/bin

TARGET = teddy

LIBS += -L$$PREFIX/lib -lqui

DEPENDPATH += $$ROOT/src/teddy/src
INCLUDEPATH += $$ROOT $$ROOT/src/teddy $$ROOT/src/teddy/src $$ROOT/src/teddy/ui

HEADERS += src/prefdialog.h src/teddy.h src/xmltable.h src/xmltablestyle.h
SOURCES += src/main.cpp src/prefdialog.cpp src/teddy.cpp src/xmltable.cpp
FORMS += ui/messagelist.ui ui/externaleditor.ui ui/general.ui

win32 {
	RC_FILE = src/teddy.rc
}

mac {
	RC_FILE = src/teddy.icns
}
