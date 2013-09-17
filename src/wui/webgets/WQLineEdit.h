/***************************************************************************
WQLineEdit.h
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

#ifndef WQLINEEDIT_H_
#define WQLINEEDIT_H_

#include <AQUi/WQWidget.h>

class WQLineEditPrivate;

class WQLineEdit : public WQWidget
{
  Q_OBJECT
  AQ_DECLARE_PRIVATE( WQLineEdit )

public :

  explicit WQLineEdit( WQWidget * parent = 0 );

  WWidget * ww();

  void update();

public Q_SLOTS:

  void setText( const QString & text );

Q_SIGNALS:

  void textChanged( const QString & );

private Q_SLOTS:

  void changed();

private:

  AQ_WUI_INVOKABLE( changed )
};

#endif /*WQLINEEDIT_H_*/
