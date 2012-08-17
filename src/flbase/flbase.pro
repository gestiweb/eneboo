PRJ_MODULE      = FLBase
exists(../../settings.pro):include(../../settings.pro)
else:error("settings.pro missing, unable to build")
TEMPLATE = lib
CONFIG += warn_off \
    create_prl \
    link_prl \
    qt
!mac:CONFIG += plugin
unix:CONFIG += x11
win32 { 
    CONFIG += dll
    DLLDESTDIR = $$PREFIX/bin
}
DESTDIR = $$PREFIX/lib
TARGET = flbase
INCLUDEPATH += $$ROOT/src/qt/src/kernel
INCLUDEPATH += $$ROOT \
    $$ROOT/src/flbase \
    $$ROOT/src/lrelease \
    $$ROOT/src/barcode \
    $$ROOT/src/kugar \
    $$ROOT/src/advance \
    $$ROOT/src/posprinter \
    $$ROOT/src/flmail \
    $$ROOT/src/serialport \
    $$ROOT/src

win32 { 
    INCLUDEPATH += $$ROOT/src/qsa/src/qsa
    INCLUDEPATH += $$ROOT/src/qsa/src/ide
}

mac { 
    INCLUDEPATH += $$ROOT/src/qsa/src/qsa
    INCLUDEPATH += $$ROOT/src/qsa/src/ide
}
enable_qwt:INCLUDEPATH += $$ROOT/src/qwt/src
enable_digidoc:INCLUDEPATH += $$ROOT/src/libdigidoc \
    $$ROOT/src/libdigidoc/libxml2 \
    $$ROOT/src/libdigidoc/libxml2/include \
    $$ROOT/src/libdigidoc/libxml2/libxml \
    $$ROOT/src/libdigidoc/openssl \
    $$ROOT/src/libdigidoc/openssl/ssl \
    $$ROOT/src/libdigidoc/openssl/include \
    $$ROOT/src/libdigidoc/openssl/crypto
DEPENDPATH += $$ROOT/src/lrelease \
    $$ROOT/src/barcode \
    $$ROOT/src/kugar \
    $$ROOT/src/advance \
    $$ROOT/src/flmail
LIBS += -L$$PREFIX/lib \
    -lqui \
    -lbarcode \
    -lkugar \
    -ladvance \
    -lflmail \
    -llrelease \
    -lxsltproc
mac:LIBS *= -ldl -lxml2 -lxslt -lexslt
enable_qwt:LIBS += -lqwt
enable_digidoc:LIBS += -llibdigidoc \
    -lcrypto
enable_digidoc:mac:LIBS += -lssl -lxml2
load(qsa)
VERSION = 2.4
include(../serialport/serialport.pri)
SOURCES += AQApplication.cpp \
    $$ROOT/AQConfig.cpp \
    FLAbout.cpp \
    FLApplication.cpp \
    FLCompoundKey.cpp \
    FLDataTable.cpp \
    FLFieldMetaData.cpp \
    FLFormDB.cpp \
    FLFormRecordDB.cpp \
    FLFormSearchDB.cpp \
    FLGroupByQuery.cpp \
    FLManager.cpp \
    FLParameterQuery.cpp \
    FLRelationMetaData.cpp \
    FLReportEngine.cpp \
    FLReportViewer.cpp \
    FLConnectDBDialog.cpp \
    FLSqlCursor.cpp \
    FLSqlQuery.cpp \
    FLTableMetaData.cpp \
    FLUtil.cpp \
    FLObjectFactory.cpp \
    FLFieldDB.cpp \
    FLTableDB.cpp \
    FLWidgetAction.cpp \
    FLCodBar.cpp \
    FLMemCache.cpp \
    FLTranslations.cpp \
    FLTranslator.cpp \
    FLManagerModules.cpp \
    FLScriptEditor.cpp \
    FLVar.cpp \
    FLSha1.cpp \
    FLDiskCache.cpp \
    FLSqlSavePoint.cpp \
    FLAccessControl.cpp \
    FLAccessControlLists.cpp \
    FLAccessControlFactory.cpp \
    FLSqlDatabase.cpp \
    FLSqlDriver.cpp \
    FLSqlConnections.cpp \
    FLSmtpClient.cpp \
    FLImageViewer.cpp \
    FLPicture.cpp \
    FLNetwork.cpp \
    FLStylePainter.cpp \
    FLTable.cpp \
    FLJasperViewer.cpp \
    FLJasperEngine.cpp \
    FLReportPages.cpp \
    FLModulesStaticLoader_p.cpp \
    FLSettings.cpp
