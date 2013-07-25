/****************************************************************************
** $Id: qt/qprinter_mac.cpp   3.3.8   edited Jan 11 14:38 $
**
** Implementation of QPrinter class for mac
**
** Created : 001019
**
** Copyright (C) 1992-2007 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech ASA of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Macintosh may use this file in accordance with the Qt Commercial
** License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qprinter.h"

#ifndef QT_NO_PRINTER

#include "qapplication.h"
#include "qapplication_p.h"
#include "qdir.h"
#include "qfile.h"
#include "qfileinfo.h"
#include "qmacstyle_mac.h"
#include "qpaintdevicemetrics.h"
#include "qprintdialog.h"
#include "qprinter_p.h"
#include "qpsprinter_p.h"
#include "qstyle.h"
#include "qt_mac.h"

#include <stdlib.h>


/*****************************************************************************
  External functions
 *****************************************************************************/
CFStringRef qstring2cfstring(const QString &); //qglobal.cpp
QString cfstring2qstring(CFStringRef);

/*****************************************************************************
  QPrinter member functions
 *****************************************************************************/

// QPrinter states

#define PST_IDLE        0
#define PST_ACTIVE      1
#define PST_ERROR       2
#define PST_ABORTED     3

static inline void qt_get_margins(PMPageFormat pformat, uint *top, uint *left,
                                  uint *bottom, uint *right)
{
  PMRect paperr, pager;
  if (PMGetAdjustedPaperRect(pformat, &paperr) != noErr) {
    qWarning("Qt: QPrinter: Unexpected condition reached %s:%d", __FILE__, __LINE__);
    return;
  }
  if (PMGetAdjustedPageRect(pformat, &pager) != noErr) {
    qWarning("Qt: QPrinter: Unexpected condition reached %s:%d", __FILE__, __LINE__);
    return;
  }
  if (top)
    *top = (uint)(pager.top - paperr.top);
  if (left)
    *left = (uint)(pager.left - paperr.left);
  if (bottom)
    *bottom = (uint)(paperr.bottom - pager.bottom);
  if (right)
    *right = (uint)(paperr.right - pager.right);
}

struct PaperSize {
  int w;
  int h;
};

static const PaperSize sizes[] = {
  { 210, 297 },   // A4
  { 176, 250 },   // B5
  { 216, 279 },   // U.S. Letter
  { 216, 356 },   // U.S. Legal
  { 191, 254 },   // U.S. Executive
  { 841, 1189 },  // A0
  { 594, 841 },   // A1
  { 420, 594 },   // A2
  { 297, 420 },   // A3
  { 148, 210 },   // A5
  { 105, 148 },   // A6
  { 74, 105 },    // A7
  { 52, 74 },     // A8
  { 37, 52 },     // A9
  { 1000, 1414 }, // B0
  { 707, 1000 },  // B1
  { 31, 44 },     // B10
  { 500, 707 },   // B2
  { 353, 500 },   // B3
  { 250, 353 },   // B4
  { 125, 176 },   // B6
  { 88, 125 },    // B7
  { 62, 88 },     // B8
  { 44, 62 },     // B9
  { 162, 229 },   // C5E
  { 105, 241 },   // Comm10E
  { 110, 222 },   // DLE
  { 216, 330 },   // Folio
  { 432, 279 },   // Ledger
  { 279, 432 }   // Tabloid
};

void qprinterSize2CarbonSize(QPrinter::PageSize &size, PMPageFormat &format, PMPrintSession session)
{
  if (size == QPrinter::Custom)
    return;
  // Basically, get the supported pages supported by the printer and see what will work,
  // Since we document undefined behavior when we don't specify anything, don't worry if we can't.
  PaperSize newSize = sizes[size];
  CFArrayRef formats;
  PMPrinter printer;

  if (PMSessionGetCurrentPrinter(session, &printer) == noErr
      && PMSessionCreatePageFormatList(session, printer, &formats) == noErr) {
    CFIndex total = CFArrayGetCount(formats);
    PMPageFormat tmp;
    PMRect paper;
    for (CFIndex idx = 0; idx < total; ++idx) {
      tmp = (PMPageFormat)CFArrayGetValueAtIndex(formats, idx);
      PMGetUnadjustedPaperRect(tmp, &paper);
      int wMM = int((paper.right - paper.left) / 72 * 25.4 + 0.5);
      int hMM = int((paper.bottom - paper.top) / 72 * 25.4 + 0.5);
      if (newSize.w == wMM && newSize.h == hMM) {
        PMCopyPageFormat(tmp, format);
        break;
      }
    }
    CFRelease(formats);
  }
}

