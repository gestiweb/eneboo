TEMPLATE	= lib
CONFIG		+= qt warn_on plugin
HEADERS		= quickcustom.h quickpaintwidget.h quickcolorbutton.h
SOURCES		= quickcustom.cpp quickpaintwidget.cpp quickcolorbutton.cpp
INTERFACES	=
DESTDIR		= $(QTDIR)/plugins/qsa
TARGET		= quickcustom

isEmpty(QT_SOURCE_TREE):QT_SOURCE_TREE=$(QTDIR)

INCLUDEPATH     += $$QT_SOURCE_TREE/tools/designer/interfaces

target.path=$$plugins.path
isEmpty(target.path):target.path=$$QT_PREFIX/plugins
INSTALLS += target
