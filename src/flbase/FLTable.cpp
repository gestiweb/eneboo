/***************************************************************************
 FLTable.cpp
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

#include "FLTable.h"

FLTable::FLTable(QWidget * parent, const char * name) :
  QTable(parent, name)
{
}

void FLTable::paintCell(QPainter * p, int row, int col, const QRect & cr, bool selected, const QColorGroup & cg)
{
  if (!selected) {
    QString key(QString("%1,%2").arg(row).arg(col));

    bool colorChanges = cellBgColors_.contains(key);
    bool alignmentChanges = cellAlignments_.contains(key);
    if (colorChanges || alignmentChanges) {

      QTableItem * itm = item(row, col);

      if (itm) {
        QTable::paintCell(p, row, col, cr, selected, cg);

        QBrush bu;
        if (colorChanges) {
          bu.setColor(cellBgColors_[key]);
          bu.setStyle(SolidPattern);
        }

        p->setPen(selected ? cg.highlightedText() : cg.text());
        if (colorChanges) {
          p->fillRect(1, 1, cr.width() - 2, cr.height() - 2, bu);
        }
        int cAlign;
        if (alignmentChanges) {
          cAlign = cellAlignments_[key];
        } else {
          cAlign = itm->alignment();
        }
        p->drawText(2, 2, cr.width() - 4, cr.height() - 4, itm->wordWrap() ? (cAlign | Qt::WordBreak) : cAlign,
                    itm->text());

        return;
      }
    }
  }

  QTable::paintCell(p, row, col, cr, selected, cg);
}

void FLTable::setCellBackgroundColor(int row, int col, const QColor & color)
{
  QString key(QString("%1,%2").arg(row).arg(col));
  cellBgColors_[key] = color;
}

void FLTable::setCellAlignment(int row, int col, const int a)
{
  QString key(QString("%1,%2").arg(row).arg(col));
  cellAlignments_[key] = a;
}

// [d5] Versión 1 para pintado de celda. DC 17/03/2010
/*
void FLTable::chgCellBackgroundColor( int row, int col, const QColor & color , const FLTable & obj) {
  setCellBackgroundColor( row,col,color );
	QPainter p( this ); // Primera prueba para declaración de QPainter. En teoría This es el QTable...
	//QPainter p( parent ); // Probamos con parent...
	const QColorGroup &cg  = colorGroup();
	
	QRect cr = obj.cellGeometry( row, col );			
	bool selected = obj.isSelected( row, col );
	
	paintCell( p, row, col, cr, selected, &cg );
}
*/
