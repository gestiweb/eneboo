/***************************************************************************
                          FLConnectDBDialog.h
                          -------------------
 begin                : Sun Jul 1 2001
 copyright            : (C) 2001-2005 by InfoSiAL S.L.
 email                : mail@infosial.com
***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 ***************************************************************************/
/***************************************************************************
   Este  programa es software libre. Puede redistribuirlo y/o modificarlo
   bajo  los  t�rminos  de  la  Licencia  P�blica General de GNU   en  su
   versi�n 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

#ifndef FLSESSIONDB_H
#define FLSESSIONDB_H

#include <qlineedit.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qimage.h>
#include <qcheckbox.h>
#include <qregexp.h>

#include "FLWidgetConnectDB.h"

class FLTableMetaData;

/**
Cuadro de dialogo para conectar con la base de datos.

Esta clase es la encargada de mostrar un cuadro de dialogo
que nos permite conectar con la base de datos.
Nos permite elegir el driver de la base de datos, la maquina
a la que conectar, puerto, usuario y contrase�a.

@author InfoSiAL S.L.
*/
class FL_EXPORT FLConnectDBDialog: public FLWidgetConnectDB
{

  Q_OBJECT

public:

  /**
  constructor
  */
  FLConnectDBDialog(bool disabled = false, QWidget *parent = 0, const char *name = 0,
                    WFlags fl = 0, const QString &strConn = QString::null);

  /**
  destructor
  */
  ~FLConnectDBDialog();

  /**
  Devuelve si hubo un error en el inicio de sesion
  */
  bool error() const {
    return error_;
  }

protected:

  /**
  Captura evento cerrar
  */
  void closeEvent(QCloseEvent *e);

  void paintEvent(QPaintEvent *pe);

public slots:

  /**
  Intenta realizar la conexion, recibe la se�al cuando se pulsa el boton conectar
  */
  void tryConnect();

protected slots:

  /**
  Recibe se�al cuando se pulsa bot�n cancelar conexi�n
  */
  void cancel();

  /**
  Acciones asociadas cuando se elige el tipo de driver
  */
  void driverChanged(const QString &);

  /**
  Maneja el comboBox Editable con la lista de nombres.
  */
  void changeDB(const QString &DB);

  /**
  Para almacenar marcador con los datos de conexi�n establecidos en el formulario
  */
  void addMark();

  /**
  Para eliminar el marcador con los datos de conexi�n seleccionado en la lista de marcadores
  */
  void removeMark();

  /**
  Establece los datos de conexi�n cuando se selecciona un marcador
  */
  void selectMark(const QString &);

  /**
  Muestra una ventana emergente con los marcadores, al seleccionar uno intenta conectar con los
  datos que define
  */
  void popupMarks();

  /**
  Establece el estado del indicador para recordar contrase�a
  */
  void setRememberPasswd(bool on);

  /**
  Desactivar avances de AbanQ v3
  */
  void setOldApi(bool on);

private slots:

  /**
  Intenta conectar con los datos de conexi�n definidos en un marcador
  */
  void tryConnectMark(const QString &);

private:

  /**
  Indica que ha habido un error
  */
  bool error_;

  /**
  Indicador recordar contrase�a
  */
  bool rememberPasswd_;

  /**
  Indicador flag relacionada con conexion actual
  */
  QString connectionPath_;


  /**
  Desactivar avances de AbanQ v3
  */
  bool oldApi_;

  QImage logo;
};

#endif
