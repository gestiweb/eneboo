/***************************************************************************
 AQReports.cpp
 -------------------
 begin                : 09/06/2010
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

#include "AQReports_p.h"
#include "AQReports.h"
#include "AQJavaPrintService_p.h"
#include "AQJasperCompileManager_p.h"
#include "AQJasperFillManager_p.h"
#include "AQJasperPrintManager_p.h"

class AQReports : public AQReportsInterface
{
public:

  AQReports();
  virtual ~AQReports();

  bool init(const char *javacp);
  const char *jarDeps() const;
  bool setConnection(const char *connstr);
  bool viewReport(const char *sourceFile);
  bool viewReport(const char *sourceFile, const char *xmlDataSourceFile);
  void installMsgHandler(AQMsgHandler msgHandler, void *receiver);
  void *create(const char *className);

private:

  AQReportsPrivate *d;
};

extern "C" void *aqreports_interface()
{
  return new AQReports();
}

AQReports::AQReports()
{
  d = new AQReportsPrivate;
}

AQReports::~AQReports()
{
  delete d;
}

void *AQReports::create(const char *className)
{
  if (strcmp(className, "JasperCompileManager") == 0)
    return new AQJasperCompileManager(d);

  if (strcmp(className, "JasperFillManager") == 0)
    return new AQJasperFillManager(d);

  if (strcmp(className, "JasperPrintManager") == 0)
    return new AQJasperPrintManager(d);

  if (strcmp(className, "JavaPrintService") == 0)
    return new AQJavaPrintService(d);

  AQ_ERR_MSG("Can't create unknown class");
  return 0;
}

bool AQReports::init(const char *javacp)
{
#if defined(AQ_OS_MACX)
  int optc = 3;
  char *optv[optc];

  optv[0] = (char *) "-Djdbc.drivers=org.postgresql.Driver:com.mysql.jdbc.Driver";
  optv[1] = (char *) "-Djava.awt.headless=true";
  optv[2] = (char *) javacp;
#else
  int optc = 2;
  char *optv[optc];

  optv[0] = (char *) "-Djdbc.drivers=org.postgresql.Driver:com.mysql.jdbc.Driver";
  optv[1] = (char *) javacp;
#endif

  if (!d->initDone_ && initJavaWrapper(optc, optv) < 0) {
    AQ_ERR_MSG("initJavaWrapper failed");
    return false;
  } else
    d->initDone_ = true;

  return true;
}

const char *AQReports::jarDeps() const
{
  return JAR_DEPS;
}

bool AQReports::setConnection(const char *connstr)
{
  delete d->conn_;

  java::sql::DriverManager *driverMgr = new java::sql::DriverManager(wrapperIntern);
  try {
    d->conn_ = driverMgr->getConnection(connstr);
  } catch (jthrowable exc) {
    java::lang::Throwable *thr = new java::lang::Throwable(exc);
    AQ_ERR_MSG(thr->getMessage());
    return false;
  }

  delete driverMgr;

  return true;
}

bool AQReports::viewReport(const char *sourceFile)
{
  if (!d->conn_)
    return false;

  bool ret = true;
  JASPER::engine::JasperCompileManager *compileMgr = 0;
  JASPER::engine::JasperReport *report = 0;
  JASPER::engine::JasperFillManager *fillMgr = 0;
  JASPER::engine::JasperPrint *print = 0;
  JASPER::view::JasperViewer *viewer = 0;

  try {
    compileMgr = new JASPER::engine::JasperCompileManager(wrapperIntern);
    report = compileMgr->compileReport(sourceFile);

    fillMgr = new JASPER::engine::JasperFillManager(wrapperIntern);
    print = fillMgr->fillReport(report, 0, d->conn_);

    viewer = new JASPER::view::JasperViewer(wrapperIntern);
    viewer->viewReport(print, false);
  } catch (jthrowable exc) {
    java::lang::Throwable *thr = new java::lang::Throwable(exc);
    AQ_ERR_MSG(thr->getMessage());
    ret = false;
  }

  delete viewer;
  delete print;
  delete fillMgr;
  delete report;
  delete compileMgr;

  return ret;
}

bool AQReports::viewReport(const char *sourceFile, const char *xmlDataSourceFile)
{
  bool ret = true;
  JASPER::engine::JasperCompileManager *compileMgr = 0;
  JASPER::engine::JasperReport *report = 0;
  java::io::File *xmlFile = 0;
  JASPER::engine::data::JRXmlDataSource *xmlDatSrc = 0;
  JASPER::engine::JRDataSource *datSrc = 0;
  JASPER::engine::JasperFillManager *fillMgr = 0;
  JASPER::engine::JasperPrint *print = 0;
  JASPER::view::JasperViewer *viewer = 0;

  try {
    compileMgr = new JASPER::engine::JasperCompileManager(wrapperIntern);
    report = compileMgr->compileReport(sourceFile);

    xmlFile = new java::io::File(xmlDataSourceFile);
    xmlDatSrc = new JASPER::engine::data::JRXmlDataSource(xmlFile, "/KugarData/Row");
    xmlDatSrc->setDatePattern("yyyy-MM-dd");
    xmlDatSrc->setNumberPattern("###0.00;-###0.00");
    datSrc = new JASPER::engine::JRDataSource(xmlDatSrc->getJavaObject());

    fillMgr = new JASPER::engine::JasperFillManager(wrapperIntern);
    print = fillMgr->fillReport(report, 0, datSrc);

    viewer = new JASPER::view::JasperViewer(wrapperIntern);
    viewer->viewReport(print, false);
  } catch (jthrowable exc) {
    java::lang::Throwable *thr = new java::lang::Throwable(exc);
    AQ_ERR_MSG(thr->getMessage());
    ret = false;
  }

  delete viewer;
  delete print;
  delete fillMgr;
  delete datSrc;
  delete xmlDatSrc;
  delete xmlFile;
  delete report;
  delete compileMgr;

  return ret;
}

void AQReports::installMsgHandler(AQMsgHandler msgHandler, void *receiver)
{
  d->msgHandler_ = msgHandler;
  d->receiver_ = receiver;
}
