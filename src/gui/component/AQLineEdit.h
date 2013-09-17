/***************************************************************************
                          AQLineEdit.h
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

#ifndef AQLINEEDIT_H_
#define AQLINEEDIT_H_

#include <QLineEdit>
#include <QSpinBox>

class AQLineEdit: public QLineEdit
{
  Q_OBJECT

public:

  AQLineEdit( QWidget * parent = 0 );
  int type;

protected:

  void focusOutEvent( QFocusEvent * f );
  void focusInEvent( QFocusEvent * f );
};

class AQSpinBox: public QSpinBox
{

public:

  AQSpinBox( QWidget * parent = 0 ) :
      QSpinBox( parent )
  {
    setAlignment( Qt::AlignRight );
  }
};

#endif /*AQLINEEDIT_H_*/
