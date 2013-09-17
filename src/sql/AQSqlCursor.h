/***************************************************************************
           AQSqlCursor.h
        -------------------
begin                : 08/08/2007
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

#ifndef AQSQLCURSOR_H_
#define AQSQLCURSOR_H_

#include <AQSql/AQSql.h>
#include <AQSql/q3sqlcursor.h>

#include <QObject>

class AQSqlCursorPrivate;
class AQSqlDatabase;
class AQTableMD;
class AQRelationMD;

/**
Esta clase es una extensión de la clase Q3SqlCursor
para la necesidades especificas de AbanQ.
 
Hereda de Q3SqlCursor, y añade ciertas funcionalidades.
Esta clase se utiliza para gestionar los registros de
una sola tabla. Tambien mantiene una copia de los metadatos
(ver AQTableMD) de dicha tabla.
 
También puede ser utilizada para manejar conjuntos de registros
provenientes de una consulta, si la definición de los metadatos
corresponden a una consulta. En este caso las operaciones sobre
el cursor se realizarán sobre la tabla principal de la consulta,
ver AQTableMD::query().
 
El funcionamiento de esta clase radica en el modo de acceso
en el que se encuentre, INSERTAR,MODIFICAR,BORRAR,NAVEGAR.
Se mantiene siempre un registro completo del cursor, en un buffer,
dependiendo del modo de acceso, este buffer contendrá un registro
listo para INSERTAR en blanco, para MODIFICAR con el contenido
del registro actual del cursor, para BORRAR con el contenio del registro
actual listo para borrar, o para NAVEGAR con el contenido del registro
actual en módo de sólo lectura.
 
Las acciones asociadas a INSERTAR, MODIFICAR o BORRAR, son llevadas
a cabo cuando se envía el contenido del buffer de nuevo al cursor, 
con AQSqlCursor::commitBuffer, esta acción lleva implicito un chequeo de
integridad, ver AQSqlCursor::checkIntegrity.
 
@author InfoSiAL S.L.
*/
class AQSqlCursor: public QObject, public Q3SqlCursor
{
  Q_OBJECT

public:

  /**
  constructor.

  @param name              Nombre de la tabla
  @param autopopulate      Si es cierto carga la informacion de los campos de la tabla en la base de datos
  @param connName          Nombre de la conexion
  @param cursorMaster      Cursor maestro, si este actua como detalle.
  @param relDetailToMaster Descripcion de la relacion entre este cursor que actua como detalle, con el cursor
                           que actua como maestro
  */
  AQSqlCursor( const QString & name = QString(), bool autopopulate = true,
               const QString & connName = "default", AQSqlCursor * cursorMaster = 0,
               AQRelationMD * relDetailToMaster = 0, QObject * parent = 0 );

  /**
  destructor.
  */
  ~AQSqlCursor();

  /**
  Para obtener los metadatos de la tabla.

  @return Objeto AQTableMD con los metadatos de la tabla asociada al cursor
  */
  const AQTableMD * metadata() const;

  /**
  Para obtener los metadatos de la tabla.

  Version no constante, aplica ACL ( control de acceso )

  @return Objeto AQTableMD con los metadatos de la tabla asociada al cursor
  */
  AQTableMD * metadata();

  /**
  Para obtener el filtro principal del cursor.

  @return Cadena de texto con el filtro principal
  */
  QString mainFilter() const;

  /**
  Establece el filtro principal del cursor.

  @param f Cadena con el filtro, corresponde con una clausura WHERE
  */
  void setMainFilter( const QString & f );

  /**
  Para obtener el modo de acceso actual del cursor.

  @return Constante AQSql::Mode que define en que modo de acceso esta preparado
          el buffer del cursor
  */
  AQSql::Mode modeAccess() const;

  /**
  Establece el modo de acceso para el cursor.

  @param m  Constante AQSql::Mode que indica en que modo de acceso
            se quiere establecer el cursor
  */
  void setModeAccess( AQSql::Mode m );

  /**
  Establece el valor de un campo del buffer con un valor.

  @param fN Nombre del campo
  @param v  Valor a establecer para el campo
  */
  void setValueBuffer( const QString & fN, const QVariant & v );

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
  Dice si un campo está deshabilitado.

