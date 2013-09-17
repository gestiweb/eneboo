/***************************************************************************
WQMenu.h
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

#ifndef WQMENU_H_
#define WQMENU_H_

#include <AQUi/WQWidget.h>

class WQMenuPrivate;
class WQAction;
class QAction;

namespace Wt
{
  namespace Ext
  {
    class Menu;
  }
}

class WQMenu : public WQWidget
{
  Q_OBJECT
  AQ_DECLARE_PRIVATE( WQMenu )

public :

  explicit WQMenu( WObject * parent = 0 );
  explicit WQMenu( const QString & title, WObject * parent = 0 );

  using WQWidget::addAction;
  QAction * addAction( const QString & text );
  QAction * addAction( const QIcon & icon, const QString & text );

  QAction * addMenu( WQMenu * menu );
  WQMenu * addMenu( const QString & title );
  WQMenu * addMenu( const QIcon & icon, const QString & title );

  QAction * addSeparator();

  QAction * insertMenu( QAction * before, WQMenu * menu );
  QAction * insertSeparator( QAction * before );

  bool isEmpty() const;
  void clear();

  QAction * menuAction() const;

  QString title() const;
  void setTitle( const QString & title );

  QIcon icon() const;
  void setIcon( const QIcon & icon );

  Ext::Menu * wmenu() const;

  Signal<QAction *>& triggered();

private:

  void triggerAction( QAction * action );
  void updateActions();

  Q_PRIVATE_SLOT( d_func(), void _q_overrideMenuActionDestroyed() );

  friend class WQAction;
};

#endif /*WQMENU_H_*/
