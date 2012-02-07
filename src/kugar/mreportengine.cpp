/***************************************************************************
 mreportengine.cpp  -  Kugar report engine
 -------------------
 begin     : Sun Aug 15 1999
 copyright : (C) 1999 by Mutiny Bay Software
 email     : info@mutinybaysoftware.com
 copyright : (C) 2002 Alexander Dymo
 email     : cloudtemple@mksat.net
 copyright : (C) 2002-2004 InfoSiAL S.L.
 email     : mail@infosial.com
 ***************************************************************************/

#include <qfile.h>
#include <qprinter.h>

#include "mreportengine.h"
#include "mreportviewer.h"
#include "mutil.h"

#include "posprinter.h"
#if defined(Q_OS_WIN32)
#include "psprinter.h"
#endif

/** Constructor */
MReportEngine::MReportEngine(QObject *parent) :
  QObject(parent)
{
  // Set page params
  pageSize = MReportEngine::A4;
  pageOrientation = MReportEngine::Portrait;
  topMargin = 0;
  bottomMargin = 0;
  leftMargin = 0;
  rightMargin = 0;
  p = 0;
  printToPos = false;
  currRecord_ = 0;
  fillRecords_ = false;

#if defined (Q_OS_LINUX)
  setRelDpi(78.);
#else
#if defined(Q_OS_WIN32)
  setRelDpi(78.);
#else
  setRelDpi(78.);
#endif
#endif

  // Set cancel flag
  cancelRender = false;

  // Set the grand total and gran detail total lists to AutoDelete
  grandTotal = new QPtrList<QMemArray<double> > ();
  grandTotal->setAutoDelete(true);

  for (int i = 0; i < 10; i++) {
    gDTFooters[i] = new QPtrList<QMemArray<double> > ();
    gDTFooters[i]->setAutoDelete(true);
    gDTSFooters[i] = new QValueVector<QString> ();
  }

  // Set the details lists to AutoDelete
  dHeaders = new QPtrList<MReportSection> ();
  dHeaders->setAutoDelete(true);

  details = new QPtrList<MReportSection> ();
  details->setAutoDelete(true);

  dFooters = new QPtrList<MReportSection> ();
  dFooters->setAutoDelete(true);

  addOnHeaders = new QPtrList<MReportSection> ();
  addOnHeaders->setAutoDelete(true);

  addOnFooters = new QPtrList<MReportSection> ();
  addOnFooters->setAutoDelete(true);

  MReportSection::resetIdSecGlob();

  rHeader = new MReportSection("ReportHeader");
  rHeader->setPrintFrequency(MReportSection::FirstPage);

  pHeader = new MReportSection("PageHeader");
  pHeader->setPrintFrequency(MReportSection::EveryPage);

  pFooter = new MReportSection("PageFooter");
  pFooter->setPrintFrequency(MReportSection::EveryPage);

  rFooter = new MReportSection("ReportFooter");
  rFooter->setPrintFrequency(MReportSection::LastPage);

  // Set the default page metrics
  QSize ps(getPageMetrics(pageSize, pageOrientation));
  pageWidth = ps.width();
  pageHeight = ps.height();
  rd = new QDomDocument("KUGAR_DATA");
  rt = new QDomDocument("KUGAR_TEMPLATE");
  p = new FLStylePainter();
  p->setRelDpi(relCalcDpi_);
}

/** Copy constructor */
MReportEngine::MReportEngine(const MReportEngine &mReportEngine, QObject *parent) :
  QObject(parent)
{
  copy(&mReportEngine);
}

/** Assignment operator */
MReportEngine MReportEngine::operator=(const MReportEngine &mReportEngine)
{
  if (&mReportEngine == this)
    return *this;
  // Copy the derived class's data
  copy(&mReportEngine);
  return *this;
}

/** Destructor */
MReportEngine::~MReportEngine()
{
  clear();
  if (p)
    delete p;
}

void MReportEngine::clear()
{
  clearGrantTotals();

  if (grandTotal)
    delete grandTotal;
  for (int i = 0; i < 10; i++) {
    if (gDTFooters[i])
      delete gDTFooters[i];
    if (gDTSFooters[i])
      delete gDTSFooters[i];
  }

  clearFormatting();

  if (addOnHeaders) {
    addOnHeaders->clear();
    delete addOnHeaders;
  }

  if (dHeaders) {
    dHeaders->clear();
    delete dHeaders;
  }

  if (details) {
    details->clear();
    delete details;
  }

  if (dFooters) {
    dFooters->clear();
    delete dFooters;
  }

  if (addOnFooters) {
    addOnFooters->clear();
    delete addOnFooters;
  }

  if (rHeader)
    delete rHeader;

  if (pHeader)
    delete pHeader;

  if (pFooter)
    delete pFooter;

  if (rFooter)
    delete rFooter;

  if (rd)
    delete rd;

  if (rt)
    delete rt;
}

void MReportEngine::clearGrantTotals()
{
  if (grandTotal)
    grandTotal->clear();
  for (int i = 0; i < 10; i++) {
    if (gDTFooters[i])
      gDTFooters[i]->clear();
    if (gDTSFooters[i])
      gDTSFooters[i]->clear();
  }
}

/** Clears report formatting */
void MReportEngine::clearFormatting()
{
  rHeader->clear();
  pHeader->clear();

  MReportSection *secIt;

  for (secIt = addOnHeaders->first(); secIt; secIt = addOnHeaders->next())
    secIt->clear();

  for (secIt = dHeaders->first(); secIt; secIt = dHeaders->next())
    secIt->clear();

  for (secIt = details->first(); secIt; secIt = details->next())
    secIt->clear();

  for (secIt = dFooters->first(); secIt; secIt = dFooters->next())
    secIt->clear();

  for (secIt = addOnFooters->first(); secIt; secIt = addOnFooters->next())
    secIt->clear();

  pFooter->clear();
  rFooter->clear();
}

// Set the report's data from an inline string.  Return true if it was valid
// data.
bool MReportEngine::setReportData(const QString &data)
{
  if (!rd->setContent(data)) {
    qWarning("Unable to parse report data");
    return false;
  }
  initData();
  return true;
}

// Set the report's data from an i/o device.  Return true if it was valid data.
bool MReportEngine::setReportData(QIODevice *dev)
{
  if (!rd->setContent(dev)) {
    qWarning("Unable to parse report data");
    return false;
  }
  initData();
  return true;
}

bool MReportEngine::setReportData(QDomNode &d)
{
  *rd = d.cloneNode(true).toDocument();
  initData();
  return true;
}

