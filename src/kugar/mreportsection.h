/***************************************************************************
             mreportsection.h  -  Kugar report section
             -------------------
   begin     : Mon Aug 23 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
   copyright : (C) 2002 Alexander Dymo
   email     : cloudtemple@mksat.net
***************************************************************************/

#ifndef MREPORTSECTION_H
#define MREPORTSECTION_H

#include <qptrlist.h>
#include <qobject.h>
#include <qvaluevector.h>
#include <qvaluelist.h>
#include <qdom.h>
#include <qpicture.h>

#include "mlineobject.h"
#include "mlabelobject.h"
#include "mspecialobject.h"
#include "mcalcobject.h"
#include "mpagecollection.h"

/**
Kugar report section

@author Mutiny Bay Software
*/

class MReportSection: public QObject
{

public:

  /**
  Section print frequency constants
  */
  enum PrintFrequency {
    FirstPage = 0,
    EveryPage,
    LastPage
  };

  /**
  Constructor
  */
  MReportSection(const QString &strIdSec = QString::null);

  /**
  Copy constructor
  */
  MReportSection(const MReportSection &mReportSection);

  /**
  Assignment operator
  */
  MReportSection operator=(const MReportSection &mReportSection);

  /**
  Destructor
  */
  virtual ~ MReportSection();

protected:

  /**
  Sections's height
  */
  int height;

  /**
  Sections's width
  */
  int width;

  /**
  Sections's level in hierarchy
  */
  int level;

  /**
  New page at end of the section
  */
  bool newPage_;

  /**
  Current footer at bottom of the page
  */
  bool placeAtBottom_;

  /**
  Draw details on all pages
  */
  bool drawAllPages_;

  /**
  Section print frequency
  */
  int frequency;

  /**
  Page number - used by special fields
  */
  int pageNumber;

  /**
  Name of the fiels that conditios the drawing of the section
  */
  QString drawIf;

  /**
  Report date - used by special fields
  */
  QDate reportDate;

private:

  /**
  Section's line collection
  */
  QPtrList<MLineObject> lines;

  /**
  Section's label collection
  */
  QPtrList<MLabelObject> labels;

  /**
  Section's special field collection
  */
  QPtrList<MSpecialObject> specialFields;

  /**
  Section's calculated field collection
  */
  QPtrList<MCalcObject> calculatedFields;

  /**
  Section's field collection
  */
  QPtrList<MFieldObject> fields;

public:

  /**
  Adds a new field object to the section's field collection
  */
  void addField(MFieldObject *field);

  /**
  Adds a new line object to the section's line collection
  */
  void addLine(MLineObject *line);

  /**
  Adds a new label object to the section's label collection
  */
  void addLabel(MLabelObject *label);

  /**
  Adds a new special field object to the section's special field collection
  */
  void addSpecialField(MSpecialObject *special);

  /**
  Adds a new calculated field object to the section's calculated field collection
  */
  void addCalculatedField(MCalcObject *calc);

  /**
  Sets the height of the section
  */
  void setHeight(int h);

  /**
  Sets the width of the section
  */
  void setWidth(const int w);

  /**
  Gets the height of the section
  */
  int getHeight() const;
  int getHeight(FLStylePainter *p);

  /**
  Gets the width of the section
  */
  int getWidth() const;

  /**
  Sets the level of the section
  */
  void setLevel(int l);

  /**
  Gets the level of the section
  */
  int getLevel() const;

  /**
  Sets the condition for drawing the section
  */
  void setDrawIf(const QString &dI);

  /**
  Gets the condition for drawing the section
  */
  QString getDrawIf() const;

  /**
  Sets new page at end of the section
  */
  void setNewPage(bool b);

  /**
  Sets the current footer at de bottom of the page
  */
  void setPlaceAtBottom(bool b);

  /**
  Sets if the detail is on all pages
  */
  void setDrawAllPages(bool b);

  /**
  Gets if there is new page at end of the section
  */
  bool newPage();

