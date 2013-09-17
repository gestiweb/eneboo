/***************************************************************************
AQCore.h
-------------------
begin                : 11/08/2007
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

#ifndef AQCORE_H_
#define AQCORE_H_

#include <AQCore/AQConfig.h>

#include <QList>
#include <QHash>
#include <QString>

#include <QDebug>

/**
Macros
*/
/** Mensajes y depuracion */
#define aqCrit(C) qCritical() << C
#define aqFatal(C) qFatal(C)
#ifdef AQ_WARN
# define aqWarn(C) qWarning() << C
#else
# define aqWarn(C) {}
#endif
#ifdef AQ_DEBUG
# define aqDebug(C) qWarning() << C
#else
# define aqDebug(C) {}
#endif
/** Caché Disco */
#define AQ_DISKCACHE_INS      AQDiskCache().insert
#define AQ_DISKCACHE_FIND     AQDiskCache().find
#define AQ_DISKCACHE_CLR      AQDiskCache().clear
#define AQ_DISKCACHE_FILEPATH AQDiskCache().absoluteFilePath
#define AQ_DISKCACHE_DIRPATH  AQDiskCache().absoluteDirPath
/** Formato de Fecha y Hora */
#define AQ_FORMAT_D   QLatin1String( "dd-MM-yyyy" )
#define AQ_FORMAT_T   QLatin1String( "hh:mm:ss" )
#define AQ_FORMAT_DT  QLatin1String( "dd-MM-yyyy | hh:mm:ss" )
/** Fecha y Hora actual */
#define AQ_NOW_D  QDate::currentDate().toString( AQ_FORMAT_D )
#define AQ_NOW_T  QTime::currentTime().toString( AQ_FORMAT_T )
#define AQ_NOW_DT QDateTime::currentDateTime().toString( AQ_FORMAT_DT )
/** Generales */
#define AQ_KEYBASE  AQConfig().keybase()

#define AQ_DECLARE_PRIVATE(Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(aq_d_ptr); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(aq_d_ptr); } \
    friend class Class##Private;

#define AQ_DECLARE_PRIVATE_D(Dptr, Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(Dptr); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(Dptr); } \
    friend class Class##Private;

#define AQ_DECLARE_PUBLIC(Class)                                    \
    inline Class* q_func() { return static_cast<Class *>(aq_q_ptr); } \
    inline const Class* q_func() const { return static_cast<const Class *>(aq_q_ptr); } \
    friend class Class;

#define AQ_DECLARE_PUBLIC_Q(Qptr, Class)                                    \
    inline Class* q_func() { return static_cast<Class *>(Qptr); } \
    inline const Class* q_func() const { return static_cast<const Class *>(Qptr); } \
    friend class Class;

#define AQ_D(Class) Class##Private * const d = d_func()
#define AQ_Q(Class) Class * const q = q_func()

#define AQ_TR AQCoreUtil::translate

#define AQ_NEWEVENT( aqEvent )      new QEvent( (QEvent::Type) AQCoreEvent::aqEvent )
#define AQ_POSTEVENT( O, aqEvent )  QCoreApplication::postEvent( O, AQ_NEWEVENT( aqEvent ) )
#define AQ_SENDEVENT( O, aqEvent )  { QEvent ev( (QEvent::Type) AQCoreEvent::aqEvent ); \
                                      QCoreApplication::sendEvent( O, &ev ); }

class AQActionMD;
class AQRelationMD;
class AQFieldMD;
class AQTableMD;
class AQShellCmdInfo;

/**
Manejador de mensajes de consola
*/
enum AQMsgType {
  AQLogMsg,
  AQDebugMsg,
  AQWarningMsg,
  AQCriticalMsg,
  AQFatalMsg,
  AQSystemMsg = AQCriticalMsg
};
enum AQMsgFormat {
  AQPlainMsg,
  AQHtmlMsg
};
typedef void ( *AQConsoleMsgHandler ) ( AQMsgType, AQMsgFormat, QObject *, const char * );
/**
Lista de relaciones
*/
typedef QList<AQRelationMD *> AQRelationMDList;
/**
Hash de acciones
*/
typedef QHash<QString, AQActionMD *> AQActionHash;
/**
Hash de campos
*/
typedef QHash<QString, AQFieldMD *> AQFieldMDHash;
/**
Hash de tablas
*/
typedef QHash<QString, AQTableMD *> AQTableMDHash;
/**
Hash de cadenas de texto
*/
typedef QHash<QString, QString> AQStringHash;
/**
Hash de informaciones de comandos de shell
*/
typedef QHash<QString, AQShellCmdInfo> AQShellCmdInfoHash;

namespace AQCore
{
  /**
  Constantes de tipos especiales no contemplados en QVariant
  */
  enum specialType {
    /**
    Tipo SERIAL o autoincremento
    */
    Serial = 100,
    /**
    Tipo de campo unlock
    */
    Unlock = 200
  };

  /** Constantes de tipos de cardinalidades de una relacion */
  enum Cardinality {
    /** Uno a muchos */
    RELATION_1M = 0,
    /** Muchos a uno */
    RELATION_M1 = 1
  };

  /** Constantes de tipos de ficheros de definición */
  enum TypeDefFile {
    /** .ui */
    UiDef = 0,
    /** .qry */
    QryDef = 1,
    /** .kut */
    KutDef = 2,
    /** .mtd */
    MtdDef = 3,
    /** .qry */
    TsDef = 4,
    /** .xml ( actions ) */
    ActDef = 5,
    /** .xml ( acl ) */
    AclDef = 6,
    /** none */
    NoDefFile = 99
  };
}

namespace AQCoreEvent
{
  /** Tipos de eventos internos */
  enum InternalEventType {
    /** Primer identificador que sirve de base para los eventos internos */
    MinInternal = 2000,
    /** Peticion de actualizado inmediato */
    UpdateNow = 2001,
    /** Peticion para rehacer Layout */
    LayoutRequest = 2002,
    /** Peticion de actualizado */
    UpdateRequest = 2003,
    /** Peticion de inicializacion inmediata */
    InitNow = 2004
  };
}

#endif /*AQCORE_H_*/