// Initialise after reading a report's data.
void MReportEngine::initData()
{
  // Get the record set (we assume there is only one).
  for (QDomNode n = rd->firstChild(); !n.isNull(); n = n.nextSibling()) {
    if (n.nodeName() == "KugarData") {
      // Get the records.
      records = n.childNodes();
      // See if there is a prefered template given.
      QDomNamedNodeMap attr = n.attributes();
      QDomNode tempattr = attr.namedItem("Template");
      QString tempname = tempattr.nodeValue();
      if (!tempname.isNull())
        emit preferedTemplate(tempname);
      break;
    }
  }
}

// Set the report's template from an inline string.  Return true if it was a
// valid template.
bool MReportEngine::setReportTemplate(const QString &tpl)
{
  clearFormatting();
  if (!rt->setContent(tpl)) {
    qWarning("Unable to parse report template");
    return false;
  }
  initTemplate();
  return true;
}

bool MReportEngine::setReportTemplate(QDomNode &d)
{
  clearFormatting();
  *rt = d.cloneNode(true).toDocument();
  initTemplate();
  return true;
}

// Set the report's template from an i/o device.  Return true if it was a valid
// template.
bool MReportEngine::setReportTemplate(QIODevice *dev)
{
  clearFormatting();
  if (!rt->setContent(dev)) {
    qWarning("Unable to parse report template");
    return false;
  }
  initTemplate();
  return true;
}

/** Walks the document tree, setting the report layout */
void MReportEngine::initTemplate()
{
  QDomNode report;
  QDomNode child;
  MReportSection::resetIdSecGlob();

  // Get the report - assume there is only one.
  if (addOnHeaders) {
    addOnHeaders->clear();
  }
  if (dHeaders) {
    dHeaders->clear();
  }
  if (details) {
    details->clear();
  }
  if (addOnFooters) {
    addOnFooters->clear();
  }
  if (dFooters) {
    dFooters->clear();
  }

  for (report = rt->firstChild(); !report.isNull(); report = report.nextSibling())
    if (report.nodeName() == "KugarTemplate")
      break;

  setReportAttributes(&report);

  // Get all the child report elements
  QDomNodeList children = report.childNodes();

  int childCount = children.length();

  for (int j = 0; j < childCount; j++) {
    child = children.item(j);

    if (child.nodeType() == QDomNode::ElementNode) {
      // Report Header
      if (child.nodeName() == "ReportHeader")
        setSectionAttributes(rHeader, &child);

      else if (child.nodeName() == "PageHeader")
        setSectionAttributes(pHeader, &child);

      else if (child.nodeName() == "AddOnHeader") {
        MReportSection *addOnHeader = new MReportSection("AddOnHeader");
        addOnHeaders->append(addOnHeader);
        setDetMiscAttributes(addOnHeader, &child);
        setDetailAttributes(addOnHeader, &child);

      } else if (child.nodeName() == "DetailHeader") {
        MReportSection *dHeader = new MReportSection("DetailHeader");
        dHeaders->append(dHeader);
        setDetMiscAttributes(dHeader, &child);
        setDetailAttributes(dHeader, &child);

      } else if (child.nodeName() == "Detail") {
        MReportSection *detail = new MReportSection("Detail");
        details->append(detail);
        setDetailAttributes(detail, &child);

      } else if (child.nodeName() == "DetailFooter") {
        MReportSection *dFooter = new MReportSection("DetailFooter");
        setDetMiscAttributes(dFooter, &child);
        setDetailAttributes(dFooter, &child);
        dFooters->append(dFooter);

      } else if (child.nodeName() == "AddOnFooter") {
        MReportSection *addOnFooter = new MReportSection("AddOnFooter");
        setDetMiscAttributes(addOnFooter, &child);
        setDetailAttributes(addOnFooter, &child);
        addOnFooters->append(addOnFooter);

      } else if (child.nodeName() == "PageFooter") {
        setSectionAttributes(pFooter, &child);
      } else if (child.nodeName() == "ReportFooter")
        setSectionAttributes(rFooter, &child);
    }
  }
}

/** Cancels rendering of a report */
void MReportEngine::slotCancelRendering()
{
  cancelRender = true;
}

/** Finds the detail header object, which is apropriate for the given level */
MReportSection *MReportEngine::findDetailHeader(int level)
{
  MReportSection *sec;
  for (sec = dHeaders->first(); sec; sec = dHeaders->next()) {
    if (sec->getLevel() == level)
      return sec;
  }
  return 0;
}

/** Finds the add-on detail header object, which is apropriate for the given level */
MReportSection *MReportEngine::findAddOnHeader(int level)
{
  MReportSection *sec;
  for (sec = addOnHeaders->first(); sec; sec = addOnHeaders->next()) {
    if (sec->getLevel() == level)
      return sec;
  }
  return 0;
}

/** Finds the detail object, which is apropriate for the given level */
MReportSection *MReportEngine::findDetail(int level)
{
  MReportSection *sec;
  for (sec = details->first(); sec; sec = details->next()) {
    if (sec->getLevel() == level)
      return sec;
  }
  return 0;
}

/** Finds the detail footer object, which is apropriate for the given level */
MReportSection *MReportEngine::findDetailFooter(int level)
{
  MReportSection *sec;
  for (sec = dFooters->first(); sec; sec = dFooters->next()) {
    if (sec->getLevel() == level)
      return sec;
  }
  return 0;
}

/**
 Finds the detail add-on footer object, which is apropriate for the given level.
 If no footer is found, trys to find a general add-on footer level -1)
 */
MReportSection *MReportEngine::findAddOnFooter(int level)
{
  MReportSection *sec;
  for (sec = addOnFooters->first(); sec; sec = addOnFooters->next()) {
    if (sec->getLevel() == level)
      return sec;
  }
  for (sec = addOnFooters->first(); sec; sec = addOnFooters->next()) {
    if (sec->getLevel() == -1)
      return sec;
  }
  return 0;
}

/** Renders the report as a page collection - the caller
 * is responsible for de-allocating the returned
 * collection
 *
 * copyright : (C) 2002-2008 InfoSiAL S.L.
 * email     : mail@infosial.com
 *
 */
