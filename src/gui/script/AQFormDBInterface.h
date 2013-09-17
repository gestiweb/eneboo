/***************************************************************************
AQFormDBInterface.h
-------------------
begin                : 25/09/2007
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

#ifndef AQFORMDBINTERFACE_H_
#define AQFORMDBINTERFACE_H_

#include <AQScript/AQComponentDBInterface.h>
#include <AQUi/AQFormDB.h>

#ifdef AQ_OBJ_FACTORY_IMPL
static void deleteSharedAQFormDB( AQFormDB * obj )
{
  if ( !obj->parent() )
    obj->deleteLater();
}
#endif

class AQSqlCursorInterfaceAQ2;
class AQActionInterface;

/**
Interfaz para AQFormDB
*/ 
/**
Formularios de AbanQ.
 
Un formulario se podrá asociar a una acción (AQAction)
que será utilizada para construir, levantar y enlazar a
la base de datos la interfaz gráfica de usuario.
 
Un formulario podrá tener activa en un momento dado una
interfaz de usuario que se puede construir dinámicamente
según la información de la acción o que se puede indicar
explícitamente con lo métodos set*Ui.
 
De forma general un formulario podrá ser de tipo maestro
o de tipo registro, dependiendo si visualiza un sólo registro
en forma detallada o si visualiza un conjunto de registro en
forma de lista. 
 
@author InfoSiAL S.L.
*/
class AQFormDBInterface : public AQComponentDBInterface
{
  Q_OBJECT

  Q_PROPERTY( bool ignoreCloseEvent READ ignoreCloseEvent WRITE setIgnoreCloseEvent )
  Q_PROPERTY( bool deleteOnClose READ deleteOnClose WRITE setDeleteOnClose )

public:

  QSharedPointer<AQFormDB> obj_;

public Q_SLOTS:

  /**
  Establece la accion asociada al formulario

  Es necesario asignar una acción al formulario para que este pueda 
  construir, levantar y enlazar a la base de datos la interfaz gráfica de usuario.

  @param act Accion del tipo AQAction a utilizar. Si es vacia desvincula.
  */
  void setAQAction( AQActionInterface * act );

  /**
  @return Objeto AQAction usada por el componente.
  */
  AQActionInterface * aqAction() const;

  /**
  Asigna explícitamente la interfaz a usar para el formulario 
  en tipo maestro.

  @param uiMaster Interfaz de usuario a usar en tipo maestro. El formulario toma
                  en propiedad esta interfaz y puede ser invalidada en cualquier
                  momento
  */
  void setUiMaster( QWidget * uiMaster );

  /**
  Sobrecarga del método anterior, para utilizar el nombre de un fichero .ui
  con el que se construye la interfaz

  @param uiFileName Nombre del fichero de descripción de interfaz, incluyendo 
                    la extension .ui, p.e. clientes.ui
  */
  void setUiMaster( const QString & uiFileName );

  /**
  @return La interfaz de usuario usada en tipo maestro
  */
  QWidget * uiMaster() const;

  /**
  Asigna explícitamente la interfaz a usar para el formulario
  en tipo registro.

  @param uiMaster Interfaz de usuario a usar en tipo registro. El formulario toma
                  en propiedad esta interfaz y puede ser invalidada en cualquier
                  momento 
  */
  void setUiRecord( QWidget * uiRecord );

  /**
  Sobrecarga del método anterior, para utilizar el nombre de un fichero .ui
  con el que se construye la interfaz

  @param uiFileName Nombre del fichero de descripción de interfaz, incluyendo 
                    la extension .ui, p.e. clientes.ui
  */
  void setUiRecord( const QString & uiFileName );

  /**
  @return La interfaz de usuario usada en tipo registro
  */
  QWidget * uiRecord() const;

  /**
  @return La intefaz de usuario actualmente activa
  */
  QWidget * uiActive() const;

