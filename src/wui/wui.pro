AQMODULE = AQUi
HEADERS = webgets/WQWidgetDialog.h \
    webgets/WQBoxLayout_p.h \
    webgets/WQBoxLayout.h \
    webgets/WQComboBox_p.h \
    webgets/WQComboBox.h \
    webgets/WQSpacer_p.h \
    webgets/WQSpacer.h \
    webgets/WQRadioButton_p.h \
    webgets/WQRadioButton.h \
    webgets/WQCheckBox_p.h \
    webgets/WQCheckBox.h \
    webgets/WQToolButton_p.h \
    webgets/WQToolButton.h \
    webgets/WQPushButton_p.h \
    webgets/WQPushButton.h \
    webgets/WQLabel_p.h \
    webgets/WQLabel.h \
    webgets/WQGridLayout_p.h \
    webgets/WQGridLayout.h \
    wtqt/AQLoadingIndicator.h \
    wtqt/AQDispatchThread.h \
    AQApplication_p.h \
    AQApplication.h \
    AQUi.h \
    ../gui/AQAction_p.h \
    ../gui/AQAction.h \
    ../gui/component/AQComponentDB.h \
    ../gui/component/AQFieldDB.h \
    ../gui/component/AQTableDB.h \
    ../gui/component/AQDataTable.h \
    ../gui/component/AQImageView.h \
    ../gui/component/AQValidator.h \
    ../gui/component/AQLineEdit.h \
    ../gui/mainwindow/AQ3MainWindow.h \
    ../gui/mainwindow/AQFormDB.h \
    ../gui/mainwindow/AQActionInfo.h \
    ../gui/mainwindow/AQReportViewer.h \
    ../gui/util/AQUiUtil.h \
    ../gui/dialog/AQAbout.h \
    ../gui/dialog/ui_AQ3About.h \
    mainwindow/AQMainWindow_p.h \
    mainwindow/AQTabView_p.h \
    mainwindow/AQTabView.h \
    mainwindow/AQMainWindow.h \
    dialog/AQConnectDB.h \
    webgets/WQTableView_p.h \
    webgets/WQTableView.h \
    webgets/WQTabWidget_p.h \
    webgets/WQTabWidget.h \
    webgets/WQGroupBox_p.h \
    webgets/WQGroupBox.h \
    webgets/WQLineEdit_p.h \
    webgets/WQLineEdit.h \
    webgets/WQFrame_p.h \
    webgets/WQFrame.h \
    webgets/WQTree_p.h \
    webgets/WQTree.h \
    webgets/WQIcon.h \
    webgets/WQMenu_p.h \
    webgets/WQWidget_p.h \
    webgets/WQAction.h \
    webgets/WQMenu.h \
    webgets/WQDialog.h \
    webgets/WQWidget.h \
    wtqt/WQEventLoop.h \
    wtqt/WQApplication.h \
    ../gui/manager/AQUiManager.h \
    ../gui/AQInvokerThreadContext.h
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
SOURCES = webgets/WQWidgetDialog.cpp \
    webgets/WQBoxLayout.cpp \
    webgets/WQComboBox.cpp \
    webgets/WQSpacer.cpp \
    webgets/WQRadioButton.cpp \
    webgets/WQCheckBox.cpp \
    webgets/WQToolButton.cpp \
    webgets/WQPushButton.cpp \
    webgets/WQLabel.cpp \
    webgets/WQGridLayout.cpp \
    wtqt/AQLoadingIndicator.cpp \
    wtqt/AQDispatchThread.cpp \
    AQApplication.cpp \
    ../gui/AQAction.cpp \
    ../gui/component/AQFieldDB.cpp \
    ../gui/component/AQTableDB.cpp \
    ../gui/component/AQDataTable.cpp \
    ../gui/component/AQImageView.cpp \
    ../gui/component/AQComponentDB.cpp \
    ../gui/component/AQValidator.cpp \
    ../gui/component/AQLineEdit.cpp \
    ../gui/mainwindow/AQ3MainWindow.cpp \
    ../gui/mainwindow/AQFormDB.cpp \
    ../gui/mainwindow/AQActionInfo.cpp \
    ../gui/mainwindow/AQReportViewer.cpp \
    ../gui/dialog/AQAbout.cpp \
    mainwindow/AQTabView.cpp \
    mainwindow/AQMainWindow.cpp \
    dialog/AQConnectDB.cpp \
    util/AQUiUtil.cpp \
    webgets/WQTableView.cpp \
    webgets/WQTabWidget.cpp \
    webgets/WQGroupBox.cpp \
    webgets/WQLineEdit.cpp \
    webgets/WQFrame.cpp \
    webgets/WQTree.cpp \
    webgets/WQIcon.cpp \
    webgets/WQAction.cpp \
    webgets/WQMenu.cpp \
    webgets/WQDialog.cpp \
    webgets/WQWidget.cpp \
    wtqt/WQEventLoop.cpp \
    wtqt/WQApplication.cpp \
    ../gui/manager/AQUiManager.cpp \
    ../gui/AQInvokerThreadContext.cpp
INSTALLS += target
LIBS += -lAQSql \
    -lwt \
    -lwthttp \
    -lwtext
include(../../includes.pri)
FORMS = ../gui/mainwindow/AQFormDB.ui \
    ../gui/component/AQTableDB.ui \
    ../gui/component/AQFieldDB.ui
FORMS3 = ../gui/dialog/AQ3About.ui
RESOURCES = wui.qrc
