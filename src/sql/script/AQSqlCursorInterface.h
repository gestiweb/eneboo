/***************************************************************************
AQSqlCursorInterface.h
-------------------
begin                : 27/09/2007
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

#ifndef AQSQLCURSORINTERFACE_H_
#define AQSQLCURSORINTERFACE_H_

#include <AQScript/AQObjectInterfaceQSA.h>
#include <AQSql/AQSqlCursor.h>

#ifdef AQ_OBJ_FACTORY_IMPL
static void deleteSharedAQSqlCursor( AQSqlCursor * obj )
{
  if ( !obj->parent() )
    obj->deleteLater();
}
#endif

/**
Interfaz para AQSqlCursor
*/
class AQSqlCursorInterface : public AQObjectInterfaceQSA
{
  Q_OBJECT

  Q_PROPERTY( QString mainFilter READ mainFilter WRITE setMainFilter )
  Q_PROPERTY( int modeAccess READ modeAccess WRITE setModeAccess )
  Q_PROPERTY( int size READ size )
  Q_PROPERTY( QString table READ table )
  Q_PROPERTY( bool valid READ isValid )
  Q_PROPERTY( bool undefined READ isUndefined )

public:

  QSharedPointer<AQSqlCursor> obj_;

public Q_SLOTS:

  /**
  Establece el valor de un campo del buffer con un valor.

  @param fN Nombre del campo
  @param v  Valor a establecer para el campo
  */
  void setValueBuffer( const QString & fN, const QVariant & v ) const;

  /**
  Devuelve el valor de un campo del buffer.

  @param fN Nombre del campo
  */
  QVariant valueBuffer( const QString & fN ) const;

  /**
  Devuelve el valor de un campo del buffer copiado antes de sufrir cambios.

  @param fN Nombre del campo
  */
  QVariant valueBufferCopy( const QString & fN ) const;

  /**
  Devuelve si el contenido de un campo en el buffer en nulo.

  @param name Nombre del campo en el buffer
  */
  bool isNull ( const QString & name ) const;

  /**
  Establece que el contenido de un campo en el buffer sea nulo.

  @param name Nombre del campo en el buffer
  */
  void setNull ( const QString & name ) const;

  /**
  Devuelve si el contenido de un campo en el bufferCopy en nulo.

  @param name Nombre del campo en el bufferCopy
  */
  bool isCopyNull ( const QString & name ) const;

  /**
  Establece que el contenido de un campo en el bufferCopy sea nulo.

  @param name Nombre del campo en el bufferCopy
  */
  void setCopyNull ( const QString & name ) const;

  /**
  Dice si un campo está deshabilitado.

  Un campo estará deshabilitado, porque está clase le dará un valor automáticamente.
  Estos campos son lo campos que están en una relación con otro cursor, por lo que
  su valor lo toman del campo foráneo con el que se relacionan. 

  @param  fN  Nombre del campo a comprobar
  @return TRUE si está deshabilitado y FALSE en caso contrario
  */
  bool fieldDisabled( const QString & fN ) const;

  /**
  Activa o desactiva los chequeos de integridad referencial.

  @param a TRUE los activa y FALSE los desactiva
  */
  void setActivatedCheckIntegrity( bool a ) const;

  /**
  Activa o desactiva las acciones a realizar antes y después de un commit

  @param a TRUE las activa y FALSE las desactiva
  */
  void setActivatedCommitActions( bool a ) const;

  /**
  Realiza comprobaciones de intregidad.

  Se comprueba la integridad referencial al intentar borrar, tambien se comprueba la no duplicidad de
  claves primarias y si hay nulos en campos que no lo permiten cuando se inserta o se edita.
  Si alguna comprobacion falla muestra un cuadro de diálogo con el tipo de fallo encontrado y el metodo
  devuelve FALSE.

  @param  showError Si es TRUE muestra el cuadro de dialogo con el error que se produce al no
                    pasar las comprobaciones de integridad
  @return TRUE si se ha podido entregar el buffer al cursor, y FALSE si ha fallado alguna comprobacion
          de integridad
  */
  bool checkIntegrity( bool showError = true ) const;

  /**
  Refresca el contenido del cursor.

  Si no se ha indicado cursor relacionado obtiene el cursor completo, segun la consulta
  por defecto. Si se ha indicado que depende de otro cursor con el que se relaciona,
  el contenido del cursor dependerá del valor del campo que determina la relación.
  Si se indica el nombre de un campo se considera que el buffer sólo ha cambiado en ese
  campo y así evitar repeticiones en el refresco.

  @param fN Nombre del campo de buffer que ha cambiado
  */
  void refresh( const QString & fN = QString() ) const;

