/***************************************************************************
AQ3TableInterfaceAQ2.h
-------------------
begin                : 27/02/2009
copyright            : (C) 2003-2009 by InfoSiAL S.L.
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

#ifndef AQ3TABLEINTERFACE_H_
#define AQ3TABLEINTERFACE_H_

#include <AQScript/AQComponentDBInterface.h>
#include <Q3Table>

typedef Q3Table AQ3Table;

#ifdef AQ_OBJ_FACTORY_IMPL
static void deleteSharedAQ3Table( AQ3Table * obj )
{
  if ( !obj->parent() )
    obj->deleteLater();
}
#endif

class AQ3TableInterfaceAQ2 : public AQComponentDBInterface
{
  Q_OBJECT

public:

  QSharedPointer<AQ3Table> obj_;

public Q_SLOTS:

  void setRowReadOnly( int, bool )
  {
    AQ_NO_PORT_AQ2( AQ3Table::setRowReadOnly( int, bool ) );
  }
  void setColumnReadOnly( int, bool )
  {
    AQ_NO_PORT_AQ2( AQ3Table::setColumnReadOnly( int, bool ) );
  }
  void removeRow( int )
  {
    AQ_NO_PORT_AQ2( AQ3Table::removeRow( int ) );
  }
  void editCell( int, int )
  {
    AQ_NO_PORT_AQ2( AQ3Table::editCell( int, int ) );
  }
  void hideColumn( int )
  {
    AQ_NO_PORT_AQ2( AQ3Table::hideColumn( int ) );
  }
  QString text( int, int )
  {
    AQ_NO_PORT_AQ2( AQ3Table::text( int, int ) );
    return QString();
  }
  void setColumnLabels( const QString &, const QString & )
  {
    AQ_NO_PORT_AQ2( AQ3Table::setColumnLabels( const QString &, const QString & ) );
  }
  void setRowLabels( const QString &, const QString & )
  {
    AQ_NO_PORT_AQ2( AQ3Table::setRowLabels( const QString &, const QString & ) );
  }
  void setNumCols( int )
  {
    AQ_NO_PORT_AQ2( AQ3Table::setNumCols( int ) );
  }
  void setColumnWidth( int, int )
  {
    AQ_NO_PORT_AQ2( AQ3Table::setColumnWidth( int, int ) );
  }
  int currentRow()
  {
    AQ_NO_PORT_AQ2( AQ3Table::currentRow() );
    return 0;
  }
  int currentColumn()
  {
    AQ_NO_PORT_AQ2( AQ3Table::currentColumn() );
    return 0;
  }
  void selectRow( int )
  {
    AQ_NO_PORT_AQ2( AQ3Table::selectRow( int ) );
  }
  int numRows()
  {
    AQ_NO_PORT_AQ2( AQ3Table::numRows() );
    return 0;
  }
  int numCols()
  {
    AQ_NO_PORT_AQ2( AQ3Table::numCols() );
    return 0;
  }
  void setText( int, int, const QString & )
  {
    AQ_NO_PORT_AQ2( AQ3Table::setText( int, int, const QString & ) );
  }
  void insertRows( int, int = 1 )
  {
    AQ_NO_PORT_AQ2( AQ3Table::insertRows( int, int = 1 ) );
  }
  void insertColumns( int, int = 1 )
  {
    AQ_NO_PORT_AQ2( AQ3Table::insertColumns( int, int = 1 ) );
  }
  void adjustColumn( int )
  {
    AQ_NO_PORT_AQ2( AQ3Table::adjustColumn( int ) );
  }
  void adjustRow( int )
  {
    AQ_NO_PORT_AQ2( AQ3Table::adjustRow( int ) );
  }
  void emitCurrentChanged( int, int )
  {
    AQ_NO_PORT_AQ2( AQ3Table::emitCurrentChanged( int, int ) );
  }
  void emitClicked( int, int, int, const QPoint & )
  {
    AQ_NO_PORT_AQ2( AQ3Table::emitClicked( int, int, int, const QPoint & ) );
  }
  void emitDoubleClicked( int, int, int, const QPoint & )
  {
    AQ_NO_PORT_AQ2( AQ3Table::emitDoubleClicked( int, int, int, const QPoint & ) );
  }
  void emitValueChanged( int, int )
  {
    AQ_NO_PORT_AQ2( AQ3Table::emitValueChanged( int, int ) );
  }

Q_SIGNALS:

  void currentChanged( int, int );
  void clicked( int, int );
  void doubleClicked( int, int );
  void valueChanged( int, int );

protected:

  friend class AQUiObjectFactoryQSA;

  /**
  Constructor
  */
  AQ3TableInterfaceAQ2( const AQScriptEngineQSA * eng,
                       QSharedPointer<AQ3Table> aqTable );

  /**
  Destructor
  */
  virtual ~AQ3TableInterfaceAQ2();
};

#endif /*AQ3TABLEINTERFACE_H_*/
