TEMPLATE = lib
TARGET	 = qgfxvoodoo

CONFIG  += qt warn_off release plugin
DESTDIR	 = ../../../gfxdrivers

DEFINES	-= QT_NO_QWS_VOODOO3
unix:OBJECTS_DIR	= .obj

HEADERS		= ../../../../include/qgfxvoodoo_qws.h \
		  ../../../../include/qgfxvoodoodefs_qws.h
SOURCES		= main.cpp \
		  ../../../../src/embedded/qgfxvoodoo_qws.cpp


target.path += $$plugins.path/gfxdrivers
INSTALLS 	+= target
