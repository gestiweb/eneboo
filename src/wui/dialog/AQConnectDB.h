/***************************************************************************
AQConnectDB.h
-------------------
begin                : 25/08/2007
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

#ifndef AQCONNECTDB_H_
#define AQCONNECTDB_H_

#include <AQUi/WQDialog.h>

class AQConnectDBWidget;

/**
 Cuadro de diálogo para conectar con una base de datos.

 Esta clase es la encargada de mostrar un cuadro de dialogo
 que nos permite conectar con la base de datos.
 Nos permite elegir el driver de la base de datos, la maquina
 a la que conectar, puerto, usuario y contraseña.

 @author InfoSiAL S.L.
 */
class AQConnectDB : public WQDialog
{
  Q_OBJECT

public:

  /**
   constructor

   @param connectionName Nombre de la conexion a utilizar
   */
  AQConnectDB( const QString & connectionName = "default" );

  /**
   Devuelve si hubo un error en el inicio de sesion
   */
  bool error() const
  {
    return error_;
  }

public Q_SLOTS:

  /**
   Intenta realizar la conexion, recibe la señal cuando se pulsa el boton conectar
   */
  void tryConnect();

protected:

  WQWidget * widget();

private:

  AQConnectDBWidget * widget_;
  QString connectionName_;
  bool error_;

  friend class AQConnectDBWidget;
};

#endif /*AQCONNECTDB_H_*/