  /**
  Refresca el buffer según el modo de acceso establecido.

  Lleva informacion del cursor al buffer para editar o navegar, o prepara el buffer para
  insertar o borrar.

  Si existe un campo contador se invoca a la función "calculateCounter" del script del
  contexto (ver AQSqlCursor::ctxt_) establecido para el cursor. A esta función se le pasa
  como argumento el nombre del campo contador y debe devolver el valor que debe contener
  ese campo.
  */
  bool refreshBuffer() const;

  /**
  Devuelve la posición del registro actual del cursor.

  Ver QSqlCursor::at()
  */
  int at() const;

  /**
  Redefinición del método seek() de QSqlCursor.

  Este método simplemente invoca al método seek() original de QSqlCursor() y refresca
  el buffer con el metodo AQSqlCursor::refreshBuffer().

  @param emit Si TRUE emite la señal AQSqlCursor::currentChanged()
  */
  bool seek( int i, bool relative = false, bool emite = false ) const;

  /**
  Redefinición del métedo next() de QSqlCursor.

  Este método simplemente invoca al método next() original de QSqlCursor() y refresca el
  buffer con el metodo AQSqlCursor::refreshBuffer().

  @param emit Si TRUE emite la señal AQSqlCursor::currentChanged()
  */
  bool next( bool emite = true ) const;

  /**
  Redefinición del método prev() de QSqlCursor.

  Este método simplemente invoca al método prev() original de QSqlCursor() y refresca
  el buffer con el metodo AQSqlCursor::refreshBuffer().

  @param emit Si TRUE emite la señal AQSqlCursor::currentChanged()
  */
  bool prev( bool emite = true ) const;

  /**
  Redefinición del método first() de QSqlCursor.

  Este método simplemente invoca al método first() original de QSqlCursor() y refresca el
  buffer con el metodo AQSqlCursor::refreshBuffer().

  @param emit Si TRUE emite la señal AQSqlCursor::currentChanged()
  */
  bool first( bool emite = true ) const;

  /**
  Redefinición del método last() de QSqlCursor.

  Este método simplemente invoca al método last() original de QSqlCursor() y refresca el
  buffer con el metodo AQSqlCursor::refreshBuffer().

  @param emit Si TRUE emite la señal AQSqlCursor::currentChanged()
  */
  bool last( bool emite = true ) const;

  /**
  Redefinición del método del() de QSqlCursor.

  Este método invoca al método del() original de QSqlCursor() y comprueba si hay borrado
  en cascada, en caso afirmativo borrar también los registros relacionados en cardinalidad 1M.
  */
  int del( bool invalidate = true ) const;

  /**
  Redefinición del método select() de QSqlCursor
  */
  bool select( const QString & filter ) const;

  /**
  Redefinicion del método select() de QSqlCursor
  */
  bool select() const;

  /**
  Obtiene la posición del registro actual, según la clave primaria contenida en el buffer.

  La posición del registro actual dentro del cursor se calcula teniendo en cuenta el
  filtro actual ( AQSqlCursor::curFilter() ) y el campo o campos de ordenamiento
  del mismo ( Q3SqlCursor::sort() ).
  Este método es útil, por ejemplo, para saber en que posición dentro del cursor 
  se ha insertado un registro.

  @return Posición del registro dentro del cursor.
  */
  int atFrom() const;

  /**
  Obtiene la posición dentro del cursor del primer registro que en el campo indicado
  empieze con el valor solicitado. Supone que los registros están ordenados por dicho
  campo, para realizar una búsqueda binaria.

  La posición del registro actual dentro del cursor se calcula teniendo en cuenta el
  filtro actual ( AQSqlCursor::curFilter() ) y el campo o campos de ordenamiento
  del mismo ( QSqlCursor::sort() ).
  Este método es útil, por ejemplo, para saber en que posición dentro del cursor
  se encuentra un registro con un cierto valor en un campo.

  @param  fN        Nombre del campo en el que buscar el valor
  @param  v         Valor a buscar ( mediante like 'v%' )
  @param  orderAsc  TRUE (por defecto) si el orden es ascendente, FALSE si es descendente
  @return Posición del registro dentro del cursor, o -1 si no encuentra coincidencia.
  */
  int atFromBinarySearch( const QString & fN, const QString & v, bool orderAsc = true ) const;

