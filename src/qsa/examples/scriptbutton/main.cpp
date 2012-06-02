#include "scriptbutton.h"

#include <qsinterpreter.h>
#include <qapplication.h>
#include <qwidget.h>
#include <qlcdnumber.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include "widgetwrapper.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );
    QWidget widget;

    // create two sample widgets
    QGroupBox *lcdGroup = new QGroupBox( 2, Qt::Vertical, "lcd", &widget );
    QGroupBox *editGroup = new QGroupBox( 2, Qt::Vertical, "edit", &widget );
    QLCDNumber *lcd = new QLCDNumber( lcdGroup, "lcd" );
    QLineEdit *edit = new QLineEdit( editGroup, "edit" );
    edit->setText( "text" );

    // some help text
    QLabel *help = new QLabel( "Right-click on any of the buttons to edit "
			       "its properties.", &widget );
    new QLabel( "Properties: <ul><li>value</li>"
		"<li>setHexMode()</li><li>...</li></ul>", lcdGroup );
    new QLabel( "Properties: <ul><li>text</li><li>maxLength</li>"
		"<li>clear()</li><li>...</li></ul>", editGroup );

    QSProject project;

    // add script buttons
    ScriptButton *button1 = new ScriptButton( &project, "Increase Counter",
					      &widget, "button1" );
    ScriptButton *button2 = new ScriptButton( &project, "Reset Counter",
					      &widget, "button2" );
    ScriptButton *button3 = new ScriptButton( &project, "Convert to uppercase",
					      &widget, "button2" );
    QPushButton *button4 = new QPushButton( "&Quit", &widget, "button3" );

    button1->setScriptCode( "Application.lcd.value++;" );
    button2->setScriptCode( "Application.lcd.value = 0;" );
    button3->setScriptCode( "Application.edit.text = Application.edit.text.upper();" );

    QObject::connect(button4, SIGNAL(clicked()), &app, SLOT(quit()));

    // layout widgets
    QVBoxLayout *ml = new QVBoxLayout( &widget, 10 );
    ml->addWidget( help );
    QHBoxLayout *hbox = new QHBoxLayout( ml );
    hbox->addWidget( lcdGroup );
    hbox->addWidget( editGroup );
    QVBoxLayout *vbox = new QVBoxLayout( hbox );
    vbox->addWidget( button1 );
    vbox->addWidget( button2 );
    vbox->addWidget( button3 );
    vbox->addStretch( -1 );
    vbox->addWidget( button4 );

    // teach interpreter about widgets
    project.interpreter()->addWrapperFactory( new WidgetWrapperFactory() );
    project.interpreter()->addObjectFactory( new QtNamespaceProvider() );
    project.addObject( lcd );
    project.addObject( edit );

    app.setMainWidget( &widget );
    widget.show();

    return app.exec();
}