MPageCollection *MReportEngine::renderReport(int initRow, int initCol,
                                             MPageCollection *pages, uint flags)
{
  fillRecords_ = flags & MReportEngine::FillRecords;
  bool pageBreak = flags & MReportEngine::PageBreak;
  bool append = flags & MReportEngine::Append;
  cancelRender = false;
  currRecord_ = 0;
  csvData_ = QString::null;
  p->setStyleName(styleName_);

  // Create the page collection
  QPicture *currentPage = 0;
  QPicture *currentPageCopy = 0;
  bool lastPageFound = false;
  if (pages == 0) {
    pages = new MPageCollection(this);
    currPage = 0;
  } else {
    if (append && !pageBreak) {
      currX = leftMargin ;
      lastPageFound = true;
      currentPage = pages->getLastPage();
      p->painter()->end();
      currentPageCopy = new QPicture(*currentPage);
      p->painter()->begin(currentPage);
      currentPageCopy->play(p->painter());
      delete currentPageCopy;
    }
  }

  // Initialize the basic page data
  currHeight = pageHeight - (bottomMargin + pFooter->getHeight());
  currDate = QDate::currentDate();

  // Initialize the grand total array
  clearGrantTotals();

  for (int i = 0; i < rFooter->getCalcFieldCount(); i++)
    grandTotal->append(new QMemArray<double>);

  // Create the first page
  if (!lastPageFound)
    startPage(pages);
  unsigned int rowCount = records.length();

  if (rowCount <= 1)
    rowCount = 2;

  uint nRecord = 0;

  // Draw the details
  drawDetail(pages, 0, nRecord, initRow, initCol);

  // Finish the last page of the report
  endPage(pages);

  // Destroy the page painter
  p->painter()->end();

  // Set the page collection attributes
  pages->setPageDimensions(QSize(pageWidth, pageHeight));
  pages->setPageSize(pageSize);
  pages->setPageOrientation(pageOrientation);
  pages->setPrintToPos(printToPos);
  pages->setPageMargins(topMargin, leftMargin, bottomMargin, rightMargin);


  fillRecords_ = false;

  // Send final status
  emit signalRenderStatus(rowCount / 2);

  return pages;
}

/** Starts a new page of the report */
void MReportEngine::startPage(MPageCollection *pages, int levelAddOn)
{
  // Set the current y pos
  currY = topMargin;
  currX = leftMargin;

  // Create a new page
  pages->appendPage();
  // Increment the page count
  currPage++;

  // Set the page painter to the current page
  p->painter()->begin(pages->getCurrentPage());

  // Draw the report header
  drawReportHeader(pages);
  // Draw the page header
  drawPageHeader(pages);

  // Draw the add-on headers
  if (currPage > 1 && levelAddOn >= 0) {
    drawAddOnHeader(pages, -1, grandTotal);
    for (int i = 0; i <= levelAddOn; i++)
      drawAddOnHeader(pages, i, gDTFooters[i], gDTSFooters[i]);
  }
}

/** Finishes the current page of the report */
void MReportEngine::endPage(MPageCollection *pages)
{
  // Draw the report footer
  drawReportFooter(pages);
  // Draw the page footer
  drawPageFooter(pages);
}

/** Finishes the current page and adds a new page */
void MReportEngine::newPage(MPageCollection *pages, int levelAddOn)
{
  // Draw the page footer
  drawPageFooter(pages);

  // Stop the painter
  p->painter()->end();

  // Start a new page
  startPage(pages, levelAddOn);
}

/** Draws the report header object to the current page */
void MReportEngine::drawReportHeader(MPageCollection *pages)
{
  if (rHeader->getHeight() == 0)
    return;

  if ((rHeader->printFrequency() == MReportSection::FirstPage && currPage == 1) ||
      (rHeader->printFrequency() == MReportSection::EveryPage)) {
    rHeader->setPageNumber(currPage);
    rHeader->setReportDate(currDate);
    int sectionHeight = rHeader->getHeight();
    rHeader->draw(p, leftMargin, currY, sectionHeight);
    currY += sectionHeight;
  }
}

/** Draws the page header to the current page */
void MReportEngine::drawPageHeader(MPageCollection *pages)
{
  if (pHeader->getHeight() == 0)
    return;

  if ((currY + pHeader->getHeight()) > currHeight)
    newPage(pages);

  if ((pHeader->printFrequency() == MReportSection::FirstPage && currPage == 1) ||
      (pHeader->printFrequency() == MReportSection::EveryPage)) {
    pHeader->setPageNumber(currPage);
    pHeader->setReportDate(currDate);
    int sectionHeight = pHeader->getHeight();
    pHeader->draw(p, leftMargin, currY, sectionHeight);
    currY += sectionHeight;
  }
}

/** Draws the page footer to the current page */
void MReportEngine::drawPageFooter(MPageCollection *pages)
{
  if (pFooter->getHeight() == 0)
    return;

  QString value;
  QDomNode record = records.item(currRecord_);
  QDomNamedNodeMap fields = record.attributes();

  pFooter->setCalcFieldData();

  if ((pFooter->printFrequency() == MReportSection::FirstPage && currPage == 1) ||
      (pFooter->printFrequency() == MReportSection::EveryPage)) {
    pFooter->setPageNumber(currPage);
    pFooter->setReportDate(currDate);
    int sectionHeight = pFooter->getHeight();
    pFooter->draw(p, leftMargin, (pageHeight - bottomMargin) - pFooter->getHeight(),
                  sectionHeight);
    currY += sectionHeight;
  }
}

