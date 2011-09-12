/***************************************************************************
 AQSFormDB_p.h
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

#ifndef AQSFORMDB_P_H_
#define AQSFORMDB_P_H_

#include "AQSWidget_p.h"
#include "AQSImage_p.h"
#include "AQObjects.h"

class AQSFormDB : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_AQOBJECT(FormDB, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual void setCursor(FLSqlCursor *);
  FLSqlCursor *cursor() const;
  QWidget *mainWidget() const;
  void setIdMDI(const QString &);
  QString idMDI() const;
  virtual void setMainWidget(QWidget *);
  virtual void setMainWidget();
  virtual void setMainWidget(const QString &);
  QImage *snapShot();
  void saveSnapShot(const QString &);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,WFlags"));
    candidates[1].append(QString::fromLatin1("AQSqlCursor*"));
    candidates[2].append(QString::fromLatin1("AQSqlCursor*,QString"));
    candidates[3].append(QString::fromLatin1("AQSqlCursor*,QString,QWidget*"));
    candidates[4].append(QString::fromLatin1("AQSqlCursor*,QString,QWidget*,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQFormDB;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new AQFormDB(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new AQFormDB(argValue<QWidget *>(args[0]),
                          *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new AQFormDB(argValue<QWidget *>(args[0]),
                          *(argValue<QString *>(args[1])),
                          static_cast<WFlags>(args[2].variant().toUInt()));
    if (sgt == QString::fromLatin1("QString"))
      return new AQFormDB(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,QWidget*"))
      return new AQFormDB(*(argValue<QString *>(args[0])),
                          argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("QString,QWidget*,WFlags"))
      return new AQFormDB(*(argValue<QString *>(args[0])),
                          argValue<QWidget *>(args[1]),
                          static_cast<WFlags>(args[2].variant().toUInt()));
    if (sgt == QString::fromLatin1("AQSqlCursor*"))
      return new AQFormDB(argValue<AQSqlCursor *>(args[0]));
    if (sgt == QString::fromLatin1("AQSqlCursor*,QString"))
      return new AQFormDB(argValue<AQSqlCursor *>(args[0]),
                          *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("AQSqlCursor*,QString,QWidget*"))
      return new AQFormDB(argValue<AQSqlCursor *>(args[0]),
                          *(argValue<QString *>(args[1])),
                          argValue<QWidget *>(args[2]));
    if (sgt == QString::fromLatin1("AQSqlCursor*,QString,QWidget*,WFlags"))
      return new AQFormDB(argValue<AQSqlCursor *>(args[0]),
                          *(argValue<QString *>(args[1])),
                          argValue<QWidget *>(args[2]),
                          static_cast<WFlags>(args[3].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,WFlags"));
    candidates[1].append(QString::fromLatin1("AQSqlCursor*"));
    candidates[2].append(QString::fromLatin1("AQSqlCursor*,QString"));
    candidates[3].append(QString::fromLatin1("AQSqlCursor*,QString,QWidget*"));
    candidates[4].append(QString::fromLatin1("AQSqlCursor*,QString,QWidget*,WFlags"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSFormDB::setCursor(FLSqlCursor *arg0)
{
  AQ_CALL_VOID(setCursor(arg0));
}
inline FLSqlCursor *AQSFormDB::cursor() const
{
  AQ_CALL_RET(cursor());
}
inline QWidget *AQSFormDB::mainWidget() const
{
  AQ_CALL_RET(mainWidget());
}
inline void AQSFormDB::setIdMDI(const QString &arg0)
{
  AQ_CALL_VOID(setIdMDI(arg0));
}
inline QString AQSFormDB::idMDI() const
{
  AQ_CALL_RET_V(idMDI(), QString);
}
inline void AQSFormDB::setMainWidget(QWidget *arg0)
{
  AQ_CALL_VOID(setMainWidget(arg0));
}
inline void AQSFormDB::setMainWidget()
{
  AQ_CALL_VOID(setMainWidget());
}
inline void AQSFormDB::setMainWidget(const QString &arg0)
{
  AQ_CALL_VOID(setMainWidget(arg0));
}
inline QImage *AQSFormDB::snapShot()
{
  AQ_CALL_RET_PTR(snapShot(), QImage);
}
inline void AQSFormDB::saveSnapShot(const QString &arg0)
{
  AQ_CALL_VOID(saveSnapShot(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSFORMDB_P_H_ */
// @AQWIDGET@
