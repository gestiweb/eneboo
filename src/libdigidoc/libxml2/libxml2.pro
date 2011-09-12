exists(../../../settings.pro) {
	include(../../../settings.pro)
}
else {
	error("settings.pro missing, unable to build")
}

TEMPLATE = lib
CONFIG += warn_off plugin
CONFIG -= x11 qt

win32 {
	CONFIG += dll
	DLLDESTDIR = $$PREFIX/bin
	LIBS = -L$$PREFIX/lib -lpthreadGCE2 -lm -lws2_32
}

DESTDIR = $$PREFIX/lib

DEFINES += HAVE_CONFIG_H 
win32:DEFINES += IN_LIBXML

INCLUDEPATH +=  $$ROOT/src/libdigidoc/libxml2 $$ROOT/src/libdigidoc/libxml2/include $$ROOT/src/libdigidoc/libxml2/libxml
QMAKE_LIBS_X11SM =

TARGET = xml2

VERSION = 2.7.7

HEADERS += acconfig.h \
           config.h \
           elfgcchack.h \
           libxml.h \
           trio.h \
           triodef.h \
           trionan.h \
           triop.h \
           triostr.h \
           include/win32config.h \
           include/wsockcompat.h \
           include/libxml/c14n.h \
           include/libxml/catalog.h \
           include/libxml/chvalid.h \
           include/libxml/debugXML.h \
           include/libxml/dict.h \
           include/libxml/DOCBparser.h \
           include/libxml/encoding.h \
           include/libxml/entities.h \
           include/libxml/globals.h \
           include/libxml/hash.h \
           include/libxml/HTMLparser.h \
           include/libxml/HTMLtree.h \
           include/libxml/list.h \
           include/libxml/nanoftp.h \
           include/libxml/nanohttp.h \
           include/libxml/parser.h \
           include/libxml/parserInternals.h \
           include/libxml/pattern.h \
           include/libxml/relaxng.h \
           include/libxml/SAX.h \
           include/libxml/SAX2.h \
           include/libxml/schemasInternals.h \
           include/libxml/schematron.h \
           include/libxml/threads.h \
           include/libxml/tree.h \
           include/libxml/uri.h \
           include/libxml/valid.h \
           include/libxml/xinclude.h \
           include/libxml/xlink.h \
           include/libxml/xmlautomata.h \
           include/libxml/xmlerror.h \
           include/libxml/xmlexports.h \
           include/libxml/xmlIO.h \
           include/libxml/xmlmemory.h \
           include/libxml/xmlmodule.h \
           include/libxml/xmlreader.h \
           include/libxml/xmlregexp.h \
           include/libxml/xmlsave.h \
           include/libxml/xmlschemas.h \
           include/libxml/xmlschemastypes.h \
           include/libxml/xmlstring.h \
           include/libxml/xmlunicode.h \
           include/libxml/xmlversion.h \
           include/libxml/xmlwriter.h \
           include/libxml/xpath.h \
           include/libxml/xpathInternals.h \
           include/libxml/xpointer.h
SOURCES += c14n.c \
           catalog.c \
           chvalid.c \
           debugXML.c \
           dict.c \
           DOCBparser.c \
           encoding.c \
           entities.c \
           error.c \
           globals.c \
           hash.c \
           HTMLparser.c \
           HTMLtree.c \
           legacy.c \
           list.c \
           nanoftp.c \
           nanohttp.c \
           parser.c \
           parserInternals.c \
           pattern.c \
           relaxng.c \
           SAX.c \
           SAX2.c \
           schematron.c \
           threads.c \
           tree.c \
           trio.c \
           trionan.c \
           triostr.c \
           uri.c \
           valid.c \
           xinclude.c \
           xlink.c \
           xmlIO.c \
           xmllint.c \
           xmlmemory.c \
           xmlmodule.c \
           xmlreader.c \
           xmlregexp.c \
           xmlsave.c \
           xmlschemas.c \
           xmlschemastypes.c \
           xmlstring.c \
           xmlunicode.c \
           xmlwriter.c \
           xpath.c \
           xpointer.c
           