  /**
  Activa en el formulario la interfaz de usuario correspondiente al tipo maestro 
  indicada en la acción o establecida explícitamente con setMasterUi.

  La interfaz activada y todos sus componetes del tipo AQComponentDB se vinculan a la accion 
  establecida para el formulario.

  Para desactivar la interfaz, la accion asociada se debe establecer a vacia con setAQAction( AQSharedAction() ).
  */
  void activateMasterUi();

  /**
  Esencialmente hace lo mismo que el método anterior salvo que en este caso
  el formulario se configura como un formulario maestro de detalle, incrustado
  en un formulario de registro.
  */
  void activateDetailUi();

  /**
  Esencialmente hace lo mismo que el método anterior salvo que en este caso
  configura el formulario con la interfaz en tipo registro indicada en la acción
  o establecida explícitamente con setRecordUi.
  */
  void activateRecordUi();

  /**
  Muestra el formulario maestro como un diálogo en un nuevo bucle de eventos
  para esperar a seleccionar registro.

  Se espera el nombre de un campo del cursor devolviendo el valor de dicho 
  campo en el registro posicionado cuando se acepta el formulario
  y un QVariant::Invalid si se cancela.

  @param n Nombre del un campo del cursor del formulario
  @return El valor del campo si se acepta, o QVariant::Invalid si se cancela
  */
  QVariant exec( const QString & n );

  /**
  Muestra el formulario de registro como un diálogo en un nuevo bucle de eventos.
  */
  void exec();

  /**
  Aceptar formulario
  */
  void accept();

  /**
  Cancelar formulario
  */
  void reject();

  /**
  @return El código resultado de ejecutar el formulario, Accepted o Rejected
  */
  int result() const;

protected Q_SLOTS:

  /**
  Establece que se ignore o no el evento de cerrar ventana.

  Aunque se ignore el evento de cerrar, se seguirá emitiendo la señal closeRequest,
  que se puede caputar para realizar accciones antes de cerrar y/o delegar
  el cierre y destrucción del formulario en otro objeto externo

  Por defecto el formulario tiene establecido setIgnoreCloseEvent( false )
  */
  void setIgnoreCloseEvent( bool ign = true );

  /**
  @return Si el formulario ignora el evento de cerrar
  */
  bool ignoreCloseEvent() const;

  /**
  Establece que el formulario sea destruido ( con deleteLater() ) o no cuando este sea cerrado.

  Si se estableció setIgnoreCloseEvent( true ) o el formulario tiene un padre, la destrucción no
  se llevará a cabo aunque se indique lo contrario llamando a este método

  Por defecto el formulario tiene establecido setDeleteOnClose( false )
  */
  void setDeleteOnClose( bool del = true );

  /**
  @return Si el formulario se destruye al cerrarlo
  */
  bool deleteOnClose() const;

private Q_SLOTS:

  /**
  Uso interno
  */
  void emitCloseRequest();

Q_SIGNALS:

  /**
  Señal emitida justo antes de cerrar el formulario. Usando ignoreCloseEvent
  permite controlar como se debe cerrar el formulario
  */
  void closeRequest( AQFormDBInterface * form = 0 );

  /**
  Cuando el formulario actua como un diálogo modal emite estas señales
  */
  void accepted();
  void rejected();

protected:

  friend class AQUiObjectFactoryQSA;

  /**
  Constructor
  */
  AQFormDBInterface( const AQScriptEngineQSA * eng,
                     QSharedPointer<AQFormDB> aqFormDB );

  /**
  Constructor
  */
  AQFormDBInterface( const AQScriptEngineQSA * eng, bool noUi = false );

  /**
  Destructor
  */
  virtual ~AQFormDBInterface();

private:

  void connects() const;
};



/**
Interfaz de soporte para AbanQ v2
*/
class AQFormDBInterfaceAQ2 : public AQFormDBInterface
{
  Q_OBJECT

  Q_PROPERTY( AQActionInterface* aqAction READ aqAction WRITE setAQAction SCRIPTABLE false )

public:

  AQFormDBInterfaceAQ2( const AQScriptEngineQSA * eng,
                        QSharedPointer<AQFormDB> aqFormDB );

