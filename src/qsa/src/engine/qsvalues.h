/****************************************************************************
** $Id: qsvalues.h  1.1.5   edited 2006-02-23T15:39:57$
**
** Copyright (C) 2001-2006 Trolltech AS.  All rights reserved.
**
** This file is part of the Qt Script for Applications framework (QSA).
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding a valid Qt Script for Applications license may use
** this file in accordance with the Qt Script for Applications License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about QSA Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
*****************************************************************************/

#ifndef QSVALUES_H
#define QSVALUES_H

#include "qsglobal.h"
#include "qsobject.h"

class QUICKCORE_EXPORT QSUndefined : public QSObject
{
public:
  QSUndefined(const QSEnv *env);
};


class QUICKCORE_EXPORT QSNull : public QSObject
{
public:
  QSNull(const QSEnv *env);
};


class QUICKCORE_EXPORT QSBoolean : public QSObject
{
public:
  QSBoolean(const QSEnv *env, bool b = FALSE);
  bool value() const;
};

//### AbanQ
inline bool QSBoolean::value() const
{
  return bVal();
}
//### AbanQ

class QUICKCORE_EXPORT QSNumber : public QSObject
{
public:
  QSNumber(const QSEnv *, double value);

  double value() const;
  int intValue() const;
  bool isNaN() const;
  bool isInf() const;
};

//### AbanQ
inline double QSNumber::value() const
{
  return dVal();
}
inline int QSNumber::intValue() const
{
  return (int)dVal();
}
//### AbanQ

#include "qsoperations.h"

class QUICKCORE_EXPORT QSString : public QSObject
{
public:
  QSString(const QSEnv *env, const QString &s);
  QString value() const;

  //### AbanQ
  // ## strictly speaking not related to QSString
  static QString from(double d) {
    // ### -0
    if (QS::isNaN(d))
      return QString::fromLatin1("NaN");
    else if (QS::isInf(d))
      return d > 0 ? QString::fromLatin1("+Infinity") : QString::fromLatin1("-Infinity");

    return QString::number(d, 'G', 16);
  }
  static double toDouble(const QString &s) {
    // ### hex, Infinity
    bool ok;
    double d = s.toDouble(&ok);
    if (!ok) {
      if (s.stripWhiteSpace().isEmpty()) {
        return 0;
      }
      return NaN;
    }
    return d;
  }
  static ulong toULong(const QString &s, bool *ok = 0) {
    double d = QSString::toDouble(s);
    bool b = TRUE;

    if (QS::isNaN(d) || d != ulong(d)) {
      b = FALSE;
      d = 0;
    }

    if (ok)
      *ok = b;

    return ulong(d);
  }
  //### AbanQ
};

class QUICKCORE_EXPORT QSArray : public QSObject
{
public:
  QSArray(const QSEnv *env);
};

#endif
