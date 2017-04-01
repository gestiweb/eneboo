/***************************************************************************
 AQSUrlInfo_p.h
 -------------------
 begin                : 07/11/2011
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

#ifndef AQSURLINFO_P_H_
#define AQSURLINFO_P_H_

#include "AQSVoidPtr_p.h"
#include <qurlinfo.h>

// @AQ_PTR_INDIRECTION@

class AQSUrlInfo : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(UrlInfo, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

public slots:
  QString urlName() const {
    AQ_CALL_RET_V(name(), QString);
  }
  bool urlIsValid() const {
    AQ_CALL_RET_V(isValid(), bool);
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual void setName(const QString &);
  virtual void setDir(bool);
  virtual void setFile(bool);
  virtual void setSymLink(bool);
  virtual void setOwner(const QString &);
  virtual void setGroup(const QString &);
  virtual void setSize(uint);
  virtual void setWritable(bool);
  virtual void setReadable(bool);
  virtual void setPermissions(int);
  virtual void setLastModified(const QDateTime &);
  bool isValid() const;
  QString name() const;
  int permissions() const;
  QString owner() const;
  QString group() const;
  uint size() const;
  QDateTime lastModified() const;
  QDateTime lastRead() const;
  bool isDir() const;
  bool isFile() const;
  bool isSymLink() const;
  bool isWritable() const;
  bool isReadable() const;
  bool isExecutable() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[2].append(QString::fromLatin1("QUrlOperator,QString"));
    candidates[1].append(QString::fromLatin1("QUrlInfo"));
    candidates[13].append(QString::fromLatin1("QString,int,QString,QString,uint,QDateTime,QDateTime,bool,bool,bool,bool,bool,bool"));
    candidates[13].append(QString::fromLatin1("QUrl*,int,QString,QString,uint,QDateTime,QDateTime,bool,bool,bool,bool,bool,bool"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QUrlInfo;
    if (sgt == QString::fromLatin1("QUrlOperator,QString"))
      return new QUrlInfo(*(argValue<QUrlOperator *>(args[0])),
                          *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QUrlInfo"))
      return new QUrlInfo(*(argValue<QUrlInfo *>(args[0])));
    if (sgt == QString::fromLatin1("QString,int,QString,QString,uint,QDateTime,QDateTime,bool,bool,bool,bool,bool,bool"))
      return new QUrlInfo(*(argValue<QString *>(args[0])),
                          args[1].variant().toInt(),
                          *(argValue<QString *>(args[2])),
                          *(argValue<QString *>(args[3])),
                          args[4].variant().toUInt(),
                          *(argValue<QDateTime *>(args[5])),
                          *(argValue<QDateTime *>(args[6])),
                          args[7].variant().toBool(),
                          args[8].variant().toBool(),
                          args[9].variant().toBool(),
                          args[10].variant().toBool(),
                          args[11].variant().toBool(),
                          args[12].variant().toBool());
    if (sgt == QString::fromLatin1("QUrl*,int,QString,QString,uint,QDateTime,QDateTime,bool,bool,bool,bool,bool,bool"))
      return new QUrlInfo(*argValue<QUrl *>(args[0]),
                          args[1].variant().toInt(),
                          *(argValue<QString *>(args[2])),
                          *(argValue<QString *>(args[3])),
                          args[4].variant().toUInt(),
                          *(argValue<QDateTime *>(args[5])),
                          *(argValue<QDateTime *>(args[6])),
                          args[7].variant().toBool(),
                          args[8].variant().toBool(),
                          args[9].variant().toBool(),
                          args[10].variant().toBool(),
                          args[11].variant().toBool(),
                          args[12].variant().toBool());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[2].append(QString::fromLatin1("QUrlOperator,QString"));
    candidates[1].append(QString::fromLatin1("QUrlInfo"));
    candidates[13].append(QString::fromLatin1("QString,int,QString,QString,uint,QDateTime,QDateTime,bool,bool,bool,bool,bool,bool"));
    candidates[13].append(QString::fromLatin1("QUrl*,int,QString,QString,uint,QDateTime,QDateTime,bool,bool,bool,bool,bool,bool"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSUrlInfo::setName(const QString &arg0)
{
  AQ_CALL_VOID(setName(arg0));
}
inline void AQSUrlInfo::setDir(bool arg0)
{
  AQ_CALL_VOID(setDir(arg0));
}
inline void AQSUrlInfo::setFile(bool arg0)
{
  AQ_CALL_VOID(setFile(arg0));
}
inline void AQSUrlInfo::setSymLink(bool arg0)
{
  AQ_CALL_VOID(setSymLink(arg0));
}
inline void AQSUrlInfo::setOwner(const QString &arg0)
{
  AQ_CALL_VOID(setOwner(arg0));
}
inline void AQSUrlInfo::setGroup(const QString &arg0)
{
  AQ_CALL_VOID(setGroup(arg0));
}
inline void AQSUrlInfo::setSize(uint arg0)
{
  AQ_CALL_VOID(setSize(arg0));
}
inline void AQSUrlInfo::setWritable(bool arg0)
{
  AQ_CALL_VOID(setWritable(arg0));
}
inline void AQSUrlInfo::setReadable(bool arg0)
{
  AQ_CALL_VOID(setReadable(arg0));
}
inline void AQSUrlInfo::setPermissions(int arg0)
{
  AQ_CALL_VOID(setPermissions(arg0));
}
inline void AQSUrlInfo::setLastModified(const QDateTime &arg0)
{
  AQ_CALL_VOID(setLastModified(arg0));
}
inline bool AQSUrlInfo::isValid() const
{
  AQ_CALL_RET_V(isValid(), bool);
}
inline QString AQSUrlInfo::name() const
{
  AQ_CALL_RET_V(name(), QString);
}
inline int AQSUrlInfo::permissions() const
{
  AQ_CALL_RET_V(permissions(), int);
}
inline QString AQSUrlInfo::owner() const
{
  AQ_CALL_RET_V(owner(), QString);
}
inline QString AQSUrlInfo::group() const
{
  AQ_CALL_RET_V(group(), QString);
}
inline uint AQSUrlInfo::size() const
{
  AQ_CALL_RET_V(size(), uint);
}
inline QDateTime AQSUrlInfo::lastModified() const
{
  AQ_CALL_RET_V(lastModified(), QDateTime);
}
inline QDateTime AQSUrlInfo::lastRead() const
{
  AQ_CALL_RET_V(lastRead(), QDateTime);
}
inline bool AQSUrlInfo::isDir() const
{
  AQ_CALL_RET_V(isDir(), bool);
}
inline bool AQSUrlInfo::isFile() const
{
  AQ_CALL_RET_V(isFile(), bool);
}
inline bool AQSUrlInfo::isSymLink() const
{
  AQ_CALL_RET_V(isSymLink(), bool);
}
inline bool AQSUrlInfo::isWritable() const
{
  AQ_CALL_RET_V(isWritable(), bool);
}
inline bool AQSUrlInfo::isReadable() const
{
  AQ_CALL_RET_V(isReadable(), bool);
}
inline bool AQSUrlInfo::isExecutable() const
{
  AQ_CALL_RET_V(isExecutable(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSURLINFO_P_H_ */
// @AQSVOIDPTR@
