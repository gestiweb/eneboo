/***************************************************************************
 AQSJasperEngine_p.h
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

#ifndef AQSJASPERENGINE_P_H_
#define AQSJASPERENGINE_P_H_

#include "AQSObject_p.h"
#include "AQObjects.h"
#include "AQSDomNode_p.h"

class AQSJasperEngine : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_AQOBJECT(JasperEngine, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool setReportData(QDomNode *);
  bool setReportData(AQSDomNode *);
  bool setReportTemplate(const QString &);
  QDomDocument rptXmlData() const;
  QDomDocument rptXmlTemplate() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QObject*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new AQJasperEngine;
    if (sgt == QString::fromLatin1("QObject*"))
      return new AQJasperEngine(argValue<QObject *>(args[0]));
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
inline bool AQSJasperEngine::setReportData(QDomNode *arg0)
{
  AQ_CALL_RET_V(setReportData(*arg0), bool);
}
inline bool AQSJasperEngine::setReportData(AQSDomNode *arg0)
{
  AQ_CALL_RET_V(setReportData(*arg0), bool);
}
inline bool AQSJasperEngine::setReportTemplate(const QString &arg0)
{
  AQ_CALL_RET_V(setReportTemplate(arg0), bool);
}
inline QDomDocument AQSJasperEngine::rptXmlData() const
{
  AQ_CALL_RET_V(rptXmlData(), QDomDocument);
}
inline QDomDocument AQSJasperEngine::rptXmlTemplate() const
{
  AQ_CALL_RET_V(rptXmlTemplate(), QDomDocument);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSJASPERENGINE_P_H_ */
// @AQOBJECT@
