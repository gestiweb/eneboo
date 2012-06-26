/***************************************************************************
 AQOdsImage.h
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

#ifndef AQODSIMAGE_H_
#define AQODSIMAGE_H_

#include "odf-gen/image.h"

class AQOdsImage : public Image
{
public:
  AQOdsImage(const QString &name,
             const AQOdsLength &width,
             const AQOdsLength &height,
             const AQOdsLength &x,
             const AQOdsLength &y,
             const QString &link)
    : Image(name, width, height, x, y, link) {}
};

MAP_ODS_TYPE(AQOdsImage, image);

#endif /* AQODSIMAGE_H_ */
