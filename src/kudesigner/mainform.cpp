/****************************************************************************
** Form implementation generated from reading ui file 'mainform.ui'
**
** Created: jue nov 22 22:25:30 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.8   edited Jan 11 14:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "mainform.h"

#include <qvariant.h>
#include <qpainter.h>
#include <qstatusbar.h>
#include <dlgnew.h>
#include <qdialog.h>
#include <qradiobutton.h>
#include <qmessagebox.h>
#include <qprinter.h>
#include <qpaintdevicemetrics.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>

#include "cv.h"
#include "creportitem.h"
#include "clabel.h"
#include "cspecialfield.h"
#include "cfield.h"
#include "ccalcfield.h"
#include "cline.h"
#include "mainform.ui.h"
/*
 *  Constructs a fmMain as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
fmMain::fmMain( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    if ( !name )
	setName( "fmMain" );
    setCursor( QCursor( 0 ) );

    // actions
    fileNewAction = new QAction( this, "fileNewAction" );
    fileNewAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "i-regular-36-aa.png" ) ) );
    fileOpenAction = new QAction( this, "fileOpenAction" );
    fileOpenAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "fileopen" ) ) );
    fileSaveAction = new QAction( this, "fileSaveAction" );
    fileSaveAction->setEnabled( FALSE );
    fileSaveAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "filesave" ) ) );
    fileSaveAsAction = new QAction( this, "fileSaveAsAction" );
    fileSaveAsAction->setEnabled( FALSE );
    filePrintAction = new QAction( this, "filePrintAction" );
    filePrintAction->setEnabled( FALSE );
    filePrintAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "print" ) ) );
    fileExitAction = new QAction( this, "fileExitAction" );
    editUndoAction = new QAction( this, "editUndoAction" );
    editUndoAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "undo" ) ) );
    editRedoAction = new QAction( this, "editRedoAction" );
    editRedoAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "redo" ) ) );
    editCutAction = new QAction( this, "editCutAction" );
    editCutAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "editcut" ) ) );
    editCopyAction = new QAction( this, "editCopyAction" );
    editCopyAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "editcopy" ) ) );
    editPasteAction = new QAction( this, "editPasteAction" );
    editPasteAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "editpaste" ) ) );
    editFindAction = new QAction( this, "editFindAction" );
    editFindAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "searchfind" ) ) );
    helpContentsAction = new QAction( this, "helpContentsAction" );
    helpIndexAction = new QAction( this, "helpIndexAction" );
    helpAboutAction = new QAction( this, "helpAboutAction" );
    helpAboutQTAction = new QAction( this, "helpAboutQTAction" );
    reportAddReportHeader = new QAction( this, "reportAddReportHeader" );
    reportAddReportHeader->setEnabled( FALSE );
    reportAddReportHeader->setIconSet( QIconSet( QPixmap::fromMimeSource( "i-rh.png" ) ) );
    reportAddPageHeader = new QAction( this, "reportAddPageHeader" );
    reportAddPageHeader->setEnabled( FALSE );
    reportAddPageHeader->setIconSet( QIconSet( QPixmap::fromMimeSource( "i-ph.png" ) ) );
    reportAddDetailHeader = new QAction( this, "reportAddDetailHeader" );
    reportAddDetailHeader->setEnabled( FALSE );
    reportAddDetailHeader->setIconSet( QIconSet( QPixmap::fromMimeSource( "i-dh.png" ) ) );
    reportAddDetail = new QAction( this, "reportAddDetail" );
    reportAddDetail->setEnabled( FALSE );
    reportAddDetail->setIconSet( QIconSet( QPixmap::fromMimeSource( "i-d.png" ) ) );
    reportAddDetailFooter = new QAction( this, "reportAddDetailFooter" );
    reportAddDetailFooter->setEnabled( FALSE );
    reportAddDetailFooter->setIconSet( QIconSet( QPixmap::fromMimeSource( "i-df.png" ) ) );
    reportAddPageFooter = new QAction( this, "reportAddPageFooter" );
    reportAddPageFooter->setEnabled( FALSE );
    reportAddPageFooter->setIconSet( QIconSet( QPixmap::fromMimeSource( "i-pf.png" ) ) );
    reportAddReportFooter = new QAction( this, "reportAddReportFooter" );
    reportAddReportFooter->setEnabled( FALSE );
    reportAddReportFooter->setIconSet( QIconSet( QPixmap::fromMimeSource( "i-rf.png" ) ) );
    AddReportSections = new QActionGroup( this, "AddReportSections" );
    AddReportSections->setEnabled( FALSE );
    AddReportSections->setUsesDropDown( FALSE );
    reportAddNothing = new QAction( AddReportSections, "reportAddNothing" );
    reportAddNothing->setToggleAction( TRUE );
    reportAddNothing->setOn( TRUE );
    reportAddNothing->setIconSet( QIconSet( QPixmap::fromMimeSource( "frame_edit.png" ) ) );
    reportAddLine = new QAction( AddReportSections, "reportAddLine" );
    reportAddLine->setToggleAction( TRUE );
    reportAddLine->setIconSet( QIconSet( QPixmap::fromMimeSource( "frame_chart.png" ) ) );
    reportAddCalculatedField = new QAction( AddReportSections, "reportAddCalculatedField" );
    reportAddCalculatedField->setToggleAction( TRUE );
    reportAddCalculatedField->setIconSet( QIconSet( QPixmap::fromMimeSource( "frame_formula.png" ) ) );
    reportAddSpecialField = new QAction( AddReportSections, "reportAddSpecialField" );
    reportAddSpecialField->setToggleAction( TRUE );
    reportAddSpecialField->setIconSet( QIconSet( QPixmap::fromMimeSource( "frame_query.png" ) ) );
    reportAddField = new QAction( AddReportSections, "reportAddField" );
    reportAddField->setToggleAction( TRUE );
    reportAddField->setIconSet( QIconSet( QPixmap::fromMimeSource( "frame_field.png" ) ) );
    addReportLabel = new QAction( AddReportSections, "addReportLabel" );
    addReportLabel->setToggleAction( TRUE );
    addReportLabel->setIconSet( QIconSet( QPixmap::fromMimeSource( "frame_text.png" ) ) );


    // toolbars
    toolBar = new QToolBar( QString(""), this, DockTop ); 

    fileNewAction->addTo( toolBar );
    fileOpenAction->addTo( toolBar );
    fileSaveAction->addTo( toolBar );
    toolBar->addSeparator();
    filePrintAction->addTo( toolBar );
    Toolbar_2 = new QToolBar( QString(""), this, DockBottom ); 

    reportAddReportHeader->addTo( Toolbar_2 );
    reportAddPageHeader->addTo( Toolbar_2 );
    reportAddDetailHeader->addTo( Toolbar_2 );
    reportAddDetail->addTo( Toolbar_2 );
    reportAddDetailFooter->addTo( Toolbar_2 );
    reportAddPageFooter->addTo( Toolbar_2 );
    reportAddReportFooter->addTo( Toolbar_2 );
    Toolbar = new QToolBar( QString(""), this, DockLeft ); 

    reportAddNothing->addTo( Toolbar );
    Toolbar->addSeparator();
    addReportLabel->addTo( Toolbar );
    reportAddField->addTo( Toolbar );
    reportAddSpecialField->addTo( Toolbar );
    reportAddCalculatedField->addTo( Toolbar );
    Toolbar->addSeparator();
    reportAddLine->addTo( Toolbar );


    // menubar
    menubar = new QMenuBar( this, "menubar" );


    fileMenu = new QPopupMenu( this );
    fileNewAction->addTo( fileMenu );
    fileOpenAction->addTo( fileMenu );
    fileSaveAction->addTo( fileMenu );
    fileSaveAsAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    filePrintAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileExitAction->addTo( fileMenu );
    menubar->insertItem( QString(""), fileMenu, 1 );

    PopupMenu = new QPopupMenu( this );
    reportAddReportHeader->addTo( PopupMenu );
    reportAddPageHeader->addTo( PopupMenu );
    reportAddDetailHeader->addTo( PopupMenu );
    reportAddDetail->addTo( PopupMenu );
    reportAddDetailFooter->addTo( PopupMenu );
    reportAddPageFooter->addTo( PopupMenu );
    reportAddReportFooter->addTo( PopupMenu );
    menubar->insertItem( QString(""), PopupMenu, 2 );

    PopupMenu_2 = new QPopupMenu( this );
    reportAddNothing->addTo( PopupMenu_2 );
    PopupMenu_2->insertSeparator();
    addReportLabel->addTo( PopupMenu_2 );
    reportAddField->addTo( PopupMenu_2 );
    reportAddSpecialField->addTo( PopupMenu_2 );
    reportAddCalculatedField->addTo( PopupMenu_2 );
    PopupMenu_2->insertSeparator();
    reportAddLine->addTo( PopupMenu_2 );
    menubar->insertItem( QString(""), PopupMenu_2, 3 );

    helpMenu = new QPopupMenu( this );
    helpContentsAction->addTo( helpMenu );
    helpIndexAction->addTo( helpMenu );
    helpMenu->insertSeparator();
    helpAboutAction->addTo( helpMenu );
    helpAboutQTAction->addTo( helpMenu );
    menubar->insertItem( QString(""), helpMenu, 4 );

    languageChange();
    resize( QSize(553, 353).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( fileNewAction, SIGNAL( activated() ), this, SLOT( fileNew() ) );
    connect( fileSaveAction, SIGNAL( activated() ), this, SLOT( fileSave() ) );
    connect( fileSaveAsAction, SIGNAL( activated() ), this, SLOT( fileSaveAs() ) );
    connect( filePrintAction, SIGNAL( activated() ), this, SLOT( filePrint() ) );
    connect( fileExitAction, SIGNAL( activated() ), this, SLOT( fileExit() ) );
    connect( editUndoAction, SIGNAL( activated() ), this, SLOT( editUndo() ) );
    connect( editRedoAction, SIGNAL( activated() ), this, SLOT( editRedo() ) );
    connect( editCutAction, SIGNAL( activated() ), this, SLOT( editCut() ) );
    connect( editCopyAction, SIGNAL( activated() ), this, SLOT( editCopy() ) );
    connect( editPasteAction, SIGNAL( activated() ), this, SLOT( editPaste() ) );
    connect( editFindAction, SIGNAL( activated() ), this, SLOT( editFind() ) );
    connect( helpIndexAction, SIGNAL( activated() ), this, SLOT( helpIndex() ) );
    connect( helpContentsAction, SIGNAL( activated() ), this, SLOT( helpContents() ) );
    connect( helpAboutAction, SIGNAL( activated() ), this, SLOT( helpAbout() ) );
    connect( helpAboutQTAction, SIGNAL( activated() ), this, SLOT( helpAboutQT() ) );
    connect( reportAddReportFooter, SIGNAL( activated() ), this, SLOT( addReportFooter() ) );
    connect( reportAddPageFooter, SIGNAL( activated() ), this, SLOT( addPageFooter() ) );
    connect( reportAddDetailFooter, SIGNAL( activated() ), this, SLOT( addDetailFooter() ) );
    connect( reportAddDetail, SIGNAL( activated() ), this, SLOT( addDetail() ) );
    connect( reportAddDetailHeader, SIGNAL( activated() ), this, SLOT( addDetailHeader() ) );
    connect( reportAddPageHeader, SIGNAL( activated() ), this, SLOT( addPageHeader() ) );
    connect( reportAddReportHeader, SIGNAL( activated() ), this, SLOT( addReportHeader() ) );
    connect( addReportLabel, SIGNAL( toggled(bool) ), this, SLOT( toggleLabel(bool) ) );
    connect( reportAddLine, SIGNAL( toggled(bool) ), this, SLOT( toggleLine(bool) ) );
    connect( reportAddCalculatedField, SIGNAL( toggled(bool) ), this, SLOT( toggleCalculated(bool) ) );
    connect( reportAddSpecialField, SIGNAL( toggled(bool) ), this, SLOT( toggleSpecial(bool) ) );
    connect( reportAddField, SIGNAL( toggled(bool) ), this, SLOT( toggleField(bool) ) );
    connect( reportAddNothing, SIGNAL( toggled(bool) ), this, SLOT( toggleNothing(bool) ) );
    connect( fileOpenAction, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
fmMain::~fmMain()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void fmMain::languageChange()
{
    setCaption( tr( "Report Designer" ) );
    fileNewAction->setText( tr( "New" ) );
    fileNewAction->setMenuText( tr( "&New" ) );
    fileNewAction->setAccel( tr( "Ctrl+N" ) );
    fileOpenAction->setText( tr( "Open" ) );
    fileOpenAction->setMenuText( tr( "&Open..." ) );
    fileOpenAction->setAccel( tr( "Ctrl+O" ) );
    fileSaveAction->setText( tr( "Save" ) );
    fileSaveAction->setMenuText( tr( "&Save" ) );
    fileSaveAction->setAccel( tr( "Ctrl+S" ) );
    fileSaveAsAction->setText( tr( "Save As" ) );
    fileSaveAsAction->setMenuText( tr( "Save &As..." ) );
    filePrintAction->setText( tr( "Print" ) );
    filePrintAction->setMenuText( tr( "&Print..." ) );
    filePrintAction->setAccel( tr( "Ctrl+P" ) );
    fileExitAction->setText( tr( "Exit" ) );
    fileExitAction->setMenuText( tr( "E&xit" ) );
    editUndoAction->setText( tr( "Undo" ) );
    editUndoAction->setMenuText( tr( "&Undo" ) );
    editUndoAction->setAccel( tr( "Ctrl+Z" ) );
    editRedoAction->setText( tr( "Redo" ) );
    editRedoAction->setMenuText( tr( "&Redo" ) );
    editRedoAction->setAccel( tr( "Ctrl+Y" ) );
    editCutAction->setText( tr( "Cut" ) );
    editCutAction->setMenuText( tr( "&Cut" ) );
    editCutAction->setAccel( tr( "Ctrl+X" ) );
    editCopyAction->setText( tr( "Copy" ) );
    editCopyAction->setMenuText( tr( "C&opy" ) );
    editCopyAction->setAccel( tr( "Ctrl+C" ) );
    editPasteAction->setText( tr( "Paste" ) );
    editPasteAction->setMenuText( tr( "&Paste" ) );
    editPasteAction->setAccel( tr( "Ctrl+V" ) );
    editFindAction->setText( tr( "Find" ) );
    editFindAction->setMenuText( tr( "&Find..." ) );
    editFindAction->setAccel( tr( "Ctrl+F" ) );
    helpContentsAction->setText( tr( "Contents" ) );
    helpContentsAction->setMenuText( tr( "&Contents..." ) );
    helpIndexAction->setText( tr( "Index" ) );
    helpIndexAction->setMenuText( tr( "&Index..." ) );
    helpAboutAction->setText( tr( "About" ) );
    helpAboutAction->setMenuText( tr( "&About..." ) );
    helpAboutQTAction->setText( tr( "About QT" ) );
    helpAboutQTAction->setMenuText( tr( "&About QT..." ) );
    reportAddReportHeader->setText( tr( "Add Report Header" ) );
    reportAddReportHeader->setMenuText( tr( "Report Header" ) );
    reportAddReportHeader->setToolTip( tr( "Add report header" ) );
    reportAddPageHeader->setText( tr( "Add Page Header" ) );
    reportAddPageHeader->setMenuText( tr( "Page Header" ) );
    reportAddDetailHeader->setText( tr( "Add Detail Header" ) );
    reportAddDetailHeader->setMenuText( tr( "Detail Header" ) );
    reportAddDetail->setText( tr( "Add Detail" ) );
    reportAddDetail->setMenuText( tr( "Detail" ) );
    reportAddDetailFooter->setText( tr( "Add Detail Footer" ) );
    reportAddDetailFooter->setMenuText( tr( "Detail Footer" ) );
    reportAddPageFooter->setText( tr( "Add Page Footer" ) );
    reportAddPageFooter->setMenuText( tr( "Page Footer" ) );
    reportAddReportFooter->setText( tr( "Add Report Footer" ) );
    reportAddReportFooter->setMenuText( tr( "Report Footer" ) );
    AddReportSections->setText( tr( "Add Report Sections" ) );
    AddReportSections->setToolTip( tr( "Add Report Sections" ) );
    reportAddNothing->setText( tr( "Clear Selection" ) );
    reportAddNothing->setMenuText( tr( "Clea&r Selection" ) );
    reportAddNothing->setToolTip( tr( "Clear Selection" ) );
    reportAddLine->setText( tr( "Add Line" ) );
    reportAddLine->setMenuText( tr( "L&ine" ) );
    reportAddLine->setToolTip( tr( "Add Line" ) );
    reportAddCalculatedField->setText( tr( "Add Calculated Field" ) );
    reportAddCalculatedField->setMenuText( tr( "&Calculated Field" ) );
    reportAddCalculatedField->setToolTip( tr( "Add Calculated Field" ) );
    reportAddSpecialField->setText( tr( "Add Special Field" ) );
    reportAddSpecialField->setMenuText( tr( "&Special Field" ) );
    reportAddSpecialField->setToolTip( tr( "Add Special Field" ) );
    reportAddField->setText( tr( "Add Field" ) );
    reportAddField->setMenuText( tr( "&Field" ) );
    reportAddField->setToolTip( tr( "Add Field" ) );
    addReportLabel->setText( tr( "Add Label" ) );
    addReportLabel->setMenuText( tr( "&Label" ) );
    addReportLabel->setToolTip( tr( "Add Label" ) );
    AddReportSections->setMenuText(tr( "Add Report Sections" ));
    toolBar->setLabel( tr( "Tools" ) );
    Toolbar_2->setLabel( tr( "Report Sections" ) );
    Toolbar->setLabel( tr( "Report Items" ) );
    if (menubar->findItem(1))
        menubar->findItem(1)->setText( tr( "&File" ) );
    if (menubar->findItem(2))
        menubar->findItem(2)->setText( tr( "&Sections" ) );
    if (menubar->findItem(3))
        menubar->findItem(3)->setText( tr( "&Items" ) );
    if (menubar->findItem(4))
        menubar->findItem(4)->setText( tr( "&Help" ) );
}

