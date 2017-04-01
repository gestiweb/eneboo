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
#include <qprocess.h>
#include "FLObjectFactory.h"
#include "FLJasperViewer.h"
#include "FLJasperEngine.h"
#include "FLJasperEngine_p.h"
#include "FLSqlDatabase.h"
#include "FLManagerModules.h"

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
  QString driver;
  QString Argumentos;

  if (db->driverName().contains("QMYSQL")) {
    connStr = "jdbc:mysql://%1:%2/%3";
      driver = "com.mysql.jdbc.Driver";
  } else {
    connStr = "jdbc:postgresql://%1:%2/%3";
      driver = "org.postgresql.Driver";
  }

  connStr = connStr.arg(db->host()).arg(db->port()).arg(db->database());
  connStr.local8Bit();
   QString prefix(QString(AQ_LIB) + "/enebooreports.jar" );

  QString comando = "java -jar "+ prefix +" "+ sourceFile + " " + driver +" "+ connStr +" "+db->user()+" "+db->password();

  #ifdef FL_DEBUG
   qWarning(comando);
  #endif

   QDir pathActual;
   pathActual.setCurrent(AQ_DISKCACHE_DIRPATH);

   QProcess *proc;
   proc = new QProcess();
   proc->addArgument( "java" );
   proc->addArgument( "-jar" );
   proc->addArgument( prefix );
   proc->addArgument( sourceFile.local8Bit());
   proc->addArgument( driver );
   proc->addArgument( connStr.local8Bit());
   proc->addArgument( db->user() );
   proc->addArgument( db->password() );
  if ( !proc->start() ) {
    delete proc;
      QMessageBox::warning(0, QApplication::tr("Aviso"),
                           QApplication::tr("No se ha ejecutado el comnado de llamada a la librería Eneboo Reports.\n")
                               + QApplication::tr("Se necesita para poder usar informes de JaperReports."),
                           QMessageBox::Ok, 0, 0);
    return false;
   }  
   pathActual.setCurrent(AQ_USRHOME);
   while ( proc->isRunning() )
    qApp->processEvents();

  return true;
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

   QString prefix(QString(AQ_LIB) + "/enebooreports.jar" );
   QString driver = "XMLDAT";

QProcess *proc;
   proc = new QProcess();
   proc->addArgument( "java" );
   proc->addArgument( "-jar" );
   proc->addArgument( prefix );
   proc->addArgument( sourceFile.local8Bit());
   proc->addArgument( driver );
   proc->addArgument( xmlDataSourceFile.local8Bit());

 if ( !proc->start() ) {
    delete proc;
      QMessageBox::warning(0, QApplication::tr("Aviso"),
                           QApplication::tr("No se ha ejecutado el comnado de llamada a la librería Eneboo Reports.\n")
                               + QApplication::tr("Se necesita para poder usar informes de JaperReports."),
                           QMessageBox::Ok, 0, 0);
    return false;
   } 

 while ( proc->isRunning() )
    qApp->processEvents();

  return true;


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
     FLSqlDatabase *db = d->qry_->db();
    char *packageVersion_;
    bool ret;
    QByteArray cabeceraBa;
    QByteArray datosFichero;
    QString cabecera;
    qWarning("Cargando reporte " +t+".");
    QByteArray byteCode;
    QByteArray datosFicheroExpandido;
    QByteArray Nuevo;
    QString jrxml;
    char *space;
    QDomDocument enebooReportHead;
    if (!db->dbAux())
      return false;
    QSqlQuery reportQry(QString::null, db->dbAux());
    reportQry.setForwardOnly(true);
    if(!reportQry.exec("SELECT data FROM flreports WHERE name = '" + t + "'"))
        {
        qWarning("No se ha ejecutado la consulta");
        return false;
        }

    else
    {
    while (reportQry.next())
           {
         byteCode = reportQry.value(0).toByteArray();
           }
     QDataStream reportePkg_(byteCode, IO_ReadOnly);

     reportePkg_ >> packageVersion_;

    reportePkg_ >> cabeceraBa;
    QTextIStream ti(qUncompress(cabeceraBa));
    cabecera = ti.read(); //Recogemos la cabecera
    bool maestro = false;
    QTextCodec *codecReport = QTextCodec::codecForName("utf8");
    ret = FLUtil::domDocumentSetContent(enebooReportHead, codecReport->toUnicode(cabecera));
    if (ret) {
              QDomElement enebooReportFile(enebooReportHead.documentElement());
              if (enebooReportFile.tagName().lower() != "files") {
#ifdef FL_DEBUG
              qWarning("FLJasperViewer: " + tr("La cabecera de fichero no es del tipo Eneboo Report"));
#endif
              ret = false;
                                             }
              else
                                             { //Procesamos la cebecera
              QDomElement nodoLista;
              QString nombreFichero;

              QDomNodeList lista(enebooReportFile.elementsByTagName("name"));
                        for (int i = 0; i < lista.count(); ++i) {
                                                                 nodoLista = lista.item(i).toElement();
                                                                 nombreFichero = nodoLista.text();
                                                                 nombreFichero =  AQ_DISKCACHE_DIRPATH + "/" + nombreFichero;
                                                                 qWarning(nombreFichero);
                                                                 reportePkg_ >> datosFichero;

                                                                 if (nombreFichero.endsWith(".jrxml"))
                                                                                    {
                                                                                     maestro = true;
                                                                                     qWarning("Encontrado fichero jrxml");
                                                                                     QTextIStream contenidoJrxml(qUncompress(datosFichero));
                                                                                     jrxml = contenidoJrxml.read();
                                                                                    // qWarning(jrxml);
                                                                                     }
                                                                                     else
                                                                                     {
                                                                                      QFile fir(nombreFichero);
                                                                                      if (fir.open(IO_WriteOnly))
                                                                                           {
                                                                                            fir.writeBlock(qUncompress(datosFichero));
                                                                                            fir.close();
                                                                                              } else return false;




                                                                                      }
                                                               }
                        if (maestro == false) return false; //Si no encuentro un frxml cancelo.


                                     }
             }
    }

  QTextCodec *codec = QTextCodec::codecForName("utf8");
  QString xml = jrxml;
    ret = FLUtil::domDocumentSetContent(d->xml_, codec->toUnicode(xml));
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
