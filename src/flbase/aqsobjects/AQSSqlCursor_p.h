/***************************************************************************
 AQSSqlCursor_p.h
 -------------------
 begin                : 09/02/2011
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

#ifndef AQSSQLCURSOR_P_H_
#define AQSSQLCURSOR_P_H_

#include "AQSObject_p.h"
#include "AQObjects.h"

class AQSSqlCursor : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_AQOBJECT(SqlCursor, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int modeAccess() const;
  void setModeAccess(int);
  void setValueBuffer(const QString &, const QVariant &);
  QVariant valueBuffer(const QString &);
  QVariant valueBufferCopy(const QString &) const;
  QString mainFilter() const;
  void setMainFilter(const QString &);
  bool transaction(bool = false);
  bool rollback();
  bool commit(bool = true);
  int size() const;
  void setForwardOnly(bool);
  QSqlError *lastError() const;
  QString lastQuery() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQSqlCursor;
    if (sgt == QString::fromLatin1("QString"))
      return new AQSqlCursor(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,QString"))
      return new AQSqlCursor(*(argValue<QString *>(args[0])),
                             *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSSqlCursor::modeAccess() const
{
  AQ_CALL_RET_V(modeAccess(), int);
}
inline void AQSSqlCursor::setModeAccess(int arg0)
{
  AQ_CALL_VOID(setModeAccess(arg0));
}
inline void AQSSqlCursor::setValueBuffer(const QString &arg0,  const QVariant &arg1)
{
  AQ_CALL_VOID(setValueBuffer(arg0, arg1));
}
inline QVariant AQSSqlCursor::valueBuffer(const QString &arg0)
{
  AQ_CALL_RET_V(valueBuffer(arg0), QVariant);
}
inline QVariant AQSSqlCursor::valueBufferCopy(const QString &arg0) const
{
  AQ_CALL_RET_V(valueBufferCopy(arg0), QVariant);
}
inline QString AQSSqlCursor::mainFilter() const
{
  AQ_CALL_RET_V(mainFilter(), QString);
}
inline void AQSSqlCursor::setMainFilter(const QString &arg0)
{
  AQ_CALL_VOID(setMainFilter(arg0));
}
inline bool AQSSqlCursor::transaction(bool arg0)
{
  AQ_CALL_RET_V(transaction(arg0), bool);
}
inline bool AQSSqlCursor::rollback()
{
  AQ_CALL_RET_V(rollback(), bool);
}
inline bool AQSSqlCursor::commit(bool arg0)
{
  AQ_CALL_RET_V(commit(arg0), bool);
}
inline int AQSSqlCursor::size() const
{
  AQ_CALL_RET_V(size(), int);
}
inline void AQSSqlCursor::setForwardOnly(bool arg0)
{
  AQ_CALL_VOID(setForwardOnly(arg0));
}
inline QSqlError *AQSSqlCursor::lastError() const
{
  AQ_CALL_RET_PTR(lastError(), QSqlError);
}
inline QString AQSSqlCursor::lastQuery() const
{
  AQ_CALL_RET_V(lastQuery(), QString);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSQLCURSOR_P_H_ */
// @AQOBJECT@