  /**
  Gets if the footer must be drawed at the bottom of the page
  */
  bool placeAtBottom();

  /**
  Gets if the detail is on all pages
  */
  bool drawAllPages();

  /**
  Set the current page number - used by special fields
  */
  void setPageNumber(int page);

  /**
  Set the current date - used by special fields
  */
  void setReportDate(const QDate &date);

  /**
  Sets the print frequency of the section
  */
  void setPrintFrequency(int printFrequency);

  /**
  Returns the print frequency of the section
  */
  int printFrequency();

  /**
  Returns the name of the bound field for the calculated field object at the given index
  */
  QString getCalcFieldName(int idx);

  /**
  Returns the index of the calculated field object for the given bound field
  */
  int getCalcFieldIndex(const QString &field);

  /**
  Returns the index of the calculated field object for the given bound field
  */
  int getFieldIndex(const QString &field);

  /**
  Sets the data for the specified calculated field
  */
  void setCalcFieldData(int idx, const QString &data);

  /**
  Sets the data for the field at the specified index
  */
  void setFieldData(int idx, const QString &data,
                    QDomNode *record = 0, bool fillRecord = false);

  /**
  Sets the data for the all calculated fields - list size must match number of calculated fields
  */
  void setCalcFieldData(QPtrList<QMemArray<double> > *values = 0,
                        QValueVector<QString> *valueS = 0,
                        QDomNode *record = 0, bool fillRecord = false);

  /**
  Sets the data for the calculated fields whose calculation source is the grand total
  */
  void setCalcFieldDataGT(QPtrList<QMemArray<double> > *values,
                          QDomNode *record = 0, bool fillRecord = false);

  /**
  Calculates the value for a calculated field
  @param field: Field to calculate
  @param *values: pointer to array of source values
  @param valueS: current value of the field (used in the No Operation calculation)
  @param *record: pointer to the current xml node of values (used in the Call Function calculation)
  */
  void calculateField(MCalcObject *field, QMemArray<double> *values,
                      QString valueS = QString::null, QDomNode *record = 0, bool fillRecord = false);

  /**
  Returns the number of fields in the section
  */
  int getFieldCount() const;

  /**
  Returns the name of the bound field for field object at the given index
  */
  QString getFieldName(int idx);

  /**
  Returns the number of calculated fields in the section
  */
  int getCalcFieldCount() const;

  /**
  Draws the section to the specified painter & x/y-offsets
  */
  virtual void draw(FLStylePainter *p, int xoffset, int yoffset, int &newHeight);

  /**
  Draws objects at the header
  */
  void drawHeaderObjects(FLStylePainter *p, MPageCollection *pages, MReportSection *header);

  /**
  Frees all resources allocated by the report section
  */
  virtual void clear();

  /**
  Returns wether the Report Section must be drawed or not depending on the DrawIf attribute and the current record values
  */
  bool mustBeDrawed(QDomNode *record);

  /**
  Returns the horizontal offset of the last section drawed
  */
  int getLastXOffset() const;

  /**
  Returns the vertical offset of the last section drawed
  */
  int getLastYOffset() const;

  /**
  Returns the page index of the last section drawed
  */
  uint getLastPageIndex() const;

  /**
  Sets the page index of the last section drawed
  */
  void setLastPageIndex(uint i);

  /**
  Returns the field object at the given index
  */
  MFieldObject *getField(int idx);

  /**
  Returns the calculatedfield object at the given index
  */
  MCalcObject *getCalcField(int idx);

  QPicture *onPage();
  void setOnPage(QPicture *page);

  QString csvData();

  void setIdSec(uint id) {
    idSec_ = id;
  }
  uint idSec() const {
    return idSec_;
  }
  static void resetIdSecGlob();

protected:

  /**
  Draws the section base objects to the specified painter & x/y offsets
  */
  void drawObjects(FLStylePainter *p, int xoffset, int yoffset, int &newHeight);

private:

  /**
  Copies member data from one object to another.

  Used by the copy constructor and assignment operator
  */
  void copy(const MReportSection *mReportSection);

