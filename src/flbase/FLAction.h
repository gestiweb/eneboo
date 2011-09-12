/***************************************************************************
                             FLAction.h
                          -------------------
 begin                : dom jun 30 2002
 copyright            : (C) 2002-2005 by InfoSiAL S.L.
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

#ifndef FLACTION_H
#define FLACTION_H

#include <qstring.h>

/**
Esta clase contiene información de acciones para abrir formularios.

Se utiliza para enlazar automáticamente formularios con su script,
interfaz y tabla de origen.

@author InfoSiAL S.L.
*/
class FLAction
{
public:

  /**
  constructor.
  */
  FLAction() {}

  /**
  Establece el nombre de la accion
  */
  void setName(const QString &n);

  /**
  Establece el nombre del script asociado al formulario de edición de registros
  */
  void setScriptFormRecord(const QString &s);

  /**
  Establece el nombre del script asociado al formulario maestro
  */
  void setScriptForm(const QString &s);

  /**
  Establece el nombre de la tabla origen del formulario maestro
  */
  void setTable(const QString &t);

  /**
  Establece el nombre del formulario maestro
  */
  void setForm(const QString &f);

  /**
  Establece el nombre del formulario de edición de registros
  */
  void setFormRecord(const QString &f);

  /**
  Establece el texto de la barra de título del formulario maestro
  */
  void setCaption(const QString &c);

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

inline void FLAction::setName(const QString &n)
{
  name_ = n;
}

inline void FLAction::setScriptFormRecord(const QString &s)
{
  scriptFormRecord_ = s + ".qs";
}

inline void FLAction::setScriptForm(const QString &s)
{
  scriptForm_ = s + ".qs";
}

inline void FLAction::setTable(const QString &t)
{
  table_ = t;
}

inline void FLAction::setForm(const QString &f)
{
  form_ = f + ".ui";
}

inline void FLAction::setFormRecord(const QString &f)
{
  formRecord_ = f + ".ui";
}

inline void FLAction::setCaption(const QString &c)
{
  caption_ = c;
}

inline QString FLAction::name() const
{
  return name_;
}

inline QString FLAction::scriptFormRecord() const
{
  return scriptFormRecord_;
}

inline QString FLAction::scriptForm() const
{
  return scriptForm_;
}

inline QString FLAction::table() const
{
  return table_;
}

inline QString FLAction::caption() const
{
  return caption_;
}

inline QString FLAction::form() const
{
  return form_;
}

inline QString FLAction::formRecord() const
{
  return formRecord_;
}

#endif
