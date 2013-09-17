/***************************************************************************
WQBoxLayout_p.h
-------------------
begin                : 21/02/2010
copyright            : (C) 2003-2010 by InfoSiAL S.L.
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

#ifndef WQBOXLAYOUT_P_H_
#define WQBOXLAYOUT_P_H_

#include "AQUi/WQWidget_p.h"

class WQBoxLayoutPrivate : public WQWidgetPrivate
{
  AQ_DECLARE_PUBLIC( WQBoxLayout )

public :

  WQBoxLayoutPrivate();
  ~WQBoxLayoutPrivate();

  void init();

  int indexOf( QLayoutItem * item ) const;

  WBoxLayout * wl_;
  QBoxLayout * qLay_;
  bool vertical_;
};

#endif /* WQBOXLAYOUT_P_H_ */
