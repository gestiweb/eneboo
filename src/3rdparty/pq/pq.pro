AQMODULE = libpq

include(../../../settings.pri)

TEMPLATE = lib
TARGET = aqpq
VERSION = 5.0

QT -= gui core

DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
INSTALLS += target
CONFIG = release dll create_prl

DEFINES +=  SYSCONFDIR=\'\""$$ABANQINSDIR"\"\' _GNU_SOURCE FRONTEND ENABLE_THREAD_SAFETY _THREAD_SAFE _POSIX_PTHREAD_SEMANTICS

LIBS += -lcrypt

INCLUDEPATH += ./include

HEADERS = fe-auth.h \
    libpq-fe.h \
    libpq-int.h \
    pqexpbuffer.h \
    pqsignal.h \
    include/postgres_ext.h

SOURCES = encnames.c \
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

include(../../../includes.pri)

