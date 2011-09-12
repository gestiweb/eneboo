/***************************************************************************
 AQSFieldDB_p.h
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

#ifndef AQSFIELDDB_P_H_
#define AQSFIELDDB_P_H_

#include "AQSWidget_p.h"
#include "AQObjects.h"

class AQSFieldDB : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_AQOBJECT(FieldDB, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  FLSqlCursor *cursor();
  void setDisabled(const bool);
  QString actionName() const;
  void setActionName(const QString &);
  QString fieldName() const;
  void setFieldName(const QString &);
  QString tableName() const;
  void setTableName(const QString &);
  QString foreignField() const;
  void setForeignField(const QString &);
  QString fieldRelation() const;
  QString fieldAlias() const;
  QWidget *editor() const;
  void setFilter(const QString &);
  QString filter() const;
  void setFieldRelation(const QString &);
  void setFieldAlias(const QString &);
  void setValue(QVariant);
  void setPixmap(const QString &);
  void setFocus();
  void selectAll();
  QVariant value();
  void close();
  int insertAccel(const QString &);
  void removeAccel(int);
  void setTextFormat(const int &);
  int textFormat() const;
  bool showAlias() const;
  void setShowAlias(const bool);
  bool showEditor() const;
  void setShowEditor(const bool);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQFieldDB;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new AQFieldDB(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new AQFieldDB(argValue<QWidget *>(args[0]),
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
inline FLSqlCursor *AQSFieldDB::cursor()
{
  AQ_CALL_RET(cursor());
}
inline void AQSFieldDB::setDisabled(const bool arg0)
{
  AQ_CALL_VOID(setDisabled(arg0));
}
inline QString AQSFieldDB::actionName() const
{
  AQ_CALL_RET_V(actionName(), QString);
}
inline void AQSFieldDB::setActionName(const QString &arg0)
{
  AQ_CALL_VOID(setActionName(arg0));
}
inline QString AQSFieldDB::fieldName() const
{
  AQ_CALL_RET_V(fieldName(), QString);
}
inline void AQSFieldDB::setFieldName(const QString &arg0)
{
  AQ_CALL_VOID(setFieldName(arg0));
}
inline QString AQSFieldDB::tableName() const
{
  AQ_CALL_RET_V(tableName(), QString);
}
inline void AQSFieldDB::setTableName(const QString &arg0)
{
  AQ_CALL_VOID(setTableName(arg0));
}
inline QString AQSFieldDB::foreignField() const
{
  AQ_CALL_RET_V(foreignField(), QString);
}
inline void AQSFieldDB::setForeignField(const QString &arg0)
{
  AQ_CALL_VOID(setForeignField(arg0));
}
inline QString AQSFieldDB::fieldRelation() const
{
  AQ_CALL_RET_V(fieldRelation(), QString);
}
inline QString AQSFieldDB::fieldAlias() const
{
  AQ_CALL_RET_V(fieldAlias(), QString);
}
inline QWidget *AQSFieldDB::editor() const
{
  AQ_CALL_RET(editor());
}
inline void AQSFieldDB::setFilter(const QString &arg0)
{
  AQ_CALL_VOID(setFilter(arg0));
}
inline QString AQSFieldDB::filter() const
{
  AQ_CALL_RET_V(filter(), QString);
}
inline void AQSFieldDB::setFieldRelation(const QString &arg0)
{
  AQ_CALL_VOID(setFieldRelation(arg0));
}
inline void AQSFieldDB::setFieldAlias(const QString &arg0)
{
  AQ_CALL_VOID(setFieldAlias(arg0));
}
inline void AQSFieldDB::setValue(QVariant arg0)
{
  AQ_CALL_VOID(setValue(arg0));
}
inline void AQSFieldDB::setPixmap(const QString &arg0)
{
  AQ_CALL_VOID(setPixmap(arg0));
}
inline void AQSFieldDB::setFocus()
{
  AQ_CALL_VOID(setFocus());
}
inline void AQSFieldDB::selectAll()
{
  AQ_CALL_VOID(selectAll());
}
inline QVariant AQSFieldDB::value()
{
  AQ_CALL_RET_V(value(), QVariant);
}
inline void AQSFieldDB::close()
{
  AQ_CALL_VOID(close());
}
inline int AQSFieldDB::insertAccel(const QString &arg0)
{
  AQ_CALL_RET_V(insertAccel(arg0), int);
}
inline void AQSFieldDB::removeAccel(int arg0)
{
  AQ_CALL_VOID(removeAccel(arg0));
}
inline void AQSFieldDB::setTextFormat(const int &arg0)
{
  AQ_CALL_VOID(setTextFormat(arg0));
}
inline int AQSFieldDB::textFormat() const
{
  AQ_CALL_RET_V(textFormat(), int);
}
inline bool AQSFieldDB::showAlias() const
{
  AQ_CALL_RET_V(showAlias(), bool);
}
inline void AQSFieldDB::setShowAlias(const bool arg0)
{
  AQ_CALL_VOID(setShowAlias(arg0));
}
inline bool AQSFieldDB::showEditor() const
{
  AQ_CALL_RET_V(showEditor(), bool);
}
inline void AQSFieldDB::setShowEditor(const bool arg0)
{
  AQ_CALL_VOID(setShowEditor(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSFIELDDB_P_H_ */
// @AQWIDGET@
