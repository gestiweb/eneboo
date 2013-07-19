/***************************************************************************
 mfieldobject.cpp  -  Kugar report field object
 -------------------
 begin     : Mon Aug 23 1999
 copyright : (C) 1999 by Mutiny Bay Software
 email     : info@mutinybaysoftware.com
 ***************************************************************************/

#include "mfieldobject.h"
#include "mutil.h"

#include "FLCodBar.h"

/** Constructor */
MFieldObject::MFieldObject() :
  MLabelObject()
{
  // Set the defaults
  fieldName = "";
  dataType = MFieldObject::String;
  format = MUtil::MDY_SLASH;
  precision = 0;
  currency = 8364;
  negativeValueColor.setRgb(255, 0, 0);
  comma = 0;
  blankZero = 0;
  codbarType = FLCodBar::nameToType("code128");
  codbarRes = 72;
  saveColor.setRgb(0, 0, 0);
}

/** Copy constructor */
MFieldObject::MFieldObject(const MFieldObject &mFieldObject) :
  MLabelObject((MLabelObject &) mFieldObject)
{
  copy(&mFieldObject);
}

/** Assignment operator */
MFieldObject MFieldObject::operator=(const MFieldObject &mFieldObject)
{
  if (&mFieldObject == this)
    return *this;

  // Copy the derived class's data
  copy(&mFieldObject);

  // Copy the base class's data
  ((MLabelObject &) *this) = mFieldObject;

  return *this;
}

/** Destructor */
MFieldObject::~MFieldObject()
{
}

/** Sets the field's data string */
void MFieldObject::setText(const QString txt)
{
  QDate d;
  int pos, ret;
  QString month, day, year;
  QRegExp regexp("[0-9][0-9](-|//)[0-9][0-9](-|//)[0-9][0-9][0-9][0-9]");
  double val;

  // Set the data
  switch (dataType) {
    case MFieldObject::String:
      if (aqApp->multiLangEnabled() && !txt.isEmpty()) {
        text = trUtf8(txt.utf8());
        if (text == txt)
          text = tr(txt);
      } else
        text = txt;
      break;
    case MFieldObject::Integer:
      val = txt.toDouble();
      if ((val < 0.5) && (val > -0.5) && blankZero) {
        text = QString::null;
      } else {
        text.setNum(val, 'f', 0);
        formatNegValue();
        if (comma)
          formatCommas();
      }
      break;
    case MFieldObject::Float:
      val = txt.toDouble();
      if ((val < 0.0000001) && (val > -0.0000001) && blankZero) {
        text = QString::null;
      } else {
        text.setNum(val, 'f', precision);
        formatNegValue();
        if (comma)
          formatCommas();
      }
      break;
    case MFieldObject::Date:
      if (txt.isEmpty())
        text = QString::null;
      else {
        // Check if we can read the date and if so, reformat it ...
        pos = regexp.search(txt.mid(0));
        ret = regexp.matchedLength();

        if (ret == -1) {
          year = txt.left(4);
          day = txt.right(2);
          month = txt.mid(5, 2);

          if (year.toInt() != 0 && month.toInt() != 0 && day.toInt() != 0) {
            d.setYMD(year.toInt(), month.toInt(), day.toInt());
            text = MUtil::formatDate(d, format);
          } else {
            text = QString::null;
          }
        } else
          text = txt;
      }
      break;
    case MFieldObject::Currency:
      val = txt.toDouble();
      if ((val < 0.01) && (val > -0.01) && blankZero) {
        text = QString::null;
      } else {
        text.setNum(val, 'f', 2);
        formatNegValue();
        if (comma)
          formatCommas();
        text.append(currency);
      }
      break;
    case MFieldObject::Pixmap:
      if (!txt.isEmpty() && paintFunction.isEmpty()) {
        if (!pixmap)
          pixmap = new QPixmap();
        QCString cs(txt);
        if (!QPixmapCache::find(cs.left(100), *pixmap)) {
          if (QFile::exists(txt))
            pixmap->load(txt);
          else
            pixmap->loadFromData(cs);
          if (!pixmap->isNull())
            QPixmapCache::insert(cs.left(100), *pixmap);
        }
        if (pixmap->isNull()) {
          delete pixmap;
          pixmap = 0;
        }
      } else {
        if (pixmap) {
          delete pixmap;
          pixmap = 0;
        }
      }
      break;
    case MFieldObject::Codbar: {
      if (!txt.isEmpty() && paintFunction.isEmpty()) {
        FLCodBar cb(txt, codbarType, 10, 1, 0, 0, true, Qt::black, Qt::white, codbarRes);
        if (!pixmap)
          pixmap = new QPixmap();
        if (!cb.pixmap().isNull())
          *pixmap = cb.pixmap();
        else {
          delete pixmap;
          pixmap = 0;
        }
      } else {
        if (pixmap) {
          delete pixmap;
          pixmap = 0;
        }
      }
    }
    break;
    case MFieldObject::Bool:
      bool ok;
      double d = txt.toDouble(&ok);
      if (txt.upper() == "FALSE" || txt.upper() == "F" || (d == 0 && ok))
        text = tr("No");
      else
        text = tr("Sí");
      break;
  }
}