//copyright : (C) 2002-2004 InfoSiAL S.L.
//email     : mail@infosial.com
void MReportEngine::drawDetail(MPageCollection *pages, int level, uint &currRecord,
                               const int initRow, const int initCol)
{
  currRecord_ = currRecord;

  QString detailValue;
  int nextLevel;
  int detailHeight = 0;
  MReportSection *detail = findDetail(level);
  MReportSection *footer;

  if (detail)
    detailHeight = detail->getHeight();

  if (!canDrawDetailHeader(level, currRecord, currY)) {
    if (level > 0)
      drawAddOnFooter(pages, (level - 1), gDTFooters[(level - 1)], gDTSFooters[(level - 1)]);

    newPage(pages, level);

    // Sólo dibujamos el detail header si no hemos dibujado ya el addOnHeader
    if (!findAddOnHeader(level))
      drawDetailHeader(pages, level);
  } else {
    drawDetailHeader(pages, level);
  }

  if (!detail) {
    drawDetailFooter(pages, level);
    return;
  }

  gDTFooters[level]->clear();
  gDTSFooters[level]->clear();

  int currLevel;
  int chkRow = 0;
  int loops = 0;

  if (initCol != 0)
    currX = leftMargin + (detail->getWidth() * (initCol - 1));

  if (initRow != 0)
    currY = topMargin + (detail->getHeight() * (initRow - 1));

  currLevel = level;

  do {
    QDomNode record = records.item(currRecord);
    if (record.nodeType() == QDomNode::ElementNode) {
      if (currLevel == level) {
        if ((chkRow = (currRecord / 2) % 20) == 0)
          emit signalRenderStatus(currRecord / 2);
        if (cancelRender) {
          MLabelObject lblCancel;
          lblCancel.setFont("Arial", 20, MLabelObject::Bold, false);
          lblCancel.setText(tr("INFORME INCOMPLETO\nCANCELADO POR EL USUARIO"));
          lblCancel.setGeometry(20, pageHeight / 2, 450, 70);
          lblCancel.setHorizontalAlignment(MLabelObject::Center);
          lblCancel.setVerticalAlignment(MLabelObject::Middle);
          lblCancel.draw(p);
          return;
        }

        QDomNamedNodeMap fields = record.attributes();
        reserveSizeForCalcFields(&fields, level);

        detail->setPageNumber(currPage);
        detail->setReportDate(currDate);

        if (!canDrawDetail(level, currRecord, currY)) {
          if (loops) {
            drawAddOnFooter(pages, level, gDTFooters[level], gDTSFooters[level]);
          } else {
            if (level > 0) {
              drawAddOnFooter(pages, (level - 1), gDTFooters[(level - 1)], gDTSFooters[(level - 1)]);
            }
          }
          newPage(pages, level);
        }

        QDomNode record = records.item(currRecord);
        QDomNode *ptrRecord = &record;

        setFieldValues(&fields, level, detail, ptrRecord);

        if (detail->mustBeDrawed(ptrRecord)) {
          detail->setCalcFieldData(0, 0, ptrRecord, fillRecords_);
          int sectionHeight = detail->getHeight();
          detail->draw(p, currX, currY, sectionHeight);
          detailHeight = sectionHeight;

          currX += detail->getWidth();

          if (currX >= (pageWidth - rightMargin - leftMargin)) {
            currX = leftMargin;
            currY += sectionHeight;
          }

          MReportSection *rS = findDetail(level + 1);
          if (!rS) {
            for (uint i = 0; i <= level; i++) {
              rS = findDetailHeader(i);
              if (rS)
                csvData_ += rS->csvData();
              rS = findDetail(i);
              if (rS)
                csvData_ += rS->csvData();
            }
            csvData_ += "\n";
          }
        }

        ++currRecord;
      } else {
        drawDetail(pages, currLevel, currRecord);
      }

      if (currRecord < records.count()) {
        record = records.item(currRecord);
        QDomNamedNodeMap fields = record.attributes();
        detailValue = fields.namedItem("level").nodeValue();
        currLevel = detailValue.toInt();
      }

      if (cancelRender) {
        MLabelObject lblCancel;
        lblCancel.setFont("Arial", 20, MLabelObject::Bold, false);
        lblCancel.setText(tr("INFORME INCOMPLETO\nCANCELADO POR EL USUARIO"));
        lblCancel.setGeometry(20, pageHeight / 2, 450, 70);
        lblCancel.setHorizontalAlignment(MLabelObject::Center);
        lblCancel.setVerticalAlignment(MLabelObject::Middle);
        lblCancel.draw(p);
        return;
      }
    }

    loops++;
  } while (level <= currLevel && currRecord < records.count());

  drawDetailFooter(pages, level, gDTFooters[level], gDTSFooters[level]);

  footer = findDetailFooter(level);
  if (footer && currRecord < records.count())
    if (footer->newPage())
      newPage(pages);
}

bool MReportEngine::canDrawDetailHeader(uint level, uint currRecord, uint yPos)
{
  int headerHeight = 0;
  MReportSection *header = findDetailHeader(level);
  if (header)
    headerHeight = header->getHeight();

  if (!canDrawDetail(level, currRecord, (yPos + headerHeight)))
    return false;

  return true;
}

bool MReportEngine::canDrawDetail(uint level, uint currRecord, uint yPos)
{
  QDomNode nextRecord;
  QString detailValue;
  uint nextLevel;
  QDomNode record = records.item(currRecord);

  if ((currRecord + 1) < records.count()) {
    nextRecord = records.item(currRecord + 1);
    QDomNamedNodeMap nextFields = nextRecord.attributes();

    detailValue = nextFields.namedItem("level").nodeValue();
    nextLevel = detailValue.toInt();
  } else
    nextLevel = 0;

  int detailHeight = 0;

  MReportSection *detail = findDetail(level);

  if (detail) {
    QDomNamedNodeMap fields = record.attributes();
    setFieldValues(&fields, level, detail, &record, true);
    detailHeight = detail->getHeight(p);
  }
  int addOnFooterHeight = 0;

  MReportSection *addOnFooter = findAddOnFooter(level);

  if (addOnFooter)
    addOnFooterHeight = addOnFooter->getHeight();

  // Control de fin de página
  if (level == nextLevel) {
    if ((yPos + detailHeight + addOnFooterHeight) > currHeight) {
      return false;
    }
  } else if (level > nextLevel) {
    // El siguiente nivel es inferior: Se comprueba que es posible incluir un detalle del nivel actual
    // más todas los pies de detalle desde el nivel actual hasta el siguiente nivel,
    // más el addOnFooter del siguiente nivel
    int footersHeight = 0;

    for (int levelFooter = level; levelFooter > nextLevel; levelFooter--) {
      MReportSection *footerAux = findDetailFooter(levelFooter);

      if (footerAux)
        footersHeight += footerAux->getHeight();
    }

    int addOnFooterAuxHeight = 0;
    MReportSection *addOnFooterAux = findAddOnFooter(nextLevel);
    if (addOnFooterAux)
      addOnFooterAuxHeight = addOnFooterAux->getHeight();

    if ((yPos + detailHeight + footersHeight + addOnFooterAuxHeight) > currHeight) {

      return false;
    }
  } else if (level < nextLevel) {
    // El siguiente nivel es superior: Se comprueba que es posible incluir un detalle del nivel actual
    // más todas las cabeceras de detalle desde el nivel actual hasta el siguiente nivel
    // más un detalle del siguiente nivel más el addOnFooter del siguiente nivel
    int headersHeight = 0;

    for (int levelFooter = (level + 1); levelFooter <= nextLevel; levelFooter++) {
      MReportSection *headerAux = findDetailHeader(levelFooter);

      if (headerAux)
        headersHeight += headerAux->getHeight();
    }

    MReportSection *detailAux = findDetail(nextLevel);

    if (detailAux)
      headersHeight += detailAux->getHeight();

    int addOnFooterAuxHeight = 0;
    MReportSection *addOnFooterAux = findAddOnFooter(nextLevel);
    if (addOnFooterAux)
      addOnFooterAuxHeight = addOnFooterAux->getHeight();

    if ((currY + detailHeight + headersHeight + addOnFooterAuxHeight) > currHeight) {
      return false;
    }
  }

  return true;
}

