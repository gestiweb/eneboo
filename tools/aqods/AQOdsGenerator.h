/***************************************************************************
 AQOdsGenerator.h
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

#ifndef AQODSGENERATOR_H_
#define AQODSGENERATOR_H_

#include <sstream>
#include <qstring.h>

class AQOdsGenerator
{
public:
  AQOdsGenerator() {}

  std::stringstream &strStream() {
    return strStream_;
  }

  void clear() {
    strStream_.str("");
  }

  QString str() const {
    return strStream_.str();
  }

  bool generateOds(const QString &fileNameOut) const;

private:
  std::stringstream strStream_;
};

#endif /* AQODSGENERATOR_H_ */
