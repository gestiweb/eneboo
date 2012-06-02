SOURCES	+= main.cpp
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
FORMS	= console.ui
TEMPLATE	=app

# Leave away .. in your own code. See comment in ../qsa.prf.
load( ../qsa )
