/***************************************************************************
           mreportsection.cpp  -  Kugar report section
           -------------------
 begin     : Mon Aug 23 1999
 copyright : (C) 1999 by Mutiny Bay Software
 email     : info@mutinybaysoftware.com
 copyright : (C) 2002 Alexander Dymo
 email     : cloudtemple@mksat.net
***************************************************************************/

#include "mreportsection.h"
#include "mutil.h"
#include "FLApplication.h"
#include "FLObjectFactory.h"

uint MReportSection::idSecGlob_ = 0;

/** Constructor */
MReportSection::MReportSection(const QString &strIdSec) : strIdSec_(strIdSec)
{
  idSec_ = idSecGlob_++;

  // Set geometry
  height = 1;
  width = 584;
  level = 0;

  // Set print frequency
  frequency = MReportSection::EveryPage;

  // Set special field data
  reportDate = QDate::currentDate();
  pageNumber = 0;

  // Set the line list to AutoDelete
  lines.setAutoDelete(true);
  // Set the label list to AutoDelete
  labels.setAutoDelete(true);
  // Set the special field list to AutoDelete
  specialFields.setAutoDelete(true);
  // Set the label list to AutoDelete
  fields.setAutoDelete(true);
  // Set the calc field list to AutoDelete
  calculatedFields.setAutoDelete(true);
}

/** Copy constructor */
MReportSection::MReportSection(const MReportSection &mReportSection)
{
  copy(&mReportSection);
}

/** Assignment operator */
MReportSection MReportSection::
operator= (const MReportSection &mReportSection)
{
  if (&mReportSection == this)
    return *this;

  // Copy the derived class's data
  copy(&mReportSection);

  return *this;
}

/** Destructor */
MReportSection::~MReportSection()
{
  clear();
}

/** Frees all resources allocated by the report section */
void MReportSection::clear()
{
  // Clear the line collection
  lines.clear();
  // Clear the label collection
  labels.clear();
  // Clear the special field collection
  specialFields.clear();
  // Clear the calculated field collection
  calculatedFields.clear();
  // Clear the field collection
  fields.clear();
}

/** Adds a new line object to the section's line collection */
void MReportSection::addLine(MLineObject *line)
{
  lines.append(line);
}

/** Adds a new label object to the section's label collection */
void MReportSection::addLabel(MLabelObject *label)
{
  labels.append(label);
  label->setSectionIndex(labels.at());
}

/** Adds a new special field object to the section's special field collection */
void MReportSection::addSpecialField(MSpecialObject *special)
{
  specialFields.append(special);
  special->setSectionIndex(specialFields.at());
}

/** Adds a new calculated field object to the section's calculated field collection */
void MReportSection::addCalculatedField(MCalcObject *calc)
{
  calculatedFields.append(calc);
  calc->setSectionIndex(calculatedFields.at());
}

/** Adds a new field object to the section's field collection */
void MReportSection::addField(MFieldObject *field)
{
  fields.append(field);
  field->setSectionIndex(fields.at());
}

void MReportSection::setFieldData(int idx, const QString &data,
                                  QDomNode *record, bool fillRecord)
{
  MFieldObject *field = fields.at(idx);
  field->setText(data);
  if (record && fillRecord) {
    MFieldObject::DataType fieldType =
      (MFieldObject::DataType)field->getDataType();
    switch (fieldType) {
      case MFieldObject::Date:
      case MFieldObject::Pixmap:
      case MFieldObject::Codbar:
        break;
      default:
        record->toElement().setAttribute(strIdSec_ + QString::number(level) + '_' +
                                         field->getFieldName(),
                                         field->getText());
    }
  }
}

