/***************************************************************************
                        FLAccessControlFactory.h
                          -------------------
 begin                : Fri Oct 10 2005
 copyright            : (C) 2004-2005 by InfoSiAL S.L.
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

#ifndef FLACCESSCONTROLFACTORY_H
#define FLACCESSCONTROLFACTORY_H

class FLAccessControl;

/**
Ofrece un mecanismo para fabricar objetos del tipo FLAccessControl específicos
para el control de acceso de objetos de alto nivel de distintos tipos, mediante
FLAccessControlFactory::create.

También permite determinar mediante FLAccessControlFactory::type, si a un tipo de objeto
de alto nivel se le puede controlar su acceso, es decir, está definida la clase derivada
de FLAccessControl que se encarga del control de dicho tipo de objeto.

Para poder fabricar un objeto de control de acceso que se encarge de un nuevo tipo de objetos
de alto nivel, la clase correspondiente deberá estar registrada. En general, este registro se
realiza siguiendo estos pasos:

- Declarar la clase que hereda de forma pública de FLAccessControl, y que se encargará del control
de acceso del nuevo tipo de objetos de alto nivel.

- Definir los métodos FLAccessControl::type y FLAccessControl::processObject , para devolver
el nombre del tipo de objetos de alto nivel de los que se encarga y para implementar el proceso
que aplicará la regla de control de acceso.

- Incluir una nueva entrada en FLAccessControlFactory::create para saber como crear objetos que controlarán
el acceso de este nuevo tipo de objetos de alto nivel.

- Incluir una nueva entrada en FLAccessControlFactory::type para poder determinar que al nuevo tipo de objeto
de alto nivel es posible controlar su acceso.

Como ejemplo se puede observar la implementación de FLAccessControlMainWindow, que se encarga del control de
acceso de ventanas principales.

IMPORTANTE : Los objetos de alto nivel deben heredar de QObject.

@author InfoSiAL S.L.
*/
class FL_EXPORT FLAccessControlFactory
{

public:

  /**
  Crea un objeto controlador del acceso para objetos de alto nivel del tipo dado.

  @param type Tipo del objeto de alto nivel del que se solicita un controlador de acceso.
  @return Un nuevo objeto que puede controlar el acceso del objeto dado o 0 si no pudo crearlo.
  */
  static FLAccessControl *create(const QString &type);

  /**
  Dado un objeto general (tipo QObject) de alto nivel, identifica si existe un controlador que puede controlar
  su acceso devolviendo en tal caso el nombre de tipo asignado.

  @param Objeto de alto nivel del cual se quiere conocer su tipo.
  @return Devuelve el nombre del tipo asociado al objeto de alto nivel dado, si existe controlador de acceso para él,
  en caso contrario devuelve una cadena vacía.
  */
  static QString type(QObject *obj);
};

#endif
