HEADERS	+= $$IDEDIR/qsworkbench.h \
  	   $$IDEDIR/idewindow.ui.h

SOURCES	+= $$IDEDIR/qsworkbench.cpp

FORMS = $$IDEDIR/findtext.ui \
	$$IDEDIR/gotoline.ui \
	$$IDEDIR/idewindow.ui \
	$$IDEDIR/outputcontainer.ui \
	$$IDEDIR/projectcontainer.ui \
	$$IDEDIR/replacetext.ui \
	$$IDEDIR/preferencescontainer.ui

IMAGES	= $$IDEDIR/images/d_editcopy.png \
	$$IDEDIR/images/d_editcut.png \
	$$IDEDIR/images/d_editdelete.png \
	$$IDEDIR/images/d_editpaste.png \
	$$IDEDIR/images/d_fileopen.png \
	$$IDEDIR/images/d_filesave.png \
	$$IDEDIR/images/d_help.png \
	$$IDEDIR/images/d_play.png \
	$$IDEDIR/images/d_playprev.png \
	$$IDEDIR/images/d_redo.png \
	$$IDEDIR/images/d_searchfind.png \
	$$IDEDIR/images/d_undo.png \
	$$IDEDIR/images/d_stop.png \	
	$$IDEDIR/images/editcopy.png \
	$$IDEDIR/images/editcut.png \
	$$IDEDIR/images/editdelete.png \
	$$IDEDIR/images/editpaste.png \
	$$IDEDIR/images/scriptnew.png \
	$$IDEDIR/images/fileopen.png \
	$$IDEDIR/images/filesave.png \
	$$IDEDIR/images/help.png \
	$$IDEDIR/images/scriptobject.png \
	$$IDEDIR/images/play.png \
	$$IDEDIR/images/playprev.png \
	$$IDEDIR/images/redo.png \
	$$IDEDIR/images/searchfind.png \
	$$IDEDIR/images/undo.png \
	$$IDEDIR/images/script.png \
	$$IDEDIR/images/qsa.png \
	$$IDEDIR/images/splash.png \
	$$IDEDIR/images/stop.png

headers.files = $$headers.files $$IDEDIR/qsscripter.h
