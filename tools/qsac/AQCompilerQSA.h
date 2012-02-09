/***************************************************************************
 AQCompilerQSA.h
 -------------------
 begin                : 06/03/2011
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

#ifndef AQCOMPILERQSA_H_
#define AQCOMPILERQSA_H_

#include <qstringlist.h>

#define AQQSAC_VERSION "AbanQ qsac 2.4.0"

class AQCompilerQSA
{
public:
  QByteArray byteCode(const QString &, int = 0, int = 0, bool = false) { return QByteArray(); }
  int errorType() const { return 0; }
  QValueList<int> errorLines() const { return QValueList<int>(); }
  QStringList errorMessages() const { return QStringList(); }
  static QString code(const QByteArray &) { return QString::null; }
};

#endif /*AQCOMPILERQSA_H_*/
