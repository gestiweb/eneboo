TEMPLATE = app
INCLUDEPATH += .

# Input
HEADERS += playfield.h
INTERFACES += gamewindow.ui
SOURCES += main.cpp playfield.cpp

# Leave away .. in your own code. See comment in ../qsa.prf.
load( ../qsa )
