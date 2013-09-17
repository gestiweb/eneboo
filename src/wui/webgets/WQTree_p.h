/***************************************************************************
WQTree_p.h
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

#ifndef WQTREE_P_H_
#define WQTREE_P_H_

#include "AQUi/WQWidget_p.h"

class WQTreePrivate : public WQWidgetPrivate
{
  AQ_DECLARE_PUBLIC( WQTree )

public:

  WQTreePrivate();
  ~WQTreePrivate();

  void init( const QString & title = QString() );

  void updateTree();
  WTreeNode * addAction( QAction * action, WTreeNode * item );
  void updateAction( QAction * action, WTreeNode * item );
  void refreshText( const QString & text, WTreeNode * item );
  void refreshIcon( const QIcon & icon, WTreeNode * item );

  void setOverrideTreeAction( QAction * );
  void _q_overrideTreeActionDestroyed();

  WTreeNode * tree_;
  WSignalMapper<QAction *> * actionMapper_;
  QList<WTreeNode *> items_;
  QHash<QAction *, WTreeNode *> hashActionsItems_;

  QAction * defaultTreeAction_;
  QAction * treeAction_;

  Signal<QAction *> triggered_;
};

#endif /*WQTREE_P_H_*/
