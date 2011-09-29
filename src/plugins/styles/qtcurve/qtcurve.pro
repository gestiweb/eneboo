exists(../../../../settings.pro) {
        include(../../../../settings.pro)
}
else {
	error("settings.pro missing, unable to build")
}

TEMPLATE = lib
CONFIG += warn_off plugin
INCLUDEPATH += .
HEADERS += qtcurve.h \
           shortcuthandler.h \
           qtc_kstyle.h \
           pixmaps.h
SOURCES += common.cpp \
           config_file.cpp \
           colorutils.cpp \
           qtc_kstyle.cpp \
           qtcurve.cpp \
           shortcuthandler.cpp
DESTDIR = $$PREFIX/plugins/styles
DEFINES += qtcurve_EXPORTS
TARGET = qtcurve

