/***************************************************************************
 FLReportViewer.cpp
 -------------------
 begin                : vie jun 28 2002
 copyright            : (C) 2002-2005 by InfoSiAL S.L.
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

#include <qeventloop.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qclipboard.h> /// Añadimos libreria de portapapeles
#include "mreportviewer.h"
#include "FLReportViewer.h"
#include "FLReportEngine.h"
#include "FLSmtpClient.h"
#include "FLUtil.h"
#include "FLPicture.h"
#include "FLStylePainter.h"
#include "FLReportPages.h"

#include "AQConfig.h"

FLReportViewer::FLReportViewer(QWidget *parent, const char *name, bool embedInParent, FLReportEngine *rptEngine) :
  FLWidgetReportViewer(parent, name,
                       ((parent && embedInParent) ? 0 :
                        (Qt::WStyle_Customize |
                         Qt::WStyle_Maximize |
                         Qt::WStyle_Title |
                         Qt::WStyle_NormalBorder |
                         Qt::WType_Dialog |
                         Qt::WShowModal |
                         Qt::WStyle_SysMenu))),
  loop(false), reportPrinted_(false),
  rptViewer_(0), rptEngine_(0),
  report(0), qry_(0),
  slotsPrintDisabled_(false), slotsExportDisabled_(false),
  printing_(false)
{

  if (!name)
    QObject::setName("FLReportViewer");

  embedInParent_ = (parent && embedInParent);

  if (embedInParent_) {
    autoClose_ = false;
    menubar->hide();
    chkAutoClose->hide();
    spnResolution->hide();
    spnPixel->hide();
    salir->setVisible(false);
    if (!parent->layout()) {
      QVBoxLayout *lay = new QVBoxLayout(parent);
      lay->addWidget(this);
    } else
      parent->layout()->add(this);
  } else {
    autoClose_ = FLUtil::readSettingEntry("rptViewer/autoClose", "false").toBool();
    chkAutoClose->setChecked(autoClose_);
  }

  rptViewer_ = new MReportViewer(this);
  setReportEngine(rptEngine == 0 ? new FLReportEngine(this) : rptEngine);

  setFont(qApp->font());
  setFocusPolicy(QWidget::StrongFocus);

  lePara->setText(FLUtil::readSettingEntry("email/to").toString());
  leDe->setText(FLUtil::readSettingEntry("email/from").toString());
  leMailServer->setText(FLUtil::readSettingEntry("email/mailserver").toString());

  initCentralWidget_ = centralWidget();

  smtpClient_ = new FLSmtpClient(this);
  connect(smtpClient_, SIGNAL(status(const QString &)), lblEstado, SLOT(setText(const QString &)));

  setCentralWidget(rptViewer_);
  frEMail->hide();
  initCentralWidget_->hide();
  guacaInit(); //Inicializamos guaca
  if (!embedInParent_) {
    spnResolution->setValue(FLUtil::readSettingEntry("rptViewer/dpi", QVariant(rptViewer_->resolution()).toString()).toInt());
    spnPixel->setValue(FLUtil::readSettingEntry("rptViewer/pixel",
                                                QString::number((double) rptEngine_->relDpi(), 'f', 2)).toDouble() * 10.);
  }
  report = rptViewer_->reportPages();
}

FLReportViewer::~FLReportViewer()
{
  if (!embedInParent_)
    FLUtil::writeSettingEntry("rptViewer/autoClose", QVariant(autoClose_).toString());
}

void FLReportViewer::setReportEngine(FLReportEngine *r)
{
  if (rptEngine_ == r)
    return;
  bool noSigDestroy = !(sender() && sender() == rptEngine_);
  if (rptEngine_) {
    disconnect(rptEngine_, SIGNAL(destroyed()), this, SLOT(setReportEngine()));
    if (noSigDestroy && rptEngine_->parent() == this) {
      rptEngine_->deleteLater();
      rptEngine_ = 0;
    }
  }
  rptEngine_ = r;
  if (rptEngine_) {
    template_ = rptEngine_->rptNameTemplate();
    qry_ = rptEngine_->rptQueryData();
    if (rptEngine_->rptXmlTemplate())
      xmlTemplate_ = *rptEngine_->rptXmlTemplate();
    if (rptEngine_->rptXmlTemplate())
      xmlData_ = *rptEngine_->rptXmlTemplate();
    connect(rptEngine_, SIGNAL(destroyed()), this, SLOT(setReportEngine()));

    ledStyle->setDisabled(false);
    save_page_SVG->setDisabled(false);
    save_page_tpl_SVG->setDisabled(false);
    ledStyle->setDisabled(false);
    load_tpl_SVG->setDisabled(false);
  } else {
    ledStyle->setDisabled(true);
    save_page_SVG->setDisabled(true);
    save_page_tpl_SVG->setDisabled(true);
    ledStyle->setDisabled(true);
    load_tpl_SVG->setDisabled(true);
  }
  if (noSigDestroy)
    rptViewer_->setReportEngine(rptEngine_);
}

void FLReportViewer::exec()
{
    
    QString fileTemp = "document_";
    fileTemp += QDateTime::currentDateTime().toString().replace(":", "").replace(" ", ""); //Esto lo cambiamos por un string con el tiempo
    fileTemp += ".pdf";
    
    if (!soyGuaca_) /// Si no soy Guaca , me comporto normalmente.
    {

  if (loop) {
#ifdef FL_DEBUG
    qWarning(tr("FLReportViewer::exec(): Se ha detectado una llamada recursiva"));
#endif
    return;
  }
  QWidget::show();
  if (embedInParent_)
    return;
  loop = true;
  QApplication::eventLoop()->enterLoop();
  clearWFlags(WShowModal);
} else /// de lo contrario, soy Guaca
          {
          QClipboard *clipboard = QApplication::clipboard();
           reportPrinted_ = rptViewer_->printReportToPDF(guacaFolder_ + fileTemp);
           clipboard->setText("GUACA_IMPRIME" + fileTemp);  /// Pongo en el portapapeles la bandera
           } 
}

QString FLReportViewer::csvData()
{
  return (rptEngine_ ? rptEngine_->csvData() : QString::null);
}

void FLReportViewer::closeEvent(QCloseEvent *e)
{
  if (printing_)
    return;
    
  QWidget::show();
  frameGeometry();
  QWidget::hide();

  if (!embedInParent_) {
    QRect geo(x(), y(), width(), height());
    aqApp->saveGeometryForm(QObject::name(), geo);
  }

  if (loop && !embedInParent_) {
    loop = false;
    QApplication::eventLoop()->exitLoop();
  }

  QWidget::closeEvent(e);
  deleteLater();
}

void FLReportViewer::showEvent(QShowEvent *e)
{
  QWidget::showEvent(e);

  if (!embedInParent_) {
    QRect geo(aqApp->geometryForm(QObject::name()));
    if (geo.isValid()) {
      QRect desk = QApplication::desktop() ->availableGeometry(this);
      QRect inter = desk.intersect(geo);
      resize(geo.size());
      if (inter.width() * inter.height() > (geo.width() * geo.height() / 20))
        move(geo.topLeft());
    }
  }
}

bool FLReportViewer::renderReport(const int initRow, const int initCol, const bool append, const bool displayReport)
{
    if (!rptEngine_)
        return false;
    uint flags = 0;
    if (append) {
    	flags = flags | MReportViewer::Append;
    }
    if (displayReport) {
    	flags = flags | MReportViewer::Display;
    }
    
    bool ret = renderReport(initRow, initCol, flags);
    report = rptViewer_->reportPages();
    return ret;
}

bool FLReportViewer::renderReport(const int initRow, const int initCol, const uint flags)
{
  if (!rptEngine_)
    return false;
  bool ret = rptViewer_->renderReport(initRow, initCol, flags);
  report = rptViewer_->reportPages();
  return ret;
}

void FLReportViewer::slotFirstPage()
{
  rptViewer_->slotFirstPage();
}

void FLReportViewer::slotLastPage()
{
  rptViewer_->slotLastPage();
}

void FLReportViewer::slotNextPage()
{
  rptViewer_->slotNextPage();
}

void FLReportViewer::slotPrevPage()
{
  rptViewer_->slotPrevPage();
}

void FLReportViewer::slotZoomUp()
{
  rptViewer_->slotZoomUp();
}

void FLReportViewer::slotZoomDown()
{
  rptViewer_->slotZoomDown();
}

void FLReportViewer::exportFileCSVData()
{
  if (slotsExportDisabled_)
    return;

  QString fileName = QFileDialog::getSaveFileName("", tr("Fichero CSV (*.csv *.txt)"), this, tr("Exportar a CSV"),
                                                  tr("Exportar a CSV"));

  if (fileName.isEmpty())
    return;

  if (!fileName.upper().contains(".CSV"))
    fileName += ".csv";

  if (QFile::exists(fileName)
      && QMessageBox::question(this, tr("Sobreescribir %1").arg(fileName),
                               tr("Ya existe un fichero llamado %1. ¿ Desea sobreescribirlo ?").arg(fileName), tr("&Sí"),
                               tr("&No"), QString::null, 0, 1)) {
    return;
  }

  QFile file(fileName);

  if (file.open(IO_WriteOnly)) {
    QTextStream stream(&file);
    stream << csvData() << "\n";
    file.close();
  } else {
    QMessageBox::critical(
      this,
      tr("Error abriendo fichero"),
      tr("No se pudo abrir el fichero %1 para escribir: %2").arg(fileName,
                                                                 qApp->translate("QFile", file.errorString())));
  }
}

void FLReportViewer::exportToPDF()
{
  if (slotsExportDisabled_)
    return;

  QString fileName = QFileDialog::getSaveFileName("", tr("Fichero PDF (*.pdf)"), this, tr("Exportar a PDF"),
                                                  tr("Exportar a PDF"));

  if (fileName.isEmpty())
    return;

  if (!fileName.upper().contains(".PDF"))
    fileName += ".pdf";

  if (QFile::exists(fileName)
      && QMessageBox::question(this, tr("Sobreescribir %1").arg(fileName),
                               tr("Ya existe un fichero llamado %1. ¿ Desea sobreescribirlo ?").arg(fileName), tr("&Sí"),
                               tr("&No"), QString::null, 0, 1))
    return;

  slotPrintReportToPDF(fileName);
}

void FLReportViewer::sendEMailPDF()
{
  QString name = leDocumento->text().isEmpty() ? "informe.pdf" : leDocumento->text();
  QString fileName = QFileDialog::getSaveFileName(AQ_USRHOME + "/" + name + ".pdf",
                                                  tr("Fichero PDF a enviar(*.pdf)"), this,
                                                  tr("Exportar a PDF par enviar"), tr("Exportar a PDF para enviar"));

  if (fileName.isEmpty())
    return;
  if (!fileName.upper().contains(".PDF"))
    fileName += ".pdf";
  if (QFile::exists(fileName)
      && QMessageBox::question(this, tr("Sobreescribir %1").arg(fileName),
                               tr("Ya existe un fichero llamado %1. ¿ Desea sobreescribirlo ?").arg(fileName), tr("&Sí"),
                               tr("&No"), QString::null, 0, 1)) {
    return;
  }

  bool autoCloseSave = autoClose_;

  autoClose_ = false;
  slotPrintReportToPDF(fileName);
  autoClose_ = autoCloseSave;

  FLUtil::writeSettingEntry("email/to", lePara->text());
  FLUtil::writeSettingEntry("email/from", leDe->text());
  FLUtil::writeSettingEntry("email/mailserver", leMailServer->text());

  QFileInfo fi(fileName);

  name = fi.fileName();

  smtpClient_->setMailServer(leMailServer->text());
  smtpClient_->setTo(lePara->text());
  smtpClient_->setFrom(leDe->text());
  smtpClient_->setSubject(leAsunto->text().isEmpty() ? name : leAsunto->text());
  smtpClient_->setBody(leCuerpo->text() + "\n\n");

  QString html = "<html><body><a href=\"http://abanq.org/\">";
  html += "<img src=\"cid:logo.png@3d8b627b6292\"/></a><br/><br/></body></html>";
  smtpClient_->addTextPart(html, "text/html");
  smtpClient_->addAttachment(fileName);
  smtpClient_->startSend();
}

void FLReportViewer::showInitCentralWidget(bool show)
{
  if (show) {
    rptViewer_->hide();
    setCentralWidget(initCentralWidget_);
    leDocumento->setText("doc-" + QDateTime::currentDateTime().toString().replace(":", "").replace(" ", ""));
    frEMail->show();
    initCentralWidget_->show();
  } else {
    initCentralWidget_->hide();
    frEMail->hide();
    setCentralWidget(rptViewer_);
    rptViewer_->show();
  }
}

void FLReportViewer::saveSVGStyle()
{
  if (report) {
    QString fileName = QFileDialog::getSaveFileName("", tr("Fichero SVG (*.svg)"), this, tr("Guardar en SVG"),
                                                    tr("Guardar en SVG"));
    if (fileName.isEmpty())
      return;

    if (!fileName.upper().contains(".SVG"))
      fileName += ".svg";
    if (QFile::exists(fileName)
        && QMessageBox::question(this, tr("Sobreescribir %1").arg(fileName),
                                 tr("Ya existe un fichero llamado %1. ¿ Desea sobreescribirlo ?").arg(fileName),
                                 tr("&Sí"), tr("&No"), QString::null, 0, 1))
      return;

    FLStylePainter::setSVGMode(true);
    updateReport();
    FLStylePainter::setSVGMode(false);

    QStringList fileNames;
    QString fname;
    QPicture *page;
    QSize psize;
    for (int i = 0; i < report->pageCount(); ++i) {
      fname = fileName + QString::number(i);
      fileNames.append(fname);
      page = report->getPageAt(i);
      psize = report->pageDimensions();
      page->setBoundingRect(QRect(QPoint(0, 0), psize));
      page->save(fname, "svg");
    }

    FLStylePainter::normalizeSVGFile(fileName, fileNames);

    updateReport();
  }
}

void FLReportViewer::saveSimpleSVGStyle()
{
  QString backStyleName = styleName_;
  styleName_ = "_simple";
  //styleName_ = "_mark";
  saveSVGStyle();
  styleName_ = backStyleName;
  updateReport();
}

void FLReportViewer::loadSVGStyle()
{
  QString fileName = QFileDialog::getOpenFileName("", tr("Fichero SVG (*.svg)"), this, tr("Cargar estilo SVG"),
                                                  tr("Cargar estilo SVG"));
  if (fileName.isEmpty())
    return;
  ledStyle->setText("file:" + fileName);
  updateReport();
}

void FLReportViewer::slotExit()
{
  QWidget::close();
}

void FLReportViewer::slotPrintReportToPS(const QString &outPsFile)
{
  if (slotsPrintDisabled_)
    return;
  setDisabled(true);
  printing_ = true;
  reportPrinted_ = rptViewer_->printReportToPS(outPsFile);
  if (reportPrinted_ && autoClose_)
    QTimer::singleShot(0, this, SLOT(slotExit()));
  printing_ = false;
  setDisabled(false);
}

void FLReportViewer::slotPrintReportToPDF(const QString &outPdfFile)
{
  if (slotsPrintDisabled_)
    return;
  setDisabled(true);
  printing_ = true;
  reportPrinted_ = rptViewer_->printReportToPDF(outPdfFile);
  if (reportPrinted_ && autoClose_)
    QTimer::singleShot(0, this, SLOT(slotExit()));
  printing_ = false;
  setDisabled(false);
}

void FLReportViewer::slotPrintReport()
{
  if (slotsPrintDisabled_)
    return;
  setDisabled(true);
  printing_ = true;
  reportPrinted_ = rptViewer_->printReport();
  if (reportPrinted_ && autoClose_)
    QTimer::singleShot(0, this, SLOT(slotExit()));
  printing_ = false;
  setDisabled(false);
}

bool FLReportViewer::setReportData(FLSqlQuery *q)
{
  qry_ = q;
  if (rptEngine_ && rptEngine_->setReportData(q)) {
    xmlData_ = *rptEngine_->rptXmlData();
    return true;
  }
  return false;
}

bool FLReportViewer::setReportData(FLSqlCursor *t)
{
  return (rptEngine_ ? rptEngine_->setReportData(t) : false);
}

bool FLReportViewer::setReportData(QDomNode d)
{
  xmlData_ = d;
  qry_ = 0;
  return (rptEngine_ ? rptEngine_->setFLReportData(d) : false);
}

bool FLReportViewer::setReportTemplate(const QString &t, const QString &style)
{
  template_ = t;
  styleName_ = style;
  if (rptEngine_ && rptEngine_->setFLReportTemplate(t)) {
    rptEngine_->setStyleName(style);
    xmlTemplate_ = *rptEngine_->rptXmlTemplate();
    return true;

  }
  return false;
}

bool FLReportViewer::setReportTemplate(QDomNode d, const QString &style)
{
  xmlTemplate_ = d;
  template_ = QString::null;
  styleName_ = style;
  if (!rptEngine_)
    return false;
  rptEngine_->setFLReportTemplate(d);
  rptEngine_->setStyleName(style);
  return true;
}

QSize FLReportViewer::sizeHint() const
{
  return rptViewer_->sizeHint();
}

void FLReportViewer::setNumCopies(const int numCopies)
{
  rptViewer_->setNumCopies(numCopies);
}

void FLReportViewer::setPrintToPos(bool ptp)
{
  rptViewer_->setPrintToPos(ptp);
}

void FLReportViewer::setPrinterName(const QString &pName)
{
  rptViewer_->setPrinterName(pName);
}

bool FLReportViewer::reportPrinted()
{
  return reportPrinted_;
}

void FLReportViewer::setAutoClose(bool b)
{
  if (embedInParent_)
    autoClose_ = false;
  else
    autoClose_ = b;
}

void FLReportViewer::setResolution(int dpi)
{
  QString valn;
  FLUtil::writeSettingEntry("rptViewer/dpi", valn.setNum(dpi));
  rptViewer_->setResolution(dpi);
}

void FLReportViewer::setPixel(int relDpi)
{
  QString valn;
  FLUtil::writeSettingEntry("rptViewer/pixel", valn.setNum((double)(relDpi / 10.), 'f', 2));
  if (rptEngine_)
    rptEngine_->setRelDpi(relDpi / 10.);
}

void FLReportViewer::setDefaults()
{
  spnResolution->setValue(300);
#if defined (Q_OS_LINUX)
  spnPixel->setValue(780);
#else
#if defined(Q_OS_WIN32)
  spnPixel->setValue(780);
#else
  spnPixel->setValue(800);
#endif
#endif
}

void FLReportViewer::updateReport()
{
  emit requestUpdateReport();

  if (qry_ || !xmlData_.isNull()) {
    if (!rptEngine_)
      setReportEngine(new FLReportEngine(this));

    setResolution(spnResolution->value());
    setPixel(spnPixel->value());

    if (!template_.isEmpty())
      setReportTemplate(template_, styleName_);
    else
      setReportTemplate(xmlTemplate_, styleName_);
    if (qry_)
      setReportData(qry_);
    else
      setReportData(xmlData_);
    renderReport(0, 0, false, false);
  }
  updateDisplay();
}

FLPicture *FLReportViewer::getCurrentPage()
{
  if (report)
    return new FLPicture(report->getCurrentPage(), this);
  return 0;
}

FLPicture *FLReportViewer::getFirstPage()
{
  if (report)
    return new FLPicture(report->getFirstPage(), this);
  return 0;
}

FLPicture *FLReportViewer::getPreviousPage()
{
  if (report)
    return new FLPicture(report->getPreviousPage(), this);
  return 0;
}

FLPicture *FLReportViewer::getNextPage()
{
  if (report)
    return new FLPicture(report->getNextPage(), this);
  return 0;
}

FLPicture *FLReportViewer::getLastPage()
{
  if (report)
    return new FLPicture(report->getLastPage(), this);
  return 0;
}

FLPicture *FLReportViewer::getPageAt(uint i)
{
  if (report)
    return new FLPicture(report->getPageAt(i), this);
  return 0;
}

void FLReportViewer::updateDisplay()
{
  rptViewer_->slotUpdateDisplay();
}

void FLReportViewer::clearPages()
{
  if (report)
    report->clear();
}

void FLReportViewer::appendPage()
{
  if (report)
    report->appendPage();
}

int FLReportViewer::getCurrentIndex()
{
  if (report)
    return report->getCurrentIndex();
  return -1;
}

void FLReportViewer::setCurrentPage(int idx)
{
  if (report)
    report->setCurrentPage(idx);
}

void FLReportViewer::setPageSize(int s)
{
  if (report)
    report->setPageSize(s);
}

void FLReportViewer::setPageOrientation(int o)
{
  if (report)
    report->setPageOrientation(o);
}

void FLReportViewer::setPageDimensions(QSize dim)
{
  if (report)
    report->setPageDimensions(dim);
}

int FLReportViewer::pageSize()
{
  if (report)
    return report->pageSize();
  return -1;
}

int FLReportViewer::pageOrientation()
{
  if (report)
    return report->pageOrientation();
  return -1;
}

QSize FLReportViewer::pageDimensions()
{
  if (report)
    return report->pageDimensions();
  return QSize();
}

int FLReportViewer::pageCount()
{
  if (report)
    return report->pageCount();
  return -1;
}

void FLReportViewer::setStyleName(const QString &style)
{
  styleName_ = style;
}

void FLReportViewer::rptViewerEmbedInParent(QWidget *parentFrame)
{
  if (!parentFrame)
    return;

  setCentralWidget(0);
  rptViewer_->reparent(parentFrame, 0, QPoint(0, 0));

  if (!parentFrame->layout()) {
    QVBoxLayout *lay = new QVBoxLayout(parentFrame);
    lay->addWidget(rptViewer_);
  } else
    parentFrame->layout()->add(rptViewer_);

  rptViewer_->show();
}

void FLReportViewer::rptViewerReparent(QWidget *parentFrame)
{
  if (!parentFrame)
    return;

  QAction *actExit = ::qt_cast<QAction *>(child("salir", "QAction"));
  if (actExit)
    actExit->setVisible(false);

  reparent(parentFrame, 0, QPoint(0, 0));

  if (!parentFrame->layout()) {
    QVBoxLayout *lay = new QVBoxLayout(parentFrame);
    lay->addWidget(this);
  } else
    parentFrame->layout()->add(this);

  QWidget::show();
}

void FLReportViewer::setReportPages(FLReportPages *pgs)
{
  setReportEngine(0);
  qry_ = 0;
  xmlData_ = QDomNode();
  rptViewer_->setReportPages(pgs ? pgs->pageCollection() : 0);
  report = rptViewer_->reportPages();
}

void FLReportViewer::setColorMode(uint c)
{
  rptViewer_->setColorMode(c);
}

uint FLReportViewer::colorMode() const
{
  return rptViewer_->colorMode();
}

void FLReportViewer::disableSlotsPrintExports(bool disablePrints, bool disableExports)
{
  slotsPrintDisabled_ = disablePrints;
  slotsExportDisabled_ = disableExports;
}

void FLReportViewer::exportToOds()
{
  if (slotsExportDisabled_)
    return;

  rptViewer_->exportToOds();
}
