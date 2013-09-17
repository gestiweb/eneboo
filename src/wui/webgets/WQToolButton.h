/***************************************************************************
WQToolButton.h
-------------------
begin                : 13/02/2010
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

#ifndef WQTOOLBUTTON_H_
#define WQTOOLBUTTON_H_

#include <AQUi/WQWidget.h>

class WQToolButtonPrivate;

class WQToolButton : public WQWidget
{
  Q_OBJECT
  AQ_DECLARE_PRIVATE( WQToolButton )

public :

  explicit WQToolButton( WQWidget * parent = 0 );

  WWidget * ww();

  void update();

Q_SIGNALS:

  void clicked();

public Q_SLOTS:

  void click();

private:

  AQ_WUI_INVOKABLE( clicked )
};

#endif /* WQTOOLBUTTON_H_ */
