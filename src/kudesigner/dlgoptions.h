/****************************************************************************
** Form interface generated from reading ui file 'dlgoptions.ui'
**
** Created: jue nov 22 22:25:22 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.8   edited Jan 11 14:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DLGOPTIONS_H
#define DLGOPTIONS_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QLineEdit;
class QTable;

class dlgOptions : public QDialog
{
    Q_OBJECT

public:
    dlgOptions( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~dlgOptions();

    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonHelp;
    QLineEdit* LineEdit1;
    QTable* taProps;

protected:
    QGridLayout* dlgOptionsLayout;
    QVBoxLayout* Layout5;
    QSpacerItem* Spacer1;

protected slots:
    virtual void languageChange();

};

#endif // DLGOPTIONS_H