  Un campo estará deshabilitado, porque esta clase le dará un valor automáticamente.
  Estos campos son los que están en una relación con otro cursor, por lo que
  su valor lo toman del campo foráneo con el que se relacionan.

  @param  fN Nombre del campo a comprobar
  @return TRUE si está deshabilitado y FALSE en caso contrario
  */
  bool fieldDisabled( const QString & fN ) const;

  /**
  Copia el contenido del AQSqlCursor::buffer_ actual en AQSqlCursor::bufferCopy_.

  Al realizar esta copia se podra comprobar posteriormente si el buffer actual y la copia realizada
  difieren mediante el metodo AQSqlCursor::isModifiedBuffer().
  */
  void updateBufferCopy() const;

  /**
  Indica si el contenido actual del buffer difiere de la copia guardada.

  Ver AQSqlCursor::bufferCopy_ .

  @return TRUE si el buffer y la copia son distintas, FALSE en caso contrario
  */
  bool isModifiedBuffer() const;

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
  Devuelve el cursor maestro.
  */
  AQSqlCursor * cursorMaster() const;

  /**
  Descripcion de la relacion de este cursor, detalle, con el cursor maestro.
  */
  AQRelationMD * relDetailToMaster() const;

  /**
  Desbloquea el registro actual del cursor.

  @param fN Nombre del campo
  @param v  Valor para el campo unlock
  */
  void setUnLock( const QString & fN, bool v );

  /**
  Para comprobar si el registro actual del cursor está bloqueado.

  @return TRUE si está bloqueado, FALSE en caso contrario.
  */
  bool isLocked() const;

  /**
  Devuelve si el contenido de un campo en el buffer es nulo.

  @param i Posicion del campo en el buffer
  */
  bool bufferIsNull( int i ) const;

  /**
  Devuelve si el contenido de un campo en el buffer es nulo.

  @param name Nombre del campo en el buffer
  */
  bool bufferIsNull( const QString & name ) const;

  /**
  Establece que el contenido de un campo en el buffer sea nulo.

  @param i Posicion del campo en el buffer
  */
  void bufferSetNull( int i ) const;

  /**
  Establece que el contenido de un campo en el buffer sea nulo.

  @param name Nombre del campo en el buffer
  */
  void bufferSetNull( const QString & name ) const;

  /**
  Devuelve si el contenido de un campo en el bufferCopy en nulo.

  @param i Posicion del campo en el bufferCopy
  */
  bool bufferCopyIsNull( int i ) const;

  /**
  Devuelve si el contenido de un campo en el bufferCopy en nulo.

  @param name Nombre del campo en el bufferCopy
  */
  bool bufferCopyIsNull( const QString & name ) const;

  /**
  Establece que el contenido de un campo en el bufferCopy sea nulo.

  @param i Posicion del campo en el bufferCopy
  */
  void bufferCopySetNull( int i ) const;

  /**
  Establece que el contenido de un campo en el bufferCopy sea nulo.

  @param name Nombre del campo en el bufferCopy
  */
  void bufferCopySetNull( const QString & name ) const;

  /**
  Obtiene la posición del registro actual, según la clave primaria contenida en el buffer.

  La posición del registro actual dentro del cursor se calcula teniendo en cuenta el
  filtro actual ( AQSqlCursor::curFilter() ) y el campo o campos de ordenamiento
  del mismo ( Q3SqlCursor::sort() ).
  Este método es útil, por ejemplo, para saber en que posición dentro del cursor 
  se ha insertado un registro.

  @return Posición del registro dentro del cursor.
  */
  int atFrom();

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
  int atFromBinarySearch( const QString & fN, const QString & v, bool orderAsc = true );

  /**
  Redefinido por conveniencia
  */
  bool exec( const QString & query );

  /**
  Para obtener la base de datos sobre la que trabaja
  */
  AQSqlDatabase db() const;

  /**
  Refresca el buffer segun el modo de acceso establecido.

  Lleva informacion del cursor al buffer para editar o navegar, o prepara el buffer para
  insertar o borrar.

  Si existe un campo contador se invoca a la función "calculateCounter" del script del
  objeto "formRecord" + nombreTabla. A esta función se le pasa como argumento el nombre del 
  campo contador y debe devolver el valor que debe contener ese campo.

  @return TRUE si se ha podido realizar el refresco, FALSE en caso contrario
  */
  bool refreshBuffer();

