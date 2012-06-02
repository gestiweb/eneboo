#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

class QTextEdit;

#include <qsyntaxhighlighter.h>
#include <qtextedit.h>
#include <qsobjectfactory.h>

class SyntaxHighlighter : public QObject, public QSyntaxHighlighter
{
    Q_OBJECT

public:
    SyntaxHighlighter( QTextEdit *edit )
	: QObject( 0, "SyntaxHighlighter" ), QSyntaxHighlighter( edit ), estate( 0 ) {
    }

    void setEndState( int state ) { estate = state; }
    int endState() const { return estate; }

    int highlightParagraph( const QString &text, int endStateOfLastPara ) {
	estate =0;
	emit highlight( text, endStateOfLastPara );
	return estate;
    }

public slots:
    void setFormat( int start, int len, const QFont &font, const QColor &color ) {
	QSyntaxHighlighter::setFormat( start, len, font, color );
    }
    void setFormat( int start, int len, const QColor &color ) {
	QSyntaxHighlighter::setFormat( start, len, color );
    }
    void setFormat( int start, int len, const QFont &font ) {
	QSyntaxHighlighter::setFormat( start, len, font );
    }

signals:
    void highlight( const QString &text, int endStateOfLastPara );

private:
    int estate;

};

class ConstructorFactory : public QSObjectFactory
{
public:
    ConstructorFactory()
    {
	registerClass( "SyntaxHighlighter" );
    }

    QObject *create( const QString &className,
		     const QSArgumentList &arguments,
		     QObject * ) {
	if ( className == "SyntaxHighlighter" ) {
	    if ( arguments.count() != 1 ) {
		throwError( QString( "new SyntaxHighlighter: Expexted one argument "
				     "of type QTextEdit but got %1 arguments" ).
			    arg( arguments.count() ) );
		return 0;
	    }
	    if ( arguments[0].type() != QSArgument::QObjectPtr ||
		 !arguments[0].qobject()->inherits( "QTextEdit" ) ) {
		throwError( QString( "new SyntaxHighlighter: Expexted one argument "
				     "of type QTextEdit but got one of type %1" ).
			    arg( arguments[0].typeName() ) );
		return 0;
	    }
	    return new SyntaxHighlighter( (QTextEdit*)arguments[0].qobject() );
	}
	return 0;
    }
};


#endif
