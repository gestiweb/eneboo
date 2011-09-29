/***************************************************************************
             mfieldobject.h  -  Kugar report field object
             -------------------
   begin     : Mon Aug 23 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#ifndef MFIELDOBJECT_H
#define MFIELDOBJECT_H

#include <qregexp.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qpixmapcache.h>

#include "mlabelobject.h"

/**
Kugar report field object

@author Mutiny Bay Software
*/
class MFieldObject: public MLabelObject
{

  Q_OBJECT

public:

  /**
  Data type constants
  */
  enum DataType {
    String = 0,
    Integer,
    Float,
    Date,
    Currency,
    Pixmap,
    Codbar,
    Bool
  };

  /**
  Constructor
  */
  MFieldObject();

  /**
  Copy constructor
  */
  MFieldObject(const MFieldObject &mFieldObject);

  /**
  Assignment operator
  */
  MFieldObject operator=(const MFieldObject &mFieldObject);

  /**
  Destructor
  */
  virtual ~ MFieldObject();

protected:

  /**
  Field name
  */
  QString fieldName;

  /**
  Field data type
  */
  int dataType;

  /**
  Code bar type
  */
  int codbarType;

  /**
  Code bar res
  */
  int codbarRes;

  /**
  Field date format
  */
  int format;

  /**
  Field precision
  */
  int precision;

  /**
  Field currency symbol
  */
  QChar currency;

  /**
  Field's negative value color
  */
  QColor negativeValueColor;

  /**
  Field's original color
  */
  QColor saveColor;

  /**
  Field's comma flag
  */
  int comma;

  /**
  Field's blank zero flag
  */
  int blankZero;

public:

  int getDataType() const;
  int getBlankZero() const;

  /**
  Returns the bound data field name
  */
  QString getFieldName() const;

  /**
  Sets the bound data field
  */
  void setFieldName(const QString field);

  /**
  Sets the field's data string - default is an empty string
  */
  void setText(const QString txt);

  /**
  Sets the field's data type
  */
  void setDataType(int t);

  /**
  Sets code bar type
  */
  void setCodBarType(const QString &t);

  /**
  Sets code bar res in dpi (dots per inch)
  */
  void setCodBarRes(int r);

  /**
  Sets the field's date formatting
  */
  void setDateFormat(int f);

  /**
  Sets the field's precision
  */
  void setPrecision(int p);

  /**
  Sets the field's currency symbol
  */
  void setCurrency(const QChar c);

  /**
  Sets the object's negative value color - default is red
  */
  void setNegValueColor(int r, int g, int b);

  /**
  Sets if object should delimit numeric values with commas
  */
  void setCommaSeparator(int c);

  /**
  Sets if zero values are substituted by spaces.
  */
  void setBlankZero(int z);

  /**
  Draws the field
  @param  p: Pintor, que pintas con amor
  */
  virtual int draw(FLStylePainter *p);

private:

  /**
  Formats a string representation of a negative number using the negative value color
  */
  void formatNegValue();

  /**
  Formats a string representation of number with comma seperators
  */
  void formatCommas();

  /**
  Copies member data from one object to another.

  Used by the copy constructor and assignment operator
  */
  void copy(const MFieldObject *mFieldObject);

  friend class MReportSection;
};

inline int MFieldObject::getDataType() const
{
  return dataType;
}

/** Returns the bound data field name */
inline QString MFieldObject::getFieldName() const
{
  return fieldName;
}

/** Sets the bound data field */
inline void MFieldObject::setFieldName(const QString field)
{
  fieldName = field;
}

/** Sets the field's data type */
inline void MFieldObject::setDataType(int t)
{
  dataType = t;
  if (dataType == MFieldObject::Integer ||
      dataType == MFieldObject::Float ||
      dataType == MFieldObject::Currency)
    saveColor = foregroundColor;
}

/** Sets code bar res in dpi (dots per inch) */
inline void MFieldObject::setCodBarRes(int r)
{
  codbarRes = r;
}

/** Sets the field's date formatting */
inline void MFieldObject::setDateFormat(int f)
{
  format = f;
}

/** Sets the field's precision */
inline void MFieldObject::setPrecision(int p)
{
  precision = p;
}

/** Sets the field's currency symbol */
inline void MFieldObject::setCurrency(const QChar c)
{
  if (c.isNull())
    currency = QChar(8364);
  else
    currency = c;
}

/** Sets the object's negative value color - default is red*/
inline void MFieldObject::setNegValueColor(int r, int g, int b)
{
  negativeValueColor.setRgb(r, g, b);
}

/** Sets if object should delimit numeric values with commas */
inline void MFieldObject::setCommaSeparator(int c)
{
  comma = c;
}

/** Sets if zero values are substituted by spaces. */
inline void MFieldObject::setBlankZero(int z)
{
  blankZero = z;
}

inline int MFieldObject::getBlankZero() const {
  return blankZero;
}
#endif
