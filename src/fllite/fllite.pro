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

mac {
  TARGET = AbanQ
} else {
  TARGET = abanq 
}

INCLUDEPATH += $$ROOT/src/qt/src/kernel
INCLUDEPATH += $$ROOT $$ROOT/src/flbase
DEPENDPATH += $$ROOT/src/flbase

LIBS *= -L$$PREFIX/lib
enable_hoard:LIBS *= -lhoard
enable_hoard:!win32:LIBS *= -L$$PREFIX/lib -ldl
LIBS *= -L$$PREFIX/lib -lflbase
mac:LIBS *= -ladvance -lqui -lflmail -lqwt

load(qsa)

SOURCES += main.cpp

contains(DEFINES, FL_QUICK_CLIENT) {
	IMAGES += images/splashabanq.png
} else {
	IMAGES += images/splashabanq.png
}

win32:QMAKE_LFLAGS += -mconsole -mwindows
