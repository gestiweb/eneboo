/***************************************************************************
AQTabView_p.h
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

#ifndef AQTABVIEW_P_H_
#define AQTABVIEW_P_H_

#include "AQUi/WQWidget_p.h"

class AQTabViewPrivate : public WQWidgetPrivate
{
  AQ_DECLARE_PUBLIC( AQTabView )

public:

  AQTabViewPrivate();
  ~AQTabViewPrivate();

  void setupUi();
  int updateIndex();

  void closeFirstTab();

  int currentIndex_;
  Ext::TabWidget * ww_;
  Ext::ToolBar * tb_;
  Ext::Button * pbAdd_;
  Ext::Button * pbClose_;

  bool tabOnTop_;
  bool createButtons_;
  bool indexNeedUpdate_;
  bool firstPanel_;
};

#endif /*AQTABVIEW_P_H_*/
