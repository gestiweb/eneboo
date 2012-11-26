/****************************************************************************
** Form interface generated from reading ui file 'ui/messagelist.ui'
**
** Created: jue nov 22 22:25:36 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.8   edited Jan 11 14:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MESSAGELIST_H
#define MESSAGELIST_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QListView;
class QListViewItem;

class MessageList : public QWidget
{
    Q_OBJECT

public:
    MessageList( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~MessageList();

    QListView* messageListView;

protected:
    QGridLayout* MessageListLayout;

protected slots:
    virtual void languageChange();

private:
    void init();

};

#endif // MESSAGELIST_H
