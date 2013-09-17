/***************************************************************************
WQTree.h
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

#ifndef WQTREE_H_
#define WQTREE_H_

#include <AQUi/WQWidget.h>

class WQTreePrivate;
class WQAction;
class QAction;

namespace Wt
{
  class WTreeNode;
}

class WQTree : public WQWidget
{
  Q_OBJECT
  AQ_DECLARE_PRIVATE( WQTree )

public:

  explicit WQTree( WObject * parent = 0 );
  explicit WQTree ( const QString & title, WObject * parent = 0 );

  using WQWidget::addAction;
  QAction * addAction( const QString & text );
  QAction * addAction( const QIcon & icon, const QString & text );

  QAction * addTree( WQTree * tree );
  WQTree * addTree( const QString & title );
  WQTree * addTree( const QIcon & icon, const QString & title );

  QAction * insertTree( QAction * before, WQTree * tree );

  bool isEmpty() const;
  void clear();

  QAction * treeAction() const;

  QString title() const;
  void setTitle( const QString & title );

  QIcon icon() const;
  void setIcon( const QIcon & icon );

  WTreeNode * wtree() const;

  Signal<QAction *>& triggered();

private:

  void triggerAction( QAction * action );
  void updateActions();

  Q_PRIVATE_SLOT( d_func(), void _q_overrideTreeActionDestroyed() );

  friend class WQAction;
};

#endif /*WQTREE_H_*/
