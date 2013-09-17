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

#include <uic/ui_AQConnectDB.h>

/**
Cuadro de diálogo para conectar con una base de datos.
 
Esta clase es la encargada de mostrar un cuadro de dialogo
que nos permite conectar con la base de datos.
Nos permite elegir el driver de la base de datos, la maquina
a la que conectar, puerto, usuario y contraseña.
 
@author InfoSiAL S.L.
*/
class AQConnectDB : public QDialog, private Ui::AQConnectDB
{
  Q_OBJECT

public:

  /**
  constructor 

  @param connectionName Nombre de la conexion a utilizar
  */
  AQConnectDB( const QString & connectionName = "default", QWidget * parent = 0, Qt::WindowFlags f = 0 );

  /**
  Devuelve si hubo un error en el inicio de sesion
  */
  bool error() const
  {
    return error_;
  }

protected:

  /**
  Captura evento pintar
  */
  void paintEvent( QPaintEvent * pe );

public Q_SLOTS:

  /**
  Intenta realizar la conexion, recibe la señal cuando se pulsa el boton conectar
  */
  void tryConnect();

  /**
  Reimplementado para evitar que el diálogo se oculte automáticamente si al aceptar
  se produce un error en la conexión
  */
  void done ( int r );

protected Q_SLOTS:

  /**
  Recibe señal cuando se pulsa botón cancelar conexión
  */
  void cancel();

  /**
  Acciones asociadas cuando se elige el tipo de driver
  */
  void driverChanged( const QString & );

  /**
  Maneja el comboBox Editable con la lista de nombres.
  */
  void changeDB( const QString &DB );

  /**
  Para almacenar marcador con los datos de conexión establecidos en el formulario
  */
  void addMark();

  /**
  Para eliminar el marcador con los datos de conexión seleccionado en la lista de marcadores
  */
  void removeMark();

  /**
  Establece los datos de conexión cuando se selecciona un marcador
  */
  void selectMark( const QString & );

  /**
  Muestra una ventana emergente con los marcadores, al seleccionar uno intenta conectar con los
  datos que define
  */
  void popupMarks();

private Q_SLOTS:

  /**
  Intenta conectar con los datos de conexión definidos en un marcador
  */
  void tryConnectMark( const QString & );

private:

  /**
  Indica que ha habido un error
  */
  bool error_;

  /**
  Nombre de la conexión
  */
  QString connectionName_;

  /**
  Imagen de fondo
  */
  QImage logo;
};

#endif /*AQCONNECTDB_H_*/
