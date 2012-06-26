#include "playfield.h"
#include "gamewindow.h"
#include <qpainter.h>

PlayField::PlayField( QWidget *parent, const char *name )
    : QWidget( parent, name, WRepaintNoErase )
{
    left = FALSE;
    right = FALSE;
    up = FALSE;
    down = FALSE;
    space = FALSE;
    setFocusPolicy( StrongFocus );
    setFixedSize( 500, 400 );
    mode = None;
}

PlayField::~PlayField()
{
}

int PlayField::addItem( const QPixmap &pm )
{
    items.append( PlayFieldItem(pm) );
    return items.count()-1;
}

void PlayField::moveItem( int id, int x, int y )
{
    PlayFieldItem &it = items[id];
    it.x = x;
    it.y = y;
}

void PlayField::enableItem( int id, bool enable )
{
    PlayFieldItem &it = items[id];
    it.enabled = enable;
}

void PlayField::paintEvent( QPaintEvent * )
{
    QPixmap pm( size() );
    QPainter p;
    p.begin( &pm, this );
    p.setBrush( black );
    p.drawRect( 0, 0, width(), height() );
    if ( mode==None ) {
	ItemListConstIterator it = items.begin();
	while( it!=items.end() ) {
	    const PlayFieldItem item = *it;
	    if( item.enabled )
		p.drawPixmap( item.x, item.y, item.pixmap );
	    it++;
	}
    } else {
	p.setPen( white );
	p.setFont( QFont( "serif", 20 ) );
	QString s = mode==Win ? "You won!!" : "Game over";
	QRect r = p.boundingRect( QRect(), 0, s );
	p.drawText( width()/2-r.width()/2, height()/2-r.height()/2, s );
    }
    p.end();
    bitBlt( this, 0, 0, &pm );
}

void PlayField::gameOver( bool success )
{
    mode = success ? Win : Lose;
    emit gameCompleted();
}

void PlayField::clear()
{
    items.clear();
    mode = None;
}

void PlayField::keyPressEvent( QKeyEvent *ev )
{
    switch( ev->key() )
	{
	case Key_Left: left = TRUE; break;
	case Key_Right: right = TRUE; break;
	case Key_Up: up = TRUE; break;
	case Key_Down: down = TRUE; break;
	case Key_Space: space = TRUE; break;
	}
}

void PlayField::keyReleaseEvent( QKeyEvent *ev )
{
    switch( ev->key() )
	{
	case Key_Left: left = FALSE; break;
	case Key_Right: right = FALSE; break;
	case Key_Up: up = FALSE; break;
	case Key_Down: down = FALSE; break;
	case Key_Space: space = FALSE; break;
	}
}

