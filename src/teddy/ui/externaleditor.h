/****************************************************************************
** Form interface generated from reading ui file 'ui/externaleditor.ui'
**
** Created: jue nov 22 22:25:36 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.8   edited Jan 11 14:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef EXTERNALEDITORPREF_H
#define EXTERNALEDITORPREF_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QComboBox;

class ExternalEditorPref : public QWidget
{
    Q_OBJECT

public:
    ExternalEditorPref( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~ExternalEditorPref();

    QFrame* frame4;
    QLabel* textLabel1;
    QLineEdit* lineEdit_extEditor;
    QPushButton* pushButton1;
    QCheckBox* checkBox_allowExtEditor;
    QLabel* textLabel1_2;
    QComboBox* comboBox_outputEncoding;

public slots:
    virtual void selectApp();
    virtual void setEditor( const QString & val );
    virtual QString editor();
    virtual void setDefaultEditor();
    virtual bool editorActive();
    virtual void setEditorActive( bool active );
    virtual QString outputEncoding();
    virtual void setOutputEncoding( const QString & val );

protected:
    QGridLayout* ExternalEditorPrefLayout;
    QSpacerItem* spacer5;
    QSpacerItem* spacer3;
    QSpacerItem* spacer4;
    QHBoxLayout* frame4Layout;
    QHBoxLayout* layout1;
    QSpacerItem* spacer2;

protected slots:
    virtual void languageChange();

private:
    void init();

};

#endif // EXTERNALEDITORPREF_H
