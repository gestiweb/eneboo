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
#include <fstream>
#include <qstring.h>

class AQOdsGenerator
{
public:
  AQOdsGenerator();
  ~AQOdsGenerator();

  std::stringstream &strStream() {
    return strStream_;
  }

  std::fstream &fileStream() {
    return fileStream_;
  }

  QString fileNameStream() const {
    return fileName_;
  }

  void clear();

  QString str();

  bool generateOds(const QString &fileNameOut);

private:
  std::stringstream strStream_;
  std::fstream fileStream_;
  QString fileName_;
};

#endif /* AQODSGENERATOR_H_ */
