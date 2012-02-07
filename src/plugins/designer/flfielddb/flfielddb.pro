exists(../../../../settings.pro) {
        include(../../../../settings.pro)
}
else {
	error("settings.pro missing, unable to build")
}

TEMPLATE = lib 
CONFIG += warn_off plugin 
DESTDIR = $$PREFIX/plugins/designer
TARGET = flfielddb 
INCLUDEPATH += $$ROOT $$ROOT/src/flbase
SOURCES += main.cpp 
mac {
load(qsa)
LIBS += -L$$PREFIX/lib -lflbase -ladvance -lqui -lflmail -lqwt
enable_digidoc:LIBS += -llibdigidoc -lcrypto -lssl -lxml2
LIBS += -lxslt -lexslt -lxsltproc
} 
else {
LIBS += -L$$PREFIX/lib -lflbase
}
IMAGES += images/dataline.png
