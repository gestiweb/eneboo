PRJ_MODULE      = SqlDrivers-mysql
exists(../../../../settings.pro) {
	include(../../../../settings.pro)
}
else {
	error("settings.pro missing, unable to build")
}
TEMPLATE = lib

CONFIG += warn_off plugin

DEFINES += DONT_USE_RAID MYSQL_CLIENT DBUG_OFF

DESTDIR = $$PREFIX/plugins/sqldrivers

TARGET = qmysql

INCLUDEPATH += $$ROOT $$ROOT/src/flbase $$ROOT/src/libpq $$ROOT/src/libmysql/include

mac {
load(qsa)
LIBS += -L$$PREFIX/lib -lflbase -lmysqlclient -ladvance -lqui -lflmail -lqwt
enable_digidoc:LIBS += -llibdigidoc -lcrypto -lssl -lxml2
} 
else {
LIBS += -L$$PREFIX/lib -lflbase -lmysqlclient
}

HEADERS = flsql_mysql.h

SOURCES = flsql_mysql.cpp main.cpp
