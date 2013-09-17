/***************************************************************************
AQAccessControlTable.h
-------------------
begin                : 28/01/2009
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

#ifndef AQACCESSCONTROLTABLE_H_
#define AQACCESSCONTROLTABLE_H_

#include <AQAcl/AQAccessControl.h>

class AQAccessControlTable : public AQAccessControl
{
public:

  /**
  @return El tipo del que se encarga; "table".
  */
  QString type() const;

  /**
  Procesa objetos que son de la clase AQTableMD.

  Sólo controla los objetos que pertenecen a él de la clase AQFieldMD
  y sólo permite hacerlos no visibles o no editables. Esto en la práctica
  significa que los componentes de los formularios de la clase AQFieldDB y
  AQTableDB utilizarán los metadatos para autoconfigurarse y serán no visibles
  o no editables si así lo establecen los objetos AQTableMD y AQFieldMD
  que definen esos metadatos.

  Los permisos que acepta son :

  - "-w" o "--" (no_lectura/escritura o no_lectura/no_escritura) -> no visible
  - "r-" (lectura/no_escritura) -> no editable

  Esto permite que cualquier componente de un formulario de AbanQ del tipo AQFieldDB o 
  AQTableDB sea no visible o no editable a partir de los metadatos que utiliza,
  independientemente del formulario en que se encuentre. Por ejemplo, si definimos
  que el campo "nombre" de la tabla "clientes" es no visible, implicará que cada vez
  que un componente tipo AQFieldDB forme parte de un formulario y esté enlazando con el campo
  "nombre" de "clientes" su contenido sea no visible para el usuario.
  */
  void processObject( void * obj ) const;

  /**
  Crea la regla la regla de control de acceso a partir de objetos de la clase AQTableMD.

  Al igual que en AQAccessControlTable::processObject() sólo tiene en cuenta
  las propiedades 'visible' y 'editable' de los campos, a partir de las cuales
  crea la regla.
  */
  void setFromObject( void * obj );
};

#endif /*AQACCESSCONTROLTABLE_H_*/
