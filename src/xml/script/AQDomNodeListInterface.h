/***************************************************************************
AQDomNodeListInterface.h
-------------------
begin                : 03/03/2009
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

#ifndef AQDOMNODELISTINTERFACE_H_
#define AQDOMNODELISTINTERFACE_H_

#include <AQScript/AQObjectInterfaceQSA.h>
#include <QDomNodeList>

class AQDomNodeInterface;

class AQDomNodeListInterface : public AQObjectInterfaceQSA
{
  Q_OBJECT

public:

  QDomNodeList obj_;

public Q_SLOTS:

  AQDomNodeInterface * item( int )
  {
    AQ_NO_PORT_AQ2( AQDomNodeList::item( int ) );
    return 0;
  }
  uint length() const
  {
    AQ_NO_PORT_AQ2( AQDomNodeList::length() const );
    return 0;
  }
  uint count() const
  {
    AQ_NO_PORT_AQ2( AQDomNodeList::count() const );
    return 0;
  }
  QDomNodeList obj()
  {
    AQ_NO_PORT_AQ2( AQDomNodeList::obj() );
    return QDomNodeList();
  }

protected:

  friend class AQXmlObjectFactoryQSA;

  /**
  Constructor
  */
  AQDomNodeListInterface( const AQScriptEngineQSA * eng,
                          const QDomNodeList & nl = QDomNodeList() );
};

#endif /*AQDOMNODELISTINTERFACE_H_*/
