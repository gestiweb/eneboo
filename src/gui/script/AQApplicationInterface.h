/***************************************************************************
AQApplicationInterface.h
-------------------
begin                : 24/09/2007
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

#ifndef AQAPPLICATIONINTERFACE_H_
#define AQAPPLICATIONINTERFACE_H_

#include <AQScript/AQObjectInterfaceQSA.h>
#include <AQScript/AQScriptEngineQSA.h>

class AQMainWindowInterface;
class QPixmap;
class QTextEdit;
class AQApplication;

/**
Interfaz para AQApplication y funciones genéricas del sistema
*/
class AQApplicationInterface : public AQObjectInterfaceQSA
{
  Q_OBJECT

public Q_SLOTS:

  /**
  @return Ventana principal actualmente activa
  */
  AQMainWindowInterface * activeMainWindow() const;

  /**
  @return Ventana principal asociada a la conexión de base de datos indicada
  */
  AQMainWindowInterface * mainWindow( const QString & connectionName ) const;

  /**
  Crea una nueva ventana principal abriendo una conexion a una base de datos
  para ella.

  Muestra al usuario el diálogo de conexión a base de datos, si la conexión
  tiene éxito abre una ventana principal para esa base de datos abierta.

  La ventana principal creada pasa a ser la activa
  */
  void newMainWindow();

  /**
  Cierra la ventana principal asociada a la conexión de base de datos indicada.
  */
  void closeMainWindow( const QString & connectionName );

  /**
  Fuerza a que los eventos pendientes de la aplicación sean procesados
  */
  void processEvents();

  /**
  Obtiene una lista de controles de clase c para el formulario wN

  @param  wN        Nombre del formulario con el formato [Form|Record]<nombre de la acción>
  @param  c         Nombre de la clase del objeto que se busca
  @param  connName  Nombre de la conexion

  @return Cadena que contiene la lista con formato nombreObjeto1/descObjeto1*nomObjeto2/descObj2*...
  */
  QString getWidgetList( const QString & wN, const QString & c,
                         const QString & connName = "default" ) const;

  /**
  Obtiene la lista de pestañas que contiene un control QTabWidget

  @param  wN        Nombre del formulario con el formato [Form|Record]<nombre de la acción>
  @param  n         Nombre del tabWidget
  @param  connName  Nombre de la conexion

  @return Cadena que contiene la lista con formato nomPestaña1/descPestaña1*nomPestaña2/descPestaña2*...
  */
  QString getTabWidgetPages( const QString & wN, const QString & n,
                             const QString & connName = "default" ) const;

  /**
  @return El prefijo de instalación
  */
  QString installPrefix() const;

  /**
  @return El directorio principal utilizado por la caché de disco para almacenar los archivos
  */
  QString diskCacheAbsDirPath() const;

  /**
  @return Cadena que contiene número de versión de AbanQ
  */
  QString version() const;

  /**
  Indica si el módulo especificado está cargado en el sistema.

  @param  idM       Cadena con el nombre del módulo
  @param  connName  Nombre de la conexion
  @return TRUE en caso de que el módulo esté cargado, FALSE en caso contrario
  */
  bool isLoadedModule( const QString & idM, const QString & connName = "default" ) const;

  /**
  Indica si la aplicación se compiló en modo DEBUG

  @return TRUE en caso de que la aplicación se compilara en modo DEBUG, FALSE en caso contrario
  */
  bool isDebugEnabled() const;

  /**
  Obtiene la versión de un módulo.

  @param  idM       Identificador del módulo del que se quiere saber su versión
  @param  connName  Nombre de la conexion
  @return Cadena con la versión
  */
  QString versionModule( const QString & idM, const QString & connName = "default" ) const;

  /**
  Obtiene el nombre del driver con el que se conecta a la base de datos

  @param  connName  Nombre de la conexion
  @return Nombre del driver
  */
  QString nameDriver( const QString & connName = "default" ) const;

