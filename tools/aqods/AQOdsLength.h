/***************************************************************************
 AQOdsLength.h
 -------------------
 begin                : 13/12/2011
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

#ifndef AQODSLENGTH_H_
#define AQODSLENGTH_H_

#include "odf-gen/length.h"

class AQOdsLength : public Length
{
public:
  AQOdsLength(double value) : Length(value) {}

  virtual QString value() const {
    return str();
  }
};

class AQOdsCentimeters : public AQOdsLength
{
public:
  AQOdsCentimeters(double value) : AQOdsLength(value) {}

  QString value() const {
    return str();
  }

  const char *unit() const {
    return "cm";
  }
};

#endif /* AQODSLENGTH_H_ */
