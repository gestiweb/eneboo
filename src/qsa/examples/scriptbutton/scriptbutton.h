#ifndef SCRIPTBUTTON_H
#define SCRIPTBUTTON_H

#include <qpushbutton.h>

class QPopupMenu;
class QSProject;
class ScriptEditor;

class ScriptButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY( QString scriptCode READ scriptCode WRITE setScriptCode )

public:
    // standard QPushButton constructors
    ScriptButton( QSProject *proj, QWidget *parent, const char* name=0 );
    ScriptButton( QSProject *proj, const QString &text,
		  QWidget *parent, const char* name=0 );

    void setScriptCode( const QString &c );
    QString scriptCode() const { return code; }

public slots:
    void executeScript();

protected:
    void contextMenuEvent( QContextMenuEvent *e );

private slots:
    void editCode();
    void renameButton();

private:
    void init( QSProject *p );

    QString code;
    QPopupMenu *popup;
    ScriptEditor *edit;
    QSProject *project;
};

#endif