  /**
  Obtiene el nombre del usuario con el que se conecta a la base de datos

  @param  connName  Nombre de la conexion
  @return Nombre del usuario
  */
  QString nameUser( const QString & connName = "default" ) const;

  /**
  Obtiene el nombre de la base de datos

  @param  connName  Nombre de la conexion
  @return Nombre de la base de datos
  */
  QString nameBD( const QString & connName = "default" ) const;

  /**
  Obtiene el nombre del host de la conexión

  @param  connName  Nombre de la conexion
  @return Nombre del host
  */
  QString nameHost( const QString & connName = "default" ) const;

  /**
  @param  connName  Nombre de la conexion
   
  @return Identificador único y global para la sessión actualmente abierta en la base de datos indicada
  */
  QString idSession( const QString & connName = "default" ) const;

  /**
  Añade una base de datos a las conexiones disponibles.

  La base de datos será abierta. Si ya existiera una conexión con el mismo nombre
  la base datos correspondiente será cerrada y borrada, sustituyéndola por la nueva.

  @param  driverAlias     Alias del driver ( PostgreSQL, MySQL, SQLite, ... ), ver AQSqlDatabase.
  @param  nameDB          Nombre de la base de datos a la que conectar
  @param  user            Usuario de la conexión
  @param  password        Contraseña para el usuario
  @param  host            Nombre o dirección del servidor de la base de datos
  @param  port            Puerto TCP de conexion
  @param  connectionName  Nombre de la conexion
  @return TRUE si se pudo realizar la conexión, FALSE en caso contrario
  */
  bool addDatabase( const QString & driverAlias, const QString & nameDB,
                    const QString & user, const QString & password, const QString & host,
                    int port, const QString & connectionName ) const;

  /**
  Elimina una base de datos de las conexiones disponibles.

  Cierra la base de datos correspondiente y la elimina.

  @param  connectionName  Nombre de la conexion
  @return TRUE si se pudo eliminar la base de datos, FALSE en caso contrario
  */
  bool removeDatabase( const QString & connectionName ) const;

  /**
  Convierte una cadena a Unicode

  @param  codec Nombre de la codificación del texto
  @param  text  Texto a convertir
  */
  QString toUnicode( const QString & text, const QString & codecName ) const;

  /**
  Convierte una cadena en Unicode a la codificacion indicada

  @param  codec Nombre de la codificación a utilizar
  @param  text  Texto a convertir
  */
  QByteArray fromUnicode( const QString & text, const QString & codecName ) const;

protected:

  /**
  Constructor
  */
  AQApplicationInterface( const AQScriptEngineQSA * eng ) :
      AQObjectInterfaceQSA( eng )
  {}

  friend class AQUiObjectFactoryQSA;
};

inline QString AQApplicationInterface::installPrefix() const
{
  return AQPREFIX;
}

inline QString AQApplicationInterface::version() const
{
  return AQVERSION;
}

inline bool AQApplicationInterface::isDebugEnabled() const
{
#ifdef AQ_DEBUG
  return true;
#endif
  return false;
}



/**
Interfaz de soporte para AbanQ v2
*/
class AQApplicationInterfaceAQ2 : public AQApplicationInterface
{
  Q_OBJECT

public:

  AQApplicationInterfaceAQ2( const AQScriptEngineQSA * eng ) :
      AQApplicationInterface( eng )
  {}

public Q_SLOTS:

  void startTest( const QString &, const QString &, const QString & );

  AQMainWindowInterface * mainWidget() const;

  void statusHelpMsg( const QString & );

  void openMasterForm( const QString &, const QPixmap & );

  void execMainScript( const QString & );

  QString testSessionsList( const QString & );

  void updateAreas();

  bool isTestEnabled();

  QString * nameObjectScript( const QString & );

  void showDocPage( const QString & )
  {
    AQ_NO_PORT_AQ2( AQApplication::showDocPage() );
  }

  QVariant call( const QString &, const QVariantList & = QVariantList(),
                 const QString & = QString(), const QString & = "default" );

