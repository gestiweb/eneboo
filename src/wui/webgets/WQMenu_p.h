/***************************************************************************
WQMenu_p.h
-------------------
begin                : 11/08/2008
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

#ifndef WQMENU_P_H_
#define WQMENU_P_H_

#include "AQUi/WQWidget_p.h"

class WQMenuPrivate : public WQWidgetPrivate
{
  AQ_DECLARE_PUBLIC( WQMenu )

public:

  WQMenuPrivate();
  ~WQMenuPrivate();

  void init();

  void updateMenu();
  Ext::MenuItem * addAction( QAction * action, Ext::MenuItem * item );
  void updateAction( QAction * action, Ext::MenuItem * item );

  void setOverrideMenuAction( QAction * );
  void _q_overrideMenuActionDestroyed();

  Ext::Menu * menu_;
  WSignalMapper<QAction *> * actionMapper_;
  QList<Ext::MenuItem *> items_;
  QList<QAction *> separators_;
  QHash<QAction *, Ext::MenuItem *> hashActionsItems_;

  QAction * defaultMenuAction_;
  QAction * menuAction_;

  Signal<QAction *> triggered_;
};

#endif /*WQMENU_P_H_*/
