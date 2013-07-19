PRJ_MODULE      = pthreads
exists(../../settings.pro) {
	include(../../settings.pro)
}
else {
        error("settings.pro missing, unable to build")
}

TEMPLATE = lib
DEFINES += _REENTRANT__CLEANUP_C PTW32_BUILD PTW32_BUILD_INLINED HAVE_PTW32_CONFIG_H
CONFIG += warn_off
CONFIG -= qt debug
TARGET = pthreadAQ
DESTDIR = $$PREFIX/lib
INCLUDEPATH += $$ROOT/src/pthreads

win32 {
  CONFIG += dll
  DLLDESTDIR = $$PREFIX/bin
  RC_FILE = version.rc
  LIBS += -lws2_32
  QMAKE_CFLAGS = 
  QMAKE_LFLAGS =
}

VERSION = 2.9.1

QMAKE_CXXFLAGS += -O3
QMAKE_CFLAGS += -O3

SOURCES	+= pthread.c

