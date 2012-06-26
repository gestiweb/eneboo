/***************************************************************************
 AQSZipWriter_p.h
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

#ifndef AQSZIPWRITER_P_H_
#define AQSZIPWRITER_P_H_

#include "AQSIODevice_p.h"
#include "AQObjects.h"

class AQSZipWriter : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(ZipWriter, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QIODevice *device() const;
  bool isWritable() const;
  bool exists() const;
  uint status() const;
  void setCompressionPolicy(uint);
  uint compressionPolicy() const;
  void setCreationPermissions(uint);
  uint creationPermissions() const;
  void addFile(const QString &, QByteArray *);
  void addFile(const QString &, AQSByteArray *);
  void addFile(const QString &, QIODevice *);
  void addFile(const QString &, AQSIODevice *);
  void addDirectory(const QString &);
  void addSymLink(const QString &, const QString &);
  void close();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,uint"));
    candidates[1].append(QString::fromLatin1("QIODevice*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QString"))
      return new AQZipWriter(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,uint"))
      return new AQZipWriter(*(argValue<QString *>(args[0])),
                             args[1].variant().toUInt());
    if (sgt == QString::fromLatin1("QIODevice*"))
      return new AQZipWriter(argValue<QIODevice *>(args[0]));
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
inline QIODevice *AQSZipWriter::device() const
{
  AQ_CALL_RET(device());
}
inline bool AQSZipWriter::isWritable() const
{
  AQ_CALL_RET_V(isWritable(), bool);
}
inline bool AQSZipWriter::exists() const
{
  AQ_CALL_RET_V(exists(), bool);
}
inline uint AQSZipWriter::status() const
{
  AQ_CALL_RET_V(status(), uint);
}
inline void AQSZipWriter::setCompressionPolicy(uint arg0)
{
  AQ_CALL_VOID(setCompressionPolicy(static_cast<AQZipWriter::CompressionPolicy>(arg0)));
}
inline uint AQSZipWriter::compressionPolicy() const
{
  AQ_CALL_RET_V(compressionPolicy(), uint);
}
inline void AQSZipWriter::setCreationPermissions(uint arg0)
{
  AQ_CALL_VOID(setCreationPermissions(static_cast<QFileInfo::PermissionSpec>(arg0)));
}
inline uint AQSZipWriter::creationPermissions() const
{
  AQ_CALL_RET_V(creationPermissions(), uint);
}
inline void AQSZipWriter::addFile(const QString &arg0,  QByteArray *arg1)
{
  AQ_CALL_VOID(addFile(arg0, *arg1));
}
inline void AQSZipWriter::addFile(const QString &arg0, AQSByteArray *arg1)
{
  AQ_CALL_VOID(addFile(arg0, *arg1));
}
inline void AQSZipWriter::addFile(const QString &arg0,  QIODevice *arg1)
{
  AQ_CALL_VOID(addFile(arg0, arg1));
}
inline void AQSZipWriter::addFile(const QString &arg0, AQSIODevice *arg1)
{
  AQ_CALL_VOID(addFile(arg0, *arg1));
}
inline void AQSZipWriter::addDirectory(const QString &arg0)
{
  AQ_CALL_VOID(addDirectory(arg0));
}
inline void AQSZipWriter::addSymLink(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_VOID(addSymLink(arg0, arg1));
}
inline void AQSZipWriter::close()
{
  AQ_CALL_VOID(close());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSZIPWRITER_P_H_ */
// @AQOBJECT_VOID@