  AQFormDBInterfaceAQ2( const AQScriptEngineQSA * eng, bool noUi = false );

  AQFormDBInterfaceAQ2( const AQScriptEngineQSA * eng,
                        const QString & a );

  AQFormDBInterfaceAQ2( const AQScriptEngineQSA * eng,
                        AQSqlCursorInterfaceAQ2 * c,
                        const QString & a );

public Q_SLOTS:

  void setCursor( AQSqlCursorInterfaceAQ2 * cursor );

  AQSqlCursorInterfaceAQ2 * cursor() const;

  virtual QWidget * mainWidget() const;

  virtual void setMainWidget( QWidget * widget );

  virtual void setMainWidget();

  virtual void setMainWidget( const QString & uiFileName );

  AQFormDB * obj() const;

  void emitClosed();

  /*QImage*/
  void snapShot()
  {
    AQ_NO_PORT_AQ2( AQFormDB::snapShot );
  }

  void saveSnapShot( const QString & )
  {
    AQ_NO_PORT_AQ2( AQFormDB::saveSnapShot );
  }

  void showForDocument()
  {
    AQ_NO_PORT_AQ2( AQFormDB::showForDocument );
  }

  void setMaximized();

Q_SIGNALS:

  void closed();

private:

  friend class AQScriptEngineQSA;

  AQSqlCursor * cursorNoUi_;
};


class AQFormRecordDBInterfaceAQ2 : public AQFormDBInterfaceAQ2
{
  Q_OBJECT

  Q_PROPERTY( AQActionInterface* aqAction READ aqAction WRITE setAQAction SCRIPTABLE false )

public:

  AQFormRecordDBInterfaceAQ2( const AQScriptEngineQSA * eng,
                              QSharedPointer<AQFormDB> aqFormDB ) :
      AQFormDBInterfaceAQ2( eng, aqFormDB )
  {}

  AQFormRecordDBInterfaceAQ2( const AQScriptEngineQSA * eng, bool noUi = false ) :
      AQFormDBInterfaceAQ2( eng, noUi )
  {}

  AQFormRecordDBInterfaceAQ2( const AQScriptEngineQSA * eng,
                              AQSqlCursorInterfaceAQ2 * c,
                              const QString & a ) :
      AQFormDBInterfaceAQ2( eng, c, a )
  {}

public Q_SLOTS:

  virtual QWidget * mainWidget() const;

  virtual void setMainWidget( QWidget * widget );

  virtual void setMainWidget();

  virtual void setMainWidget( const QString & uiFileName );

  void acceptContinue()
  {
    AQ_NO_PORT_AQ2( AQFormRecordDB::acceptContinue );
  }
};


class AQFormSearchDBInterfaceAQ2 : public AQFormDBInterfaceAQ2
{
  Q_OBJECT

  Q_PROPERTY( AQActionInterface* aqAction READ aqAction WRITE setAQAction SCRIPTABLE false )

public:

  AQFormSearchDBInterfaceAQ2( const AQScriptEngineQSA * eng,
                              QSharedPointer<AQFormDB> aqFormDB ) :
      AQFormDBInterfaceAQ2( eng, aqFormDB )
  {}

  AQFormSearchDBInterfaceAQ2( const AQScriptEngineQSA * eng, bool noUi = false ) :
      AQFormDBInterfaceAQ2( eng, noUi )
  {}

  AQFormSearchDBInterfaceAQ2( const AQScriptEngineQSA * eng,
                              const QString & a ) :
      AQFormDBInterfaceAQ2( eng, a )
  {}

  AQFormSearchDBInterfaceAQ2( const AQScriptEngineQSA * eng,
                              AQSqlCursorInterfaceAQ2 * c,
                              const QString & a ) :
      AQFormDBInterfaceAQ2( eng, c, a )
  {}

public Q_SLOTS:

  bool accepted() const;
};

#endif /*AQFORMDBINTERFACE_H_*/
