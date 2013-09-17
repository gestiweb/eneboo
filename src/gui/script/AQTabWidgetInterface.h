/***************************************************************************
AQTabWidgetInterface.h
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

#ifndef AQTABWIDGETINTERFACE_H_
#define AQTABWIDGETINTERFACE_H_

#include <AQScript/AQComponentDBInterface.h>
#include <QTabWidget>

typedef QTabWidget AQTabWidget;

#ifdef AQ_OBJ_FACTORY_IMPL
static void deleteSharedAQTabWidget( AQTabWidget * obj )
{
  if ( !obj->parent() )
    obj->deleteLater();
}
#endif

class AQTabWidgetInterfaceAQ2 : public AQComponentDBInterface
{
  Q_OBJECT

public:

  QSharedPointer<AQTabWidget> obj_;

public Q_SLOTS:

  void setTabEnabled( int, bool )
  {
    AQ_NO_PORT_AQ2( AQTabWidget::setTabEnabled( int, bool ) );
  }
  void setTabEnabled( const QString &, bool )
  {
    AQ_NO_PORT_AQ2( AQTabWidget::setTabEnabled( const QString &, bool ) );
  }
  void showPage( int )
  {
    AQ_NO_PORT_AQ2( AQTabWidget::showPage( int ) );
  }
  void showPage( const QString & )
  {
    AQ_NO_PORT_AQ2( AQTabWidget::showPage( QString & ) );
  }
  void emitCurrentChanged( QWidget * )
  {
    AQ_NO_PORT_AQ2( AQTabWidget::emitCurrentChanged( QWidget * ) );
  }

Q_SIGNALS:

  void currentChanged( const QString & );

protected:

  friend class AQUiObjectFactoryQSA;

  /**
  Constructor
  */
  AQTabWidgetInterfaceAQ2( const AQScriptEngineQSA * eng,
                           QSharedPointer<AQTabWidget> aqTabWidget );

  /**
  Destructor
  */
  virtual ~AQTabWidgetInterfaceAQ2();
};

#endif /*AQTABWIDGETINTERFACE_H_*/
