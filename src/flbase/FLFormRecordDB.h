/***************************************************************************
                           FLFormRecordDB.h
                          -------------------
 begin                : Sat Jul 28 2001
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

#ifndef FLFORMRECORDDB_H
#define FLFORMRECORDDB_H

#include <qpushbutton.h>
#include <qsqlrecord.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include "FLFormDB.h"

class FLFormRecordDBInterface;

/**
Subclase de FLFormDB pensada para editar registros.

Básicamente esta clase hace lo mismo que su clase
base FLFormDB, lo único que añade son dos botones
Aceptar y/o Cancelar para confirmar o cancelar
los cambios que se realizan en los componentes de
datos que contiene.

Esta clase es idónea para cargar los formularios de
edición de registros definidos en los metadatos
( FLTableMetaData ).

@author InfoSiAL S.L.
*/
class FLFormRecordDB: public FLFormDB
{
  Q_OBJECT

public:

  /**
  constructor.

  Solo acepta que se le indique un cursor ya creado.

  @param cursor Objeto FLSqlCursor con el cursor con el que tratar.
  @param actionName Nombre de la acción asociada al formulario
  @param showAcceptContinue Indica si se debe mostrar el botón de Aceptar y Continuar
  */
  FLFormRecordDB(FLSqlCursor *cursor, const QString &actionName = QString::null,
                 QWidget *parent = 0, bool showAcceptContinue = true);

  /**
  destructor
  */
  ~FLFormRecordDB();

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
  Devuelve el nombre de la clase del formulario en tiempo de ejecución
  */
  QString formClassName() const;

protected:

  /**
  Inicialización
  */
  void initForm();

  /**
  Nombre interno del formulario
  */
  QString formName() const;

  /**
  Une la interfaz de script al objeto del formulario
  */
  void bindIface();

  /**
  Desune la interfaz de script al objeto del formulario
  */
  void unbindIface();

  /**
  Indica si la interfaz de script está unida al objeto formulario
  */
  bool isIfaceBind() const;

  /**
  Captura evento cerrar
  */
  void closeEvent(QCloseEvent *e);

  /**
  Validación de formulario.

  Invoca a la función "validateForm" del script asociado cuando se acepta el
  formulario y sólo continua con el commit del registro cuando esa función
  de script devuelve TRUE.

  Si FLTableMetaData::concurWarn() es true y dos o mas sesiones/usuarios están
  modificando los mismos campos mostrará un aviso de advertencia.

  @return TRUE si el formulario ha sido validado correctamente
  */
  bool validateForm();

  /**
  Aceptación de formulario.

  Invoca a la función "acceptedForm" del script asociado al formulario, cuando
  se acepta el formulario y justo antes de hace el commit del registro.
  */
  void acceptedForm();

  /**
  Después de fijar los cambios del buffer del registro actual.

  Invoca a la función "afterCommitBuffer" del script asociado al formulario,
  justo después de hacer el commit del buffer del registro.
  */
  void afterCommitBuffer();

  /**
  Despues de fijar la transacción.

  Invoca a la función "afterCommitTransaction" del script asociado al formulario,
  juesto despues de terminar la transacción en curso aceptando.
  */
  void afterCommitTransaction();

  /**
  Cancelación de formulario.

  Invoca a la función "canceledForm" del script asociado al formulario, cuando se
  cancela el formulario.
  */
  void canceledForm();

  /**
  Boton Aceptar
  */
  QPushButton *pushButtonAccept;

  /**
  Boton Aceptar y continuar
  */
  QPushButton *pushButtonAcceptContinue;

  /**
  Boton Primero
  */
  QPushButton *pushButtonFirst;

  /**
  Boton Anterior
  */
  QPushButton *pushButtonPrevious;

  /**
  Boton Siguiente
  */
  QPushButton *pushButtonNext;

  /**
  Boton Ultimo
  */
  QPushButton *pushButtonLast;

public slots:

  /**
  Invoca a la función "init()" del script asociado al formulario
  */
  bool initScript();

  /**
  Se activa al pulsar el boton aceptar
  */
  virtual void accept();

  /**
  Se activa al pulsar el boton aceptar y continuar
  */
  virtual void acceptContinue();

  /**
  Se activa al pulsar el botón cancelar
  */
  virtual void reject();

  /**
  Devuelve el script asociado al formulario
  */
  virtual QSScript *script() const;

protected slots:

  /**
  Ir al primer anterior
  */
  virtual void firstRecord();

  /**
  Ir al registro anterior
  */
  virtual void previousRecord();

  /**
  Ir al registro siguiente
  */
  virtual void nextRecord();

  /**
  Ir al ultimo registro
  */
  virtual void lastRecord();

  /**
  Desactiva el botón cancelar
  */
  void disablePushButtonCancel();

private:

  /**
  Indica si se debe mostrar el botón Aceptar y Continuar
  */
  bool showAcceptContinue_;

  /**
  Indica que se está intentando aceptar los cambios
  */
  bool accepting;

  /**
  Modo en el que inicialmente está el cursor
  */
  int initialModeAccess;

  /**
  Registra el nivel de anidamiento de transacciones en el que se entra al iniciar el formulario
  */
  int initTransLevel;
};

#endif
