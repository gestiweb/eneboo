PRJ_MODULE      = Eneboo-src
exists(../settings.pro) {
  include(../settings.pro)
}
else {
  error("settings.pro missing, unable to build")
}

TEMPLATE = subdirs

CONFIG *= warn_off
CONFIG *= ordered


enable_hoard:SUBDIRS += hoard
enable_qwt:SUBDIRS += qwt
enable_digidoc:SUBDIRS += libdigidoc/openssl/crypto libdigidoc/openssl/ssl libdigidoc/libxml2 libdigidoc

SUBDIRS += libxslt
SUBDIRS += lrelease barcode kugar advance flmail flbase libpq sqlite libmysql plugins dbf kudesigner teddy fllite
unix:SUBDIRS += flfcgi

