/****************************************************************************
** Form implementation generated from reading ui file 'ui/messagelist.ui'
**
** Created: jue nov 22 22:25:36 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.8   edited Jan 11 14:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "messagelist.h"

#include <qvariant.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "messagelist.ui.h"

/*
 *  Constructs a MessageList as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
MessageList::MessageList( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "MessageList" );
    MessageListLayout = new QGridLayout( this, 1, 1, 0, 6, "MessageListLayout"); 

    messageListView = new QListView( this, "messageListView" );
    messageListView->addColumn( tr( "Location" ) );
    messageListView->addColumn( tr( "Message" ) );
    QFont messageListView_font(  messageListView->font() );
    messageListView_font.setPointSize( 8 );
    messageListView->setFont( messageListView_font ); 

    MessageListLayout->addWidget( messageListView, 0, 0 );
    languageChange();
    resize( QSize(531, 241).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
MessageList::~MessageList()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MessageList::languageChange()
{
    setCaption( tr( "MessageList" ) );
    messageListView->header()->setLabel( 0, tr( "Location" ) );
    messageListView->header()->setLabel( 1, tr( "Message" ) );
}

