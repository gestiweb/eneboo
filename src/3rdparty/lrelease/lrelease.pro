AQMODULE = AQCore

include(../../../settings.pri)

TEMPLATE = lib
TARGET = AQlrelease
VERSION = 1.0

CONFIG += qt console
CONFIG += create_prl staticlib
CONFIG -= dll
CONFIG -= no_keywords

DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
INSTALLS += target

HEADERS = AQTranslations.h
SOURCES = AQTranslations.cpp

HEADERS         += src/metatranslator.h \
                  src/translator.h \
                  src/profileevaluator.h \
                  src/proparser.h \
                  src/xliff.h
SOURCES         += src/metatranslator.cpp \
                  src/translator.cpp \
                  src/profileevaluator.cpp \
                  src/proparser.cpp \
                  src/xliff.cpp

QT -= gui
QT += xml

PROPARSERPATH = src
INCLUDEPATH += $$PROPARSERPATH
# Input
HEADERS += $$PROPARSERPATH/proitems.h \
        $$PROPARSERPATH/proreader.h
SOURCES += $$PROPARSERPATH/proitems.cpp \
        $$PROPARSERPATH/proreader.cpp

INCLUDEPATH     += src

include(../../../includes.pri)