QPrinter::PageSize carbonSize2QPrinterSize(const PMPageFormat pformat)
{
  PMRect paper;
  PMGetUnadjustedPaperRect(pformat, &paper);
  int wMM = int((paper.right - paper.left) / 72 * 25.4 + 0.5);
  int hMM = int((paper.bottom - paper.top) / 72 * 25.4 + 0.5);
  for (int i = QPrinter::A4; i < QPrinter::NPageSize; ++i) {
    if (sizes[i].w == wMM && sizes[i].h == hMM)
      return (QPrinter::PageSize)i;
  }
  return QPrinter::Custom;
}

QPrinter::QPrinter(PrinterMode m) : QPaintDevice(QInternal::Printer | QInternal::ExternalDevice)
{
  d = new QPrinterPrivate;
  if (PMCreateSession(&psession) != noErr)
    psession = NULL;

  switch (m) {
    case Compatible:
      devFlags |= QInternal::CompatibilityMode;
      // fall through
    case PrinterResolution:
    case HighResolution: {
      bool found = FALSE;
      PMPrinter printer = 0;
      if (psession && PMSessionGetCurrentPrinter(psession, &printer) == noErr) {
        PMResolution pres;
        UInt32 count = 0, maxRes = 0;
        if (PMPrinterGetPrinterResolutionCount(printer, &count) == noErr && count)
          for (; count > 0; --count)
            if (PMPrinterGetIndexedPrinterResolution(printer, count, &pres) == noErr) {
              found = TRUE;
              maxRes = QMAX((uint)pres.vRes, maxRes);
              res = maxRes;
            }
      }
      if (!found)
        res = 600; //just to have something
      break;
    }
    case ScreenResolution: {
      short vr, hr;
      ScreenRes(&hr, &vr);
      res = vr;
      break;
    }
  }

  if (PMCreatePageFormat(&pformat) == noErr &&
      PMSessionDefaultPageFormat(psession, pformat) == noErr)
    page_size = carbonSize2QPrinterSize(pformat);
  else
    page_size = A4;

  //other
  orient = Portrait;

  page_order = FirstPageFirst;
  paper_source = OnlyOne;
  color_mode = GrayScale;
  ncopies = 1;
  from_pg = to_pg = min_pg = max_pg = 0;
  state = PST_IDLE;
  output_file = FALSE;
  to_edge     = FALSE;

  //mac specific
  pformat = kPMNoPageFormat;
  psettings = kPMNoPrintSettings;
  prepare(&pformat);
  prepare(&psettings);
  interpret(&pformat);
  interpret(&psettings);

  d->printerOptions = 0;
  setPrintRange(AllPages);
}

QPrinter::~QPrinter()
{
#if 0
  if (pformat != kPMNoPageFormat)
    PMDisposePageFormat(pformat);
  if (psettings != kPMNoPrintSettings)
    PMDisposePrintSettings(psettings);
#endif
}


bool QPrinter::newPage()
{
  if (state != PST_ACTIVE)
    return FALSE;
  if (PMSessionEndPage(psession) != noErr)  { //end the last page
    state = PST_ERROR;
    return FALSE;
  }

  if (PMSessionBeginPage(psession, pformat, 0) != noErr)  { //start a new one
    state = PST_ERROR;
    return FALSE;
  }
  if (PMSessionGetGraphicsContext(psession, kPMGraphicsContextQuickdraw, &hd) != noErr) {
    state = PST_ERROR;
    return FALSE;
  }

  if (fullPage()) {
    uint top, left, bottom, right;
    qt_get_margins(pformat, &top, &left, &bottom, &right);
    QMacSavedPortInfo mp(this);
    SetOrigin(top, left);
  } else {
    QMacSavedPortInfo mp(this);
    SetOrigin(0, 0);
  }
  return TRUE;
}


bool QPrinter::abort()
{
  if (state != PST_ACTIVE)
    return FALSE;
  if (PMSessionEndPage(psession) == noErr &&
      PMSessionEndDocument(psession) == noErr) {
    hd = NULL;
    state = PST_ABORTED;
  }
  return aborted();
}

bool QPrinter::aborted() const
{
  return (state == PST_ABORTED);
}

