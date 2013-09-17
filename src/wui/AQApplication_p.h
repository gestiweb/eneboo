/***************************************************************************
AQApplication_p.h
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

#ifndef AQAPPLICATION_P_H_
#define AQAPPLICATION_P_H_

#include "AQUi/WQWidget_p.h"

class AQApplicationPrivate : public WQWidgetPrivate
{
  AQ_DECLARE_PUBLIC( AQApplication )

public :

  AQApplicationPrivate( WQApplication * wqApp );

  void init();
  void finish();
  void initTabView();
  bool execDialogConnectDB( const QString & connectionName = "default" );
  bool openDefaultConnectionDB( const QString & connectionName = "default" );
  bool closeConnectionDB( const QString & connectionName = "default" );
  void loadTranslation( const QString & prefixFileTs, const QString & connectionName = "default" );

  WQApplication * wqApp_;
  AQTabView * tabView_;
  Ext::Panel * containerPanel_;
  int connCount;
  QString curStyle;
  QString curStyleSheet;
  QDateTime * timeUser_;
  QString sessionId_;
  bool quited_;
  bool interrupt_;

  AQMemCache iconsCache_;
};

Q_DECLARE_METATYPE( QAction* )

#endif /*AQAPPLICATION_P_H_*/
