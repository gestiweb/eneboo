/***************************************************************************
             mreportengine.h  -  Kugar report engine
             -------------------
   begin     : Sun Aug 15 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
   copyright : (C) 2002 Alexander Dymo
   email     : cloudtemple@mksat.net
   copyright : (C) 2002-2004 InfoSiAL S.L.
   email     : mail@infosial.com
***************************************************************************/

#ifndef MREPORTENGINE_H
#define MREPORTENGINE_H

#include <qobject.h>
#include <qpicture.h>
#include <qpaintdevicemetrics.h>
#include <qsize.h>
#include <qmemarray.h>
#include <qvaluevector.h>
#include <qdom.h>
#include <FLUtil.h>
#include <FLStylePainter.h>
#include <math.h>

#include "mlineobject.h"
#include "mlabelobject.h"
#include "mspecialobject.h"
#include "mfieldobject.h"
#include "mreportsection.h"
#include "mreportdetail.h"
#include "mpagecollection.h"

class AQPointKey;
class AQPaintItem;
typedef QMap<AQPointKey, AQPaintItem> AQPaintItemMap;

/**
Kugar report engine

@author Mutiny Bay Software
*/
class MReportEngine: public QObject
{
  Q_OBJECT

  Q_ENUMS(RenderReportFlags)

public:

  enum PageOrientation {
    Portrait,
    Landscape
  };

  enum PageSize {
    A4, B5, Letter, Legal, Executive,
    A0, A1, A2, A3, A5, A6, A7, A8, A9, B0, B1,
    B10, B2, B3, B4, B6, B7, B8, B9, C5E, Comm10E,
    DLE, Folio, Ledger, Tabloid, Custom, NPageSize = Custom, CustomOld = 31
  };

  // Si cambias este Enum hazlo tambien en mreportviewer.h
  enum RenderReportFlags {
    Append        = 0x00000001,
    Display       = 0x00000002,
    PageBreak     = 0x00000004,
    FillRecords   = 0x00000008
  };

  MReportEngine(QObject *parent = 0);
  MReportEngine(const MReportEngine &mReportEngine, QObject *parent = 0);
  MReportEngine operator=(const MReportEngine &mReportEngine);
  virtual ~MReportEngine();

  bool setReportData(const QString &);
  bool setReportData(QIODevice *);
  bool setReportData(QDomNode &);
  bool setReportTemplate(const QString &);
  bool setReportTemplate(QIODevice *);
  bool setReportTemplate(QDomNode &);

  void exportToOds(MPageCollection *pages);

  MPageCollection *renderReport(int initRow = 0, int initCol = 0,
                                MPageCollection *pages = 0,
                                uint flags = MReportEngine::Display);
public slots:

  int getRenderSteps() {
    return records.length() / 2;
  }

  QString csvData();

  float relDpi() const {
    return relDpi_;
  }

  float relCalcDpi() const {
    return relCalcDpi_;
  }

  void setStyleName(const QString &style) {
    styleName_ = style;
  }

  void slotCancelRendering();
  void setRelDpi(float relDpi);

signals:

  void signalRenderStatus(int);
  void preferedTemplate(const QString &);

protected:

  /**
  Report data document
  */
  QDomDocument *rd;

  /**
  Report template document
  */
  QDomDocument *rt;

  /**
  Report painter
  */
  FLStylePainter *p;

  /**
  Report page size
  */
  int pageSize;

  /**
  Report page orientation
  */
  int pageOrientation;

  /**
  Report top margin
  */
  int topMargin;

  /**
  Report top margin
  */
  int bottomMargin;

  /**
  Report top margin
  */
  int leftMargin;

  /**
  Report top margin
  */
  int rightMargin;

  /**
  Page width in mm for PageSize = Custom
  */
  int customWidthMM;

  /**
  Page height in mm for PageSize = Custom
  */
  int customHeightMM;

  /**
  Report page width
  */
  int pageWidth;

  /**
  Report page height
  */
  int pageHeight;

  /**
  Report header
  */
  MReportSection *rHeader;

  /**
  Page header
  */
  MReportSection *pHeader;

  /**
  Detail add-on headers
  */
  QPtrList<MReportSection> *addOnHeaders;

  /**
  Detail add-on footers
  */
  QPtrList<MReportSection> *addOnFooters;

  /**
  Detail headers
  */
  QPtrList< MReportSection> *dHeaders;

  /**
  Detail sections
  */
  QPtrList<MReportSection> *details;

  /**
  Detail footers
  */
  QPtrList<MReportSection> *dFooters;

  /**
  Page footer
  */
  MReportSection *pFooter;

  /**
  Report footer
  */
  MReportSection *rFooter;

  /**
  Current y position on page
  */
  int currY;

  /**
  Current x position on page
  */
  int currX;

  /**
  Current height of page excluding margins
  */
  int currHeight;

  /**
  Current page number
  */
  int currPage;

  /**
  Current report date
  */
  QDate currDate;

  /**
  Current record being treated
  */
  int currRecord_;

  /**
  Grand total array
  */
  QPtrList<QMemArray<double> > *grandTotal;

  /**
  Footer's total array
  */
  QPtrList<QMemArray<double> > *gDTFooters[ 10 ];
  QValueVector<QString> *gDTSFooters[ 10 ];

  /**
  Print to POS printer
  */
  bool printToPos;

  /**
  Cancel rendering flag
  */
  bool cancelRender;

