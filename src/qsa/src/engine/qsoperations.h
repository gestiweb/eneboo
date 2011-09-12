/****************************************************************************
** $Id: qsoperations.h  1.1.5   edited 2006-02-23T15:39:57$
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

#ifndef QSOPERATIONS_H
#define QSOPERATIONS_H

#include "qsobject.h"

  //### AbanQ
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifndef HAVE_FLOAT_H   /* just for !Windows */
#define HAVE_FLOAT_H 0
#define HAVE_FUNC__FINITE 0
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#ifndef HAVE_FUNC_ISINF
#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif
#endif /* HAVE_FUNC_ISINF */

#ifdef HAVE_FLOAT_H
#include <float.h>
#endif

#if defined (__APPLE__) || defined (__FREEBSD__)
extern "C" {
  int finite(double);
}
#endif
//### AbanQ

namespace QS
{

  //### AbanQ


  /**
   * @return True if d is not a number (platform support required).
   */
  inline bool isNaN(double d)
  {
    uchar *ch = (uchar *)&d;
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    return (ch[0] & 0x7f) == 0x7f && ch[1] > 0xf0;
#else
    return (ch[7] & 0x7f) == 0x7f && ch[6] > 0xf0;
#endif
  }


  /**
   * @return True if d is infinite (platform support required).
   */
  inline bool isInf(double d)
  {
#if defined(HAVE_FUNC_ISINF)
    return isinf(d);
#elif defined(HAVE_FUNC_FINITE) || defined(__APPLE__)
    return finite(d) == 0 && d == d;
#elif defined(HAVE_FUNC__FINITE)
    return
#  if defined( Q_CC_BOR ) // Crashes with out this test...
      !isNaN(d) &&
#  endif
      _finite(d) == 0 && d == d;
#else
    return FALSE;
#endif
  }

  /**
   * This operator performs an abstract relational comparision of the two
   * arguments that can be of arbitrary type. If possible, conversions to the
   * string or number type will take place before the comparison.
   *
   * @return 1 if v1 is "less-than" v2, 0 if the relation is "greater-than-or-
   * equal". -1 if the result is undefined.
   */
  inline double max(double d1, double d2)
  {
    /* TODO: check for NaN */
    return (d1 > d2) ? d1 : d2;
  }
  inline double min(double d1, double d2)
  {
    /* TODO: check for NaN */
    return (d1 < d2) ? d1 : d2;
  }

  /**
   * Additive operator. Either performs an addition or substraction of v1
   * and v2.
   * @param oper '+' or '-' for an addition or substraction, respectively.
   * @return The result of the operation.
   */
  QSObject add(const QSEnv *env, const QSObject &v1, const QSObject &v2, char oper);

  /**
   * Multiplicative operator. Either multiplies/divides v1 and v2 or
   * calculates the remainder from an division.
   * @param oper '*', '/' or '%' for a multiplication, division or
   * modulo operation.
   * @return The result of the operation.
   */
  QSObject mult(const QSEnv *env, const QSObject &v1, const QSObject &v2, char oper);
};

#endif
