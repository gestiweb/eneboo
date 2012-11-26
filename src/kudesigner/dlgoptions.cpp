/****************************************************************************
** Form implementation generated from reading ui file 'dlgoptions.ui'
**
** Created: jue nov 22 22:25:30 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.8   edited Jan 11 14:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "dlgoptions.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qtable.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

/*
 *  Constructs a dlgOptions as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
dlgOptions::dlgOptions( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "dlgOptions" );
    setSizeGripEnabled( TRUE );
    dlgOptionsLayout = new QGridLayout( this, 1, 1, 5, 5, "dlgOptionsLayout"); 

    Layout5 = new QVBoxLayout( 0, 0, 6, "Layout5"); 

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout5->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    Layout5->addWidget( buttonCancel );

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setAutoDefault( TRUE );
    Layout5->addWidget( buttonHelp );
    Spacer1 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout5->addItem( Spacer1 );

    dlgOptionsLayout->addMultiCellLayout( Layout5, 0, 1, 1, 1 );

    LineEdit1 = new QLineEdit( this, "LineEdit1" );
    LineEdit1->setMinimumSize( QSize( 0, 70 ) );
    LineEdit1->setPaletteBackgroundColor( QColor( 220, 220, 220 ) );
    LineEdit1->setFocusPolicy( QLineEdit::NoFocus );
    LineEdit1->setAlignment( int( QLineEdit::AlignAuto ) );
    LineEdit1->setReadOnly( TRUE );

    dlgOptionsLayout->addWidget( LineEdit1, 1, 0 );

    taProps = new QTable( this, "taProps" );
    taProps->setNumCols( taProps->numCols() + 1 );
    taProps->horizontalHeader()->setLabel( taProps->numCols() - 1, tr( "Property" ) );
    taProps->setNumCols( taProps->numCols() + 1 );
    taProps->horizontalHeader()->setLabel( taProps->numCols() - 1, tr( "Value" ) );
    taProps->setNumRows( 0 );
    taProps->setNumCols( 2 );
    taProps->setSorting( FALSE );
    taProps->setSelectionMode( QTable::SingleRow );

    dlgOptionsLayout->addWidget( taProps, 0, 0 );
    languageChange();
    resize( QSize(322, 384).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // tab order
    setTabOrder( buttonOk, buttonCancel );
    setTabOrder( buttonCancel, buttonHelp );
    setTabOrder( buttonHelp, LineEdit1 );
}

/*
 *  Destroys the object and frees any allocated resources
 */
dlgOptions::~dlgOptions()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dlgOptions::languageChange()
{
    setCaption( tr( "Report Item Options" ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonHelp->setText( tr( "&Help" ) );
    buttonHelp->setAccel( QKeySequence( tr( "F1" ) ) );
    taProps->horizontalHeader()->setLabel( 0, tr( "Property" ) );
    taProps->horizontalHeader()->setLabel( 1, tr( "Value" ) );
}

