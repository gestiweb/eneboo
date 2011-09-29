/***************************************************************************
 AQSDataTableDB_p.h
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

#ifndef AQSDATATABLEDB_P_H_
#define AQSDATATABLEDB_P_H_

#include "AQSDataTable_p.h"
#include "AQObjects.h"

class AQSDataTableDB : public AQSDataTable
{
  Q_OBJECT

  AQ_DECLARE_AQS_AQOBJECT(DataTableDB, DataTable);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setPersistentFilter(const QString &);
  FLSqlCursor *cursor() const;
  void setFLReadOnly(const bool);
  void setEditOnly(const bool);
  void setInsertOnly(const bool);
  QVariantList primarysKeysChecked() const;
  void clearChecked();
  void setPrimaryKeyChecked(const QVariant &, bool);
  void setShowAllPixmaps(bool);
  void setFunctionGetColor(const QString &);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,bool"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQDataTableDB;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new AQDataTableDB(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new AQDataTableDB(argValue<QWidget *>(args[0]),
                               *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,bool"))
      return new AQDataTableDB(argValue<QWidget *>(args[0]),
                               *(argValue<QString *>(args[1])),
                               args[2].variant().toBool());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,bool"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSDataTableDB::setPersistentFilter(const QString &arg0)
{
  AQ_CALL_VOID(setPersistentFilter(arg0));
}
inline FLSqlCursor *AQSDataTableDB::cursor() const
{
  AQ_CALL_RET(cursor());
}
inline void AQSDataTableDB::setFLReadOnly(const bool arg0)
{
  AQ_CALL_VOID(setFLReadOnly(arg0));
}
inline void AQSDataTableDB::setEditOnly(const bool arg0)
{
  AQ_CALL_VOID(setEditOnly(arg0));
}
inline void AQSDataTableDB::setInsertOnly(const bool arg0)
{
  AQ_CALL_VOID(setInsertOnly(arg0));
}
inline QVariantList AQSDataTableDB::primarysKeysChecked() const
{
  AQ_CALL_RET_V(primarysKeysChecked(), QVariantList);
}
inline void AQSDataTableDB::clearChecked()
{
  AQ_CALL_VOID(clearChecked());
}
inline void AQSDataTableDB::setPrimaryKeyChecked(const QVariant &arg0,  bool arg1)
{
  AQ_CALL_VOID(setPrimaryKeyChecked(arg0, arg1));
}
inline void AQSDataTableDB::setShowAllPixmaps(bool arg0)
{
  AQ_CALL_VOID(setShowAllPixmaps(arg0));
}
inline void AQSDataTableDB::setFunctionGetColor(const QString &arg0)
{
  AQ_CALL_VOID(setFunctionGetColor(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDATATABLEDB_P_H_ */
// @AQWIDGET@
