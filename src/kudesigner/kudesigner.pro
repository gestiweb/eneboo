exists(../../settings.pro) {
	include(../../settings.pro)
}
else {
        error("settings.pro missing, unable to build")
}
PRJ_MODULE = kudesigner
TEMPLATE = app

CONFIG += warn_off qt link_prl
unix:CONFIG += x11
CONFIG -= dll

DESTDIR = $$PREFIX/bin

win32:RC_FILE = kudesigner.rc
mac:RC_FILE = kugar.icns

TARGET = kudesigner

INCLUDEPATH += $$ROOT/src/kudesigner

LIBS += -L$$PREFIX/lib

SOURCES += canvbox.cpp \
           ccalcfield.cpp \
           cfield.cpp \
           clabel.cpp \
           cline.cpp \
           creportitem.cpp \
           cspecialfield.cpp \
           cv.cpp \
           itemopts.cpp \
           main.cpp \
           mycanvas.cpp 
HEADERS += canvbox.h \
           canvdefs.h \
           ccalcfield.h \
           cfield.h \
           clabel.h \
           cline.h \
           creportitem.h \
           cspecialfield.h \
           cv.h \
           dlgnew.ui.h \
           itemopts.h \
           mainform.ui.h \
           mycanvas.h 
IMAGES += images/fileopen \
          images/filesave \
          images/print \
          images/undo \
          images/redo \
          images/editcut \
          images/editcopy \
          images/editpaste \
          images/searchfind \
          images/font_names.png \
          images/frame_text.png \
          images/insert_table_col.png \
          images/frame_chart.png \
          images/frame_edit.png \
          images/frame_formula.png \
          images/frame_image.png \
          images/frame_query.png \
          images/frame_text.png \
          images/insert_table_col.png \
          images/insert_table_row.png \
          images/i-regular-36-aa.png \
          images/arrow.png \
          images/frame_edit.png \
          images/frame_field.png \
          images/i-regular-36-bb.png \
          images/i-d.png \
          images/i-df.png \
          images/i-dh.png \
          images/i-pf.png \
          images/i-ph.png \
          images/i-rh.png \
          images/i-rf.png 
FORMS += dlgnew.ui \
         dlgoptions.ui \
         mainform.ui 
 

