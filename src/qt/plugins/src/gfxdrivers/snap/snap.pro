TEMPLATE	= lib
CONFIG      += qt warn_off release plugin

!exists( $(SCITECH)/include/snap/graphics.h) {
    message("SciTech SNAP SDK is not properly set up! Please make sure the SCITECH")
    message("environment variable is pointing to the SciTech SNAP SDK.")
    error("Please fix and re-build the makefiles.")
}

HEADERS		= ../../../../include/qgfxsnap_qws.h

SOURCES		= main.cpp \
		    ../../../../src/embedded/qgfxsnap_qws.cpp

unix:OBJECTS_DIR	= .obj

TARGET		= qgfxsnap
DESTDIR		= ../../../gfxdrivers
DEFINES		-= QT_NO_QWS_SNAP
INCLUDEPATH += $(SCITECH)/include

debug:LIBS  += -L$(SCITECH)/lib/debug/linux/gcc/x86/so -lpm
else:LIBS   += -L$(SCITECH)/lib/release/linux/gcc/x86/so -lpm

target.path += $$plugins.path/gfxdrivers
INSTALLS 	+= target

