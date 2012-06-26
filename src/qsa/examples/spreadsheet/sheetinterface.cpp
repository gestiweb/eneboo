#include "sheetinterface.h"
#include <qtable.h>
#include "spreadsheet.h"

SheetInterface::SheetInterface( QTable *t, SpreadSheet *s, const char *name )
	: QObject( t, name ), sheet( t ), spreadSheet( s )
{
    connect( t, SIGNAL( currentChanged( int, int ) ),
	     this, SIGNAL( currentChanged( int, int ) ) );
}

int SheetInterface::numSelections() const
{
    return sheet->numSelections();
}

void SheetInterface::setText( int row, int col, const QString &val )
{
    sheet->setText( row, col, val );
}

QString SheetInterface::text( int row, int col ) const
{
    return sheet->text( row, col );
}

QRect SheetInterface::selection( int num ) const
{
    QTableSelection s( sheet->selection( num ) );
    QRect r;
    r.setLeft( s.leftCol() );
    r.setTop( s.topRow() );
    r.setRight( s.rightCol() );
    r.setBottom( s.bottomRow() );
    return r;
}

void SheetInterface::setCurrentRow( int r )
{
    sheet->setCurrentCell( r, sheet->currentColumn() );
}

void SheetInterface::setCurrentColumn( int c )
{
    sheet->setCurrentCell( sheet->currentRow(), c );
}

int SheetInterface::currentRow() const
{
    return sheet->currentRow();
}

int SheetInterface::currentColumn() const
{
    return sheet->currentColumn();
}

int SheetInterface::numRows() const
{
    return sheet->numRows();
}

int SheetInterface::numCols() const
{
    return sheet->numCols();
}
