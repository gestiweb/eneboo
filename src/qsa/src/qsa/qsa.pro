exists(../../../../settings.pro) {
        include(../../../../settings.pro)
}
else {
        error("settings.pro missing, unable to build")
}

TEMPLATE	= lib
CONFIG		+= warn_on link_prl
!mac:CONFIG += plugin
shared:CONFIG += qt dll
TARGET 		= qsa
VERSION  	= 1.1.5

INCLUDEPATH += $$ROOT/src/flbase

isEmpty(QSA_INSTALL_PREFIX) {
  QSA_BUILD_PREFIX=$$(QTDIR)
} else {
  QSA_BUILD_PREFIX=../..
}

DESTDIR         = $$QSA_BUILD_PREFIX/lib
DLLDESTDIR      = $$QSA_BUILD_PREFIX/bin

HEADERS		= \
		  qsproject.h \
		  qsinterpreter.h \
		  qsobjectfactory.h \
		  qswrapperfactory.h \
		  qseditor.h \
		  qsinputdialogfactory.h \
		  qsargument.h \
                  qsscript.h \
                  qsutilfactory.h

SOURCES		= \
		  qsproject.cpp \
		  qsinterpreter.cpp \
		  qsobjectfactory.cpp \
		  qswrapperfactory.cpp \
		  qsinputdialogfactory.cpp \
		  qseditor.cpp \
		  qsargument.cpp \
                  qsscript.cpp \
                  qsutilfactory.cpp

shared {
	win32:DEFINES	+= QSA_DLL
} else {
	win32:DEFINES   += QSA_NO_DLL
}

# I think this belongs here
isEmpty(QT_SOURCE_TREE):QT_SOURCE_TREE=$(QTDIR)
isEmpty(QT_INSTALL_PREFIX):QT_INSTALL_PREFIX=$(QTDIR)

INCLUDEPATH	+= ../engine \
		   ../../tools/designer/editor \
		   ../kernel \
		    . 

DEFINES += HAVE_CONFIG_H
DEFINES += QT_INTERNAL_XML
DEFINES += QT_INTERNAL_WORKSPACE
DEFINES += QT_INTERNAL_ICONVIEW
DEFINES += QT_INTERNAL_TABLE

LIBS += -L$${QT_INSTALL_PREFIX}/lib

mac:QMAKE_LFLAGS_PREBIND    = -prebind -seg1addr 0xBC000000

headers.files 	= \
		qsaglobal.h \
                qsconfig.h \
		qsobjectfactory.h \
		qswrapperfactory.h \
		qseditor.h \
		qsproject.h \
		qsinterpreter.h \
		qsargument.h \
		qsinputdialogfactory.h \
		qsscript.h

!isEmpty(QSA_INSTALL_PREFIX) {
  headers.path = $$QSA_INSTALL_PREFIX/include
  win32 {
    target.path = $$QSA_INSTALL_PREFIX/bin
  } else {
    target.path = $$QSA_INSTALL_PREFIX/lib
  }
  prf.files = qsa.prf
  prf.path = $$QSA_INSTALL_PREFIX/mkspecs/features
  documentation.files = ../../doc/html/*
  documentation.path = $$QSA_INSTALL_PREFIX/doc/html
  INSTALLS += headers target prf documentation
}

KERNELDIR = ../kernel
EDITORDIR = ../editor
!noide:IDEDIR = ../ide
EDITOR_BRANCH = ../../tools/designer/editor

include( ../kernel/kernel.pri )
include( ../editor/editor.pri )
!noide:include( ../ide/ide.pri )
include(../../tools/designer/editor/editor.pri)

#win32:PRECOMPILED_HEADER = qsa_pch.h