  /**
  The set of records being rendered.
  */
  QDomNodeList records;

  /**
  Clears report destroying all resources
  */
  void clear();

  /**
  Clears report grand totals
  */
  void clearGrantTotals();

  /**
  Clears report formatting
  */
  void clearFormatting();

  /**
  Starts a new page of the report
  */
  void startPage(MPageCollection *pages, int levelAddOn = -1);

  /**
  Finishes the current page of the report
  */
  void endPage(MPageCollection *pages);

  /**
  Finishes the current page and adds a new page
  */
  void newPage(MPageCollection *pages, int levelAddOn = -1);

  /**
  Draws the report header object to the current page
  */
  void drawReportHeader(MPageCollection *pages);

  /**
  Draws the page header to the current page
  */
  void drawPageHeader(MPageCollection *pages);

  /**
  Draws the page footer to the current page
  */
  void drawPageFooter(MPageCollection *pages);

  /**
  Draws the report footer to the current page
  */
  void drawReportFooter(MPageCollection *pages);

  /**
  Draws the detail footer to the level
  */
  void drawDetailFooter(MPageCollection *pages, int level,
                        QPtrList<QMemArray<double> > *gDT = 0,
                        QValueVector<QString> *gDTS = 0);

  /**
  Draws the detail add-on header to the level
  */
  void drawAddOnHeader(MPageCollection *pages, int level,
                       QPtrList<QMemArray<double> > *gDT,
                       QValueVector<QString> *gDTS = 0);

  /**
  Draws the detail add-on footer to the level
  */

  void drawAddOnFooter(MPageCollection *pages, int level,
                       QPtrList<QMemArray<double> > *gDT,
                       QValueVector<QString> *gDTS);
  /**
  Draws the detail header to the level
  */
  void drawDetailHeader(MPageCollection *pages, int level);

  /**
  Draws the detail to the level
  */
  void drawDetail(MPageCollection *pages, int level, uint &currRecord,
                  const int initRow = 0, const int initCol = 0);

  /**
  Gets the metrics for the selected page size
  */
  QSize getPageMetrics(int size, int orientation);

  /**
  Inits
  */
  void initData();
  void initTemplate();

  /**
  Sets the main layout attributes for the report
  */
  void setReportAttributes(QDomNode *report);

  /**
  Sets the layout attributes for the given report section
  */
  void setSectionAttributes(MReportSection *section, QDomNode *report);

  /**
  Sets the layout attributes for the detail headers and footers
  */
  void setDetMiscAttributes(MReportSection *section, QDomNode *report);

  /**
  Sets the layout attributes for the detail section
  */
  void setDetailAttributes(MReportSection *section, QDomNode *report);

  /**
  Sets a line's layout attributes
  */
  void setLineAttributes(MLineObject *line, QDomNamedNodeMap *attr);

  /**
  Sets a label's layout attributes
  */
  void setLabelAttributes(MLabelObject *label, QDomNamedNodeMap *attr);

  /**
  Sets a special field's layout attributes
  */
  void setSpecialAttributes(MSpecialObject *field,
                            QDomNamedNodeMap *attr);

  /**
  Sets a field's layout attributes
  */
  void setFieldAttributes(MFieldObject *field, QDomNamedNodeMap *attr);

  /**
  Sets a calculated field's layout attributes
  */
  void setCalculatedFieldAttributes(MCalcObject *field,
                                    QDomNamedNodeMap *attr);

  /**
  Copies member data from one object to another.

  Used by the copy constructor and assignment operator
  */
  void copy(const MReportEngine *mReportEngine);

  /**
  Decides whether the current detail header fits in the current page or not
  */
  bool canDrawDetailHeader(uint level, uint currRecord, uint yPos);

  /**
  Decides whether the current detail fits in the current page or not
  */
  bool canDrawDetail(uint level, uint currRecord, uint yPos);

  /**
  Finds the detail header object, which is apropriate for the given level
  */
  MReportSection *findDetailHeader(int level);

  /**
  Finds the add-on detail header object, which is apropriate for the given level
  */
  MReportSection *findAddOnHeader(int level);

  /**
  Finds the detail object, which is apropriate for the given level
  */
  MReportSection *findDetail(int level);

  /**
  Finds the detail footer object, which is apropriate for the given level
  */
  MReportSection *findDetailFooter(int level);

  /**
  Finds the detail add-on footer object, which is apropriate for the given level.
  If no footer is found, trys to find a general add-on footer level -1)
  @param. level: Level of the footer's detail
  */
  MReportSection *findAddOnFooter(int level);

  /**
  Reserves size in the footers' total arrays to store the items used to calculate
  calculated field values in the previous levels footers
  */
  void reserveSizeForCalcFields(QDomNamedNodeMap *fields, int level);

  /**
  Puts the field values into the detail section and sets the values of the total arrays
  items used to calculate calculated field values in the previous levels footers
  @param noTotal: Only set Field Values, do not calculate aggregations
  */
  void setFieldValues(QDomNamedNodeMap *fields, int level,
                      MReportSection *detail, QDomNode *ptrRecord,
                      bool noTotal = false);

  /**
  Uso interno
  */
  void updateCsvData(int level, uint &currRecord, QString &csvData);
  bool execPage(QPainter *painter, QDataStream &s, uint nrecords,
                AQPaintItemMap &map);

  float relDpi_;
  float relCalcDpi_;
  QString styleName_;
  bool fillRecords_;
};

#endif
