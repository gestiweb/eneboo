/***************************************************************************
WQGroupBox.h
-------------------
begin                : 05/09/2008
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

#ifndef WQGROUPBOX_H_
#define WQGROUPBOX_H_

#include <AQUi/WQWidget.h>

class WQGroupBoxPrivate;

class WQGroupBox : public WQWidget
{
  Q_OBJECT
  AQ_DECLARE_PRIVATE( WQGroupBox )

public :

  explicit WQGroupBox( WQWidget * parent = 0 );

  WWidget * ww();

  void update();
};

#endif /*WQGROUPBOX_H_*/
