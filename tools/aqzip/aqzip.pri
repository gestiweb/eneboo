AQZIP_DIR = $$ROOT/tools/aqzip

INCLUDEPATH += $$AQZIP_DIR
INCLUDEPATH += $$ROOT/src/qt/src/3rdparty/zlib

HEADERS += $$AQZIP_DIR/AQZipReader.h $$AQZIP_DIR/AQZipWriter.h $$AQZIP_DIR/qscopedpointer.h

SOURCES += $$AQZIP_DIR/AQZip.cpp
