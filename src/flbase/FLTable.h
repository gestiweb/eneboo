/***************************************************************************
 FLTable.h
 -------------------
 begin                : jue abr 23 2009
 copyright            : (C) 2001-2009 by InfoSiAL S.L.
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

#ifndef FLTABLE_H
#define FLTABLE_H

#include <qtable.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qmap.h>

#include "AQGlobal.h"

/**
 Clase que es una redefinicion de la clase QTable,
 especifica para las necesidades de AbanQ.

 @author InfoSiAL S.L.
 */
class AQ_EXPORT FLTable: public QTable {

  Q_OBJECT

public:

  /**
   constructor
   */
  FLTable( QWidget * parent = 0, const char * name = 0 );

  /**
   Establece el color de fondo para una celda
   */
  void setCellBackgroundColor( int row, int col, const QColor & color );
	
  /**
  [d5] Cambia el color de fondo para una celda  DC 16/03/2010
  */
  //void chgCellBackgroundColor( int row, int col, const QColor & color , const FLTable & obj);
  
  /**
   Establece el alineamiento de texto para una celda
   */
  void setCellAlignment( int row, int col, const int a);

protected:

  /**
   Redefinido por conveniencia para pintar la celda
   */
  void paintCell( QPainter * p, int row, int col, const QRect & cr, bool selected, const QColorGroup & cg );

private:

  /**
   Color de fondo de celdas
   */
  QMap<QString, QColor> cellBgColors_;

  /**
   Alineación de las celdas
   */
  QMap<QString, int> cellAlignments_;
};

#endif
