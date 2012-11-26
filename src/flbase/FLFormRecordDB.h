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
   bajo  los  t�rminos  de  la  Licencia  P�blica General de GNU   en  su
   versi�n 2, publicada  por  la  Free  Software Foundation.
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

B�sicamente esta clase hace lo mismo que su clase
base FLFormDB, lo �nico que a�ade son dos botones
Aceptar y/o Cancelar para confirmar o cancelar
los cambios que se realizan en los componentes de
datos que contiene.

Esta clase es id�nea para cargar los formularios de
edici�n de registros definidos en los metadatos
( FLTableMetaData ).

@author InfoSiAL S.L.
*/
class FLFormRecordDB: public FLFormDB
{
  Q_OBJECT

  friend class FLFormRecordDBInterface;

public:

  /**
  constructor.

  Solo acepta que se le indique un cursor ya creado.

  @param cursor Objeto FLSqlCursor con el cursor con el que tratar.
  @param actionName Nombre de la acci�n asociada al formulario
  @param showAcceptContinue Indica si se debe mostrar el bot�n de Aceptar y Continuar
  */
  FLFormRecordDB(FLSqlCursor *cursor, const QString &actionName = QString::null,
                 QWidget *parent = 0, bool showAcceptContinue = true);

  /**
  destructor
  */
  ~FLFormRecordDB();

  /**
  Establece el cursor que debe utilizar el formulario.

  @param c Cursor con el que trabajar
  */
  void setCursor(FLSqlCursor *c);

  /**
  Reimplementado, a�ade un widget como principal del formulario
  */
  void setMainWidget(QWidget *w);

  /**
  Sobrecargado de setMainWidget.

  Aqui toma el nombre de un formulario de la acci�n asociada y construye el Widget principal, a partir de �l.
  */
  void setMainWidget();

  /**
  Establece el t�tulo de la ventana.

  @param text Texto a establecer como t�tulo de la ventana
  @author Silix
  */
  void setCaptionWidget(const QString &text);

protected:

  /**
  Inicializaci�n
  */
  void initForm();

  /**
  Captura evento cerrar
  */
  void closeEvent(QCloseEvent *e);

  /**
  Validaci�n de formulario.

  Invoca a la funci�n "validateForm" del script asociado cuando se acepta el
  formulario y s�lo continua con el commit del registro cuando esa funci�n
  de script devuelve TRUE.

  Si FLTableMetaData::concurWarn() es true y dos o mas sesiones/usuarios est�n
  modificando los mismos campos mostrar� un aviso de advertencia.

  @return TRUE si el formulario ha sido validado correctamente
  */
  bool validateForm();

  /**
  Aceptaci�n de formulario.

  Invoca a la funci�n "acceptedForm" del script asociado al formulario, cuando
  se acepta el formulario y justo antes de hace el commit del registro.
  */
  void acceptedForm();

  /**
  Despu�s de fijar los cambios del buffer del registro actual.

  Invoca a la funci�n "afterCommitBuffer" del script asociado al formulario,
  justo despu�s de hacer el commit del buffer del registro.
  */
  void afterCommitBuffer();

  /**
  Despues de fijar la transacci�n.

  Invoca a la funci�n "afterCommitTransaction" del script asociado al formulario,
  juesto despues de terminar la transacci�n en curso aceptando.
  */
  void afterCommitTransaction();

  /**
  Cancelaci�n de formulario.

  Invoca a la funci�n "canceledForm" del script asociado al formulario, cuando se
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
  Invoca a la funci�n "init()" del script asociado al formulario
  */
  void initScript();

  /**
  Se activa al pulsar el boton aceptar
  */
  virtual void accept();

  /**
  Se activa al pulsar el boton aceptar y contninuar
  */
  virtual void acceptContinue();

  /**
  Se activa al pulsar el bot�n cancelar
  */
  virtual void reject();

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
  Desactiva el bot�n cancelar
  */
  void disablePushButtonCancel();

private:

  /**
  Indica si se han aceptado los cambios
  */
  bool accepted;

  /**
  Indica si se debe mostrar el bot�n Aceptar y Continuar
  */
  bool showAcceptContinue_;

  /**
  Interface para scripts
  */
  FLFormRecordDBInterface *iface;

  /**
  Indica que se est� intentando aceptar los cambios
  */
  bool accepting;

  /**
  Modo en el que inicialmente est� el cursor
  */
  int initialModeAccess;

  /**
  Registra el nivel de anidamiento de transacciones en el que se entra al iniciar el formulario
  */
  int initTransLevel;
};

#endif