  /**
  Redefinicion del método seek() de Q3SqlCursor.

  Este método simplemente invoca al método seek() original de Q3SqlCursor() y refresca
  el buffer con el metodo AQSqlCursor::refreshBuffer().

  @param emit Si TRUE emite la señal AQSqlCursor::currentChanged()
  */
  bool seek( int i, bool relative = false, bool emite = false );

  /**
  Redefinicion del método next() de Q3SqlCursor.

  Este método simplemente invoca al método next() original de Q3SqlCursor() y refresca el
  buffer con el metodo AQSqlCursor::refreshBuffer().

  @param emit Si TRUE emite la señal AQSqlCursor::currentChanged()
  */
  bool next( bool emite = true );

  /**
  Redefinicion del método prev() de Q3SqlCursor.

  Este método simplemente invoca al método prev() original de Q3SqlCursor() y refresca
  el buffer con el metodo AQSqlCursor::refreshBuffer().

  @param emit Si TRUE emite la señal AQSqlCursor::currentChanged()
  */
  bool prev( bool emite = true );

  /**
  Redefinicion del método first() de Q3SqlCursor.

  Este método simplemente invoca al método first() original de Q3SqlCursor() y refresca el
  buffer con el metodo AQSqlCursor::refreshBuffer().

  @param emit Si TRUE emite la señal AQSqlCursor::currentChanged()
  */
  bool first( bool emite = true );

  /**
  Redefinicion del método last() de Q3SqlCursor.

  Este método simplemente invoca al método last() original de Q3SqlCursor() y refresca el
  buffer con el metodo AQSqlCursor::refreshBuffer().

  @param emit Si TRUE emite la señal AQSqlCursor::currentChanged()
  */
  bool last( bool emite = true );

  /**
  Redefinicion del método del() de Q3SqlCursor.

  Este método invoca al método del() original de Q3SqlCursor() y comprueba si hay borrado
  en cascada, en caso afirmativo borrar también los registros relacionados en cardinalidad 1M.
  */
  int del( bool invalidate = true );

  /**
  Redefinicion del método select() de Q3SqlCursor
  */
  bool select( const QString & filter, const QSqlIndex & sort = QSqlIndex() );

  /**
  Redefinicion del método sort() de Q3SqlCursor
  */
  void setSort( const QSqlIndex& sort );

  /**
  Redefinicion del método size() de Q3SqlCursor
  */
  int size();

  /**
  Obtiene el filtro base
  */
  QString baseFilter();

  /**
  Obtiene el filtro actual
  */
  QString curFilter();

  /**
  Redefinicion del método setFilter() de Q3SqlCursor
  */
  void setFilter( const QString & filter );

  /**
  Manda el contenido del buffer al cursor, o realiza la acción oportuna para el cursor.

  Todos los cambios realizados en el buffer se hacen efectivos en el cursor al invocar este método.
  La manera de efectuar estos cambios viene determinada por el modo de acceso establecido para
  el cursor, ver AQSqlCursor::Mode, si el modo es editar o insertar actualiza con los nuevos valores de
  los campos del registro, si el modo es borrar borra el registro, y si el modo es navegacion no hace nada.
  Antes de nada tambien comprueba la integridad referencial invocando al método AQSqlCursor::checkIntegrity().

  Si existe un campo calculado se invoca a la función "calculateField" del script del
  objeto "formRecord" + nombreTabla. A esta función se le pasa como argumento el nombre del campo calculado 
  y debe devolver el valor que debe contenerese campo, p.e. si el campo es el total de una factura y de tipo 
  calculado la función "calculateField" debe devolver la suma de lineas de las facturas mas/menos impuestos y
  descuentos.

  @return TRUE si se ha podido entregar el buffer al cursor, y FALSE si ha fallado la entrega
  */
  bool commitBuffer( bool emite = true );

  /**
  Manda el contenido del buffer del cursor maestro relacionado a dicho cursor.

  Hace efectivos todos los cambios en el buffer del cursor relacionado posiconándose en el registro
  correspondiente que recibe los cambios.
  */
  bool commitBufferCursorMaster();

  /**
  Establece el acceso global para la tabla, ver AQSqlCursor::setAcosCondition().

  Este será el permiso a aplicar a todos los campos por defecto

  @param  ac Permiso global; p.e.: "r-", "-w"
  */
  void setAcTable( const QString & ac );