bool
QPrinter::prepare(PMPrintSettings *s)
{
  if (!psession && PMCreateSession(&psession) != noErr)
    return FALSE;
  if (*s == kPMNoPrintSettings) {
    if (PMCreatePrintSettings(s) != noErr)
      return FALSE;
    if (PMSessionDefaultPrintSettings(psession, *s) != noErr)
      return FALSE;
  } else {
    if (PMSessionValidatePrintSettings(psession, *s, kPMDontWantBoolean) != noErr)
      return FALSE;
  }
  if (minPage() != 0 || maxPage() != 0)
    PMSetPageRange(*s, minPage(), maxPage());
  if (d->printRange != AllPages) {
    PMSetFirstPage(*s, fromPage(), TRUE);
    PMSetLastPage(*s, toPage(), TRUE);
  }
  PMSetColorMode(*s, colorMode() == GrayScale ? kPMGray : kPMColor);
  PMSetCopies(*s, numCopies(), TRUE);
  if (outputToFile()) {
    CFStringRef cfstring = qstring2cfstring(outputFileName());
    CFURLRef outFile = CFURLCreateWithFileSystemPath(kCFAllocatorSystemDefault, cfstring,
                                                     kCFURLPOSIXPathStyle, false);
    PMSessionSetDestination(psession, *s, kPMDestinationFile, kPMDocumentFormatPDF, outFile);
    CFRelease(outFile);
    CFRelease(cfstring);
  }

  QString printName = printerName();
  if (!printName.isEmpty()) {
    CFStringRef pname = qstring2cfstring(printName);
    PMSessionSetCurrentPrinter(psession, pname);
    CFRelease(pname);
  }
  return TRUE;
}

bool
QPrinter::prepare(PMPageFormat *f)
{
  if (!psession && PMCreateSession(&psession) != noErr)
    return FALSE;
  if (*f == kPMNoPageFormat) {
    if (PMCreatePageFormat(f) != noErr)
      return FALSE;
    if (PMSessionDefaultPageFormat(psession, *f) != noErr)
      return FALSE;
  } else {
    if (PMSessionValidatePageFormat(psession, *f, kPMDontWantBoolean) != noErr)
      return FALSE;
  }
  if (page_size != carbonSize2QPrinterSize(*f))
    qprinterSize2CarbonSize(page_size, *f, psession);
  PMSetOrientation(*f, orientation() == Portrait ? kPMPortrait : kPMLandscape, TRUE);
  PMResolution pres;
  pres.hRes = res;
  pres.vRes = res;
  PMSetResolution(*f, &pres);
  return TRUE;
}


void QPrinter::setPrinterName(const QString &name)
{
  if (state != 0) {
#if defined(QT_CHECK_STATE)
    qWarning("Qt: QPrinter::setPrinterName: Cannot do this during printing");
#endif
    return;
  }
  printer_name = name;
}

bool QPrinter::setup(QWidget *)
{
  if (!psession && PMCreateSession(&psession) != noErr)
    return FALSE;
  return (printSetup() && pageSetup());
}

void QPrinter::interpret(PMPrintSettings *s)
{
  //get values
  UInt32 from, to;
  if (PMGetFirstPage(*s, &from) == noErr && PMGetLastPage(*s, &to) == noErr) {
    if (to == INT_MAX) //this means all pages!
      from = to = 0;
    setFromTo(from, to);
  }

  UInt32 copies;
  if (PMGetCopies(*s, &copies) == noErr)
    setNumCopies(copies);

  UInt32 max, min;
  if (PMGetPageRange(*s, &min, &max) == noErr) {
    // The problem here is that Apple's print dialog will always show a one even when you pass
    // it a zero. So take care of that. The other problem is that we can also call this because
    // we need to interpret the default settings, in that case, the min and max will both be
    // zero. In that case, leave them "as is" as some other functions will reset the max and
    // make everything OK when the dialog is actually called.
    int newMin = min;
    int newMax = max;

    if (newMin < 1 && newMax != 0)
      newMin = 1;
    setMinMax(newMin, newMax);
  }

  PMColorMode cm;
  if (PMGetColorMode(*s, &cm) == noErr)
    setColorMode(cm == kPMGray ? GrayScale : Color);
  // Get the current Printer Name
  CFIndex currPrinterIndex;
  PMPrinter currPrinter;
  CFArrayRef printerArray;
  OSStatus err = PMSessionCreatePrinterList(psession, &printerArray,
                                            &currPrinterIndex, &currPrinter);
  if (err != noErr)
    qWarning("Qt: QPrinter::interpret problem creating printer list %ld", err);
  if (printerArray) {
    setPrinterName(cfstring2qstring((CFStringRef)CFArrayGetValueAtIndex(printerArray,
                                                                        currPrinterIndex)));
    CFRelease(printerArray);
  }
}

