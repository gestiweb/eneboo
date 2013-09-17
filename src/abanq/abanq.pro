AQMODULE = AbanQ

include(../../settings.pri)

QMAKE_DIR             = @echo $$AQMODULE ::   [MKDIR] $< && $$QMAKE_MKDIR
QMAKE_COPY            = @echo $$AQMODULE ::   [CP___] $< && $$QMAKE_COPY
QMAKE_CC              = @echo $$AQMODULE ::   [CC___] $< && $$QMAKE_CC
QMAKE_CXX             = @echo $$AQMODULE ::   [C++__] $< && $$QMAKE_CXX
QMAKE_MOC             = @echo $$AQMODULE ::   [MOC__] $< && $$QMAKE_MOC
QMAKE_UIC             = @echo $$AQMODULE ::   [UIC__] $< && $$QMAKE_UIC
QMAKE_AR              = @echo $$AQMODULE ::   [AR___] $< && $$QMAKE_AR
QMAKE_LINK            = @echo $$AQMODULE ::   [LK___] $@ && $$QMAKE_LINK
QMAKE_LINK_SHLIB      = @echo $$AQMODULE ::   [LK___] $@ && $$QMAKE_LINK_SHLIB
QMAKE_INSTALL_FILE    = @echo $$AQMODULE ::   [INS__] $@ && $$QMAKE_INSTALL_FILE
QMAKE_INSTALL_PROGRAM = @echo $$AQMODULE ::   [INS__] $@ && $$QMAKE_INSTALL_PROGRAM

QMAKE_SEPARATE_DEBUG_INFO         = @echo $$AQMODULE ::   [DBG__] $@ && $$QMAKE_SEPARATE_DEBUG_INFO
QMAKE_INSTALL_SEPARATE_DEBUG_INFO = @echo $$AQMODULE ::   [DBG__] $@ && $$QMAKE_INSTALL_SEPARATE_DEBUG_INFO

QMAKE_MOVE            = @echo $$AQMODULE ::   [MV___] $@ && $$QMAKE_MOVE
QMAKE_DEL_FILE        = @echo $$AQMODULE ::   [RM___] $@ && $$QMAKE_DEL_FILE
QMAKE_DEL_DIR         = @echo $$AQMODULE ::   [RMDIR] $@ && $$QMAKE_DEL_DIR
QMAKE_STRIP           = @echo $$AQMODULE ::   [STRIP] $@ && $$QMAKE_STRIP
QMAKE_CHK_DIR_EXISTS  = @echo $$AQMODULE ::   [CHK__] $@ && $$QMAKE_CHK_DIR_EXISTS

TEMPLATE = app
TARGET = abanq
QT -= script
QT += core gui sql xml qt3support
CONFIG += designer
SOURCES = main.cpp
VERSION = 3.0
RESOURCES = 
DESTDIR = $$ABANQBLDDIR/bin
target.path = $$ABANQINSDIR/bin
INSTALLS += target

enable_hoard:LIBS += -lhoard -ldl
LIBS += -lAQbarcode -lAQkugar -lAQCore -lAQAcl -lAQUi -lAQSql -lAQXml -lAQScript -lAQbarcode -lqsa
