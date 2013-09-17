/***************************************************************************
AQConfig.h
-------------------
begin                : 09/08/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
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

#ifndef AQCONFIG_H_
#define AQCONFIG_H_

#include <QString>

#define  AQ_DIR_MOD AQConfig::aq_dir_mod
#define  AQ_HOME    AQConfig::aq_home
#define  AQDATA     AQConfig::aqdata
#define  AQPREFIX   AQConfig::aqprefix
#define  AQQTDIR    AQConfig::aqqtdir
#define  AQVERSION  AQConfig::aqversion

class AQConfig
{
public:

  QString keybase() const;

  static void init();

  static QString aq_dir_mod;
  static QString aq_home;
  static QString aqdata;
  static QString aqprefix;
  static QString aqqtdir;
  static QString aqversion;
};

#endif /*AQCONFIG_H_*/
