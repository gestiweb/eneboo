/***************************************************************************
AQScript.h
-------------------
begin                : 11/02/2009
copyright            : (C) 2003-2009 by InfoSiAL S.L.
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

#ifndef AQSCRIPT_H_
#define AQSCRIPT_H_

#include <AQCore/AQCore.h>
#include <QSharedPointer>

/**
Macros
*/ 
/** Ayudas para portar desde AbanQ v2 */
#define AQ_VOID_PORT_AQ2(C) \
       aqWarn( engine->aqScriptAndLineState() + \
               QLatin1String( "\nDESHECHADO, NO USAR EN AbanQ v3-> " ) + QLatin1String( #C ) );

#define AQ_NO_PORT_AQ2(C) \
       aqWarn( engine->aqScriptAndLineState() + \
               QLatin1String( "\nOBSOLETO-> " ) + QLatin1String( #C ) + \
               QLatin1String( "\n Aun no se ha definido en AbanQ v3 o todavía no ha sido portado" ) );

#define AQ_PORT_AQ2_NOP(C, P) \
       aqWarn( engine->aqScriptAndLineState() + \
               QLatin1String( "\nPortado a AbanQ v3 pero OBSOLETO-> " ) + QLatin1String( #C ) + \
               QLatin1String( "\nPara nuevos desarrollos en AbanQ v3 se recomienda:\n " ) + \
               QString( #P ).replace( QLatin1String( "::" ), QLatin1String( "." ) ) \
                            .replace( QLatin1String( "_n" ), QLatin1String( "\n" ) ) \
                            .replace( QLatin1String( " {" ), QLatin1String( " {\n" ) ) \
                            .replace( QLatin1String( "} }" ), QLatin1String( "}\n}" ) ) \
                            .replace( QLatin1Char( ';' ), QLatin1String( ";\n" ) ) \
                            .replace( QLatin1String( "return " ), QString() ) \
                            .replace( QLatin1String( "->" ), QLatin1String( "." ) ) );

#define AQ_PORT_AQ2(C, P) AQ_PORT_AQ2_NOP(C, P) P

/** Ayudas para definir interfaces en AbanQ v3 y v2 */
#define AQ_SCR_IFACE(Obj, ShrObj, Class) \
      { \
       AQObjectInterfaceQSA * iface; \
       if ( Obj ) { \
        QObject * obj__ = static_cast<QObject *>( Obj ); \
        iface = getInterface( obj__ ); \
        if ( !iface ) \
         iface = new Class##Interface( engine, \
         QSharedPointer<Class>( qobject_cast<Class *>( obj__ ), \
         deleteShared##Class ) ); \
       } else { \
        iface = getInterface( ShrObj ); \
        if ( !iface ) \
         iface = new Class##Interface( engine, \
         QSharedPointer<Class>( ShrObj.staticCast<Class>() ) ); \
       } \
       return iface; \
      }

#define AQ_SCR_IFACE_AQ2(Obj, ShrObj, Class) \
      { \
       AQObjectInterfaceQSA * iface;\
       if ( Obj ) { \
        QObject * obj__ = static_cast<QObject *>( Obj ); \
        iface = getInterface( obj__ ); \
        if ( !iface ) \
         iface = new Class##InterfaceAQ2( engine, \
         QSharedPointer<Class>( qobject_cast<Class *>( obj__ ), \
         deleteShared##Class ) ); \
       } else { \
        iface = getInterface( ShrObj ); \
        if ( !iface ) \
         iface = new Class##InterfaceAQ2( engine, \
         QSharedPointer<Class>( ShrObj.staticCast<Class>() ) ); \
       } \
       return iface; \
      }

/** Puntero compartido para objetos de interfaces */
typedef QSharedPointer<QObject> AQSharedObjectQSA;

#endif /*AQSCRIPT_H_*/
