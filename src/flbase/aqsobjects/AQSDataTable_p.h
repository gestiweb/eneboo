/***************************************************************************
 AQSDataTable_p.h
 -------------------
 begin                : 18/04/2011
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

#ifndef AQSDATATABLE_P_H_
#define AQSDATATABLE_P_H_

#include "AQSTable_p.h"
#include "AQSIconSet_p.h"
#include <qdatatable.h>

class AQSDataTable : public AQSTable
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(DataTable, Table);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual void addColumn(const QString &, const QString &, int, QIconSet *);
  virtual void addColumn(const QString &, const QString &, int, AQSIconSet *);
  virtual void removeColumn(uint);
  virtual void setColumn(uint, const QString &, const QString &, int, QIconSet *);
  virtual void setColumn(uint, const QString &, const QString &, int, AQSIconSet *);
  bool autoDelete() const;
  virtual void setSqlCursor(QSqlCursor* = 0, bool = FALSE, bool = FALSE);
  QSqlCursor *sqlCursor() const;
  virtual void setNullText(const QString &);
  virtual void setTrueText(const QString &);
  virtual void setFalseText(const QString &);
  virtual void setDateFormat(const DateFormat);
  virtual void setConfirmEdits(bool);
  virtual void setConfirmInsert(bool);
  virtual void setConfirmUpdate(bool);
  virtual void setConfirmDelete(bool);
  virtual void setConfirmCancels(bool);
  virtual void setAutoDelete(bool);
  virtual void setAutoEdit(bool);
  virtual void setFilter(const QString &);
  virtual void setSort(const QStringList &);
  virtual void setSort(const QSqlIndex &);
  void refresh(uint);
  virtual void sortColumn(int, bool = TRUE, bool = FALSE);
  virtual QString text(int, int) const;
  QVariant value(int, int) const;
  QSqlRecord *currentRecord() const;
  virtual int numCols() const;
  virtual int numRows() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[1].append(QString::fromLatin1("QSqlCursor*"));
    candidates[2].append(QString::fromLatin1("QSqlCursor*,bool"));
    candidates[3].append(QString::fromLatin1("QSqlCursor*,bool,QWidget*"));
    candidates[4].append(QString::fromLatin1("QSqlCursor*,bool,QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDataTable;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QDataTable(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QDataTable(argValue<QWidget *>(args[0]),
                            *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QSqlCursor*"))
      return new QDataTable(argValue<QSqlCursor *>(args[0]));
    if (sgt == QString::fromLatin1("QSqlCursor*,bool"))
      return new QDataTable(argValue<QSqlCursor *>(args[0]),
                            args[1].variant().toBool());
    if (sgt == QString::fromLatin1("QSqlCursor*,bool,QWidget*"))
      return new QDataTable(argValue<QSqlCursor *>(args[0]),
                            args[1].variant().toBool(),
                            argValue<QWidget *>(args[2]));
    if (sgt == QString::fromLatin1("QSqlCursor*,bool,QWidget*,QString"))
      return new QDataTable(argValue<QSqlCursor *>(args[0]),
                            args[1].variant().toBool(),
                            argValue<QWidget *>(args[2]),
                            *(argValue<QString *>(args[3])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[1].append(QString::fromLatin1("QSqlCursor*"));
    candidates[2].append(QString::fromLatin1("QSqlCursor*,bool"));
    candidates[3].append(QString::fromLatin1("QSqlCursor*,bool,QWidget*"));
    candidates[4].append(QString::fromLatin1("QSqlCursor*,bool,QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSDataTable::addColumn(const QString &arg0,  const QString &arg1,  int arg2,  QIconSet *arg3)
{
  AQ_CALL_VOID(addColumn(arg0, arg1, arg2, *arg3));
}
inline void AQSDataTable::addColumn(const QString &arg0,  const QString &arg1,  int arg2, AQSIconSet *arg3)
{
  AQ_CALL_VOID(addColumn(arg0, arg1, arg2, *arg3));
}
inline void AQSDataTable::removeColumn(uint arg0)
{
  AQ_CALL_VOID(removeColumn(arg0));
}
inline void AQSDataTable::setColumn(uint arg0,  const QString &arg1,  const QString &arg2,  int arg3,  QIconSet *arg4)
{
  AQ_CALL_VOID(setColumn(arg0, arg1, arg2, arg3, *arg4));
}
inline void AQSDataTable::setColumn(uint arg0,  const QString &arg1,  const QString &arg2,  int arg3, AQSIconSet *arg4)
{
  AQ_CALL_VOID(setColumn(arg0, arg1, arg2, arg3, *arg4));
}
inline bool AQSDataTable::autoDelete() const
{
  AQ_CALL_RET_V(autoDelete(), bool);
}
inline void AQSDataTable::setSqlCursor(QSqlCursor *arg0,  bool arg1,  bool arg2)
{
  AQ_CALL_VOID(setSqlCursor(arg0, arg1, arg2));
}
inline QSqlCursor *AQSDataTable::sqlCursor() const
{
  AQ_CALL_RET(sqlCursor());
}
inline void AQSDataTable::setNullText(const QString &arg0)
{
  AQ_CALL_VOID(setNullText(arg0));
}
inline void AQSDataTable::setTrueText(const QString &arg0)
{
  AQ_CALL_VOID(setTrueText(arg0));
}
inline void AQSDataTable::setFalseText(const QString &arg0)
{
  AQ_CALL_VOID(setFalseText(arg0));
}
inline void AQSDataTable::setDateFormat(const DateFormat arg0)
{
  AQ_CALL_VOID(setDateFormat(arg0));
}
inline void AQSDataTable::setConfirmEdits(bool arg0)
{
  AQ_CALL_VOID(setConfirmEdits(arg0));
}
inline void AQSDataTable::setConfirmInsert(bool arg0)
{
  AQ_CALL_VOID(setConfirmInsert(arg0));
}
inline void AQSDataTable::setConfirmUpdate(bool arg0)
{
  AQ_CALL_VOID(setConfirmUpdate(arg0));
}
inline void AQSDataTable::setConfirmDelete(bool arg0)
{
  AQ_CALL_VOID(setConfirmDelete(arg0));
}
inline void AQSDataTable::setConfirmCancels(bool arg0)
{
  AQ_CALL_VOID(setConfirmCancels(arg0));
}
inline void AQSDataTable::setAutoDelete(bool arg0)
{
  AQ_CALL_VOID(setAutoDelete(arg0));
}
inline void AQSDataTable::setAutoEdit(bool arg0)
{
  AQ_CALL_VOID(setAutoEdit(arg0));
}
inline void AQSDataTable::setFilter(const QString &arg0)
{
  AQ_CALL_VOID(setFilter(arg0));
}
inline void AQSDataTable::setSort(const QStringList &arg0)
{
  AQ_CALL_VOID(setSort(arg0));
}
inline void AQSDataTable::setSort(const QSqlIndex &arg0)
{
  AQ_CALL_VOID(setSort(arg0));
}
inline void AQSDataTable::refresh(uint arg0)
{
  AQ_CALL_VOID(refresh(static_cast<QDataTable::Refresh>(arg0)));
}
inline void AQSDataTable::sortColumn(int arg0,  bool arg1,  bool arg2)
{
  AQ_CALL_VOID(sortColumn(arg0, arg1, arg2));
}
inline QString AQSDataTable::text(int arg0,  int arg1) const
{
  AQ_CALL_RET_V(text(arg0, arg1), QString);
}
inline QVariant AQSDataTable::value(int arg0,  int arg1) const
{
  AQ_CALL_RET_V(value(arg0, arg1), QVariant);
}
inline QSqlRecord *AQSDataTable::currentRecord() const
{
  AQ_CALL_RET(currentRecord());
}
inline int AQSDataTable::numCols() const
{
  AQ_CALL_RET_V(numCols(), int);
}
inline int AQSDataTable::numRows() const
{
  AQ_CALL_RET_V(numRows(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDATATABLE_P_H_ */
// @AQSWIDGET@
