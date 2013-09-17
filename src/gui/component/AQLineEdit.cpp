/***************************************************************************
AQLineEdit.cpp
-------------------
begin                : 29/08/2007
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

#include "AQLineEdit.h"

#include <QValidator>

AQLineEdit::AQLineEdit( QWidget * parent ) :
    QLineEdit( parent )
{}

void AQLineEdit::focusOutEvent( QFocusEvent * f )
{
  QString s = text();
#ifndef Q_OS_WIN32
  const QValidator *v = validator();
  if ( v ) {
    v->fixup( s );
    setText( s );
  }
#endif
  QLineEdit::focusOutEvent( f );
}

void AQLineEdit::focusInEvent( QFocusEvent * f )
{
  if ( selectedText().isEmpty() )
    selectAll();
  QLineEdit::focusInEvent( f );
}
