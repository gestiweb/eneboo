/***************************************************************************
AQAction.h
-------------------
begin                : 29/01/2009
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

#ifndef AQACTION_H_
#define AQACTION_H_

#include <QVariant>

#include <AQCore/AQObject.h>
#include <AQUi/AQUi.h>

class AQActionPrivate;
class AQFormDB;
class AQActionMD;
class AQRelationMD;
class AQSqlCursor;

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
class AQAction : public AQObject
{
  Q_OBJECT

  AQ_DECLARE_PRIVATE( AQAction )

public:

  /**
  Para sólo poder crear punteros a AQAction compartidos y evitar así
  que se usen punteros planos. De esta forma se puede referenciar una misma 
  acción en varios objetos y se destruirá automáticamente cuando no
  ya no existan objetos que la referencien.

  @param actionName Nombre un objeto AQActionMD con la meta-información de la acción. 
  */
  static AQSharedAction create( const QString & actionName = QString(),
                                const QString & connName = "default" );

  /**
  Tiene el mismo cometido que el método anterior.

  Se puede usar para definir la acción como detalle de otra acción maestra

  @param actMaster         Acción maestra
  @param relMasterToDetail Meta-información que describe la relación entre la acción maestra indicada y la
                           acción que se quiere crear
  */
  static AQSharedAction create( AQSharedAction actMaster,
                                AQRelationMD * relMasterToDetail,
                                const QString & connName = "default" );

  /**
  @return Un nuevo formulario al que se le asigna esta acción y listo para usarse.
          Mediante los métodos setUi* de AQFormDB se pueden establecer distintas 
          interfaces personalizadas para el formulario de forma dinámica.
          El formulario devuelto no tiene activa ninguna interfaz.
  */
  AQFormDB * form();

  /**
  Construye un nuevo formulario y lo muestra en forma modal en un nuevo bucle de eventos.

  Auxiliar ofrecida por ergonomía.

  @param fieldName    Nombre de un campo del cursor.
  @param actionNameUi Nombre de la meta-información de una acción a usar para construir el
                      formulario en vez de usar la definida por el constructor. 
                      Usar con precaución, esto fuerza a utilizar una interfaz gráfica que no
                      tiene por qué ser coherente con el cursor mantenido a bajo nivel por la acción. 
  @return Si fieldName no es vacio, el valor del campo indicado en el registro posicionado cuando se 
          acepta el formulario. El valor devuelto será no válido si el usuario cancela el formulario.
  */
  QVariant exec( const QString & fieldName = QString(), const QString & actionNameUi = QString() );

  /**
  @return Meta-información de la acción
  */
  AQActionMD * action() const;

  /**
  @return Cursor de bajo nivel asociado. Las acciones mantienen según lo indicado
          en la meta-información un cursor de registros de una tabla o tablas de la base
          de datos.
  */
  AQSqlCursor * cursor() const;

  /**
  @return Acción maestra, 0 si no existe.
  */
  AQSharedAction aqActionMaster() const;

  /**
  @return Acción asociada a un campo del cursor, 0 si no existe. 
          Los campos del cursor que están relacionados con otros, tienen asignada una acción
          que gestiona esa relación, mediante este método es posible obtenerla.
  */
  AQSharedAction aqActionField( const QString & fieldName );

  /**
  @return Nombre de la meta-información. Ofrecida por ergonomía.
  */
  QString actName() const;

  /**
  @return Nombre de la conexión de la base de datos que utiliza esta acción. Ofrecida por ergonomía.
  */
  QString connName() const;

  /**
  Para obtener la accion que controla el cursor indicado.

  Si el cursor se ha creado explícitamente y no está controlado por una acción,
  se creará una nueva que lo tomará en propiedad y se encargará de destruirlo.
  Por lo tanto una vez que se ha llamado este método el cursor solo debería ser
  accedido a través de la acción con AQAction::cursor(). 

  Este método no se debe usar en nuevo código, sólo se mantiene para facilitar la portabilidad 
  de código desde AbanQ v2. Es muy probable que desaparezca en nuevas versiones.
  */
  static AQSharedAction fromSqlCursor( AQSqlCursor * cur );

#ifdef AQ_WARN
  static long countRefAction;
#endif

public Q_SLOTS:

  /**
  Activa la acción.

  La activación de un acción realizará distintas operaciones que varian según el contexto.

  No es necesario conocer que hará la acción a priori en un momento dado, ya que está diseñada para 
  ocultar la complejidad de las interacciones entre las distintas capas, basta con saber que
  siempre hará lo correcto según cada situación.

  @param readonly Uso interno
  */
  void activate( bool readonly );

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

Q_SIGNALS:

  /**
  Señal emitida cuando la acción abrió un formulario modal ( con exec() ) y fue aceptado por el
  usuario seleccionando un registro
  */
  void accepted();

private Q_SLOTS:

  /**
  Uso interno
  */
  void requestRecord();
  void syncPos( const QString & fieldName = QString() );

private:

  friend class AQFormDB;

  /**
  Constructor

  @param actionName Nombre un objeto AQActionMD con la meta-información de la acción.
  */
  AQAction( const QString & actionName = QString(),
            const QString & connName = "default",
            bool doInit = true );

  /**
  Constructor
   
  Define la acción como detalle de otra acción maestra

  @param actMaster         Acción maestra
  @param relMasterToDetail Meta-información que describe la relación entre la acción maestra indicada y la
                           acción que se quiere crear
  */
  AQAction( AQSharedAction actMaster,
            AQRelationMD * relMasterToDetail,
            const QString & connName = "default" );
};

#endif /*AQACTION_H_*/
