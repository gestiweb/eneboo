/***************************************************************************
 AQSTableDB_p.h
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

#ifndef AQSTABLEDB_P_H_
#define AQSTABLEDB_P_H_

#include "AQSWidget_p.h"
#include "AQObjects.h"

class AQSTableDB : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_AQOBJECT(TableDB, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QString tableName() const;
  void setTableName(const QString &);
  QString foreignField() const;
  void setForeignField(const QString &);
  QString fieldRelation() const;
  void setFieldRelation(const QString &);
  void setReadOnly(const bool);
  void setEditOnly(const bool);
  void setInsertOnly(const bool);
  void refresh();
  void refresh(const bool, const bool);
  void insertRecord();
  void editRecord();
  void browseRecord();
  void deleteRecord();
  void copyRecord();
  void show();
  void setFocus();
  void setFilter(const QString &);
  QString filter();
  QString findFilter() const;
  void setInitSearch(const QString &);
  FLSqlCursor *cursor();
  FLDataTable *tableRecords();
  void close();
  void setDisabled(const bool);
  void putFirstCol(const QString &);
  void moveCol(const QString &, const QString &);
  void setOrderCols(QStringList &);
  QStringList orderCols();
  bool checkColumnEnabled() const;
  void setCheckColumnEnabled(bool);
  QString aliasCheckColumn() const;
  void setAliasCheckColumn(const QString &);
  bool findHidden() const;
  void setFindHidden(bool);
  bool filterHidden() const;
  void setFilterHidden(bool);
  void setColumnWidth(const QString &, int);
  int columnWidth(int);
  void setRowHeight(int, int);
  int rowHeight(int);
  bool showAllPixmaps() const;
  void setShowAllPixmaps(bool);
  QString functionGetColor() const;
  void setFunctionGetColor(const QString &);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQTableDB;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new AQTableDB(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new AQTableDB(argValue<QWidget *>(args[0]),
                           *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSTableDB::tableName() const
{
  AQ_CALL_RET_V(tableName(), QString);
}
inline void AQSTableDB::setTableName(const QString &arg0)
{
  AQ_CALL_VOID(setTableName(arg0));
}
inline QString AQSTableDB::foreignField() const
{
  AQ_CALL_RET_V(foreignField(), QString);
}
inline void AQSTableDB::setForeignField(const QString &arg0)
{
  AQ_CALL_VOID(setForeignField(arg0));
}
inline QString AQSTableDB::fieldRelation() const
{
  AQ_CALL_RET_V(fieldRelation(), QString);
}
inline void AQSTableDB::setFieldRelation(const QString &arg0)
{
  AQ_CALL_VOID(setFieldRelation(arg0));
}
inline void AQSTableDB::setReadOnly(const bool arg0)
{
  AQ_CALL_VOID(setReadOnly(arg0));
}
inline void AQSTableDB::setEditOnly(const bool arg0)
{
  AQ_CALL_VOID(setEditOnly(arg0));
}
inline void AQSTableDB::setInsertOnly(const bool arg0)
{
  AQ_CALL_VOID(setInsertOnly(arg0));
}
inline void AQSTableDB::refresh()
{
  AQ_CALL_VOID(refresh());
}
inline void AQSTableDB::refresh(const bool arg0,  const bool arg1)
{
  AQ_CALL_VOID(refresh(arg0, arg1));
}
inline void AQSTableDB::insertRecord()
{
  AQ_CALL_VOID(insertRecord());
}
inline void AQSTableDB::editRecord()
{
  AQ_CALL_VOID(editRecord());
}
inline void AQSTableDB::browseRecord()
{
  AQ_CALL_VOID(browseRecord());
}
inline void AQSTableDB::deleteRecord()
{
  AQ_CALL_VOID(deleteRecord());
}
inline void AQSTableDB::copyRecord()
{
  AQ_CALL_VOID(copyRecord());
}
inline void AQSTableDB::show()
{
  AQ_CALL_VOID(show());
}
inline void AQSTableDB::setFocus()
{
  AQ_CALL_VOID(setFocus());
}
inline void AQSTableDB::setFilter(const QString &arg0)
{
  AQ_CALL_VOID(setFilter(arg0));
}
inline QString AQSTableDB::filter()
{
  AQ_CALL_RET_V(filter(), QString);
}
inline QString AQSTableDB::findFilter() const
{
  AQ_CALL_RET_V(findFilter(), QString);
}
inline void AQSTableDB::setInitSearch(const QString &arg0)
{
  AQ_CALL_VOID(setInitSearch(arg0));
}
inline FLSqlCursor *AQSTableDB::cursor()
{
  AQ_CALL_RET(cursor());
}
inline FLDataTable *AQSTableDB::tableRecords()
{
  AQ_CALL_RET(tableRecords());
}
inline void AQSTableDB::close()
{
  AQ_CALL_VOID(close());
}
inline void AQSTableDB::setDisabled(const bool arg0)
{
  AQ_CALL_VOID(setDisabled(arg0));
}
inline void AQSTableDB::putFirstCol(const QString &arg0)
{
  AQ_CALL_VOID(putFirstCol(arg0));
}
inline void AQSTableDB::moveCol(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_VOID(moveCol(arg0, arg1));
}
inline void AQSTableDB::setOrderCols(QStringList &arg0)
{
  AQ_CALL_VOID(setOrderCols(arg0));
}
inline QStringList AQSTableDB::orderCols()
{
  AQ_CALL_RET_V(orderCols(), QStringList);
}
inline bool AQSTableDB::checkColumnEnabled() const
{
  AQ_CALL_RET_V(checkColumnEnabled(), bool);
}
inline void AQSTableDB::setCheckColumnEnabled(bool arg0)
{
  AQ_CALL_VOID(setCheckColumnEnabled(arg0));
}
inline QString AQSTableDB::aliasCheckColumn() const
{
  AQ_CALL_RET_V(aliasCheckColumn(), QString);
}
inline void AQSTableDB::setAliasCheckColumn(const QString &arg0)
{
  AQ_CALL_VOID(setAliasCheckColumn(arg0));
}
inline bool AQSTableDB::findHidden() const
{
  AQ_CALL_RET_V(findHidden(), bool);
}
inline void AQSTableDB::setFindHidden(bool arg0)
{
  AQ_CALL_VOID(setFindHidden(arg0));
}
inline bool AQSTableDB::filterHidden() const
{
  AQ_CALL_RET_V(filterHidden(), bool);
}
inline void AQSTableDB::setFilterHidden(bool arg0)
{
  AQ_CALL_VOID(setFilterHidden(arg0));
}
inline void AQSTableDB::setColumnWidth(const QString &arg0,  int arg1)
{
  AQ_CALL_VOID(setColumnWidth(arg0, arg1));
}
inline int AQSTableDB::columnWidth(int arg0)
{
  AQ_CALL_RET_V(columnWidth(arg0), int);
}
inline void AQSTableDB::setRowHeight(int arg0,  int arg1)
{
  AQ_CALL_VOID(setRowHeight(arg0, arg1));
}
inline int AQSTableDB::rowHeight(int arg0)
{
  AQ_CALL_RET_V(rowHeight(arg0), int);
}
inline bool AQSTableDB::showAllPixmaps() const
{
  AQ_CALL_RET_V(showAllPixmaps(), bool);
}
inline void AQSTableDB::setShowAllPixmaps(bool arg0)
{
  AQ_CALL_VOID(setShowAllPixmaps(arg0));
}
inline QString AQSTableDB::functionGetColor() const
{
  AQ_CALL_RET_V(functionGetColor(), QString);
}
inline void AQSTableDB::setFunctionGetColor(const QString &arg0)
{
  AQ_CALL_VOID(setFunctionGetColor(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTABLEDB_P_H_ */
// @AQWIDGET@
