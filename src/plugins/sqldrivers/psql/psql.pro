exists(../../../../settings.pro) {
	include(../../../../settings.pro)
}
else {
	error("settings.pro missing, unable to build")
}

TEMPLATE = lib

CONFIG += warn_off plugin

DESTDIR = $$PREFIX/plugins/sqldrivers

DEFINES += FRONTEND WIN32_CLIENT_ONLY

TARGET = qpsql

INCLUDEPATH += $$ROOT $$ROOT/src/flbase

INCLUDEPATH += $$ROOT/src/libpq/include \
               $$ROOT/src/libpq/

mac {
load(qsa)
LIBS += -L$$PREFIX/lib -lflbase -lpq -ladvance -lqui -lflmail -lqwt
} 
else {
LIBS += -L$$PREFIX/lib -lflbase -lpq
}

HEADERS += qsql_psql.h

SOURCES += qsql_psql.cpp main.cpp