void MReportEngine::reserveSizeForCalcFields(QDomNamedNodeMap *fields, int level)
{
  for (int l = level; l >= 0; l--) {
    MReportSection *footer = findDetailFooter(l);

    if (footer) {
      gDTSFooters[l]->reserve(footer->getCalcFieldCount());

      for (int i = 0; i < footer->getCalcFieldCount(); i++)
        gDTFooters[l]->append(new QMemArray<double>);
    }

    if (footer) {
      int calcIdx;

      for (int i = 0; i < fields->count(); i++) {
        calcIdx = footer->getCalcFieldIndex(fields->item(i).nodeName());

        if (calcIdx != -1)
          (*gDTSFooters[l])[calcIdx] = fields->item(i).nodeValue();
      }
    }
  }
}

void MReportEngine::setFieldValues(QDomNamedNodeMap *fields, int level,
                                   MReportSection *detail, QDomNode *ptrRecord,
                                   bool noTotal)
{
  QString detailValue;
  int calcIdx;
  int vsize;

  for (int i = 0; i < detail->getFieldCount(); i++) {
    detailValue = fields->namedItem(detail->getFieldName(i)).nodeValue();
    detail->setFieldData(i, detailValue, ptrRecord, fillRecords_);

    if (noTotal)
      continue;

    calcIdx = rFooter->getCalcFieldIndex(detail->getFieldName(i));
    if (calcIdx != -1) {
      vsize = grandTotal->at(calcIdx)->size();
      grandTotal->at(calcIdx)->resize(vsize + 1);
      grandTotal->at(calcIdx)->at(vsize) = detailValue.toDouble();
    }

    for (int l = level; l >= 0; l--) {
      MReportSection *footer = findDetailFooter(l);
      if (!footer)
        continue;
      calcIdx = footer->getCalcFieldIndex(detail->getFieldName(i));
      if (calcIdx == -1)
        continue;
      vsize = gDTFooters[l]->at(calcIdx)->size();
      gDTFooters[l]->at(calcIdx)->resize(vsize + 1);
      gDTFooters[l]->at(calcIdx)->at(vsize) = detailValue.toDouble();
    }
  }
}

//copyright : (C) 2002-2004 InfoSiAL S.L.
//email     : mail@infosial.com
void MReportEngine::drawDetailFooter(MPageCollection *pages, int level,
                                     QPtrList<QMemArray<double> > *gDT,
                                     QValueVector<QString> *gDTS)
{
  MReportSection *footer = findDetailFooter(level);
  MReportSection *header = findDetailHeader(level);

  if (footer) {
    QDomNode record = records.item(currRecord_);

    if (!footer->mustBeDrawed(&record))
      return;

    footer->setPageNumber(currPage);
    footer->setReportDate(currDate);

    if ((currY + footer->getHeight()) > currHeight) {
      newPage(pages);
      for (int i = 0; i <= level; i++) {
        drawAddOnHeader(pages, i, gDTFooters[i], gDTSFooters[i]);
      }
    }

    if (gDT)
      footer->setCalcFieldData(gDT, gDTS, &record, fillRecords_);
    if (header)
      footer->drawHeaderObjects(p, pages, header);

    int sectionHeight = footer->getHeight();
    if (footer->placeAtBottom()) {
      footer->draw(p, leftMargin, (pageHeight - bottomMargin - pFooter->getHeight()) - footer->getHeight(),
                   sectionHeight);
    } else
      footer->draw(p, leftMargin, currY, sectionHeight);
    currY += sectionHeight;
  }
}

//copyright : (C) 2002-2004 InfoSiAL S.L.
//email     : mail@infosial.com
void MReportEngine::drawDetailHeader(MPageCollection *pages, int level)
{
  MReportSection *header = findDetailHeader(level);

  if (header) {
    QDomNode record = records.item(currRecord_);

    if (!header->mustBeDrawed(&record))
      return;

    header->setPageNumber(currPage);
    header->setReportDate(currDate);

    if ((currY + header->getHeight()) > currHeight)
      newPage(pages);

    QString value;
    QDomNamedNodeMap fields = record.attributes();

    for (int i = 0; i < header->getFieldCount(); i++) {
      value = fields.namedItem(header->getFieldName(i)).nodeValue();
      header->setFieldData(i, value, &record, fillRecords_);
    }

    header->setCalcFieldData(0, 0, &record, fillRecords_);

    int sectionHeight = header->getHeight();
    header->draw(p, leftMargin, currY, sectionHeight);
    header->setLastPageIndex(pages->getCurrentIndex());
    header->setOnPage((QPicture *) p->painter()->device());
    currY += sectionHeight;
  }
}

//copyright : (C) 2002-2004 InfoSiAL S.L.
//email     : mail@infosial.com
void MReportEngine::drawAddOnHeader(MPageCollection *pages, int level,
                                    QPtrList<QMemArray<double> > *gDT,
                                    QValueVector<QString> *gDTS)
{
  MReportSection *header = findAddOnHeader(level);

  if (header) {
    QDomNode record = records.item(currRecord_);

    if (!header->mustBeDrawed(&record))
      return;

    header->setPageNumber(currPage);
    header->setReportDate(currDate);

    if ((currY + header->getHeight()) > currHeight)
      newPage(pages);

    QString value;
    QDomNamedNodeMap fields = record.attributes();

    for (int i = 0; i < header->getFieldCount(); i++) {
      value = fields.namedItem(header->getFieldName(i)).nodeValue();
      header->setFieldData(i, value, &record, fillRecords_);
    }

    if (gDT && level > -1)
      header->setCalcFieldData(gDT, gDTS, &record, fillRecords_);
    header->setCalcFieldDataGT(grandTotal);

    int sectionHeight = header->getHeight();
    header->draw(p, leftMargin, currY, sectionHeight);
    currY += sectionHeight;
  }
}

// Draws de add-on footer
void MReportEngine::drawAddOnFooter(MPageCollection *pages, int level,
                                    QPtrList<QMemArray<double> > *gDT,
                                    QValueVector<QString> *gDTS)
{
  MReportSection *footer = findAddOnFooter(level);

  if (footer) {
    QDomNode record = records.item(currRecord_);

    if (!footer->mustBeDrawed(&record))
      return;

    footer->setPageNumber(currPage);
    footer->setReportDate(currDate);

    QString value;
    QDomNamedNodeMap fields = record.attributes();

    for (int i = 0; i < footer->getFieldCount(); i++) {
      value = fields.namedItem(footer->getFieldName(i)).nodeValue();
      footer->setFieldData(i, value, &record, fillRecords_);
    }

    if (gDT && level > -1)
      footer->setCalcFieldData(gDT, gDTS, &record, fillRecords_);
    footer->setCalcFieldDataGT(grandTotal);

    int sectionHeight = footer->getHeight();
    if (footer->placeAtBottom()) {
      footer->draw(p, leftMargin, (pageHeight - bottomMargin - pFooter->getHeight()) - footer->getHeight(),
                   sectionHeight);
    } else
      footer->draw(p, leftMargin, currY, sectionHeight);
    currY += sectionHeight;
  }
}

