PRJ_MODULE      = LibHoard
exists(../../settings.pro) {
        include(../../settings.pro)
}
else {
        error("settings.pro missing, unable to build")
}
TEMPLATE = lib
CONFIG += warn_off exceptions
!mac:CONFIG += plugin
CONFIG -= x11 qt
DEFINES +=  NDEBUG _REENTRANT=1 __GNUC__ unix
SLH = \ $${LITERAL_HASH}
DEFINES += 'CUSTOM_PREFIX(x)=hoard$$join(SLH)$$join(SLH)x'
DESTDIR = $$PREFIX/lib
TARGET = hoard
INCLUDEPATH += /usr/include/nptl . $$ROOT/src/qt/include $$ROOT/src/hoard/src $$ROOT/src/hoard/src/heaplayers $$ROOT/src/hoard/src/heaplayers/util
unix:LIBS = -ldl -lpthread

win32 {
        CONFIG += dll
        DLLDESTDIR = $$PREFIX/bin
	      enable_win64:LIBS = -L$$PREFIX/lib -lpthreadAQ -lm
	      !enable_win64:LIBS = -L$$PREFIX/lib -lpthreadAQ291 -lm
}

VERSION = 3.8

linux-g++:QMAKE_CXXFLAGS = -pthread -malign-double -pipe -O3 -finline-limit=20000 -fomit-frame-pointer -finline-functions
linux-g++-32:QMAKE_CXXFLAGS = -m32 -pthread -malign-double -pipe -O3 -finline-limit=20000 -fomit-frame-pointer -finline-functions
linux-g++-64:QMAKE_CXXFLAGS = -pthread -pipe -O3 -finline-limit=20000 -fomit-frame-pointer -finline-functions
linux-generic-g++:QMAKE_CXXFLAGS = -m32 -pthread -malign-double -pipe -O3 -finline-limit=20000 -fomit-frame-pointer -finline-functions -fexceptions
win32:QMAKE_CXXFLAGS = -malign-double -pipe -O3 -finline-limit=20000 -fomit-frame-pointer -finline-functions
QMAKE_LFLAGS += -Bsymbolic

!win32:!mac:SOURCES = ./src/gnuwrapper.cpp 
SOURCES += ./src/libhoard.cpp
