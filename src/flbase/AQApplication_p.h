/***************************************************************************
 AQApplication_p.h
 -------------------
 begin                : 29/03/2011
 copyright            : (C) 2003-2011 by InfoSiAL S.L.
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

class AQApplicationPrivate
{
public:
  AQApplicationPrivate() :
    oldApi_(true),
    aqAppScript_(0),
    proxyDesktop_(0)
  {}

  bool oldApi_;
  QSScript *aqAppScript_;
  QObject *aqAppScriptObject_;
  QWidget *proxyDesktop_;
};

#endif /* AQAPPLICATION_P_H_ */
