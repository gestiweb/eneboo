#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qsize.h>
#include <qvaluelist.h>

class PlayFieldItem;

typedef QValueList<PlayFieldItem> ItemList;
typedef QValueList<PlayFieldItem>::ConstIterator ItemListConstIterator;

class PlayField : public QWidget 
{
  Q_OBJECT

  Q_PROPERTY( bool left READ isLeft )
  Q_PROPERTY( bool right READ isRight )
  Q_PROPERTY( bool up READ isUp )
  Q_PROPERTY( bool down READ isDown )
  Q_PROPERTY( bool space READ isSpace )

  enum GameMode { None, Win, Lose };

public:
  PlayField( QWidget *parent=0, const char * name=0 );
  ~PlayField();

  QSize sizeHint() const { return QSize( 500, 400 ); }
  
  bool isLeft() const { return left; }
  bool isRight() const { return right; }
  bool isUp() const { return up; }
  bool isDown() const { return down; }
  bool isSpace() const { return space; }

public slots:
  int addItem( const QPixmap &pm );
  void moveItem( int id, int x, int y );
  void enableItem( int id, bool enable );
  void gameOver( bool success );
  void clear();

signals:
  void gameCompleted();

protected:
  void paintEvent( QPaintEvent *event );
  void keyPressEvent( QKeyEvent *ev );
  void keyReleaseEvent( QKeyEvent *ev );

private:
  ItemList items;
  int left:1;
  int right:1;
  int up:1;
  int down:1;
  int space:1;
  GameMode mode;
};

class PlayFieldItem 
{
public:
  PlayFieldItem( const QPixmap &pm=QPixmap() ) 
    : pixmap( pm ), x( 0 ), y( 0 ), enabled( TRUE ) { }
  QPixmap pixmap;
  int x;
  int y;
  bool enabled;
  
};

#endif


