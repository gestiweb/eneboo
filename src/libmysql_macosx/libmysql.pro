PRJ_MODULE      = LibMySQL-MacOSX
exists(../../settings.pro) {
	include(../../settings.pro)
}
else {
	error("settings.pro missing, unable to build")
}

TEMPLATE = lib
CONFIG += warn_off create_prl
!mac:CONFIG += plugin
CONFIG -= qt x11

unix {
	DEFINES += DEFAULT_CHARSET_HOME=\"/usr/share/mysql/charsets\" DATADIR=\"$$PREFIX/share/abanq\" SHAREDIR=\"$$PREFIX/share/abanq\"
}
win32 {
	CONFIG += dll
	DLLDESTDIR = $$PREFIX/bin
	LIBS += -L$$PREFIX/lib -lqt-mt -lwsock32
	SOURCES += dll.c my_access.c my_winthread.c my_wincond.c
	DEFINES -= UNICODE
}

macx {
	DEFINES += UNDEF_THREADS_HACK HAVE_DARWIN_THREADS _P1003_1B_VISIBLE SIGNAL_WITH_VIO_CLOSE SIGNALS_DONT_BREAK_READ IGNORE_SIGHUP_SIGQUIT
	LIBS += -L$$PREFIX/lib -lz
	QMAKE_CFLAGS += -fno-common
	QMAKE_CXXFLAGS += -fno-common
	QMAKE_LFLAGS += -fno-common
}

DESTDIR = $$PREFIX/lib

DEFINES += DONT_USE_RAID MYSQL_CLIENT DBUG_OFF THREAD _POSIX_PTHREAD_SEMANTICS _THREAD_SAFE

TARGET = mysqlclient

INCLUDEPATH = $$ROOT/src/libmysql/include $$ROOT/src/qt/src/3rdparty/zlib 

VERSION = 15

DEPENDPATH += . include
INCLUDEPATH += . include

SOURCES += array.c \
	bchange.c \
	bcmp.c \
	bmove.c \
	bmove_upp.c \
	charset.c \
	charset-def.c \
	client.c \
	ctype-big5.c \
	ctype-bin.c \
	ctype.c \
	ctype-cp932.c \
	ctype-czech.c \
	ctype-euc_kr.c \
	ctype-extra.c \
	ctype-gb2312.c \
	ctype-gbk.c \
	ctype-latin1.c \
	ctype-mb.c \
	ctype-simple.c \
	ctype-sjis.c \
	ctype-tis620.c \
	ctype-uca.c \
	ctype-ucs2.c \
	ctype-ujis.c \
	ctype-utf8.c \
	ctype-win1250ch.c \
	dbug.c \
	default.c \
	errmsg.c \
	errors.c \
	get_password.c \
	hash.c \
	int2str.c \
	is_prefix.c \
	libmysql.c \
	list.c \
	llstr.c \
	longlong2str.c \
	manager.c \
	md5.c \
	mf_cache.c \
	mf_dirname.c \
	mf_fn_ext.c \
	mf_format.c \
	mf_iocache2.c \
	mf_iocache.c \
	mf_loadpath.c \
	mf_pack.c \
	mf_path.c \
	mf_tempfile.c \
	mf_unixpath.c \
	mf_wcomp.c \
	mulalloc.c \
	my_alloc.c \
	my_compress.c \
	my_create.c \
	my_delete.c \
	my_div.c \
	my_error.c \
	my_file.c \
	my_fopen.c \
	my_fstream.c \
	my_gethostbyname.c \
	my_getopt.c \
	my_getwd.c \
	my_init.c \
	my_lib.c \
	my_malloc.c \
	my_messnc.c \
	my_net.c \
	my_once.c \
	my_open.c \
	my_port.c \
	my_pread.c \
	my_pthread.c \
	my_read.c \
	my_realloc.c \
	my_seek.c \
	my_sleep.c \
	my_static.c \
	my_strtoll10.c \
	my_symlink.c \
	my_thr_init.c \
	my_time.c \
	my_vsnprintf.c \
	my_write.c \
	net.c \
	pack.c \
	password.c \
	safemalloc.c \
	sha1.c \
	str2int.c \
	strcend.c \
	strcont.c \
	strend.c \
	strfill.c \
	string.c \
	strinstr.c \
	strmake.c \
	strmov.c \
	strnlen.c \
	strnmov.c \
	strtod.c \
	strtoll.c \
	strtoull.c \
	strxmov.c \
	strxnmov.c \
	thr_mutex.c \
	typelib.c \
	vio.c \
	viosocket.c \
	viossl.c \
	viosslfactories.c \
	xml.c

HEADERS += client_settings.h \
	my_static.h \
	mysys_priv.h \
	vio_priv.h
	
