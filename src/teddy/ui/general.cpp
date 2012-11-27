/****************************************************************************
** Form implementation generated from reading ui file 'ui/general.ui'
**
** Created: jue nov 22 22:25:36 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.8   edited Jan 11 14:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "general.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "general.ui.h"

/*
 *  Constructs a GeneralPref as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
GeneralPref::GeneralPref( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "GeneralPref" );
    GeneralPrefLayout = new QGridLayout( this, 1, 1, 11, 6, "GeneralPrefLayout"); 

    checkBox_drawBranches = new QCheckBox( this, "checkBox_drawBranches" );
    checkBox_drawBranches->setChecked( TRUE );

    GeneralPrefLayout->addWidget( checkBox_drawBranches, 7, 0 );

    checkBox_sortAttr = new QCheckBox( this, "checkBox_sortAttr" );
    checkBox_sortAttr->setChecked( TRUE );

    GeneralPrefLayout->addMultiCellWidget( checkBox_sortAttr, 6, 6, 0, 1 );

    layout9 = new QHBoxLayout( 0, 0, 6, "layout9"); 

    textLabel5 = new QLabel( this, "textLabel5" );
    layout9->addWidget( textLabel5 );

    spinBox_recentDocs = new QSpinBox( this, "spinBox_recentDocs" );
    spinBox_recentDocs->setValue( 20 );
    layout9->addWidget( spinBox_recentDocs );

    textLabel6 = new QLabel( this, "textLabel6" );
    layout9->addWidget( textLabel6 );
    spacer24 = new QSpacerItem( 201, 31, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout9->addItem( spacer24 );

    GeneralPrefLayout->addMultiCellLayout( layout9, 0, 0, 0, 1 );
    spacer3 = new QSpacerItem( 21, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    GeneralPrefLayout->addItem( spacer3, 1, 1 );

    groupBox1 = new QGroupBox( this, "groupBox1" );
    groupBox1->setColumnLayout(0, Qt::Vertical );
    groupBox1->layout()->setSpacing( 6 );
    groupBox1->layout()->setMargin( 11 );
    groupBox1Layout = new QGridLayout( groupBox1->layout() );
    groupBox1Layout->setAlignment( Qt::AlignTop );

    textLabel3 = new QLabel( groupBox1, "textLabel3" );

    groupBox1Layout->addWidget( textLabel3, 0, 0 );

    button_emptyBorder = new QPushButton( groupBox1, "button_emptyBorder" );
    button_emptyBorder->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, button_emptyBorder->sizePolicy().hasHeightForWidth() ) );
    button_emptyBorder->setMaximumSize( QSize( 30, 32767 ) );

    groupBox1Layout->addWidget( button_emptyBorder, 0, 1 );

    button_focusBorder = new QPushButton( groupBox1, "button_focusBorder" );
    button_focusBorder->setMaximumSize( QSize( 30, 32767 ) );

    groupBox1Layout->addWidget( button_focusBorder, 0, 7 );

    button_filledBorder = new QPushButton( groupBox1, "button_filledBorder" );
    button_filledBorder->setMaximumSize( QSize( 30, 32767 ) );

    groupBox1Layout->addWidget( button_filledBorder, 0, 4 );

    textLabel4 = new QLabel( groupBox1, "textLabel4" );

    groupBox1Layout->addWidget( textLabel4, 0, 3 );
    spacer28 = new QSpacerItem( 91, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    groupBox1Layout->addItem( spacer28, 0, 2 );

    textLabel7 = new QLabel( groupBox1, "textLabel7" );

    groupBox1Layout->addWidget( textLabel7, 0, 6 );
    spacer27 = new QSpacerItem( 71, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    groupBox1Layout->addItem( spacer27, 0, 5 );

    GeneralPrefLayout->addMultiCellWidget( groupBox1, 2, 2, 0, 1 );
    spacer21 = new QSpacerItem( 21, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
    GeneralPrefLayout->addItem( spacer21, 3, 1 );

    groupBox2 = new QGroupBox( this, "groupBox2" );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    groupBox2Layout = new QGridLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    spinBox_minColumnWidth = new QSpinBox( groupBox2, "spinBox_minColumnWidth" );
    spinBox_minColumnWidth->setMaxValue( 300 );
    spinBox_minColumnWidth->setMinValue( 5 );
    spinBox_minColumnWidth->setValue( 50 );

    groupBox2Layout->addWidget( spinBox_minColumnWidth, 1, 1 );

    spinBox_indent = new QSpinBox( groupBox2, "spinBox_indent" );
    spinBox_indent->setValue( 30 );

    groupBox2Layout->addWidget( spinBox_indent, 0, 1 );
    spacer29 = new QSpacerItem( 181, 31, QSizePolicy::Expanding, QSizePolicy::Minimum );
    groupBox2Layout->addItem( spacer29, 0, 2 );

    textLabel8 = new QLabel( groupBox2, "textLabel8" );

    groupBox2Layout->addWidget( textLabel8, 1, 0 );

    textLabel1 = new QLabel( groupBox2, "textLabel1" );

    groupBox2Layout->addWidget( textLabel1, 0, 0 );

    GeneralPrefLayout->addMultiCellWidget( groupBox2, 4, 4, 0, 1 );
    spacer30 = new QSpacerItem( 21, 41, QSizePolicy::Minimum, QSizePolicy::Expanding );
    GeneralPrefLayout->addItem( spacer30, 5, 1 );
    languageChange();
    resize( QSize(600, 480).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( button_emptyBorder, SIGNAL( clicked() ), this, SLOT( emptyBorderDialog() ) );
    connect( button_filledBorder, SIGNAL( clicked() ), this, SLOT( filledBorderDialog() ) );
    connect( button_focusBorder, SIGNAL( clicked() ), this, SLOT( focusBorderDialog() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
GeneralPref::~GeneralPref()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void GeneralPref::languageChange()
{
    setCaption( tr( "Form1" ) );
    checkBox_drawBranches->setText( tr( "Draw branches" ) );
    checkBox_sortAttr->setText( tr( "Sort XML attributes" ) );
    textLabel5->setText( tr( "Remember last" ) );
    textLabel6->setText( tr( "recently opened file(s)" ) );
    groupBox1->setTitle( tr( "Cell Border Color" ) );
    textLabel3->setText( tr( "<p align=\"right\">Empty:</p>" ) );
    button_emptyBorder->setText( QString::null );
    button_focusBorder->setText( QString::null );
    button_filledBorder->setText( QString::null );
    textLabel4->setText( tr( "<p align=\"right\">Filled:</p>" ) );
    textLabel7->setText( tr( "<p align=\"right\">Focused:</p>" ) );
    groupBox2->setTitle( tr( "Dimensions" ) );
    spinBox_minColumnWidth->setSuffix( tr( " pixels" ) );
    spinBox_indent->setSuffix( tr( " pixels" ) );
    textLabel8->setText( tr( "<nobr><p align=\"right\">Minimum column width:</p>" ) );
    textLabel1->setText( tr( "<nobr><p align=\"right\">Indent width:</p>" ) );
}

