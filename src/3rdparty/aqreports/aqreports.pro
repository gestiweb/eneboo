# Este fichero no se utiliza para compilar
# Solo está por motivos de integración con Eclipse
# Para compilar hay que usar build.sh
TEMPLATE = lib
TARGET = aqreports
SOURCES += src/AQReports.cpp
HEADERS += src/AQJavaPrintService_p.h \
    src/AQJasperPrintManager_p.h \
    src/AQCompileFillManager_p.h \
    src/AQJasperFillManager_p.h \
    src/AQReports_p.h \
    AQReports.h