  int lastXOffset;
  int lastYOffset;
  uint lastPageIndex;
  QPicture *onPage_;

  uint idSec_;
  static uint idSecGlob_;

  QString strIdSec_;
};

/** Gets the height of the section */
inline int MReportSection::getHeight() const
{
  return height;
}

/** Gets the width of the section */
inline int MReportSection::getWidth() const
{
  return width;
}

/** Sets the height of the section */
inline void MReportSection::setHeight(int h)
{
  height = h;
}

/** Sets the width of the section */
inline void MReportSection::setWidth(const int w)
{
  width = w;
}

/** Sets the condition for drawing the section*/
inline void MReportSection::setDrawIf(const QString &dI)
{
  drawIf = dI;
}

/** Gets the condition for drawing the section*/
inline QString MReportSection::getDrawIf() const
{
  return drawIf;
}

/** Sets the level of the section */
inline void MReportSection::setLevel(int l)
{
  level = l;
}

/** Gets the level of the section */
inline int MReportSection::getLevel() const
{
  return level;
}

/** Sets new page at end of the section */
inline void MReportSection::setNewPage(bool b)
{
  newPage_ = b;
}

/** Sets new page at end of the section */
inline void MReportSection::setPlaceAtBottom(bool b)
{
  placeAtBottom_ = b;
}

/** Sets if the detail is on all pages */
inline void MReportSection::setDrawAllPages(bool b)
{
  drawAllPages_ = b;
}

/** Gets if there is new page at end of the section */
inline bool MReportSection::newPage()
{
  return newPage_;
}

/** Gets if the footer must be drawed at the bottom of the page */
inline bool MReportSection::placeAtBottom()
{
  return placeAtBottom_;
}

/** Gets if the detail is on all pages */
inline bool MReportSection::drawAllPages()
{
  return drawAllPages_;
}

/** Set the current page number - used by special fields */
inline void MReportSection::setPageNumber(int page)
{
  pageNumber = page;
}

/** Set the current date - used by special fields */
inline void MReportSection::setReportDate(const QDate &date)
{
  reportDate = date;
}

/** Sets the print frequency of the section */
inline void MReportSection::setPrintFrequency(int printFrequency)
{
  frequency = printFrequency;
}

/** Returns the print frequency of the section */
inline int MReportSection::printFrequency()
{
  return frequency;
}

inline int MReportSection::getLastXOffset() const
{
  return lastXOffset;
}

inline int MReportSection::getLastYOffset() const
{
  return lastYOffset;
}

inline uint MReportSection::getLastPageIndex() const
{
  return lastPageIndex;
}

inline void MReportSection::setLastPageIndex(uint i)
{
  lastPageIndex = i;
}

inline QPicture *MReportSection::onPage()
{
  return onPage_;
}

inline void MReportSection::setOnPage(QPicture *page)
{
  onPage_ = page;
}

/** Returns the number of calculated fields in the section */
inline int MReportSection::getCalcFieldCount() const
{
  return calculatedFields.count();
}

/** Returns the number of fields in the detail section */
inline int MReportSection::getFieldCount() const
{
  return fields.count();
}

/** Returns the name of the bound field for field object at the given index */
inline QString MReportSection::getFieldName(int idx)
{
  return fields.at(idx)->getFieldName();
}

/** Returns the name of the bound field for the calculated field object at the given index */
inline QString MReportSection::getCalcFieldName(int idx)
{
  return calculatedFields.at(idx)->getFieldName();
}

/** Sets the data for the specified calculated field */
inline void MReportSection::setCalcFieldData(int idx, const QString &data)
{
  calculatedFields.at(idx)->setText(data);
}

/** Returns the field object at the given index */
inline MFieldObject *MReportSection::getField(int idx)
{
  return fields.at(idx);
}

/** Returns the calculatedfield object at the given index */
inline MCalcObject *MReportSection::getCalcField(int idx)
{
  return calculatedFields.at(idx);
}
#endif
