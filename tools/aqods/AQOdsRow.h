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

#include "odf-gen/row.h"

#include "AQOdsStyle.h"
#include "AQOdsChart.h"
#include "AQOdsImage.h"

static inline bool isXmlChar(const QChar &c)
{
  // Characters in this range must be accepted by XML parsers.
  // Consequently characters outside of this range need to be escaped.

  ushort uc = c.unicode();

  return uc == 0x9
         || uc == 0xA
         || uc == 0xD
         || 0x20 <= uc && uc <= 0xD7FF
         || 0xE000 <= uc && uc <= 0xFFFD;
}

// ### AbanQ
// to do compliant with the standar XML 1.0
// 2.11 End-of-Line Handling and 3.3.3 Attribute-Value Normalization
// see below encodeAttr
static inline bool isEndOfLineChar(const QChar &c)
{
  ushort uc = c.unicode();

  return uc == 0x9
         || uc == 0xA
         || uc == 0xD;
}
// ### AbanQ

static inline QString encodeAttr(const QString &str)
{
  QString tmp(str);
  uint len = tmp.length();
  uint i = 0;
  while (i < len) {
    if (tmp[(int)i] == '<') {
      tmp.replace(i, 1, "&lt;");
      len += 3;
      i += 4;
    } else if (tmp[(int)i] == '"') {
      tmp.replace(i, 1, "&quot;");
      len += 5;
      i += 6;
    } else if (tmp[(int)i] == '&') {
      tmp.replace(i, 1, "&amp;");
      len += 4;
      i += 5;
    } else if (tmp[(int)i] == '>' && i >= 2 && tmp[(int)i - 1] == ']' && tmp[(int)i - 2] == ']') {
      tmp.replace(i, 1, "&gt;");
      len += 3;
      i += 4;
    }
    // ### AbanQ
    // to do compliant with the standar XML 1.0
    // 2.11 End-of-Line Handling and 3.3.3 Attribute-Value Normalization
    else if (isEndOfLineChar(tmp[(int)i])) {
      QString repl = "&#x" + QString::number(tmp[(int)i].unicode(), 16) + ';';
      tmp.replace(i, 1, repl);
      len += repl.length() - 1;
      i += repl.length();
    }
    // ### AbanQ
    else if (!isXmlChar(tmp[(int)i])) {
      QString repl = "&#x" + QString::number(tmp[(int)i].unicode(), 16) + ';';
      qWarning("AQOdsRow: not saving invalid character %s, the document will not be well-formed", repl.latin1());
      repl = "?";
      tmp.replace(i, 1, repl);
      len += repl.length() - 1;
      i += repl.length();
    } else {
      ++i;
    }
  }

  return tmp;
}

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
    add_cell((const char *) encodeAttr(value), column_span, row_span);
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
};

#endif /* AQODSROW_H_ */
