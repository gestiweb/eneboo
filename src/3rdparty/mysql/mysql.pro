include(../../../settings.pri)

TEMPLATE = lib
TARGET = mysqlclient_r
VERSION = 14

QT -= gui core

DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
INSTALLS += target
CONFIG += dll create_prl link_prl

DEFINES += DONT_USE_RAID MYSQL_CLIENT DBUG_OFF THREAD
DEFINES += DEFAULT_CHARSET_HOME=\'\""$$ABANQINSDIR"\"\' DATADIR=\'\""$$ABANQINSDIR"\"\' SHAREDIR=\'\""$$ABANQINSDIR"\"\'

LIBS += -lcrypt -lresolv -lnsl

INCLUDEPATH += ./include

HEADERS = client_settings.h \
    include/config-netware.h \
    include/config-os2.h \
    include/config-win.h \
    include/errmsg.h \
    include/ft_global.h \
    include/hash.h \
    include/heap.h \
    include/help_end.h \
    include/help_start.h \
    include/keycache.h \
    include/m_ctype.h \
    include/m_string.h \
    include/md5.h \
    include/merge.h \
    include/my_aes.h \
    include/my_alarm.h \
    include/my_alloc.h \
    include/my_base.h \
    include/my_bitmap.h \
    include/my_config.h \
    include/my_dbug.h \
    include/my_dir.h \
    include/my_getopt.h \
    include/my_global.h \
    include/my_handler.h \
    include/my_list.h \
    include/my_net.h \
    include/my_no_pthread.h \
    include/my_nosys.h \
    include/my_pthread.h \
    include/my_semaphore.h \
    include/my_sys.h \
    include/my_time.h \
    include/my_tree.h \
    include/my_xml.h \
    include/myisam.h \
    include/myisammrg.h \
    include/myisampack.h \
    include/mysql.h \
    include/mysql_com.h \
    include/mysql_embed.h \
    include/mysql_time.h \
    include/mysql_version.h \
    include/mysqld_error.h \
    include/mysys_err.h \
    include/nisam.h \
    include/queues.h \
    include/raid.h \
    include/rijndael.h \
    include/sha1.h \
    include/sql_common.h \
    include/sql_state.h \
    include/sslopt-case.h \
    include/sslopt-longopts.h \
    include/sslopt-vars.h \
    include/t_ctype.h \
    include/thr_alarm.h \
    include/thr_lock.h \
    include/typelib.h \
    include/violite.h \
    my_static.h \
    mysys_priv.h \
    vio_priv.h
SOURCES = array.c \
    bchange.c \
    bcmp.c \
    bmove.c \
    bmove_upp.c \
    charset-def.c \
    charset.c \
    client.c \
    ctype-big5.c \
    ctype-bin.c \
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
    ctype.c \
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
    mf_iocache.c \
    mf_iocache2.c \
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
    strto.c \
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

include(../../../includes.pri)
