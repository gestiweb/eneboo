PRJ_MODULE      = Eneboo-project
exists(settings.pro) {
	include(settings.pro)
} else {
	error("settings.pro missing, unable to build")
}

TEMPLATE = subdirs 
CONFIG += warn_off 
SUBDIRS += src

