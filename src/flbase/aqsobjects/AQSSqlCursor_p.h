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

public slots:
  QSqlCursor *qSqlCursor() const {
    return static_cast<QSqlCursor *>(o_);
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  FLTableMetaData *metadata() const;
  int modeAccess() const;
  void setModeAccess(int);
  void setAtomicValueBuffer(const QString &, const QString &);
  void setValueBuffer(const QString &, const QVariant &);
  QVariant valueBuffer(const QString &);
  QVariant valueBufferCopy(const QString &) const;
  QString mainFilter() const;
  FLAction *action() const;
  void setAction(FLAction *);
  void setMainFilter(const QString &, bool = true);
  bool transaction(bool = false);
  bool rollback();
  bool commit(bool = true);
  int size() const;
  void setForwardOnly(bool);
  bool isForwardOnly() const;
  QSqlError *lastError() const;
  QString lastQuery() const;
  void openFormInMode(int, bool = true);
  void updateBufferCopy();
  bool isModifiedBuffer();
  void setEdition(bool);
  void setBrowse(bool);
  bool fieldDisabled(const QString &);
  bool inTransaction();
  void setAskForCancelChanges(bool);
  void setActivatedCheckIntegrity(bool);
  bool activatedCheckIntegrity() const;
  void setActivatedCommitActions(bool);
  bool activatedCommitActions() const;
  QString msgCheckIntegrity();
  bool checkIntegrity(bool = true);
  FLSqlCursor *cursorRelation() const;
  FLRelationMetaData *relation() const;
  void setUnLock(const QString &, bool);
  bool isLocked();
  bool bufferIsNull(int) const;
  bool bufferIsNull(const QString &) const;
  void bufferSetNull(int) const;
  void bufferSetNull(const QString &) const;
  bool bufferCopyIsNull(int) const;
  bool bufferCopyIsNull(const QString &) const;
  void bufferCopySetNull(int) const;
  void bufferCopySetNull(const QString &) const;
  int atFrom();
  int atFromBinarySearch(const QString &, const QString &, bool = true);
  bool exec(const QString &);
  FLSqlDatabase *db() const;
  int at() const;
  QVariant value(int) const;
  QVariant value(const QString &) const;
  void setValue(int, const QVariant &);
  void setValue(const QString &, const QVariant &);
  QString baseFilter();
  QString curFilter();
  QString filter() const;
  QString connectionName() const;
  QString curName() const;
  void changePos(const QString &, int = 0);
  void setOrder(const QStringList &);

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
inline FLTableMetaData *AQSSqlCursor::metadata() const
{
  AQ_CALL_RET(metadata());
}
inline int AQSSqlCursor::modeAccess() const
{
  AQ_CALL_RET_V(modeAccess(), int);
}
inline void AQSSqlCursor::setModeAccess(int arg0)
{
  AQ_CALL_VOID(setModeAccess(arg0));
}
inline void AQSSqlCursor::setAtomicValueBuffer(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setAtomicValueBuffer(arg0, arg1));
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
inline FLAction *AQSSqlCursor::action() const
{
  AQ_CALL_RET(action());
}
inline void AQSSqlCursor::setAction(FLAction *arg0)
{
  AQ_CALL_VOID(setAction(arg0));
}
inline void AQSSqlCursor::setMainFilter(const QString &arg0,  bool arg1)
{
  AQ_CALL_VOID(setMainFilter(arg0, arg1));
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
inline bool AQSSqlCursor::isForwardOnly() const
{
  AQ_CALL_RET_V(isForwardOnly(), bool);
}
inline QSqlError *AQSSqlCursor::lastError() const
{
  AQ_CALL_RET_PTR(lastError(), QSqlError);
}
inline QString AQSSqlCursor::lastQuery() const
{
  AQ_CALL_RET_V(lastQuery(), QString);
}
inline void AQSSqlCursor::openFormInMode(int arg0,  bool arg1)
{
  AQ_CALL_VOID(openFormInMode(arg0, arg1));
}
inline void AQSSqlCursor::updateBufferCopy()
{
  AQ_CALL_VOID(updateBufferCopy());
}
inline bool AQSSqlCursor::isModifiedBuffer()
{
  AQ_CALL_RET_V(isModifiedBuffer(), bool);
}
inline void AQSSqlCursor::setEdition(bool arg0)
{
  AQ_CALL_VOID(setEdition(arg0));
}
inline void AQSSqlCursor::setBrowse(bool arg0)
{
  AQ_CALL_VOID(setBrowse(arg0));
}
inline bool AQSSqlCursor::fieldDisabled(const QString &arg0)
{
  AQ_CALL_RET_V(fieldDisabled(arg0), bool);
}
inline bool AQSSqlCursor::inTransaction()
{
  AQ_CALL_RET_V(inTransaction(), bool);
}
inline void AQSSqlCursor::setAskForCancelChanges(bool arg0)
{
  AQ_CALL_VOID(setAskForCancelChanges(arg0));
}
inline void AQSSqlCursor::setActivatedCheckIntegrity(bool arg0)
{
  AQ_CALL_VOID(setActivatedCheckIntegrity(arg0));
}
inline bool AQSSqlCursor::activatedCheckIntegrity() const
{
  AQ_CALL_RET_V(activatedCheckIntegrity(), bool);
}
inline void AQSSqlCursor::setActivatedCommitActions(bool arg0)
{
  AQ_CALL_VOID(setActivatedCommitActions(arg0));
}
inline bool AQSSqlCursor::activatedCommitActions() const
{
  AQ_CALL_RET_V(activatedCommitActions(), bool);
}
inline QString AQSSqlCursor::msgCheckIntegrity()
{
  AQ_CALL_RET_V(msgCheckIntegrity(), QString);
}
inline bool AQSSqlCursor::checkIntegrity(bool arg0)
{
  AQ_CALL_RET_V(checkIntegrity(arg0), bool);
}
inline FLSqlCursor *AQSSqlCursor::cursorRelation() const
{
  AQ_CALL_RET(cursorRelation());
}
inline FLRelationMetaData *AQSSqlCursor::relation() const
{
  AQ_CALL_RET(relation());
}
inline void AQSSqlCursor::setUnLock(const QString &arg0,  bool arg1)
{
  AQ_CALL_VOID(setUnLock(arg0, arg1));
}
inline bool AQSSqlCursor::isLocked()
{
  AQ_CALL_RET_V(isLocked(), bool);
}
inline bool AQSSqlCursor::bufferIsNull(int arg0) const
{
  AQ_CALL_RET_V(bufferIsNull(arg0), bool);
}
inline bool AQSSqlCursor::bufferIsNull(const QString &arg0) const
{
  AQ_CALL_RET_V(bufferIsNull(arg0), bool);
}
inline void AQSSqlCursor::bufferSetNull(int arg0) const
{
  AQ_CALL_VOID(bufferSetNull(arg0));
}
inline void AQSSqlCursor::bufferSetNull(const QString &arg0) const
{
  AQ_CALL_VOID(bufferSetNull(arg0));
}
inline bool AQSSqlCursor::bufferCopyIsNull(int arg0) const
{
  AQ_CALL_RET_V(bufferCopyIsNull(arg0), bool);
}
inline bool AQSSqlCursor::bufferCopyIsNull(const QString &arg0) const
{
  AQ_CALL_RET_V(bufferCopyIsNull(arg0), bool);
}
inline void AQSSqlCursor::bufferCopySetNull(int arg0) const
{
  AQ_CALL_VOID(bufferCopySetNull(arg0));
}
inline void AQSSqlCursor::bufferCopySetNull(const QString &arg0) const
{
  AQ_CALL_VOID(bufferCopySetNull(arg0));
}
inline int AQSSqlCursor::atFrom()
{
  AQ_CALL_RET_V(atFrom(), int);
}
inline int AQSSqlCursor::atFromBinarySearch(const QString &arg0,  const QString &arg1,  bool arg2)
{
  AQ_CALL_RET_V(atFromBinarySearch(arg0, arg1, arg2), int);
}
inline bool AQSSqlCursor::exec(const QString &arg0)
{
  AQ_CALL_RET_V(exec(arg0), bool);
}
inline FLSqlDatabase *AQSSqlCursor::db() const
{
  AQ_CALL_RET(db());
}
inline int AQSSqlCursor::at() const
{
  AQ_CALL_RET_V(at(), int);
}
inline QVariant AQSSqlCursor::value(int arg0) const
{
  AQ_CALL_RET_V(value(arg0), QVariant);
}
inline QVariant AQSSqlCursor::value(const QString &arg0) const
{
  AQ_CALL_RET_V(value(arg0), QVariant);
}
inline void AQSSqlCursor::setValue(int arg0,  const QVariant &arg1)
{
  AQ_CALL_VOID(setValue(arg0, arg1));
}
inline void AQSSqlCursor::setValue(const QString &arg0,  const QVariant &arg1)
{
  AQ_CALL_VOID(setValue(arg0, arg1));
}
inline QString AQSSqlCursor::baseFilter()
{
  AQ_CALL_RET_V(baseFilter(), QString);
}
inline QString AQSSqlCursor::curFilter()
{
  AQ_CALL_RET_V(curFilter(), QString);
}
inline QString AQSSqlCursor::filter() const
{
  AQ_CALL_RET_V(filter(), QString);
}
inline QString AQSSqlCursor::connectionName() const
{
  AQ_CALL_RET_V(connectionName(), QString);
}
inline QString AQSSqlCursor::curName() const
{
  AQ_CALL_RET_V(curName(), QString);
}
inline void AQSSqlCursor::changePos(const QString &arg0,  int arg1)
{
  AQ_CALL_VOID(changePos(arg0, arg1));
}
inline void AQSSqlCursor::setOrder(const QStringList &arg0)
{
  AQ_CALL_VOID(setOrder(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSQLCURSOR_P_H_ */
// @AQOBJECT@