/** Draws the report footer to the current page */
void MReportEngine::drawReportFooter(MPageCollection *pages)
{
  if (rFooter->getHeight() == 0)
    return;

  if ((currY + rFooter->getHeight()) > currHeight)
    newPage(pages);

  if ((rFooter->printFrequency() == MReportSection::EveryPage) ||
      (rFooter->printFrequency() == MReportSection::LastPage)) {
    rFooter->setCalcFieldData(grandTotal);

    rFooter->setPageNumber(currPage);
    rFooter->setReportDate(currDate);
    int sectionHeight = rFooter->getHeight();
    rFooter->draw(p, leftMargin, currY, sectionHeight);
    currY += sectionHeight;
  }
}

/** Gets the metrics for the selected page size & orientation */
QSize MReportEngine::getPageMetrics(int size, int orientation)
{
  QSize ps;
  // Set the page size
  if ((QPrinter::PageSize) size >= QPrinter::Custom) {
    size = QPrinter::Custom;
    ps.setWidth(customWidthMM / 25.4 * 78.);
    ps.setHeight(customHeightMM / 25.4 * 78.);
    return ps;
  }

#if defined(Q_OS_WIN32)
  if (!printToPos) {
    PSPrinter *printer = new PSPrinter(PSPrinter::HighResolution);
    printer->setFullPage(true);
    printer->setPageSize((PSPrinter::PageSize) size);
    printer->setOrientation((PSPrinter::Orientation) orientation);
    QPaintDeviceMetrics pdm(printer);
    ps.setWidth(pdm.widthMM() / 25.4 * 78.);
    ps.setHeight(pdm.heightMM() / 25.4 * 78.);
    delete printer;
  } else {
    FLPosPrinter *printer = new FLPosPrinter();
    QPaintDeviceMetrics pdm(printer);
    ps.setWidth(pdm.widthMM() / 25.4 * 78.);
    ps.setHeight(pdm.heightMM() / 25.4 * 78.);
    delete printer;
  }

#else
  if (!printToPos) {
    QPrinter *printer = new QPrinter(QPrinter::HighResolution);
    printer->setFullPage(true);
    printer->setPageSize((QPrinter::PageSize) size);
    printer->setOrientation((QPrinter::Orientation) orientation);
    QPaintDeviceMetrics pdm(printer);
    ps.setWidth(pdm.widthMM() / 25.4 * 78.);
    ps.setHeight(pdm.heightMM() / 25.4 * 78.);
    delete printer;
  } else {
    FLPosPrinter *printer = new FLPosPrinter();
    QPaintDeviceMetrics pdm(printer);
    ps.setWidth(pdm.widthMM() / 25.4 * 78.);
    ps.setHeight(pdm.heightMM() / 25.4 * 78.);
    delete printer;
  }

#endif

  return ps;
}

/** Sets the main layout attributes for the report */
void MReportEngine::setReportAttributes(QDomNode *report)
{
  // Get the attributes for the report
  QDomNamedNodeMap attributes = report->attributes();

  pageSize = attributes.namedItem("PageSize").nodeValue().toInt();
  if ((QPrinter::PageSize) pageSize > QPrinter::Custom)
    pageSize = QPrinter::Custom;
  pageOrientation = attributes.namedItem("PageOrientation").nodeValue().toInt();
  topMargin = attributes.namedItem("TopMargin").nodeValue().toInt();
  bottomMargin = attributes.namedItem("BottomMargin").nodeValue().toInt();
  leftMargin = attributes.namedItem("LeftMargin").nodeValue().toInt();
  rightMargin = attributes.namedItem("RightMargin").nodeValue().toInt();
  styleName_ = attributes.namedItem("StyleName").nodeValue();

  if (!attributes.namedItem("CustomWidthMM").isNull())
    customWidthMM = attributes.namedItem("CustomWidthMM").nodeValue().toInt();

  if (!attributes.namedItem("CustomHeightMM").isNull())
    customHeightMM = attributes.namedItem("CustomHeightMM").nodeValue().toInt();

  if (!attributes.namedItem("PrintToPos").isNull())
    printToPos = (attributes.namedItem("PrintToPos").nodeValue().upper() == "TRUE");

  // Set the page metrics
  QSize ps(getPageMetrics(pageSize, pageOrientation));
  pageWidth = ps.width();
  pageHeight = ps.height();
}

/** Sets the layout attributes for the given report section */
void MReportEngine::setSectionAttributes(MReportSection *section, QDomNode *report)
{
  // Get the attributes for the section
  QDomNamedNodeMap attributes = report->attributes();

  // Get the section attributes
  section->setHeight(attributes.namedItem("Height").nodeValue().toInt());
  section->setPrintFrequency(attributes.namedItem("PrintFrequency").nodeValue().toInt());
  if (attributes.contains("SectionId"))
    section->setIdSec(attributes.namedItem("SectionId").nodeValue().toUInt());

  // Process the sections labels
  QDomNodeList children = report->childNodes();
  int childCount = children.length();

  // For each label, extract the attr list and add the new label
  // to the sections's label collection

  for (int j = 0; j < childCount; j++) {
    QDomNode child = children.item(j);

    if (child.nodeType() == QDomNode::ElementNode) {
      if (child.nodeName() == "Line") {
        QDomNamedNodeMap attributes = child.attributes();
        MLineObject *line = new MLineObject();

        setLineAttributes(line, &attributes);
        section->addLine(line);
      } else if (child.nodeName() == "Label") {
        QDomNamedNodeMap attributes = child.attributes();
        MLabelObject *label = new MLabelObject();

        setLabelAttributes(label, &attributes);
        section->addLabel(label);
      } else if (child.nodeName() == "Special") {
        QDomNamedNodeMap attributes = child.attributes();
        MSpecialObject *field = new MSpecialObject();

        setSpecialAttributes(field, &attributes);
        section->addSpecialField(field);
      } else if (child.nodeName() == "CalculatedField") {
        QDomNamedNodeMap attributes = child.attributes();
        MCalcObject *field = new MCalcObject();

        setCalculatedFieldAttributes(field, &attributes);
        section->addCalculatedField(field);
      }
    }
  }
}

