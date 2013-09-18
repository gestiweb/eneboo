AQMODULE = AQSql
HEADERS = util/AQSqlSavePoint.h \
    AQSqlDatabase_p.h \
    shell/AQSqlShell.h \
    filesystem/AQSqlDir.h \
    filesystem/AQSqlNodeInfo.h \
    filesystem/AQSqlFileSystem.h \
    AQSql.h \
    manager/AQManager.h \
    manager/AQManagerModules.h \
    util/AQGroupByQuery.h \
    util/AQParameterQuery.h \
    util/AQSqlUtil.h \
    AQSqlConnections.h \
    AQSqlQuery.h \
    AQSqlCursor.h \
    AQSqlDatabase.h \
    AQSqlDriver.h \
    util/AQVar.h
SOURCES = util/AQSqlSavePoint.cpp \
    shell/AQSqlShell.cpp \
    filesystem/AQSqlDir.cpp \
    filesystem/AQSqlNodeInfo.cpp \
    filesystem/AQSqlFileSystem.cpp \
    manager/AQManager.cpp \
    manager/AQManagerModules.cpp \
    util/AQGroupByQuery.cpp \
    util/AQParameterQuery.cpp \
    util/AQSqlUtil.cpp \
    AQSqlConnections.cpp \
    AQSqlQuery.cpp \
    AQSqlCursor.cpp \
    AQSqlDatabase.cpp \
    AQSqlDriver.cpp \
    util/AQVar.cpp
include (qt3support/sql.pri)
include (../../settings.pri)
TEMPLATE = lib
TARGET = AQSql
VERSION = 3.0
CONFIG += dll \
    create_prl
QT += core \
    sql \
    xml
QT -= gui
DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
INSTALLS += target
LIBS += -lAQCore -lAQAcl
include(../../includes.pri)
