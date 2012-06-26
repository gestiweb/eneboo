#ifndef SHEETINTERFACE_H
#define SHEETINTERFACE_H

#include <qobject.h>

class QTable;
class SpreadSheet;

class SheetInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY( int numSelections READ numSelections )
    Q_PROPERTY( int currentRow READ currentRow WRITE setCurrentRow )
    Q_PROPERTY( int currentColumn READ currentColumn WRITE setCurrentColumn )
    Q_PROPERTY( int numRows READ numRows )
    Q_PROPERTY( int numCols READ numCols )

public:
    SheetInterface( QTable *t, SpreadSheet *s, const char *name );

    int numSelections() const;
    void setCurrentRow( int r );
    void setCurrentColumn( int c );
    int currentRow() const;
    int currentColumn() const;
    int numRows() const;
    int numCols() const;

public slots:
    void setText( int row, int col, const QString &val );
    QString text( int row, int col ) const;
    QRect selection( int num ) const;

signals:
    void currentChanged( int row, int col );

private:
    QTable *sheet;
    SpreadSheet *spreadSheet;

};

#endif
