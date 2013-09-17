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

#include <WFitLayout>

#include <Ext/Panel>
#include <Ext/Button>
#include <Ext/TabWidget>
#include <Ext/ToolBar>

#include <QTimer>

#include "../wtqt/WQApplication.h"
#include "AQTabView.h"
#include "AQTabView_p.h"

#define AQ_CUR_INDEX ( d->indexNeedUpdate_ ? d->updateIndex() : d->currentIndex_ )

AQTabViewPrivate::AQTabViewPrivate() :
    WQWidgetPrivate()
{}

AQTabViewPrivate::~AQTabViewPrivate()
{}

void AQTabViewPrivate::setupUi()
{
  AQ_Q( AQTabView );

  ww_ = new Ext::TabWidget;
  ww_->setTabOnTop( tabOnTop_ );
  ww_->setBorder( false );
  WT_SIGNAL( ww_->currentChanged ).connect( q, &AQTabView::transferFocus );

  if ( createButtons_ ) {
    tb_ = q->toolBar();
    pbAdd_ = new Ext::Button;
    pbClose_ = new Ext::Button;

    pbAdd_->setIcon( "images/add.png" );
    AQ_WUI_CONNECT( pbAdd_->clicked, q, AQTabView, newTabClicked );
    tb_->add( pbAdd_ );

    tb_->addSeparator();

    pbClose_->setIcon( "images/close.png" );
    AQ_WUI_CONNECT( pbClose_->clicked, q, AQTabView, closeTabClicked );
    tb_->add( pbClose_ );
  }

  q->addTab();
  firstPanel_ = true;
}

int AQTabViewPrivate::updateIndex()
{
  indexNeedUpdate_ = false;
  currentIndex_ = ( ww_->count() ? ww_->currentIndex() : -1 );
  return currentIndex_;
}

void AQTabViewPrivate::closeFirstTab()
{
  ww_->removeTab( 0 );
  widgets_.removeAt( 0 );
  indexNeedUpdate_ = true;
}

AQTabView::AQTabView( WQWidget * parent ) :
    WQWidget( *new AQTabViewPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( AQTabView );
  d->tb_ = 0;
  d->currentIndex_ = -1;
  d->tabOnTop_ = true;
  d->createButtons_ = false;
  d->indexNeedUpdate_ = false;
  d->firstPanel_ = false;
  d->setupUi();
}

AQTabView::AQTabView( WQWidget * parent, bool tabOnTop, bool createButtons ) :
    WQWidget( *new AQTabViewPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( AQTabView );
  d->tb_ = 0;
  d->currentIndex_ = -1;
  d->tabOnTop_ = tabOnTop;
  d->createButtons_ = createButtons;
  d->indexNeedUpdate_ = false;
  d->firstPanel_ = false;
  d->setupUi();
}

WWidget * AQTabView::ww()
{
  AQ_D( AQTabView );
  return d->ww_;
}

void AQTabView::addWQWidget( WQWidget * w )
{
  addTab( w );
}

WQWidget * AQTabView::currentWidget()
{
  AQ_D( AQTabView );
  if ( AQ_CUR_INDEX > -1 && d->widgets_.size() )
    return d->widgets_[ d->currentIndex_ ];
  else
    return 0;
}

void AQTabView::setTitle( WQWidget * widget, const QString & title ) const
{
  AQ_D( const AQTabView );
  d->ww_->setTabText( d->ww_->indexOf( widget->ww() ->parent() ), toWString( title ) );
}

int AQTabView::count() const
{
  AQ_D( const AQTabView );
  return d->widgets_.count();
}

void AQTabView::clear()
{
  AQ_D( AQTabView );
  while ( d->ww_->count() )
    d-> ww_->removeTab( 0 );
  d->widgets_.clear();
  d->currentIndex_ = -1;
  d->indexNeedUpdate_ = false;
}

Ext::ToolBar * AQTabView::toolBar()
{
  AQ_D( AQTabView );
  if ( !d->tb_ ) {
    d->tb_ = new Ext::ToolBar;
    if ( !d->tabOnTop_ )
      d->ww_->setBottomToolBar( d->tb_ );
    else
      d->ww_->setTopToolBar( d->tb_ );
  }
  return d->tb_;
}

void AQTabView::nextTab()
{
  AQ_D( AQTabView );
  if ( AQ_CUR_INDEX < count() - 1 )
    d->ww_->setCurrentIndex( ++( d->currentIndex_ ) );
}

void AQTabView::previousTab()
{
  AQ_D( AQTabView );
  if ( AQ_CUR_INDEX > 0 )
    d->ww_->setCurrentIndex( --( d->currentIndex_ ) );
}

void AQTabView::setCurrentWidget( WQWidget * widget )
{
  AQ_D( AQTabView );
  int idx = d->ww_->indexOf( widget->ww() ->parent() );
  if ( idx > -1 ) {
    d->currentIndex_ = idx;
    d->ww_->setCurrentIndex( d->currentIndex_ );
  }
}

int AQTabView::insertTab( int, WQWidget * widget, const QString & title )
{
  addTab( widget, title );
  return -1;
}

void AQTabView::updateTitle( const QString & title )
{
  AQ_D( AQTabView );
  if ( AQ_CUR_INDEX > -1 )
    d->ww_->setTabText( d->currentIndex_, toWString( title ) );
}

void AQTabView::transferFocus( int index )
{
  AQ_D( AQTabView );
  d->currentIndex_ = index;
}

void AQTabView::closeTab()
{
  AQ_D( AQTabView );
  if ( AQ_CUR_INDEX > -1 ) {
    d->ww_->removeTab( d->currentIndex_ );
    if ( d->widgets_.size() )
      d->widgets_.removeAt( d->currentIndex_ );
    d->indexNeedUpdate_ = true;
  }
}

void AQTabView::closeTab( WQWidget * widget )
{
  AQ_D( AQTabView );
  int idx = d->ww_->indexOf( widget->ww() ->parent() );
  if ( idx > -1 ) {
    d->ww_->removeTab( idx );
    if ( d->widgets_.size() )
      d->widgets_.removeAt( idx );
    d->indexNeedUpdate_ = true;
  }
}

void AQTabView::addTab()
{
  addTab( new WQWidget, "..." );
}

void AQTabView::addTab( WQWidget * widget, const QString & title )
{
  if ( !widget )
    return ;

  if ( !widget->isWidget() ) {
    aqWarn( "AQTabView::addTab !w->isWidget " << this << widget );
    return ;
  }

  AQ_D( AQTabView );

  if ( widget ) {
    Ext::Panel * tab = new Ext::Panel;

    tab->setLayout( new WFitLayout );
    tab->setTitle( toWString( title ) );
    tab->setAutoScrollBars( true );
    tab->setBorder( false );
    tab->layout() ->addWidget( widget->ww() );

    tab->resize( WLength(), WLength( 100, WLength::Percentage ) );
    d->ww_->addTab( tab );
    addChildWidget( widget );
    d->indexNeedUpdate_ = true;
  }

  if ( d->firstPanel_ ) {
    QTimer::singleShot( 100, this, SLOT( closeFirstTab() ) );
    d->firstPanel_ = false;
  }
}

WQWidget * AQTabView::widget( int index ) const
{
  AQ_D( const AQTabView );
  if ( index > -1 && d->widgets_.size() )
    return d->widgets_[ index ];
  else
    return 0;
}

void AQTabView::hide()
{
  AQ_D( const AQTabView );
  d->ww_->hide();
}

void AQTabView::show()
{
  AQ_D( const AQTabView );
  d->ww_->show();
}

#include "moc_AQTabView.cpp"
