PRJ_MODULE      = LibXSLT

exists(../../settings.pro) {
  include(../../settings.pro)
}
else {
  error("settings.pro missing, unable to build")
}

TEMPLATE = subdirs

CONFIG *= warn_off
CONFIG *= ordered

SUBDIRS += libxslt libexslt libxsltproc


