/***************************************************************************
WQAction.cpp
-------------------
begin                : 09/08/2008
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
#include <WTree>
#include <WTreeNode>

#include <Ext/Menu>
#include <Ext/MenuItem>

#include "WQAction.h"
#include "WQMenu.h"
#include "WQMenu_p.h"
#include "WQTree.h"
#include "WQTree_p.h"

WQAction::WQAction( QObject * parent ) :
    QAction( parent ), menu_( 0 ), tree_( 0 )
{}

WQAction::WQAction( const QString & text, QObject * parent ) :
    QAction( text, parent ), menu_( 0 ), tree_( 0 )
{}

WQAction::WQAction( const QIcon & icon, const QString & text, QObject * parent ) :
    QAction( icon, text, parent ), menu_( 0 ), tree_( 0 )
{}

WQMenu * WQAction::wmenu() const
{
  return menu_;
}

void WQAction::setWMenu( WQMenu * menu )
{
  if ( menu_ )
    menu_->d_func() ->setOverrideMenuAction( 0 );
  menu_ = menu;
  if ( menu )
    menu->d_func() ->setOverrideMenuAction( this );
}

WQTree * WQAction::wtree() const
{
  return tree_;
}

void WQAction::setWTree( WQTree * tree )
{
  if ( tree_ )
    tree_->d_func() ->setOverrideTreeAction( 0 );
  tree_ = tree;
  if ( tree )
    tree->d_func() ->setOverrideTreeAction( this );
}
