include(../gen_settings.pri)

TEMPLATE = subdirs

SUBDIRS += 3rdparty corelib acl xml script sql 
SUBDIRS += $$AQUISRC_DIR
SUBDIRS += network plugins abanq script/qsac
