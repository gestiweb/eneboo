/***************************************************************************
 FLJasperViewer.cpp
 -------------------
 begin                : 08/06/2010
 copyright            : (C) 2003-2010 by InfoSiAL S.L.
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

#include <qlibrary.h>

#include "FLObjectFactory.h"
#include "FLJasperViewer.h"
#include "FLJasperEngine.h"
#include "FLJasperEngine_p.h"

class FLJasperViewerPrivate
{
public:

  FLJasperViewerPrivate() :
    qry_(0), hasXmlData_(false) {
  }

  bool viewReport(const QString &sourceFile);
  bool viewReport(const QString &sourceFile,
                  const QString &xmlDataSourceFile);

  FLSqlQuery *qry_;
  QDomDocument xml_;
  QDomDocument xmlData_;
  bool hasXmlData_;
};

bool FLJasperViewerPrivate::viewReport(const QString &sourceFile)
{
  if (!FLJasperEnginePrivate::loadJvm()) {
    AQ_WARN_NO_JAVAVM
    return false;
  }
  AQReportsInterface *aqReports = FLJasperEnginePrivate::loadAQReports();
  if (!aqReports) {
    AQ_WARN_NO_PLUGIN
    return false;
  }

  FLSqlDatabase *db = qry_->db();
  QString connStr;

  if (db->driverName().contains("QMYSQL")) {
    connStr = "jdbc:mysql://%1:%2/%3?user=%4&password=%5";
  } else {
    connStr = "jdbc:postgresql://%1:%2/%3?user=%4&password=%5";
  }
  connStr = connStr.arg(db->host())
            .arg(db->port())
            .arg(db->database())
            .arg(db->user())
            .arg(db->password());

  if (!aqReports->setConnection(connStr.local8Bit()))
    return false;

  bool ret = aqReports->viewReport(sourceFile.local8Bit());
  delete aqReports;
  return ret;
}

bool FLJasperViewerPrivate::viewReport(const QString &sourceFile,
                                       const QString &xmlDataSourceFile)
{
  if (!FLJasperEnginePrivate::loadJvm()) {
    AQ_WARN_NO_JAVAVM
    return false;
  }
  AQReportsInterface *aqReports = FLJasperEnginePrivate::loadAQReports();
  if (!aqReports) {
    AQ_WARN_NO_PLUGIN
    return false;
  }

  bool ret = aqReports->viewReport(sourceFile.local8Bit(),
                                   xmlDataSourceFile.local8Bit());
  delete aqReports;
  return ret;
}

FLJasperViewer::FLJasperViewer(QObject *parent, const char *name) :
  QObject(parent, name)
{
  d = new FLJasperViewerPrivate();
}

FLJasperViewer::~FLJasperViewer()
{
  delete d;
}

bool FLJasperViewer::setReportData(FLSqlQuery *q)
{
  if (!q)
    return false;
  if (!q->exec()) {
#ifdef FL_DEBUG
    qWarning("FLJasperViewer: " + tr("No se ha podido ejecutar la consulta del informe"));
#endif
    return false;
  }
  if (!q->next()) {
#ifdef FL_DEBUG
    qWarning("FLJasperViewer: " + tr("Consulta del informe vacia"));
#endif
    return false;
  }

  d->hasXmlData_ = false;
  d->qry_ = q;
  return true;
}

bool FLJasperViewer::setReportData(FLSqlQueryInterface *q)
{
  return setReportData(q->obj());
}

bool FLJasperViewer::setReportData(QDomNode n)
{
  d->hasXmlData_ = true;
  bool ret = true;
  d->xmlData_ = formatXmlData(n, &ret);
  return ret;
}

bool FLJasperViewer::setReportData(FLDomNodeInterface *n)
{
  return setReportData(n->obj());
}

bool FLJasperViewer::setReportTemplate(const QString &t)
{
  QString xml(d->qry_ ? d->qry_->db()->managerModules()->content(t + ".jrxml")
              : FLSqlConnections::database()->managerModules()->content(t + ".jrxml"));
  QTextCodec *codec = QTextCodec::codecForName("utf8");
  bool ret = FLUtil::domDocumentSetContent(d->xml_, codec->toUnicode(xml));
  if (ret) {
    QDomElement e(d->xml_.documentElement());
    if (e.isNull() || e.tagName().lower() != "jasperreport") {
#ifdef FL_DEBUG
      qWarning("FLJasperViewer: " + tr("La plantilla del informe no es del tipo JasperReport"));
#endif
      ret = false;
    }
  }
  return ret;
}

bool FLJasperViewer::exec()
{
  QString xmlFile;
  QString tmpFile;

  if (!d->hasXmlData_) {
    if (!d->qry_ || d->xml_.isNull())
      return false;

    QDomNodeList list(d->xml_.elementsByTagName("queryString"));

    for (int i = 0; i < list.count(); ++i) {
      QDomElement qryStr(list.item(i).toElement());
      QDomCDATASection qryDat(qryStr.firstChild().toCDATASection());
      QString data(qryDat.data());
      QString newData(d->qry_->sql());

      int posFrom = data.findRev("FROM", -1, true);
      data.remove(posFrom, data.length() - posFrom);

      posFrom = newData.findRev("FROM", -1, true);
      data += ' ' + newData.mid(posFrom);
      qryDat.setData(data);
    }

#ifdef FL_DEBUG
    qWarning("\nFLJasperViewer::exec() : " + d->xml_.toString(2));
#endif
  } else {
    if (d->xmlData_.isNull())
      return false;

    xmlFile = AQ_DISKCACHE_DIRPATH + "/jasper_xmldat_" +
              QDateTime::currentDateTime().toString("ddMMyyyyhhmmsszzz") + ".xml";
    QFile fi(xmlFile);
    if (fi.open(IO_WriteOnly)) {
      QTextStream t(&fi);
      t.setEncoding(QTextStream::UnicodeUTF8);
      d->xmlData_.firstChild().save(t, 0);
      fi.close();
    } else
      return false;
  }

  if (aqApp->multiLangEnabled()) {
    QDomNodeList nl(d->xml_.elementsByTagName("text"));
    QDomCDATASection it;
    for (uint i = 0; i < nl.count(); ++i) {
      it = nl.item(i).firstChild().toCDATASection();
      if (!it.isNull())
        it.setData(tr(it.data()));
    }
  }

  tmpFile = AQ_DISKCACHE_DIRPATH + "/jasper_" +
            QDateTime::currentDateTime().toString("ddMMyyyyhhmmsszzz") + ".jrxml";
  QFile fi(tmpFile);
  if (fi.open(IO_WriteOnly)) {
    QTextStream t(&fi);
    t.setEncoding(QTextStream::UnicodeUTF8);
    d->xml_.save(t, 2);
    fi.close();
  } else
    return false;

  if (!d->hasXmlData_) {
    bool ret = d->viewReport(tmpFile);
    QFile::remove(tmpFile);
    return ret;
  } else {
    bool ret = d->viewReport(tmpFile, xmlFile);
    QFile::remove(tmpFile);
    QFile::remove(xmlFile);
    return ret;
  }
}