/** Sets the data for the all calculated fields - list size must number of calculated fields */
void MReportSection::setCalcFieldData(QPtrList<QMemArray<double> > *values,
                                      QValueVector<QString> *valueS,
                                      QDomNode *record, bool fillRecord)
{
  MCalcObject *field;
  int i = 0;
  int calcType;
  QString value = "";

  // Calculate and set the calculated field's data

  for (field = calculatedFields.first(); field != 0;
       field = calculatedFields.next()) {

    if (field->getFromGrandTotal())
      continue;

    calcType = field->getCalculationType();

    if (calcType == MCalcObject::NoOperation) {
      if (valueS)
        value = (*valueS)[ i ];

      calculateField(field, 0, value, record, fillRecord);
    } else if (calcType == MCalcObject::CallFunction) {
      calculateField(field, 0, value, record, fillRecord);
    } else {
      if (values)
        calculateField(field, values->at(i), value, record, fillRecord);
    }

    i++;
  }
}

/** Sets the data for the calculated fields whose calculation source is the grand total */
void MReportSection::setCalcFieldDataGT(QPtrList<QMemArray<double> > *values,
                                        QDomNode *record, bool fillRecord)
{
  MCalcObject *field;
  int grandTotalIndex = 0;

  // Calculate and set the calculated field's data
  for (field = calculatedFields.first(); field != 0;
       field = calculatedFields.next()) {
    if (!field->getFromGrandTotal() && level > -1)
      continue;
    grandTotalIndex = field->getSectionIndex();
    if (grandTotalIndex != -1)
      calculateField(field, values->at(grandTotalIndex),
                     QString::null, record, fillRecord);
  }
}

/** Sets the data for the a calculated field */
void MReportSection::calculateField(MCalcObject *field, QMemArray<double> *values,
                                    QString valueS, QDomNode *record, bool fillRecord)
{
  switch (field->getCalculationType()) {
    case MCalcObject::Count:
      if (values) {
        field->setText(QString::number(MUtil::count(values)));
        if (record && fillRecord) {
          record->toElement().setAttribute(strIdSec_ + QString::number(level) + '_' +
                                           field->getFieldName(), field->getText());
        }
      }
      break;

    case MCalcObject::Sum:
      if (values) {
        field->setText(QString::number(MUtil::sum(values), 'f'));
        if (record && fillRecord) {
          record->toElement().setAttribute(strIdSec_ + QString::number(level) + '_' +
                                           field->getFieldName(), field->getText());
        }
      }
      break;

    case MCalcObject::Average:
      if (values) {
        field->setText(QString::number(MUtil::average(values), 'f'));
        if (record && fillRecord) {
          record->toElement().setAttribute(strIdSec_ + QString::number(level) + '_' +
                                           field->getFieldName(), field->getText());
        }
      }
      break;

    case MCalcObject::Variance:
      if (values) {
        field->setText(QString::number(MUtil::variance(values), 'f'));
        if (record && fillRecord) {
          record->toElement().setAttribute(strIdSec_ + QString::number(level) + '_' +
                                           field->getFieldName(), field->getText());
        }
      }
      break;

    case MCalcObject::StandardDeviation:
      if (values) {
        field->setText(QString::number(MUtil::stdDeviation(values), 'f'));
        if (record && fillRecord) {
          record->toElement().setAttribute(strIdSec_ + QString::number(level) + '_' +
                                           field->getFieldName(), field->getText());
        }
      }
      break;

    case MCalcObject::NoOperation: {
      field->setText(valueS);
      if (fillRecord && !valueS.isEmpty()) {
        record->toElement().setAttribute(strIdSec_ + QString::number(level) + '_' +
                                         field->getFieldName(), field->getText());
      }
    }
    break;

    case MCalcObject::CallFunction: {
      if (record && !field->getCalculationFunction().isEmpty()) {
        FLDomNodeInterface *dni = new FLDomNodeInterface(*record);
        QSArgumentList l;
        l << dni;
        l << QVariant(field->getFieldName());

        QVariant v(
          aqApp->call(
            field->getCalculationFunction(), l, 0
          ).variant()
        );
        if (v.isValid() && v.toString().upper() != "NAN") {
          field->setText(v.toString());

          if (fillRecord && field->getDataType() != MFieldObject::Pixmap) {
            record->toElement().setAttribute(strIdSec_ + QString::number(level) + '_' +
                                             field->getCalculationFunction(), field->getText());
          }
        }

        delete dni;
      }
    }

    break;
  }

  if (record)
    field->setDomNodeData(*record);
}

