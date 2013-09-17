AQMODULE=QSA

include (../../../../../settings.pri)

TEMPLATE	= lib
VERSION  	= 1.2.3
TARGET 		= qsa

#CONFIG += noide noeditor
#CONFIG += neweditor

DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
INSTALLS += target

HEADERS		= qsconfig.h \ 
		  qsaglobal.h \
		  qsproject.h \
		  qsinterpreter.h \
		  qsobjectfactory.h \
		  qswrapperfactory.h \
          qsscript.h \
          qsutilfactory.h

SOURCES		= \
                  qsproject.cpp \
                  qsinterpreter.cpp \
                  qsobjectfactory.cpp \
                  qswrapperfactory.cpp \
                  qsscript.cpp \
                  qsutilfactory.cpp

!nogui {
	QT += qt3support
	SOURCES += qsinputdialogfactory.cpp
	HEADERS += qsinputdialogfactory.h
}

!noeditor|neweditor{
	SOURCES += qseditor.cpp
	HEADERS += qseditor.h
	!noeditor:!neweditor:QT += qt3support
}

noeditor:!neweditor:!noide:CONFIG+=noide

shared {
	win32:DEFINES	+= QSA_DLL
} else {
	win32:DEFINES   += QSA_NO_DLL
}


INCLUDEPATH	+= ../engine \
		   ../kernel \
		    .

!noeditor:!neweditor: INCLUDEPATH += ../../tools/designer/editor

DEFINES += HAVE_CONFIG_H
DEFINES += QT_INTERNAL_XML
DEFINES += QT_INTERNAL_WORKSPACE
DEFINES += QT_INTERNAL_ICONVIEW
DEFINES += QT_INTERNAL_TABLE

LIBS += -L$$ABANQBLDDIR/lib

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
		qsscript.h \
		..\ide\qsworkbench.h

KERNELDIR = ../kernel
!noeditor {
  neweditor {
    EDITORDIR = ../neweditor
    INCLUDEPATH += ../neweditor
  } else {
    EDITORDIR = ../editor
    EDITOR_BRANCH = ../../tools/designer/editor
  }
}

!noide:IDEDIR = ../ide
include( ../kernel/kernel.pri )
!noeditor {
  neweditor {
    include( ../neweditor/editor.pri )
  } else {
    include( ../editor/editor.pri )
  }
}
!noide:include( ../ide/ide.pri )
!noeditor:!neweditor:include(../../tools/designer/editor/editor.pri)

load(uic)

PHEADERS_DIR = ../private_headers
include(../private_headers/headers.pri)

win32 {
    CONFIG += precompile_header
    PRECOMPILED_HEADER = qsa_pch.h
}

nogui {
    QT = core
    DEFINES += QSA_NO_GUI
}

include(../../../../../includes.pri)

