/***************************************************************************
 AQUnpacker.h
 -------------------
 begin                : 08/03/2011
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

#ifndef AQUNPACKER_H_
#define AQUNPACKER_H_

#include <qstringlist.h>

#define AQPACKAGER_VERSION "AbanQ Packager 2.4.1"

#define AQ_STRERROR(V) \
  errMsgs_ << QString().sprintf("'%s': %s", V.latin1(), strerror(errno))

class QFile;

class AQUnpacker
{
public:
  AQUnpacker(const QString &input);
  ~AQUnpacker();

  QString getText() const;
  QByteArray getBinary() const;

  QStringList errorMessages() const;

private:
  QFile *file_;
  QDataStream *stream_;
  QStringList errMsgs_;
};

#endif /* AQUNPACKER_H_ */