/** Returns wether the Report Section must be drawed or not depending on the DrawIf attribute and the current record values */
bool MReportSection::mustBeDrawed(QDomNode *record)
{
  QString value;
  QDomNamedNodeMap fields = record->attributes();
  QString drawIfField = getDrawIf();

  if (!drawIfField.isEmpty()) {
    QDomNode n = fields.namedItem(drawIfField);

    if (n.isNull())
      return false;

    value = n.toAttr().value();

    if (value.isEmpty() || value == "false" || value == "")
      return false;

    bool b = true;
    float f = value.toFloat(&b);
    if (f == 0 && b)
      return false;
  }

  return true;
}

/** Returns the index of the calculated field object for the given bound field */
int MReportSection::getCalcFieldIndex(const QString &field)
{
  // Find the field in the calculated field collection and return the index
  for (MCalcObject *tmpField = calculatedFields.first(); tmpField != 0;
       tmpField = calculatedFields.next()) {
    if (tmpField->getFieldName() == field)
      break;
  }
  return calculatedFields.at();
}

/** Returns the index of the field object for the given bound field */
int MReportSection::getFieldIndex(const QString &field)
{
  // Find the field in the calculated field collection and return the index
  for (MFieldObject *tmpField = fields.first(); tmpField != 0;
       tmpField = fields.next()) {
    if (tmpField->getFieldName() == field)
      break;
  }
  return fields.at();
}

/** Draws the section to the specified painter & x/y-offsets */
void MReportSection::draw(FLStylePainter *p, int xoffset, int yoffset, int &newHeight)
{
  drawObjects(p, xoffset, yoffset, newHeight);
  lastXOffset = xoffset;
  lastYOffset = yoffset;
}


/** Draws the section base objects to the specified painter & x/y offsets */
void MReportSection::drawHeaderObjects(FLStylePainter *p, MPageCollection *pages, MReportSection *header)
{
  MCalcObject *calcfield;

  // Set the offsets
  int xcalc = header->getLastXOffset();
  int ycalc = header->getLastYOffset();

  QPicture *currentPage = (QPicture *) p->painter()->device();
  QPicture *lastPage = header->onPage();
  QPicture *currentPageCopy = 0;
  QPicture *lastPageCopy = 0;

  if (currentPage != lastPage) {
    p->painter()->end();
    currentPageCopy = new QPicture(*currentPage);
    lastPageCopy = new QPicture(*lastPage);
    p->painter()->begin(lastPage);
    lastPageCopy->play(p->painter());
  }

  QObject::setName(QString("_##H%1-%2").arg(strIdSec_).arg(level));
  p->beginSection(xcalc, ycalc, width, height, this);
  uint countObj = 0;

  // Draw the calculated field collection
  for (calcfield = calculatedFields.first(); calcfield != 0;
       calcfield = calculatedFields.next()) {
    if (calcfield->getDrawAtHeader()) {
      if (calcfield->getObjectId()) {
        calcfield->QObject::setName(QString("_##H%1-Calc.%2-%3")
                                    .arg(idSec_)
                                    .arg(calcfield->fieldName)
                                    .arg(calcfield->getObjectId()));
      } else {
        calcfield->QObject::setName(QString("_##H%1-Calc.%2-%3")
                                    .arg(idSec_)
                                    .arg(calcfield->fieldName)
                                    .arg(countObj++));
      }
      p->beginMark(calcfield->getX(), calcfield->getY(), calcfield);
      calcfield->draw(p);
      p->endMark();
    }
  }

  p->endSection();

  if (currentPage != lastPage) {
    p->painter()->end();
    p->painter()->begin(currentPage);
    currentPageCopy->play(p->painter());
    delete lastPageCopy;
    delete currentPageCopy;
  }
}

