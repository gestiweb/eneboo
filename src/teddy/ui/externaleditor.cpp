/****************************************************************************
** Form implementation generated from reading ui file 'ui/externaleditor.ui'
**
** Created: jue nov 22 22:25:36 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.8   edited Jan 11 14:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "externaleditor.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "externaleditor.ui.h"

/*
 *  Constructs a ExternalEditorPref as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
ExternalEditorPref::ExternalEditorPref( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "ExternalEditorPref" );
    ExternalEditorPrefLayout = new QGridLayout( this, 1, 1, 11, 6, "ExternalEditorPrefLayout"); 

    frame4 = new QFrame( this, "frame4" );
    frame4->setFrameShape( QFrame::StyledPanel );
    frame4->setFrameShadow( QFrame::Raised );
    frame4->setLineWidth( 0 );
    frame4Layout = new QHBoxLayout( frame4, 11, 6, "frame4Layout"); 

    textLabel1 = new QLabel( frame4, "textLabel1" );
    frame4Layout->addWidget( textLabel1 );

    lineEdit_extEditor = new QLineEdit( frame4, "lineEdit_extEditor" );
    frame4Layout->addWidget( lineEdit_extEditor );

    pushButton1 = new QPushButton( frame4, "pushButton1" );
    frame4Layout->addWidget( pushButton1 );

    ExternalEditorPrefLayout->addWidget( frame4, 2, 0 );

    checkBox_allowExtEditor = new QCheckBox( this, "checkBox_allowExtEditor" );
    checkBox_allowExtEditor->setChecked( TRUE );

    ExternalEditorPrefLayout->addWidget( checkBox_allowExtEditor, 1, 0 );

    layout1 = new QHBoxLayout( 0, 0, 6, "layout1"); 

    textLabel1_2 = new QLabel( this, "textLabel1_2" );
    layout1->addWidget( textLabel1_2 );

    comboBox_outputEncoding = new QComboBox( FALSE, this, "comboBox_outputEncoding" );
    comboBox_outputEncoding->setEnabled( FALSE );
    layout1->addWidget( comboBox_outputEncoding );
    spacer2 = new QSpacerItem( 221, 41, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout1->addItem( spacer2 );

    ExternalEditorPrefLayout->addLayout( layout1, 4, 0 );
    spacer5 = new QSpacerItem( 21, 61, QSizePolicy::Minimum, QSizePolicy::Expanding );
    ExternalEditorPrefLayout->addItem( spacer5, 5, 0 );
    spacer3 = new QSpacerItem( 21, 51, QSizePolicy::Minimum, QSizePolicy::Expanding );
    ExternalEditorPrefLayout->addItem( spacer3, 3, 0 );
    spacer4 = new QSpacerItem( 21, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
    ExternalEditorPrefLayout->addItem( spacer4, 0, 0 );
    languageChange();
    resize( QSize(604, 377).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pushButton1, SIGNAL( clicked() ), this, SLOT( selectApp() ) );
    connect( checkBox_allowExtEditor, SIGNAL( toggled(bool) ), frame4, SLOT( setEnabled(bool) ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
ExternalEditorPref::~ExternalEditorPref()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ExternalEditorPref::languageChange()
{
    setCaption( tr( "ExternalEditorPref" ) );
    textLabel1->setText( tr( "Text editor:" ) );
    pushButton1->setText( tr( "Select..." ) );
    checkBox_allowExtEditor->setText( tr( "Allow file to be opened in an external editor when a parse error occurs" ) );
    textLabel1_2->setText( tr( "Output encoding:" ) );
}

