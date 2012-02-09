/***************************************************************************
 AQOdsStyle.h
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

#ifndef AQODSSTYLE_H_
#define AQODSSTYLE_H_

#include "odf-gen/style.h"

class AQOdsStyle : public Style
{
public:
  AQOdsStyle(uint flags = Style::NONE)
    : Style((Style::StyleFlags)flags) {}
};

#endif /* AQODSSTYLE_H_ */
