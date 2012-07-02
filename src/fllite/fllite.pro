PRJ_MODULE      = FLLite
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
  TARGET = Eneboo
} else {
  TARGET = eneboo
}

INCLUDEPATH += $$ROOT/src/qt/src/kernel
INCLUDEPATH += $$ROOT $$ROOT/src/flbase
DEPENDPATH += $$ROOT/src/flbase

LIBS *= -L$$PREFIX/lib
enable_hoard:LIBS *= -lhoard
enable_hoard:!win32:LIBS *= -L$$PREFIX/lib -ldl
LIBS *= -L$$PREFIX/lib -lflbase
mac:LIBS *= -ladvance -lqui -lflmail -lqwt -lxslt -lexslt -lxsltproc
enable_digidoc:mac:LIBS *= -llibdigidoc -lcrypto -lssl -lxml2

load(qsa)

SOURCES += main.cpp

IMAGES += images/splashdebugger.png \
          images/splashclient.png


win32:QMAKE_LFLAGS += -mconsole -mwindows

win32 { 
    INCLUDEPATH += $$ROOT/src/qsa/src/qsa
    INCLUDEPATH += $$ROOT/src/qsa/src/ide
}
mac { 
    INCLUDEPATH += $$ROOT/src/qsa/src/qsa
    INCLUDEPATH += $$ROOT/src/qsa/src/ide
}
