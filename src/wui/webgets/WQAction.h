/***************************************************************************
WQAction.h
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

#ifndef WQACTION_H_
#define WQACTION_H_

#include <AQCore/AQObject.h>

#include <QAction>

class WQMenu;
class WQMenuPrivate;
class WQTree;
class WQTreePrivate;

class WQAction : public QAction
{
  Q_OBJECT

public:

  explicit WQAction ( QObject * parent );
  WQAction( const QString & text, QObject * parent );
  WQAction( const QIcon & icon, const QString & text, QObject * parent );

  WQMenu * wmenu() const;
  void setWMenu( WQMenu * menu );

  WQTree * wtree() const;
  void setWTree( WQTree * tree );

private:

  WQMenu * menu_;
  WQTree * tree_;

  friend class WQMenuPrivate;
  friend class WQTreePrivate;
};

#endif /*WQACTION_H_*/
