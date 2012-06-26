REQUIRES = table
SOURCES	+= main.cpp sheetinterface.cpp 
HEADERS	+= sheetinterface.h

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
FORMS	= spreadsheet.ui addscriptdialog.ui 
IMAGES	= images/filenew images/fileopen images/filesave images/print images/undo images/redo images/editcut images/editcopy images/editpaste images/searchfind images/hi22-action-project_open.png images/hi22-action-run.png 
TEMPLATE	=app

# Leave away .. in your own code. See comment in ../qsa.prf.
load( ../qsa )
