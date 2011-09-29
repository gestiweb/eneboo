exists(../../../../settings.pro) {
        include(../../../../settings.pro)
}
else {
	error("settings.pro missing, unable to build")
}

TEMPLATE = lib
CONFIG += warn_off plugin
HEADERS = bits.h bluecurve.h
SOURCES = bluecurve.cpp main.cpp
DESTDIR = $$PREFIX/plugins/styles
TARGET = bluecurve

