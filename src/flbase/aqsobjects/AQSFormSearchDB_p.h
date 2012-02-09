/***************************************************************************
 AQSFormSearchDB_p.h
 -------------------
 begin                : 19/04/2011
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

#ifndef AQSFORMSEARCHDB_P_H_
#define AQSFORMSEARCHDB_P_H_

#include "AQSFormDB_p.h"

class AQSFormSearchDB : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_AQOBJECT(FormSearchDB, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  FLSqlCursor *cursor() const;
  QWidget *mainWidget() const;
  void setIdMDI(const QString &);
  QString idMDI() const;
  virtual void setMainWidget(QWidget *);
  virtual void setMainWidget();
  QImage *snapShot();
  void saveSnapShot(const QString &);
  QVariant exec(const QString& = QString::null);
  void setFilter(const QString &);
  bool accepted();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[1].append(QString::fromLatin1("FLSqlCursor*"));
    candidates[2].append(QString::fromLatin1("FLSqlCursor*,QString"));
    candidates[3].append(QString::fromLatin1("FLSqlCursor*,QString,QWidget*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QString"))
      return new AQFormSearchDB(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,QWidget*"))
      return new AQFormSearchDB(*(argValue<QString *>(args[0])),
                                argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("FLSqlCursor*"))
      return new AQFormSearchDB(argValue<FLSqlCursor *>(args[0]));
    if (sgt == QString::fromLatin1("FLSqlCursor*,QString"))
      return new AQFormSearchDB(argValue<FLSqlCursor *>(args[0]),
                                *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("FLSqlCursor*,QString,QWidget*"))
      return new AQFormSearchDB(argValue<FLSqlCursor *>(args[0]),
                                *(argValue<QString *>(args[1])),
                                argValue<QWidget *>(args[2]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[1].append(QString::fromLatin1("FLSqlCursor*"));
    candidates[2].append(QString::fromLatin1("FLSqlCursor*,QString"));
    candidates[3].append(QString::fromLatin1("FLSqlCursor*,QString,QWidget*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline FLSqlCursor *AQSFormSearchDB::cursor() const
{
  AQ_CALL_RET(cursor());
}
inline QWidget *AQSFormSearchDB::mainWidget() const
{
  AQ_CALL_RET(mainWidget());
}
inline void AQSFormSearchDB::setIdMDI(const QString &arg0)
{
  AQ_CALL_VOID(setIdMDI(arg0));
}
inline QString AQSFormSearchDB::idMDI() const
{
  AQ_CALL_RET_V(idMDI(), QString);
}
inline void AQSFormSearchDB::setMainWidget(QWidget *arg0)
{
  AQ_CALL_VOID(setMainWidget(arg0));
}
inline void AQSFormSearchDB::setMainWidget()
{
  AQ_CALL_VOID(setMainWidget());
}
inline QImage *AQSFormSearchDB::snapShot()
{
  AQ_CALL_RET_PTR(snapShot(), QImage);
}
inline void AQSFormSearchDB::saveSnapShot(const QString &arg0)
{
  AQ_CALL_VOID(saveSnapShot(arg0));
}
inline QVariant AQSFormSearchDB::exec(const QString &arg0)
{
  AQ_CALL_RET_V(exec(arg0), QVariant);
}
inline void AQSFormSearchDB::setFilter(const QString &arg0)
{
  AQ_CALL_VOID(setFilter(arg0));
}
inline bool AQSFormSearchDB::accepted()
{
  AQ_CALL_RET_V(accepted(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSFORMSEARCHDB_P_H_ */
// @AQWIDGET@