void QPrinter::interpret(PMPageFormat *f)
{
  //get values
  PMOrientation o;
  if (PMGetOrientation(*f, &o) == noErr)
    setOrientation(o == kPMPortrait ? Portrait : Landscape);

  //Finally we update the scale so the resolution is effected by it
  PMSessionValidatePageFormat(psession, *f, kPMDontWantBoolean);
  setPageSize(carbonSize2QPrinterSize(*f));
}

// shows the native mac print setup dialog
/*!
    \internal
*/
bool QPrinter::printSetup()
{
  if (!psession && PMCreateSession(&psession) != noErr)
    return FALSE;
  QMacBlockingFunction block;
  //setup
  if (!prepare(&psettings))
    return FALSE;
  {
    //simulate modality
    QWidget modal_widg(0, __FILE__ "__modal_dlg",
                       Qt::WType_TopLevel | Qt::WStyle_Customize | Qt::WStyle_DialogBorder);
    qt_enter_modal(&modal_widg);
    Boolean ret;
    if (!outputToFile() && PMSessionPrintDialog(psession, psettings, pformat, &ret) != noErr)
      ret = FALSE;
    qt_leave_modal(&modal_widg);
    if (!ret)
      return FALSE;
  }

  interpret(&psettings);
  return TRUE;
}

// shows the native mac page setup dialog
/*!
    \internal
*/
bool QPrinter::pageSetup()
{
  if (!psession && PMCreateSession(&psession) != noErr)
    return FALSE;
  QMacBlockingFunction block;
  //page format
  if (!prepare(&pformat))
    return FALSE;
  {
    //simulate modality
    QWidget modal_widg(0, __FILE__ "__modal_dlg",
                       Qt::WType_TopLevel | Qt::WStyle_Customize | Qt::WStyle_DialogBorder);
    qt_enter_modal(&modal_widg);
    Boolean ret;
    if (PMSessionPageSetupDialog(psession, pformat, &ret) != noErr)
      ret = FALSE;
    qt_leave_modal(&modal_widg);
    if (!ret)
      return FALSE;
  }

  interpret(&pformat);
  return TRUE;
}


bool QPrinter::cmd(int c, QPainter *, QPDevCmdParam *)
{
  if (!psession && PMCreateSession(&psession) != noErr)
    return FALSE;

  if (c ==  PdcBegin) {                     // begin; start printing
    if (state != PST_IDLE) {
      qDebug("Qt: internal: printer: two PdcBegin(s).");
      return FALSE;
    }

    //just to be sure they've been setup
    prepare(&pformat);
    prepare(&psettings);

    //validate the settings
    if (PMSessionValidatePrintSettings(psession, psettings, kPMDontWantBoolean) != noErr)
      return FALSE;
    if (PMSessionValidatePageFormat(psession, pformat, kPMDontWantBoolean) != noErr)
      return FALSE;

    if (PMSessionBeginDocument(psession, psettings, pformat) != noErr) //begin the document
      return FALSE;


    if (PMSessionBeginPage(psession, pformat, 0) != noErr) //begin the page
      return FALSE;
    if (PMSessionGetGraphicsContext(psession, kPMGraphicsContextQuickdraw,
                                    &hd) != noErr) //get the gworld
      return FALSE;
    state = PST_ACTIVE;
    if (fullPage()) {
      uint top, left, bottom, right;
      qt_get_margins(pformat, &top, &left, &bottom, &right);
      QMacSavedPortInfo mp(this);
      SetOrigin(top, left);
    } else {
      QMacSavedPortInfo mp(this);
      SetOrigin(0, 0);
    }
  } else if (c == PdcEnd) {
    if (hd && state != PST_IDLE) {
      PMSessionEndPage(psession);
      PMSessionEndDocument(psession);
      hd = NULL;
    }
    state  = PST_IDLE;
  } else {                                    // all other commands...
    if (state == PST_ABORTED || ((state == PST_ACTIVE || state == PST_ERROR) && PMSessionError(psession) != noErr))
      return FALSE;
  }
  return TRUE;
}

static inline int qt_get_PDMWidth(PMPageFormat pformat, bool fullPage)
{
  int val = 0;
  PMRect r;
  if (fullPage) {
    if (PMGetAdjustedPaperRect(pformat, &r) == noErr)
      val = (int)(r.right - r.left);
  } else {
    if (PMGetAdjustedPageRect(pformat, &r) == noErr)
      val = (int)(r.right - r.left);
  }
  return val;
}

