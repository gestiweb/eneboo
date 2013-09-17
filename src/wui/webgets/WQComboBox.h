/***************************************************************************
WQComboBox.h
-------------------
begin                : 14/02/2010
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

#ifndef WQCOMBOBOX_H_
#define WQCOMBOBOX_H_

#include <AQUi/WQWidget.h>

class WQComboBoxPrivate;

class WQComboBox : public WQWidget
{
  AQ_DECLARE_PRIVATE( WQComboBox )

public :

  explicit WQComboBox( WQWidget * parent = 0 );

  WWidget * ww();

  void update();
};

#endif /* WQCOMBOBOX_H_ */
