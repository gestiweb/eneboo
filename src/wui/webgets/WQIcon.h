/***************************************************************************
WQIcon.h
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

#ifndef WQICON_H_
#define WQICON_H_

#include <QString>

class QIcon;
class QSize;
class QAbstractButton;

class WQIcon
{
public:

  WQIcon( const QIcon & icon, const QSize & size = QSize( 16, 16 ) );
  WQIcon( QAbstractButton * but, const QSize & size = QSize( 16, 16 ) );

  std::string icon() const;

private:

  void saveIcon( const QIcon & icon, QSize size );

  QString icon_;
};

#endif /*WQICON_H_*/
