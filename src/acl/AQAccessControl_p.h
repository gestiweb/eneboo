/***************************************************************************
AQAccesControl_p.h
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

#ifndef AQACCESCONTROL_P_H_
#define AQACCESCONTROL_P_H_

#include <AQCore/AQCore.h>

class AQAccessControlPrivate
{
public:

  AQAccessControlPrivate()
  {
    clear();
  }

  AQAccessControlPrivate( const AQAccessControlPrivate & other ) :
      name_( other.name_ ),
      user_( other.user_ ),
      perm_( other.perm_ ),
      acosPerms_( other.acosPerms_ )
  {}

  /**
  Limpia la regla vaciándola y liberando todos los recursos
  */
  void clear()
  {
    name_.clear();
    user_.clear();
    perm_.clear();
    acosPerms_.clear();
  }

  /**
  Almacena el nombre del objeto de alto nivel.
  */
  QString name_;

  /**
  Almacena el nombre del usuario de la base de datos.
  */
  QString user_;

  /**
  Almacena el permiso general de la regla de control de acceso.
  */
  QString perm_;

  /**
  Diccionario de permisos específicos de los ACOs (Access Control Objects)
  hijos o pertenecientes al objeto de alto nivel. El diccionario almacena la
  correspondencia entre el nombre del ACO (utilizado como clave de búsqueda)
  y el permiso a aplicar.
  */
  AQStringHash acosPerms_;
};

#endif /*AQACCESCONTROL_P_H_*/
