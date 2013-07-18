exists(../../settings.pro) {
	include(../../settings.pro)
}
else {
	error("settings.pro missing, unable to build")
}

TEMPLATE = lib

CONFIG += warn_off createprl linkprl qt
CONFIG += enable_mailclients
!mac:CONFIG += plugin
unix:CONFIG += x11

win32 {
        CONFIG += dll shared
        DLLDESTDIR = $$PREFIX/bin
}
shared {
	win32:DEFINES	+= AQMAIL_DLL AQ_DLL
	enable_mailclients:win32:DEFINES += TKCBASE_EXPORTS AETHERA_EXPORTS TKCSSL_EXPORTS
} else {
	win32:DEFINES += AQMAIL_NO_DLL AQ_NO_DLL
}
DESTDIR = $$PREFIX/lib

INCLUDEPATH += $$ROOT . $$ROOT/src/flbase \
               $$ROOT/src/libdigidoc/openssl $$ROOT/src/libdigidoc/openssl/ssl \
               $$ROOT/src/libdigidoc/openssl/include $$ROOT/src/libdigidoc/openssl/crypto
               
LIBS += -L$$PREFIX/lib

TARGET = flmail

VERSION = 1.0.0

HEADERS += aqmailglobal.h \
           addressclass.h \
           addresslistclass.h \
           charsets.h \
           dateclass.h \
           headerclass.h \
           indexclass.h \
           mailfolder.h \
           md5.h \
           messageclass.h \
           messagedescriptor.h \
           messagedevice.h \
           messagefactory.h \
           mimecodec.h \
           mimepart.h \
           smtp.h \
           localmailfolder.h
           
enable_mailclients {
HEADERS += defines.h \
           settings.h \
           qsettingsextra.h \
           accounts.h \
           names.h \
           mailinfo.h \
           library.h \
           libraryloader.h \
           sslbase.h \
           sslsocket.h \
           ssldevice.h \
           uidjar.h \
           pop3uidjar.h \
           pop3client.h \
           imaputf7convertor.h \
           accountmanager.h
}

SOURCES += addressclass.cpp \
           addresslistclass.cpp \
           charsets.cpp \
           dateclass.cpp \
           headerclass.cpp \
           indexclass.cpp \
           mailfolder.cpp \
           md5.c \
           messageclass.cpp \
           messagedescriptor.cpp \
           messagedevice.cpp \
           messagefactory.cpp \
           mimecodec.cpp \
           mimepart.cpp \
           smtp.cpp \
           localmailfolder.cpp
           
enable_mailclients {           
SOURCES += defines.cpp \
           settings.cpp \
           qsettingsextra.cpp \          
           accounts.cpp \
           names.cpp \
           mailinfo.cpp \
           library.cpp \
           libraryloader.cpp \
           sslbase.cpp \
           sslsocket.cpp \
           ssldevice.cpp \
           uidjar.cpp \
           pop3uidjar.cpp \
           pop3client.cpp \
           imaputf7convertor.cpp \
           accountmanager.cpp
}
           
mac:SOURCES += $$ROOT/AQConfig.cpp
win32:SOURCES += $$ROOT/AQConfig.cpp