  /**
  Manda el contenido del buffer al cursor, o realiza la acción oportuna para el cursor.

  Todos los cambios realizados en el buffer se hacen efectivos en el cursor al invocar este método.
  La manera de efectuar estos cambios viene determinada por el modo de acceso establecido para
  el cursor, ver AQSqlCursor::Mode, si el modo es editar o insertar actualiza con los nuevos valores de
  los campos del registro, si el modo es borrar borra el registro, y si el modo es navegacion no hace nada.
  Antes de nada tambien comprueba la integridad referencial invocando al método AQSqlCursor::checkIntegrity().

  Si existe un campo calculado se invoca a la función "calculateField" del script del
  contexto (ver AQSqlCursor::ctxt_) establecido para el cursor. A esta función se le pasa
  como argumento el nombre del campo calculado y debe devolver el valor que debe contener
  ese campo, p.e. si el campo es el total de una factura y de tipo calculado la función
  "calculateField" debe devolver la suma de lineas de las facturas mas/menos impuestos y
  descuentos.

  @return TRUE si se ha podido entregar el buffer al cursor, y FALSE si ha fallado la entrega
  */
  bool commitBuffer( bool = false ) const;

  /**
  Manda el contenido del buffer del cursor maestro relacionado a dicho cursor.

  Hace efectivos todos los cambios en el buffer del cursor relacionado posiconándose en el registro
  correspondiente que recibe los cambios.
  */
  bool commitBufferCursorMaster() const;

  /**
  Devuelve el cursor maestro.
  */
  AQSqlCursorInterface * cursorMaster() const;

  /**
  Establece los campos unlock de un registro, saltándose las comprobación

  @param fN Nombre del campo
  @param v  Valor para el campo unlock
  */
  void setUnLock( const QString & fN, bool v ) const;

  /**
  Para comprobar si el registro actual del cursor está bloqueado.

  @return TRUE si está bloqueado, FALSE en caso contrario.
  */
  bool isLocked() const;

  /**
  Devuelve el tipo de un campo de la tabla

  @return Entero con el código del tipo de dato del campo
  */
  int fieldType( const QString & fN ) const;

  /**
  Devuelve la clave primaria de la tabla asociada al cursor

  @return Cadena que contiene el nombre del campo que es clave primaria de la tabla
  */
  QString primaryKey() const;

  /**
  Indica si el contenido actual del buffer difiere de la copia guardada.

  Ver AQSqlCursor::bufferCopy_ 

  @return TRUE si el buffer y la copia son distintas, FALSE en caso contrario
  */
  bool isModifiedBuffer() const;

  /**
  Para emitir explícitamente las señales
  */
  void emitNewBuffer();
  void emitBufferChanged( const QString & v );
  void emitCursorUpdated();
  void emitCurrentChanged( int v );
  void emitBufferCommited();

protected Q_SLOTS:

  /**
  @return Si la interfaz es indefinida, es decir, no tiene objeto subyacente
  */
  bool isUndefined() const;

  /**
  Obtiene el modo de acceso actual del cursor.

  @return Constante AQSql::Mode que define el modo de acceso actual del cursor
  */
  int modeAccess() const;

  /**
  Establece el modo de acceso para el cursor.

  @param m Constante AQSql::Mode que indica en que modo de acceso se quiere establecer el cursor
  */
  void setModeAccess( int m );

  /**
  Obtiene el filtro principal del cursor.

  @return Cadena de texto con el filtro principal
  */
  QString mainFilter() const;

  /**
  Establece el filtro principal del cursor.

  @param f Cadena con el filtro, corresponde con una clausura WHERE
  */
  void setMainFilter( const QString & f ) const;

  /**
  Devuelve el nombre de la tabla asociada al cursor

  @return Cadena que contiene la tabla asociada al cursor
  */
  QString table() const;

  /**
  Devuelve el número de filas seleccionadas en el cursor

  @return Número de filas
  */
  int size() const;

  /**
  QSqlCursor::isValid()
  */
  bool isValid() const;

Q_SIGNALS:

  /**
  Indica que se ha cargado un nuevo buffer
  */
  void newBuffer();

  /**
  Indica si ha cambiado un campo del buffer, junto con la señal se envía el nombre del campo que
  ha cambiado.
  */
  void bufferChanged( const QString & );

  /**
  Indica que se ha actualizado el cursor
  */
  void cursorUpdated();

  /**
  Indica que la posición del registro activo dentro del cursor ha cambiado
  */
  void currentChanged( int );

  /**
  Indica que se ha realizado un commit
  */
  void bufferCommited();

protected:

  friend class AQSqlObjectFactoryQSA;

  /**
  Constructor
  */
  AQSqlCursorInterface( const AQScriptEngineQSA * eng,
                        QSharedPointer<AQSqlCursor> aqCursor );

  /**
  Constructor
  */
  AQSqlCursorInterface( const AQScriptEngineQSA * eng,
                        const QString & n = QString(),
                        const QString & connName = "default" );

