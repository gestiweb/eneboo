/***************************************************************************
                            FLFormSearchDB.h
                          -------------------
 begin                : Sun Nov 4 2001
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
   bajo  los  términos  de  la  Licencia  Pública General de GNU   en  su
   versión 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

#ifndef FLFORMSEARCHDB_H
#define FLFORMSEARCHDB_H

#include <qpushbutton.h>
#include <qsqlrecord.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qeventloop.h>

#include "FLFormDB.h"

class FLFormSearchDBInterface;

/**
Subclase de la clase FLFormDB, pensada para buscar un registro
en una tabla.

El comportamiento de elegir un registro se modifica para solamente
cerrar el formulario y así el objeto que lo invoca pueda obtener
del cursor dicho registro.

También añade botones Aceptar y Cancelar. Aceptar indica que se ha
elegido el registro activo (igual que hacer doble clic sobre él o
pulsar la tecla Intro) y Cancelar aborta la operación.

@author InfoSiAL S.L.
*/
class FLFormSearchDB: public FLFormDB
{
  Q_OBJECT

public:

  /**
  constructor.

  @param actionName Nombre de la acción asociada al formulario
  */
  FLFormSearchDB(const QString &actionName, QWidget *parent = 0);

  /** constructor sobrecargado.

  @param cursor Objeto FLSqlCursor para asignar a este formulario
  @param actionName Nombre de la acción asociada al formulario
  */
  FLFormSearchDB(FLSqlCursor *cursor, const QString &actionName = QString::null,
                 QWidget *parent = 0);

  /**
  destructor
  */
  ~FLFormSearchDB();

  /**
  Establece el cursor que debe utilizar el formulario.

  @param c Cursor con el que trabajar
  */
  void setCursor(FLSqlCursor *c);

  /**
  Reimplementado, añade un widget como principal del formulario
  */
  void setMainWidget(QWidget *w);

  /**
  Sobrecargado de setMainWidget.

  Aqui toma el nombre de un formulario de la acción asociada y construye el Widget principal, a partir de él.
  */
  void setMainWidget();

  /**
  Muestra el formulario y entra en un nuevo bucle de eventos
  para esperar, a seleccionar registro.

  Se espera el nombre de un campo del cursor
  devolviendo el valor de dicho campo si se acepta el formulario
  y un QVariant::Invalid si se cancela.

  @param n Nombre del un campo del cursor del formulario
  @return El valor del campo si se acepta, o QVariant::Invalid si se cancela
  */
  QVariant exec(const QString &n = QString::null);

  /**
  Aplica un filtro al cursor
  */
  void setFilter(const QString &f);

  /**
  Devuelve si se ha aceptado el formulario
  */
  bool accepted() {
    return accepted_;
  }

protected:

  /**
  Inicialización
  */
  void initForm();

  /**
  Captura evento cerrar
  */
  void closeEvent(QCloseEvent *e);

  /**
  Boton Aceptar
  */
  QPushButton *pushButtonAccept;

public slots:

  /**
  Invoca a la función "init()" del script asociado al formulario
  */
  void initScript();

  /**
  Redefinida por conveniencia
  */
  void hide();

  /**
  Se activa al pulsar el boton aceptar
  */
  virtual void accept();

  /**
  Se activa al pulsar el botón cancelar
  */
  virtual void reject();

private:

  /**
  Almacena si se ha abierto el formulario con el método FLFormSearchDB::exec()
  */
  bool loop;

  /**
  Almacena que se aceptado, es decir NO se ha pulsado, botón cancelar
  */
  bool accepted_;

  /**
  Interface para scripts
  */
  FLFormSearchDBInterface *iface;
};

#endif