/** Sets the layout attributes for the detail headers and footers */
void MReportEngine::setDetMiscAttributes(MReportSection *section, QDomNode *report)
{
  // Get the attributes for the section
  QDomNamedNodeMap attributes = report->attributes();

  // Get the section attributes
  section->setDrawIf(attributes.namedItem("DrawIf").nodeValue());

  if (attributes.contains("SectionId"))
    section->setIdSec(attributes.namedItem("SectionId").nodeValue().toUInt());

  QDomNode levelNode = attributes.namedItem("Level");

  if (!levelNode.isNull())
    section->setLevel(attributes.namedItem("Level").nodeValue().toInt());
  else
    section->setLevel(-1);

  QDomNode n = attributes.namedItem("NewPage");

  if (!n.isNull())
    section->setNewPage(n.nodeValue().upper() == "TRUE");
  else
    section->setNewPage(false);

  n = attributes.namedItem("PlaceAtBottom");

  if (!n.isNull())
    section->setPlaceAtBottom(n.nodeValue().upper() == "TRUE");
  else
    section->setPlaceAtBottom(false);

  n = attributes.namedItem("DrawAllPages");

  if (!n.isNull())
    section->setDrawAllPages(n.nodeValue().upper() == "TRUE");
  else
    section->setDrawAllPages(false);
}

/** Sets the layout attributes for the detail section */
void MReportEngine::setDetailAttributes(MReportSection *section, QDomNode *report)
{
  // Get the attributes for the detail section
  QDomNamedNodeMap attributes = report->attributes();

  section->setHeight(attributes.namedItem("Height").nodeValue().toInt());

  if (attributes.contains("SectionId"))
    section->setIdSec(attributes.namedItem("SectionId").nodeValue().toUInt());

  QDomNode levelNode = attributes.namedItem("Level");
  if (!levelNode.isNull())
    section->setLevel(attributes.namedItem("Level").nodeValue().toInt());
  else
    section->setLevel(-1);

  section->setDrawIf(attributes.namedItem("DrawIf").nodeValue());

  QString cols = attributes.namedItem("Cols").nodeValue();

  if (!cols)
    cols = "1";

  int width = ceil((pageWidth - rightMargin - leftMargin) / cols.toFloat());
  section->setWidth(width);

  // Process the report detail labels
  QDomNodeList children = report->childNodes();

  int childCount = children.length();

  for (int j = 0; j < childCount; j++) {
    QDomNode child = children.item(j);

    if (child.nodeType() == QDomNode::ElementNode) {
      if (child.nodeName() == "Line") {
        QDomNamedNodeMap attributes = child.attributes();
        MLineObject *line = new MLineObject();

        setLineAttributes(line, &attributes);
        section->addLine(line);
      } else if (child.nodeName() == "Label") {
        QDomNamedNodeMap attributes = child.attributes();
        MLabelObject *label = new MLabelObject();

        setLabelAttributes(label, &attributes);
        section->addLabel(label);
      } else if (child.nodeName() == "Special") {
        QDomNamedNodeMap attributes = child.attributes();
        MSpecialObject *field = new MSpecialObject();

        setSpecialAttributes(field, &attributes);
        section->addSpecialField(field);
      } else if (child.nodeName() == "CalculatedField") {
        QDomNamedNodeMap attributes = child.attributes();
        MCalcObject *field = new MCalcObject();

        setCalculatedFieldAttributes(field, &attributes);
        section->addCalculatedField(field);
      } else if (child.nodeName() == "Field") {
        QDomNamedNodeMap attributes = child.attributes();
        MFieldObject *field = new MFieldObject();

        setFieldAttributes(field, &attributes);
        section->addField(field);
      }
    }
  }
}

/** Sets a line's layout attributes */
void MReportEngine::setLineAttributes(MLineObject *line, QDomNamedNodeMap *attr)
{
  line->setLine(attr->namedItem("X1").nodeValue().toInt(), attr->namedItem("Y1").nodeValue().toInt(),
                attr->namedItem("X2").nodeValue().toInt(), attr->namedItem("Y2").nodeValue().toInt());

  QString tmp = attr->namedItem("Color").nodeValue();

  line->setColor(tmp.left(tmp.find(",")).toInt(),
                 tmp.mid(tmp.find(",") + 1,
                         (tmp.findRev(",") - tmp.find(",")) - 1).toInt(),
                 tmp.right(tmp.length() - tmp.findRev(",") - 1).toInt());

  line->setWidth(attr->namedItem("Width").nodeValue().toInt());
  line->setStyle(attr->namedItem("Style").nodeValue().toInt());
  if (attr->contains("ObjectId"))
    line->setObjectId(attr->namedItem("ObjectId").nodeValue().toInt());
}

/** Sets a label's layout attributes */
void MReportEngine::setLabelAttributes(MLabelObject *label, QDomNamedNodeMap *attr)
{
  QString tmp;

  label->setPaintFunction(attr->namedItem("PaintFunction").nodeValue());
  label->setLabelFunction(attr->namedItem("LabelFunction").nodeValue());
  label->setText(attr->namedItem("Text").nodeValue().stripWhiteSpace());
  label->setGeometry(attr->namedItem("X").nodeValue().toInt(), attr->namedItem("Y").nodeValue().toInt(),
                     attr->namedItem("Width").nodeValue().toInt(), attr->namedItem("Height").nodeValue().toInt());

  tmp = attr->namedItem("BackgroundColor").nodeValue();

  if (tmp.upper() == "NOCOLOR") {
    label->setTransparent(true);
    label->setBackgroundColor(255, 255, 255);
  } else {
    label->setTransparent(false);
    label->setBackgroundColor(tmp.left(tmp.find(",")).toInt(),
                              tmp.mid(tmp.find(",") + 1,
                                      (tmp.findRev(",") - tmp.find(",")) - 1).toInt(),
                              tmp.right(tmp.length() - tmp.findRev(",") - 1).toInt());
  }

  tmp = attr->namedItem("ForegroundColor").nodeValue();

  label->setForegroundColor(tmp.left(tmp.find(",")).toInt(),
                            tmp.mid(tmp.find(",") + 1,
                                    (tmp.findRev(",") - tmp.find(",")) - 1).toInt(),
                            tmp.right(tmp.length() - tmp.findRev(",") - 1).toInt());

  tmp = attr->namedItem("BorderColor").nodeValue();
  label->setBorderColor(tmp.left(tmp.find(",")).toInt(),
                        tmp.mid(tmp.find(",") + 1,
                                (tmp.findRev(",") - tmp.find(",")) - 1).toInt(),
                        tmp.right(tmp.length() - tmp.findRev(",") - 1).toInt());

  label->setBorderWidth(attr->namedItem("BorderWidth").nodeValue().toInt());
  label->setBorderStyle(attr->namedItem("BorderStyle").nodeValue().toInt());
  label->setFont(attr->namedItem("FontFamily").nodeValue(),
                 attr->namedItem("FontSize").nodeValue().toFloat() * relCalcDpi_,
                 attr->namedItem("FontWeight").nodeValue().toInt(),
                 (attr->namedItem("FontItalic").nodeValue().toInt() == 0 ? false : true));
  label->setHorizontalAlignment(attr->namedItem("HAlignment").nodeValue(). toInt());
  label->setVerticalAlignment(attr->namedItem("VAlignment").nodeValue(). toInt());
  label->setWordWrap(attr->namedItem("WordWrap").nodeValue().toInt() == 0 ? false : true);
  label->setChangeHeight(attr->namedItem("ChangeHeight").nodeValue().toInt() == 0 ? false : true);
  label->setDrawAtBottom(attr->namedItem("DrawAtBottom").nodeValue().toInt() == 0 ? false : true);
  label->setAdjustFontSize(attr->namedItem("AdjustFontSize").nodeValue().toInt() == 1 ? true : false);
  if (attr->contains("ObjectId"))
    label->setObjectId(attr->namedItem("ObjectId").nodeValue().toInt());
}

