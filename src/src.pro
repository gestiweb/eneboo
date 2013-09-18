include(../gen_settings.pri)

TEMPLATE = subdirs

SUBDIRS += 3rdparty corelib acl xml sql 
SUBDIRS += $$AQUISRC_DIR
SUBDIRS += network plugins script abanq script/qsac