static inline int qt_get_PDMHeight(PMPageFormat pformat, bool fullPage)
{
  int val = 0;
  PMRect r;
  if (fullPage) {
    if (PMGetAdjustedPaperRect(pformat, &r) == noErr)
      val = (int)(r.bottom - r.top);
  } else {
    if (PMGetAdjustedPageRect(pformat, &r) == noErr)
      val = (int)(r.bottom - r.top);
  }
  return val;
}

int QPrinter::metric(int m) const
{
  int val = 1;
  switch (m) {
    case QPaintDeviceMetrics::PdmWidth: {
      bool orientInSync = true;
      PMOrientation o;
      QPrinter::Orientation tmpOrient = orientation();
      if (PMGetOrientation(pformat, &o) == noErr) {
        orientInSync = ((o == kPMPortrait && tmpOrient == Portrait)
                        || o == kPMLandscape && tmpOrient == Landscape);
      }
      PageSize s = pageSize();
      if (s == QPrinter::Custom) {
        val = tmpOrient == Portrait ? customPaperSize_.width() : customPaperSize_.height();
      } else {
        if (s > QPrinter::Custom)
          s = QPrinter::Custom;
        if (state == PST_ACTIVE || (tmpOrient == Portrait || orientInSync)) {
          val = qt_get_PDMWidth(pformat, fullPage());
        } else {
          val = qt_get_PDMHeight(pformat, fullPage());
        }
      }
      break;
    }
    case QPaintDeviceMetrics::PdmHeight: {
      bool orientInSync = true;
      PMOrientation o;
      QPrinter::Orientation tmpOrient = orientation();
      if (PMGetOrientation(pformat, &o) == noErr) {
        orientInSync = ((o == kPMPortrait && tmpOrient == Portrait)
                        || o == kPMLandscape && tmpOrient == Landscape);
      }
      PageSize s = pageSize();
      if (s == QPrinter::Custom) {
        val = tmpOrient == Portrait ? customPaperSize_.height() : customPaperSize_.width();
      } else {        
        if (s > QPrinter::Custom)
          s = QPrinter::Custom;
        if (state == PST_ACTIVE || (tmpOrient == Portrait || orientInSync)) {
          val = qt_get_PDMHeight(pformat, fullPage());
        } else {
          val = qt_get_PDMWidth(pformat, fullPage());
        }
      }
      break;
    }
    // We don't have to worry about the printer state here as metric() does that for us.
    case QPaintDeviceMetrics::PdmWidthMM:
      val = metric(QPaintDeviceMetrics::PdmWidth);
      val = (val * 254 + 5 * res) / (10 * res);
      break;
    case QPaintDeviceMetrics::PdmHeightMM:
      val = metric(QPaintDeviceMetrics::PdmHeight);
      val = (val * 254 + 5 * res) / (10 * res);
      break;
    case QPaintDeviceMetrics::PdmPhysicalDpiX:
    case QPaintDeviceMetrics::PdmPhysicalDpiY: {
      PMPrinter printer;
      if (PMSessionGetCurrentPrinter(psession, &printer) == noErr) {
        PMResolution resolution;
        PMPrinterGetPrinterResolution(printer, kPMCurrentValue, &resolution);
        val = (int)resolution.vRes;
        break;
      }
      //otherwise fall through
    }
    case QPaintDeviceMetrics::PdmDpiY:
    case QPaintDeviceMetrics::PdmDpiX:
      val = res;
      break;
    case QPaintDeviceMetrics::PdmNumColors:
      val = (1 << metric(QPaintDeviceMetrics::PdmDepth));
      break;
    case QPaintDeviceMetrics::PdmDepth:
      val = 24;
      break;
    default:
      val = 0;
#if defined(QT_CHECK_RANGE)
      qWarning("Qt: QPixmap::metric: Invalid metric command");
#endif
  }
  return val;
}


QSize QPrinter::margins() const
{
  uint t, l, r, b;
  // If the printer is not active, we need to flip the values for the landscape case.
  margins(&t, &l, &r, &b);

  if (state == PST_ACTIVE || orientation() == Portrait)
    return QSize(t, l);
  else
    return QSize(r, t);
}

void QPrinter::setMargins(uint, uint, uint, uint)
{
}

void QPrinter::margins(uint *top, uint *left, uint *bottom, uint *right) const
{
  // If the printer is not active, we need to flip the values for the landscape case.
  if (state == PST_ACTIVE || orientation() == Portrait)
    qt_get_margins(pformat, top, left, bottom, right);
  else
    qt_get_margins(pformat, right, top, left, bottom);
}


#endif
