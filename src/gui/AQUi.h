/***************************************************************************
AQUi.h
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

#ifndef AQUI_H_
#define AQUI_H_

#include <QSharedPointer>

class AQApplication;
class AQAction;

/**
Macros
*/
/** Puntero estático a la aplicación */
#define AbanQ ( static_cast<AQApplication *> ( QCoreApplication::instance() ) )
/** Hilo GUI */
#define AQ_GUI_THREAD ( QCoreApplication::instance()->thread() )
#define AQ_ISGUI_THREAD ( QCoreApplication::instance() != 0 && AQ_GUI_THREAD == QThread::currentThread() )
/** Cajas de mensajes */
#define AQ_MSGBOX_CRIT AQUiUtil().msgBoxCritical
#define AQ_MSGBOX_WARN AQUiUtil().msgBoxWarn
#define AQ_MSGBOX_QUES AQUiUtil().msgBoxQuestion
#define AQ_MSGBOX_INFO AQUiUtil().msgBoxInfo
/** Botones y repuestas para cajas de mensajes */
#define AQ_Ok   AQUi::Ok
#define AQ_Yes  AQUi::Yes
#define AQ_No   AQUi::No
#define AQ_NoB  AQUi::NoButton

/** Puntero compartido para AQAction */
typedef QSharedPointer<AQAction> AQSharedAction;

namespace AQUi
{
  /**
  Tipos de formularios
  */
  enum FormType
  {
    FormMaster = 0,
    FormRecord = 1,
    FormDetail = 2
  };

  /**
  Codigos de respuesta de cuadros de dialogos
  */
  enum DialogCode {
    Rejected = 0,
    Accepted = 1,
    NoDialogCode = 2
  };

  /**
  Flags para cajas de mensajes
  */
  enum StandardButton
  {
    NoButton = 0x00000000,
    Ok = 0x00000400,
    Save = 0x00000800,
    SaveAll = 0x00001000,
    Open = 0x00002000,
    Yes = 0x00004000,
    YesToAll = 0x00008000,
    No = 0x00010000,
    NoToAll = 0x00020000,
    Abort = 0x00040000,
    Retry = 0x00080000,
    Ignore = 0x00100000,
    Close = 0x00200000,
    Cancel = 0x00400000,
    Discard = 0x00800000,
    Help = 0x01000000,
    Apply = 0x02000000,
    Reset = 0x04000000,
    RestoreDefaults = 0x08000000
  };

  Q_DECLARE_FLAGS( StandardButtons, StandardButton )
}

Q_DECLARE_OPERATORS_FOR_FLAGS( AQUi::StandardButtons )

#endif /*AQUI_H_*/
