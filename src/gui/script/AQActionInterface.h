/***************************************************************************
AQActionInterface.h
-------------------
begin                : 10/02/2009
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

#ifndef AQACTIONINTERFACE_H_
#define AQACTIONINTERFACE_H_

#include <AQScript/AQObjectInterfaceQSA.h>
#include <AQUi/AQAction.h>

class AQSqlCursorInterface;
class AQSqlCursorInterfaceAQ2;
class AQFormDBInterface;

#ifdef AQ_OBJ_FACTORY_IMPL
static void deleteSharedAQAction( AQAction * obj )
{
  if ( !obj->parent() )
    obj->deleteLater();
}
#endif

/**
Interfaz para AQAction
*/ 
/**
Acciones de alto nivel de AbanQ.
 
Las acciones de este tipo ( no confundir con AQActionMD, que sólo son 
almacenes de meta-información ) son controladores que gestionan el vinculo
entre los objetos de la capa persistente del módulo AQSql y los objetos de 
la capa de presentación del módulo AQUi ( patrón MVC ).
 
Las acciones pueden formar jerarquías maestro-detalle, también conocidas como
padre-hijo, permitiendo crear complejas estructuras que son sincronizadas de
forma transparente, automática y coherente, además de realizar las operaciones
necesarias sobre ellas.
 
@author InfoSiAL S.L.
*/
class AQActionInterface : public AQObjectInterfaceQSA
{
  Q_OBJECT

  Q_PROPERTY( QString actName READ actName )
  Q_PROPERTY( QString connName READ connName )
  Q_PROPERTY( bool undefined READ isUndefined )

public:

  AQSharedAction obj_;

public Q_SLOTS:

  /**
  @return Un nuevo formulario al que se le asigna esta acción y listo para usarse.
          Mediante los métodos setUi* de AQFormDB se pueden establecer distintas 
          interfaces personalizadas para el formulario de forma dinámica.
          El formulario devuelto no tiene activa ninguna interfaz.
  */
  AQFormDBInterface * form();

  /**
  Construye un nuevo formulario y lo muestra en forma modal en un nuevo bucle de eventos.

  Auxiliar ofrecida por ergonomía.

  @param fieldName    Nombre de un campo del cursor.
  @param actionNameUi Nombre de la meta-información de una acción a usar para construir el
                      formulario en vez de usar la definida por el constructor. 
                      Usar con precaución, esto fuerza a utilizar una interfaz gráfica que no
                      tiene por qué ser coherente con el cursor mantenido a bajo nivel por la acción. 
  @return Si fieldName no es vacio, el valor del campo indicado en el registro posicionado cuando se 
          acepta el formulario
  */
  QVariant exec( const QString & fieldName = QString(), const QString & actionNameUi = QString() );

  /**
  @return Cursor de bajo nivel asociado. Las acciones mantienen según lo indicado
          en la meta-información un cursor de registros de una tabla o tablas de la base
          de datos.
  */
  AQSqlCursorInterface * cursor() const;

  /**
  @return Acción maestra, 0 si no existe.
  */
  AQActionInterface * aqActionMaster() const;

  /**
  @return Acción asociada a un campo del cursor, 0 si no existe. 
          Los campos del cursor que están relacionados con otros, tienen asignada una acción
          que gestiona esa relación, mediante este método es posible obtenerla.
  */
  AQActionInterface * aqActionField( const QString & fieldName );

  /**
  Auxiliares ofrecidas por ergonomía para realizar las operaciones mas habituales con registros 
  del cursor; insertar, editar, borrar, etc..

  @param actionNameUi Nombre de la meta-información de una acción a usar para construir el
                      formulario en vez de usar la definida por el constructor. 
                      Usar con precaución, esto fuerza a utilizar una interfaz gráfica que no
                      tiene por qué ser coherente con el cursor mantenido a bajo nivel por la acción.
  */
  void insertRecord( const QString & actionNameUi = QString() );
  void editRecord( const QString & actionNameUi = QString() );
  void browseRecord( const QString & actionNameUi = QString() );
  void copyRecord( const QString & actionNameUi = QString() );
  void deleteRecord();

protected Q_SLOTS:

  /**
  @return Si la interfaz es indefinida, es decir, no tiene objeto subyacente
  */
  bool isUndefined() const;

  /**
  @return Nombre de la meta-información. Ofrecida por ergonomía.
  */
  QString actName() const;

  /**
  @return Nombre de la conexión de la base de datos que utiliza esta acción. Ofrecida por ergonomía.
  */
  QString connName() const;

Q_SIGNALS:

  /**
  Señal emitida cuando la acción abrió un formulario modal ( con exec() ) y fue aceptado por el
  usuario eligiendo un registro
  */
  void accepted();

protected:

  friend class AQUiObjectFactoryQSA;

  /**
  Constructor
  */
  AQActionInterface( const AQScriptEngineQSA * eng,
                     AQSharedAction aqAction );

  /**
  Constructor

  @param actionName Nombre un objeto AQActionMD con la meta-información de la acción.
  */
  AQActionInterface( const AQScriptEngineQSA * eng,
                     const QString & actionName = QString(),
                     const QString & connName = "default" );

  /**
  Destructor
  */
  virtual ~AQActionInterface();

  void connects() const;
};

inline bool AQActionInterface::isUndefined() const
{
  return !obj_;
}


/**
Interfaz estática para AQAction
*/
class AQActionInterfaceStatic : public AQObjectInterfaceQSA
{
  Q_OBJECT

public:

  AQActionInterfaceStatic( const AQScriptEngineQSA * eng ) :
      AQObjectInterfaceQSA( eng )
  {}

public Q_SLOTS:

  /**
  Para obtener la accion que controla el cursor indicado.

  Si el cursor se ha creado explícitamente y no está controlado por una acción,
  se creará una nueva que lo tomará en propiedad y se encargará de destruirlo.
  Por lo tanto una vez que se ha llamado este método el cursor solo debería ser
  accedido a través de la acción con AQAction::cursor(). 

  Este método no se debe usar en nuevo código, sólo se mantiene para facilitar la portabilidad 
  de código desde AbanQ v2. Es muy probable que desaparezca en nuevas versiones.
  */
  AQActionInterface * fromSqlCursor( const AQSqlCursorInterfaceAQ2 * cur ) const;
};

Q_DECLARE_METATYPE( AQActionInterface* )

#endif /*AQACTIONINTERFACE_H_*/
