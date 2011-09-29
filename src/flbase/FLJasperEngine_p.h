/***************************************************************************
 FLJasperEngine_p.h
 -------------------
 begin                : 02/01/2011
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

#ifndef FLJASPERENGINE_P_H_
#define FLJASPERENGINE_P_H_

#include <string>
#include <list>

using namespace std;
typedef list<string> AQStdStringList;

typedef void (*AQMsgHandler)(FLJasperEngine::AQReportsMsgType, const char *, void *);
typedef void *(*aqReportsInterface)();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// AQReports Interfaces
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define AQ_JASPER_IFACE(I,V) \
  AQ##I##Interface * V = reinterpret_cast<AQ##I##Interface *>(d->aqReports_->create( #I ))

class AQReportsInterface
{
public:
  AQReportsInterface() {}
  virtual ~AQReportsInterface() {}
  virtual bool init(const char *) = 0;
  virtual const char *jarDeps() const = 0;
  virtual bool setConnection(const char *) = 0;
  virtual bool viewReport(const char *) = 0;
  virtual bool viewReport(const char *, const char *) = 0;
  virtual void installMsgHandler(AQMsgHandler, void *) = 0;
  virtual void *create(const char *) = 0;
};

class AQJasperCompileManagerInterface
{
public:
  virtual void compileReportToFile(const char *, const char *) = 0;
};

class AQJasperFillManagerInterface
{
public:
  virtual void fillReportToFile(const char *, const char *,
                                const char *, const char *,
                                const char *, const char *) = 0;
};

class AQJasperPrintManagerInterface
{
public:
  virtual void loadJasperPrint(const char *) = 0;
  virtual int pageCount() const = 0;
  virtual int pageWidth() const = 0;
  virtual int pageHeight() const = 0;
  virtual int pageOrientation() = 0;
  virtual void printPageToImage(const char *, int, const char *) = 0;
  virtual bool printReport(bool) = 0;
  virtual bool printReport(const char *, int = 1) = 0;
  virtual bool exportReportToFile(const char *, const char *) = 0;
};

class AQJavaPrintServiceInterface
{
public:
  virtual AQStdStringList printServiceNames() = 0;
private:
  virtual void *printService(const char *) = 0;
  friend class AQJasperPrintManagerInterface;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// AQReports Interfaces
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define AQ_WARN_NO_JAVAVM QApplication::restoreOverrideCursor();                                          \
  QMessageBox::warning(0, QApplication::tr("Aviso"),                                                      \
                       QApplication::tr("No se ha encontrado la Máquina Virtual de Java.\n")              \
                       + QApplication::tr("Se necesita para poder usar informes de JasperReports."),      \
                       QMessageBox::Ok, 0, 0);

#define AQ_WARN_NO_PLUGIN QApplication::restoreOverrideCursor();                                          \
  QMessageBox::warning(0, QApplication::tr("Aviso"),                                                      \
                       QApplication::tr("No se ha encontrado el plugin AQReports.\n")                     \
                       + QApplication::tr("Se necesita para poder usar informes de JasperReports.\n")     \
                       + QApplication::tr("Por favor visita http://abanq.org para obtenerlo."),           \
                       QMessageBox::Ok, 0, 0);

static inline QDomDocument formatXmlData(QDomNode n, bool *ok = 0)
{
  QString strXml(n.toDocument().toString());
  strXml.replace(QRegExp("\\s*\\|\\|\\s*' '\\s*\\|\\|\\s*"), "_");
  QDomDocument docXml;
  bool ret = FLUtil::domDocumentSetContent(docXml, strXml);
  if (ok)
    *ok = ret;
  return docXml;
}

class FLJasperEnginePrivate
{
public:

  FLJasperEnginePrivate(FLJasperEngine *q);
  ~FLJasperEnginePrivate();

  bool existsJasperFile() const;
  bool existsJRPrintFile() const;
  void cleanupJasperFile();
  void cleanupJRPrintFile();
  bool pluginLinked();
  QString defaultExportFormat() const;
  bool loadAQReportsPlugin();
  QString createTemplateFile();
  QString createDataFile();

  static bool loadJvm();
  static AQReportsInterface *loadAQReports(FLJasperEngine *q = 0);

private:

  FLJasperEngine *q_;
  bool showErrorDialog_;

  QString selectExpression_;
  QString datePattern_;
  QString numberPattern_;

  QDomDocument xmlData_;
  QDomDocument xmlTemplate_;

  QString jasperFile_;
  QString jrprintFile_;
  FLReportPages *reportPages_;

  QString defaultExportFormat_;

  AQReportsInterface *aqReports_;
  static QStringList jvmLibBlackList_;
  static QString jvmLib_;
  static aqReportsInterface aqReportsInterface_;

  friend class FLJasperEngine;
};

#endif /* FLJASPERENGINE_P_H_ */
