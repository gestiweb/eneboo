/***************************************************************************
WQGridLayout.h
-------------------
begin                : 29/08/2008
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

#ifndef WQGRIDLAYOUT_H_
#define WQGRIDLAYOUT_H_

#include <AQUi/WQWidget.h>

class WQGridLayoutPrivate;

class WQGridLayout : public WQWidget
{
  Q_OBJECT
  AQ_DECLARE_PRIVATE( WQGridLayout )

public :

  explicit WQGridLayout( WQWidget * parent = 0 );

  WLayout * wl();

  void addWQWidget( WQWidget * w );
  void update();
};

#endif /*WQGRIDLAYOUT_H_*/
