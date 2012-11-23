/****************************************************************************
** Form interface generated from reading ui file 'mainform.ui'
**
** Created: jue nov 22 22:25:22 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.8   edited Jan 11 14:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef FMMAIN_H
#define FMMAIN_H

#include <qvariant.h>
#include <qmainwindow.h>
#include <qapplication.h>
#include <qspinbox.h>
#include <qinputdialog.h>
#include <qfiledialog.h>
#include <qfile.h>
#include <qdom.h>
#include "canvbox.h"
#include "mycanvas.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class ReportCanvas;
class QCanvas;
class dlgNew;
class QRadioButton;
class QMessageBox;
class QPrinter;
class QCombobox;
class QPaintDeviceMetrics;
class CanvasReportItem;
class CanvasLabel;
class CanvasField;
class CanvasSpecialField;
class CanvasCalculatedField;
class CanvasLine;
class CanvasReportHeader;
class CanvasReportFooter;
class CanvasPageFooter;
class CanvasPageHeader;
class CanvasDetailFooter;
class CanvasDetailHeader;
class CanvasDetail;
class QPainter;

class fmMain : public QMainWindow
{
    Q_OBJECT

public:
    fmMain( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~fmMain();

    QMenuBar *menubar;
    QPopupMenu *fileMenu;
    QPopupMenu *PopupMenu;
    QPopupMenu *PopupMenu_2;
    QPopupMenu *helpMenu;
    QToolBar *toolBar;
    QToolBar *Toolbar_2;
    QToolBar *Toolbar;
    QAction* fileNewAction;
    QAction* fileOpenAction;
    QAction* fileSaveAction;
    QAction* fileSaveAsAction;
    QAction* filePrintAction;
    QAction* fileExitAction;
    QAction* editUndoAction;
    QAction* editRedoAction;
    QAction* editCutAction;
    QAction* editCopyAction;
    QAction* editPasteAction;
    QAction* editFindAction;
    QAction* helpContentsAction;
    QAction* helpIndexAction;
    QAction* helpAboutAction;
    QAction* helpAboutQTAction;
    QAction* reportAddReportHeader;
    QAction* reportAddPageHeader;
    QAction* reportAddDetailHeader;
    QAction* reportAddDetail;
    QAction* reportAddDetailFooter;
    QAction* reportAddPageFooter;
    QAction* reportAddReportFooter;
    QActionGroup* AddReportSections;
    QAction* reportAddNothing;
    QAction* reportAddLine;
    QAction* reportAddCalculatedField;
    QAction* reportAddSpecialField;
    QAction* reportAddField;
    QAction* addReportLabel;

public slots:
    virtual void destroy();
    virtual void init();
    virtual void fileNew();
    virtual void setReportAttributes( QDomNode * report, QString fName );
    virtual void setReportItemAttributes( QDomNode * node, CanvasReportItem * item );
    virtual void addReportItems( QDomNode * node, CanvasBand * section );
    virtual void setReportHeaderAttributes( QDomNode * node );
    virtual void setReportFooterAttributes( QDomNode * node );
    virtual void setPageHeaderAttributes( QDomNode * node );
    virtual void setPageFooterAttributes( QDomNode * node );
    virtual void setDetailHeaderAttributes( QDomNode * node );
    virtual void setDetailAttributes( QDomNode * node );
    virtual void setDetailFooterAttributes( QDomNode * node );
    virtual void fileOpen();
    virtual void fileSave();
    virtual void fileSaveAs();
    virtual void filePrint();
    virtual void fileExit();
    virtual void editUndo();
    virtual void editRedo();
    virtual void editCut();
    virtual void editCopy();
    virtual void editPaste();
    virtual void editFind();
    virtual void helpIndex();
    virtual void helpContents();
    virtual void helpAbout();
    virtual void helpAboutQT();
    virtual void addDetail();
    virtual void addDetailFooter();
    virtual void addDetailHeader();
    virtual void addPageFooter();
    virtual void addPageHeader();
    virtual void addReportFooter();
    virtual void addReportHeader();
    virtual void toggleLabel( bool t );
    virtual void toggleLine( bool t );
    virtual void toggleCalculated( bool t );
    virtual void toggleSpecial( bool t );
    virtual void toggleField( bool t );
    virtual void toggleNothing( bool t );
    virtual void saveReport();
    virtual void fileOpen( QString name );

protected:
    MyCanvas *canvas;
    ReportCanvas *rc;


protected slots:
    virtual void languageChange();

};

#endif // FMMAIN_H