/** Draws the section base objects to the specified painter & x/y offsets */
void MReportSection::drawObjects(FLStylePainter *p, int xoffset, int yoffset, int &newHeight)
{
  MLineObject *line;
  MLabelObject *label;
  MSpecialObject *special;
  MCalcObject *calcfield;
  MFieldObject *field;

  // Set the offsets
  int xcalc = xoffset;
  int ycalc = yoffset;

  int modifiedHeight = 0;

  QObject::setName(QString("_##%1-%2").arg(strIdSec_).arg(level));
  p->beginSection(xcalc, ycalc, width, height, this);
  uint countObj = 0;
  uint yObjectPos = 0;

  // Draw the line collection
  for (line = lines.first(); line != 0; line = lines.next()) {
    if (line->getObjectId()) {
      line->QObject::setName(QString("_##Line%1-%2")
                             .arg(idSec_)
                             .arg(line->getObjectId()));
    } else {
      line->QObject::setName(QString("_##Line%1-%2")
                             .arg(idSec_)
                             .arg(countObj++));
    }
    p->beginMark(line->xpos1, line->ypos1, line);
    line->draw(p);
    p->endMark();
  }

  // Draw the label collection
  for (label = labels.first(); label != 0; label = labels.next()) {
    if (label->getObjectId()) {
      label->QObject::setName(QString("_##Label%1-%2")
                              .arg(idSec_)
                              .arg(label->getObjectId()));
    } else {
      label->QObject::setName(QString("_##Label%1-%2")
                              .arg(idSec_)
                              .arg(countObj++));
    }
    yObjectPos = label->getDrawAtBottom() ? newHeight - label->getHeight() : label->getY();
    p->beginMark(label->getX(), yObjectPos, label);
    modifiedHeight = label->draw(p);
    p->endMark();

    if (modifiedHeight && (label->getY() + modifiedHeight) > height)
      newHeight = label->getY() + modifiedHeight;
  }

  // Draw the calculated field collection
  for (calcfield = calculatedFields.first(); calcfield != 0;
       calcfield = calculatedFields.next()) {
    if (!calcfield->getDrawAtHeader()) {
      if (calcfield->getObjectId()) {
        calcfield->QObject::setName(QString("_##%1-Calc.%2-%3")
                                    .arg(idSec_)
                                    .arg(calcfield->fieldName)
                                    .arg(calcfield->getObjectId()));
      } else {
        calcfield->QObject::setName(QString("_##%1-Calc.%2-%3")
                                    .arg(idSec_)
                                    .arg(calcfield->fieldName)
                                    .arg(countObj++));
      }
      yObjectPos = calcfield->getDrawAtBottom() ? newHeight - calcfield->getHeight() : calcfield->getY();
      p->beginMark(calcfield->getX(), yObjectPos, calcfield);
      modifiedHeight = calcfield->draw(p);
      p->endMark();

      if (modifiedHeight && (calcfield->getY() + modifiedHeight) > height)
        newHeight = calcfield->getY() + modifiedHeight;
    }
  }

  // Draw the special field collection
  for (special = specialFields.first(); special != 0;
       special = specialFields.next()) {
    if (special->getObjectId()) {
      special->QObject::setName(QString("_##SpecialField%1-%2")
                                .arg(idSec_)
                                .arg(special->getObjectId()));
    } else {
      special->QObject::setName(QString("_##SpecialField%1-%2")
                                .arg(idSec_)
                                .arg(countObj++));
    }
    yObjectPos = special->getDrawAtBottom() ? newHeight - special->getHeight() : special->getY();
    p->beginMark(special->getX(), yObjectPos, special);

    switch (special->getType()) {

      case MSpecialObject::Date:
        special->setText(reportDate);
        break;

      case MSpecialObject::PageNumber:
        special->setText(pageNumber);
        break;
    }

    special->draw(p);
    p->endMark();
  }

  // Draw the field collection
  for (field = fields.first(); field != 0; field = fields.next()) {
    if (field->getObjectId()) {
      field->QObject::setName(QString("_##%1.%2-%3")
                              .arg(idSec_)
                              .arg(field->fieldName)
                              .arg(field->getObjectId()));
    } else {
      field->QObject::setName(QString("_##%1.%2-%3")
                              .arg(idSec_)
                              .arg(field->fieldName)
                              .arg(countObj++));
    }
    yObjectPos = field->getDrawAtBottom() ? newHeight - field->getHeight() : field->getY();
    p->beginMark(field->getX(), yObjectPos, field);
    modifiedHeight = field->draw(p);
    p->endMark();

    if (modifiedHeight && (field->getY() + modifiedHeight) > height)
      newHeight = field->getY() + modifiedHeight;
  }

  p->endSection();
}

