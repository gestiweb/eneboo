/***************************************************************************
AQUiInterface.h
-------------------
begin                : 26/02/2009
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

#ifndef AQUIINTERFACE_H_
#define AQUIINTERFACE_H_

#include <QObject>

#include <AQUi/AQUi.h>

/**
Interfaz estática para módulo AQUi
*/
class AQUiInterfaceStatic : public QObject
{
  Q_OBJECT

  Q_ENUMS( DialogCode )

  Q_PROPERTY( QString objectName READ objectName WRITE setObjectName SCRIPTABLE false )

public:

  /**
  Codigos de respuesta de cuadros de dialogos
  */
  enum DialogCode {
    Rejected = AQUi::Rejected,
    Accepted = AQUi::Accepted,
    NoDialogCode = AQUi::NoDialogCode
  };
};

#endif /*AQUIINTERFACE_H_*/