/** Sets code bar type */
void MFieldObject::setCodBarType(const QString &t)
{
  codbarType = FLCodBar::nameToType(t);
}

int MFieldObject::draw(FLStylePainter *p)
{
  if (dataType != MFieldObject::Codbar)
    return MLabelObject::draw(p);

  if (pixmap && pixmap->isNull()) {
    delete pixmap;
    pixmap = 0;
    drawBase(p);
    return 0;
  } else if (pixmap) {
    if (!changeHeight) {
      int sy = pixmap->height() - height;
      if (sy < 0)
        sy = 0;
      if (!p->drawPixmap(*pixmap, 0, sy, width, height, this))
        p->painter()->drawPixmap(0, 0, *pixmap, 0, sy, width, height);
      return 0;
    } else {
      uint originalHeight = height;
      int pixH = pixmap->height();
      height = pixH;
      if (!p->drawPixmap(*pixmap, 0, 0, width, height, this))
        p->painter()->drawPixmap(0, 0, *pixmap, 0, 0, width, height);
      height = originalHeight;
      return pixH;
    }
  }
}

/** Formats a string representation of a negative number using the negative value color */
void MFieldObject::formatNegValue()
{
  if (text.toDouble() < 0)
    foregroundColor = negativeValueColor;
  else
    foregroundColor = saveColor;
}

/** Formats the string representation of a number with comma separators */
void MFieldObject::formatCommas()
{
  QString tmp;
  int i, j;
  int offset;

  // If a neg value
  if (text.toDouble() < 0)
    offset = 1;
  else
    offset = 0;

  // Look for decimal point
  int pos = text.findRev(QApplication::tr("."));

  // If a decimal was not found, start at end of string
  if (pos == -1)
    pos = text.length();
  else
    tmp = QApplication::tr(",") + text.mid(pos + 1, text.length());

  // Move through the string and insert the commas
  for (i = pos - 1, j = 0; i >= offset; i--) {
    tmp = text.mid(i, 1) + tmp;
    j++;
    if ((j == 3) && (i - 1 >= offset)) {
      tmp = QApplication::tr(".") + tmp;
      j = 0;
    }
  }

  // If neg value, add back the sign
  if (offset)
    tmp = "-" + tmp;

  // Set the new string to the field
  text = tmp;
}

/** Copies member data from one object to another.
 Used by the copy constructor and assignment operator */
void MFieldObject::copy(const MFieldObject *mFieldObject)
{
  // Copy the fields's data type and format
  fieldName = mFieldObject->fieldName;
  dataType = mFieldObject->dataType;
  format = mFieldObject->format;
  precision = mFieldObject->precision;
  currency = mFieldObject->currency;
  negativeValueColor = mFieldObject->negativeValueColor;
  saveColor = mFieldObject->saveColor;
  comma = mFieldObject->comma;
  blankZero = mFieldObject->blankZero;
  codbarType = mFieldObject->codbarType;
  codbarRes = mFieldObject->codbarRes;
}

int MFieldObject::RTTI() const
{
  return MReportObject::Field;
}
