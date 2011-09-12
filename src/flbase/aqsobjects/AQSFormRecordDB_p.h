/***************************************************************************
 AQSFormRecordDB_p.h
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

#ifndef AQSFORMRECORDDB_P_H_
#define AQSFORMRECORDDB_P_H_

#include "AQSFormDB_p.h"

class AQSFormRecordDB : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_AQOBJECT(FormRecordDB, Widget);

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

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("AQSqlCursor*"));
    candidates[2].append(QString::fromLatin1("AQSqlCursor*,QString"));
    candidates[3].append(QString::fromLatin1("AQSqlCursor*,QString,QWidget*"));
    candidates[4].append(QString::fromLatin1("AQSqlCursor*,QString,QWidget*,bool"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("AQSqlCursor*"))
      return new AQFormRecordDB(argValue<AQSqlCursor *>(args[0]));
    if (sgt == QString::fromLatin1("AQSqlCursor*,QString"))
      return new AQFormRecordDB(argValue<AQSqlCursor *>(args[0]),
                                *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("AQSqlCursor*,QString,QWidget*"))
      return new AQFormRecordDB(argValue<AQSqlCursor *>(args[0]),
                                *(argValue<QString *>(args[1])),
                                argValue<QWidget *>(args[2]));
    if (sgt == QString::fromLatin1("AQSqlCursor*,QString,QWidget*,bool"))
      return new AQFormRecordDB(argValue<AQSqlCursor *>(args[0]),
                                *(argValue<QString *>(args[1])),
                                argValue<QWidget *>(args[2]),
                                args[3].variant().toBool());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("AQSqlCursor*"));
    candidates[2].append(QString::fromLatin1("AQSqlCursor*,QString"));
    candidates[3].append(QString::fromLatin1("AQSqlCursor*,QString,QWidget*"));
    candidates[4].append(QString::fromLatin1("AQSqlCursor*,QString,QWidget*,bool"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline FLSqlCursor *AQSFormRecordDB::cursor() const
{
  AQ_CALL_RET(cursor());
}
inline QWidget *AQSFormRecordDB::mainWidget() const
{
  AQ_CALL_RET(mainWidget());
}
inline void AQSFormRecordDB::setIdMDI(const QString &arg0)
{
  AQ_CALL_VOID(setIdMDI(arg0));
}
inline QString AQSFormRecordDB::idMDI() const
{
  AQ_CALL_RET_V(idMDI(), QString);
}
inline void AQSFormRecordDB::setMainWidget(QWidget *arg0)
{
  AQ_CALL_VOID(setMainWidget(arg0));
}
inline void AQSFormRecordDB::setMainWidget()
{
  AQ_CALL_VOID(setMainWidget());
}
inline QImage *AQSFormRecordDB::snapShot()
{
  AQ_CALL_RET_PTR(snapShot(), QImage);
}
inline void AQSFormRecordDB::saveSnapShot(const QString &arg0)
{
  AQ_CALL_VOID(saveSnapShot(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSFORMRECORDDB_P_H_ */
// @AQWIDGET@
