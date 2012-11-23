/****************************************************************************
** Form interface generated from reading ui file 'dlgnew.ui'
**
** Created: jue nov 22 22:25:22 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.8   edited Jan 11 14:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DLGNEW_H
#define DLGNEW_H

#include <qvariant.h>
#include <qdialog.h>
#include <qsize.h>
#include <qfiledialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QLabel;
class QLineEdit;
class QComboBox;
class QSpinBox;
class QButtonGroup;
class QRadioButton;
class FileChooser;

class dlgNew : public QDialog
{
    Q_OBJECT

public:
    dlgNew( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~dlgNew();

    QPushButton* buttonHelp;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QLabel* laPageSize;
    QLabel* laFileName;
    QLineEdit* fcReportName;
    QComboBox* cbPageSize;
    QLabel* laRight;
    QLabel* laTop;
    QSpinBox* sbBottom;
    QSpinBox* sbRight;
    QSpinBox* sbTop;
    QLabel* laBottom;
    QLabel* laLeft;
    QSpinBox* sbLeft;
    QButtonGroup* bgPaper;
    QRadioButton* rbLandscape;
    QRadioButton* rbPortrait;

public slots:
    virtual void init();
    virtual void cbPageSize_highlighted( const QString & s );

protected:
    public: QSize pSize;

    QGridLayout* dlgNewLayout;
    QSpacerItem* Spacer4;
    QHBoxLayout* Layout1;
    QSpacerItem* Horizontal_Spacing2;
    QGridLayout* Layout5;
    QGridLayout* Layout5_2;

protected slots:
    virtual void languageChange();

};

#endif // DLGNEW_H
