/***************************************************************************
AQAccessControlFactory.h
-------------------
begin                : 09/08/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
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

#ifndef AQACCESSCONTROLFACTORY_H_
#define AQACCESSCONTROLFACTORY_H_

#include <QString>

class QObject;
class AQAccessControl;

/**
Ofrece un mecanismo para fabricar objetos del tipo AQAccessControl específicos
para el control de acceso de objetos de alto nivel de distintos tipos, mediante
AQAccessControlFactory::create.
 
También permite determinar mediante AQAccessControlFactory::type, si a un tipo de objeto
de alto nivel se le puede controlar su acceso, es decir, está definida la clase derivada
de AQAccessControl que se encarga del control de dicho tipo de objeto.
 
Para poder fabricar un objeto de control de acceso que se encarge de un nuevo tipo de objetos
de alto nivel, la clase correspondiente deberá estar registrada. En general, este registro se
realiza siguiendo estos pasos:
 
- Declarar la clase que hereda de forma pública de AQAccessControl, y que se encargará del control
de acceso del nuevo tipo de objetos de alto nivel.
 
- Definir los métodos AQAccessControl::type y AQAccessControl::processObject , para devolver
el nombre del tipo de objetos de alto nivel de los que se encarga y para implementar el proceso
que aplicará la regla de control de acceso.
 
- Incluir una nueva entrada en AQAccessControlFactory::create para saber como crear objetos que controlarán
el acceso de este nuevo tipo de objetos de alto nivel.
 
- Incluir una nueva entrada en AQAccessControlFactory::type para poder determinar que al nuevo tipo de objeto
de alto nivel es posible controlar su acceso.
 
Como ejemplo se puede observar la implementación de AQAccessControlMainWindow, que se encarga del control de
acceso de ventanas principales.
 
IMPORTANTE : Los objetos de alto nivel deben heredar de QObject.
 
@author InfoSiAL S.L.
*/
class AQAccessControlFactory
{
public:

  /**
  Crea un objeto controlador del acceso para objetos de alto nivel del tipo dado.

  @param  type  Tipo del objeto de alto nivel del que se solicita un controlador de acceso.
  @return Un nuevo objeto que puede controlar el acceso del objeto dado o 0 si no pudo crearlo.
  */
  static AQAccessControl * create( const QString & type );

  /**
  Dado un objeto general (tipo QObject) de alto nivel, identifica si existe un controlador que puede controlar
  su acceso devolviendo en tal caso el nombre de tipo asignado.

  @param  obj Objeto de alto nivel del cual se quiere conocer su tipo.
  @return Devuelve el nombre del tipo asociado al objeto de alto nivel dado, si existe controlador de acceso para él,
          en caso contrario devuelve una cadena vacía.
  */
  static QString type( QObject * obj );
};

#endif /*AQACCESSCONTROLFACTORY_H_*/
