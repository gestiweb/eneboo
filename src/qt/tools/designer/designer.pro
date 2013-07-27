TEMPLATE = subdirs

CONFIG  += ordered

!macx-g++-cross:SUBDIRS	=  uic 
SUBDIRS	+= uilib \
	   designer \
	   app

dll:SUBDIRS *=  editor plugins
shared:SUBDIRS *=  editor plugins
REQUIRES=full-config #nocrosscompiler
