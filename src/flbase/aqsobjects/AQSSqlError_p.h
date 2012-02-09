/***************************************************************************
 AQSSqlError_p.h
 -------------------
 begin                : 20/11/2011
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

#ifndef AQSSQLERROR_P_H_
#define AQSSQLERROR_P_H_

#include "AQSVoidPtr_p.h"
#include <qsqlerror.h>

// @AQ_PTR_INDIRECTION@

class AQSSqlError : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(SqlError, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QString driverText() const;
  virtual void setDriverText(const QString &);
  QString databaseText() const;
  virtual void setDatabaseText(const QString &);
  int type() const;
  virtual void setType(int);
  int number() const;
  virtual void setNumber(int);
  QString text() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QString"));
    candidates[3].append(QString::fromLatin1("QString,QString,int"));
    candidates[4].append(QString::fromLatin1("QString,QString,int,int"));
    candidates[1].append(QString::fromLatin1("QSqlError*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QSqlError;
    if (sgt == QString::fromLatin1("QString"))
      return new QSqlError(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,QString"))
      return new QSqlError(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QString,QString,int"))
      return new QSqlError(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toInt());
    if (sgt == QString::fromLatin1("QString,QString,int,int"))
      return new QSqlError(*(argValue<QString *>(args[0])),
                           *(argValue<QString *>(args[1])),
                           args[2].variant().toInt(),
                           args[3].variant().toInt());
    if (sgt == QString::fromLatin1("QSqlError*"))
      return new QSqlError(*argValue<QSqlError *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QString"));
    candidates[3].append(QString::fromLatin1("QString,QString,int"));
    candidates[4].append(QString::fromLatin1("QString,QString,int,int"));
    candidates[1].append(QString::fromLatin1("QSqlError*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSSqlError::driverText() const
{
  AQ_CALL_RET_V(driverText(), QString);
}
inline void AQSSqlError::setDriverText(const QString &arg0)
{
  AQ_CALL_VOID(setDriverText(arg0));
}
inline QString AQSSqlError::databaseText() const
{
  AQ_CALL_RET_V(databaseText(), QString);
}
inline void AQSSqlError::setDatabaseText(const QString &arg0)
{
  AQ_CALL_VOID(setDatabaseText(arg0));
}
inline int AQSSqlError::type() const
{
  AQ_CALL_RET_V(type(), int);
}
inline void AQSSqlError::setType(int arg0)
{
  AQ_CALL_VOID(setType(arg0));
}
inline int AQSSqlError::number() const
{
  AQ_CALL_RET_V(number(), int);
}
inline void AQSSqlError::setNumber(int arg0)
{
  AQ_CALL_VOID(setNumber(arg0));
}
inline QString AQSSqlError::text() const
{
  AQ_CALL_RET_V(text(), QString);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSQLERROR_P_H_ */
// @AQSVOIDPTR@