/** Sets a special field's layout attributes */
void MReportEngine::setSpecialAttributes(MSpecialObject *field, QDomNamedNodeMap *attr)
{
  field->setType(attr->namedItem("Type").nodeValue().toInt());
  field->setDateFormat(attr->namedItem("DateFormat").nodeValue().toInt());

  setLabelAttributes((MLabelObject *) field, attr);
}

/** Sets a field's layout attributes */
void MReportEngine::setFieldAttributes(MFieldObject *field, QDomNamedNodeMap *attr)
{
  field->setFieldName(attr->namedItem("Field").nodeValue());
  field->setDataType(attr->namedItem("DataType").nodeValue().toInt());
  field->setDateFormat(attr->namedItem("DateFormat").nodeValue().toInt());
  field->setPrecision(attr->namedItem("Precision").nodeValue().toInt());
  field->setCurrency(QChar(attr->namedItem("Currency").nodeValue().toUInt()));
  field->setCommaSeparator(attr->namedItem("CommaSeparator").nodeValue(). toInt());
  field->setCodBarType(attr->namedItem("CodBarType").nodeValue());
  int res = attr->namedItem("CodBarRes").nodeValue().toInt();
  field->setCodBarRes(res > 0 ? res : 72);
  field->setBlankZero(attr->namedItem("BlankZero").nodeValue().toInt());

  QString tmp = attr->namedItem("NegValueColor").nodeValue();

  field->setNegValueColor(tmp.left(tmp.find(",")).toInt(),
                          tmp.mid(tmp.find(",") + 1,
                                  (tmp.findRev(",") - tmp.find(",")) - 1).toInt(),
                          tmp.right(tmp.length() - tmp.findRev(",") - 1).toInt());

  setLabelAttributes((MLabelObject *) field, attr);
}

/** Sets a calculated field's layout attributes */
void MReportEngine::setCalculatedFieldAttributes(MCalcObject *field, QDomNamedNodeMap *attr)
{
  field->setCalculationType(attr->namedItem("CalculationType").nodeValue(). toInt());
  field->setCalculationFunction(attr->namedItem("FunctionName").nodeValue());
  setFieldAttributes((MFieldObject *) field, attr);

  field->setDrawAtHeader(attr->namedItem("DrawAtHeader").nodeValue());
  field->setFromGrandTotal(attr->namedItem("FromGrandTotal").nodeValue());
  setFieldAttributes((MFieldObject *) field, attr);
}

void MReportEngine::setRelDpi(float relDpi)
{
  relDpi_ = relDpi;
#if defined (Q_OS_LINUX)
  QPaintDeviceMetrics pdm(QApplication::desktop());
  if (pdm.logicalDpiX() < pdm.logicalDpiY())
    relCalcDpi_ = relDpi_ / pdm.logicalDpiY();
  else
    relCalcDpi_ = relDpi_ / pdm.logicalDpiX();
#else
#if defined(Q_OS_WIN32)
  QPaintDeviceMetrics pdm(QApplication::desktop());
  relCalcDpi_ = relDpi_ / pdm.logicalDpiY();
#else
  QPaintDeviceMetrics pdm(QApplication::desktop());
  if (pdm.logicalDpiX() < pdm.logicalDpiY())
    relCalcDpi_ = relDpi_ / pdm.logicalDpiY();
  else
    relCalcDpi_ = relDpi_ / pdm.logicalDpiX();
#endif
#endif
  if (p)
    p->setRelDpi(relCalcDpi_);
}

/** Copies member data from one object to another.
 Used by the copy constructor and assignment operator */
void MReportEngine::copy(const MReportEngine *mReportEngine)
{
  clear();

  // Copy document data
  rd = mReportEngine->rd;
  rt = mReportEngine->rt;

  // Copy page metrics
  pageSize = mReportEngine->pageSize;
  pageOrientation = mReportEngine->pageOrientation;
  topMargin = mReportEngine->topMargin;
  bottomMargin = mReportEngine->bottomMargin;
  leftMargin = mReportEngine->leftMargin;
  rightMargin = mReportEngine->rightMargin;
  pageWidth = mReportEngine->pageWidth;
  pageHeight = mReportEngine->pageHeight;
  relDpi_ = mReportEngine->relDpi_;
  relCalcDpi_ = mReportEngine->relCalcDpi_;
  fillRecords_ = mReportEngine->fillRecords_;

  // Copy the report header
  rHeader = mReportEngine->rHeader;
  // Copy the page header
  pHeader = mReportEngine->pHeader;
  // Copy the detail sections
  MReportSection *detail;
  QPtrList<MReportSection> * temp = mReportEngine->details;
  temp->setAutoDelete(true);
  for (detail = temp->first(); detail; detail = temp->next()) {
    MReportSection *new_detail = new MReportSection;
    *new_detail = *detail;
    details->append(new_detail);
  }
  delete temp;

  // Copy the page footer
  pFooter = mReportEngine->pFooter;
  // Copy the report footer
  rFooter = mReportEngine->rFooter;

  // Copy the rendering state
  currY = mReportEngine->currY;
  currHeight = mReportEngine->currHeight;
  currPage = mReportEngine->currPage;
  currDate = mReportEngine->currDate;
  cancelRender = mReportEngine->cancelRender;

  // Copy grand totals list
  grandTotal = mReportEngine->grandTotal;
}
