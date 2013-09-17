/***************************************************************************
WQIcon.cpp
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

#include <QIcon>
#include <QFile>
#include <QPixmap>
#include <QSize>
#include <QBuffer>
#include <QAbstractButton>

#include <AQCore/AQCoreUtil.h>
#include <AQUi/AQInvokerThreadContext.h>

#include "../AQApplication.h"
#include "../AQUi.h"
#include "WQIcon.h"

WQIcon::WQIcon( const QIcon & icon, const QSize & size )
{
  if ( !icon.isNull() ) {

    QString key( QString::number( icon.cacheKey() ) );

    icon_ = AQApplicationWeb::findIconCache( key );
    if ( !icon_.isEmpty() )
      return ;

    if ( !AQ_ISGUI_THREAD ) {
      AQ_INVOKER<WQIcon, AQNoClass, QIcon, QSize> ivk;
      ivk.invoke( this, &WQIcon::saveIcon, icon, size );
    } else
      saveIcon( icon, size );

    AQApplicationWeb::insertIconCache( key, icon_ );

  }
}

WQIcon::WQIcon( QAbstractButton * but, const QSize & size )
{
  if ( but ) {
    QString key( but->objectName() );

    icon_ = AQApplicationWeb::findIconCache( key );
    if ( !icon_.isEmpty() )
      return ;

    icon_ = "images/iconscache/" + key + ".png";

    if ( !QFile::exists( icon_ ) ) {
      QIcon icon( but->icon() );

      if ( !icon.isNull() ) {
        if ( !AQ_ISGUI_THREAD ) {
          AQ_INVOKER<WQIcon, AQNoClass, QIcon, QSize> ivk;
          ivk.invoke( this, &WQIcon::saveIcon, icon, size );
        } else
          saveIcon( icon, size );
      }
    }

    AQApplicationWeb::insertIconCache( key, icon_ );

  }
}

std::string WQIcon::icon() const
{
  return icon_.toStdString();
}

void WQIcon::saveIcon( const QIcon & icon, QSize size )
{
  if ( icon_.isEmpty() ) {
    QByteArray ba;
    QBuffer bu( &ba );
    bu.open( QIODevice::WriteOnly );
    QPixmap pix( icon.pixmap( size ) );
    pix.save( &bu, "PNG" );
    bu.close();
    icon_ = "images/iconscache/" + AQCoreUtil::sha1( ba ) + ".png";
    if ( !QFile::exists( icon_ ) )
      pix.save( icon_ );
  } else
    icon.pixmap( size ).save( icon_ );
}
