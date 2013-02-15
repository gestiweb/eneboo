/****************************************************************************
** Form interface generated from reading ui file 'ui/general.ui'
**
** Created: jue nov 22 22:25:36 2012
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.8   edited Jan 11 14:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GENERALPREF_H
#define GENERALPREF_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QCheckBox;
class QLabel;
class QSpinBox;
class QGroupBox;
class QPushButton;

class GeneralPref : public QWidget
{
    Q_OBJECT

public:
    GeneralPref( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~GeneralPref();

    QCheckBox* checkBox_drawBranches;
    QCheckBox* checkBox_sortAttr;
    QLabel* textLabel5;
    QSpinBox* spinBox_recentDocs;
    QLabel* textLabel6;
    QGroupBox* groupBox1;
    QLabel* textLabel3;
    QPushButton* button_emptyBorder;
    QPushButton* button_focusBorder;
    QPushButton* button_filledBorder;
    QLabel* textLabel4;
    QLabel* textLabel7;
    QGroupBox* groupBox2;
    QSpinBox* spinBox_minColumnWidth;
    QSpinBox* spinBox_indent;
    QLabel* textLabel8;
    QLabel* textLabel1;

    virtual QColor emptyColor();
    virtual QColor filledColor();
    virtual QColor focusColor();
    virtual int numRecentDocs();
    virtual int indentWidth();
    virtual int minColumnWidth();
    virtual bool sortAttr();
    virtual bool drawBranches();

public slots:
    virtual void setDefaults();
    virtual void setColorIcon();
    virtual void setEmptyColor( QColor c );
    virtual void setFilledColor( QColor c );
    virtual void setFocusColor( QColor c );
    virtual void emptyBorderDialog();
    virtual void filledBorderDialog();
    virtual void focusBorderDialog();
    virtual void setNumRecentDocs( int val );
    virtual void setIndentWidth( int width );
    virtual void setMinColumnWidth( int width );
    virtual void setSortAttr( bool val );
    virtual void setDrawBranches( bool val );

protected:
    QGridLayout* GeneralPrefLayout;
    QSpacerItem* spacer3;
    QSpacerItem* spacer21;
    QSpacerItem* spacer30;
    QHBoxLayout* layout9;
    QSpacerItem* spacer24;
    QGridLayout* groupBox1Layout;
    QSpacerItem* spacer28;
    QSpacerItem* spacer27;
    QGridLayout* groupBox2Layout;
    QSpacerItem* spacer29;

protected slots:
    virtual void languageChange();

private:
    QColor emptyBorderColor;
    QColor filledBorderColor;
    QColor focusBorderColor;

    void init();

};

#endif // GENERALPREF_H