  QVariant call( const QString &, const QString & );

  void continueTesting( const QString & );

  void setCaptionMainWidget( const QString & );

  QString lastTextCaption() const;

  void installACL( const QString & )
  {
    AQ_NO_PORT_AQ2( AQApplication::installACL() );
  }

  void printTextEdit( QTextEdit * )
  {
    AQ_NO_PORT_AQ2( AQApplication::printTextEdit );
  }

  void reinit();

  void cleanupMetaData( const QString & = "default" );

  void Mr_Proper( const QString & = "default" );

  QString canonicalForm( const QString & )
  {
    AQ_NO_PORT_AQ2( AQApplication::canonicalForm );
    return QString();
  }

  QString translate( const QString & )
  {
    AQ_NO_PORT_AQ2( AQApplication::translate );
    return QString();
  }

  QString fold( const QString & )
  {
    AQ_NO_PORT_AQ2( AQApplication::fold );
    return QString();
  }

  QString unfold( const QString & )
  {
    AQ_NO_PORT_AQ2( AQApplication::unfold );
    return QString();
  }

  QString encodeEndLine( const QString & )
  {
    AQ_NO_PORT_AQ2( AQApplication::encodeEndLine );
    return QString();
  }

  QString decodeEndLine( const QString & )
  {
    AQ_NO_PORT_AQ2( AQApplication::decodeEndLine );
    return QString();
  }

  QString encodeQuotedPrintable( const QString &, bool = false )
  {
    AQ_NO_PORT_AQ2( AQApplication::encodeQuotedPrintable );
    return QString();
  }

  QString decodeQuotedPrintable( const QString & )
  {
    AQ_NO_PORT_AQ2( AQApplication::decodeQuotedPrintable );
    return QString();
  }

  QString connectionNameForAction( const QString & = QString() );

  void setReimpMethod( const QString &, const QString & );

  bool existsReimpMethod( const QString & ) const;

  QVariant callReimp( const QString &, const QVariantList & = QVariantList() );

  QString dialogGetFileImage()
  {
    AQ_NO_PORT_AQ2( AQApplication::dialogGetFileImage );
    return QString();
  }

  QPixmap toPixmap( const QVariant & )
  {
    AQ_NO_PORT_AQ2( AQApplication::toPixmap );
    return QPixmap();
  }

  QVariant fromPixmap( const QPixmap & )
  {
    AQ_NO_PORT_AQ2( AQApplication::fromPixmap );
    return QVariant();
  }

  int transactionLevel()
  {
    AQ_NO_PORT_AQ2( AQApplication::transactionLevel );
    return -1;
  }

  bool addSysCode( const QString &, const QString & = QString() )
  {
    AQ_NO_PORT_AQ2( AQApplication::addSysCode );
    return false;
  }

  void setScriptEntryFunction( const QString & )
  {
    AQ_NO_PORT_AQ2( AQApplication::setScriptEntryFunction );
  }

  void setDatabaseLockDetection( bool = true, int = 30000, int = -1,
                                 bool = true, const QString & = QString(),
                                 const QString & = "default" )
  {
    AQ_NO_PORT_AQ2( AQApplication::setDatabaseLockDetection );
  }

  QStringList locksStatus( const QString & = "default" )
  {
    AQ_NO_PORT_AQ2( AQApplication::locksStatus );
    return QStringList();
  }

  QStringList detectLocks( const QString & = "default" )
  {
    AQ_NO_PORT_AQ2( AQApplication::detectLocks );
    return QStringList();
  }

  QStringList detectRisksLocks( const QString & = QString(), const QString & = QString(),
                                const QString & = "default" )
  {
    AQ_NO_PORT_AQ2( AQApplication::detectRisksLocks );
    return QStringList();
  }

  AQApplication * obj() const;

Q_SIGNALS:

  void databaseLockDetected();
};

#endif /*AQAPPLICATIONINTERFACE_H_*/
