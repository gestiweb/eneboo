exists(../../settings.pro) {
	include(../../settings.pro)
}
else {
	error("settings.pro missing, unable to build")
}
	
TEMPLATE = lib

CONFIG += warn_off create_prl link_prl qt staticlib
CONFIG -= dll

DESTDIR = $$PREFIX/lib

TARGET = lrelease

VERSION = 1.0

SOURCES += metatranslator.cpp proparser.cpp

HEADERS += metatranslator.h proparser.h

unix {
        QMAKE_CFLAGS += $$QMAKE_CFLAGS_SHLIB
        QMAKE_CXXFLAGS += $$QMAKE_CXXFLAGS_SHLIB
}
