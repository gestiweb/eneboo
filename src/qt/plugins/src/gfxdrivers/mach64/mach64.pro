TEMPLATE = lib
TARGET	 = qgfxmach64

CONFIG  += qt warn_off release plugin
DESTDIR	 = ../../../gfxdrivers

DEFINES	-= QT_NO_QWS_MACH64
unix:OBJECTS_DIR = .obj

HEADERS		= ../../../../include/qgfxmach64_qws.h
SOURCES		= main.cpp \
		  ../../../../src/embedded/qgfxmach64_qws.cpp


target.path += $$plugins.path/gfxdrivers
INSTALLS 	+= target
