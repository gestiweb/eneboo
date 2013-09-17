AQMODULE = AQbarcode

SOURCES += AQCodBar.cpp \
    src/codabar.cpp \
    src/code128.cpp \
    src/code39.cpp \
    src/code93.cpp \
    src/ean.cpp \
    src/i25.cpp \
    src/library.cpp \
    src/msi.cpp \
    src/pcl.cpp \
    src/plessey.cpp \
    src/ps.cpp
HEADERS += AQCodBar.h \
    src/barcode.h
include(../../../settings.pri)
TEMPLATE = lib
TARGET = AQbarcode
VERSION = 1.0
CONFIG += release dll create_prl warn_off
DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
INSTALLS += target
include(../../../includes.pri)
