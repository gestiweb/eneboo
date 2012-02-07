/***************************************************************************
 AQSReportEngine_p.h
 -------------------
 begin                : 02/11/2011
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

#ifndef AQSREPORTENGINE_P_H_
#define AQSREPORTENGINE_P_H_

#include "AQSObject_p.h"
#include "AQObjects.h"
#include "AQSDomNode_p.h"

class AQSReportEngine : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_AQOBJECT(ReportEngine, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool setReportData(FLSqlQuery *);
  bool setFLReportData(QDomNode *);
  bool setFLReportData(AQSDomNode *);
  bool setReportData(FLSqlCursor *);
  bool setFLReportTemplate(const QString &);
  bool setFLReportTemplate(QDomNode *);
  bool setFLReportTemplate(AQSDomNode *);
  FLSqlQuery *rptQueryData() const;
  QString rptNameTemplate() const;
  QDomDocument *rptXmlData() const;
  QDomDocument *rptXmlTemplate() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQReportEngine;
    if (sgt == QString::fromLatin1("QObject*"))
      return new AQReportEngine(argValue<QObject *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSReportEngine::setReportData(FLSqlQuery *arg0)
{
  AQ_CALL_RET_V(setReportData(arg0), bool);
}
inline bool AQSReportEngine::setFLReportData(QDomNode *arg0)
{
  AQ_CALL_RET_V(setFLReportData(*arg0), bool);
}
inline bool AQSReportEngine::setFLReportData(AQSDomNode *arg0)
{
  AQ_CALL_RET_V(setFLReportData(*arg0), bool);
}
inline bool AQSReportEngine::setReportData(FLSqlCursor *arg0)
{
  AQ_CALL_RET_V(setReportData(arg0), bool);
}
inline bool AQSReportEngine::setFLReportTemplate(const QString &arg0)
{
  AQ_CALL_RET_V(setFLReportTemplate(arg0), bool);
}
inline bool AQSReportEngine::setFLReportTemplate(QDomNode *arg0)
{
  AQ_CALL_RET_V(setFLReportTemplate(*arg0), bool);
}
inline bool AQSReportEngine::setFLReportTemplate(AQSDomNode *arg0)
{
  AQ_CALL_RET_V(setFLReportTemplate(*arg0), bool);
}
inline FLSqlQuery *AQSReportEngine::rptQueryData() const
{
  AQ_CALL_RET(rptQueryData());
}
inline QString AQSReportEngine::rptNameTemplate() const
{
  AQ_CALL_RET_V(rptNameTemplate(), QString);
}
inline QDomDocument *AQSReportEngine::rptXmlData() const
{
  AQ_CALL_RET(rptXmlData());
}
inline QDomDocument *AQSReportEngine::rptXmlTemplate() const
{
  AQ_CALL_RET(rptXmlTemplate());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSREPORTENGINE_P_H_ */
// @AQOBJECT@
