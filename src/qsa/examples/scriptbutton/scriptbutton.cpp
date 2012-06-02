#include "scriptbutton.h"

#include <qsinterpreter.h>
#include <qseditor.h>
#include <qpopupmenu.h>
#include <qinputdialog.h>
#include <qtextedit.h>
#include <qlayout.h>

class ScriptEditor : public QDialog
{
public:
    ScriptEditor( QSProject *proj, QWidget *parent = 0, const char *name = 0 );
    void setCode( const QString &c );
    QString code() const { return ed->text(); }

protected:
    void keyPressEvent( QKeyEvent *e ) {
	if ( e->key() == Key_Escape ) { // let the editor handle the ESC key
	    e->ignore();
	    return;
	}
	QDialog::keyPressEvent( e );
    }

private:
    QSEditor *ed;
};

ScriptEditor::ScriptEditor( QSProject *proj, QWidget *parent, const char *name )
    : QDialog( parent, name )
{
    QString scName = QString( "script%1.qs" ).arg( proj->scripts().count() );
    ed = proj->createEditor( proj->createScript( scName ), this );
    QPushButton *ok = new QPushButton( "&OK", this );
    QPushButton *cancel = new QPushButton( "&Cancel", this );
    connect( ok, SIGNAL( clicked() ), SLOT( accept() ) );
    connect( cancel, SIGNAL( clicked() ), SLOT( reject() ) );

    QVBoxLayout *vbox = new QVBoxLayout( this, 5 );
    vbox->addWidget( ed );
    QHBoxLayout *hbox = new QHBoxLayout( vbox );
    hbox->addWidget( ok );
    hbox->addStretch( -1 );
    hbox->addWidget( cancel );
    resize( 400, 400 );
}

void ScriptEditor::setCode( const QString &c )
{
    ed->setText( c );
}

ScriptButton::ScriptButton( QSProject *proj,
			    QWidget *parent, const char* name )
    : QPushButton( parent, name )
{
    init( proj );
}

ScriptButton::ScriptButton( QSProject *proj, const QString &text,
			    QWidget *parent, const char* name )
    : QPushButton( text, parent, name )
{
    init( proj );
}

void ScriptButton::init( QSProject *proj )
{
    project = proj;
    popup = 0;
    edit = 0;
    connect( this, SIGNAL(clicked()), this, SLOT(executeScript()) );
}

void ScriptButton::setScriptCode( const QString &c )
{
    code = c;
}

void ScriptButton::executeScript()
{
    if ( !code.isEmpty() )
	project->interpreter()->evaluate( code );
}

void ScriptButton::contextMenuEvent( QContextMenuEvent *e )
{
    if ( !popup ) {
	popup = new QPopupMenu( this );
	popup->insertItem( "Rename Button", this, SLOT(renameButton()) );
	popup->insertItem( "Edit Code", this, SLOT(editCode()) );
    }
    popup->exec( e->globalPos() );
    e->accept();
}

void ScriptButton::renameButton()
{
    bool ok;
    QString n = QInputDialog::getText( "Script Button",
				       "Enter new button name",
				       QLineEdit::Normal,
				       text(), &ok, this );
    if ( ok )
	setText( n );
}

void ScriptButton::editCode()
{
    if ( !edit ) {
	edit = new ScriptEditor( project );
	edit->setCaption( QString( "%1 Script Code" ).arg( name() ) );
    }
    edit->setCode( scriptCode() );
    if ( edit->exec() == QDialog::Accepted )
	setScriptCode( edit->code() );
}
