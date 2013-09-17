/***************************************************************************
AQ3ListViewItemInterface.h
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

#ifndef AQ3LISTVIEWITEMINTERFACE_H_
#define AQ3LISTVIEWITEMINTERFACE_H_

#include <AQScript/AQObjectInterfaceQSA.h>
#include <Q3ListViewItem>

class AQ3ListViewItem : public QObject, public Q3ListViewItem
{
  Q_OBJECT

public:

  AQ3ListViewItem( Q3ListView * parent ) :
      Q3ListViewItem( parent )
  {}
  AQ3ListViewItem( Q3ListViewItem * parent ) :
      Q3ListViewItem( parent )
  {}
  AQ3ListViewItem( AQ3ListViewItem * parent ) :
      QObject( parent ), Q3ListViewItem( parent )
  {}

public Q_SLOTS:

  void setKey( const QString & k )
  {
    key_ = k;
  }

  QString key() const
  {
    return key_;
  }

private:

  QString key_;
};

#ifdef AQ_OBJ_FACTORY_IMPL
static void deleteSharedAQ3ListViewItem( AQ3ListViewItem * obj )
{
  if ( !obj->QObject::parent() )
    obj->deleteLater();
}
#endif

class AQ3ListViewItemInterfaceAQ2 : public AQObjectInterfaceQSA
{
  Q_OBJECT

public:

  QSharedPointer<AQ3ListViewItem> obj_;

public Q_SLOTS:

  void setKey( const QString & )
  {
    AQ_NO_PORT_AQ2( AQ3ListViewItem::setKey( const QString & ) );
  }
  QString key() const
  {
    AQ_NO_PORT_AQ2( AQ3ListViewItem::key() const );
    return QString();
  }
  void setText( int, const QString & )
  {
    AQ_NO_PORT_AQ2( AQ3ListViewItem::setText( int, const QString & ) );
  }
  QString text( int ) const
  {
    AQ_NO_PORT_AQ2( AQ3ListViewItem::text( int ) const );
    return QString();
  }
  AQ3ListViewItem * obj()
  {
    AQ_NO_PORT_AQ2( AQ3ListViewItem::obj() );
    return 0;
  }
  void setObj( AQ3ListViewItem * )
  {
    AQ_NO_PORT_AQ2( AQ3ListViewItem::setObj( AQ3ListViewItem * ) );
  }
  void setOpen( bool )
  {
    AQ_NO_PORT_AQ2( AQ3ListViewItem::setOpen( bool ) );
  }
  AQ3ListViewItemInterfaceAQ2 * firstChild() const
  {
    AQ_NO_PORT_AQ2( AQ3ListViewItem::firstChild() const );
    return 0;
  }
  AQ3ListViewItemInterfaceAQ2 * nextSibling() const
  {
    AQ_NO_PORT_AQ2( AQ3ListViewItem::nextSibling() const );
    return 0;
  }
  AQ3ListViewItemInterfaceAQ2 * parent() const
  {
    AQ_NO_PORT_AQ2( AQ3ListViewItem::parent() const );
    return 0;
  }
  void del()
  {
    AQ_NO_PORT_AQ2( AQ3ListViewItem::del() );
  }
  void setExpandable( bool )
  {
    AQ_NO_PORT_AQ2( AQ3ListViewItem::setExpandable( bool ) );
  }
  bool isExpandable() const
  {
    AQ_NO_PORT_AQ2( AQ3ListViewItem::isExpandable() const );
    return true;
  }
  void setPixmap( int, const QVariant & )
  {
    AQ_NO_PORT_AQ2( AQ3ListViewItem::setPixmap( int, const QVariant & ) );
  }

protected:

  friend class AQUiObjectFactoryQSA;

  /**
  Constructor
  */
  AQ3ListViewItemInterfaceAQ2( const AQScriptEngineQSA * eng,
                               QSharedPointer<AQ3ListViewItem> aqListViewItem );

  /**
  Destructor
  */
  virtual ~AQ3ListViewItemInterfaceAQ2();
};

#endif /*AQ3LISTVIEWITEMINTERFACE_H_*/
