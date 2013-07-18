TEMPLATE = subdirs

CONFIG  += ordered

SUBDIRS	=  uic \
	   uilib \
	   designer
linux-g++*:SUBDIRS += app

dll:SUBDIRS *=  editor plugins
shared:SUBDIRS *=  editor plugins
REQUIRES=full-config #nocrosscompiler
