/***************************************************************************
AQCodBarInterface.h
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

#ifndef AQCODBARINTERFACE_H_
#define AQCODBARINTERFACE_H_

#include <AQScript/AQObjectInterfaceQSA.h>
#include <AQbarcode/AQCodBar.h>

#ifdef AQ_OBJ_FACTORY_IMPL
static void deleteSharedAQCodBar( AQCodBar * obj )
{
  if ( !obj->QObject::parent() )
    obj->deleteLater();
}
#endif

class AQCodBarInterfaceAQ2 : public AQObjectInterfaceQSA
{
  Q_OBJECT

public:

  QSharedPointer<AQCodBar> obj_;

public Q_SLOTS:

  QPixmap pixmap()
  {
    AQ_NO_PORT_AQ2( AQCodBar::pixmap() );
    return QPixmap();
  }
  QPixmap pixmapError()
  {
    AQ_NO_PORT_AQ2( AQCodBar::pixmapError() );
    return QPixmap();
  }
  QString typeToName( int )
  {
    AQ_NO_PORT_AQ2( AQCodBar::typeToName( int ) );
    return QString();
  }
  QString value()
  {
    AQ_NO_PORT_AQ2( AQCodBar::value() );
    return QString();
  }
  int type()
  {
    AQ_NO_PORT_AQ2( AQCodBar::type() );
    return 0;
  }
  int margin()
  {
    AQ_NO_PORT_AQ2( AQCodBar::margin() );
    return 0;
  }
  double scale()
  {
    AQ_NO_PORT_AQ2( AQCodBar::scale() );
    return 0;
  }
  double cut()
  {
    AQ_NO_PORT_AQ2( AQCodBar::cut() );
    return 0;
  }
  bool text()
  {
    AQ_NO_PORT_AQ2( AQCodBar::text() );
    return true;
  }
  int rotation()
  {
    AQ_NO_PORT_AQ2( AQCodBar::rotation() );
    return 0;
  }
  QColor fg()
  {
    AQ_NO_PORT_AQ2( AQCodBar::fg() );
    return QColor();
  }
  QColor bg()
  {
    AQ_NO_PORT_AQ2( AQCodBar::bg() );
    return QColor();
  }
  int res()
  {
    AQ_NO_PORT_AQ2( AQCodBar::res() );
    return 0;
  }
  bool validBarcode()
  {
    AQ_NO_PORT_AQ2( AQCodBar::validBarcode() );
    return true;
  }
  void setCaption( const QString & )
  {
    AQ_NO_PORT_AQ2( AQCodBar::setCaption( const QString & ) );
  }
  QString caption()
  {
    AQ_NO_PORT_AQ2( AQCodBar::caption() );
    return QString();
  }
  void setValue( const QString & )
  {
    AQ_NO_PORT_AQ2( AQCodBar::setValue( const QString & ) );
  }
  void setType( int )
  {
    AQ_NO_PORT_AQ2( AQCodBar::setType( int ) );
  }
  void setMargin( int )
  {
    AQ_NO_PORT_AQ2( AQCodBar::setMargin( int ) );
  }
  void setScale( double )
  {
    AQ_NO_PORT_AQ2( AQCodBar::setScale( double ) );
  }
  void setCut( double )
  {
    AQ_NO_PORT_AQ2( AQCodBar::setCut( double ) );
  }
  void setText( bool )
  {
    AQ_NO_PORT_AQ2( AQCodBar::setText( bool ) );
  }
  void setRotation( int )
  {
    AQ_NO_PORT_AQ2( AQCodBar::setRotation( int ) );
  }
  void setFg( const QColor & )
  {
    AQ_NO_PORT_AQ2( AQCodBar::setFg( const QColor & ) );
  }
  void setBg( const QColor & )
  {
    AQ_NO_PORT_AQ2( AQCodBar::setBg( const QColor & ) );
  }
  void setRes( int )
  {
    AQ_NO_PORT_AQ2( AQCodBar::setRes( int ) );
  }
  int nameToType( const QString & )
  {
    AQ_NO_PORT_AQ2( AQCodBar::nameToType( const QString & ) );
    return 0;
  }
  void cleanUp()
  {
    AQ_NO_PORT_AQ2( AQCodBar::cleanUp() );
  }
  AQCodBar * obj()
  {
    AQ_NO_PORT_AQ2( AQCodBar::obj() );
    return 0;
  }

protected:

  friend class AQUiObjectFactoryQSA;

  /**
  Constructor
  */
  AQCodBarInterfaceAQ2( const AQScriptEngineQSA * eng, const QString & value,
                        int type, int margin, double scale, double cut, int rotation, bool text_flag = false,
                        QColor fg = Qt::black, QColor bg = Qt::white, int res = 72 );

  /**
  Constructor
  */
  AQCodBarInterfaceAQ2( const AQScriptEngineQSA * eng,
                        QSharedPointer<AQCodBar> aqCodBar );

  /**
  Destructor
  */
  virtual ~AQCodBarInterfaceAQ2();
};

#endif /*AQCODBARINTERFACE_H_*/
