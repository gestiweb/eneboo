TEMPLATE	= subdirs
no-png {
    message("Tools not available without PNG support")
} else {
    SUBDIRS		= assistant/lib \
		      designer \
		      linguist
}

CONFIG+=ordered
REQUIRES=full-config
