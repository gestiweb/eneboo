/***************************************************************************
 AQSSqlSelectCursor_p.h
 -------------------
 begin                : 22/03/2012
 copyright            : (C) 2003-2012 by InfoSiAL S.L.
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

#ifndef AQSSQLSELECTCURSOR_P_H_
#define AQSSQLSELECTCURSOR_P_H_

#include "AQSVoidPtr_p.h"
#include <qsqlselectcursor.h>

class AQSSqlSelectCursor : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(SqlSelectCursor, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

public slots:
  QSqlCursor *qSqlCursor() {
    wrap_ = true;
    return static_cast<QSqlCursor *>(o_);
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool exec(const QString &);
  bool select();
  QVariant value(int) const;
  QVariant value(const QString &) const;
  bool isNull(int) const;
  bool isNull(const QString &) const;
  QSqlError *lastError() const;
  bool isValid() const;
  bool isActive() const;
  int at() const;
  QString lastQuery() const;
  int size() const;
  bool isForwardOnly() const;
  void setForwardOnly(bool);
  bool seek(int, bool = false);
  bool next();
  bool prev();
  bool first();
  bool last();
  void changePos(const QString &, int = 0);
  void setOrder(const QStringList &);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QSqlDatabase*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QSqlSelectCursor;
    if (sgt == QString::fromLatin1("QString"))
      return new QSqlSelectCursor(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,QSqlDatabase*"))
      return new QSqlSelectCursor(*(argValue<QString *>(args[0])),
                                  argValue<QSqlDatabase *>(args[1]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QSqlDatabase*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSSqlSelectCursor::exec(const QString &arg0)
{
  AQ_CALL_RET_V(exec(arg0), bool);
}
inline bool AQSSqlSelectCursor::select()
{
  AQ_CALL_RET_V(select(), bool);
}
inline QVariant AQSSqlSelectCursor::value(int arg0) const
{
  AQ_CALL_RET_V(value(arg0), QVariant);
}
inline QVariant AQSSqlSelectCursor::value(const QString &arg0) const
{
  AQ_CALL_RET_V(value(arg0), QVariant);
}
inline bool AQSSqlSelectCursor::isNull(int arg0) const
{
  AQ_CALL_RET_V(isNull(arg0), bool);
}
inline bool AQSSqlSelectCursor::isNull(const QString &arg0) const
{
  AQ_CALL_RET_V(isNull(arg0), bool);
}
inline QSqlError *AQSSqlSelectCursor::lastError() const
{
  AQ_CALL_RET_PTR(lastError(), QSqlError);
}
inline bool AQSSqlSelectCursor::isValid() const
{
  AQ_CALL_RET_V(isValid(), bool);
}
inline bool AQSSqlSelectCursor::isActive() const
{
  AQ_CALL_RET_V(isActive(), bool);
}
inline int AQSSqlSelectCursor::at() const
{
  AQ_CALL_RET_V(at(), int);
}
inline QString AQSSqlSelectCursor::lastQuery() const
{
  AQ_CALL_RET_V(lastQuery(), QString);
}
inline int AQSSqlSelectCursor::size() const
{
  AQ_CALL_RET_V(size(), int);
}
inline bool AQSSqlSelectCursor::isForwardOnly() const
{
  AQ_CALL_RET_V(isForwardOnly(), bool);
}
inline void AQSSqlSelectCursor::setForwardOnly(bool arg0)
{
  AQ_CALL_VOID(setForwardOnly(arg0));
}
inline bool AQSSqlSelectCursor::seek(int arg0,  bool arg1)
{
  AQ_CALL_RET_V(seek(arg0, arg1), bool);
}
inline bool AQSSqlSelectCursor::next()
{
  AQ_CALL_RET_V(next(), bool);
}
inline bool AQSSqlSelectCursor::prev()
{
  AQ_CALL_RET_V(prev(), bool);
}
inline bool AQSSqlSelectCursor::first()
{
  AQ_CALL_RET_V(first(), bool);
}
inline bool AQSSqlSelectCursor::last()
{
  AQ_CALL_RET_V(last(), bool);
}
inline void AQSSqlSelectCursor::changePos(const QString &arg0,  int arg1)
{
  AQ_CALL_VOID(changePos(arg0, arg1));
}
inline void AQSSqlSelectCursor::setOrder(const QStringList &arg0)
{
  AQ_CALL_VOID(setOrder(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSQLSELECTCURSOR_P_H_ */
// @AQSVOIDPTR@
