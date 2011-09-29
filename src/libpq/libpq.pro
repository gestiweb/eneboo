exists(../../settings.pro) {
        include(../../settings.pro)
}
else {
        error("settings.pro missing, unable to build")
}

TEMPLATE = lib
CONFIG += warn_off
!mac:CONFIG += plugin
CONFIG -= qt x11

win32 {
        CONFIG += dll
        DLLDESTDIR = $$PREFIX/bin
}

DEFINES +=  SYSCONFDIR='"$$PREFIX"' FRONTEND ENABLE_THREAD_SAFETY _THREAD_SAFE _POSIX_PTHREAD_SEMANTICS
DESTDIR = $$PREFIX/lib
TARGET = pq
unix:INCLUDEPATH = . $$ROOT/src/libpq/include
win32:INCLUDEPATH = . $$ROOT/src/pthreads $$ROOT/src/libpq/include $$ROOT/src/libpq/include/port/win32

unix:!mac:LIBS += -lcrypt -lresolv -lnsl
win32:LIBS += -L$$PREFIX/lib -lpthreadGCE2 -lshfolder -lws2_32 -lm

win32:RC_FILES = libpq.rc

VERSION = 4.1

SOURCES =  encnames.c \
           fe-auth.c \
           fe-connect.c \
           fe-exec.c \
           fe-lobj.c \
           fe-misc.c \
           fe-print.c \
           fe-protocol2.c \
           fe-protocol3.c \
           fe-secure.c \
           ip.c \
           md5.c \
           noblock.c \
           pgstrcasecmp.c \
           pqexpbuffer.c \
           pqsignal.c \
           thread.c \
           wchar.c

HEADERS  = libpq-fe.h \
           libpq-int.h \
           fe-auth.h \
           pqexpbuffer.h \
           pqsignal.h

win32 {
SOURCES += win32.c \
           pthread-win32.c \
	         libpqdll.c \	 
	         crypt.c \
	         open.c \
	         inet_aton.c \
	         getaddrinfo.c
HEADERS += win32.h \
	         pthread-win32.h \
	         libpq-fe.h
}	         


