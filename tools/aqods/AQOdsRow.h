/***************************************************************************
 AQOdsRow.h
 -------------------
 begin                : 12/12/2011
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

#ifndef AQODSROW_H_
#define AQODSROW_H_

#include <qregexp.h>

#include "odf-gen/row.h"

#include "AQOdsStyle.h"
#include "AQOdsChart.h"
#include "AQOdsImage.h"

extern QString encodeAttrODS(const QString &);

class AQOdsRow : public Row
{
public:
  AQOdsRow(AQOdsSheet &sheet) : Row(sheet) {}

  AQOdsRow &opIn(const AQOdsChart &value,
                 uint column_span = 0,
                 uint row_span = 0) {
    add_cell(value, column_span, row_span);
    return *this;
  }

  AQOdsRow &opIn(const AQOdsImage &value,
                 uint column_span = 0,
                 uint row_span = 0) {
    add_cell(value, column_span, row_span);
    return *this;
  }

  AQOdsRow &opIn(const AQOdsStyle &style) {
    add_style(style);
    return *this;
  }

  AQOdsRow &opIn(const QString &value,
                 uint column_span = 0,
                 uint row_span = 0) {
    add_cell((const char *)encodeAttrODS(formatURLs(value)).utf8(), column_span, row_span);
    return *this;
  }

  AQOdsRow &opIn(double value,
                 uint column_span = 0,
                 uint row_span = 0) {
    add_cell(value, column_span, row_span);
    return *this;
  }

  AQOdsRow &coveredCell() {
    add_coveredcell();
    return *this;
  }

  AQOdsRow &addBgColor(const AQOdsColor &color) {
    add_bgcolor(color);
    return *this;
  }

  AQOdsRow &addFgColor(const AQOdsColor &color) {
    add_fgcolor(color);
    return *this;
  }

  AQOdsRow &setFixedPrecision(uint p) {
    set_fixed_precision(p);
    return *this;
  }

private:
  QString formatURLs(const QString &v) const {
    if (v.find("http://") == -1)
      return v;
    QRegExp rx("((([A-Za-z]{3,9}:(?:\\/\\/)?)(?:[\\-;:&=\\+\\$,\\w]+@)?[A-Za-z0-9\\.\\-]+|(?:www\\.|[\\-;:&=\\+\\$,\\w]+@)[A-Za-z0-9\\.\\-]+)((?:\\/[\\+~%\\/\\.\\w\\-]*)?\\??(?:[\\-\\+=&;%@\\.\\w]*)#?(?:[\\.\\!\\/\\\\\\w]*))?)");
    QString theUrl(v);
    return theUrl.replace(rx, "__HREF1__='\\1'__HREF2__\\1__HREF3__");
  }
};

#endif /* AQODSROW_H_ */
