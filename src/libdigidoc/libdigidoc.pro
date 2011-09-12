exists(../../settings.pro) {
	include(../../settings.pro)
}
else {
	error("settings.pro missing, unable to build")
}

TEMPLATE = lib

CONFIG += warn_off create_prl plugin
CONFIG -= x11 qt

win32 {
	CONFIG += dll
	DLLDESTDIR = $$PREFIX/bin
	DEFINES += BUILDINGDLL
	LIBS = -L$$PREFIX/lib -lpthreadGCE2 -lm -lqt-mt
	INCLUDEPATH += $$ROOT/src/qt/src/3rdparty/zlib
}

DESTDIR = $$PREFIX/lib

DEFINES += DIGIDOC_DEBUG WITH_SOAPDEFS_H SYSCONFDIR=\"$$PREFIX/share/abanq\"

TARGET = libdigidoc

INCLUDEPATH += $$ROOT/src $$ROOT/src/libdigidoc $$ROOT/src/libdigidoc/libxml2 \
               $$ROOT/src/libdigidoc/libxml2/include $$ROOT/src/libdigidoc/libxml2/libxml \
               $$ROOT/src/libdigidoc/openssl $$ROOT/src/libdigidoc/openssl/ssl \
               $$ROOT/src/libdigidoc/openssl/include $$ROOT/src/libdigidoc/openssl/crypto
QMAKE_LIBS_X11SM =

LIBS += -L$$PREFIX/lib -lssl -lcrypto -lxml2

VERSION = 2.6.0

HEADERS += DigiDocConfig.h DigiDocLib.h  DigiDocPKCS11.h DigiDocError.h DigiDocSAXParser.h DigiDocParser.h DigiDocMem.h DigiDocStack.h \
	   DigiDocEnc.h DigiDocObj.h DigiDocEncGen.h DigiDocEncSAXParser.h DigiDocDebug.h DigiDocDefs.h DigiDocCert.h DigiDocConvert.h \
	   DigiDocVerify.h DigiDocOCSP.h DigiDocGen.h DigiDocDfExtract.h
SOURCES += DigiDocConfig.c  DigiDocLib.c  DigiDocObj.c DigiDocPKCS11.c DigiDocError.c DigiDocParser.c DigiDocDebug.c DigiDocSAXParser.c \
	   DigiDocMem.c DigiDocStack.c DigiDocEnc.c DigiDocEncGen.c DigiDocEncSAXParser.c DigiDocCert.c DigiDocConvert.c DigiDocGen.c \ 
	   DigiDocVerify.c DigiDocOCSP.c DigiDocDfExtract.c

