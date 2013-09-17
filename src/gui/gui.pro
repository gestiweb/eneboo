AQMODULE = AQUi
HEADERS = AQInvokerThreadContext.h \
    AQAction_p.h \
    AQAction.h \
    mainwindow/AQTreeWidget.h \
    mainwindow/AQActionInfo.h \
    component/AQDataTable.h \
    mainwindow/AQFormDB.h \
    mainwindow/AQTabView.h \
    mainwindow/AQ3MainWindow.h \
    component/AQConsole.h \
    component/AQComponentDB.h \
    component/AQValidator.h \
    component/AQLineEdit.h \
    component/AQImageView.h \
    mainwindow/AQMainWindow.h \
    AQApplication.h \
    AQUi.h \
    dialog/AQConnectDB.h \
    manager/AQUiManager.h \
    mainwindow/AQReportViewer.h \
    util/AQUiUtil.h \
    dialog/AQAbout.h \
    dialog/ui_AQ3About.h \
    component/AQFieldDB.h \
    component/AQTableDB.h
include (../../settings.pri)
TEMPLATE = lib
TARGET = AQUi
VERSION = 3.0
CONFIG += dll \
    uic3 \
    create_prl \
    designer
QT += gui \
    sql \
    qt3support
DLLDESTDIR = $$ABANQBLDDIR/lib
DESTDIR = $$ABANQBLDDIR/lib
target.path = $$ABANQINSDIR/lib
SOURCES = AQInvokerThreadContext.cpp \
    AQAction.cpp \
    mainwindow/AQTreeWidget.cpp \
    mainwindow/AQActionInfo.cpp \
    component/AQDataTable.cpp \
    mainwindow/AQFormDB.cpp \
    mainwindow/AQTabView.cpp \
    mainwindow/AQ3MainWindow.cpp \
    component/AQConsole.cpp \
    component/AQComponentDB.cpp \
    component/AQValidator.cpp \
    component/AQLineEdit.cpp \
    component/AQImageView.cpp \
    mainwindow/AQMainWindow.cpp \
    AQApplication.cpp \
    dialog/AQConnectDB.cpp \
    manager/AQUiManager.cpp \
    mainwindow/AQReportViewer.cpp \
    util/AQUiUtil.cpp \
    dialog/AQAbout.cpp \
    component/AQFieldDB.cpp \
    component/AQTableDB.cpp
INSTALLS += target
include(../../includes.pri)
FORMS = mainwindow/AQFormDB.ui \
    mainwindow/AQTabView.ui \
    component/AQConsole.ui \
    component/AQTableDB.ui \
    component/AQFieldDB.ui \
    mainwindow/AQMainWindow.ui \
    dialog/AQConnectDB.ui
FORMS3 = dialog/AQ3About.ui
RESOURCES = gui.qrc
