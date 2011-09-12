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

TARGET = barcode

VERSION = 0.98

SOURCES += codabar.cpp \
           code128.cpp \
           code39.cpp \
           code93.cpp \
           ean.cpp \
           i25.cpp \
           library.cpp \
           msi.cpp \
           pcl.cpp \
           plessey.cpp \
           ps.cpp

HEADERS += barcode.h

unix {
        QMAKE_CFLAGS += $$QMAKE_CFLAGS_SHLIB
        QMAKE_CXXFLAGS += $$QMAKE_CXXFLAGS_SHLIB
}
