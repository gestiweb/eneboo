TEMPLATE = lib
TARGET	 = qgfxtransformed

CONFIG  += qt warn_off release plugin
DESTDIR	 = ../../../gfxdrivers

DEFINES	-= QT_NO_QWS_TRANSFORMED
unix:OBJECTS_DIR = .obj

HEADERS		= ../../../../include/qgfxtransformed_qws.h
SOURCES		= main.cpp \
		  ../../../../src/embedded/qgfxtransformed_qws.cpp


target.path=$$plugins.path/gfxdrivers
INSTALLS += target
