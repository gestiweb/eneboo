/***************************************************************************
WQMenu.cpp
-------------------
begin                : 06/08/2008
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

#include <Ext/Menu>
#include <Ext/MenuItem>

#include <QAction>
#include <QHash>

#include "../wtqt/WQApplication.h"
#include "WQIcon.h"
#include "WQAction.h"
#include "WQMenu.h"
#include "WQMenu_p.h"
#include "../AQUi.h"

WQMenuPrivate::WQMenuPrivate() :
    WQWidgetPrivate()
{
  canContain_ = false;
}

WQMenuPrivate::~WQMenuPrivate()
{}

void WQMenuPrivate::init()
{
  AQ_Q( WQMenu );
  menu_ = new Ext::Menu;
  actionMapper_ = new WSignalMapper<QAction *> ( q );
  WT_SIGNAL( actionMapper_->mapped ).connect( q, &WQMenu::triggerAction );
  defaultMenuAction_ = menuAction_ = new WQAction( q );
  static_cast<WQAction *> ( menuAction_ ) ->menu_ = q;
}

void WQMenuPrivate::updateMenu()
{
  QAction * action;
  Ext::MenuItem * item;

  for ( int i = 0; i < items_.size(); ++i )
    items_.at( i ) ->hide();

  for ( int i = 0; i < actions_.size(); ++i ) {
    action = actions_.at( i );
    item = addAction( action, hashActionsItems_.value( action ) );
    updateAction( action, item );
  }
}

Ext::MenuItem * WQMenuPrivate::addAction( QAction * action, Ext::MenuItem * item )
{
  if ( action->isSeparator() && !separators_.contains( action ) ) {
    separators_ << action;
    item = new Ext::MenuItem( "Separator" );
    items_ << item;
    hashActionsItems_.insert( action, item );
    menu_->addSeparator();
    return item;
  }

  AQ_Q( WQMenu );

  if ( !item ) {
    item = new Ext::MenuItem( toWString( action->text().remove( "&" ) ) );
    actionMapper_->mapConnect( WT_SIGNAL( item->activated ), action );
    items_ << item;
    hashActionsItems_.insert( action, item );

    WQAction * wa = qobject_cast<WQAction *> ( action );
    if ( wa && wa->wmenu() ) {
      item->setMenu( wa->wmenu() ->wmenu() );
      WT_SIGNAL( wa->wmenu()->triggered ).connect( q, &WQMenu::triggerAction );
    }

    QIcon icon( action->icon() );
    if ( !icon.isNull() )
      item->setIcon( WQIcon( icon ).icon() );

    menu_->add( item );
  }

  return item;
}

void WQMenuPrivate::updateAction( QAction * action, Ext::MenuItem * item )
{
  if ( !item || action->isSeparator() )
    return ;

  item->setEnabled( action->isEnabled() );

  if ( action->isVisible() )
    item->show();
  else
    item->hide();
}

void WQMenuPrivate::setOverrideMenuAction( QAction * a )
{
  AQ_Q( WQMenu );
  QObject::disconnect( menuAction_, SIGNAL( destroyed() ), q, SLOT( _q_overrideMenuActionDestroyed() ) );
  if ( a ) {
    menuAction_ = a;
    QObject::connect( a, SIGNAL( destroyed() ), q, SLOT( _q_overrideMenuActionDestroyed() ) );
  } else
    menuAction_ = defaultMenuAction_;
}

void WQMenuPrivate::_q_overrideMenuActionDestroyed()
{
  menuAction_ = defaultMenuAction_;
}

WQMenu::WQMenu( WObject * parent ) :
    WQWidget( *new WQMenuPrivate, parent )
{
  AQ_D( WQMenu );
  d->init();
}

WQMenu::WQMenu( const QString & title, WObject * parent ) :
    WQWidget( *new WQMenuPrivate, parent )
{
  AQ_D( WQMenu );
  d->init();
  d->menuAction_->setText( title );
}

QAction * WQMenu::addAction( const QString & text )
{
  QAction * ret = new QAction( text, this );
  addAction( ret );
  return ret;
}

QAction *WQMenu::addAction( const QIcon & icon, const QString & text )
{
  QAction * ret = new QAction( icon, text, this );
  addAction( ret );
  return ret;
}

QAction * WQMenu::addMenu( WQMenu * menu )
{
  QAction * action = menu->menuAction();
  addAction( action );
  return action;
}

WQMenu * WQMenu::addMenu( const QString &title )
{
  WQMenu * menu = new WQMenu( title, this );
  addAction( menu->menuAction() );
  return menu;
}

WQMenu * WQMenu::addMenu( const QIcon & icon, const QString & title )
{
  WQMenu * menu = new WQMenu( title, this );
  menu->setIcon( icon );
  addAction( menu->menuAction() );
  return menu;
}

QAction * WQMenu::addSeparator()
{
  QAction * action = new QAction( this );
  action->setSeparator( true );
  addAction( action );
  return action;
}

QAction * WQMenu::insertMenu( QAction * before, WQMenu * menu )
{
  QAction * action = menu->menuAction();
  insertAction( before, action );
  return action;
}

QAction * WQMenu::insertSeparator( QAction *before )
{
  QAction * action = new QAction( this );
  action->setSeparator( true );
  insertAction( before, action );
  return action;
}

bool WQMenu::isEmpty() const
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

void WQMenu::clear()
{
  QList<QAction*> acts( actions() );
  for ( int i = 0; i < acts.size(); i++ )
    WQWidget::removeAction( acts[ i ], false );

  AQ_D( WQMenu );
  d->updateMenu();
  d->items_.clear();
  d->hashActionsItems_.clear();
}

QAction * WQMenu::menuAction() const
{
  return d_func() ->menuAction_;
}

QString WQMenu::title() const
{
  return d_func() ->menuAction_->text();
}

void WQMenu::setTitle( const QString & text )
{
  d_func() ->menuAction_->setText( text );
}

QIcon WQMenu::icon() const
{
  return d_func() ->menuAction_->icon();
}

void WQMenu::setIcon( const QIcon & icon )
{
  d_func() ->menuAction_->setIcon( icon );
}

Ext::Menu * WQMenu::wmenu() const
{
  AQ_D( const WQMenu );
  return d->menu_;
}

void WQMenu::updateActions()
{
  AQ_D( WQMenu );
  d->updateMenu();
}

Signal<QAction *>& WQMenu::triggered()
{
  AQ_D( WQMenu );
  return d->triggered_;
}

void WQMenu::triggerAction( QAction * action )
{
  AQ_D( WQMenu );
  d->triggered_.emit( action );
}

#include "moc_WQMenu.cpp"