  /**
  Establece la lista de control de acceso (ACOs) para los campos de la tabla, 
  ver AQSqlCursor::setAcosCondition().

  Esta lista de textos deberá tener en sus componentes de orden par los nombres de los campos,
  y en los componentes de orden impar el permiso a aplicar a ese campo,
  p.e.: "nombre", "r-", "descripcion", "--", "telefono", "rw",...

  Los permisos definidos aqui sobreescriben al global.

  @param acos Lista de cadenas de texto con los nombre de campos y permisos.
  */
  void setAcosTable( const QStringList & acos );

  /**
  Establece la condicion que se debe cumplir para aplicar el control de acceso.

  Para cada registro se evalua esta condicion y si se cumple, aplica la regla
  de control de acceso establecida con AQSqlCursor::setAcTable y AQSqlCursor::setAcosTable.

  Ejemplos:

  // valueBuffer( "nombre" ) == "pepe"
  setAcosCondition( "nombre", VALUE, "pepe" );
    
  // QRegExp( "pe*" ).exactMatch( valueBuffer( "nombre" ).toString() )
  setAcosCondition( "nombre", REGEXP, "pe*" );
    
  // call( "sys.checkAcos" ) == true
  setAcosCondition( "sys.checkAcos", FUNCTION, true );
    
  @param  cond      Tipo de evaluacion;
                    VALUE compara con un valor fijo
                    REGEXP compara con una expresion regular
                    FUNCTION compara con el valor devuelto por una funcion de script
    
  @param  condName  Si es vacio no se evalua la condicion y la regla no se aplica nunca.
                    Para VALUE y REGEXP nombre de un campo.
                    Para FUNCTION nombre de una funcion de script.  A la función se le pasa como
                    argumento el objeto cursor.
    
  @param  condVal   Valor que hace que la condicion sea cierta
  */
  void setAcosCondition( const QString & condName, AQSql::AcosConditionEval cond,
                         const QVariant & condVal );

#ifdef AQ_WARN
  static long countRefCursor;
#endif

public Q_SLOTS:

  /**
  Refresca el contenido del cursor.

  Si no se ha indicado cursor relacionado obtiene el cursor completo, segun la consulta
  por defecto. Si se ha indicado que depende de otro cursor con el que se relaciona,
  el contenido del cursor dependerá del valor del campo que determina la relación.
  Si se indica el nombre de un campo se considera que el buffer sólo ha cambiado en ese
  campo y así evitar repeticiones en el refresco.

  @param fN Nombre del campo de buffer que ha cambiado
  */
  void refresh( const QString & fN = QString() );

  /**
  Actualiza el conjunto de registros con un retraso.

  Acepta un lapsus de tiempo en milisegundos, activando el cronómetro interno para
  que realize el refresh definitivo al cumplirse dicho lapsus.

  @param msec Cantidad de tiempo del lapsus, en milisegundos.
  */
  void refreshDelayed( int msec = 0 );


Q_SIGNALS:

  /**
  Indica que se ha cargado un nuevo buffer
  */
  void newBuffer();

  /**
  Indica ha cambiado un campo del buffer, junto con la señal se envía el nombre del campo que
  ha cambiado.
  */
  void bufferChanged( const QString & );

  /**
  Indica que se ha actualizado el cursor, añadiendo o quitando nuevos registros
  */
  void cursorUpdated();

  /**
  Indica que la posicion del registro activo dentro del cursor ha cambiado
  */
  void currentChanged( int );

  /**
  Indica que se ha realizado un commit del buffer interno
  */
  void bufferCommited();

  /**
  Indica que se ha ejecutado una nueva seleccion de registros, se ha ejecutado AQSqlCursor::select
  */
  void newSelect();

private:

  friend class AQAction;
  friend class AQActionPrivate;

  /**
  Para establecer y obtener una referencia de bajo nivel a la acción que controla este cursor.

  Este método no se debe usar en nuevo código, sólo se mantiene para facilitar la portabilidad 
  de código desde AbanQ v2. Es muy probable que desaparezca en nuevas versiones.
  */
  void setAQActionHandler( QObject * aqActionHandler );
  QObject * aqActionHandler() const;

  /**
  Privado
  */
  AQSqlCursorPrivate * d;
};

#endif /*AQSQLCURSOR_H_*/
