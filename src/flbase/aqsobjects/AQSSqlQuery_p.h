/***************************************************************************
 AQSSqlQuery_p.h
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

#ifndef AQSSQLQUERY_P_H_
#define AQSSQLQUERY_P_H_

#include "AQSObject_p.h"
#include "AQObjects.h"

class AQSSqlQuery : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_AQOBJECT(SqlQuery, Object);

public slots:
  void setSelect(const QString &s, const QString &sep = QString::fromLatin1(",")) {
    AQ_CALL_VOID(setSelect(s, sep));
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QString select() const;
  QString from() const;
  QString where() const;
  QString orderBy() const;
  void setFrom(const QString &);
  void setWhere(const QString &);
  void setOrderBy(const QString &);
  QString sql() const;
  QStringList fieldList() const;
  QVariant value(int) const;
  QVariant value(const QString &) const;
  bool isNull(int) const;
  QString posToFieldName(const int) const;
  int fieldNameToPos(const QString &) const;
  void setTablesList(const QString &);
  QVariant valueParam(const QString &) const;
  void setValueParam(const QString &, const QVariant &);
  bool next();
  bool prev();
  bool first();
  bool last();
  bool exec();
  void showDebug();
  int size() const;
  void setForwardOnly(bool);
  bool isForwardOnly() const;
  QSqlError *lastError() const;
  QString lastQuery() const;
  int at() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQSqlQuery;
    if (sgt == QString::fromLatin1("QObject*"))
      return new AQSqlQuery(argValue<QObject *>(args[0]));
    if (sgt == QString::fromLatin1("QObject*,QString"))
      return new AQSqlQuery(argValue<QObject *>(args[0]),
                            *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSSqlQuery::select() const
{
  AQ_CALL_RET_V(select(), QString);
}
inline QString AQSSqlQuery::from() const
{
  AQ_CALL_RET_V(from(), QString);
}
inline QString AQSSqlQuery::where() const
{
  AQ_CALL_RET_V(where(), QString);
}
inline QString AQSSqlQuery::orderBy() const
{
  AQ_CALL_RET_V(orderBy(), QString);
}
inline void AQSSqlQuery::setFrom(const QString &arg0)
{
  AQ_CALL_VOID(setFrom(arg0));
}
inline void AQSSqlQuery::setWhere(const QString &arg0)
{
  AQ_CALL_VOID(setWhere(arg0));
}
inline void AQSSqlQuery::setOrderBy(const QString &arg0)
{
  AQ_CALL_VOID(setOrderBy(arg0));
}
inline QString AQSSqlQuery::sql() const
{
  AQ_CALL_RET_V(sql(), QString);
}
inline QStringList AQSSqlQuery::fieldList() const
{
  AQ_CALL_RET_V(fieldList(), QStringList);
}
inline QVariant AQSSqlQuery::value(int arg0) const
{
  AQ_CALL_RET_V(value(arg0), QVariant);
}
inline QVariant AQSSqlQuery::value(const QString &arg0) const
{
  AQ_CALL_RET_V(value(arg0), QVariant);
}
inline bool AQSSqlQuery::isNull(int arg0) const
{
  AQ_CALL_RET_V(isNull(arg0), bool);
}
inline QString AQSSqlQuery::posToFieldName(const int arg0) const
{
  AQ_CALL_RET_V(posToFieldName(arg0), QString);
}
inline int AQSSqlQuery::fieldNameToPos(const QString &arg0) const
{
  AQ_CALL_RET_V(fieldNameToPos(arg0), int);
}
inline void AQSSqlQuery::setTablesList(const QString &arg0)
{
  AQ_CALL_VOID(setTablesList(arg0));
}
inline QVariant AQSSqlQuery::valueParam(const QString &arg0) const
{
  AQ_CALL_RET_V(valueParam(arg0), QVariant);
}
inline void AQSSqlQuery::setValueParam(const QString &arg0,  const QVariant &arg1)
{
  AQ_CALL_VOID(setValueParam(arg0, arg1));
}
inline bool AQSSqlQuery::next()
{
  AQ_CALL_RET_V(next(), bool);
}
inline bool AQSSqlQuery::prev()
{
  AQ_CALL_RET_V(prev(), bool);
}
inline bool AQSSqlQuery::first()
{
  AQ_CALL_RET_V(first(), bool);
}
inline bool AQSSqlQuery::last()
{
  AQ_CALL_RET_V(last(), bool);
}
inline bool AQSSqlQuery::exec()
{
  AQ_CALL_RET_V(exec(), bool);
}
inline void AQSSqlQuery::showDebug()
{
  AQ_CALL_VOID(showDebug());
}
inline int AQSSqlQuery::size() const
{
  AQ_CALL_RET_V(size(), int);
}
inline void AQSSqlQuery::setForwardOnly(bool arg0)
{
  AQ_CALL_VOID(setForwardOnly(arg0));
}
inline bool AQSSqlQuery::isForwardOnly() const
{
  AQ_CALL_RET_V(isForwardOnly(), bool);
}
inline QSqlError *AQSSqlQuery::lastError() const
{
  AQ_CALL_RET_PTR(lastError(), QSqlError);
}
inline QString AQSSqlQuery::lastQuery() const
{
  AQ_CALL_RET_V(lastQuery(), QString);
}
inline int AQSSqlQuery::at() const
{
  AQ_CALL_RET_V(at(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSQLQUERY_P_H_ */
// @AQOBJECT@
