/***************************************************************************
AQTabView.cpp
-------------------
begin                : 12/09/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
email                : mail@infosial.com
***************************************************************************/ 
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 ***************************************************************************/ 
/***************************************************************************
   Este  programa es software libre. Puede redistribuirlo y/o modificarlo
   bajo  los  términos  de  la  Licencia  Pública General de GNU   en  su
   versión 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

#include <QToolButton>
#include <QStackedWidget>
#include <QCloseEvent>
#include <QPainter>

#include <AQCore/AQCore.h>

#include "AQTabView.h"

AQTabView::AQTabView( QWidget * parent ) : QWidget( parent )
{
  ui.setupUi( this );
#ifdef AQ_LOGO
  logo.load( ":/images/logo_abanq.png" );
#endif

  init();
}

AQTabView::~AQTabView()
{}

QWidget * AQTabView::currentWidget() const
{
  return ui.tab->currentWidget();
}

QWidgetList AQTabView::widgets() const
{
  QWidgetList list;
  for ( int i = 0; i < count(); ++i )
    list.append( ui.tab->widget( i ) );
  return list;
}

void AQTabView::setTitle( QWidget * widget, const QString & title ) const
{
  ui.tab->setTabText( ui.tab->indexOf( widget ), title );
}

int AQTabView::count() const
{
  return ui.tab->count();
}

void AQTabView::setTabPosition( QTabWidget::TabPosition pos )
{
  ui.tab->setTabPosition( pos );
}

void AQTabView::setTabShape( QTabWidget::TabShape s )
{
  ui.tab->setTabShape( s );
}

void AQTabView::closeEvent( QCloseEvent * e )
{
  e->ignore();
  Q_EMIT closeRequest();
}

void AQTabView::init()
{
  while ( ui.tab->count() ) {
    QWidget * page = ui.tab->widget( 0 );
    ui.tab->removeTab( 0 );
    delete page;
  }

  connect( ui.tab, SIGNAL( currentChanged( int ) ), this, SLOT( transferFocus() ) );

  QToolButton * newTabButton = new QToolButton( this );
  ui.tab->setCornerWidget( newTabButton, Qt::TopLeftCorner );
  newTabButton->setCursor( Qt::ArrowCursor );
  newTabButton->setAutoRaise( true );
  newTabButton->setIcon( QIcon( ":/images/add.png" ) );
  connect( newTabButton, SIGNAL( clicked() ), this, SIGNAL( newTabClicked() ) );
  newTabButton->setToolTip( tr( "Nueva pestaña" ) );
  QToolButton * closeTabButton = new QToolButton( this );
  ui.tab->setCornerWidget( closeTabButton, Qt::TopRightCorner );
  closeTabButton->setCursor( Qt::ArrowCursor );
  closeTabButton->setAutoRaise( true );
  closeTabButton->setIcon( QIcon( ":/images/close.png" ) );
  connect( closeTabButton, SIGNAL( clicked() ), this, SIGNAL( closeTabClicked() ) );
  closeTabButton->setToolTip( tr( "Cerrar pestaña" ) );
  closeTabButton->setEnabled( false );

  QStackedWidget * stack = ui.tab->findChild<QStackedWidget *>();
  stack->setContentsMargins( 0, 0, 0, 0 );
}

void AQTabView::nextTab() const
{
  if ( ui.tab->currentIndex() <= count() - 1 )
    ui.tab->setCurrentIndex( ui.tab->currentIndex() + 1 );
}

void AQTabView::previousTab() const
{
  int idx = ui.tab->currentIndex() - 1;
  if ( idx >= 0 )
    ui.tab->setCurrentIndex( idx );
}

void AQTabView::setCurrentWidget( QWidget * widget ) const
{
  ui.tab->setCurrentWidget( widget );
}

int AQTabView::insertTab( int index, QWidget * widget, const QString & title ) const
{
  int ret = ui.tab->insertTab( index, widget, title );
  ui.tab->cornerWidget( Qt::TopRightCorner ) ->setEnabled( count() );
  return ret;
}

void AQTabView::updateTitle( const QString & title ) const
{
  ui.tab->setTabText( ui.tab->indexOf( currentWidget() ), title );
}

void AQTabView::transferFocus() const
{
  if ( currentWidget() )
    currentWidget() ->setFocus();
}

void AQTabView::closeTab() const
{
  if ( !count() )
    return ;
  ui.tab->removeTab( ui.tab->indexOf( currentWidget() ) );
  ui.tab->cornerWidget( Qt::TopRightCorner ) ->setEnabled( count() );
}

void AQTabView::closeTab( QWidget * widget ) const
{
  if ( !count() )
    return ;
  ui.tab->removeTab( ui.tab->indexOf( widget ) );
  ui.tab->cornerWidget( Qt::TopRightCorner ) ->setEnabled( count() );
}

void AQTabView::addTab( QWidget * widget ) const
{
  ui.tab->addTab( widget, tr( "..." ) );
  ui.tab->cornerWidget( Qt::TopRightCorner ) ->setEnabled( count() );
}

QWidget * AQTabView::widget( int index ) const
{
  return ui.tab->widget( index );
}

#ifdef AQ_LOGO
void AQTabView::paintEvent( QPaintEvent * pe )
{
  if ( !ui.tab->count() ) {
    QPainter p( this );
    p.setClipRegion( pe->region() );
    int dx = width() - logo.width();
    int dy = height() - logo.height();
    p.fillRect( pe->rect(), QColor( logo.pixel( 1, 1 ) ) );
    QRect rImage( dx, dy, logo.width(), logo.height() );
    if ( pe->rect().intersects( rImage ) )
      p.drawImage ( rImage.topLeft(), logo );
    QRect rLine( dx + 31, 0, 4, dy );
    if ( pe->rect().intersects( rLine ) ) {
      p.setPen( QColor( 0, 0, 32 ) );
      p.setBrush( QColor( 0, 0, 32 ) );
      p.drawRect( rLine );
    }
    p.end();
  }
  QWidget::paintEvent( pe );
}
#endif
