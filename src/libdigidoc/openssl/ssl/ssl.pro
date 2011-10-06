PRJ_MODULE      = OpenSSL
exists(../../../../settings.pro) {
	include(../../../../settings.pro)
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
	LIBS = -L$$PREFIX/lib -lpthreadGCE2 -lm
}

DESTDIR = $$PREFIX/lib

unix:DEFINES += OPENSSL_SYS_UNIX
win32:DEFINES += OPENSSL_SYS_WIN32

INCLUDEPATH = $$ROOT/src/libdigidoc/openssl $$ROOT/src/libdigidoc/openssl/ssl \
              $$ROOT/src/libdigidoc/openssl/include $$ROOT/src/libdigidoc/openssl/crypto
QMAKE_LIBS_X11SM =

LIBS = -L$$PREFIX/lib -lcrypto

TARGET = ssl

VERSION = 0.9.8

HEADERS += dtls1.h \
           kssl.h \
           kssl_lcl.h \
           ssl.h \
           ssl2.h \
           ssl23.h \
           ssl3.h \
           ssl_locl.h \
           tls1.h
SOURCES += s2_meth.c   s2_srvr.c s2_clnt.c  s2_lib.c  s2_enc.c s2_pkt.c \
        s3_meth.c   s3_srvr.c s3_clnt.c  s3_lib.c  s3_enc.c s3_pkt.c s3_both.c \
        s23_meth.c s23_srvr.c s23_clnt.c s23_lib.c          s23_pkt.c \
        t1_meth.c   t1_srvr.c t1_clnt.c  t1_lib.c  t1_enc.c \
        d1_meth.c   d1_srvr.c d1_clnt.c  d1_lib.c  d1_pkt.c \
        d1_both.c d1_enc.c \
        ssl_lib.c ssl_err2.c ssl_cert.c ssl_sess.c \
        ssl_ciph.c ssl_stat.c ssl_rsa.c \
        ssl_asn1.c ssl_txt.c ssl_algs.c \
        bio_ssl.c ssl_err.c kssl.c t1_reneg.c
           
