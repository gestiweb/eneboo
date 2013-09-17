/***************************************************************************
AQActionMD.h
-------------------
begin                : 06/08/2007
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

#ifndef AQACTIONMD_H_
#define AQACTIONMD_H_

#include <QString>

/**
Contiene la meta-información de acciones de AbanQ.
 
@author InfoSiAL S.L.
*/
class AQActionMD
{
public:

  /**
  Establece el nombre de la accion
  */
  void setName( const QString & n );

  /**
  Establece el nombre del script asociado al formulario de edición de registros
  */
  void setScriptFormRecord( const QString & s );

  /**
  Establece el nombre del script asociado al formulario maestro
  */
  void setScriptForm( const QString & s );

  /**
  Establece el nombre de la tabla origen del formulario maestro
  */
  void setTable( const QString & t );

  /**
  Establece el nombre del formulario maestro
  */
  void setForm( const QString & f );

  /**
  Establece el nombre del formulario de edición de registros
  */
  void setFormRecord( const QString & f );

  /**
  Establece el texto de la barra de título del formulario maestro
  */
  void setCaption( const QString & c );

  /**
  Obtiene el nombre de la accion
  */
  QString name() const;

  /**
  Obtiene el nombre del script asociado al formulario de edición de registros
  */
  QString scriptFormRecord() const;

  /**
  Obtiene el nombre del script asociado al formulario maestro
  */
  QString scriptForm() const;

  /**
  Obtiene  la tabla asociada a la accion
  */
  QString table() const;

  /**
  Obtiene el texto de la barra de título del formulario
  */
  QString caption() const;

  /**
  Obtiene el nombre del formulario mestro
  */
  QString form() const;

  /**
  Obtiene el nombre del formulario de edición de registros
  */
  QString formRecord() const;

private:

  /**
  Nombre de la accion
  */
  QString name_;

  /**
  Nombre del script asociado al formulario de edición de registros
  */
  QString scriptFormRecord_;

  /**
  Nombre del script asociado al formulario maestro
  */
  QString scriptForm_;

  /**
  Nombre de la tabla origen para el formulario maestro
  */
  QString table_;

  /**
  Nombre del formulario maestro
  */
  QString form_;

  /**
  Nombre del formulario de edición de registros
  */
  QString formRecord_;

  /**
  Texto para la barra de título del formulario maestro
  */
  QString caption_;
};

inline void AQActionMD::setName( const QString & n )
{
  name_ = n;
}

inline void AQActionMD::setScriptFormRecord( const QString & s )
{
  scriptFormRecord_ = s + ".qs";
}

inline void AQActionMD::setScriptForm( const QString & s )
{
  scriptForm_ = s + ".qs";
}

inline void AQActionMD::setTable( const QString & t )
{
  table_ = t;
}

inline void AQActionMD::setForm( const QString & f )
{
  form_ = f + ".ui";
}

inline void AQActionMD::setFormRecord( const QString & f )
{
  formRecord_ = f + ".ui";
}

inline void AQActionMD::setCaption( const QString & c )
{
  caption_ = c;
}

inline QString AQActionMD::name() const
{
  return name_;
}

inline QString AQActionMD::scriptFormRecord() const
{
  return scriptFormRecord_;
}

inline QString AQActionMD::scriptForm() const
{
  return scriptForm_;
}

inline QString AQActionMD::table() const
{
  return table_;
}

inline QString AQActionMD::caption() const
{
  return caption_;
}

inline QString AQActionMD::form() const
{
  return form_;
}

inline QString AQActionMD::formRecord() const
{
  return formRecord_;
}

#endif /*AQACTIONMD_H_*/
