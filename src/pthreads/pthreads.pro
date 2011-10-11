PRJ_MODULE      = pthreads
exists(../../settings.pro) {
	include(../../settings.pro)
}
else {
        error("settings.pro missing, unable to build")
}

TEMPLATE = lib
DEFINES += _REENTRANT HAVE_CONFIG_H
CONFIG += warn_off exceptions
CONFIG -= qt debug
TARGET = pthreadGCE
DESTDIR = $$PREFIX/lib
INCLUDEPATH += $$ROOT/src/pthreads

win32 {
  CONFIG += dll
  DLLDESTDIR = $$PREFIX/bin
  RC_FILE = version.rc
  LIBS += -lws2_32
  QMAKE_CC = $(CXX)
  QMAKE_CFLAGS = 
  QMAKE_LFLAGS =
}

VERSION = 2

SOURCES	+= attr.c \
		barrier.c \
		cancel.c \
		cleanup.c \
		condvar.c \
		create.c \
		dll.c \
		errno.c \
		exit.c \
		fork.c \
		global.c \
		misc.c \
		mutex.c \
		nonportable.c \
		private.c \
		rwlock.c \
		sched.c \
		semaphore.c \
		signal.c \
		spin.c \
		sync.c \
		tsd.c

