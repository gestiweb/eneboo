AQMODULE = AQScript
include(../../gen_settings.pri)
HEADERS = ../xml/script/AQXmlObjectFactoryQSA.h \
    ../xml/script/AQDomNodeListInterface.h \
    ../sql/script/AQVarInterface.h \
    ../sql/script/AQSqlInterface.h \
    AQScript.h \
    AQObjectFactoryQSA.h \
    compiler/AQParserQSA.h \
    compiler/AQGrammar.h \
    compiler/AQNodesQSA.h \
    compiler/AQCompilerQSA.h \
    AQObjectInterfaceQSA.h \
    ../sql/script/AQSqlQueryInterface.h \
    AQScriptEngineQSA.h \
    ../sql/script/AQSqlCursorInterface.h \
    ../sql/script/AQSqlObjectFactoryQSA.h \
    ../corelib/script/AQCoreObjectFactoryQSA.h \
    ../corelib/script/AQUtilInterface.h
SOURCES = ../xml/script/AQXmlObjectFactoryQSA.cpp \
    ../xml/script/AQDomNodeListInterface.cpp \
    ../sql/script/AQVarInterface.cpp \
    AQObjectFactoryQSA.cpp \
    compiler/AQParserQSA.cpp \
    compiler/AQGrammar.cpp \
    compiler/AQNodesQSA.cpp \
    compiler/AQCompilerQSA.cpp \
    AQObjectInterfaceQSA.cpp \
    ../sql/script/AQSqlQueryInterface.cpp \
    ../sql/script/AQSqlCursorInterface.cpp \
    ../sql/script/AQSqlObjectFactoryQSA.cpp \
    AQScriptEngineQSA.cpp \
    ../corelib/script/AQCoreObjectFactoryQSA.cpp \
    ../corelib/script/AQUtilInterface.cpp
include(../$$AQUISRC_DIR/script/script.pri)
include(../../settings.pri)
TEMPLATE = lib
TARGET = AQScript
VERSION = 3.0
CONFIG += dll \
    create_prl
QT += sql \
    xml \
    qt3support \
    gui
DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
INSTALLS += target
include(../../includes.pri)
