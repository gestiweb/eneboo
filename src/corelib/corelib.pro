AQMODULE = AQCore
HEADERS = metadata/AQActionMD.h \
    metadata/AQCompKeyMD.h \
    metadata/AQFieldMD_p.h \
    AQObject_p.h \
    AQObject.h \
    shell/AQShell.h \
    shell/AQShellCmdInfo.h \
    shell/AQShellCmd.h \
    metadata/AQTableMD.h \
    metadata/AQRelationMD.h \
    metadata/AQFieldMD.h \
    AQCore.h \
    cache/AQDiskCache.h \
    cache/AQMemCache.h \
    util/AQCoreUtil.h \
    AQTranslator.h \
    AQConfig.h
SOURCES = metadata/AQCompKeyMD.cpp \
    AQConfig.cpp \
    AQObject.cpp \
    shell/AQShell.cpp \
    shell/AQShellCmdInfo.cpp \
    shell/AQShellCmd.cpp \
    metadata/AQTableMD.cpp \
    metadata/AQRelationMD.cpp \
    metadata/AQFieldMD.cpp \
    cache/AQDiskCache.cpp \
    cache/AQMemCache.cpp \
    util/AQCoreUtil.cpp \
    AQTranslator.cpp
include (../../settings.pri)
TEMPLATE = lib
TARGET = AQCore
VERSION = 3.0
CONFIG += dll \
    create_prl
CONFIG -= no_keywords
QT -= gui
QT += xml
DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
INSTALLS += target
include(../../includes.pri)
LIBS += -lAQsha1 \
    -lAQlrelease
aqwui:LIBS += -lwt \
    -lwthttp \
    -lwtext
