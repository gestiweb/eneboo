/***************************************************************************
 FLModulesStaticLoader_p.h
 -------------------
 begin                : 08/01/2011
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

#ifndef FLMODULESSTATICLOADER_P_H_
#define FLMODULESSTATICLOADER_P_H_

class FLManagerModules;
class FLStaticLoaderWarning;

struct AQStaticDirInfo {
  inline AQStaticDirInfo(const QString &entry);
  inline AQStaticDirInfo(uint active, const QString &path);
  uint active_: 1;
  QString path_;
};

struct AQStaticBdInfo {
  inline AQStaticBdInfo(const QString &bd) :
    bd_(bd) {
    readSettings();
  }

  AQStaticDirInfo *findPath(const QString &path);
  void readSettings();
  void writeSettings();

  bool enabled_;
  QString bd_;
  QPtrList<AQStaticDirInfo> dirs_;
};

class FLModulesStaticLoader
{
private:

  FLModulesStaticLoader() {}

  static void setup(AQStaticBdInfo *b);
  static QString content(const QString &n, AQStaticBdInfo *b);

  static FLStaticLoaderWarning *warn_;

  friend class FLManagerModules;
};

#endif /* FLMODULESSTATICLOADER_P_H_ */
