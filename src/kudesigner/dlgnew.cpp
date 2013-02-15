/****************************************************************************
** Form implementation generated from reading ui file 'dlgnew.ui'
**
** Created: jue nov 22 22:25:30 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.8   edited Jan 11 14:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "dlgnew.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "dlgnew.ui.h"
/*
 *  Constructs a dlgNew as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
dlgNew::dlgNew( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "dlgNew" );
    setSizeGripEnabled( TRUE );
    dlgNewLayout = new QGridLayout( this, 1, 1, 11, 6, "dlgNewLayout"); 

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setAutoDefault( TRUE );
    Layout1->addWidget( buttonHelp );
    Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( Horizontal_Spacing2 );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( buttonCancel );

    dlgNewLayout->addLayout( Layout1, 3, 0 );

    Layout5 = new QGridLayout( 0, 1, 1, 0, 6, "Layout5"); 

    laPageSize = new QLabel( this, "laPageSize" );
    laPageSize->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, laPageSize->sizePolicy().hasHeightForWidth() ) );

    Layout5->addWidget( laPageSize, 0, 1 );

    laFileName = new QLabel( this, "laFileName" );

    Layout5->addWidget( laFileName, 0, 0 );

    fcReportName = new QLineEdit( this, "fcReportName" );

    Layout5->addWidget( fcReportName, 1, 0 );

    cbPageSize = new QComboBox( FALSE, this, "cbPageSize" );
    cbPageSize->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, cbPageSize->sizePolicy().hasHeightForWidth() ) );

    Layout5->addWidget( cbPageSize, 1, 1 );

    dlgNewLayout->addLayout( Layout5, 0, 0 );

    Layout5_2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout5_2"); 

    laRight = new QLabel( this, "laRight" );

    Layout5_2->addWidget( laRight, 3, 1 );

    laTop = new QLabel( this, "laTop" );

    Layout5_2->addWidget( laTop, 1, 0 );

    sbBottom = new QSpinBox( this, "sbBottom" );

    Layout5_2->addWidget( sbBottom, 4, 0 );

    sbRight = new QSpinBox( this, "sbRight" );

    Layout5_2->addWidget( sbRight, 4, 1 );

    sbTop = new QSpinBox( this, "sbTop" );

    Layout5_2->addWidget( sbTop, 2, 0 );

    laBottom = new QLabel( this, "laBottom" );

    Layout5_2->addWidget( laBottom, 3, 0 );

    laLeft = new QLabel( this, "laLeft" );

    Layout5_2->addWidget( laLeft, 1, 1 );

    sbLeft = new QSpinBox( this, "sbLeft" );

    Layout5_2->addWidget( sbLeft, 2, 1 );

    bgPaper = new QButtonGroup( this, "bgPaper" );
    bgPaper->setMinimumSize( QSize( 0, 70 ) );

    rbLandscape = new QRadioButton( bgPaper, "rbLandscape" );
    rbLandscape->setGeometry( QRect( 10, 40, 300, 20 ) );

    rbPortrait = new QRadioButton( bgPaper, "rbPortrait" );
    rbPortrait->setGeometry( QRect( 10, 20, 310, 20 ) );
    rbPortrait->setChecked( TRUE );

    Layout5_2->addMultiCellWidget( bgPaper, 0, 0, 0, 1 );

    dlgNewLayout->addLayout( Layout5_2, 1, 0 );
    Spacer4 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    dlgNewLayout->addItem( Spacer4, 2, 0 );
    languageChange();
    resize( QSize(404, 323).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( cbPageSize, SIGNAL( highlighted(const QString&) ), this, SLOT( cbPageSize_highlighted(const QString&) ) );

    // tab order
    setTabOrder( cbPageSize, rbPortrait );
    setTabOrder( rbPortrait, sbTop );
    setTabOrder( sbTop, sbLeft );
    setTabOrder( sbLeft, sbBottom );
    setTabOrder( sbBottom, sbRight );
    setTabOrder( sbRight, buttonOk );
    setTabOrder( buttonOk, buttonCancel );
    setTabOrder( buttonCancel, buttonHelp );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
dlgNew::~dlgNew()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dlgNew::languageChange()
{
    setCaption( tr( "New Report" ) );
    buttonHelp->setText( tr( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    laPageSize->setText( tr( "Page size:" ) );
    laFileName->setText( tr( "Report template name:" ) );
    cbPageSize->clear();
    cbPageSize->insertItem( tr( "A4" ) );
    cbPageSize->insertItem( tr( "B5" ) );
    cbPageSize->insertItem( tr( "Letter" ) );
    cbPageSize->insertItem( tr( "Legal" ) );
    cbPageSize->insertItem( tr( "Executive" ) );
    cbPageSize->insertItem( tr( "A0" ) );
    cbPageSize->insertItem( tr( "A1" ) );
    cbPageSize->insertItem( tr( "A2" ) );
    cbPageSize->insertItem( tr( "A3" ) );
    cbPageSize->insertItem( tr( "A5" ) );
    cbPageSize->insertItem( tr( "A6" ) );
    cbPageSize->insertItem( tr( "A7" ) );
    cbPageSize->insertItem( tr( "A8" ) );
    cbPageSize->insertItem( tr( "A9" ) );
    cbPageSize->insertItem( tr( "B0" ) );
    cbPageSize->insertItem( tr( "B1" ) );
    cbPageSize->insertItem( tr( "B10" ) );
    cbPageSize->insertItem( tr( "B2" ) );
    cbPageSize->insertItem( tr( "B3" ) );
    cbPageSize->insertItem( tr( "B4" ) );
    cbPageSize->insertItem( tr( "B6" ) );
    cbPageSize->insertItem( tr( "B7" ) );
    cbPageSize->insertItem( tr( "B8" ) );
    cbPageSize->insertItem( tr( "B9" ) );
    cbPageSize->insertItem( tr( "C5E" ) );
    cbPageSize->insertItem( tr( "Comm10E" ) );
    cbPageSize->insertItem( tr( "DLE" ) );
    cbPageSize->insertItem( tr( "Folio" ) );
    cbPageSize->insertItem( tr( "Ledger" ) );
    cbPageSize->insertItem( tr( "Tabloid" ) );
    laRight->setText( tr( "Right margin:" ) );
    laTop->setText( tr( "Top margin:" ) );
    laBottom->setText( tr( "Bottom margin:" ) );
    laLeft->setText( tr( "Left margin:" ) );
    bgPaper->setTitle( tr( "Paper" ) );
    rbLandscape->setText( tr( "Landscape" ) );
    rbPortrait->setText( tr( "Portrait" ) );
}

