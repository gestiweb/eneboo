exists(../../../../settings.pro) {
	include(../../../../settings.pro)
} else {
  error("settings.pro missing, unable to build")
}

AQ_LPR_DIR = $$ROOT/src/posprinter/lpr

# TEMPLATE = lib
#CONFIG -= moc
#CONFIG += warn_off create_prl link_prl
#CONFIG -= qt
#!mac:CONFIG += plugin
#unix:CONFIG -= x11
INCLUDEPATH += $$AQ_LPR_DIR/lpr $$AQ_LPR_DIR/common_source
DEFINES += _GNU_SOURCE __KAME__

#win32 {
#	CONFIG += dll
#	DLLDESTDIR = $$PREFIX/bin
#}
#DESTDIR = $$PREFIX/lib
#TARGET = lpr

SOURCES += $$ROOT/src/posprinter/lpr/lpr/lpr.c \
           $$ROOT/src/posprinter/lpr/common_source/startdaemon.c \
           $$ROOT/src/posprinter/lpr/common_source/common.c \
	       $$ROOT/src/posprinter/lpr/common_source/common_vars.c \
	       $$ROOT/src/posprinter/lpr/common_source/getcap.c

SOURCES += $$ROOT/src/posprinter/lpr/common_source/strlcpy.c \
           $$ROOT/src/posprinter/lpr/common_source/strlcat.c
