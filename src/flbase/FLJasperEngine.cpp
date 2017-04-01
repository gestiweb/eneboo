/***************************************************************************
 FLJasperEngine.cpp
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

#include <qlibrary.h>

#include "mpagecollection.h"
#include "FLReportPages.h"
#include "FLObjectFactory.h"
#include "FLJasperEngine.h"
#include "FLJasperEngine_p.h"

FLJasperEngine::FLJasperEngine(QObject *parent) :
  QObject(parent)
{
  d = new FLJasperEnginePrivate(this);
}

FLJasperEngine::~FLJasperEngine()
{
  delete d;
}

bool FLJasperEngine::setReportData(QDomNode n)
{
  bool ret = true;
  d->xmlData_ = formatXmlData(n, &ret);
  return ret;
}

bool FLJasperEngine::setReportData(FLDomNodeInterface *n)
{
  return setReportData(n->obj());
}

bool FLJasperEngine::setReportTemplate(const QString &t)
{
  QString xml(FLSqlConnections::database()->managerModules()->content(t + ".jrxml"));
  QTextCodec *codec = QTextCodec::codecForName("utf8");
  bool ret = FLUtil::domDocumentSetContent(d->xmlTemplate_, codec->toUnicode(xml));
  if (ret) {
    QDomElement e(d->xmlTemplate_.documentElement());
    if (e.isNull() || e.tagName().lower() != "jasperreport") {
      msgHandler(AQErrorMsg, tr("La plantilla del informe no es del tipo JasperReport"));
      ret = false;
    }
  }
  return ret;
}

FLDomNodeInterface *FLJasperEngine::reportData() const
{
  return FLDomNodeInterface::nodeInterface(d->xmlData_);
}

QDomDocument FLJasperEngine::rptXmlData() const
{
  return d->xmlData_;
}

FLDomNodeInterface *FLJasperEngine::reportTemplate() const
{
  return FLDomNodeInterface::nodeInterface(d->xmlTemplate_);
}

QDomDocument FLJasperEngine::rptXmlTemplate() const
{
  return d->xmlTemplate_;
}

void FLJasperEngine::setDataSourceParameters(const QString &selectExpression,
                                             const QString &datePattern,
                                             const QString &numberPattern)
{
  d->selectExpression_ = selectExpression;
  d->datePattern_ = datePattern;
  d->numberPattern_ = numberPattern;
}

QString FLJasperEngine::compileReport()
{
  if (!d->pluginLinked())
    return QString::null;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QString tplFile(d->createTemplateFile());
  if (tplFile.isEmpty()) {
    QApplication::restoreOverrideCursor();
    return QString::null;
  }
  QString jasperFile(tplFile + QString::fromLatin1(".jasper"));

  AQ_JASPER_IFACE(JasperCompileManager, compileMgr);
  compileMgr->compileReportToFile(tplFile.local8Bit(), jasperFile.local8Bit());
  delete compileMgr;
  QFile::remove(tplFile);
  if (!QFile::exists(jasperFile)) {
    QApplication::restoreOverrideCursor();
    return QString::null;
  }

  d->cleanupJasperFile();
  d->jasperFile_ = jasperFile;

  QApplication::restoreOverrideCursor();
  return jasperFile;
}

QString FLJasperEngine::fillReport()
{
  if (!d->pluginLinked())
    return QString::null;

  if (!d->existsJasperFile() && compileReport().isEmpty())
    return QString::null;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QString dataFile(d->createDataFile());
  if (dataFile.isEmpty()) {
    QApplication::restoreOverrideCursor();
    return QString::null;
  }
  QString jrprintFile(dataFile + QString::fromLatin1(".jrprint"));

  AQ_JASPER_IFACE(JasperFillManager, fillMgr);
  fillMgr->fillReportToFile(d->jasperFile_.local8Bit(), jrprintFile.local8Bit(),
                            dataFile.local8Bit(), d->selectExpression_.local8Bit(),
                            d->datePattern_.local8Bit(), d->numberPattern_.local8Bit());
  delete fillMgr;
  QFile::remove(dataFile);
  if (!QFile::exists(jrprintFile)) {
    QApplication::restoreOverrideCursor();
    return QString::null;
  }

  d->cleanupJRPrintFile();
  d->jrprintFile_ = jrprintFile;

  QApplication::restoreOverrideCursor();
  return jrprintFile;
}

FLReportPages *FLJasperEngine::printReportToPages()
{
  FLReportPages *pages = new FLReportPages();

  if (!d->pluginLinked())
    return pages;

  if (!d->existsJRPrintFile() && fillReport().isEmpty())
    return pages;

  QApplication::setOverrideCursor(Qt::WaitCursor);
  FLUtil::createProgressDialog(tr("Cargando informe..."), 1, d->jrprintFile_);
  FLUtil::setProgress(0, d->jrprintFile_);

  AQ_JASPER_IFACE(JasperPrintManager, printMgr);
  printMgr->loadJasperPrint(d->jrprintFile_.local8Bit());

  int pageCount = printMgr->pageCount();
  QString imgFile(d->jrprintFile_ + QString::fromLatin1(".png"));
  MPageCollection *mpages = new MPageCollection();
  QImage img;
  QPainter pte;

  FLUtil::setTotalSteps(pageCount, d->jrprintFile_);
  pages->setPageCollection(mpages);
  pages->setPageOrientation(printMgr->pageOrientation());
  pages->setPageDimensions(QSize(printMgr->pageWidth(), printMgr->pageHeight()));
  for (int i = 0; i < pageCount; ++i) {
    mpages->appendPage();
    printMgr->printPageToImage(imgFile.local8Bit(), i, "png");
    img.load(imgFile, "PNG");
    pte.begin(mpages->getCurrentPage());
    pte.drawImage(0, 0, img);
    pte.end();
    QFile::remove(imgFile);
    FLUtil::setProgress(i + 1, d->jrprintFile_);
  }
  delete printMgr;

  FLUtil::destroyProgressDialog(d->jrprintFile_);
  QApplication::restoreOverrideCursor();
  d->reportPages_ = pages;
  return pages;
}

void FLJasperEngine::updateReportPages()
{
  if (!d->reportPages_)
    return;

  if (compileReport().isEmpty() || fillReport().isEmpty())
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);
  FLUtil::createProgressDialog(tr("Cargando informe..."), 1, d->jrprintFile_);
  FLUtil::setProgress(0, d->jrprintFile_);

  AQ_JASPER_IFACE(JasperPrintManager, printMgr);
  printMgr->loadJasperPrint(d->jrprintFile_.local8Bit());

  int pageCount = printMgr->pageCount();
  QString imgFile(d->jrprintFile_ + QString::fromLatin1(".png"));
  MPageCollection *mpages = d->reportPages_->pageCollection();
  int mPagesCount = mpages->pageCount();
  QImage img;
  QPainter pte;

  FLUtil::setTotalSteps(pageCount, d->jrprintFile_);
  mpages->setPageOrientation(printMgr->pageOrientation());
  mpages->setPageDimensions(QSize(printMgr->pageWidth(), printMgr->pageHeight()));
  for (int i = 0; i < pageCount; ++i) {
    if (i >= mPagesCount)
      mpages->appendPage();
    else
      mpages->setCurrentPage(i);
    printMgr->printPageToImage(imgFile.local8Bit(), i, "png");
    img.load(imgFile, "PNG");
    pte.begin(mpages->getCurrentPage());
    pte.drawImage(0, 0, img);
    pte.end();
    QFile::remove(imgFile);
    FLUtil::setProgress(i + 1, d->jrprintFile_);
  }
  delete printMgr;

  FLUtil::destroyProgressDialog(d->jrprintFile_);
  QApplication::restoreOverrideCursor();
}

bool FLJasperEngine::printReport(bool withPrintDialog)
{
  if (!d->pluginLinked())
    return false;

  if (!d->existsJRPrintFile() && fillReport().isEmpty())
    return false;

  AQ_JASPER_IFACE(JasperPrintManager, printMgr);
  printMgr->loadJasperPrint(d->jrprintFile_.local8Bit());
  bool ret = printMgr->printReport(withPrintDialog);
  delete printMgr;
  return ret;
}

bool FLJasperEngine::printReport(const QString &printServiceName, int copies)
{
  if (!d->pluginLinked())
    return false;

  if (!d->existsJRPrintFile() && fillReport().isEmpty())
    return false;

  AQ_JASPER_IFACE(JasperPrintManager, printMgr);
  printMgr->loadJasperPrint(d->jrprintFile_.local8Bit());
  bool ret = printMgr->printReport(printServiceName.local8Bit(), copies);
  delete printMgr;
  return ret;
}

QStringList FLJasperEngine::printServiceNames()
{
  if (!d->pluginLinked())
    return QStringList();

  AQ_JASPER_IFACE(JavaPrintService, printSrv);
  AQStdStringList srvNames = printSrv->printServiceNames();
  AQStdStringList::const_iterator it(srvNames.begin());
  QStringList ret;

  while (it != srvNames.end())
    ret << QString(*it++);

  delete printSrv;
  return ret;
}

bool FLJasperEngine::exportReportToFile(const QString &outFileName, const QString &format)
{
  if (!d->pluginLinked())
    return false;

  if (!d->existsJRPrintFile() && fillReport().isEmpty())
    return false;

  AQ_JASPER_IFACE(JasperPrintManager, printMgr);
  printMgr->loadJasperPrint(d->jrprintFile_.local8Bit());
  bool ret = printMgr->exportReportToFile(outFileName.local8Bit(),
                                          format.upper().local8Bit());
  delete printMgr;
  return ret;
}

void FLJasperEngine::execExportFileDialog(const QString &defaultFileName,
                                          const QString &defaultFormat)
{
  bool multiLang = aqApp->multiLangEnabled();
  aqApp->setMultiLang(false);

  QFileDialog *fd = new QFileDialog(0, "aq_execExportFileDialog", true);

  fd->setMode(QFileDialog::AnyFile);
  fd->setFilters(QStringList()
                 << "Pdf"
                 << "Csv"
                 << "Docx"
                 << "EmbeddedImagesXml"
                 << "Html"
                 << "MultipleSheetsXls"
                 << "Odt"
                 << "Rtf"
                 << "SingleSheetXls"
                 << "Xml");
  setDefaultExportFormat(defaultFormat);
  fd->setSelectedFilter(d->defaultExportFormat());
  fd->setSelection(defaultFileName);
  fd->setCaption(tr("Exportar Informe a..."));

  connect(fd, SIGNAL(filterSelected(const QString &)),
          this, SLOT(setDefaultExportFormat(const QString &)));

  if (fd->exec() == QDialog::Accepted) {
    QString outFile(fd->selectedFile());
    if (!outFile.isEmpty())
      exportReportToFile(outFile, d->defaultExportFormat());
  }

  delete fd;
  aqApp->setMultiLang(multiLang);
}

void FLJasperEngine::exportReportToPdf(const QString &outFileName,
                                       bool withFileDialog)
{
  if (withFileDialog)
    execExportFileDialog(outFileName, "Pdf");
  else
    exportReportToFile(outFileName, "Pdf");
}

void FLJasperEngine::exportReportToCsv(const QString &outFileName,
                                       bool withFileDialog)
{
  if (withFileDialog)
    execExportFileDialog(outFileName, "Csv");
  else
    exportReportToFile(outFileName, "Csv");
}


QString FLJasperEngine::lastJasperFile() const
{
  return d->jasperFile_;
}

QString FLJasperEngine::lastJRPrintFile() const
{
  return d->jrprintFile_;
}

FLReportPages *FLJasperEngine::lastReportPages() const
{
  return d->reportPages_;
}

void FLJasperEngine::setShowErrorDialog(bool b)
{
  d->showErrorDialog_ = b;
}

void FLJasperEngine::setDefaultExportFormat(const QString &format)
{
  d->defaultExportFormat_ = format;
}

void FLJasperEngine::msgHandler(AQReportsMsgType type, const QString &msg)
{
  emit msgReceived(type, msg);

  switch (type) {
    case AQErrorMsg:
#ifdef FL_DEBUG
      qWarning("FLJasperEngine::AQErrorMsg : " + msg);
#endif
      if (d->showErrorDialog_) {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(0, QApplication::tr("Error"), msg, QMessageBox::Ok, 0, 0);
      }
      break;
    case AQLogMsg:
#ifdef FL_DEBUG
      qWarning("FLJasperEngine::AQLogMsg : " + msg);
#endif
      break;
    case AQSysMsg:
      break;
  }
}

void JasperEngineMsgHandler(FLJasperEngine::AQReportsMsgType type, const char *msg, void *rcv)
{
  reinterpret_cast<FLJasperEngine *>(rcv)->msgHandler(type, QString::fromLatin1(msg));
}

QString FLJasperEnginePrivate::jvmLib_;
QStringList FLJasperEnginePrivate::jvmLibBlackList_;
aqReportsInterface FLJasperEnginePrivate::aqReportsInterface_ = 0;

FLJasperEnginePrivate::FLJasperEnginePrivate(FLJasperEngine *q) :
  q_(q), showErrorDialog_(true), aqReports_(0), reportPages_(0),
  selectExpression_("/KugarData/Row"), datePattern_("yyyy-MM-dd"),
  numberPattern_("###0.00;-###0.00")
{
}

FLJasperEnginePrivate::~FLJasperEnginePrivate()
{
  cleanupJasperFile();
  cleanupJRPrintFile();
  delete aqReports_;
}

bool FLJasperEnginePrivate::existsJasperFile() const
{
  return (!jasperFile_.isEmpty() && QFile::exists(jasperFile_));
}

bool FLJasperEnginePrivate::existsJRPrintFile() const
{
  return (!jrprintFile_.isEmpty() && QFile::exists(jrprintFile_));
}

void FLJasperEnginePrivate::cleanupJasperFile()
{
  if (existsJasperFile())
    QFile::remove(jasperFile_);
  jasperFile_ = QString::null;
}

void FLJasperEnginePrivate::cleanupJRPrintFile()
{
  if (existsJRPrintFile())
    QFile::remove(jrprintFile_);
  jrprintFile_ = QString::null;
}

bool FLJasperEnginePrivate::pluginLinked()
{
  if (aqReports_ != 0)
    return true;
  QApplication::setOverrideCursor(Qt::WaitCursor);
  bool linked = loadAQReportsPlugin();
  QApplication::restoreOverrideCursor();
  return linked;
}

QString FLJasperEnginePrivate::defaultExportFormat() const
{
  return (defaultExportFormat_.isEmpty() ? "Pdf" : defaultExportFormat_);
}

bool FLJasperEnginePrivate::loadAQReportsPlugin()
{
  if (!loadJvm()) {
    AQ_WARN_NO_JAVAVM
    return false;
  }
  aqReports_ = loadAQReports(q_);
  if (!aqReports_) {
    AQ_WARN_NO_PLUGIN
    return false;
  }
  return true;
}

QString FLJasperEnginePrivate::createTemplateFile()
{
  if (xmlTemplate_.isNull())
    return QString::null;

  QDomDocument xml = xmlTemplate_.cloneNode(true).toDocument();

  if (aqApp->multiLangEnabled()) {
    QDomNodeList nl(xml.elementsByTagName("text"));
    QDomCDATASection it;
    for (uint i = 0; i < nl.count(); ++i) {
      it = nl.item(i).firstChild().toCDATASection();
      if (!it.isNull())
        it.setData(q_->tr(it.data()));
    }

    QDomElement ite;
    nl = xml.elementsByTagName("textField");
    for (uint i = 0; i < nl.count(); ++i) {
      ite = nl.item(i).toElement();
      if (!ite.isNull() && ite.hasAttribute("pattern")) {
        QString pt(ite.attribute("pattern"));
        if (pt.startsWith("@@") && pt.endsWith("@@"))
          ite.setAttribute("pattern", q_->tr(pt));
      }
    }
  }

  QString fileName(AQ_DISKCACHE_DIRPATH + "/jasper_" +
                   QDateTime::currentDateTime().toString("ddMMyyyyhhmmsszzz") +
                   QString::fromLatin1(".jrxml"));
  QFile fi(fileName);
  if (!fi.open(IO_WriteOnly))
    return QString::null;

  QTextStream t(&fi);
  t.setEncoding(QTextStream::UnicodeUTF8);
  xml.save(t, 0);

  return fileName;
}

QString FLJasperEnginePrivate::createDataFile()
{
  if (xmlData_.isNull())
    return QString::null;

  QString fileName(AQ_DISKCACHE_DIRPATH + "/jasper_xmldat_" +
                   QDateTime::currentDateTime().toString("ddMMyyyyhhmmsszzz") +
                   QString::fromLatin1(".xml"));
  QFile fi(fileName);
  if (!fi.open(IO_WriteOnly))
    return QString::null;

  QTextStream t(&fi);
  t.setEncoding(QTextStream::UnicodeUTF8);
  xmlData_.firstChild().save(t, 0);

  return fileName;
}

bool FLJasperEnginePrivate::loadJvm()
{
  return true; //Pasamos por alto la precarga
  if (jvmLib_.isEmpty()) {
    jvmLib_ = FLUtil::readSettingEntry("aqreports/jvmLib").toString();
    QStringList searchPaths;
    bool searchAgain = false;

    if (jvmLib_.isEmpty()) {
      QString javaHome(getenv("JAVA_HOME"));
#if defined(Q_OS_MACX)
      QChar sep(':');
      QString ld_library_path("DYLD_LIBRARY_PATH");
      QString jvmName("JavaVM");
      if (!javaHome.isEmpty())
        searchPaths << javaHome;
      searchPaths << "/System/Library/Frameworks/JavaVM.framework/Versions/Current";
#endif
#if defined(Q_OS_LINUX)
      QChar sep(':');
      QString ld_library_path("LD_LIBRARY_PATH");
      QString jvmName("libjvm.so");
      if (!javaHome.isEmpty())
        searchPaths << javaHome;
      searchPaths << "/usr/lib/jvm/default-java" << "/usr/lib/jvm/java";
#endif
#if defined(Q_OS_WIN32)
      QChar sep(';');
      QString ld_library_path("PATH");
      QString jvmName("jvm.dll");
      if (!javaHome.isEmpty())
        searchPaths << javaHome;
      searchPaths << "c:/program files (x86)/java"
                  << "c:/program files/java";
#endif

      searchPaths += QStringList::split(sep, getenv(ld_library_path.local8Bit()));
      QStringList list(FLUtil::findFiles(searchPaths, jvmName));
      for (QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
#if defined(Q_OS_WIN32)
        if (!(*it).contains("/bin/") || jvmLibBlackList_.contains(*it))
          continue;
#else
        if (jvmLibBlackList_.contains(*it))
          continue;
#endif
        QLibrary libjvm(*it);
        if (!libjvm.resolve("JNI_CreateJavaVM"))
          continue;
        jvmLib_ = *it;
        FLUtil::writeSettingEntry("aqreports/jvmLib", QDir::convertSeparators(jvmLib_));
        break;
      }
    } else
      searchAgain = true;

#if defined(Q_OS_WIN32)
    if (!jvmLib_.isEmpty()) {
      QString tmpJvm(jvmLib_);
      QLibrary msvcr(QDir::cleanDirPath(tmpJvm.replace("jvm.dll", "") + "/../new_plugin") + "/msvcr71.dll");
      msvcr.setAutoUnload(false);
      msvcr.load();
    }
#endif

    QLibrary libjvm(jvmLib_.isEmpty() ? "jvm" : jvmLib_);
    libjvm.setAutoUnload(false);
    if (!libjvm.resolve("JNI_CreateJavaVM")) {
      if (searchAgain)
        jvmLibBlackList_ << jvmLib_;
      jvmLib_ = QString::null;
      FLUtil::writeSettingEntry("aqreports/jvmLib", QDir::convertSeparators(jvmLib_));
      return (searchAgain ? loadJvm() : false);
    }
  }

  return true;
}

AQReportsInterface *FLJasperEnginePrivate::loadAQReports(FLJasperEngine *q)
{
  AQReportsInterface *aqReports = 0;

  if (!aqReportsInterface_) {
#if defined(Q_OS_MACX)
    QChar sep(':');
    QLibrary aqreports(AQ_LIB + "/aqreports");
#else
#if defined(Q_OS_LINUX)
    QChar sep(':');
#endif
#if defined(Q_OS_WIN32)
    QChar sep(';');
#endif
    QLibrary aqreports("aqreports");
#endif
    aqreports.setAutoUnload(false);
    aqReportsInterface_ = (aqReportsInterface)(aqreports.resolve("aqreports_interface"));
    if (!aqReportsInterface_)
      return 0;

    aqReports = reinterpret_cast<AQReportsInterface *>(aqReportsInterface_());
    if (q)
      aqReports->installMsgHandler(JasperEngineMsgHandler, q);

    QString prefix(AQ_LIB + "/javaclasses/");
    QString cpath("-Djava.class.path=" + prefix + QStringList::split(':', aqReports->jarDeps()).join(sep + prefix));
    QStringList list(FLUtil::findFiles(prefix + "preload", "*.jar"));
    if (!list.isEmpty())
      cpath += sep + list.join(sep);

    if (!aqReports->init(cpath.local8Bit()))
      return 0;
  }

  if (!aqReports) {
    aqReports = reinterpret_cast<AQReportsInterface *>(aqReportsInterface_());
    if (q)
      aqReports->installMsgHandler(JasperEngineMsgHandler, q);
  }

  return aqReports;
}