  /**
  Destructor
  */
  virtual ~AQSqlCursorInterface();

  void connects() const;
};

inline bool AQSqlCursorInterface::isUndefined() const
{
  return !obj_;
}


/**
Interfaz de soporte para AbanQ v2
*/
class AQSqlCursorInterfaceAQ2 : public AQSqlCursorInterface
{
  Q_OBJECT

  Q_ENUMS( Mode )
  Q_ENUMS( AcosConditionEval )

  Q_PROPERTY( AQSqlCursorInterface* cursorMaster READ cursorMaster SCRIPTABLE false )

public:

  /**
  Constantes para indicar el tipo de acceso al cursor
  */
  enum Mode {
    /** Insertar, en este modo el buffer se prepara para crear un nuevo registro */
    Insert = AQSql::INSERT,
    /** Edición, en este modo el buffer se prepara para editar el registro activo */
    Edit = AQSql::EDIT,
    /** Borrar, en este modo el buffer se prepara para borrar el registro activo */
    Del = AQSql::DEL,
    /** Navegacion, en este modo solo se puede visualizar el buffer */
    Browse = AQSql::BROWSE,
    /** Insertar copiando actual, si el buffer actual es vacio funciona igual que INSERT
        sino es vacio primero lo guarda, prepara el buffer para crear un nuevo registro y
        despues copia el contenido del anteriormente guardado */
    InsertCopy = AQSql::INSERTCOPY
  };

  /**
  Constantes para indicar el tipo de condicion para evaluar si se aplica o no el control de acceso
  */
  enum AcosConditionEval {
    /** evalua un valor fijo */
    Value = AQSql::VALUE,
    /** evalua una expresion regular */
    Regexp = AQSql::REGEXP,
    /** evalua el valor devuelto por una funcion de script */
    Function = AQSql::FUNCTION
  };

  AQSqlCursorInterfaceAQ2( const AQScriptEngineQSA * eng,
                           QSharedPointer<AQSqlCursor> aqCursor );

  AQSqlCursorInterfaceAQ2( const AQScriptEngineQSA * eng,
                           const QString & n = QString(),
                           const QString & connName = "default" ) :
      AQSqlCursorInterface( eng, n, connName ),
      edition_( true ),
      browse_( true )
  {
    AQ_PORT_AQ2_NOP( FLSqlCursor, AQSqlCursor; );
  }

public Q_SLOTS:

  bool setEditMode();

  void setAtomicValueBuffer( const QString &, const QString & )
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::setAtomicValueBuffer );
  }

  void setEdition( bool b );

  void setBrowse( bool b );

  bool inTransaction() const
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::inTransaction );
    return true;
  }

  bool transaction( bool )
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::transaction );
    return true;
  }

  bool rollback()
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::rollback );
    return true;
  }

  bool commit()
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::commit );
    return true;
  }

  void setAskForCancelChanges( bool )
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::setAskForCancelChanges );
  }

  bool commitBufferCursorRelation();

  AQSqlCursorInterfaceAQ2 * cursorRelation() const;

  void setContext( QObject * )
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::setContext );
  }

  QObject * context() const
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::context );
    return 0;
  }

  AQSqlCursor * obj() const;

  void emitRecordChoosed();

  void emitAutoCommit();

  QString action() const;

  void setAction( const QString & actionNameUi );

  void editRecord();

  void insertRecord();

  void browseRecord();

  void chooseRecord();

  int transactionLevel() const
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::transactionLevel );
    return -1;
  }

  /*QStringList*/
  void transactionsOpened()
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::transactionsOpened );
  }

  void rollbackOpened( int = -1, const QString & = QString() )
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::rollbackOpened );
  }

  void commitOpened( int = -1, const QString & = QString() )
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::commitOpened );
  }

  void checkRisksLocks( bool = false )
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::checkRisksLocks );
  }

  void setAcTable( const QString & )
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::setAcTable );
  }

  void setAcosTable( const QStringList & )
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::setAcosTable );
  }

  void setAcosCondition( const QString &, int, const QVariant & )
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::setAcosCondition );
  }

  void clearAcosCondition()
  {
    AQ_NO_PORT_AQ2( AQSqlCursor::clearAcosCondition );
  }

  void setForwardOnly( bool );

Q_SIGNALS:

  void recordChoosed();
  void autoCommit();

private:

  bool edition_;
  bool browse_;
  QString actionNameUi_;
};

Q_DECLARE_METATYPE( AQSqlCursorInterface* )
Q_DECLARE_METATYPE( AQSqlCursorInterfaceAQ2* )

#endif /*AQSQLCURSORINTERFACE_H_*/
