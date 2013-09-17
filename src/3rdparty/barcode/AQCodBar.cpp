/***************************************************************************
AQCodBar.cpp
-------------------
begin                : 07/08/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
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

#include "AQCodBar.h"
#include "src/barcode.h"

AQCodBar::AQCodBar( barcodeData * data )
{
  copyBarcode( data, &barcode );
}

AQCodBar::AQCodBar( const QString & value, int type, int margin, double scale, double cut,
                    int rotation, bool text_flag, QColor fg, QColor bg, int res ) :
    proc( 0 ),
    readingStdout( false ),
    writingStdout( false )
{
  barcode.value = value;
  barcode.type = type;
  barcode.margin = margin;
  barcode.scale = scale;
  barcode.cut = cut;
  barcode.rotation = rotation;
  barcode.text = text_flag;
  barcode.fg = fg;
  barcode.bg = bg;
  barcode.valid = false;
  barcode.res = res;
}

AQCodBar::AQCodBar() :
    proc( 0 ),
    readingStdout( false ),
    writingStdout( false )
{}

AQCodBar::~AQCodBar()
{}