SOURCES += FLDigiDoc.cpp
HEADERS += AQApplication.h \
    FLAbout.h \
    FLAction.h \
    FLApplication_p.h \
    FLCompoundKey.h \
    FLDataTable.h \
    FLFieldMetaData.h \
    FLFormDB.h \
    FLFormRecordDB.h \
    FLFormSearchDB.h \
    FLGroupByQuery.h \
    FLManager.h \
    FLParameterQuery.h \
    FLRelationMetaData.h \
    FLReportEngine.h \
    FLReportViewer.h \
    FLConnectDBDialog.h \
    FLSqlCursor.h \
    FLSqlQuery.h \
    FLTableMetaData.h \
    FLUtil.h \
    FLObjectFactory.h \
    FLFieldDB.h \
    FLTableDB.h \
    FLWidgetAction.h \
    FLCodBar.h \
    FLMemCache.h \
    FLTranslations.h \
    FLTranslator.h \
    FLManagerModules.h \
    FLScriptEditor.h \
    FLVar.h \
    FLSha1.h \
    FLDiskCache.h \
    FLSqlSavePoint.h \
    FLAccessControl.h \
    FLAccessControlLists.h \
    FLAccessControlFactory.h \
    FLSqlDatabase.h \
    FLSqlDriver.h \
    FLSqlConnections.h \
    FLSmtpClient.h \
    FLImageViewer.h \
    FLPicture.h \
    FLNetwork.h \
    FLStylePainter.h \
    FLTable.h \
    FLJasperViewer.h \
    FLJasperEngine.h \
    FLJasperEngine_p.h \
    FLReportPages.h \
    FLModulesStaticLoader_p.h \
    FLSettings.h
HEADERS += FLDigiDoc.h
FORMS += FLWidgetAbout.ui \
    FLWidgetConnectDB.ui \
    FLWidgetReportViewer.ui \
    FLWidgetFieldDB.ui \
    FLWidgetTableDB.ui \
    FLWidgetScriptEditor.ui \
    FLStaticLoaderUI.ui
IMAGES += images/infosial.png \
    images/about.png \
    images/cascada.png \
    images/exit.png \
    images/font.png \
    images/last.png \
    images/ok.png \
    images/remove.png \
    images/aboutqt.png \
    images/cerrar.png \
    images/first.png \
    images/help_index.png \
    images/mosaico.png \
    images/previous.png \
    images/button_ok.png \
    images/button_cancel.png \
    images/estilo.png \
    images/home.png \
    images/next.png \
    images/reload.png \
    images/date.png \
    images/bomba.png \
    images/filesaveas.png \
    images/logo_abanq.png \
    images/mini_logo_abanq.png \
    images/icono_abanq.png \
    images/bug.png \
    images/unlock.png \
    images/lock.png \
    images/file_open.png \
    images/eraser.png \
    images/locked.png \
    images/goto.png \
    images/settings.png \
    images/table.png \
    images/minus.png \
    images/plus.png \
    images/down.png \
    images/up.png \
    images/bottom.png \
    images/top.png \
    images/document-new.png \
    images/load.png \
    images/printer.png \
    images/folder.png \
    images/check_x_on.png \
    images/export.png \
    images/import.png \
    images/fileexport.png
include(aqsobjects/aqsobjects.pri)
include(../../tools/qsac/qsac.pri)
include(../../tools/packager/packager.pri)
include(../../tools/aqzip/aqzip.pri)
include(../../tools/aqods/aqods.pri)
exists(../../tools/roller/roller.pri):include(../../tools/roller/roller.pri)


