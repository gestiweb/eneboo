AQODS_DIR = $$ROOT/tools/aqods

INCLUDEPATH += $$AQODS_DIR $$AQODS_DIR/odf-gen

HEADERS += $$AQODS_DIR/AQOds.h \
           $$AQODS_DIR/AQOdsCellAddress.h \
           $$AQODS_DIR/AQOdsChart.h \
           $$AQODS_DIR/AQOdsColor.h \
           $$AQODS_DIR/AQOdsGenerator.h \
           $$AQODS_DIR/AQOdsImage.h \
           $$AQODS_DIR/AQOdsLength.h \
           $$AQODS_DIR/AQOdsRow.h \
           $$AQODS_DIR/AQOdsSheet.h \
           $$AQODS_DIR/AQOdsSpreadSheet.h \
           $$AQODS_DIR/AQOdsStyle.h \
           $$AQODS_DIR/odf-gen/celladdress.h \
           $$AQODS_DIR/odf-gen/chart.h \
           $$AQODS_DIR/odf-gen/color.h \
           $$AQODS_DIR/odf-gen/element.h \
           $$AQODS_DIR/odf-gen/image.h \
           $$AQODS_DIR/odf-gen/length.h \
           $$AQODS_DIR/odf-gen/odfgen.h \
           $$AQODS_DIR/odf-gen/odsgenerator.h \
           $$AQODS_DIR/odf-gen/odstype.h \
           $$AQODS_DIR/odf-gen/row.h \
           $$AQODS_DIR/odf-gen/sheet.h \
           $$AQODS_DIR/odf-gen/span.h \
           $$AQODS_DIR/odf-gen/spreadsheet.h \
           $$AQODS_DIR/odf-gen/style.h

SOURCES += $$AQODS_DIR/AQOdsGenerator.cpp