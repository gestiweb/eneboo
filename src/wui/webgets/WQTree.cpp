/***************************************************************************
WQTree.cpp
-------------------
begin                : 12/08/2008
copyright            : (C) 2003-2008 by InfoSiAL S.L.
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

#include <WSignalMapper>
#include <WTreeNode>
#include <WIconPair>
#include <WText>

#include <QAction>
#include <QHash>

#include "../wtqt/WQApplication.h"
#include "WQIcon.h"
#include "WQAction.h"
#include "WQTree.h"
#include "WQTree_p.h"
#include "../AQUi.h"

WQTreePrivate::WQTreePrivate() :
    WQWidgetPrivate()
{
  canContain_ = false;
}

WQTreePrivate::~WQTreePrivate()
{}

void WQTreePrivate::init( const QString & title )
{
  AQ_Q( WQTree );
  tree_ = new WTreeNode( toWString( title ) );
  tree_->setImagePack( "icons/" );
  actionMapper_ = new WSignalMapper<QAction *> ( q );
  WT_SIGNAL( actionMapper_->mapped ).connect( q, &WQTree::triggerAction );
  defaultTreeAction_ = treeAction_ = new WQAction( q );
  treeAction_->setText( title );
  static_cast<WQAction *> ( treeAction_ ) ->tree_ = q;
}

void WQTreePrivate::updateTree()
{
  QAction * action;
  WTreeNode * item;

  for ( int i = 0; i < items_.size(); ++i )
    items_.at( i ) ->hide();

  for ( int i = 0; i < actions_.size(); ++i ) {
    action = actions_.at( i );
    item = addAction( action, hashActionsItems_.value( action ) );
    updateAction( action, item );
  }
}

WTreeNode * WQTreePrivate::addAction( QAction * action, WTreeNode * item )
{
  AQ_Q( WQTree );
  if ( !item ) {
    WQAction * wa = qobject_cast<WQAction *> ( action );
    if ( wa && wa->wtree() ) {
      item = wa->wtree() ->wtree();
      tree_->addChildNode( item );
      refreshText( action->text(), item );
      WT_SIGNAL( wa->wtree()->triggered ).connect( q, &WQTree::triggerAction );
    } else
      item = new WTreeNode( toWString( action->text().remove( "&" ) ), 0, tree_ );

    actionMapper_->mapConnect( WT_SIGNAL( item->label() ->clicked ), action );
    items_ << item;
    hashActionsItems_.insert( action, item );

    refreshIcon( action->icon(), item );
  }

  return item;
}

void WQTreePrivate::updateAction( QAction * action, WTreeNode * item )
{
  if ( !item )
    return ;

  item->setSelectable( action->isEnabled() );

  if ( action->isVisible() )
    item->show();
  else
    item->hide();
}

void WQTreePrivate::refreshText( const QString & text, WTreeNode * item )
{
  if ( item )
    item->label() ->setText( toWString( QString( text ).remove( "&" ) ) );
}

void WQTreePrivate::refreshIcon( const QIcon & icon, WTreeNode * item )
{
  if ( !item || icon.isNull() )
    return ;
  WQIcon wicon( icon );
  WIconPair * iconp = new WIconPair( wicon.icon(), wicon.icon(), false );
  item->setLabelIcon( iconp );
}

void WQTreePrivate::setOverrideTreeAction( QAction * a )
{
  AQ_Q( WQTree );
  QObject::disconnect( treeAction_, SIGNAL( destroyed() ), q, SLOT( _q_overrideTreeActionDestroyed() ) );
  if ( a ) {
    treeAction_ = a;
    QObject::connect( a, SIGNAL( destroyed() ), q, SLOT( _q_overrideTreeActionDestroyed() ) );
  } else
    treeAction_ = defaultTreeAction_;
}

void WQTreePrivate::_q_overrideTreeActionDestroyed()
{
  treeAction_ = defaultTreeAction_;
}

WQTree::WQTree( WObject * parent ) :
    WQWidget( *new WQTreePrivate, parent )
{
  AQ_D( WQTree );
  d->init();
}

WQTree::WQTree( const QString & title, WObject * parent ) :
    WQWidget( *new WQTreePrivate, parent )
{
  AQ_D( WQTree );
  d->init( title );
}

QAction * WQTree::addAction( const QString & text )
{
  QAction * ret = new QAction( text, this );
  addAction( ret );
  return ret;
}

QAction *WQTree::addAction( const QIcon & icon, const QString & text )
{
  QAction * ret = new QAction( icon, text, this );
  addAction( ret );
  return ret;
}

QAction * WQTree::addTree( WQTree * tree )
{
  QAction * action = tree->treeAction();
  addAction( action );
  return action;
}

WQTree * WQTree::addTree( const QString &title )
{
  WQTree * tree = new WQTree( title, this );
  addTree( tree );
  return tree;
}

WQTree * WQTree::addTree( const QIcon & icon, const QString & title )
{
  WQTree * tree = new WQTree( title, this );
  tree->setIcon( icon );
  addTree( tree );
  return tree;
}

QAction * WQTree::insertTree( QAction * before, WQTree * tree )
{
  QAction * action = tree->treeAction();
  insertAction( before, action );
  return action;
}

bool WQTree::isEmpty() const
{
  bool ret = true;
  QList<QAction*> acts( actions() );
  QAction * action;
  for ( int i = 0; ret && i < acts.size(); ++i ) {
    action = acts.at( i );
    if ( !action->isSeparator() && action->isVisible() )
      ret = false;
  }
  return ret;
}

void WQTree::clear()
{
  QList<QAction*> acts( actions() );
  for ( int i = 0; i < acts.size(); i++ )
    WQWidget::removeAction( acts[ i ], false );

  AQ_D( WQTree );
  d->updateTree();
  d->items_.clear();
  d->hashActionsItems_.clear();
}

QAction * WQTree::treeAction() const
{
  return d_func() ->treeAction_;
}

QString WQTree::title() const
{
  return d_func() ->treeAction_->text();
}

void WQTree::setTitle( const QString & text )
{
  AQ_D( WQTree );
  d->treeAction_->setText( text );
  d->refreshText( text, d->hashActionsItems_.value( d->treeAction_ ) );
}

QIcon WQTree::icon() const
{
  return d_func() ->treeAction_->icon();
}

void WQTree::setIcon( const QIcon & icon )
{
  AQ_D( WQTree );
  d->treeAction_->setIcon( icon );
  d->refreshIcon( icon, d->hashActionsItems_.value( d->treeAction_ ) );
}

WTreeNode * WQTree::wtree() const
{
  AQ_D( const WQTree );
  return d->tree_;
}

void WQTree::updateActions()
{
  AQ_D( WQTree );
  d->updateTree();
}

Signal<QAction *>& WQTree::triggered()
{
  AQ_D( WQTree );
  return d->triggered_;
}

void WQTree::triggerAction( QAction * action )
{
  AQ_D( WQTree );
  d->triggered_.emit( action );
}

#include "moc_WQTree.cpp"
