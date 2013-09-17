/***************************************************************************
AQMainWindowInterface.h
-------------------
begin                : 09/02/2009
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

#ifndef AQMAINWINDOWINTERFACE_H_
#define AQMAINWINDOWINTERFACE_H_

#include <AQScript/AQComponentDBInterface.h>
#include <AQUi/AQMainWindow.h>

#ifdef AQ_OBJ_FACTORY_IMPL
static void deleteSharedAQMainWindow( AQMainWindow * obj )
{
  if ( !obj->QObject::parent() )
    obj->deleteLater();
}
#endif

class AQFormDBInterface;
class AQActionInterface;

/**
Intefaz para AQMainWindow
*/ 
/**
Ventanas principales de AbanQ
 
Una ventana principal está asociada a la conexión de una
base de datos. Es el contenedor principal donde se muestran
los componentes de la interfaz de usuario.
 
Ofrece varios métodos para manejar la acciones de
AbanQ (AQAction), menús, formularios, etc..
 
@author InfoSiAL S.L.
*/
class AQMainWindowInterface : public AQComponentDBInterface
{
  Q_OBJECT

public:

  QSharedPointer<AQMainWindow> obj_;

public Q_SLOTS:

  /**
  Activa la acción indicada abriendo el formulario asociado en una nueva pestaña

  @param idAction Nombre de la accion a activar. A partir de él construye el objeto 
                  AQAction correspondiente
  @return El formulario abierto. La ventana principal mantiene la propiedad sobre el
          formulario y puede ser invalidado en cualquier momento.
  */
  AQFormDBInterface * activateAction( const QString & idAction );

  /**
  Hace lo mismo que el método anterior pero acepta un objeto AQAction ya construido
  en vez del nombre de la acción.

  La ventana principal toma en propiedad el objeto AQAction pasado como parámetro y
  puede ser invalidado en cualquier momento
  */
  AQFormDBInterface * activateAction( AQActionInterface * aqAction );

protected:

  friend class AQUiObjectFactoryQSA;

  /**
  Constructor
  */
  AQMainWindowInterface( const AQScriptEngineQSA * eng,
                         QSharedPointer<AQMainWindow> aqMainWindow );

  /**
  Destructor
  */
  virtual ~AQMainWindowInterface();
};

#endif /*AQMAINWINDOWINTERFACE_H_*/
