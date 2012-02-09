/***************************************************************************
 AQSZipReader_p.h
 -------------------
 begin                : 12/12/2011
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

#ifndef AQSZIPREADER_P_H_
#define AQSZIPREADER_P_H_

#include "AQSIODevice_p.h"
#include "AQObjects.h"

class AQSZipReader : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(ZipReader, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QIODevice *device() const;
  bool isReadable() const;
  bool exists() const;
  int count() const;
  QByteArray fileData(const QString &) const;
  bool extractAll(const QString &) const;
  uint status() const;
  void close();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,uint"));
    candidates[1].append(QString::fromLatin1("QIODevice*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QString"))
      return new AQZipReader(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,uint"))
      return new AQZipReader(*(argValue<QString *>(args[0])),
                             args[1].variant().toUInt());
    if (sgt == QString::fromLatin1("QIODevice*"))
      return new AQZipReader(argValue<QIODevice *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,uint"));
    candidates[1].append(QString::fromLatin1("QIODevice*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QIODevice *AQSZipReader::device() const
{
  AQ_CALL_RET(device());
}
inline bool AQSZipReader::isReadable() const
{
  AQ_CALL_RET_V(isReadable(), bool);
}
inline bool AQSZipReader::exists() const
{
  AQ_CALL_RET_V(exists(), bool);
}
inline int AQSZipReader::count() const
{
  AQ_CALL_RET_V(count(), int);
}
inline QByteArray AQSZipReader::fileData(const QString &arg0) const
{
  AQ_CALL_RET_V(fileData(arg0), QByteArray);
}
inline bool AQSZipReader::extractAll(const QString &arg0) const
{
  AQ_CALL_RET_V(extractAll(arg0), bool);
}
inline uint AQSZipReader::status() const
{
  AQ_CALL_RET_V(status(), uint);
}
inline void AQSZipReader::close()
{
  AQ_CALL_VOID(close());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSZIPREADER_P_H_ */
// @AQOBJECT_VOID@
