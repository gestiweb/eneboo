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
#include <QList>

class AQApplicationWeb;
class WQApplication;
class WQWidget;
class AQAction;

/**
 Macros
 */
/** Puntero estático a la aplicación */
#define AbanQ  ( static_cast<AQApplicationWeb *> ( QCoreApplication::instance() ) )
#define AbanQWT ( AQApplicationWeb::wqAppInstance() )
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

/** Formato de señal segun la serie de la version de Wt */
#ifndef WT_SERIES_2
# define WT_SIGNAL( C ) C()
#else
# define WT_SIGNAL( C ) C
#endif
/** Definicion de metodos invocables desde WUI */
#define AQ_WUI_INVOKABLE( aqMet )               void invoke##aqMet() { invokeMethod(#aqMet); }
#define AQ_WUI_INVOKABLE_1( aqMet, Arg1 )       void invoke##aqMet(Arg1 val1) { invokeMethod(#aqMet,Q_ARG(Arg1,val1)); }
#define AQ_WUI_INVOKABLE_2( aqMet, Arg1, Arg2 ) void invoke##aqMet(Arg1 val1,Arg2 val2) { invokeMethod(#aqMet,Q_ARG(Arg1,val1),Q_ARG(Arg2,val2)); }
/** Conector de señal WUI a metodo invocable */
#define AQ_WUI_CONNECT( S, P, T, M ) WT_SIGNAL(S).connect(P, &T::invoke##M)
/** Generales de ayuda para WUI */
#define toWString( s )    WString::fromUTF8((const char*)(s).toUtf8())
#define toQString( s )    QString::fromUtf8((s).toUTF8().c_str())
#define toWAlig( aqAlig ) ( aqAlig.testFlag( Qt::AlignLeft )    ? Wt::AlignLeft     : ( Wt::AlignmentFlag ) 0 ) | \
                          ( aqAlig.testFlag( Qt::AlignRight )   ? Wt::AlignRight    : ( Wt::AlignmentFlag ) 0 ) | \
                          ( aqAlig.testFlag( Qt::AlignHCenter ) ? Wt::AlignCenter   : ( Wt::AlignmentFlag ) 0 ) | \
                          ( aqAlig.testFlag( Qt::AlignJustify ) ? Wt::AlignJustify  : ( Wt::AlignmentFlag ) 0 ) | \
                          ( aqAlig.testFlag( Qt::AlignTop )     ? Wt::AlignTop      : ( Wt::AlignmentFlag ) 0 ) | \
                          ( aqAlig.testFlag( Qt::AlignBottom )  ? Wt::AlignBottom   : ( Wt::AlignmentFlag ) 0 ) | \
                          ( aqAlig.testFlag( Qt::AlignVCenter ) ? Wt::AlignMiddle   : ( Wt::AlignmentFlag ) 0 )

#ifdef AQ_WARN
# define wuiLog( C )      AQApplicationWeb::wqAppInstance()->log( "" ) << C << ":obj:" << this
# define wuiShortLog( C ) AQApplicationWeb::wqAppInstance()->log( "" ) << C
#else
# define wuiLog( C )      {}
# define wuiShortLog( C ) {}
#endif

#ifdef AQ_DEBUG
# define wuiDebugLog( C )       AQApplicationWeb::wqAppInstance()->log( "" ) << C << ":obj:" << this
# define wuiDebugShortLog( C )  AQApplicationWeb::wqAppInstance()->log( "" ) << C
#else
# define wuiDebugLog( C )       {}
# define wuiDebugShortLog( C )  {}
#endif

/**
 Lista de relaciones
 */
typedef QList<WQWidget *> WQWidgetList;

/** Puntero compartido para AQAction */
typedef QSharedPointer<AQAction> AQSharedAction;

namespace AQUi
{
  /**
   Tipos de formularios
   */
  enum FormType {
    FormMaster = 0,
    FormRecord = 1,
    FormDetail = 2
  };

  /**
   Iconos para cajas de mensajes
   */
  enum Icon {
    NoIcon = 0,
    Information = 1,
    Warning = 2,
    Critical = 3,
    Question = 4
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
  enum StandardButton {
    NoButton = 0x00,
    Ok = 0x01,
    Cancel = 0x02,
    Yes = 0x04,
    No = 0x08,
    Abort = 0x10,
    Retry = 0x20,
    Ignore = 0x40,
    YesAll = 0x80,
    NoAll = 0x100
  };

  Q_DECLARE_FLAGS( StandardButtons, StandardButton )
}

Q_DECLARE_OPERATORS_FOR_FLAGS( AQUi::StandardButtons )

#endif /*AQUI_H_*/