QString MReportSection::csvData()
{
  MCalcObject *calcfield;
  MFieldObject *field;
  QString fieldValue;
  QString csvData;

  for (calcfield = calculatedFields.first(); calcfield != 0;
       calcfield = calculatedFields.next()) {
    if (calcfield->getCalculationType() == MCalcObject::NoOperation ||
        calcfield->getCalculationType() == MCalcObject::CallFunction) {
      fieldValue = calcfield->getText();
      fieldValue.replace("\n", "-");
      csvData += "|" + fieldValue;
    }
  }

  for (field = fields.first(); field != 0; field = fields.next()) {
    fieldValue = field->getText();
    fieldValue.replace("\n", "-");
    csvData += "|" + fieldValue;
  }

  return csvData;
}

/** Calculates the height of se section according to its ChangeHeight Fields */
int MReportSection::getHeight(FLStylePainter *p)
{
  MLabelObject *label;
  MCalcObject *calcfield;
  MFieldObject *field;

  int modifiedHeight = 0;
  int newHeight = height;

  for (label = labels.first(); label != 0; label = labels.next()) {
    modifiedHeight = label->calcHeight(p);
    if (modifiedHeight && (label->getY() + modifiedHeight) > height)
      newHeight = label->getY() + modifiedHeight;
  }

  for (calcfield = calculatedFields.first(); calcfield != 0; calcfield = calculatedFields.next()) {
    if (!calcfield->getDrawAtHeader()) {
      modifiedHeight = calcfield->calcHeight(p);
      if (modifiedHeight && (calcfield->getY() + modifiedHeight) > height)
        newHeight = calcfield->getY() + modifiedHeight;
    }
  }

  for (field = fields.first(); field != 0; field = fields.next()) {
    modifiedHeight = field->calcHeight(p);
    if (modifiedHeight && (field->getY() + modifiedHeight) > height)
      newHeight = field->getY() + modifiedHeight;
  }

  return newHeight;
}

void MReportSection::resetIdSecGlob()
{
  idSecGlob_ = 0;
}

/** Copies member data from one object to another.
      Used by the copy constructor and assignment operator */
void MReportSection::copy(const MReportSection *mReportSection)
{
  clear();

  strIdSec_ = mReportSection->strIdSec_;
  idSec_ = mReportSection->idSec_;

  // Copy the section's geometry
  height = mReportSection->height;

  // Copy the print frequency
  frequency = mReportSection->frequency;

  // Copy the line list
  lines = mReportSection->lines;
  // Copy the label list
  labels = mReportSection->labels;
  // Copy the special field list
  specialFields = mReportSection->specialFields;
  // Copy the calculated field list
  calculatedFields = mReportSection->calculatedFields;
  // Copy the field list
  fields = mReportSection->fields;
}
