/***************************************************************************
AQ3ListViewInterface.h
-------------------
begin                : 28/02/2009
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

#ifndef AQ3LISTVIEWINTERFACE_H_
#define AQ3LISTVIEWINTERFACE_H_

#include <AQScript/AQComponentDBInterface.h>
#include <Q3ListView>

typedef Q3ListView AQ3ListView;

#ifdef AQ_OBJ_FACTORY_IMPL
static void deleteSharedAQ3ListView( AQ3ListView * obj )
{
  if ( !obj->parent() )
    obj->deleteLater();
}
#endif

class Q3ListViewItem;
class AQ3ListViewItemInterfaceAQ2;

class AQ3ListViewInterfaceAQ2 : public AQComponentDBInterface
{
  Q_OBJECT

public:

  QSharedPointer<AQ3ListView> obj_;

public Q_SLOTS:

  int addColumn( const QString &, int = -1 )
  {
    AQ_NO_PORT_AQ2( AQ3ListView::addColumn( const QString &, int = -1 ) );
    return 0;
  }
  void setColumnText( int, const QString & )
  {
    AQ_NO_PORT_AQ2( AQ3ListView::setColumnText( int, const QString & ) );
  }
  void setColumnAlignment( int, int )
  {
    AQ_NO_PORT_AQ2( AQ3ListView::setColumnAlignment( int, int ) );
  }
  Q3ListView * obj()
  {
    AQ_NO_PORT_AQ2( AQ3ListView::obj() );
    return 0;
  }
  void clear()
  {
    AQ_NO_PORT_AQ2( AQ3ListView::clear() );
  }
  AQ3ListViewItemInterfaceAQ2 * firstChild() const
  {
    AQ_NO_PORT_AQ2( AQ3ListView::firstChild() const );
    return 0;
  }
  void emitDoubleClicked( Q3ListViewItem * )
  {
    AQ_NO_PORT_AQ2( AQ3ListView::emitDoubleClicked( Q3ListViewItem * ) );
  }
  void emitExpanded( Q3ListViewItem * )
  {
    AQ_NO_PORT_AQ2( AQ3ListView::emitExpanded( Q3ListViewItem * ) );
  }
  void emitSelectionChanged( Q3ListViewItem * )
  {
    AQ_NO_PORT_AQ2( AQ3ListView::emitSelectionChanged( Q3ListViewItem * ) );
  }

Q_SIGNALS:

  void doubleClicked( AQ3ListViewItemInterfaceAQ2 * );
  void expanded( AQ3ListViewItemInterfaceAQ2 * );
  void selectionChanged( AQ3ListViewItemInterfaceAQ2 * );

protected:

  friend class AQUiObjectFactoryQSA;

  /**
  Constructor
  */
  AQ3ListViewInterfaceAQ2( const AQScriptEngineQSA * eng,
                           QSharedPointer<AQ3ListView> aqListView );

  /**
  Destructor
  */
  virtual ~AQ3ListViewInterfaceAQ2();
};

#endif /*AQ3LISTVIEWINTERFACE_H_*/
