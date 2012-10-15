/***************************************************************************
 FLObjectFactory.h
 -------------------
 begin                : jue abr 23 2003
 copyright            : (C) 2003-2008 by InfoSiAL S.L.
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

#ifndef FLOBJECTFACTORY_H
#define FLOBJECTFACTORY_H

#include "FLApplication.h"
#include "FLSqlCursor.h"
#include "FLSqlQuery.h"
#include "FLUtil.h"
#include "FLTableMetaData.h"
#include "FLFormDB.h"
#include "FLFormRecordDB.h"
#include "FLFormSearchDB.h"
#include "FLFieldDB.h"
#include "FLTableDB.h"
#include "FLDataTable.h"
#include "FLManager.h"
#include "FLManagerModules.h"
#include "FLReportViewer.h"
#include "FLReportEngine.h"
#include "FLReportPages.h"
#include "FLCodBar.h"
#include "FLScriptEditor.h"
#include "FLAction.h"
#include "FLVar.h"
#include "FLAccessControlLists.h"
#include "FLSqlDatabase.h"
#include "posprinter.h"
#include "FLSqlConnections.h"
#include "FLSmtpClient.h"
#include "FLImageViewer.h"
#include "FLPicture.h"
#include "FLDiskCache.h"
#include "FLNetwork.h"
#include "FLTable.h"
#include "mimecodec.h"
#include "qextserialport.h"
#include "FLDigiDoc.h"
#include "FLSettings.h"
#include "AQConfig.h"

#include <qsobjectfactory.h>
#include <qapplication.h>
#include <qdatetime.h>
#include <qtable.h>
#include <qlistview.h>
#include <qdom.h>
#include <qtabwidget.h>
#include <qptrdict.h>

class FLDomNodeInterface;
class FLDomNodesCache;
class FLSqlQueryInterface;

#define DN_ ( dn_ ? dn_ : newNode() )


//! Clase general de la aplicación
/**
 Recursos generales de la aplicación.
 Utilizada para abrir formularios, ejecutar scripts, procesar eventos, etc

 @author InfoSiAL S.L.
 */
class  FLApplicationInterface : public QObject
{

  Q_OBJECT
  Q_ENUMS(FLScaleMode)

public:

  enum FLScaleMode {
    ScaleFree,
    ScaleMin,
    ScaleMax
  };

  /**
   construtor
   */
  FLApplicationInterface(FLApplication *obj) : QObject(obj) {
    obj_ = obj;
    connect(obj_, SIGNAL(databaseLockDetected()), this, SIGNAL(databaseLockDetected()));
  }

public slots:

  /**
  Obtiene el widget principal del módulo activo.

  @return Objeto QWidget que corresponde con el widget principal del formulario
  */
  QWidget *mainWidget() const {
    return obj_->mainWidget();
  }

  /**
  @return La ventana principal de un módulo
  */
  QWidget *modMainWidget(const QString &idModulo) const {
    return obj_->modMainWidget(idModulo);
  }

  /**
   Cambia el mensaje de estado en toda la barra de estado temporalmente
   */
  void statusHelpMsg(const QString &text) {
    obj_->statusHelpMsg(text);
  }

  /**
   Abre en el MDI el formulario maestro del tipo FLFormDB asociado a una acción.

   @param n Nombre de la acción asociada al formulario maestro
   @param p Imagen QPixmap que sirve de icono para el formulario
   */
  void openMasterForm(const QString &n, const QPixmap &p = QPixmap()) {
    obj_->openMasterForm(n, p);
  }

  /**
   Ejecuta la función "main" del script "scriptform" de una acción.

   @param n Nombre de la acción
   */
  void execMainScript(const QString &n) {
    obj_->execMainScript(n);
  }

  /**
   Fuerza a que los eventos pendientes de la aplicación sean procesados
   */
  void processEvents() {
    obj_->processEvents();
  }

  /**
   Obtiene una lista de controles de clase c para el formulario wN

   @param wN Nombre del formulario con el formato [Form|Record]<nombre de la acción>
   @param c Nombre de la clase del objeto que se busca

   @return Cadena que contiene la lista con formato nombreObjeto1/descObjeto1*nomObjeto2/descObj2*...
   */
  QString getWidgetList(const QString &wN, const QString &c) {
    return obj_->getWidgetList(wN, c);
  }

  /**
   Obtiene la lista de pestañas que contiene un control QTabWidget

   @param wN Nombre del formulario con el formato [Form|Record]<nombre de la acción>
   @param n Nombre del tabWidget

   @return Cadena que contiene la lista con formato nomPestaña1/descPestaña1*nomPestaña2/descPestaña2*...
   */
  QString getTabWidgetPages(const QString &wN, const QString &n) {
    return obj_->getTabWidgetPages(wN, n);
  }

  /**
   Prefijo de instalación

   @return Cadena que contiene el prefijo de instalación
   */
  QString installPrefix() {
    return AQ_PREFIX;
  }

  /**
   @return El directorio principal utilizado por la caché de disco
   para almacenar los archivos
   */
  QString diskCacheAbsDirPath() {
    return AQ_DISKCACHE_DIRPATH;
  }

  /**
   Devuelve el número de versión en una cadena, por ejemplo "2.3.0" ó "3.0.5"

   @return Cadena que contiene número de versión
   */
  QString version() {
    return AQ_VERSION;
  }

  /**
   Actualiza las areas cargadas en el sistema
   */
  void updateAreas() {
    obj_->initToolBox();
  }

  /**
   Indica si el módulo especificado está cargado en el sistema.

   @param idM Cadena con el nombre del módulo
   @param connName Nombre de la conexion
   @return TRUE en caso de que el módulo esté cargado, FALSE en caso contrario
   */
  bool isLoadedModule(const QString &idM, const QString &connName = "default") {
    return FLSqlConnections::database(connName)->managerModules()->listAllIdModules().contains(idM);
  }

  /**
   Indica si la aplicación se compiló en modo DEBUG

   @return TRUE en caso de que la aplicación se compilara en modo DEBUG, FALSE en caso contrario
   */
  bool isDebugEnabled() {
#ifdef FL_DEBUG
    return true;
#endif
    return false;
  }

  /**
   Indica si la aplicación se compiló en modo DEBUGGER

   @return TRUE en caso de que la aplicación se compilara en modo DEBUGGER, FALSE en caso contrario
   */
  bool isDebuggerEnabled() {
#ifdef QSDEBUGGER
    return true;
#endif
    return false;
  }

  /**
   Indica si la aplicación se ejecuta en modo DEBUGGER

   @return TRUE en caso de que la aplicación se ejecuta en modo DEBUGGER, FALSE en caso contrario
   */
  bool isDebuggerMode() {
#ifdef QSDEBUGGER
    return FLSettings::readBoolEntry("application/isDebuggerMode");
#endif
    return false;
  }

  /**
   Indica si la aplicación se compiló en modo TEST

   @return TRUE en caso de que la aplicación se compilara en modo TEST, FALSE en caso contrario
   */
  bool isTestEnabled() {
    return false;
  }

  /**
   Obtiene la versión de un módulo.

   @param idM Identificador del módulo del que se quiere saber su versión
   @param connName Nombre de la conexion
   @return Cadena con la versión
   */
  QString versionModule(const QString &idM, const QString &connName = "default") {
    return FLSqlConnections::database(connName)->managerModules()->versionModule(idM);
  }

  /**
  OBSOLETA no usar
  */
  QString nameObjectScript(const QString &nFS) {
    qWarning("nameObjectScript: OBSOLETA no usar");
    return QString::null;
  }

  /**
   Muestra una página html. Usado en la documentación

   @param url Dirección url de la página
   */
  void showDocPage(const QString &url) {
    obj_->showDocPage(url);
  }

  /**
   Ejecuta una función del script en el contexto de un objeto.

   @param function Nombre de la función.
   @param arguments Argumentos de la función.
   @param context Nombre del objeto contexto para el script.
   */
  QSArgument call(const QString &function, const QSArgumentList &arguments = QSArgumentList(),
                  const QString &nameObjectContext = QString::null) {
    return obj_->call(function, arguments, nameObjectContext);
  }

  /**
   Ejecuta una función del script en el contexto de un objeto, sin argumentos

   @param function Nombre de la función.
   @param context Nombre del objeto contexto para el script.
   */
  QSArgument call(const QString &function, const QString &nameObjectContext) {
    return obj_->call(function, QSArgumentList(), nameObjectContext);
  }

  /**
   Establece el título de la ventana principal.

   El título de la ventana principal siempre tendrá una parte fija establecida
   automáticamente por el motor y se le añadira el texto que se quiera establecer

   @text Texto a establecer en el título de la ventana principal
   */
  void setCaptionMainWidget(const QString &text) {
    obj_->setCaptionMainWidget(text);
  }

  /**
   Obtiene el último texto establecido con FLApplication::setCaptionMainWidget para el título de la ventana principal
   */
  QString lastTextCaption() const {
    return obj_->lastTextCaption();
  }

  /**
   Obtiene el nombre del driver con el que se conecta a la base de datos

   @param connName Nombre de la conexion
   @return Nombre del driver
   */
  QString nameDriver(const QString &connName = "default") const {
    return FLSqlConnections::database(connName)->driverName();
  }

  /**
   Obtiene el nombre del usuario con el que se conecta a la base de datos

   @param connName Nombre de la conexion
   @return Nombre del usuario
   */
  QString nameUser(const QString &connName = "default") const {
    return FLSqlConnections::database(connName)->user();
  }

  /**
   Obtiene el nombre de la base de datos

   @param connName Nombre de la conexion
   @return Nombre de la base de datos
   */
  QString nameBD(const QString &connName = "default") const {
    return FLSqlConnections::database(connName)->database();
  }

  /**
   Obtiene el nombre del host de la conexión

   @param connName Nombre de la conexion
   @return Nombre del host
   */
  QString nameHost(const QString &connName = "default") const {
    return FLSqlConnections::database(connName)->host();
  }

  /**
   Obtiene el puerto TCP de la conexión

   @param connName Nombre de la conexion
   @return Número de puerto
   */
  int portBD(const QString &connName = "default") const {
    return FLSqlConnections::database(connName)->port();
  }

  /**
   Crea un nuevo fichero "acl.xml" y lo almacena sustituyendo el anterior, en el caso de que exista.

   @param idacl Identificador del registro de la tabla "flacls" a utilizar para crear "acl.xml".
   */
  void installACL(const QString &idacl) {
    FLAccessControlLists *acl = obj_->acl();
    if (acl)
      acl->installACL(idacl);
  }

  /**
   Devuelve el identificador de la sesión actual

   @return Identificador de sesión
   */
  QString idSession() {
    return aqApp ->timeUser().toString(Qt::ISODate);
  }

  /**
   Imprime el contenido de un editor de texto ( QTextEdit ).

   @param editor Objeto QTextEdit del que imprimir su contenido
   */
  void printTextEdit(QTextEdit *editor) {
    obj_->printTextEdit(editor);
  }

  /**
   Reinicializa la aplicación.

   Vuelve a cargar las traducciones, los scripts y reinicializa
   la caja de herramientas.
   */
  void reinit() {
    QTimer::singleShot(0, obj_, SLOT(reinit()));
  }

  /**
   Limpieza la tabla flmetadata, actualiza el cotenido xml con el de los fichero .mtd
   actualmente cargados.

   @param connName Nombre de la conexion
   */
  void cleanupMetaData(const QString &connName = "default") {
    FLSqlConnections::database(connName)->manager()->cleanupMetaData();
  }

  /**
   Regenerar base de datos

   @param connName Nombre de la conexion
   */
  void Mr_Proper(const QString &connName = "default") {
    FLSqlConnections::database(connName)->Mr_Proper();
  }

  /**
   Añade una base de datos a las conexiones disponibles.

   La base de datos será abierta. Si ya existiera una conexión con el mismo nombre
   la base datos correspondiente será cerrada y borrada, sustituyéndola por la nueva.

   @param driverAlias Alias del driver ( PostgreSQL, MySQL, SQLite, ... ), ver FLSqlDatabase.
   @param nameDB  Nombre de la base de datos a la que conectar
   @param user  Usuario de la conexión
   @param password Contraseña para el usuario
   @param host  Nombre o dirección del servidor de la base de datos
   @param port  Puerto TCP de conexion
   @param connectionName Nombre de la nueva conexion
   @return TRUE si se pudo realizar la conexión, FALSE en caso contrario
   */
  bool addDatabase(const QString &driverAlias, const QString &nameDB, const QString &user,
                   const QString &password, const QString &host, int port, const QString &connectionName) {
    return FLSqlConnections::addDatabase(driverAlias, nameDB, user, password, host, port, connectionName);
  }

  /**
   Añade una base de datos a las conexiones disponibles utilizando los datos de otra conexión

   @param newConnName    Nombre a utilizar para la nueva conexion
   @param sourceConnName Nombre de una conexión existente a utilizar como origen de los datos de conexión
   @return TRUE si se pudo realizar la conexión, FALSE en caso contrario
   */
  bool addDatabase(const QString &newConnName, const QString &sourceConnName = "default") {
    FLSqlDatabase *srcDb = FLSqlConnections::database(sourceConnName);
    if (!srcDb)
      return false;
    return FLSqlConnections::addDatabase(srcDb->driverName(), srcDb->database(),
                                         srcDb->user(), srcDb->password(), srcDb->host(),
                                         srcDb->port(), newConnName);
  }

  /**
   Elimina una base de datos de las conexiones disponibles.

   Cierra la base de datos correspondiente y la elimina.

   @param connectionName Nombre de la conexion
   @return TRUE si se pudo eliminar la base de datos, FALSE en caso contrario
   */
  bool removeDatabase(const QString &connectionName) {
    return FLSqlConnections::removeDatabase(connectionName);
  }

  /**
   Convierte una cadena a Unicode

   @param codec Nombre de la codificación del texto
   @param text Texto a convertir
   */
  QString toUnicode(const QString &text, const QString &codecName) {
    QTextCodec *codec = QTextCodec::codecForName(codecName);
    if (codec) {
      return codec->toUnicode(text);
    } else {
      return text;
    }
  }

    /**
   Crea un fichero con la codificación especificada

   @param file Nombre del fichero a escribir
   @param encode Codificación. Puede ser ISO-8859-1 o UTF8
   @param contenido Datos a grabar en el fichero
   */
  void write(const QString &encode, const QString &file, const QString &text) {
      QFile f(file);
       if (f.open(IO_WriteOnly))
     {
     QTextStream dt(&f);
     
     if (encode == "ISO-8859-1")
         {
         dt.setCodec( QTextCodec::codecForName("ISO-8859-1") ); 
         dt << text.latin1();
         } 
    else
         {
         dt.setCodec( QTextCodec::codecForName("UTF-8") );
         dt << text.latin1();
         }
     f.close();
     }
  }
  /**
   Convierte una cadena en Unicode a la codificacion indicada

   @param codec Nombre de la codificación a utilizar
   @param text Texto a convertir
   */
  QCString fromUnicode(const QString &text, const QString &codecName) {
    QTextCodec *codec = QTextCodec::codecForName(codecName);
    if (codec)
      return codec->fromUnicode(text);
    else
      return text.utf8();
  }

  /** wrap a text without CR-LF chars on the 76 character boundary.*/
  QCString canonicalForm(const QString &str) {
    return MIMECodec::canonicalForm(str.ascii());
  }

  /** translate a MIME-encoded text.*/
  QString mimeTranslate(const QString &str) {
    return MIMECodec::translate(str.ascii());
  }

  /** MIME-fold text (used for headers).*/
  QCString fold(const QString &str) {
    return MIMECodec::fold(str.ascii());
  }

  /** unfold MIME-folded text.*/
  QCString unfold(const QString &str) {
    return MIMECodec::unfold(str.ascii());
  }

  /** encode UNIX text (cr-ended) into MIME text (crlf-ended).*/
  QCString encodeEndLine(const QString &str) {
    return MIMECodec::encodeEndLine(str.ascii());
  }

  /** decode MIME text (crlf-ended) into UNIX text (cr-ended).*/
  QCString decodeEndLine(const QString &str) {
    return MIMECodec::decodeEndLine(str.ascii());
  }

  /** encode text in quoted-printable format (the bool parameter forces extra MIME gateway compatibility).*/
  QCString encodeQuotedPrintable(const QString &str, bool compat = false) {
    return MIMECodec::encodeQuotedPrintable(str.ascii(), compat);
  }

  /** decode quoted-printable encoded text.*/
  QCString decodeQuotedPrintable(const QString &str) {
    return MIMECodec::decodeQuotedPrintable(str.ascii());
  }

  /**
   Abre un cuadro de dialogo para seleccionar un fichero de imagen.

   El cuadro de diálogo incluye un marco de previsualización

   @return Ruta y nombre del fichero seleccionado, o vacío si se canceló
   */
  QString dialogGetFileImage();

  /**
   Convierte si es posible el valor QVariant a un QPixmap
   */
  QPixmap toPixmap(const QVariant &v);
  QPixmap toPixmap(const QString &s) {
    return toPixmap(QVariant(s));
  }

  /**
   Convierte si es posible el valor QPixmap a un QVariant
   */
  QVariant fromPixmap(const QPixmap &p);

  /**
   Escalado suavizado de un Pixmap (ver QImage::smoothScale)
   */
  QPixmap scalePixmap(const QPixmap &p, int w, int h,
                      FLScaleMode mode = FLApplicationInterface::ScaleFree);

  /**
  Ver QPixmap::fromMimeSource()
  */
  QPixmap pixmapFromMimeSource(const QString &abs_name) {
    return QPixmap::fromMimeSource(abs_name);
  }

  /**
   @return El nivel actual de anidamiento de transacciones, 0 no hay transaccion
   */
  int transactionLevel() {
    return FLSqlCursor::transactionLevel();
  }

  /**
   Añade código script al objeto sys, para poder ejecutarlo dinámicamente.

   Si el código ya existe no lo añade
   @param code Código script
   @param scriptEntryFunction Funcion del código del script a ejecutar despues de evaluarlo
   @return TRUE si se añadió el código, FALSE si el código ya existe.
   */
  bool addSysCode(const QString &code, const QString &scriptEntryFunction = QString::null) {
    return obj_->addSysCode(code, scriptEntryFunction);
  }

  /**
   Establece el nombre de la funcion de entrada tras evaluar el código
   */
  void setScriptEntryFunction(const QString &scriptEntryFunction) {
    obj_->setScriptEntryFunction(scriptEntryFunction);
  }

  /**
   Para activar/desactivar la detección de posibles bloqueos de una conexión a base
   de datos sobre el resto.

   Al activar la detección se inicia un cronómetro que periódicamente comprueba bloqueos,
   si existe un bloqueo se emite la señal databaseLockDetected(), y se muestra un mensaje en el
   caso que se indique hacerlo. Al desactivarla se para este cronómetro y por tanto las
   comprobaciones periódicas.

   La detección de bloqueos solo funciona en las bases de datos que lo soportan,
   ver FLSqlDatabase::canDetectLocks().

   @param  on              True activado False desactivado
   @param  msecLapsus      Lapso de tiempo que debe transcurrir entre cada comprobación, por defecto 30 seg.
   @param  limChecks       Numero límite de comprobaciones a realizar, al llegar a este límite el cronómetro
   para. Si es -1 no hay limite.
   @param  showWarn        True para mostrar un cuadro de diálogo con el aviso de bloqueo
   @param  msgWarn         Texto a mostrar en el diálogo de aviso, si es vacío mostrará uno estándar
   @param  connectionName  Nombre de la conexion a la base de datos
   */
  void setDatabaseLockDetection(bool on = true, int msecLapsus = 30000, int limChecks = -1,
                                bool showWarn = true, const QString &msgWarn = QString::null,
                                const QString &connectionName = "default") {
    obj_->setDatabaseLockDetection(on, msecLapsus, limChecks, showWarn, msgWarn, connectionName);
  }

  /**
   Para obtener información sobre el estado de los bloqueos existentes en la base de datos.

   Si hay bloqueos devuelve una lista de cadenas de texto en forma de registros de información. En esta lista
   la primera cadena de texto contiene los nombres de los campos de información incluidos y separados con "@",
   las siguientes cadenas son una por cada bloqueo con la información correspondiente.
   Si hay registros bloqueados produciendo situaciones de espera, se incluye información de los mismos cuando
   la cadena empieza por "##", indicando el nombre del campo clave primaria y el valor para el registro bloqueado.

   Ejemplo:

   "relation@locktype@pid"
   "stocks@RowExclusiveLock@8229"
   "##idstock=203"
   "secuencias@ExclusiveLock@8393"

   @param  connectionName  Nombre de la conexion a la base de datos
   @return Lista con información de los bloqueos, si es vacia no hay bloqueos.
   */
  QStringList locksStatus(const QString &connectionName = "default") {
    return FLSqlConnections::database(connectionName)->locksStatus();
  }

  /**
   Comprueba si las transacciones de la base de datos actual están bloqueando a otras conexiones.

   Si hay bloqueos devuelve una lista de los mismos con el formato descrito en FLSqlDatabase::locksStatus()

   @param  connectionName  Nombre de la conexion a la base de datos
   @return Lista con información de los bloqueos, si es vacia no hay bloqueos.
   */
  QStringList detectLocks(const QString &connectionName = "default") {
    return FLSqlConnections::database(connectionName)->detectLocks();
  }

  /**
   Comprueba si hay riesgo de caer en un bloqueo en espera con otras conexiones.

   Si hay riesgo devuelve una lista de los bloqueos candidatos con el mismo formato descrito en FLSqlDatabase::locksStatus()

   @param  table  El nombre de una tabla para solo comprobar los riesgos sobre ella, o vacio
                  para comprobarlos en todas las tablas.
   @param  primaryKeyValue El valor de la clave primaria de un registro para solo comprobar los riesgos sobre el,
                           o vacio para comprobarlos en todos. ( No funciona con claves compuestas ).
   @param  connectionName  Nombre de la conexion a la base de datos
   @return Lista con información de los bloqueos, si es vacia no hay bloqueos.
   */
  QStringList detectRisksLocks(const QString &table = QString::null, const QString &primaryKeyValue = QString::null,
                               const QString &connectionName = "default") {
    return FLSqlConnections::database(connectionName)->detectRisksLocks(table, primaryKeyValue);
  }

  /**
  Muestra un mensaje emergente con un aviso.

  Ejemplo:
  \code
    var msgWarn = "<p><img source=\"locked.png\" align=\"right\"><b><u>Prueba</u></b><br><br>" +
                  "Esto es una prueba de mensaje emergente, al pulsar el enlace se " +
                  "llamará a la funcion flfactppal.pub_msgNoDisponible('Informes')." +
                  "<br><br><em><a href=\"flfactppal.pub_msgNoDisponible\">Invocar funcion</a></em></p>";
    var scriptCalls = [];
    scriptCalls["flfactppal.pub_msgNoDisponible"] = "Informes";
    sys.popupWarn(msgWarn, scriptCalls);
  \endcode

  @param  msgWarn     Texto a mostrar en el aviso
  @param  scriptCalls Diccionario de funciones de scripts y sus argumentos correspondientes a los
                      hiperenlaces incluidos en el mensaje
  */
  void popupWarn(const QString &msgWarn,
                 const QMap<QString, QVariant> &scriptCalls);

  /**
  @return El contenido de la consulta indicada como un documento XML con el formato de datos que acepta Kugar
  */
  FLDomNodeInterface *toXmlReportData(FLSqlQueryInterface *q);

  /**
  Traducción de una cadena al idioma local

  Se hace una llamada a la función tr() de la clase QObject para hacer la traducción.
  Se utiliza para traducciones desde fuera de objetos QObject

  @param contexto Contexto en el que se encuentra la cadena, generalmente se refiere a la clase en la que está definida
  @param s        Cadena de texto a traducir
  @return         Devuelve la cadena traducida al idioma local
  */
  QString translate(const QString &contexto, const QString &s) const {
    return obj_->translate(contexto, s);
  }
  QString translate(const QString &s) const {
    return obj_->translate("scripts", s);
  }

  /**
  Traducción multiidioma de una cadena texto

  @param s  Cadena de texto a traducir mediante el traductor "multilang"
  @param l  Identificador del idioma (ES, EN, IT, CA, etc..) al que traducir
  @return   La cadena de texto traducida al idioma indicado
  */
  QString trMulti(const QString &s, const QString &l) {
    return obj_->trMulti(s, l);
  }

  /**
  Activa/Desactiva la traducción dinámica multiidioma

  @param enable   Si es TRUE activa a partir de ese momento la traducción dinámica multiidioma.
                  Importante: Todos los mensajes serán traducidos mediante el traductor "multilang" y
                  al lenguaje activado, ignorando las traducciones existentes locales.
                  Si es FALSE desactiva la traducción dinámica y los mensajes volverán a traducirse
                  utilizando el traductor para el idioma local.
  @param langid   Identificador del idioma a utilizar para la traducción multiidioma. Si 'enable' es FALSE
                  o es vacio este parámetro será ignorado, y se seguirá utilizando el último idioma establecido.
  */
  void setMultiLang(bool enable = true, const QString &langid = QString::null) {
    obj_->setMultiLang(enable, langid);
  }

  /**
  @return Si la traducción dinámica multiidioma está activada
  */
  bool multiLangEnabled() const {
    return obj_->multiLangEnabled();
  }

  /**
  @return El identificador del idioma actualmente establedico para la traducción dinámica multiidioma
  */
  QString multiLangId() const {
    return obj_->multiLangId();
  }

  /**
  Abre el entorno integrado de desarrollo de scripts QSA Workbench
  */
  void openQSWorkbench() {
    obj_->openQSWorkbench();
  }

  /**
  Evalua el código de todos los scripts del proyecto QSA

  Si se ha establecido una funcion de entrada con setScriptEntryFunction(),
  justo antes de llamar a este método, esta es invocada tras la evaluacion.
  */
  void evaluateProject() {
    obj_->evaluateProject();
  }

  QString osName() const {
    return FLUtil::getOS();
  }

  QString localLang() const {
    return FLUtil::getIdioma();
  }

  QString localEncoding() const {
    QTextCodec *codec = QTextCodec::codecForLocale();
    return (codec ? codec->mimeName() : "");
  }

  bool isQuickBuild() const {
#ifdef FL_QUICK_CLIENT
    return true;
#endif
    return false;
  }

  bool isDebuggerBuild() const {
#ifdef AQ_DEBUGGER
    return true;
#endif
    return false;
  }

  bool isNebulaBuild() const {
#ifdef AQ_NEBULA_BUILD
    return true;
#endif
    return false;
  }

  FLApplication *obj() {
    return obj_;
  }

signals:

  /**
   Señal emitida cuando se ha detectado un bloqueo en la conexion a la base de datos
   */
  void databaseLockDetected();

private:

  FLApplication *obj_;
};

class FLSqlCursorsCache;

//! Acceso a un cursor sobre la base de datos
/**
 Es la principal clase de acceso a la base de datos.
 Permite leer o establecer valores de las tablas, filtrar, editar o eliminar registros, etc.

 @author InfoSiAL S.L.
 */
class  FLSqlCursorInterface : public QObject
{
  Q_OBJECT
  Q_ENUMS(Mode)
  Q_ENUMS(AcosConditionEval)

public:

  enum Mode {
    Insert = 0,
    Edit = 1,
    Del = 2,
    Browse = 3
  };

  enum AcosConditionEval {
    Value = 0,
    Regexp = 1,
    Function = 2
  };

  /**
   Constructor
   */
  FLSqlCursorInterface(const QString &n = QString::null,
                       const QString &connName = "default") :
    QObject(0), obj_(0), cursorRelation_(0), cached_(false) {
    if (!n.isEmpty()) {
      obj_ = new FLSqlCursor(n, true, connName, 0, 0, this);
      if (obj_->cursorRelation())
        cursorRelation_ = sqlCursorInterface(obj_->cursorRelation());
      connects();
    }
  }

  /**
   Constructor
   */
  FLSqlCursorInterface(QObject *obj) :
    QObject(obj), obj_(0), cursorRelation_(0), cached_(false) {
    setObj(::qt_cast<FLSqlCursor *>(obj));
  }

  /**
   Destructor
   */
  ~FLSqlCursorInterface() {
    if (cached_ && obj_)
      removeCache();
  }

  /**
   Establece el correspondiente objeto FLSqlCursor

   @param obj Objeto FLSqlCursor
   */
  void setObj(FLSqlCursor *obj);

  /**
  Uso interno
  */
  static FLSqlCursorInterface *sqlCursorInterface(FLSqlCursor *c);

public slots:

  /**
   Obtiene el modo de acceso actual del cursor.

   @return Constante FLSqlCursorInterface::Mode que define el modo de acceso actual del cursor
   */
  int modeAccess() const {
    return (obj_ ? obj_->modeAccess() : 0);
  }

  /**
   Pasa el cursor a modo Edit

   @return True si el cursor está en modo Edit o estaba en modo Insert y ha pasado con éxito a modo Edit
   */
  bool setEditMode() {
    return (obj_ ? obj_->setEditMode() : false);
  }

  /**
   Obtiene el filtro principal del cursor.

   @return Cadena de texto con el filtro principal
   */
  QString mainFilter() const {
    return (obj_ ? obj_->mainFilter() : QString::null);
  }

  /**
   Establece el filtro principal del cursor.

   @param f Cadena con el filtro, corresponde con una clausura WHERE
   */
  void setMainFilter(const QString &f) {
    if (obj_)
      obj_->setMainFilter(QString(f).replace("NaN", ""));
  }

  /**
   Establece el modo de acceso para el cursor.

   @param m Constante FLSqlCursorInterface::Mode que indica en que modo de acceso se quiere establecer el cursor
   */
  void setModeAccess(const int m) {
    if (obj_)
      obj_->setModeAccess(m);
  }

  /**
   Establece el valor de un campo del buffer de forma atómica y fuera de transacción.

   Invoca a la función, cuyo nombre se pasa como parámetro, del script del contexto del cursor
   (ver FLSqlCursor::ctxt_) para obtener el valor del campo. El valor es establecido en el campo de forma
   atómica, bloqueando la fila durante la actualización. Esta actualización se hace fuera de la transacción
   actual, dentro de una transacción propia, lo que implica que el nuevo valor del campo está inmediatamente
   disponible para las siguientes transacciones.

   @param fN Nombre del campo
   @param functionName Nombre de la función a invocar del script
   */
  void setAtomicValueBuffer(const QString &fN, const QString &functionName) {
    if (obj_)
      obj_->setAtomicValueBuffer(fN, functionName);
  }

  /**
   Establece el valor de un campo del buffer con un valor.

   @param fN Nombre del campo
   @param v Valor a establecer para el campo
   */
  void setValueBuffer(const QString &fN, const QVariant &v) {
    if (obj_) {
      if (v.isValid() && !v.isNull()) {
        if (v.toString().upper() == "NAN") {
          obj_->setValueBuffer(fN, QVariant());
          return;
        }
      }
      obj_->setValueBuffer(fN, v);
    }
  }
  void setValueBuffer(const QString &fN, const QString &v) {
    setValueBuffer(fN, QVariant(v));
  }
  void setValueBuffer(const QString &fN, bool v) {
    setValueBuffer(fN, QVariant(v));
  }
  void setValueBuffer(const QString &fN, double v) {
    setValueBuffer(fN, QVariant(v));
  }
  void setValueBuffer(const QString &fN, int v) {
    setValueBuffer(fN, QVariant(v));
  }

  /**
   Devuelve el valor de un campo del buffer.

   @param fN Nombre del campo
   */
  QVariant valueBuffer(const QString &fN) const {
    return (obj_ ? obj_->valueBuffer(fN) : QVariant());
  }

  /**
   Devuelve el valor de un campo del buffer copiado antes de sufrir cambios.

   @param fN Nombre del campo
   */
  QVariant valueBufferCopy(const QString &fN) const {
    return (obj_ ? obj_->valueBufferCopy(fN) : QVariant());
  }

  /**
   Devuelve si el contenido de un campo en el buffer en nulo.

   @param name Nombre del campo en el buffer
   */
  bool isNull(const QString &name) const {
    return (obj_ ? obj_->bufferIsNull(name) : false);
  }

  /**
   Establece que el contenido de un campo en el buffer sea nulo.

   @param name Nombre del campo en el buffer
   */
  void setNull(const QString &name) {
    if (obj_)
      obj_->bufferSetNull(name);
  }

  /**
   Devuelve si el contenido de un campo en el bufferCopy en nulo.

   @param name Nombre del campo en el bufferCopy
   */
  bool isCopyNull(const QString &name) const {
    return (obj_ ? obj_->bufferCopyIsNull(name) : false);
  }

  /**
   Establece que el contenido de un campo en el bufferCopy sea nulo.

   @param name Nombre del campo en el bufferCopy
   */
  void setCopyNull(const QString &name) {
    if (obj_)
      obj_->bufferCopySetNull(name);
  }

  /**
   Establece el valor de FLSqlCursor::edition.

   @param b TRUE o FALSE
   */
  void setEdition(bool b) {
    if (obj_)
      obj_->setEdition(b);
  }

  /**
   Establece el valor de FLSqlCursor::browse.

   @param b TRUE o FALSE
   */
  void setBrowse(bool b) {
    if (obj_)
      obj_->setBrowse(b);
  }

  /**
   Dice si un campo está deshabilitado.

   Un campo estará deshabilitado, porque está clase le dará un valor automáticamente.
   Estos campos son lo campos que están en una relación con otro cursor, por lo que
   su valor lo toman del campo foráneo con el que se relacionan. El objeto FLReceiver pasa
   a ser propiedad del objeto FLSqlCursor, por lo tanto al destruir el objeto FLSqlCursor se
   borrará automáticamente el objeto FLReceiver.

   @param fN Nombre del campo a comprobar
   @return TRUE si está deshabilitado y FALSE en caso contrario
   */
  bool fieldDisabled(const QString &fN) {
    return (obj_ ? obj_->fieldDisabled(fN) : false);
  }

  /**
   Indica si hay una transacción en curso.

   @return TRUE si hay una transaccion en curso, FALSE en caso contrario
   */
  bool inTransaction() {
    return (obj_ ? obj_->inTransaction() : false);
  }

  /**
   Inicia una transacción.

   Si ya hay una transacción en curso no hace nada. Si lock es igual a TRUE inicia
   un bloqueo de la tabla, el bloqueo termina cuando se termina la transacción, con
   commit() o rollback()

   @return TRUE si la operación tuvo exito
   */
  bool transaction(bool lock) {
    return (obj_ ? obj_->transaction(lock) : false);
  }

  /**
   Deshace las operaciones de una transacción y la acaba.

   @return TRUE si la operación tuvo exito
   */
  bool rollback() {
    return (obj_ ? obj_->rollback() : false);
  }

  /**
   Hace efectiva la transacción y la acaba.

   @param notify Si TRUE emite la señal cursorUpdated y pone el cursor en modo BROWSE,
   si FALSE no hace ninguna de estas dos cosas y emite la señal de autoCommit
   @return TRUE si la operación tuvo exito
   */
  bool commit() {
    return (obj_ ? obj_->commit() : false);
  }

  /**
   Establece el valor de FLSqlCursor::askForCancelChanges_ .

   @param a Valor a establecer (TRUE o FALSE)
   */
  void setAskForCancelChanges(bool a) {
    if (obj_)
      obj_->setAskForCancelChanges(a);
  }

  /**
   Activa o desactiva los chequeos de integridad referencial.

   @param a TRUE los activa y FALSE los desactiva
   */
  void setActivatedCheckIntegrity(bool a) {
    if (obj_)
      obj_->setActivatedCheckIntegrity(a);
  }

  /**
   Activa o desactiva las acciones a realizar antes y después de un commit

   @param a TRUE las activa y FALSE las desactiva
   */
  void setActivatedCommitActions(bool a) {
    if (obj_)
      obj_->setActivatedCommitActions(a);
  }

  /**
   Se comprueba la integridad referencial al intentar borrar, tambien se comprueba la no duplicidad de
   claves primarias y si hay nulos en campos que no lo permiten cuando se inserta o se edita.
   Si alguna comprobacion falla devuelve un mensaje describiendo el fallo.
   */
  QString msgCheckIntegrity() {
    return (obj_ ? obj_->msgCheckIntegrity() : QString::null);
  }

  /**
   Realiza comprobaciones de intregidad.

   Se comprueba la integridad referencial al intentar borrar, tambien se comprueba la no duplicidad de
   claves primarias y si hay nulos en campos que no lo permiten cuando se inserta o se edita.
   Si alguna comprobacion falla muestra un cuadro de diálogo con el tipo de fallo encontrado y el metodo
   devuelve FALSE.

   @param showError Si es TRUE muestra el cuadro de dialogo con el error que se produce al no
   pasar las comprobaciones de integridad
   @return TRUE si se ha podido entregar el buffer al cursor, y FALSE si ha fallado alguna comprobacion
   de integridad
   */
  bool checkIntegrity(bool showError = true) {
    return (obj_ ? obj_->checkIntegrity(showError) : false);
  }

  /**
   Refresca el contenido del cursor.

   Si no se ha indicado cursor relacionado obtiene el cursor completo, segun la consulta
   por defecto. Si se ha indicado que depende de otro cursor con el que se relaciona,
   el contenido del cursor dependerá del valor del campo que determina la relación.
   Si se indica el nombre de un campo se considera que el buffer sólo ha cambiado en ese
   campo y así evitar repeticiones en el refresco.

   @param fN Nombre del campo de buffer que ha cambiado
   */
  void refresh(const QString &fN = QString::null) {
    if (obj_)
      obj_->refresh(fN);
  }

  /**
   Refresca el buffer según el modo de acceso establecido.

   Lleva informacion del cursor al buffer para editar o navegar, o prepara el buffer para
   insertar o borrar.

   Si existe un campo contador se invoca a la función "calculateCounter" del script del
   contexto (ver FLSqlCursor::ctxt_) establecido para el cursor. A esta función se le pasa
   como argumento el nombre del campo contador y debe devolver el valor que debe contener
   ese campo.
   */
  bool refreshBuffer() {
    return (obj_ ? obj_->refreshBuffer() : false);
  }

  /**
   Devuelve la posición del registro actual del cursor.

   Ver QSqlCursor::at()
   */
  int at() {
    return (obj_ ? obj_->at() : -1);
  }

  /**
   Redefinición del método seek() de QSqlCursor.

   Este método simplemente invoca al método seek() original de QSqlCursor() y refresca
   el buffer con el metodo FLSqlCursor::refreshBuffer().

   @param emit Si TRUE emite la señal FLSqlCursor::currentChanged()
   */
  bool seek(int i, bool relative = false, bool emite = false) {
    return (obj_ ? obj_->seek(i, relative, emite) : false);
  }

  /**
   Redefinición del métedo next() de QSqlCursor.

   Este método simplemente invoca al método next() original de QSqlCursor() y refresca el
   buffer con el metodo FLSqlCursor::refreshBuffer().

   @param emit Si TRUE emite la señal FLSqlCursor::currentChanged()
   */
  bool next(bool emite = true) {
    return (obj_ ? obj_->next(emite) : false);
  }

  /**
   Redefinición del método prev() de QSqlCursor.

   Este método simplemente invoca al método prev() original de QSqlCursor() y refresca
   el buffer con el metodo FLSqlCursor::refreshBuffer().

   @param emit Si TRUE emite la señal FLSqlCursor::currentChanged()
   */
  bool prev(bool emite = true) {
    return (obj_ ? obj_->prev(emite) : false);
  }

  /**
   Redefinición del método first() de QSqlCursor.

   Este método simplemente invoca al método first() original de QSqlCursor() y refresca el
   buffer con el metodo FLSqlCursor::refreshBuffer().

   @param emit Si TRUE emite la señal FLSqlCursor::currentChanged()
   */
  bool first(bool emite = true) {
    return (obj_ ? obj_->first(emite) : false);
  }

  /**
   Redefinición del método last() de QSqlCursor.

   Este método simplemente invoca al método last() original de QSqlCursor() y refresca el
   buffer con el metodo FLSqlCursor::refreshBuffer().

   @param emit Si TRUE emite la señal FLSqlCursor::currentChanged()
   */
  bool last(bool emite = true) {
    return (obj_ ? obj_->last(emite) : false);
  }

  /**
   Redefinición del método del() de QSqlCursor.

   Este método invoca al método del() original de QSqlCursor() y comprueba si hay borrado
   en cascada, en caso afirmativo borrar también los registros relacionados en cardinalidad 1M.
   */
  int del(bool invalidate = true) {
    return (obj_ ? obj_->del(invalidate) : false);
  }

  /**
   Redefinición del método select() de QSqlCursor
   */
  bool select(const QString &filter, const QSqlIndex &sort = QSqlIndex()) {
    return (obj_ ? obj_->select(QString(filter).replace("NaN", ""), sort) : false);
  }

  /**
   Redefinicion del método select() de QSqlCursor
   */
  bool select() {
    if (!obj_)
      return false;
    obj_->QSqlCursor::setFilter(obj_->curFilter());
    return obj_->QSqlCursor::select();
  }

  /**
   Devuelve el número de filas seleccionadas en el cursor

   @return Número de filas
   */
  int size() {
    return (obj_ ? obj_->size() : 0);
  }

  /**
   Obtiene la posición del registro actual, según la clave primaria contenida en el buffer.

   La posición del registro actual dentro del cursor se calcula teniendo en cuenta el
   filtro actual ( FLSqlCursor::curFilter() ) y el campo o campos de ordenamiento
   del mismo ( QSqlCursor::sort() ).
   Este método es útil, por ejemplo, para saber en que posición dentro del cursor
   se ha insertado un registro.

   @return Posición del registro dentro del cursor, o 0 si no encuentra coincidencia.
   */
  int atFrom() {
    return (obj_ ? obj_->atFrom() : -1);
  }

  /**
   Obtiene la posición dentro del cursor del primer registro que en el campo indicado
   empieze con el valor solicitado. Supone que los registros están ordenados por dicho
   campo, para realizar una búsqueda binaria.

   La posición del registro actual dentro del cursor se calcula teniendo en cuenta el
   filtro actual ( FLSqlCursor::curFilter() ) y el campo o campos de ordenamiento
   del mismo ( QSqlCursor::sort() ).
   Este método es útil, por ejemplo, para saber en que posición dentro del cursor
   se encuentra un registro con un cierto valor en un campo.

   @param  fN  Nombre del campo en el que buscar el valor
   @param  v   Valor a buscar ( mediante like 'v%' )
   @param  orderAsc TRUE (por defecto) si el orden es ascendente, FALSE si es descendente
   @return Posición del registro dentro del cursor, o 0 si no encuentra coincidencia.
   */
  int atFromBinarySearch(const QString &fN, const QString &v, bool orderAsc = true) {
    return (obj_ ? obj_->atFromBinarySearch(fN, v, orderAsc) : -1);
  }

  /**
   Manda el contenido del buffer al cursor, o realiza la acción oportuna para el cursor.

   Todos los cambios realizados en el buffer se hacen efectivos en el cursor al invocar este método.
   La manera de efectuar estos cambios viene determinada por el modo de acceso establecido para
   el cursor, ver FLSqlCursor::Mode, si el modo es editar o insertar actualiza con los nuevos valores de
   los campos del registro, si el modo es borrar borra el registro, y si el modo es navegacion no hace nada.
   Antes de nada tambien comprueba la integridad referencial invocando al método FLSqlCursor::checkIntegrity().

   Si existe un campo calculado se invoca a la función "calculateField" del script del
   contexto (ver FLSqlCursor::ctxt_) establecido para el cursor. A esta función se le pasa
   como argumento el nombre del campo calculado y debe devolver el valor que debe contener
   ese campo, p.e. si el campo es el total de una factura y de tipo calculado la función
   "calculateField" debe devolver la suma de lineas de las facturas mas/menos impuestos y
   descuentos.

   @param  checkLocks  True para comprobar riesgos de bloqueos para esta tabla y el registro actual
   @return TRUE si se ha podido entregar el buffer al cursor, y FALSE si ha fallado la entrega
   */
  bool commitBuffer(bool checkLocks = false) {
    return (obj_ ? obj_->commitBuffer(true, checkLocks) : false);
  }

  /**
   Manda el contenido del buffer del cursor relacionado a dicho cursor.

   Hace efectivos todos los cambios en el buffer del cursor relacionado posiconándose en el registro
   correspondiente que recibe los cambios.
   */
  bool commitBufferCursorRelation() {
    return (obj_ ? obj_->commitBufferCursorRelation() : false);
  }

  /**
   Devuelve el cursor relacionado con este.
   */
  FLSqlCursorInterface *cursorRelation() {
    return cursorRelation_;
  }

  /**
   Establece el contexto de ejecución de scripts

   Ver FLSqlCursor::ctxt_.

   @param c Contexto de ejecucion
   */
  void setContext(QObject *c) {
    if (obj_)
      obj_->setContext(c);
  }

  /**
   Para obtener el contexto de ejecución de scripts.

   Ver FLSqlCursor::ctxt_.

   @return Contexto de ejecución
   */
  QObject *context() const {
    return (obj_ ? obj_->context() : 0);
  }

  /**
   Para obtener la acción asociada al cursor.

   @return  Objeto FLAction
   */
  QString action() {
    if (!obj_)
      return QString::null;
    if (!obj_->action())
      return QString::null;
    return obj_->action() ->name();
  }

  /**
   Establece la acción asociada al cursor.

   @param a Objeto FLAction
   */
  void setAction(const QString &action) {
    if (obj_) {
      const FLAction *a = obj_->db()->manager()->action(action);
      if (a)
        obj_->setAction(a);
    }
  }

  /**
   Establece los campos unlock de un registro, saltándose las comprobación

   @param fN Nombre del campo
   @param v Valor para el campo unlock
   */
  void setUnLock(const QString &fN, bool v) {
    if (obj_)
      obj_->setUnLock(fN, v);
  }

  /**
   Para comprobar si el registro actual del cursor está bloqueado.

   @return TRUE si está bloqueado, FALSE en caso contrario.
   */
  bool isLocked() {
    return (obj_ ? obj_->isLocked() : false);
  }

  /**
   Abre el formulario de edicion de registro definido en los metadatos (FLTableMetaData) listo
   para editar el registro activo del cursor.
   */
  void editRecord() {
    if (obj_) {
      if (obj_->parent() == this) {
        removeChild(obj_);
        if (qApp->focusWidget())
          qApp->focusWidget()->insertChild(obj_);
      }
      obj_->editRecord();
    }
  }

  /**
   Abre el formulario de inserción de registro definido en los metadatos (FLTableMetaData) listo
   para insertar el registro activo del cursor.
   */
  void insertRecord() {
    if (obj_) {
      if (obj_->parent() == this) {
        removeChild(obj_);
        if (qApp->focusWidget())
          qApp->focusWidget()->insertChild(obj_);
      }
      obj_->insertRecord();
    }
  }

  /**
   Abre el formulario de edicion de registro definido en los metadatos (FLTableMetaData) listo
   para visualizar el registro activo del cursor.
   */
  void browseRecord() {
    if (obj_) {
      if (obj_->parent() == this) {
        removeChild(obj_);
        if (qApp->focusWidget())
          qApp->focusWidget()->insertChild(obj_);
      }
      obj_->browseRecord();
    }
  }

  /**
   Realiza la acción asociada a elegir un registro del cursor, por defecto se abre el formulario de
   edición de registro,llamando al método FLSqlCursor::editRecord(), si la bandera FLSqlCursor::edition
   indica TRUE, si indica FALSE este método no hace nada
   */
  void chooseRecord() {
    if (obj_)
      obj_->chooseRecord();
  }

  /**
   Devuelve el nombre de la tabla asociada al cursor

   @return Cadena que contiene la tabla asociada al cursor
   */
  QString table() const {
    return (obj_ ? (obj_->metadata() ? obj_->metadata() ->name() : QString::null) : QString::null);
  }

  /**
   Devuelve el tipo de un campo de la tabla
   100 - serial
   200 - unlock

   @return Entero con el código del tipo de dato del campo
   */
  const int fieldType(const QString &fN) const {
    return (obj_ ? (obj_->metadata() ? obj_->metadata() ->fieldType(fN) : 0) : 0);
  }

  /**
   Devuelve la clave primaria de la tabla asociada al cursor

   @return Cadena que contiene el nombre del campo que es clave primaria de la tabla
   */
  QString primaryKey() const {
    return (obj_ ? (obj_->metadata() ? obj_->metadata() ->primaryKey() : QString::null) : QString::null);
  }

  /**
   QSqlCursor::isValid()
   */
  bool isValid() const {
    return (obj_ ? obj_->isValid() : false);
  }

  /**
   Indica si el contenido actual del buffer difiere de la copia guardada.

   Ver FLSqlCursor::bufferCopy_ .

   @return TRUE si el buffer y la copia son distintas, FALSE en caso contrario
   */
  bool isModifiedBuffer() {
    return (obj_ ? obj_->isModifiedBuffer() : false);
  }

  /**
   @return El nivel actual de anidamiento de transacciones, 0 no hay transaccion
   */
  int transactionLevel() {
    return FLSqlCursor::transactionLevel();
  }

  /**
   @return La lista con los niveles de las transacciones que ha iniciado este cursor y continuan abiertas
   */
  QStringList transactionsOpened() {
    return (obj_ ? obj_->transactionsOpened() : QStringList());
  }

  /**
   Deshace transacciones abiertas por este cursor.

   @param count  Cantidad de transacciones a deshacer, -1 todas.
   @param msg    Cadena de texto que se muestra en un cuadro de diálogo antes de deshacer las transacciones.
   Si es vacía no muestra nada.
   */
  void rollbackOpened(int count = -1, const QString &msg = QString::null) {
    if (obj_)
      obj_->rollbackOpened(count, msg);
  }

  /**
   Termina transacciones abiertas por este cursor.

   @param count  Cantidad de transacciones a terminar, -1 todas.
   @param msg    Cadena de texto que se muestra en un cuadro de diálogo antes de terminar las transacciones.
   Si es vacía no muestra nada.
   */
  void commitOpened(int count = -1, const QString &msg = QString::null) {
    if (obj_)
      obj_->commitOpened(count, msg);
  }

  /**
   Entra en un bucle de comprobacion de riesgos de bloqueos para esta tabla y el registro actual

   El bucle continua mientras existan bloqueos, hasta que se vuelva a llamar a este método con
   'terminate' activado o cuando el usuario cancele la operación.

   @param  terminate True terminará el bucle de comprobaciones si está activo
   */
  void checkRisksLocks(bool terminate = false) {
    if (obj_)
      obj_->checkRisksLocks(terminate);
  }

  /**
   Establece el acceso global para la tabla, ver FLSqlCursor::setAcosCondition().

   Este será el permiso a aplicar a todos los campos por defecto

   @param  ac Permiso global; p.e.: "r-", "-w"
   */
  void setAcTable(const QString &ac) {
    if (obj_)
      obj_->setAcTable(ac);
  }

  /**
   Establece la lista de control de acceso (ACOs) para los campos de la tabla, , ver FLSqlCursor::setAcosCondition().

   Esta lista de textos deberá tener en sus componentes de orden par los nombres de los campos,
   y en los componentes de orden impar el permiso a aplicar a ese campo,
   p.e.: "nombre", "r-", "descripcion", "--", "telefono", "rw",...

   Los permisos definidos aqui sobreescriben al global.

   @param acos Lista de cadenas de texto con los nombre de campos y permisos.
   */
  void setAcosTable(const QStringList &acos) {
    if (obj_)
      obj_->setAcosTable(acos);
  }

  /**
   Establece la condicion que se debe cumplir para aplicar el control de acceso.

   Para cada registro se evalua esta condicion y si se cumple, aplica la regla
   de control de acceso establecida con FLSqlCursor::setAcTable y FLSqlCursor::setAcosTable.

   Ejemplos:

   setAcosCondition( "nombre", VALUE, "pepe" ); // valueBuffer( "nombre" ) == "pepe"
   setAcosCondition( "nombre", REGEXP, "pe*" ); // QRegExp( "pe*" ).exactMatch( valueBuffer( "nombre" ).toString() )
   setAcosCondition( "sys.checkAcos", FUNCTION, true ); // call( "sys.checkAcos" ) == true

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
  void setAcosCondition(const QString &condName, int cond, const QVariant &condVal) {
    if (obj_)
      obj_->setAcosCondition(condName, (FLSqlCursor::AcosConditionEval) cond, condVal);
  }

  /**
   Limpia la condicion utilizada para aplicar el control de acceso.

   Tras llamar a este método no se evaluará la condición y por lo tanto no se aplicará
   ninguna regla de acceso. Se debe utilizar FLSqlCursor::setAcosCondition() para habilitar de nuevo
   una condicion de evaluación.
   */
  void clearAcosCondition() {
    if (obj_)
      obj_->setAcosCondition(QString::null, FLSqlCursor::VALUE, QVariant());
  }

  /**
  QSqlCursor::setForwardOnly()
  */
  void setForwardOnly(bool forward) {
    obj_->setForwardOnly(forward);
  }

  /**
  Comprueba si hay una colisión de campos editados por dos sesiones simultáneamente.

  @return Lista con los nombres de los campos que colisionan
  */
  QStringList concurrencyFields() {
    return obj_->concurrencyFields();
  }

  FLSqlCursor *obj() {
    return obj_;
  }

  /**
  Uso interno. Obsoleto
  */
  void emitNewBuffer() {
    emit newBuffer();
  }
  void emitBufferChanged(const QString &v) {
    emit bufferChanged(v);
  }
  void emitCursorUpdated() {
    emit cursorUpdated();
  }
  void emitRecordChoosed() {
    emit recordChoosed();
  }
  void emitCurrentChanged(int v) {
    emit currentChanged(v);
  }
  void emitAutoCommit() {
    emit autoCommit();
  }
  void emitBufferCommited() {
    emit bufferCommited();
  }

signals:

  /**
   Indica que se ha cargado un nuevo buffer
   */
  void newBuffer();

  /**
   Indica si ha cambiado un campo del buffer, junto con la señal se envía el nombre del campo que
   ha cambiado.
   */
  void bufferChanged(const QString &);

  /**
   Indica que se ha actualizado el cursor
   */
  void cursorUpdated();

  /**
   Indica que se ha elegido un registro, mediante doble clic sobre él o bien pulsando la tecla Enter
   */
  void recordChoosed();

  /**
   Indica que la posición del registro activo dentro del cursor ha cambiado
   */
  void currentChanged(int);

  /**
   Indica que se ha realizado un commit automático para evitar bloqueos
   */
  void autoCommit();

  /**
   Indica que se ha realizado un commit
   */
  void bufferCommited();

private:

  void connects() const;
  void removeCache();

  FLSqlCursor *obj_;
  FLSqlCursorInterface *cursorRelation_;
  bool cached_;

  static FLSqlCursorsCache *cursorsCache;

  friend class FLObjectFactory;
};

//! Manejo de consultas sql
/**
 Permite realizar una consulta SQL completa con todas las cláusulas
 (select, from, where, group by, etc)

 @author InfoSiAL S.L.
 */
class  FLSqlQueryInterface : public QObject
{
  Q_OBJECT

public:

  /**
   Constructor
   */
  FLSqlQueryInterface(const QString &n = QString::null, const QString &connName = "default") : QObject(0) {
    if (n.isEmpty())
      obj_ = new FLSqlQuery(this, connName);
    else
      obj_ = FLSqlConnections::database(connName)->manager()->query(n, this);
  }

  /**
   Constructor
   */
  FLSqlQueryInterface(FLSqlQuery *obj) : QObject(obj), obj_(0) {
    obj_ = obj;
  }

public slots:

  /**
   Obtiene la parte SELECT de la sentencia SQL de la consulta
   */
  QString select() const {
    return obj_->select();
  }

  /**
   Obtiene la parte FROM de la sentencia SQL de la consulta
   */
  QString from() const {
    return obj_->from();
  }

  /**
   Obtiene la parte WHERE de la sentencia SQL de la consulta
   */
  QString where() const {
    return obj_->where();
  }

  /**
   Para obtener la parte ORDER BY de la sentencia SQL de la consulta
   */
  QString orderBy() const {
    return obj_->orderBy();
  }

  /**
   Para establecer la parte SELECT de la sentencia SQL de la consulta.

   @param  s Cadena de texto con la parte SELECT de la sentencia SQL que
   genera la consulta. Esta cadena NO debe incluir la palabra reservada
   SELECT, ni tampoco el caracter '*' como comodín. Solo admite la lista
   de campos que deben aparecer en la consulta separados por la cadena
   indicada en el parámetro 'sep'
   @param  sep Cadena utilizada como separador en la lista de campos. Por defecto
   se utiliza la coma.
   */
  void setSelect(const QString &s, const QString &sep = QString::fromLatin1(",")) {
    obj_->setSelect(s, sep);
  }

  /**
   Establece la parte FROM de la sentencia SQL de la consulta.

   @param f Cadena de texto con la parte FROM de la sentencia SQL que
   genera la consulta
   */
  void setFrom(const QString &f) {
    obj_->setFrom(QString(f).replace("NaN", ""));
  }

  /**
   Establece la parte WHERE de la sentencia SQL de la consulta.

   @param s Cadena de texto con la parte WHERE de la sentencia SQL que
   genera la consulta
   */
  void setWhere(const QString &w) {
    obj_->setWhere(QString(w).replace("NaN", ""));
  }

  /**
   Establece la parte ORDER BY de la sentencia SQL de la consulta.

   @param s Cadena de texto con la parte ORDER BY de la sentencia SQL que
   genera la consulta
   */
  void setOrderBy(const QString &oB) {
    obj_->setOrderBy(QString(oB).replace("NaN", ""));
  }

  /**
   Para obtener la sentencia completa SQL de la consulta.

   Este método une las tres partes de la consulta (SELECT, FROM Y WHERE),
   sustituye los parámetros por el valor que tienen en el diccionario y devuelve
   todo en una cadena de texto.

   @return Cadena de texto con la sentencia completa SQL que genera la consulta
   */
  QString sql() const {
    return obj_->sql();
  }

  /**
   Reimplementación de QSqlQuery::value(int)
   */
  QVariant value(int i) const {
    return obj_->value(i);
  }

  QVariant value(const QString &n) const {
    return obj_->value(n);
  }

  /**
   Devuelve si el contenido de un campo de la consulta es nulo.

   @param i Posicion del campo en el buffer
   */
  bool isNull(int i) const {
    return obj_->isNull(i);
  }

  /**
   Devuelve si el contenido de un campo de la consulta es nulo.

   @param n nombre del del campo en la sentencia SELECT
   */
  bool isNull(QString n) const {
    return obj_->isNull(n);
  }



  /**
   Devuelve el nombre de campo, dada su posicion en la consulta.

   @param p Posicion del campo en la consulta, empieza en cero y de izquierda
   a derecha
   @return Nombre del campo correspondiente. Si no existe el campo devuelve
   QString::null
   */
  QString posToFieldName(const int p) const {
    return obj_->posToFieldName(p);
  }

  /**
   Devuelve la posición de una campo en la consulta, dado su nombre.

   @param n Nombre del campo
   @return Posicion del campo en la consulta. Si no existe el campo devuelve -1
   */
  int fieldNameToPos(const QString &n) const {
    return obj_->fieldNameToPos(n);
  }

  /**
   Establece la lista de nombres de las tablas de la consulta

   @param tl Cadena de texto con los nombres de las tablas
   separados por comas, p.e. "tabla1,tabla2,tabla3"
   */
  void setTablesList(const QString &tl) {
    obj_->setTablesList(tl);
  }

  /**
   Obtiene el valor de un parámetro.

   @param name Nombre del parámetro.
   */
  QVariant valueParam(const QString &name) const {
    return obj_->valueParam(name);
  }

  /**
   Establece el valor de un parámetro.

   @param name Nombre del parámetro
   @param v Valor para el parámetros
   */
  void setValueParam(const QString &name, const QVariant &v) {
    obj_->setValueParam(name, v);
  }

  /**
   Recupera el siguiente registro del resultado de la consulta asociada con el cursor

   @return TRUE si existe un siguiente registro, FALSE si no existe.
   */
  bool next() {
    return obj_->next();
  }

  /**
   Recupera el anterior registro del resultado de la consulta asociada con el cursor

   @return TRUE si existe un anterior registro, FALSE si no existe.
   */
  bool prev() {
    return obj_->prev();
  }

  /**
   Recupera el primer registro del resultado de la consulta asociada con el cursor

   @return TRUE si existe un primer registro, FALSE si no existe.
   */
  bool first() {
    return obj_->first();
  }

  /**
   Recupera el último registro del resultado de la consulta asociada con el cursor

   @return TRUE si existe un último registro, FALSE si no existe.
   */
  bool last() {
    return obj_->last();
  }

  /**
   Ejecuta la consulta
   */
  bool exec() {
    return obj_->exec();
  }

  /**
   Este método muestra el contenido de la consulta, por la sálida estándar.

   Está pensado sólo para tareas de depuración
   */
  void showDebug() {
    obj_->showDebug();
  }

  /**
   Devuelve el número de líneas que devuelve la consulta

   @return Número de líneas (-1 si la consulta no está activa o la BD no da información sobre número de filas)
   */
  int size() const {
    return obj_->size();
  }

  void setForwardOnly(bool forward) {
    obj_->setForwardOnly(forward);
  }

  FLSqlQuery *obj() {
    return obj_;
  }

private:

  FLSqlQuery *obj_;
};

//! Acceso a campos de las tablas desde formularios.
/**
 Acceso a los campos de las tablas de base de datos mediante controles de formulario.
 Permite leer y establecer valores y cambiar las propiedades de los campos de un formulario.

 @author InfoSiAL S.L.
 */
class  FLFieldDBInterface : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString fieldName READ fieldName WRITE setFieldName)
  Q_PROPERTY(QString tableName READ tableName WRITE setTableName)
  Q_PROPERTY(QString foreignField READ foreignField WRITE setForeignField)
  Q_PROPERTY(QString fieldRelation READ fieldRelation WRITE setFieldRelation)
  Q_PROPERTY(QString fieldAlias READ fieldAlias WRITE setFieldAlias)
  Q_PROPERTY(QString actionName READ actionName WRITE setActionName)
  Q_PROPERTY(bool showAlias READ showAlias WRITE setShowAlias)
  Q_PROPERTY(bool showEditor READ showEditor WRITE setShowEditor)
  Q_PROPERTY(int textFormat READ textFormat WRITE setTextFormat)

public:

  /**
   Constructor
   */
  FLFieldDBInterface(QWidget *p, const QString &n) : QObject(p) {
    obj_ = new FLFieldDB(p, n);
    if (obj_)
      connects();
  }

  /**
   Constructor
   */
  FLFieldDBInterface(FLFieldDB *obj) : QObject(obj) {
    obj_ = obj;
    if (obj_)
      connects();
  }

  /**
   Constructor
   */
  FLFieldDBInterface() : QObject(0) {}

public slots:

  /**
   Devuelve el cursor del formulario

   @return Cursor del formulario
   */
  FLSqlCursorInterface *cursor() {
    if (!obj_)
      return new FLSqlCursorInterface();
    return FLSqlCursorInterface::sqlCursorInterface(obj_->cursor());
  }

  /**
   Inhabilita el campo

   @param b. TRUE si hay que inhabilitar el campo, FALSE si hay que habilitarlo
   */
  void setDisabled(const bool b) {
    obj_->setDisabled(b);
    obj_->setKeepDisabled(b);
  }

  /**
   Obtiene el nombre de la acción.

   @return Nombre de la accion
   */
  QString actionName() const {
    return obj_->actionName();
  }

  /**
   Establece el nombre de la acción.

   @param aN Nombre de la accion
   */
  void setActionName(const QString &aN) {
    obj_->setActionName(aN);
  }

  /**
   Obtiene el nombre del campo.

   @return Nombre del campo
   */
  QString fieldName() const {
    return obj_->fieldName();
  }

  /**
   Establece el nombre del campo.

   @param fN Nombre del campo
   */
  void setFieldName(const QString &fN) {
    obj_->setFieldName(fN);
  }

  /**
   Obtiene el nombre de la tabla foránea.

   @return Nombre de la tabla
   */
  QString tableName() const {
    return obj_->tableName();
  }

  /**
   Establece el nombre de la tabla foránea.

   @param fT Nombre de la tabla
   */
  void setTableName(const QString &fT) {
    obj_->setTableName(fT);
  }

  /**
   Obtiene el nombre del campo foráneo.

   @return Nombre del campo
   */
  QString foreignField() const {
    return obj_->foreignField();
  }

  /**
   Establece el nombre del campo foráneo.

   @param fN Nombre del campo
   */
  void setForeignField(const QString &fN) {
    obj_->setForeignField(fN);
  }

  /**
   Obtiene el nombre del campo relacionado.

   @return Nombre del campo
   */
  QString fieldRelation() const {
    return obj_->fieldRelation();
  }

  /**
   @return Alias del campo, es el valor mostrado en la etiqueta
   */
  QString fieldAlias() const {
    return obj_->fieldAlias();
  }

  /**
   Obtiene el widget editor.

   @return Objeto con el editor del campo
   */
  QWidget *editor() const {
    return obj_->editor();
  }

  /**
   Añade un filtro al cursor.

   */
  void setFilter(const QString &f) {
    obj_->setFilter(f);
  }

  /**
   Obtiene el filtro del cursor.

   */
  QString filter() const {
    return obj_->filter();
  }

  /**
   Establece el nombre del campo relacionado.

   @param fN Nombre del campo
   */
  void setFieldRelation(const QString &fN) {
    obj_->setFieldRelation(fN);
  }

  /**
   Para establecer el alias del campo, mostrado en su etiqueta si showAlias es true

   @param alias Alias del campo, es el valor de la etiqueta. Si es vacio no hace nada.
   */
  void setFieldAlias(const QString &alias) {
    obj_->setFieldAlias(alias);
  }

  /**
   Establece el valor contenido en el campo.

   @param v Valor a establecer
   */
  void setValue(const QVariant &v) {
    if (v.isValid() && !v.isNull()) {
      if (v.toString().upper() == "NAN") {
        obj_->setValue(QVariant());
        return;
      }
    }
    obj_->setValue(v);
  }
  void setValue(const QString &v) {
    setValue(QVariant(v));
  }
  void setValue(bool v) {
    setValue(QVariant(v));
  }
  void setValue(double v) {
    setValue(QVariant(v));
  }
  void setValue(int v) {
    setValue(QVariant(v));
  }

  /**
   Establece una imagen en el campo

   @param filename Ruta del fichero que contiene la imagen
   */
  void setPixmap(const QString &filename) {
    obj_->setPixmap(filename);
  }

  /**
  Guarda imagen de campos tipo Pixmap en una ruta determinada.

  @param filename: Ruta al fichero donde se guardará la imagen
  @param fmt Indica el formato con el que guardar la imagen
  @author Silix
  */
  void savePixmap(const QString &filename, const char *format) {
    obj_->savePixmap(filename, format);
  }

  /**
  Devueve el objeto imagen asociado al campo

  @return imagen asociada al campo
  @author Silix
  */
  QPixmap pixmap() {
    return obj_->pixmap();
  }

  /**
   Hace que el control tome el foco
   */
  void setFocus() {
    obj_->setFocus();
  }

  /**
   Marca como seleccionado el contenido del campo.
   */
  void selectAll() {
    obj_->selectAll();
  }

  /**
   Obtiene el valor contenido en el campo.
   */
  QVariant value() {
    obj_->showWidget();
    return obj_->value();
  }

  /**
   Esconde el campo
   */
  void close() {
    obj_->close();
  }

  /**
   Inserta como acelerador de teclado una combinación de teclas, devociendo su identificador

   @param key Cadena de texto que representa la combinación de teclas (p.e. "Ctrl+Shift+O")
   @return El identificador asociado internamente a la combinación de teclas aceleración insertada
   */
  int insertAccel(const QString &key) {
    obj_->insertAccel(key);
  }

  /**
   Elimina, desactiva, una combinación de teclas de aceleración según su identificador.

   @param id Identificador de la combinación de teclas de aceleración
   */
  void removeAccel(int id) {
    obj_->removeAccel(id);
  }

  /**
   Establece el formato del texto

   @param f Formato del campo
   */
  void setTextFormat(const int &f) {
    Qt::TextFormat tt;
    switch (f) {
      case 0: {
        tt = Qt::PlainText;
        break;
      }
      case 1: {
        tt = Qt::RichText;
        break;
      }
      case 2: {
        tt = Qt::AutoText;
        break;
      }
      case 3: {
        tt = Qt::LogText;
        break;
      }
      default : {
        return;
      }
    }
    obj_->setTextFormat(tt);
  }

  /**
   @return El formato del texto
   */
  int textFormat() const {
    return obj_->textFormat();
  }

  /**
   Cuando se pulsa una combinación de teclas de aceleración se emite esta señal indicando el identificador
   de la combinación de teclas pulsada
   */
  void emitActivatedAccel(int id) {
    emit activatedAccel(id);
  }

  /**
   Devuelve el valor de la propiedad showAlias. Esta propiedad es
   usada para saber si hay que mostrar el alias cuando se está
   en modo de cursor relacionado.
   */
  bool showAlias() const {
    return obj_->showAlias();
  }

  /**
   Establece el estado de la propiedad showAlias.
   */
  void setShowAlias(const bool value) {
    obj_->setShowAlias(value);
  }

  /**
   Devuelve el valor de la propiedad showEditor.
   */
  bool showEditor() const {
    return obj_->showEditor();
  }

  /**
   Establece el valor de la propiedad showEditor.
   */
  void setShowEditor(const bool show) {
    obj_->setShowEditor(show);
  }

  void setPartDecimal(int d) {
    obj_->setPartDecimal(d);
  }

  FLFieldDB *obj() {
    return obj_;
  }

signals:

  /**
   Cuando se pulsa una combinación de teclas de aceleración se emite esta señal indicando el identificador
   de la combinación de teclas pulsada
   */
  void activatedAccel(int);

  /**
   Señal emitida si se pulsa la tecla F2 en el editor
   */
  void keyF2Pressed();

  /**
   Señal emitida si se pulsa la tecla F4 en el editor
   */
  void keyF4Pressed();

  /**
   Señal emitida si se pulsa la tecla Return
   */
  void keyReturnPressed();

private:

  void connects() const;

  FLFieldDB *obj_;
};

//! Acceso a tablas desde formularios.
/**
 Acceso a las tablas de base de datos mediante controles de formulario.
 Permite leer y establecer valores y cambiar las propiedades.

 @author InfoSiAL S.L.
 */
class  FLTableDBInterface : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString tableName READ tableName WRITE setTableName)
  Q_PROPERTY(QString foreignField READ foreignField WRITE setForeignField)
  Q_PROPERTY(QString fieldRelation READ fieldRelation WRITE setFieldRelation)
  Q_PROPERTY(bool checkColumnEnabled READ checkColumnEnabled WRITE setCheckColumnEnabled)
  Q_PROPERTY(QString aliasCheckColumn READ aliasCheckColumn WRITE setAliasCheckColumn)
  Q_PROPERTY(bool findHidden READ findHidden WRITE setFindHidden)
  Q_PROPERTY(bool filterHidden READ filterHidden WRITE setFilterHidden)
  Q_PROPERTY(bool showAllPixmaps READ showAllPixmaps WRITE setShowAllPixmaps)
  Q_PROPERTY(QString functionGetColor READ functionGetColor WRITE setFunctionGetColor)

public:

  /**
   Constructor
   */
  FLTableDBInterface(QWidget *p, const QString &n) : QObject(p) {
    obj_ = new FLTableDB(p, n);
    if (obj_)
      connects();
  }

  /**
   Constructor
   */
  FLTableDBInterface(FLTableDB *obj) : QObject(obj) {
    obj_ = obj;
    if (obj_)
      connects();
  }

  /**
   Constructor
   */
  FLTableDBInterface() : QObject(0) {}

public slots:

  /**
   Obtiene el nombre de la tabla asociada.

   @return Nombre de la tabla asociado
   */
  QString tableName() const {
    return obj_->tableName();
  }

  /**
   Establece el nombre de la tabla asociada.

   @param fT Nombre de la tabla asociada
   */
  void setTableName(const QString &fT) {
    obj_->setTableName(fT);
  }

  /**
   Obtiene el nombre del campo foráneo.

   @return Nombre del campo
   */
  QString foreignField() const {
    return obj_->foreignField();
  }

  /**
   Establece el nombre del campo foráneo.

   @param fN Nombre del campo
   */
  void setForeignField(const QString &fN) {
    obj_->setForeignField(fN);
  }

  /**
   Obtiene el nombre del campo relacionado.

   @return Nombre del campo
   */
  QString fieldRelation() const {
    return obj_->fieldRelation();
  }

  /**
   Establece el nombre del campo relacionado.

   @param fN Nombre del campo
   */
  void setFieldRelation(const QString &fN) {
    obj_->setFieldRelation(fN);
  }

  /**
   Establece si el componente esta en modo solo lectura o no.
   */
  void setReadOnly(const bool mode) {
    obj_->setReadOnly(mode);
  }

  /**
   Establece si el componente esta en modo solo edición o no.
   */
  void setEditOnly(const bool mode) {
    obj_->setEditOnly(mode);
  }

  /**
   Establece el componente a sólo inserción o no.
   */
  void setInsertOnly(const bool mode) {
    obj_->setInsertOnly(mode);
  }

  /**
   Actualiza el conjunto de registros.
   */
  void refresh() {
    obj_->refresh(false, true);
  }

  void refresh(const bool refreshHead, const bool refreshData) {
    obj_->refresh(refreshHead, refreshData);
  }

  /**
   Invoca al método FLSqlCursor::insertRecord()
   */
  void insertRecord() {
    obj_->insertRecord();
  }

  /**
   Invoca al método FLSqlCursor::editRecord()
   */
  void editRecord() {
    obj_->editRecord();
  }

  /**
   Invoca al método FLSqlCursor::browseRecord()
   */
  void browseRecord() {
    obj_->browseRecord();
  }

  /**
   Invoca al método FLSqlCursor::deleteRecord()
   */
  void deleteRecord() {
    obj_->deleteRecord();
  }

  /**
   Invoca al método FLSqlCursor::copyRecord()
   */
  void copyRecord() {
    obj_->copyRecord();
  }

  /**
   Redefinida por conveniencia
   */
  void show() {
    obj_->show();
  }

  /**
   Hace que el la tabla tome el foco
   */
  void setFocus() {
    obj_->setFocus();
  }

  /**
   Hace que la tabla aplique un filtro
   @param f: Filtro a aplicar
   */
  void setFilter(const QString &f) {
    obj_->setFilter(f);
  }

  /**
   Obtiene el filtro actual de la tabla
   @return Filtro
   */
  QString filter() {
    return obj_->filter();
  }

  /**
   Obtiene el filtro impuesto en el Find.

   */
  QString findFilter() const {
    return obj_->findFilter();
  }

  /**
   Establece el filtro inicial de búsqueda
   */
  void setInitSearch(const QString &iS) {
    obj_->setInitSearch(iS);
  }

  /**
   Obtiene el cursor utilizado por el componente.

   return Objeto FLSqlCursor con el cursor que contiene los registros para ser utilizados en el formulario
   */
  FLSqlCursorInterface *cursor() {
    if (!obj_)
      return new FLSqlCursorInterface();
    return FLSqlCursorInterface::sqlCursorInterface(obj_->cursor());
  }

  void emitReadOnlyChanged(bool b) {
    emit readOnlyChanged(b);
  }

  void emitEditOnlyChanged(bool b) {
    emit editOnlyChanged(b);
  }

  void emitInsertOnlyChanged(bool b) {
    emit insertOnlyChanged(b);
  }

  /**
   Emite la señal currentChanged, cuando el objeto tableRecords envía dicha señal
   */
  void emitCurrentChanged() {
    emit currentChanged();
  }

  /**
   Establece la fila actual

   @param r. Índice de la fila
   */
  void setCurrentRow(const int r) {
    FLDataTable *t = obj_->tableRecords();
    if (t) {
      obj_->showWidget();
      t->selectRow(r);
    }
  }

  /**
   Devuelve el número de fila actual

   @return Índice de la fila actual
   */
  int currentRow(void) {
    FLDataTable *t = obj_->tableRecords();
    if (t) {
      obj_->showWidget();
      return t->currentRow();
    }
    return -1;
  }

  /**
   Esconde la tabla
   */
  void close() {
    obj_->close();
  }

  FLTableDB *obj() {
    return obj_;
  }

  /**
   Inhabilita la tabla

   @param b. TRUE si hay que inhabilitar la tabla, FALSE si hay que habilitarla
   */
  void setDisabled(const bool b) {
    obj_->setDisabled(b);
  }

  /**
   Establece el campo de búsqueda
   */
  void putFirstCol(const QString &c) {
    obj_->putFirstCol(c);
  }

  /**
   Mueve una columna de un campo origen a la columna de otro campo destino

   @param  from  Nombre del campo de la columna de origen
   @param  to    Nombre del campo de la columna de destino
   */
  void moveCol(const QString &from, const QString &to) {
    obj_->moveCol(from, to);
  }

  /**
   Establece el orden de las columnas de la tabla.

   @param fields Lista de los nombres de los campos ordenada según se desea que aparezcan en la tabla
   */
  void setOrderCols(QStringList &fields) {
    obj_->setOrderCols(fields);
  }

  /**
   Devuelve la lista de los campos ordenada por sus columnas en la tabla de izquierda a derecha
   */
  QStringList orderCols() {
    return obj_->orderCols();
  }

  /**
   Obtiene si la columna de selección está activada
   */
  bool checkColumnEnabled() const {
    return obj_->checkColumnEnabled();
  }

  /**
   Establece el estado de activación de la columna de selección

   El cambio de estado no será efectivo hasta el siguiente refresh.
   */
  void setCheckColumnEnabled(bool b) {
    obj_->setCheckColumnEnabled(b);
  }

  /**
   Obiente el texto de la etiqueta de encabezado para la columna de selección
   */
  QString aliasCheckColumn() const {
    return obj_->aliasCheckColumn();
  }

  /**
   Establece el texto de la etiqueta de encabezado para la columna de selección

   El cambio del texto de la etiqueta no será efectivo hasta el próximo refresh
   */
  void setAliasCheckColumn(const QString &t) {
    obj_->setAliasCheckColumn(t);
  }

  /**
   Obtiene la lista con las claves primarias de los registros seleccionados por chequeo
   */
  QStringList primarysKeysChecked();

  /**
   Limpia la lista con las claves primarias de los registros seleccionados por chequeo
   */
  void clearChecked() {
    FLDataTable *dt = obj_->tableRecords();
    if (dt)
      dt->clearChecked();
  }

  /**
   Establece el estado seleccionado por chequeo para un registro, indicando el valor de su clave primaria
   */
  void setPrimaryKeyChecked(const QVariant &primaryKeyValue, bool on) {
    FLDataTable *dt = obj_->tableRecords();
    if (dt)
      dt->setPrimaryKeyChecked(primaryKeyValue, on);
  }

  /**
   Indica que ha cambiado el estado del campo de selección de un registro. Es decir
   se ha incluido o eliminado su clave primaria de la lista de claves primarias seleccionadas.
   Esta señal se emite cuando el usuario hace click en el control de chequeo y cuando se cambia
   programáticamente el chequeo mediante el método FLDataTable::setPrimaryKeyChecked.

   @param  primaryKeyValue El valor de la clave primaria del registro correspondiente
   @param  on  El nuevo estado; TRUE chequeo activado, FALSE chequeo desactivado
   */
  void emitPrimaryKeyToggled(const QVariant &primaryKeyValue, bool on) {
    emit primaryKeyToggled(primaryKeyValue, on);
  }

  /**
   Obtiene si el marco de búsqueda está oculto
   */
  bool findHidden() const {
    return obj_->findHidden();
  }

  /**
   Oculta o muestra el marco de búsqueda

   @param  h TRUE lo oculta, FALSE lo muestra
   */
  void setFindHidden(bool h) {
    obj_->setFindHidden(h);
  }

  /**
   Obtiene si el marco para conmutar entre datos y filtro está oculto
   */
  bool filterHidden() const {
    return obj_->filterHidden();
  }

  /**
   Oculta o muestra el marco para conmutar entre datos y filtro

   @param  h TRUE lo oculta, FALSE lo muestra
   */
  void setFilterHidden(bool h) {
    obj_->setFilterHidden(h);
  }

  /**
   Establece el ancho de una columna

   @param  field Nombre del campo de la base de datos correspondiente a la columna
   @param  w     Ancho de la columna
   */
  void setColumnWidth(const QString &field, int w) {
    obj_->setColumnWidth(field, w);
  }

  /**
   @return Ancho de la columna
   */
  int columnWidth(int c) {
    return obj_->columnWidth(c);
  }

  /**
   Establece el alto de una fila

   @param  row Número de orden de la fila, empezando en 0
   @param  h   Alto de la fila
   */
  void setRowHeight(int row, int h) {
    obj_->setRowHeight(row, h);
  }

  /**
   @return Alto de la fila
   */
  int rowHeight(int row) {
    return obj_->rowHeight(row);
  }

  /**
   Ver FLTableDB::showAllPixmaps_
   */
  bool showAllPixmaps() const {
    return obj_->showAllPixmaps();
  }

  /**
   Ver FLTableDB::showAllPixmaps_
   */
  void setShowAllPixmaps(bool s) {
    obj_->setShowAllPixmaps(s);
  }

  /**
   Ver FLTableDB::functionGetColor_
   */
  QString functionGetColor() const {
    return obj_->functionGetColor();
  }

  /**
   Ver FLTableDB::functionGetColor_
   */
  void setFunctionGetColor(const QString &f) {
    obj_->setFunctionGetColor(f);
  }

signals:

  /**
   Señal emitida cuando se establece si el componente es o no de solo lectura.
   */
  void readOnlyChanged(bool);

  /**
   Señal emitida cuando se establece si el componente es o no de solo edición.
   */
  void editOnlyChanged(bool);

  /**
   Señal emitida cuando se establece si el componente es o no de solo inserción.
   */
  void insertOnlyChanged(bool);

  /**
   Señal emitida cuando se establece cambia el registro seleccionado.
   */
  void currentChanged();

  /**
   Indica que ha cambiado el estado del campo de selección de un registro. Es decir
   se ha incluido o eliminado su clave primaria de la lista de claves primarias seleccionadas.
   Esta señal se emite cuando el usuario hace click en el control de chequeo y cuando se cambia
   programáticamente el chequeo mediante el método FLDataTable::setPrimaryKeyChecked.

   @param  primaryKeyValue El valor de la clave primaria del registro correspondiente
   @param  on  El nuevo estado; TRUE chequeo activado, FALSE chequeo desactivado
   */
  void primaryKeyToggled(const QVariant &, bool);

private:

  void connects() const;

  FLTableDB *obj_;
};

//! Acceso directo a tablas para editar su estructura
/**
 Acceso a las tablas de base de datos.
 Permite editar su estructura: nombre, alias o propiedades de los campos entre otras

 @author InfoSiAL S.L.
 */
#define NEW_TABLE if (!obj_) { obj_ = new FLTable(qApp->mainWidget()); connects(); }

class FLTableInterface : public QObject
{
  Q_OBJECT
  Q_ENUMS(CellAlignment)

public:

  /**
   Constructor
   */
  FLTableInterface(QWidget *p, const QString &n) : QObject(p) {
    obj_ = new FLTable(p, n);
    if (obj_)
      connects();
  }

  /**
   Constructor
   */
  FLTableInterface() : QObject(0), obj_(0) {}

  /**
   Constructor
   */
  FLTableInterface(QTable *t) : QObject(t), obj_(t) {
    connects();
  }

  enum CellAlignment {
    AlignAuto = Qt::AlignAuto,
    AlignLeft = Qt::AlignLeft,
    AlignRight = Qt::AlignRight,
    AlignHCenter = Qt::AlignHCenter,
    AlignJustify = Qt::AlignJustify,
    AlignTop = Qt::AlignTop,
    AlignBottom = Qt::AlignBottom,
    AlignVCenter = Qt::AlignVCenter,
    AlignCenter = Qt::AlignCenter
  };

public slots:

  /**
   Cambia una fila de modo solo lectura a lectura - escritura y viceversa.

   @param row. número de fila
   @param ro. TRUE si la fila es de sólo lectura, FALSE si no lo es
   */
  void setRowReadOnly(int row, bool ro) {
    NEW_TABLE
    obj_->setRowReadOnly(row, ro);
  }

  /**
   Ordena una columna

   @param col: Número de columna
   @param ascending. Orden ascendente (true) o descendente (false)
   @param wholeRows. Ordenar sólo la columna (false) o las filas enteras (true)
   */
  void sortColumn(int col, bool ascending = true, bool wholeRows = false) {
    NEW_TABLE
    obj_->sortColumn(col, ascending, wholeRows);
  }

  /**
   Cambia una columna de modo solo lectura a lectura - escritura y viceversa.

   @param col. número de columna
   @param ro. TRUE si la fila es de sólo lectura, FALSE si no lo es
   */
  void setColumnReadOnly(int col, bool ro) {
    NEW_TABLE
    obj_->setColumnReadOnly(col, ro);
  }

  /**
   Elimina una fila

   @param row.número de la fila a eliminar
   */
  void removeRow(int row) {
    NEW_TABLE
    obj_->removeRow(row);
  }

  /**
   Elimina todas las filas
   */
  void clear();

  /**
   Inicia la edición de una celda

   @param fil. número de la fila
   @param col. número de la columna
   */
  void editCell(int row, int col) {
    NEW_TABLE
    obj_->editCell(row, col, false);
  }

  /**
   Esconde una columna

   @param col. número de la columna
   */
  void hideColumn(int col) {
    NEW_TABLE
    obj_->hideColumn(col);
  }

  /**
   Esconde una fila

   @param row. número de la fila
   */
  void hideRow(int row) {
    NEW_TABLE
    obj_->hideRow(row);
  }

  /**
   Muestra una columna

   @param col. número de la columna
   */
  void showColumn(int col) {
    NEW_TABLE
    obj_->showColumn(col);
  }

  /**
   Muestra una fila

   @param row. número de la fila
   */
  void showRow(int row) {
    NEW_TABLE
    obj_->showRow(row);
  }

  /**
   Devuelve el texto que contiene una determinada casilla de la tabla

   @param row. número de la fila de la casilla
   @param col. número de la columna de la casilla
   */
  QString text(int row, int col) {
    NEW_TABLE
    return obj_->text(row, col);
  }

  /**
   Establece los títulos de todas las columnas

   @param sep. Caracter separador de los títulos
   @param str. Lista de títulos separados por el carácter separador
   */
  void setColumnLabels(const QString &sep, const QString &str) {
    NEW_TABLE
    QStringList sL = QStringList::split(sep, str);
    obj_->setColumnLabels(sL);
  }

  /**
   Establece los títulos de las filas

   @param sep. Caracter separador de los títulos
   @param str. Lista de títulos separados por el carácter separador
   */
  void setRowLabels(const QString &sep, const QString &str) {
    NEW_TABLE
    QStringList sL = QStringList::split(sep, str);
    obj_->setRowLabels(sL);
  }

  /**
   Establece el margen para los títulos de las filas

   @param m. Margen
   */
  void setLeftMargin(int m) {
    NEW_TABLE
    obj_->setLeftMargin(m);
  }

  /**
   Establece el margen para los títulos de las columnas

   @param m. Margen
   */
  void setTopMargin(int m) {
    NEW_TABLE
    obj_->setTopMargin(m);
  }

  /**
   Establece el número total de columnas de la tabla

   @param c. número de columnas que tendrá la tabla
   */
  void setNumCols(const int c) {
    NEW_TABLE
    obj_->setNumCols(c);
  }

  /**
   Establece el número total de filas de la tabla

   @param c. número de filas que tendrá la tabla
   */
  void setNumRows(const int r) {
    NEW_TABLE
    obj_->setNumRows(r);
  }

  /**
   Establece la anchura de un columna
   @param c. Número de la columna
   @param w. Anchura
   */
  void setColumnWidth(const int c, const int w) {
    NEW_TABLE
    obj_->setColumnWidth(c, w);
  }

  /**
   @return Ancho de la columna
   */
  int columnWidth(int c) {
    NEW_TABLE
    return obj_->columnWidth(c);
  }

  /**
   Establece el alto de una fila

   @param  row Número de orden de la fila, empezando en 0
   @param  h   Alto de la fila
   */
  void setRowHeight(int row, int h) {
    NEW_TABLE
    obj_->setRowHeight(row, h);
  }

  /**
   @return Alto de la fila
   */
  int rowHeight(int row) {
    NEW_TABLE
    return obj_->rowHeight(row);
  }

  /**
   Devuelve el número de fila actual.

   @return  número de fila actual
   */
  int currentRow() {
    NEW_TABLE
    return obj_->currentRow();
  }

  /**
   Devuelve el númwero de columna actual.

   @return  número de columna actual
   */
  int currentColumn() {
    NEW_TABLE
    return obj_->currentColumn();
  }

  /**
   Selecciona el número de fila establecido

   @param row. número de fila que se desea seleccionar
   */
  void selectRow(int row) {
    NEW_TABLE
    obj_->selectRow(row);
  }

  /**
   Devuelve el número total de filas de la tabla

   @return número de filas
   */
  int numRows() {
    NEW_TABLE
    return obj_->numRows();
  }

  /**
   Devuelve el número total de columnas de la tabla

   @return número de columnas
   */
  int numCols() {
    NEW_TABLE
    return obj_->numCols();
  }

  /**
   Cambia una fila por otra
   @param row1: Fila 1
   @param row2: Fila 2
   @param swapHeader: Indica si hay que cambiar también las cabeceras de las filas
   */
  void swapRows(int row1, int row2, bool swapHeader = false) {
    NEW_TABLE
    return obj_->swapRows(row1, row2, swapHeader);
  }

  /**
   Cambia una columna por otra
   @param col1: Columna 1
   @param col2: Columna 2
   @param swapHeader: Indica si hay que cambiar también las cabeceras de las columnas
   */
  void swapColumns(int col1, int col2, bool swapHeader = false) {
    NEW_TABLE
    return obj_->swapColumns(col1, col2, swapHeader);
  }

  /**
   Actualiza los contenidos
   */
  void repaintContents() {
    NEW_TABLE
    return obj_->repaintContents();
  }

  /**
   Clear selections
   */
  void clearSelection(bool repaint = true) {
    NEW_TABLE
    return obj_->clearSelection(repaint);
  }

  /**
   Devuelve una lista con las filas seleccionadas, separadas por comas y comenzando por cero.
   Para objetos con selectionMode = SingleRow / MultiRow

   @return Filas seleccionadas
   */
  QStringList selectedRows();

  /**
   Devuelve una lista con las columnas seleccionadas, separadas por comas y comenzando por cero.
   Para objetos con selectionMode = Multi

   @return Columnas seleccionadas
   */
  QStringList selectedCols();

  /**
   Devuelve una lista con las celdas seleccionadas, con el formato fila.columna, separadas por comas y
   comenzando por ceros. Para objetos con selectionMode = Single / Multi

   @return Celdas seleccionadas
   */
  QStringList selectedCells();

  /**
   Establece un texto para casilla determinada

   @param row. número de fila
   @param col. número de columna
   @param text. texto que deseamos insertar
   */
  void setText(int row, int col, const QString &text) {
    NEW_TABLE
    int d = row - obj_->numRows() + 1;
    if (d > 0)
      insertRows(obj_->numRows() - 1, d);
    d = col - obj_->numCols() + 1;
    if (d > 0)
      insertColumns(obj_->numCols() - 1, d);
    obj_->setText(row, col, text);
  }

  /**
   Inserta una o más nuevas filas en el lugar establecido

   @param row. posición donde insertar las nuevas filas
   @param count. número de filas a insertar
   */
  void insertRows(int row, int count = 1) {
    NEW_TABLE
    obj_->insertRows(row, count);
  }

  /**
   Inserta una o más nuevas columnas en el lugar establecido

   @param col. posición donde insertar las nuevas columnas
   @param count. número de columnas a insertar
   */
  void insertColumns(int col, int count = 1) {
    NEW_TABLE
    obj_->insertColumns(col, count);
  }

  /**
   Ajusta el ancho de la columna automáticamente

   @param col. posición de la columna
   */
  void adjustColumn(int col) {
    NEW_TABLE
    obj_->adjustColumn(col);
  }

  /**
   Ajusta el alto de la fila automáticamente

   @param fil. posición de la fila
   */
  void adjustRow(int fil) {
    NEW_TABLE
    obj_->adjustRow(fil);
  }

  /**
   Establece el contenido de una celda como una imagen Pixmap

   @param  row Numero de la fila
   @param  col Numero de la columna
   @param  pix Imagen Pixmap
   */
  void setPixmap(int row, int col, const QPixmap &pix) {
    NEW_TABLE
    obj_->setPixmap(row, col, pix);
  }

  /**
   Obtiene la imagen Pixmap de una celda

   @param  row Numero de la fila
   @param  col Numero de la columna
   @return Imagen Pixmap
   */
  QPixmap pixmap(int row, int col) {
    NEW_TABLE
    return obj_->pixmap(row, col);
  }

  void emitCurrentChanged(int row, int col) {
    emit currentChanged(row, col);
  }

  void emitClicked(int row, int col, int, const QPoint &) {
    emit clicked(row, col);
  }

  void emitDoubleClicked(int row, int col, int, const QPoint &) {
    emit doubleClicked(row, col);
  }

  void emitValueChanged(int row, int col) {
    emit valueChanged(row, col);
  }

  void emitSelectionChanged() {
    emit selectionChanged();
  }

signals:

  /**
   Señal emitida al cambiar la casilla seleccionada de la tabla

   @param int. número de fila
   @param int. número columna
   */
  void currentChanged(int, int);

  /**
   Señal emitida al pulsar sobre una casilla determinada

   @param int. número de fila
   @param int. número columna
   */
  void clicked(int, int);

  /**
   Señal emitida al hacer dobel click en una determinada casilla

   @param int. número de fila
   @param int. número columna
   */
  void doubleClicked(int, int);

  /**
   Señal emitida al cambiarse el valor de una celda

   @param int. número de fila
   @param int. número columna
   */
  void valueChanged(int, int);

  /**
   Señal emitida al cambiarse la selección en la tabla
   */
  void selectionChanged();

protected:

  void connects() const;

  QTable *obj_;
};

class  FLTablePlusInterface : public FLTableInterface
{
  Q_OBJECT

public:

  /**
   Constructor
   */
  FLTablePlusInterface(QWidget *p, const QString &n) : FLTableInterface(p, n) {
  }

  /**
   Constructor
   */
  FLTablePlusInterface(FLTable *t) : FLTableInterface(::qt_cast<QTable *>(t)) {
  }

public slots:

  void setCellBackgroundColor(int row, int col, const QColor &color) {
    NEW_TABLE
    int d = row - obj_->numRows() + 1;
    if (d > 0)
      insertRows(obj_->numRows() - 1, d);
    d = col - obj_->numCols() + 1;
    if (d > 0)
      insertColumns(obj_->numCols() - 1, d);

    FLTable *t = ::qt_cast<FLTable *>(obj_);
    if (t)
      t->setCellBackgroundColor(row, col, color);
  }

  void setCellAlignment(int row, int col, const int a) {
    NEW_TABLE
    int d = row - obj_->numRows() + 1;
    if (d > 0)
      insertRows(obj_->numRows() - 1, d);
    d = col - obj_->numCols() + 1;
    if (d > 0)
      insertColumns(obj_->numCols() - 1, d);

    FLTable *t = ::qt_cast<FLTable *>(obj_);
    if (t)
      t->setCellAlignment(row, col, a);
  }
};

//! Acceso directo a vistas de tipo árbol
/**
 Acceso directo a vistas de tipo árbol
 Permite editar su estructura: añadiendo y quitando ramas

 @author InfoSiAL S.L.
 */
#define NEW_VIEW if (!obj_) { obj_ = new QListView(qApp->mainWidget()); connects(); }

class FLListViewItemInterface;

class FLListViewItem : public QListViewItem
{
public:

  FLListViewItem(QListView *parent): QListViewItem(parent) {}
  FLListViewItem(QListViewItem *parent): QListViewItem(parent) {}
  FLListViewItem(FLListViewItem *parent): QListViewItem(parent) {}
  void setKey(const QString &k) {
    key_ = k;
  }
  QString key() const {
    return key_;
  }

private:

  QString key_;
};

class  FLListViewInterface : public QObject
{
  Q_OBJECT

public:

  /**
   Constructor
   */
  FLListViewInterface(QWidget *p, const QString &n) : QObject(p) {
    obj_ = new QListView(p, n);
    if (obj_)
      connects();
  }

  /**
   Constructor
   */
  FLListViewInterface() : QObject(0), obj_(0) {}

  /**
   Constructor
   */
  FLListViewInterface(QListView *t) : QObject(t), obj_(t) {
    connects();
  }

public slots:

  /**
   Crea una nueva columna

   @param label. Título de la nueva columna
   @param width. Anchura de la nueva columna
   */
  int addColumn(const QString &label, int width = -1) {
    NEW_VIEW
    obj_->addColumn(label, width);
  }

  /**
   Establece el título de una columna

   @param column. Índice de la columna
   @param label. Título de la columna
   */
  void setColumnText(int column, const QString &label) {
    NEW_VIEW
    obj_->setColumnText(column, label);
  }

  /**
   Establece el la alineación de una columna

   @param column. Índice de la columna
   @param aling. Tipo de alineación
   */
  void setColumnAlignment(int column, int align) {
    NEW_VIEW
    obj_->setColumnAlignment(column, align);
  }

  QListView *obj() {
    NEW_VIEW
    return obj_;
  }

  void clear() {
    NEW_VIEW
    obj_->clear();
  }

  FLListViewItemInterface *firstChild() const;

  /**
  Uso interno
  */
  void emitDoubleClicked(QListViewItem *lvI);
  void emitExpanded(QListViewItem *lvI);
  void emitSelectionChanged(QListViewItem *lvI);

signals:

  /**
   Señal emitida al hacerse docble click sobre un elemento de la lista

   @param FLListViewItemInterface Elemento seleccionado
   */
  void doubleClicked(FLListViewItemInterface *);
  void expanded(FLListViewItemInterface *);
  void selectionChanged(FLListViewItemInterface *);

private:

  void connects() const;

  QListView *obj_;
};

//! Acceso directo a elementos de vistas de tipo árbol
/**
 Acceso directo a elementos de vistas de tipo árbol
 Permite añadir elementos a la vista de tipo árbol

 @author InfoSiAL S.L.
 */
class  FLListViewItemInterface : public QObject
{
  Q_OBJECT

public:

  /**
   Constructor
   */
  FLListViewItemInterface(FLListViewInterface *parent) : QObject(0), obj_(0) {
    if (parent)
      obj_ = new FLListViewItem(parent->obj());
  }

  /**
   Constructor
   */
  FLListViewItemInterface(FLListViewItemInterface *parent) : QObject(0), obj_(0) {
    if (parent)
      obj_ = new FLListViewItem(parent->obj());
  }

  /**
   Constructor
   */
  FLListViewItemInterface() : QObject(0), obj_(0) {}

  /**
   Constructor
   */
  FLListViewItemInterface(FLListViewItem *lvI) : QObject(0), obj_(0) {
    if (lvI)
      setObj(lvI);
  }

  /**
   Destuctor
   */
  ~FLListViewItemInterface() {}

public slots:

  /**
   Establece la clave asociada al elemento

   @param k. Clave
   */
  void setKey(const QString &k) {
    obj_->setKey(k);
  }

  /**
   Obtiene la clave del elemento

   @return Clave
   */
  QString key() const {
    return obj_->key();
  }

  /**
   Establece el texto del elemento

   @param column. Columna donde está el texto
   @param text. Texto
   */
  void setText(int column, const QString &text) {
    obj_->setText(column, text);
  }

  /**
   Establece el texto del elemento

   @param column. Columna donde está el texto
   @param text. Texto
   */
  QString text(int column) const {
    return obj_->text(column);
  }

  FLListViewItem *obj() {
    return obj_;
  }

  void setObj(FLListViewItem *lvI) {
    obj_ = lvI;
  }

  void setOpen(bool o) {
    obj_->setOpen(o);
  }

  FLListViewItemInterface *firstChild() const;

  FLListViewItemInterface *nextSibling() const;

  FLListViewItemInterface *parent() const;

  void del() {
    delete obj_;
  }

  void setExpandable(bool enable) {
    obj_->setExpandable(enable);
  }

  bool isExpandable() const {
    return obj_->isExpandable();
  }

  void setPixmap(int column, const QVariant &pm);

private:

  FLListViewItem *obj_;
};

//! Controles 'tab' (pestañas) de los formularios
/**
 Acceso a un control tipo 'tab' (pestañas) de un formulario. Permite establecer la pestaña activa.

 @author InfoSiAL S.L.
 */
class  FLTabWidgetInterface : public QObject
{
  Q_OBJECT

public:

  /**
   Constructor
   */
  FLTabWidgetInterface() {
    obj_ = new QTabWidget(qApp->mainWidget());
    connects();
  }

  /**
   Constructor
   */
  FLTabWidgetInterface(QTabWidget *t) : QObject(t), obj_(t) {
    if (obj_)
      connects();
  }

public slots:

  /**
   Establece el tab activado o desactivado

   @param index Número del indice del tab
   @param enable Si es TRUE el tab se mostrará habilitado, si es FALSE se pondrá inhabilitado
   */
  void setTabEnabled(int index, bool enable) {
    obj_->setTabEnabled(obj_->page(index), enable);
  }

  /**
   Establece el tab activado o desactivado

   @param name Nombre del tab
   @param enable Si es TRUE el tab se mostrará habilitado, si es FALSE se pondrá inhabilitado
   */
  void setTabEnabled(const QString &name, bool enable);

  /**
   Elimina un tab

   @param index Número del indice del tab
   */
  void removePage(int index) {
    obj_->removePage(obj_->page(index));
  }

  /**
  Elimina un tab

  @param name Nombre del tab
  */
  void removePage(const QString &name);

  /**
   Asegura que un tab está mostrado

   @param index Número del indice del tab
   */
  void showPage(int index) {
    obj_->showPage(obj_->page(index));
  }

  /**
   Asegura que un tab está mostrado

   @param name Nombre del tab
   */
  void showPage(const QString &name);

  /**
  Uso interno
  */
  void emitCurrentChanged(QWidget *w) {
    if (w)
      emit currentChanged(QString::fromLatin1(w->name()));
  }

signals:

  /**
   Señal emitida al cambiar el tab seleccionado
   @param name. Nombre del tab
   */
  void currentChanged(const QString &);

private:

  void connects() const;

  QTabWidget *obj_;
};

//! Acceso a Formularios
/**
 Acceso a los objetos formulario. Permite establecer el script,
 la ventana o el cursor asociado al formulario

 @author InfoSiAL S.L.
 */
class  FLFormDBInterface : public QObject
{
  Q_OBJECT

public:

  /**
   Constructor
   */
  FLFormDBInterface(const QString &a, QWidget *p, WFlags f = 0)
    : QObject(p), obj_(0), cursor_(0), script_(0) {
    setObj(new FLFormDB(a, p ? p : aqApp->mainWidget(), f));
  }

  /**
   Constructor
   */
  FLFormDBInterface(FLSqlCursorInterface *c, const QString &a, QWidget *p, WFlags f = 0)
    : QObject(p), obj_(0), cursor_(0), script_(0) {
    setObj(new FLFormDB(c->obj(), a, p ? p : aqApp->mainWidget(), f));
  }

  /**
   Constructor
   */
  FLFormDBInterface(FLFormDB *obj) : QObject(obj), obj_(0), cursor_(0), script_(0) {
    setObj(obj);
  }

  /**
   Constructor
   */
  FLFormDBInterface() : QObject(0), obj_(0), cursor_(0), script_(0) {}

  /**
   Destructor
   */
  ~FLFormDBInterface() {
  }

  /**
   Establece el objeto del formulario

   @param obj. objeto del formulario
   */
  void setObj(FLFormDB *obj) {
    obj_ = obj;
    if (obj_) {
      setName(obj_->name());
      cursor_ = FLSqlCursorInterface::sqlCursorInterface(obj_->cursor());
      connects();
    } else
      cursor_ = 0;
  }

  /**
   Establece el cursor del objeto relacionado con el formulario

   @param c. cursor del formulario
   */
  void setObjCursor(FLSqlCursor *c) {
    if (c && obj_)
      cursor_ = FLSqlCursorInterface::sqlCursorInterface(obj_->cursor());
  }

  /**
   Establece el script asociado al formulario

   @param s. Nombre del script
   */
  void setScript(QSScript *s) {
    script_ = s;
  }

  QSScript *script() {
    return script_;
  }

public slots:

  /**
   Establece el cursor relacionado con el formulario

   @param c. cursor del formulario
   */
  void setCursor(FLSqlCursorInterface *c) {
    if (c && obj_) {
      obj_->setCursor(c->obj());
      cursor_ = FLSqlCursorInterface::sqlCursorInterface(obj_->cursor());
    }
  }

  /**
   Devuelve el cursor del formulario

   @return Cursor del formulario
   */
  FLSqlCursorInterface *cursor() {
    if (!cursor_ && obj_)
      cursor_ = FLSqlCursorInterface::sqlCursorInterface(obj_->cursor());
    return (cursor_ ? cursor_ : new FLSqlCursorInterface());
  }

  /**
   Obtiene el widget principal del formulario.

   return Objeto QWidget que corresponde con el widget principal del formulario
   */
  QWidget *mainWidget() const {
    return (obj_ ? obj_->mainWidget() : 0);
  }

  /**
   Establece widget como principal del formulario.

   Este widget contendrá componentes que quieran enlazar con la
   base de datos, por lo que esperan estar contenidos en una clase
   FLFormDB, la cual les proporciona el cursor (registros) a los que enlazar.
   Si ya existiera otro widget como principal, este será borrado.

   Si existe un widget principal establecido con anterioridad será borrado

   @param w Widget principal para el formulario
   */
  void setMainWidget(QWidget *w) {
    if (obj_)
      obj_->setMainWidget(w);
  }

  /**
   Sobrecargado de setMainWidget.

   Aqui toma el nombre de un formulario de la acción asociada y construye el Widget principal, a partir de él.
   */
  void setMainWidget() {
    if (obj_)
      obj_->setMainWidget();
  }

  /**
   Sobrecargado de setMainWidget.

   Aqui construye el Widget principal a partir del nombre de un fichero de interfaz .ui.

   @param uiFileName Nombre del fichero de descripción de interfaz, incluyendo la extension .ui, p.e. clientes.ui
   */
  void setMainWidget(const QString &uiFileName) {
    if (obj_)
      obj_->setMainWidget(uiFileName);
  }

  /**
   Devuelve el objeto padre del formulario. Usado para obtener sus dimensiones.

   @return Objeto padre
   */
  QWidget *parentWidget() {
    return (obj_ ? obj_->parentWidget() : 0);
  }

  /**
   Devuelve un objeto hijo de formulario

   @param objName. Nombre del objeto hijo
   @return Objeto hijo
   */
  QObject *child(const QString &objName) {
    if (!obj_)
      return 0;
    QObject *o = obj_->child(objName, "QWidget");
    if (o) {
      if (::qt_cast<FLFieldDB *>(o)) {
        return new FLFieldDBInterface(static_cast<FLFieldDB *>(o));
      }
      if (::qt_cast<FLTableDB *>(o)) {
        return new FLTableDBInterface(static_cast<FLTableDB *>(o));
      }
      if (::qt_cast<FLTable *>(o)) {
        return new FLTablePlusInterface(static_cast<FLTable *>(o));
      }
      if (::qt_cast<QTable *>(o)) {
        return new FLTableInterface(static_cast<QTable *>(o));
      }
      if (::qt_cast<QTabWidget *>(o)) {
        return new FLTabWidgetInterface(static_cast<QTabWidget *>(o));
      }
      if (::qt_cast<QListView *>(o)) {
        return new FLListViewInterface(static_cast<QListView *>(o));
      }
    }
    return o;
  }

  /**
   Realiza las operaciones equivalentes a pulsar el botón cerrar

   @return TRUE si el formulario se cierra correctamente, FALSE si no es así
   */
  bool close() {
    if (obj_) {
      bool ret = obj_->close();
      obj_ = 0;
      return ret;
    } else
      return false;
  }

  FLFormDB *obj() {
    return obj_;
  }

  /**
   Inhabilita el componente

   @param b. TRUE si hay que inhabilitar el formulario, FALSE si hay que habilitarla
   */
  void setDisabled(const bool b) {
    if (obj_)
      obj_->setDisabled(b);
  }

  /**
   Obtiene la imagen o captura de pantalla del formulario.
   */
  QImage snapShot() {
    return (obj_ ? obj_->snapShot() : QImage());
  }

  /**
   Salva en un fichero con formato PNG la imagen o captura de pantalla del formulario.

   @param pathFile Ruta y nombre del fichero donde guardar la imagen
   */
  void saveSnapShot(const QString &pathFile) {
    if (obj_)
      obj_->saveSnapShot(pathFile);
  }

  /**
   Redefinida por conveniencia
   */
  void show() {
    if (obj_)
      obj_->show();
  }

  /**
   Muestra el formulario sin llamar al script "init".
   Utilizado en documentación para evitar conflictos al capturar los formularios
   */
  void showForDocument() {
    if (obj_)
      obj_->showForDocument();
  }

  /**
   Maximiza el formulario
   */
  void setMaximized() {
    if (obj_)
      obj_->setMaximized();
  }

  /**
  Establece el título de la ventana.

  @param text Texto a establecer como título de la ventana
  @author Silix
  */
  void setCaptionWidget(const QString &text) {
    obj_->setCaptionWidget(text);
  }

signals:

  /**
   Señal emitida cuando se cierra el formulario
   */
  void closed();

  /**
   Señal emitida cuando el formulario ya ha sido inicializado y está listo para usarse
   */
  void formReady();

private:

  void connects() const;

  FLFormDB *obj_;
  FLSqlCursorInterface *cursor_;
  QSScript *script_;
};

//! Acceso a Formularios de edición
/**
 Acceso a formularios de edición. Permite establecer el script,
 la ventana o el cursor asociado al formulario

 @author InfoSiAL S.L.
 */
class  FLFormRecordDBInterface : public QObject
{
  Q_OBJECT

public:

  /**
   Constructor
   */
  FLFormRecordDBInterface(FLSqlCursorInterface *c, const QString &a, QWidget *p, bool sA) : QObject(p), obj_(0),
    cursor_(0), script_(0) {
    setObj(new FLFormRecordDB(c->obj(), a, aqApp->mainWidget(), sA));
  }

  /**
   Constructor
   */
  FLFormRecordDBInterface(FLFormRecordDB *obj) : QObject(obj), obj_(0), cursor_(0), script_(0) {
    setObj(obj);
  }

  /**
   Constructor
   */
  FLFormRecordDBInterface() : QObject(0), obj_(0), cursor_(0), script_(0) {}

  /**
   Destructor
   */
  ~FLFormRecordDBInterface() {}

  void finish() {}

  /**
   Establece el objeto de formRecord

   @param obj. Objeto FLFormRecordDB
   */
  void setObj(FLFormRecordDB *obj) {
    obj_ = obj;
    if (obj_) {
      setName(obj_->name());
      cursor_ = FLSqlCursorInterface::sqlCursorInterface(obj_->cursor());
      connects();
    } else
      cursor_ = 0;
  }

  /**
   Establece el cursor del objeto relacionado con el formulario

   @param c. cursor del formulario
   */
  void setObjCursor(FLSqlCursor *c) {
    if (c && obj_)
      cursor_ = FLSqlCursorInterface::sqlCursorInterface(obj_->cursor());
  }

  /**
   Establece el script del formulario

   @param s. Script del formulario
   */
  void setScript(QSScript *s) {
    script_ = s;
  }

  /**
   Devuelve el script del formulario

   @return  el script asociado al formulario, en el caso de que lo tenga
   */
  QSScript *script() {
    return script_;
  }

public slots:

  /**
   Establece un cursor asociado al formulario

   @param c. Objeto cursor
   */
  void setCursor(FLSqlCursorInterface *c) {
    if (c) {
      if (obj_) {
        obj_->setCursor(c->obj());
        cursor_ = FLSqlCursorInterface::sqlCursorInterface(obj_->cursor());
      } else
        cursor_ = c;
    }
  }

  /**
   Devuelve el cursor asocido al formulario

   @return Objeto cursor
   */
  FLSqlCursorInterface *cursor() {
    if (!cursor_ && obj_)
      cursor_ = FLSqlCursorInterface::sqlCursorInterface(obj_->cursor());
    return (cursor_ ? cursor_ : new FLSqlCursorInterface());
  }

  /**
   Obtiene el widget principal del formulario.

   return Objeto QWidget que corresponde con el widget principal del formulario
   */
  QWidget *mainWidget() const {
    return (obj_ ? obj_->mainWidget() : 0);
  }

  /**
   Establece widget como principal del formulario.

   Este widget contendrá componentes que quieran enlazar con la
   base de datos, por lo que esperan estar contenidos en una clase
   FLFormDB, la cual les proporciona el cursor (registros) a los que enlazar.
   Si ya existiera otro widget como principal, este será borrado.

   Si existe un widget principal establecido con anterioridad será borrado

   @param w Widget principal para el formulario
   */
  void setMainWidget(QWidget *w) {
    if (obj_)
      obj_->setMainWidget(w);
  }

  /**
   Sobrecargado de setMainWidget.

   Aqui toma el nombre de un formulario de la acción asociada y construye el Widget principal, a partir de él.
   */
  void setMainWidget() {
    if (obj_)
      obj_->setMainWidget();
  }

  /**
   Sobrecargado de setMainWidget.

   Aqui construye el Widget principal a partir del nombre de un fichero de interfaz .ui.

   @param uiFileName Nombre del fichero de descripción de interfaz, incluyendo la extension .ui, p.e. clientes.ui
   */
  void setMainWidget(const QString &uiFileName) {
    if (obj_)
      ::qt_cast<FLFormDB *>(obj_)->setMainWidget(uiFileName);
  }

  /**
   Devuelve un objeto hijo de formulario

   @param objName. Nombre del objeto hijo
   @return Objeto hijo
   */
  QObject *child(const QString &objName) {
    if (!obj_)
      return 0;
    QObject *o = obj_->child(objName, "QWidget");
    if (o) {
      if (::qt_cast<FLFieldDB *>(o)) {
        return new FLFieldDBInterface(static_cast<FLFieldDB *>(o));
      }
      if (::qt_cast<FLTableDB *>(o)) {
        return new FLTableDBInterface(static_cast<FLTableDB *>(o));
      }
      if (::qt_cast<FLTable *>(o)) {
        return new FLTablePlusInterface(static_cast<FLTable *>(o));
      }
      if (::qt_cast<QTable *>(o)) {
        return new FLTableInterface(static_cast<QTable *>(o));
      }
      if (::qt_cast<QTabWidget *>(o)) {
        return new FLTabWidgetInterface(static_cast<QTabWidget *>(o));
      }
      if (::qt_cast<QListView *>(o)) {
        return new FLListViewInterface(static_cast<QListView *>(o));
      }
    }
    return o;
  }

  /**
   Realiza las operaciones equivalentes a pulsar el botón cerrar

   @return TRUE si el formulario se cierra correctamente, FALSE si no es así
   */
  bool close() {
    if (obj_) {
      bool ret = obj_->close();
      obj_ = 0;
      return ret;
    } else
      return false;
  }

  /**
   Realiza las operaciones equivalentes a pulsar el boton aceptar
   */
  void accept() {
    if (obj_)
      obj_->accept();
  }

  /**
   Realiza las operaciones equivalentes a pulsar el botón cancelar
   */
  void reject() {
    if (obj_)
      obj_->reject();
  }

  /**
   Se activa al pulsar el boton aceptar y contninuar
   */
  void acceptContinue() {
    if (obj_)
      obj_->acceptContinue();
  }

  FLFormRecordDB *obj() {
    return obj_;
  }

  /**
   Inhabilita el formulario

   @param b. TRUE si hay que inhabilitar formulario, FALSE si hay que habilitarla
   */
  void setDisabled(const bool b) {
    if (obj_)
      obj_->setDisabled(b);
  }

  /**
   Obtiene la imagen o captura de pantalla del formulario.
   */
  QImage snapShot() {
    return (obj_ ? obj_->snapShot() : QImage());
  }

  /**
   Salva en un fichero con formato PNG la imagen o captura de pantalla del formulario.

   @param pathFile Ruta y nombre del fichero donde guardar la imagen
   */
  void saveSnapShot(const QString &pathFile) {
    if (obj_)
      obj_->saveSnapShot(pathFile);
  }

  /**
   Redefinida por conveniencia
   */
  void show() {
    if (obj_)
      obj_->show();
  }

  /**
   Muestra el formulario sin llamar al script "init".
   Utilizado en documentación para evitar conflictos al capturar los formularios
   */
  void showForDocument() {
    if (obj_)
      obj_->showForDocument();
  }

  /**
   Maximiza el formulario
   */
  void setMaximized() {
    if (obj_)
      obj_->setMaximized();
  }

  /**
  Establece el título de la ventana.

  @param text Texto a establecer como título de la ventana
  @author Silix
  */
  void setCaptionWidget(const QString &text) {
    obj_->setCaptionWidget(text);
  }

signals:

  /**
   Señal emitida cuando se cierra el formulario
   */
  void closed();

  /**
   Señal emitida cuando el formulario ya ha sido inicializado y está listo para usarse
   */
  void formReady();

private:

  void connects() const;

  FLFormRecordDB *obj_;
  FLSqlCursorInterface *cursor_;
  QSScript *script_;
};

//! Acceso a Formularios de búsqueda
/**
 Acceso a formularios de búsqueda. Permite establecer el script,
 la ventana o el cursor asociado al formulario

 @author InfoSiAL S.L.
 */
class  FLFormSearchDBInterface : public QObject
{
  Q_OBJECT

public:

  /**
   Constructor
   */
  FLFormSearchDBInterface(const QString &a) : QObject(0), obj_(0), cursor_(0),
    script_(0) {
    setObj(new FLFormSearchDB(a));
  }

  /**
   Constructor
   */
  FLFormSearchDBInterface(FLSqlCursorInterface *c, const QString &a) : QObject(0), obj_(0), cursor_(0),
    script_(0) {
    setObj(new FLFormSearchDB(c->obj(), a));
  }

  /**
   Constructor
   */
  FLFormSearchDBInterface(FLFormSearchDB *obj) : QObject(obj), obj_(0), cursor_(0), script_(0) {
    setObj(obj);
  }

  /**
   Constructor
   */
  FLFormSearchDBInterface() : QObject(0), obj_(0), cursor_(0), script_(0) {
  }

  /**
   Destructor
   */
  ~FLFormSearchDBInterface() {
    close();
  }

  void finish() {}

  /**
   Establece el objeto FLFormSearchDB asociado

   @param obj. objeto a establecer
   */
  void setObj(FLFormSearchDB *obj) {
    obj_ = obj;
    if (obj_) {
      setName(obj_->name());
      cursor_ = FLSqlCursorInterface::sqlCursorInterface(obj_->cursor());
      connects();
    } else
      cursor_ = 0;
  }

  /**
   Establece el cursor del objeto relacionado con el formulario

   @param c. cursor del formulario
   */
  void setObjCursor(FLSqlCursor *c) {
    if (c && obj_)
      cursor_ = FLSqlCursorInterface::sqlCursorInterface(obj_->cursor());
  }

  /**
   Establece un script asociado al formulario

   @param s. Objeto script
   */
  void setScript(QSScript *s) {
    script_ = s;
  }

  /**
   Devuelve el script del formulario

   @return  es script del formulario, en caso de que tenga alguno asociado
   */
  QSScript *script() {
    return script_;
  }

public slots:

  /**
   Establece un cursor para el objeto
   */
  void setCursor(FLSqlCursorInterface *c) {
    if (c && obj_) {
      obj_->setCursor(c->obj());
      cursor_ = FLSqlCursorInterface::sqlCursorInterface(obj_->cursor());
    }
  }

  /**
   Devuelve el cursor asociado al formulario

   @return Objeto cursor
   */
  FLSqlCursorInterface *cursor() {
    if (!cursor_ && obj_)
      cursor_ = FLSqlCursorInterface::sqlCursorInterface(obj_->cursor());
    return (cursor_ ? cursor_ : new FLSqlCursorInterface());
  }

  /**
   Obtiene el widget principal del formulario.

   return Objeto QWidget que corresponde con el widget principal del formulario
   */
  QWidget *mainWidget() const {
    return (obj_ ? obj_->mainWidget() : 0);
  }

  /**
   Establece widget como principal del formulario.

   Este widget contendrá componentes que quieran enlazar con la
   base de datos, por lo que esperan estar contenidos en una clase
   FLFormDB, la cual les proporciona el cursor (registros) a los que enlazar.
   Si ya existiera otro widget como principal, este será borrado.

   Si existe un widget principal establecido con anterioridad será borrado

   @param w Widget principal para el formulario
   */
  void setMainWidget(QWidget *w) {
    if (obj_)
      obj_->setMainWidget(w);
  }

  /**
   Sobrecargado de setMainWidget.

   Aqui toma el nombre de un formulario de la acción asociada y construye el Widget principal, a partir de él.
   */
  void setMainWidget() {
    if (obj_)
      obj_->setMainWidget();
  }

  /**
   Sobrecargado de setMainWidget.

   Aqui construye el Widget principal a partir del nombre de un fichero de interfaz .ui.

   @param uiFileName Nombre del fichero de descripción de interfaz, incluyendo la extension .ui, p.e. clientes.ui
   */
  void setMainWidget(const QString &uiFileName) {
    if (obj_)
      ::qt_cast<FLFormDB *>(obj_)->setMainWidget(uiFileName);
  }

  /**
   Muestra el formulario y entra en un nuevo bucle de eventos
   para esperar, a seleccionar registro.

   Se espera el nombre de un campo del cursor
   devolviendo el valor de dicho campo si se acepta el formulario
   y un QVariant::Invalid si se cancela.

   @param n Nombre del un campo del cursor del formulario
   @return El valor del campo si se acepta, o QVariant::Invalid si se cancela
   */
  QVariant exec(const QString &n = QString::null) {
    return (obj_ ? obj_->exec(n) : QVariant());
  }
  void show() {
    if (obj_)
      obj_->exec();
  }

  /**
   Devuelve si se ha aceptado el formulario

   @return devuelve TRUE si el formulario ha sido aceptado y FALSE si no se ha aceptado
   */
  bool accepted() {
    return (obj_ ? obj_->accepted() : false);
  }

  /**
   Devuelve un objeto hijo de formulario

   @param objName. Nombre del objeto hijo
   @return Objeto hijo
   */
  QObject *child(const QString &objName) {
    if (!obj_)
      return 0;
    QObject *o = obj_->child(objName, "QWidget");
    if (o) {
      if (::qt_cast<FLFieldDB *>(o)) {
        return new FLFieldDBInterface(static_cast<FLFieldDB *>(o));
      }
      if (::qt_cast<FLTableDB *>(o)) {
        return new FLTableDBInterface(static_cast<FLTableDB *>(o));
      }
      if (::qt_cast<FLTable *>(o)) {
        return new FLTablePlusInterface(static_cast<FLTable *>(o));
      }
      if (::qt_cast<QTable *>(o)) {
        return new FLTableInterface(static_cast<QTable *>(o));
      }
      if (::qt_cast<QTabWidget *>(o)) {
        return new FLTabWidgetInterface(static_cast<QTabWidget *>(o));
      }
      if (::qt_cast<QListView *>(o)) {
        return new FLListViewInterface(static_cast<QListView *>(o));
      }
    }
    return o;
  }

  /**
   Realiza las operaciones equivalentes a pulsar el botón cerrar

   @return TRUE si el formulario se cierra correctamente, FALSE si no es así
   */
  bool close() {
    if (obj_) {
      bool ret = obj_->close();
      obj_ = 0;
      return ret;
    } else
      return false;
  }

  /**
   Realiza las operaciones equivalentes a pulsar el boton aceptar
   */
  void accept() {
    if (obj_)
      obj_->accept();
  }

  /**
   Realiza las operaciones equivalentes a pulsar el botón cancelar
   */
  void reject() {
    if (obj_)
      obj_->reject();
  }

  FLFormSearchDB *obj() {
    return obj_;
  }

  /**
   Inhabilita el formulario

   @param b. TRUE si hay que inhabilitar el formulario, FALSE si hay que habilitarla
   */
  void setDisabled(const bool b) {
    if (obj_)
      obj_->setDisabled(b);
  }

  /**
   Obtiene la imagen o captura de pantalla del formulario.
   */
  QImage snapShot() {
    return (obj_ ? obj_->snapShot() : QImage());
  }

  /**
   Salva en un fichero con formato PNG la imagen o captura de pantalla del formulario.

   @param pathFile Ruta y nombre del fichero donde guardar la imagen
   */
  void saveSnapShot(const QString &pathFile) {
    saveSnapShot(pathFile);
  }

  /**
   Maximiza el formulario
   */
  void setMaximized() {
    if (obj_)
      obj_->setMaximized();
  }

  /**
  Establece el título de la ventana.

  @param text Texto a establecer como título de la ventana
  @author Silix
  */
  void setCaptionWidget(const QString &text) {
    obj_->setCaptionWidget(text);
  }

signals:

  /**
   Señal emitida cuando se cierra el formulario
   */
  void closed();

  /**
   Señal emitida cuando el formulario ya ha sido inicializado y está listo para usarse
   */
  void formReady();

private:

  void connects() const;

  FLFormSearchDB *obj_;
  FLSqlCursorInterface *cursor_;
  QSScript *script_;
};

//! Control de las variables de sesión
/**
 Control de las variables de sesión. Permite leer o modificar estas variables

 @author InfoSiAL S.L.
 */
class  FLVarInterface : public QObject
{

  Q_OBJECT

public:

  /**
   Constructor
   */
  FLVarInterface() {}

  /**
   Destructor
   */
  ~FLVarInterface() {}

public slots:

  /**
   Establece un valor en la tabla de variables de sesión

   @param n Nombre de la variable
   @param v Valor asociado
   @return Verdadero en caso de establecer el valor con éxito, falso en cualquier otro caso
   */
  bool set(const QString &n, const QVariant &v) {
    return FLVar::set(n, v);
  }

  /**
   Lee un valor en la tabla de variables de sesión

   @param n Nombre de la variable a leer
   @return Valor asociado, en caso de leer el valor con éxito, falso en cualquier otro caso
   */
  QVariant get(const QString &n) {
    return FLVar::get(n);
  }
};

//! Conjunto de utilidades generales
/**
 Conjunto de utilidades generales
 (conversión de datos, traducciones, incremento de contadores...)

 @author InfoSiAL S.L.
 */
class  FLUtilInterface : public QObject
{
  Q_OBJECT
  Q_ENUMS(specialType)
  Q_ENUMS(Type)

public:

  enum specialType {
    Serial = 100,
    Unlock = 200,
    Check = 300
  };

  enum Type {
    Invalid,
    Map,
    List,
    String,
    StringList,
    Font,
    Pixmap,
    Brush,
    Rect,
    Size,
    Color,
    Palette,
    ColorGroup,
    IconSet,
    Point,
    Image,
    Int,
    UInt,
    Bool,
    Double,
    CString,
    PointArray,
    Region,
    Bitmap,
    Cursor,
    SizePolicy,
    Date,
    Time,
    DateTime,
    ByteArray,
    BitArray,
    KeySequence,
    Pen,
    LongLong,
    ULongLong
  };

  /**
   Constructor
   */
  FLUtilInterface(QObject *parent = 0) : QObject(parent) {
    QObject::setName("FLUtil" + QDateTime::currentDateTime().toString("ddMMyyyyhhmmsszzz"));
  }

public slots:
  /**
   Obtiene la parte entera de un número.

   Dado un número devuelve la parte entera correspondiente, es decir,
   cifras en la parte izquierda de la coma decimal.

   @param n Número del que obtener la parte entera. Debe ser positivo
   @return La parte entera del número, que puede ser cero
   */
  QVariant partInteger(double n) {
    return QVariant((Q_ULLONG) FLUtil::partInteger(n));
  }

  /**
   Obtiene la parte decimal de un número.

   Dado un número devuelve la parte decimal correspondiente, es decir,
   cifras en la parte derecha de la coma decimal.

   @param n Número del que obtener la parte decimal. Debe ser positivo
   @return La parte decimal del número, que puede ser cero
   */
  QVariant partDecimal(double n) {
    return QVariant((Q_ULLONG) FLUtil::partDecimal(n));
  }

  /**
   Construye un string a partir de un número, especificando el formato y precisión

   @param v. Número a convertir a QString
   @param tipo. Formato del número
   @param partDecimal. Precisión (número de cifras decimales) del número

   @return Cadena que contiene el número formateado
   */
  QString buildNumber(const QVariant &v, char tipo, int partDecimal) {
    return FLUtil::buildNumber(v, tipo, partDecimal);
  }
  QString buildNumber(double v, char tipo, int partDecimal) {
    return buildNumber(QVariant(v), tipo, partDecimal);
  }
  QString buildNumber(int v, char tipo, int partDecimal) {
    return buildNumber(QVariant(v), tipo, partDecimal);
  }

  /**
   Lee el valor de un setting en el directorio de la instalación de AbanQ

   @param key. Clave identificadora del setting
   @param def. Valor por defecto en el caso de que el setting no esté establecido
   @param ok. Indicador de que la lectura es correcta

   @return Valor del setting
   */
  QVariant readSettingEntry(const QString &key, const QString &def = QString::null, bool *ok = 0) {
    return FLUtil::readSettingEntry(key, def, ok);
  }

  /**
   Establece el valor de un setting en el directorio de instalación de AbanQ

   @param key. Clave identificadora del setting
   @param Valor del setting

   @return Indicador de si la escritura del settings se realiza correctamente
   */
  bool writeSettingEntry(const QString &key, const QString &value) {
    return FLUtil::writeSettingEntry(key, value);
  }

  /**
   Lee el valor de un setting en la tabla flsettings

   @param key. Clave identificadora del setting

   @return Valor del setting
   */
  QVariant readDBSettingEntry(const QString &key) {
    return FLUtil::readDBSettingEntry(key);
  }

  /**
   Establece el valor de un setting en la tabla flsettings

   @param key. Clave identificadora del setting
   @param Valor del setting

   @return Indicador de si la escritura del settings se realiza correctamente
   */
  bool writeDBSettingEntry(const QString &key, const QString &value) {
    return FLUtil::writeDBSettingEntry(key, value);
  }

  /**
   Redondea un valor en función de la precisión especificada para un campo tipo double de la base de datos

   @param n. Número a redondear
   @param table. Nombre de la tabla
   @param field. Nombre del campo

   @return Número redondeado
   */
  QString roundFieldValue(const QVariant &n, const QString &table, const QString &field) {
    return FLUtil::roundFieldValue(n, table, field);
  }
  QString roundFieldValue(double n, const QString &table, const QString &field) {
    return roundFieldValue(QVariant(n), table, field);
  }
  QString roundFieldValue(int n, const QString &table, const QString &field) {
    return roundFieldValue(QVariant(n), table, field);
  }

  /**
   Enunciado de las unidades de un número.

   @param n Número a tratar. Debe ser positivo
   */
  QString unidades(long n) {
    return FLUtil::unidades(n);
  }

  /**
   Enunciado de las centenas de millar de un número.

   @param n Número a tratar. Debe ser positivo
   */
  QString centenamillar(long n) {
    return FLUtil::centenamillar(n);
  }

  /**
   Enunciado de las decenas de un número.

   @param n Número a tratar. Debe ser positivo
   */
  QString decenas(long n) {
    return FLUtil::decenas(n);
  }

  /**
   Enunciado de las centenas de un número.

   @param n Número a tratar. Debe ser positivo
   */
  QString centenas(long n) {
    return FLUtil::centenas(n);
  }

  /**
   Enunciado de las unidades de millar de un número.

   @param n Número a tratar. Debe ser positivo
   */
  QString unidadesmillar(long n) {
    return FLUtil::unidadesmillar(n);
  }

  /**
   Pasa una cadena a codificación utf-8

   @param s: Cadena
   @return Cadena en formato UTF-8
   */
  QCString utf8(const QString &s) {
    return FLUtil::utf8(s);
  }

  /**
   Enunciado de las decenas de millar de un número.

   @param n Número a tratar. Debe ser positivo
   */
  QString decenasmillar(long n) {
    return FLUtil::decenasmillar(n);
  }

  /**
   Obtiene la expresión en texto de como se enuncia un número, en castellano.

   Dado un número entero, devuelve su expresión en texto de como se
   enuncia de forma hablada; por ejemplo dado el número 130,
   devolverá la cadena de texto "ciento treinta".

   @param n Número a transladar a su forma hablada. Debe ser positivo
   @return Cadena de texto con su expresión hablada
   */
  QString enLetra(long n) {
    return FLUtil::enLetra(n);
  }

  /**
   Obtiene la expresión en texto de como se enuncia una cantidad monetaria, en castellano
   y en cualquier moneda indicada.

   Dado un número doble, devuelve su expresión en texto de como se
   enuncia de forma hablada en la moneda indicada; por ejemplo dado el número 130.25,
   devolverá la cadena de texto "ciento treinta 'moneda' con veinticinco céntimos".

   @param n Número a transladar a su forma hablada. Debe ser positivo
   @param m Nombre de la moneda
   @return Cadena de texto con su expresión hablada
   */
  QString enLetraMoneda(double n, const QString &m) {
    return FLUtil::enLetraMoneda(n, m);
  }

  /**
   Obtiene la expresión en texto de como se enuncia una cantidad monetaria, en castellano
   y en Euros.

   Dado un número doble, devuelve su expresión en texto de como se
   enuncia de forma hablada en euros; por ejemplo dado el número 130.25,
   devolverá la cadena de texto "ciento treinta euros con veinticinco céntimos".

   @param n Número a transladar a su forma hablada. Debe ser positivo
   @return Cadena de texto con su expresión hablada
   */
  QString enLetraMonedaEuro(double n) {
    return FLUtil::enLetraMonedaEuro(n);
  }

  /**
   Calcula la letra asociada a un número de DNI

   @param n Número de DNI
   @return Letra asociada
   **/
  QString letraDni(long n) {
    return FLUtil::letraDni(n);
  }

  /**
   Calcula el dígito de control asociado a un número de cuenta bancaria

   @param n Número de cuenta
   @return Dígito de control
   */
  QString calcularDC(const QString &n) {
    return FLUtil::calcularDC(n);
  }

  /**
   Convierte fechas del tipo DD-MM-AAAA, DD/MM/AAAA o
   DDMMAAAA al tipo AAAA-MM-DD.

   @param  f Cadena de texto con la fecha a transformar
   @return Cadena de texto con la fecha transformada
   */
  QString dateDMAtoAMD(const QString &f) {
    return FLUtil::dateDMAtoAMD(f);
  }

  /**
   Convierte fechas del tipo AAAA-MM-DD, AAAA-MM-DD o
   AAAAMMDD al tipo DD-MM-AAAA.

   @param  f Cadena de texto con la fecha a transformar
   @return Cadena de texto con la fecha transformada
   */
  QString dateAMDtoDMA(const QString &f) {
    return FLUtil::dateAMDtoDMA(f);
  }

  /**
   Formatea una cadena de texto poniéndole separadores de miles.

   La cadena que se pasa se supone que un número, convirtiendola
   con QString::toDouble(), si la cadena no es número el resultado es imprevisible.

   @param s Cadena de texto a la que se le quieren poder separadores de miles
   @return Devuelve la cadena formateada con los separadores de miles
   */
  QString formatoMiles(const QString &s) {
    return FLUtil::formatoMiles(s);
  }

  /**
   Traducción de una cadena al idioma local

   Se hace una llamada a la función tr() de la clase QObject para hacer la traducción.
   Se utiliza para traducciones desde fuera de objetos QObject

   @param contexto Contexto en el que se encuentra la cadena, generalmente se refiere a la clase en la que está definida
   @param s Cadena de texto a traducir
   @return Devuelve la cadena traducida al idioma local
   */
  QString translate(const QString &contexto, const QString &s) {
    return FLUtil::translate(contexto, s);
  }

  /**
   Devuelve si el numero de tarjeta de Credito es valido.

   El parametro que se pasa es la cadena de texto que contiene el numero de tarjeta.

   @param num Cadena de texto con el numero de tarjeta
   @return Devuelve verdadero si el numero de tarjeta es valido
   */
  bool numCreditCard(const QString &num) {
    return FLUtil::numCreditCard(num);
  }

  /**
   Este método devuelve el siguiente valor de un campo tipo contador de una tabla.

   Este método es muy util cuando se insertan registros en los que
   la referencia es secuencial y no nos acordamos de cual fue el ultimo
   numero usado. El valor devuelto es un QVariant del tipo de campo es
   el que se busca la ultima referencia. Lo más aconsejable es que el tipo
   del campo sea 'String' porque así se le puede dar formato y ser
   usado para generar un código de barras. De todas formas la función
   soporta tanto que el campo sea de tipo 'String' como de tipo 'double'.

   @param name Nombre del campo
   @param cursor_ Cursor a la tabla donde se encuentra el campo.
   @return Qvariant con el numero siguiente.
   @author Andrés Otón Urbano.
   */
  QVariant nextCounter(const QString &name, FLSqlCursorInterface *cursor_) {
    return FLUtil::nextCounter(name, cursor_->obj());
  }
	
  /**
  dpinelo: Este mÃ©todo es una extensiÃ³n de nextCounter pero permitiendo la introducciÃ³n de una primera
  secuencia de caracteres. Es Ãºtil cuando queremos mantener diversos contadores dentro de una misma tabla.
  Ejemplo, Tabla Grupo de clientes: Agregamos un campo prefijo, que serÃ¡ una letra: A, B, C, D.
  Queremos que la numeraciÃ³n de los clientes sea del tipo A00001, o B000023. Con esta funciÃ³n, podremos
  seguir usando los mÃ©todos counter cuando agregamos esa letra.
  
  Este metodo devuelve el siguiente valor de un campo tipo contador de una tabla para una serie determinada.

  Este metodo es muy util cuando se insertan registros en los que
  la referencia es secuencial segÃºn una secuencia y no nos acordamos de cual fue el Ãºltimo
  numero usado. El valor devuelto es un QVariant del tipo de campo es
  el que se busca la ultima referencia. Lo mÃ¡s aconsejable es que el tipo
  del campo sea 'String' porque asÃ­ se le puede dar formato y ser
  usado para generar un cÃ³digo de barras. De todas formas la funciÃ³n
  soporta tanto que el campo sea de tipo 'String' como de tipo 'double'.

  @param serie serie que diferencia los contadores
  @param name Nombre del campo
  @param cursor_ Cursor a la tabla donde se encuentra el campo.
  @return Qvariant con el numero siguiente.
  @author AndrÃ©s OtÃ³n Urbano.
   */
  QVariant nextCounterSerial( const QString &serie, const QString & name, FLSqlCursorInterface * cursor_ ) {
	  return FLUtil::nextCounter( serie, name, cursor_->obj() );
  }

  /**
   Devuelve el siguiente valor de la secuencia segun la profundidad indicada por nivel.
   Para explicar el funcionamiento pondremos un ejemplo. Supongamos una secuencia tipo %A-%N.
   %A indica que se coloque en esa posicion una secuencia en letras y %N una secuencia en numero.
   La numeración de niveles va de derecha a izquierda asi el nivel 1 es %N y el nivel 2 %A.
   Si hacemos un nextSequence a nivel 1 el valor de vuelto será un %A que estubiera y un %N sumando 1
   al anterior. Si el nivel es 2 obtendremos un %A + 1, trasformado a letras, y todos los niveles a
   la derecha de este se ponen a 1 o su correspondiente en letra que seria A.

   @param nivel Indica la profundidad a la que se hace el incremento.
   @param secuencia Estructura de la secuencia.
   @param ultimo Ultimo valor de la secuencia para poder dar el siguiente valor.
   @return La secuencia en el formato facilitado.
   @author Andrés Otón Urbano
   */
  QString nextSequence(int nivel, const QString &secuencia, const QString &ultimo) {
    return FLUtil::nextSequence(nivel, secuencia, ultimo);
  }

  /**
   Convierte una cadena que es una serie de letras en su correspondiente valor numerico.

   @param letter Cadena con la serie.
   @return Una cadena pero que contiene numeros.
   */
  QString serialLettertoNumber(const QString &letter) {
    return FLUtil::serialLettertoNumber(letter);
  }

  /**
   Convierte un número a su correspondiente secuencia de Letras.

   @param number Número a convertir
   */
  QString serialNumbertoLetter(int number) {
    return FLUtil::serialNumbertoLetter(number);
  }

  /**
   Comprueba si la cabecera de un fichero de definición corresponde
   con las soportadas por AbanQ.

   Este método no sirve para los scripts, sólo para los ficheros de definición;
   mtd, ui, qry, xml, ts y kut.

   @param head Cadena de caracteres con la cabecera del fichero, bastaría
   con las tres o cuatro primeras linea del fichero no vacías
   @return TRUE si es un fichero soportado, FALSE en caso contrario
   */
  bool isFLDefFile(const QString &head) {
    return FLUtil::isFLDefFile(head);
  }

  /**
   Devuelve la longitud de una cadena

   @param srt. Cadena
   @return Longitud
   */
  uint length(const QString &str) {
    return str.length();
  }

  /**
   Devuelve una lista de los nombres de los campos de una tabla, separados por comas

   @param tabla. Nombre de la tabla
   @return  Lista de todos los nombres de los campos de esa tabla
   */
  QStringList nombreCampos(const QString &tabla) {
    return FLUtil::nombreCampos(tabla);
  }

  /**
   Obtiene el alias de un campo a partir de su nombre.

   @param fN Nombre del campo
   @param tN Nombre de la tabla
   @param connName Nombre de la conexion
   */
  QString fieldNameToAlias(const QString &fN, const QString &tN, const QString &connName = "default") const;

  /**
   Obtiene el alias de una tabla a partir de su nombre.

   @param tN Nombre de la tabla
   @param connName Nombre de la conexion
   */
  QString tableNameToAlias(const QString &tN, const QString &connName = "default") const;

  /**
   Obtiene el nombre de un campo a partir de su alias.

   @param aN Nombre del alias del campo
   @param tN Nombre de la tabla
   @param connName Nombre de la conexion
   */
  QString fieldAliasToName(const QString &aN, const QString &tN, const QString &connName = "default") const;

  /**
   Obtiene el tipo de un campo a partir de su nombre.

   @param fN Nombre del campo
   @param tN Nombre de la tabla
   @param connName Nombre de la conexion
   */
  int fieldType(const QString &fN, const QString &tN, const QString &connName = "default") const;

  /**
   Obtiene la longitud de un campo a partir de su nombre.

   @param fN Nombre del campo
   @param tN Nombre de la tabla
   @param connName Nombre de la conexion
   */
  int fieldLength(const QString &fN, const QString &tN, const QString &connName = "default") const;

  /**
   Obtiene si el campo permite nulos.

   @param fN Nombre del campo
   @param tN Nombre de la tabla
   @param connName Nombre de la conexion
   */
  bool fieldAllowNull(const QString &fN, const QString &tN, const QString &connName = "default") const;

  /**
   Obtiene si el campo es clave primaria.

   @param fN Nombre del campo
   @param tN Nombre de la tabla
   @param connName Nombre de la conexion
   */
  bool fieldIsPrimaryKey(const QString &fN, const QString &tN, const QString &connName = "default") const;

  /**
   Obtiene si el campo es clave compuesta.

   @param fN Nombre del campo
   @param tN Nombre de la tabla
   @param connName Nombre de la conexion
   */
  bool fieldIsCompoundKey(const QString &fN, const QString &tN, const QString &connName = "default") const;

  /**
   Devuelve el valor por defecto para el campo.

   @param fN Nombre del campo
   @param tN Nombre de la tabla
   @param connName Nombre de la conexion
   */
  QVariant fieldDefaultValue(const QString &fN, const QString &tN, const QString &connName = "default") const;

  /**
   Devuelve el contenido del valor de de un campo formateado para ser reconocido
   por la base de datos actual, dentro de la clausura WHERE de SQL.

   Este método toma como parametros los metadatos del campo definidos con
   FLFieldMetaData. Además de TRUE y FALSE como posibles valores de un campo
   lógico también acepta los valores Sí y No (o su traducción al idioma correspondiente).
   Las fechas son adaptadas al forma AAAA-MM-DD, que es el formato reconocido por PostgreSQL .

   @param t Tipo de datos del valor
   @param v Valor que se quiere formatear para el campo indicado
   @param upper Si TRUE convierte a mayúsculas el valor (si es de tipo cadena)
   @param connName Nombre de la conexion
   */
  QString formatValue(int t, const QVariant &v, const bool upper = false,
                      const QString &connName = "default") const;

  /**
   Suma dias a una fecha.

   @param d Fecha con la que operar
   @param nd Numero de dias que sumar. Si es negativo resta dias
   @return Fecha con el desplazamiento de dias
   */
  QDate addDays(const QDate &d, int nd) {
    return FLUtil::addDays(d, nd);
  }
  QDate addDays(const QString &d, int nd) {
    return FLUtil::addDays(QDate::fromString(d, Qt::ISODate), nd);
  }

  /**
   Suma meses a una fecha.

   @param d Fecha con la que operar
   @param nm Numero de meses que sumar. Si es negativo resta meses
   @return Fecha con el desplazamiento de meses
   */
  QDate addMonths(const QDate &d, int nm) {
    return FLUtil::addMonths(d, nm);
  }
  QDate addMonths(const QString &d, int nm) {
    return FLUtil::addMonths(QDate::fromString(d, Qt::ISODate), nm);
  }

  /**
   Suma años a una fecha.

   @param d Fecha con la que operar
   @param nm Numero de años que sumar. Si es negativo resta años
   @return Fecha con el desplazamiento de años
   */
  QDate addYears(const QDate &d, int ny) {
    return FLUtil::addYears(d, ny);
  }
  QDate addYears(const QString &d, int ny) {
    return FLUtil::addYears(QDate::fromString(d, Qt::ISODate), ny);
  }

  /**
   Diferencia de dias desde una fecha a otra.

   @param d1 Fecha de partida
   @param d2 Fecha de destino
   @return Número de días entre d1 y d2. Será negativo si d2 es anterior a d1.
   */
  int daysTo(const QDate &d1, const QDate &d2) {
    return FLUtil::daysTo(d1, d2);
  }
  int daysTo(const QString &d1, const QString &d2) {
    return FLUtil::daysTo(QDate::fromString(d1, Qt::ISODate),
                          QDate::fromString(d2, Qt::ISODate));
  }

  /**
   Ejecuta una query de tipo select, devolviendo los resultados del primer registro encontrado

   @param f Sentencia from de la query
   @param s Sentencia select de la query, que será el nombre del campo a devolver
   @param w Sentencia where de la query
   @param tL Sentencia tableslist de la query. Necesario cuando en la sentencia from se incluya más de una tabla
   @param connName Nombre de la conexion
   @return Valor resultante de la query o falso si no encuentra nada.
   */
  QVariant sqlSelect(const QString &f, const QString &s, const QString &w,
                     const QString &tL = QString::null,
                     const QString &connName = "default") {
    return FLUtil::sqlSelect(f, s, w, tL, 0, connName);
  }

  /**
   Versión rápida de sqlSelect. Ejecuta directamente la consulta sin realizar comprobaciones.
   Usar con precaución.
   */
  QVariant quickSqlSelect(const QString &f, const QString &s, const QString &w,
                          const QString &connName = "default") {
    return FLUtil::quickSqlSelect(f, s, w, connName);
  }

  /**
   Realiza la inserción de un registro en una tabla mediante un objeto FLSqlCursor

   @param t Nombre de la tabla
   @param fL Lista separada con comas de los nombres de los campos
   @param vL Lista separada con comas de los valores correspondientes
   @param connName Nombre de la conexion
   @return Verdadero en caso de realizar la inserción con éxito, falso en cualquier otro caso
   */
  bool sqlInsert(const QString &t, const QString &fL, const QString &vL, const QString &connName = "default") {
    return FLUtil::sqlInsert(t, fL, vL, connName);
  }

  /**
   Realiza la modificación de uno o más registros en una tabla mediante un objeto FLSqlCursor

   @param t Nombre de la tabla
   @param fL Lista separada con comas de los nombres de los campos
   @param vL Lista separada con comas de los valores correspondientes
   @param w Sentencia where para identificar los registros a editar.
   @param connName Nombre de la conexion
   @return Verdadero en caso de realizar la inserción con éxito, falso en cualquier otro caso
   */
  bool sqlUpdate(const QString &t, const QString &fL, const QString &vL, const QString &w, const QString &connName = "default") {
    return FLUtil::sqlUpdate(t, fL, vL, w, connName);
  }

  /**
   Borra uno o más registros en una tabla mediante un objeto FLSqlCursor

   @param t Nombre de la tabla
   @param w Sentencia where para identificar los registros a borrar.
   @param connName Nombre de la conexion
   @return Verdadero en caso de realizar la inserción con éxito, falso en cualquier otro caso
   */
  bool sqlDelete(const QString &t, const QString &w, const QString &connName = "default") {
    return FLUtil::sqlDelete(t, w, connName);
  }

  /**
   Versión rápida de sqlDelete. Ejecuta directamente la consulta sin realizar comprobaciones y sin disparar señales de commits.
   Usar con precaución.
   */
  bool quickSqlDelete(const QString &t, const QString &w, const QString &connName = "default") {
    return FLUtil::quickSqlDelete(t, w, connName);
  }

  /**
   Crea un diálogo de progreso

   @param l Label del diálogo
   @param tS Número total de pasos a realizar
   */
  void createProgressDialog(const QString &l, int tS) {
    FLUtil::createProgressDialog(l, tS, QObject::name());
  }
  void createProgressDialog() {
    FLUtil::createProgressDialog(QString::null, 0, QObject::name());
  }

  /**
   Destruye el diálogo de progreso
   */
  void destroyProgressDialog() {
    FLUtil::destroyProgressDialog(QObject::name());
  }

  /**
   Establece el grado de progreso del diálogo

   @param p Grado de progreso
   */
  void setProgress(int p) {
    FLUtil::setProgress(p, QObject::name());
  }

  /**
   Establece el número total de pasos del diálogo

   @param ts Número total de pasos
   */
  void setTotalSteps(int tS) {
    FLUtil::setTotalSteps(tS, QObject::name());
  }

  /**
   Cambia el texto de la etiqueta del diálogo

   @param l Etiqueta
   */
  void setLabelText(const QString &l) {
    FLUtil::setLabelText(l, QObject::name());
  }

  /**
   Obtiene la clave SHA1 de una cadena de texto.

   @param str Cadena de la que obtener la clave SHA1
   @return Clave correspondiente en digitos hexadecimales
   */
  QString sha1(const QString &str) {
    return FLUtil::sha1(str);
  }

  /**
   Obtiene la imagen o captura de pantalla de un formulario.

   @param n Nombre del fichero que contiene la descricpción del formulario.
   */
  QImage snapShotUI(const QString &n) {
    return FLUtil::snapShotUI(n);
  }

  /**
   Salva en un fichero con formato PNG la imagen o captura de pantalla de un formulario.

   @param n Nombre del fichero que contiene la descricpción del formulario.
   @param pathFile Ruta y nombre del fichero donde guardar la imagen
   */
  void saveSnapShotUI(const QString &n, const QString &pathFile) {
    FLUtil::saveSnapShotUI(n, pathFile);
  }

  /**
   Guarda la imagen de icono de un botón de un formulario en un ficher png. Utilizado para documentación

   @param data Contenido de la imagen en una cadena de caracteres
   @param pathFile Ruta completa al fichero donde se guadará la imagen
   */
  void saveIconFile(const QString &data, const QString &pathFile) {
    FLUtil::saveIconFile(data, pathFile);
  }

  /**
   Devuelve una cadena de dos caracteres con el código de idioma del sistema

   @return código de idioma del sistema
   */
  QString getIdioma() {
    return FLUtil::getIdioma();
  }

  /**
   Devuelve el sistema operativo sobre el que se ejecuta el programa

   @return Código del sistema operativo (WIN32, LINUX, MACX)
   */
  QString getOS() {
    return FLUtil::getOS();
  }

  /**
  Busca ficheros recursivamente en las rutas indicadas y según el patrón indicado

  Ejemplo:

  C++:
  QStringList filesFound = FLUtil::findFiles(QStringList() << "/home/user/Documents", "*.odt *.gif");
  for (QStringList::Iterator it = filesFound.begin(); it != filesFound.end(); ++it)
    qWarning(*it);

  QSA:
  var util = new FLUtil;
  var filesFound = util.findFiles( [ "/home/user/Documents" ], "*.odt *.gif");

  for(var i = 0; i < filesFound.length; ++i)
    debug(filesFound[i]);


  @param  paths   Rutas de búsqueda
  @param  filter  Patrón de filtrado para los ficheros. Admite varios separados por espacios "*.gif *.png".
                  Por defecto todos, "*"
  @param  breakOnFirstMatch Si es TRUE al encontrar el primer fichero que cumpla el patrón indicado, termina
                            la búsqueda y devuelve el nombre de ese fichero
  @return Lista de los nombres de los ficheros encontrados
  */
  QStringList findFiles(const QStringList &paths, const QString &filter = "*",
                        bool breakOnFirstMatch = false) {
    return FLUtil::findFiles(paths, filter, breakOnFirstMatch);
  }

  bool execSql(const QString &sql, const QString &connName = "default") {
    return FLUtil::execSql(sql, connName);
  }

  /**
  Guarda imagen Pixmap en una ruta determinada.

  @param data Contenido de la imagen en una cadena de caracteres
  @param filename: Ruta al fichero donde se guardará la imagen
  @param fmt Indica el formato con el que guardar la imagen
  @author Silix
  */
  void savePixmap(const QString &data, const QString &filename, const char *format) {
    FLUtil::savePixmap(data, filename, format);
  }

  /**
  Generación "casi-aleatoria" de números aleatorios por el método rápido xorshf96
  */
  void setFastRandSeed(uint seed) {
    aq_seed_rand = seed;
  }
  uint fastRand() const {
    uint t;
    aq_seed_rand_x ^= aq_seed_rand_x << 16;
    aq_seed_rand_x ^= aq_seed_rand_x >> 5;
    aq_seed_rand_x ^= aq_seed_rand_x << 1;

    t = aq_seed_rand_x;
    aq_seed_rand_x = aq_seed_rand_y;
    aq_seed_rand_y = aq_seed_rand_z;
    aq_seed_rand_z = t ^ aq_seed_rand_x ^ aq_seed_rand_y;

    return aq_seed_rand_z % aq_seed_rand;
  }

private:
  // Ver FLUtilInterface::fastRand()
  static uint aq_seed_rand;
  static uint aq_seed_rand_x;
  static uint aq_seed_rand_y;
  static uint aq_seed_rand_z;
};

//! Gestión de códigos de barras
/**
 Controla la gestión de un código de barras en base a su codificación.
 Se utiliza en artículos

 @author InfoSiAL S.L.
 */
class  FLCodBarInterface : public QObject
{

  Q_OBJECT

public:

  /**
   Constructor
   */
  FLCodBarInterface(const QString &value, int type, int margin, double scale, double cut, int rotation,
                    bool text_flag = false, QColor fg = Qt::black, QColor bg = Qt::white, int res = 72) : obj_(0), owner(false) {
    obj_ = new FLCodBar(value, type, margin, scale, cut, rotation, text_flag, fg, bg, res);
    owner = true;
  }

  /**
   constructor
   */
  FLCodBarInterface(FLCodBar *obj) : QObject(obj), obj_(0), owner(false) {
    obj_ = obj;
    owner = false;
  }

  /**
   destructor
   */
  ~FLCodBarInterface() {
    if (owner && obj_)
      obj_->deleteLater();
  }

public slots:

  /**
   Devueve el objeto imagen asociado al código de barras

   @return
   */
  QPixmap pixmap() {
    return obj_->pixmap();
  }

  /**
   Devueve el objeto imagen de error
   */
  QPixmap pixmapError() {
    return obj_->pixmapError();
  }

  /**
   Convierte un código numérico que identifica el tipo de código de barras en su descripción

   @param t. Número identificador del tipo de codificación
   @return Cadena con la descripción del tipo de codificación
   */
  QString typeToName(const int t) {
    return FLCodBar::typeToName(t);
  }

  /**
   Devuelve el valor del código de barras

   @return Valor del código de barras
   */
  QString value() {
    return obj_->value();
  }

  /**
   Devuelve el tipo de codificación

   @return Tipo de codificación
   */
  int type() {
    return obj_->type();
  }

  /**
   Devuelve el margen del objeto con respecto a la ventana

   @return Margen del objeto
   */
  int margin() {
    return obj_->margin();
  }

  /**
   Devuelve la escala del objeto

   @return Escala del objeto
   */
  double scale() {
    return obj_->scale();
  }

  /**
   */
  double cut() {
    return obj_->cut();
  }

  /**
   */
  bool text() {
    return obj_->text();
  }

  /**
   */
  int rotation() {
    return obj_->rotation();
  }

  /**
   Devuelve el color de la imagen

   @return Color de la imagen
   */
  QColor fg() {
    return obj_->fg();
  }

  /**
   Devuelve el color de fondo de la imagen

   @return Color de fondo de la imagen
   */
  QColor bg() {
    return obj_->bg();
  }

  /**
   Devuelve la resolución en puntos por pulgada

   @return Resolución en ppp
   */
  int res() {
    return obj_->res();
  }

  /**
   Indica si el valor del código de barras es válido

   @return TRUE si el código es válido, FALSE si no lo es
   */
  bool validBarcode() {
    return obj_->validBarcode();
  }

  /**
   */
  void setCaption(const QString &caption) {
    obj_->setCaption(caption);
  }

  /**
   */
  QString caption() {
    return obj_->caption();
  }

  /**
   Establece el valor del código de barras

   @param value. Valor del código de barras
   */
  void setValue(const QString &value) {
    obj_->setValue(value);
  }

  /**
   Establece el tipo de código de barras

   @param type. Tipo de codificación
   */
  void setType(const int type) {
    obj_->setType(type);
  }

  /**
   Establece el margen

   @param margin. Margen que se quiere establecer
   */
  void setMargin(const int margin) {
    obj_->setMargin(margin);
  }

  /**
   Establece la escala

   @param scale. Escala a establecer
   */
  void setScale(const double scale) {
    obj_->setScale(scale);
  }

  /**
   */
  void setCut(const double cut) {
    obj_->setCut(cut);
  }

  /**
   */
  void setText(const bool text) {
    obj_->setText(text);
  }

  /**
   */
  void setRotation(const int rotation) {
    obj_->setRotation(rotation);
  }

  /**
   Establece el color de la imagen

   @param fg. Color de la imagen
   */
  void setFg(const QColor &fg) {
    obj_->setFg(fg);
  }

  /**
   Establece el color de fondo de la imagen

   @param fg. Color de fondo de la imagen
   */
  void setBg(const QColor &bg) {
    obj_->setBg(bg);
  }

  /**
   Establece la resolución en puntos por pulgada

   @param res Resolución en punto por pulgada que tendrá la imagen del
   código de barras, la resolución se establecerá equitativamente
   tanto en el alto, como en el ancho de la imagen, es decir res x res.
   */
  void setRes(const int res) {
    obj_->setRes(res);
  }

  /**
   Convierte una descripción que identifica el tipo de código de barras en su equivamente numérico

   @param n. Cadena con la descripción del tipo de codificación
   @return Número identificador del tipo de codificación
   */
  int nameToType(const QString &n) {
    return FLCodBar::nameToType(n);
  }

  /**
   Borra la imagen del código de barras
   */
  void cleanUp() {
    obj_->cleanUp();
  }

  FLCodBar *obj() {
    return obj_;
  }

private:

  FLCodBar *obj_;
  bool owner;
};

//! Herramienta de edición de Scripts
/**
 Gestiona la herramienta visual de edición de Scripts incorporada
 en en AbanQ

 @author InfoSiAL S.L.
 */
class  FLScriptEditorInterface : public QObject
{

  Q_OBJECT

public:

  /**
   constructor

   @param nFS Nombre del fichero del script (*.qs)
   */
  FLScriptEditorInterface(const QString &nFS) : obj_(0) {
    obj_ = new FLScriptEditor(nFS, qApp->mainWidget());
  }

  /**
   destructor
   */
  ~FLScriptEditorInterface() {}

public slots:

  /**
   Establece el código a editar
   */
  void setCode(const QString &c) {
    obj_->setCode(c);
  }

  /**
   Obtiene el código en edición
   */
  QString code() const {
    return obj_->code();
  }

  /**
   Abre el cuadro de diálogo del editor como modal

   @return Indicador de si el diálogo ha sido aceptado o cancelado
   */
  int exec() {
    return obj_->exec();
  }

  FLScriptEditor *obj() {
    return obj_;
  }

private:

  FLScriptEditor *obj_;
};

//! Lista de nodos XML
/**
 Array con un listado de nodos xml obtenidos de un fichero

 @author InfoSiAL S.L.
 */
class  FLDomNodeListInterface : public QObject
{

  Q_OBJECT

public:

  /**
   Constructor
   */
  FLDomNodeListInterface(QObject *parent = 0)
    : QObject(parent) {
  }

  /**
   Constructor
   */
  FLDomNodeListInterface(const QDomNodeList &nl, QObject *parent = 0)
    : QObject(parent),
      nl_(nl) {
  }

  /**
   Destructor
   */
  ~FLDomNodeListInterface() {}

public slots:

  /**
   Nodo que ocupa una cierta posición en la lista de nodos

   @param index. Indice del nodo buscado
   @return Nodo, nulo si el índice es negativo o superior a la longitud de la lista
   */
  FLDomNodeInterface *item(int index);

  /**
   Devuelve el número de nodos de la lista

   @return Devuelve el número de nodos de la lista
   */
  uint length() const {
    return nl_.length();
  }
  uint count() const {
    return nl_.count();
  }

  QDomNodeList obj() {
    return nl_;
  }

private:

  QDomNodeList nl_;
};

//! Nodo XML
/**
 Para gestionar los nodos de un archivo de texto xml y poder leer sus propiedades en
 base a los distintos niveles de agrupamiento.

 @author InfoSiAL S.L.
 */
class  FLDomNodeInterface : public QObject
{
  Q_OBJECT

public:

  /**
   Constructor
   */
  FLDomNodeInterface(QObject *parent = 0)
    : QObject(parent),
      dn_(0),
      type_(QDomNode::BaseNode) {
    setIdNode();
  }
  FLDomNodeInterface(QDomNode::NodeType type, QObject *parent = 0)
    : QObject(parent),
      dn_(0),
      type_(type) {
    setIdNode();
  }
  FLDomNodeInterface(const QDomNode &n, QObject *parent = 0)
    : QObject(parent),
      dn_(n.isNull() ? 0 : new QDomNode(n)),
      type_(QDomNode::BaseNode) {
    setIdNode();
  }
  FLDomNodeInterface(const QDomElement &n, QObject *parent = 0)
    : QObject(parent),
      dn_(n.isNull() ? 0 : new QDomElement(n)),
      type_(QDomNode::ElementNode) {
    setIdNode();
  }
  FLDomNodeInterface(const QDomText &n, QObject *parent = 0)
    : QObject(parent),
      dn_(n.isNull() ? 0 : new QDomText(n)),
      type_(QDomNode::TextNode) {
    setIdNode();
  }
  FLDomNodeInterface(const QDomDocument &n, QObject *parent = 0)
    : QObject(parent),
      dn_(n.isNull() ? 0 : new QDomDocument(n)),
      type_(QDomNode::DocumentNode) {
    setIdNode();
  }

  /**
   Destructor
   */
  virtual ~FLDomNodeInterface();

  static FLDomNodeInterface *nodeInterface(const QDomNode &n);
  static FLDomNodeInterface *nodeInterface(const QDomElement &n);
  static FLDomNodeInterface *nodeInterface(const QDomText &n);
  static FLDomNodeInterface *nodeInterface(const QDomDocument &n);

  QDomNode *newNode() {
    delete dn_;
    dn_ = 0;
    switch (type_) {
      case QDomNode::ElementNode:
        dn_ = new QDomElement();
        break;
      case QDomNode::TextNode:
        dn_ = new QDomText();
        break;
      case QDomNode::DocumentNode:
        dn_ = new QDomDocument();
        break;
      default:
        dn_ = new QDomNode();
    }
    return dn_;
  }

  void updateNode(const QDomNode &n) {
    if (dn_)
      *dn_ = n;
    else
      dn_ = new QDomNode(n);
  }

  void updateNode(const QDomElement &n) {
    if (dn_) {
      if (type_ != QDomNode::ElementNode) {
        delete dn_;
        dn_ = new QDomElement(n);
      } else
        *dn_ = n;
    } else
      dn_ = new QDomElement(n);
    type_ = QDomNode::ElementNode;
  }

  void updateNode(const QDomText &n) {
    if (dn_) {
      if (type_ != QDomNode::TextNode) {
        delete dn_;
        dn_ = new QDomText(n);
      } else
        *dn_ = n;
    } else
      dn_ = new QDomText(n);
    type_ = QDomNode::TextNode;
  }

  void updateNode(const QDomDocument &n) {
    if (dn_) {
      if (type_ != QDomNode::DocumentNode) {
        delete dn_;
        dn_ = new QDomDocument(n);
      } else
        *dn_ = n;
    } else
      dn_ = new QDomDocument(n);
    type_ = QDomNode::DocumentNode;
  }

  void setIdNode();

  QDomNode::NodeType type() const {
    return type_;
  }

public slots:

  /**
   Obtiene el identificador único del nodo
   */
  QString idNode() const {
    return QString::number(idNode_);
  }

  /**
   Devuelve un nodo a partir de su identificador único

   @param  id Identificador único del nodo
   @return Nodo tipo FLDomNodeInterface
   */
  FLDomNodeInterface *nodeFromIdNode(const QString &id) const;

  /**
   Elimina el contenido del nodo estableciéndolo a nulo
   */
  void clear() {
    delete dn_;
    dn_ = 0;
  }

  /**
   Devuelve el valor del nodo

   @return Valor del nodo
   */
  QString nodeValue() const {
    return (dn_ ? dn_->nodeValue() : QString::null);
  }

  /**
   Devuelve el nombre del nodo

   @return Nombre del nodo
   */
  QString nodeName() const {
    return (dn_ ? dn_->nodeName() : QString::null);
  }

  /**
   Convierte el nodo a FLDomElementInterface

   @return Nodo tipo FLDomElementInterface
   */
  FLDomNodeInterface *toElement() {
    return (dn_ ? nodeInterface(dn_->toElement()) : 0);
  }

  /**
   Indica si el tipo del nodo es DomElement

   @return indicador de si el tipo del nodo es DomElement
   */
  bool isElement() const {
    return (dn_ ? dn_->isElement() : false);
  }

  /**
   Indica si el tipo del nodo es DomText

   @return indicador de si el tipo del nodo es DomText
   */
  bool isText() const {
    return (dn_ ? dn_->isText() : false);
  }

  /**
   Indica si el tipo del nodo es DomComment

   @return indicador de si el tipo del nodo es DomComment
   */
  bool isComment() const {
    return (dn_ ? dn_->isComment() : false);
  }

  /**
   Devuelve el siguiente nodo hermano

   @return Nodo hermano
   */
  FLDomNodeInterface *nextSibling() {
    return (dn_ ? nodeInterface(dn_->nextSibling()) : 0);
  }

  /**
   Devuelve el anterior nodo hermano

   @return Nodo hermano
   */
  FLDomNodeInterface *previousSibling() {
    return (dn_ ? nodeInterface(dn_->previousSibling()) : 0);
  }

  /**
   Devuelve el primer nodo hijo cuyo nombre coincide con el especificado

   @return Nodo hijo, nulo si el nombre no se encuentra
   */
  FLDomNodeInterface *namedItem(const QString &name) {
    return (dn_ ? nodeInterface(dn_->namedItem(name)) : 0);
  }

  /**
   Devuelve una copia profunda del nodo actual

   @param deep: Indica si los nodos hijos serán también clonados o no
   @return Nodo clonado
   */
  FLDomNodeInterface *cloneNode(bool deep = true) {
    return nodeInterface(DN_->cloneNode(deep));
  }

  /**
   Indica si el nodo es igual al nodo pasado por parámetro

   @param n: nodo con el que comparar
   @return true si son iguales, false en caso contrario
   */
  bool isEqualNode(FLDomNodeInterface *n) {
    if (!n)
      return false;
    QDomDocument d1;
    QDomDocument d2;
    QString content = "<dd/>";
    d1.setContent(content);
    d2.setContent(content);
    d1.firstChild().appendChild(DN_->cloneNode());
    d2.firstChild().appendChild(n->obj().cloneNode());
    return (d1.toString() == d2.toString());
  }

  /**
   Inserta un nodo delante de un nodo de referencia

   @param newChild: nodo a insertar
   @param refChild: nodo de referencia
   @return nodo insertado
   */
  FLDomNodeInterface *insertBefore(FLDomNodeInterface *newChild, FLDomNodeInterface *refChild = 0) {
    if (!dn_ || !newChild)
      return 0;
    if (!refChild)
      return nodeInterface(dn_->insertBefore(newChild->obj(), QDomNode()));
    return nodeInterface(dn_->insertBefore(newChild->obj(), refChild->obj()));
  }

  /**
   Inserta un nodo después de un nodo de referencia

   @param newChild: nodo a insertar
   @param refChild: nodo de referencia
   @return nodo insertado
   */
  FLDomNodeInterface *insertAfter(FLDomNodeInterface *newChild, FLDomNodeInterface *refChild) {
    return ((dn_ && newChild && refChild) ? nodeInterface(dn_->insertAfter(newChild->obj(), refChild->obj())) : 0);
  }

  /**
   Quita un nodo hijo

   @param oldChild: nodo a quitar

   @return nodo quitado
   */
  FLDomNodeInterface *removeChild(FLDomNodeInterface *oldChild) {
    return ((dn_ && oldChild) ? nodeInterface(dn_->removeChild(oldChild->obj())) : 0);
  }

  /**
   Indica si el nodo tiene atributos

   @return TRUE si tiene atributos, FALSE si no tiene
   */
  bool hasAttributes() const {
    return (dn_ ? dn_->hasAttributes() : false);
  }

  /**
   Indica si el nodo tiene hijos

   @return TRUE si tiene hijos, FALSE si no tiene
   */
  bool hasChildNodes() const {
    return (dn_ ? dn_->hasChildNodes() : false);
  }

  /**
   Devuelve el valor de un atributo

   @param att. Nombre del atributo
   @return Valor del atributo. Si el atributo no existe devolverá una cadena vacía
   */
  QString attributeValue(const QString &att) const {
    if (!hasAttributes())
      return QString::null;
    QDomNode n(dn_->attributes().namedItem(att));
    return (n.isNull() ? QString::null : n.nodeValue());
  }

  /**
   Devuelve el contenido del primer nodo de tipo comentario hijo del nodo actual

   @return Comentario
   */
  QString comment() const {
    if (!dn_)
      return QString::null;
    QDomNodeList nl(dn_->childNodes());
    uint ncount = nl.count();
    for (uint i = 0; i < ncount; ++i) {
      if (nl.item(i).isComment())
        return nl.item(i).nodeValue();
    }
    return QString::null;
  }

  /**
   Asigna un valor al nodo

   @param v. Valor a asignar
   */
  void setNodeValue(const QString &v) {
    DN_->setNodeValue(v);
  }

  /**
   Devuelve el primer nodo hijo del nodo XML

   @return primer nodo hijo del nodo XML
   */
  FLDomNodeInterface *firstChild() {
    return (dn_ ? nodeInterface(dn_->firstChild()) : 0);
  }

  /**
   Devuelve el ultimo nodo hijo del nodo XML

   @return ultimo nodo hijo del nodo XML
   */
  FLDomNodeInterface *lastChild() {
    return (dn_ ? nodeInterface(dn_->lastChild()) : 0);
  }

  /**
   Devuelve el nodo padre del nodo XML

   @return nodo padre del nodo XML
   */
  FLDomNodeInterface *parentNode() {
    return (dn_ ? nodeInterface(dn_->parentNode()) : 0);
  }

  /**
   Añade un nodo hijo al nodo

   @param newChild: Nodo a añadir
   @return Nodo añadido
   */
  FLDomNodeInterface *appendChild(FLDomNodeInterface *newChild) {
    return (newChild ? nodeInterface(DN_->appendChild(newChild->obj())) : 0);
  }

  /**
   Cambia un nodo hijo por otro

   @param newChild: Nodo a añadir
   @param oldChild: Nodo a quitar
   @return Referencia al nodo hijo quitado
   */
  FLDomNodeInterface *replaceChild(FLDomNodeInterface *newChild, FLDomNodeInterface *oldChild) {
    return ((newChild && oldChild) ? nodeInterface(DN_->replaceChild(newChild->obj() , oldChild->obj())) : 0);
  }

  /**
   Devuelve el la lista de nodos hijos

   @return lista de nodos hijos
   */
  FLDomNodeListInterface *childNodes() const {
    return (hasChildNodes() ? new FLDomNodeListInterface(dn_->childNodes()) : 0);
  }

  /**
   Devuelve el documento que posee el nodo

   @return documento
   */
  FLDomNodeInterface *ownerDocument() const {
    return (dn_ ? nodeInterface(dn_->ownerDocument()) : 0);
  }

  /**
   Devuelve si el nodo es null

   @return nodo null
   */
  bool isNull() const {
    return (dn_ ? dn_->isNull() : true);
  }

  /**
   @return El tipo de nodo
   */
  QDomNode::NodeType nodeType() const {
    return (dn_ ? dn_->nodeType() : QDomNode::BaseNode);
  }

  /**
   @return Objeto interno
   */
  QDomNode obj() {
    return *DN_;
  }
  QDomNode *ptroObj() {
    return dn_;
  }

  //! Elemento derivado de un nodo XML
  /**
   Al convertir un nodo xml en un elemento se puede acceder a ciertas propiedades
   del mismo, como el texto.

   @author InfoSiAL S.L.
   */

  /**
   Devuelve el texto del nodo

   @return Texto del nodo
   */
  QString text() const {
    return ((dn_ && type_ == QDomNode::ElementNode) ?
            static_cast<QDomElement *>(dn_)->text() :
            QString::null);
  }

  /**
   Obtiene el valor de un atributo del nodo

   @param name: Nombre del atributo
   @param defValue: Valor que se devolverá si el atributo no existe
   @return valor del atributo
   */
  QString attribute(const QString &name, const QString &defValue = QString::null) const {
    return ((dn_ && type_ == QDomNode::ElementNode) ?
            static_cast<QDomElement *>(dn_)->attribute(name, defValue) :
            QString::null);
  }

  /**
   Crea o cambia el valor de un atributo

   @param name: Nombre del atributo
   @param value: Valor del atributo
   */
  void setAttribute(const QString &name, const QString &value) {
    if (type_ == QDomNode::ElementNode)
      static_cast<QDomElement *>(DN_)->setAttribute(name, value);
  }

  /**
   Elimina un atributo

   @param name: Nombre del atributo
   */
  void removeAttribute(const QString &name) {
    if (dn_ && type_ == QDomNode::ElementNode)
      static_cast<QDomElement *>(dn_)->removeAttribute(name);
  }

  //! Manejador de textos en formato XML
  /**
   Permite gestionar los nodos xml de un documento de texto.

   @author InfoSiAL S.L.
   */

  /**
   Lee el documento xml a partir de una cadena de texto

   @param buffer: Cadena que contiene el documento XML
   @return Falso si hay error de parseo
   */
  bool setContent(const QString &buffer);

  /**
   Crea un nodo domElement en el documento

   @param tagName. Nombre de la etiqueta del nodo
   @return nodo domElement creado
   */
  FLDomNodeInterface *createElement(const QString &tagName) {
    if (type_ != QDomNode::DocumentNode)
      return 0;
    return nodeInterface(static_cast<QDomDocument *>(DN_)->createElement(tagName));
  }

  /**
   Crea un nodo domText en el documento

   @param value. Valor del nodo
   @return nodo domText creado
   */
  FLDomNodeInterface *createTextNode(const QString &value) {
    if (type_ != QDomNode::DocumentNode)
      return 0;
    return nodeInterface(static_cast<QDomDocument *>(DN_)->createTextNode(value));
  }

  /**
   Devuelve el texto que define el documento XML

   @return texto que define el documento XML
   */
  QString toString(const int indent = 0) {
    return ((dn_ && type_ == QDomNode::DocumentNode) ?
            static_cast<QDomDocument *>(dn_)->toString(indent) :
            QString::null);
  }

  /**
   Devuelve una lista de todos los nodos del documento cuya etiqueta coincide con el valor dado

   @param tagName. Nombre de la etiqueta
   @return Lista de nodos encontrados
   */
  FLDomNodeListInterface *elementsByTagName(const QString &tagName) const {
    if (!dn_)
      return 0;
    QDomNodeList nl;
    if (type_ == QDomNode::ElementNode)
      nl = QDomNodeList(static_cast<QDomElement *>(dn_)->elementsByTagName(tagName));
    else if (type_ == QDomNode::DocumentNode)
      nl = QDomNodeList(static_cast<QDomDocument *>(dn_)->elementsByTagName(tagName));
    return (nl.count() ? new FLDomNodeListInterface(nl) : 0);
  }

  /**
   Limpia la cache de nodos para liberar memoria.

   Todos los nodos creados con anterioridad a la llamada a esta función serán eliminados.
   */
  void cleanup();

public:

  static void cleanupDomNodesCache();

private:

  QDomNode *dn_;
  QDomNode::NodeType type_;
  uint idNode_;

  static FLDomNodesCache *nodesCache;
  static uint idNodeGlob;

  friend class FLObjectFactory;
};

inline FLDomNodeInterface *FLDomNodeListInterface::item(int index)
{
  return FLDomNodeInterface::nodeInterface(nl_.item(index));
}

//! Visor de informes
/**
Visor para previsualizar informes o colecciones de paginas

El visor tiene tiene dos modos de funcionamiento;

- Visor con capacidades de renderizado. Cuando se establecen los datos
  y plantilla, o se le asigna un motor de renderizado. En este caso esta clase
  funciona como una simplificacion de alto nivel para FLReportEngine.
- Sólo visualizador de páginas. Cuando se le indica sólo el conjunto de
  páginas a visualizar.

@author InfoSiAL S.L.
*/
class  FLReportViewerInterface : public QObject
{

  Q_OBJECT
  Q_ENUMS(RenderReportFlags)

public:

  enum RenderReportFlags {
    Append    = 0x00000001,
    Display   = 0x00000002,
    PageBreak = 0x00000004
  };

  /**
  constructor
  */
  FLReportViewerInterface() : QObject(0), obj_(0) {
    obj_ = new FLReportViewer();
    connects();
  }

  /**
  constructor
  */
  FLReportViewerInterface(FLReportViewer *obj) : QObject(obj), obj_(0) {
    obj_ = obj;
    connects();
  }

  /**
  constructor
  */
  FLReportViewerInterface(QWidget *w, bool) : QObject(w) {
    obj_ = new FLReportViewer(w, 0, true);
    connects();
  }

  /**
  constructor
  */
  FLReportViewerInterface(FLReportEngine *r) : QObject(0) {
    obj_ = new FLReportViewer(0, 0, false, r);
    connects();
  }

  /**
   destructor
   */
  ~FLReportViewerInterface() {}

public slots:

  /**
  Renderiza el informe en el visor.

  Sólo tiene efecto si se han indicado datos y plantilla para el visor con
  setReportData y setReportTemplate, o se le ha asignado un motor de informes
  explícitamente con setReportEngine o en el constructor.

  @return TRUE si todo ha ido bien
  */
  bool renderReport(const int initRow = 0, const int initCol = 0, const bool append = false, const bool displayReport = true) {
    return obj_->renderReport(initRow, initCol, append, displayReport);
  }
  bool renderReport2(const int initRow = 0, const int initCol = 0, const uint flags = FLReportViewerInterface::Display) {
    return obj_->renderReport(initRow, initCol, flags);
  }

  /**
   Establece los datos del informe a partir de una consulta

   @param q Objeto FLSqlQuery con la consulta de la que se toman los datos
   @return TRUE si todo ha ido bien
   */
  bool setReportData(FLSqlQueryInterface *q) {
    return obj_->setReportData(q->obj());
  }

  /**
   Establece los datos del informe a partir de un documento XML

   @param xmlDoc Objeto FLDomDocument con los datos
   @return TRUE si todo ha ido bien
   */
  bool setReportData(FLDomNodeInterface *xmlDoc) {
    if (xmlDoc)
      return obj_->setReportData(xmlDoc->obj());
    return false;
  }

  /**
   Establece la plantilla para el informe.

   El nombre de la plantilla corresponde con el nombre del fichero con extesión ".kut".

   @param  t     Nombre de la plantilla
   @param  style Nombre de la hoja de estilo a aplicar
   @return TRUE si todo ha ido bien
   */
  bool setReportTemplate(const QString &t, const QString &style = QString::null) {
    return obj_->setReportTemplate(t, style);
  }

  /**
   Establece la plantilla para el informe.

   El nombre de la plantilla corresponde con el nombre del fichero con extesión ".kut".

   @param  xmlDoc     Nombre de la plantilla en formato xml
   @param  style Nombre de la hoja de estilo a aplicar
   @return TRUE si todo ha ido bien
   */
  bool setReportTemplate(FLDomNodeInterface *xmlDoc, const QString &style = QString::null) {
    if (!xmlDoc) {
      return false;
    }
    return obj_->setReportTemplate(xmlDoc->obj(), style);
  }

  /**
  @return Los datos de origen del informe en un documento XML con el formato usado por Kugar.
          Se debe llamar despues de establecer los datos para el informe con set*ReportData, de
          lo contrario el resultado será indefinido.
          Importante: Devuelve un nodo de referencia al arbol XML interno usado para crear el informe,
                      si se modifica ese nodo se está modificando el XML interno del informe.
                      Se debe llamar a cloneNode si se desea trabajar sobre una copiar.
  */
  FLDomNodeInterface *reportData() const {
    return obj_->rptEngine()->reportData();
  }

  /**
  @return Plantilla del informe en un documento XML con el formato usado por Kugar.
          Se debe llamar despues de establecer los datos para el informe con set*ReportTemplate, de
          lo contrario el resultado será indefinido.
          Importante: Devuelve un nodo de referencia al arbol XML interno usado para crear el informe,
                      si se modifica ese nodo se está modificando el XML interno del informe.
                      Se debe llamar a cloneNode si se desea trabajar sobre una copiar.
  */
  FLDomNodeInterface *reportTemplate() const {
    return obj_->rptEngine()->reportTemplate();
  }

  /**
   Muestra el formulario y entra en un nuevo bucle de eventos.
   */
  void exec() {
    obj_->exec();
  }
  void show() {
    obj_->exec();
  }

  /**
  Obtiene una versión csv de los datos del informe (una vez ejecutado)

  Solo tiene efecto si el visor tiene un motor de informes activo
  */
  QString csvData() {
    return obj_->csvData();
  }

  /**
   Imprime directamente el informe sin mostrarlo
   */
  void printReport() {
    obj_->slotPrintReport();
  }

  /**
   Imprime directamente el informe en un fichero PS sin mostrarlo
   */
  void printReportToPS(const QString &outPsFile) {
    obj_->slotPrintReportToPS(outPsFile);
  }

  /**
   Imprime directamente el informe en un fichero PDF sin mostrarlo
   */
  void printReportToPDF(const QString &outPdfFile) {
    obj_->slotPrintReportToPDF(outPdfFile);
  }

  /**
   Establece el número de copias por defecto a imprimir
   */
  void setNumCopies(const int numCopies) {
    obj_->setNumCopies(numCopies);
  }

  /**
   Establece si el informe se debe imprimir en una impresora ESC/POS
   */
  void setPrintToPos(bool ptp) {
    obj_->setPrintToPos(ptp);
  }

  /**
   Establece el nombre de la impresora a la que imprimir.

   Si se establece el nombre de la impresora no se mostrará el cuadro de dialogo de impresión, y se
   usará esa impresora para imprimir directamente. Para que el cuadro de diálogo de impresión se muestre bastará
   con establecer un nombre vacío; setPrinterName( QString::null ).
   */
  void setPrinterName(const QString &pName) {
    obj_->setPrinterName(pName);
  }

  /**
   Devuelve si el último informe ha sido imprimido en impresora o archivo.
   */
  bool reportPrinted() {
    obj_->reportPrinted();
  }

  /**
   Cambia el widget padre del visor incrustandolo en la capa principal

   @param parentFrame: Nuevo padre. Debe ser QFrame con al menos una capa VBox
   */
  void reparent(QWidget *parentFrame) {
    obj_->rptViewerReparent(parentFrame);
  }

  /**
   Muestra la primera página del informe
   */
  void slotFirstPage() {
    obj_->slotFirstPage();
  }

  /**
   Muestra la útlima página del informe
   */
  void slotLastPage() {
    obj_->slotLastPage();
  }

  /**
   Muestra la siguiente página del informe
   */
  void slotNextPage() {
    obj_->slotNextPage();
  }

  /**
   Muestra la anterior página del informe
   */
  void slotPrevPage() {
    obj_->slotPrevPage();
  }

  /**
   Aumenta zoom de la página actual
   */
  void slotZoomUp() {
    obj_->slotZoomUp();
  }

  /**
   Disminuye zoom de la página actual
   */
  void slotZoomDown() {
    obj_->slotZoomDown();
  }

  /**
   Exporta a un fichero de disco la version CSV del informe
   */
  void exportFileCSVData() {
    obj_->exportFileCSVData();
  }

  /**
   Exporta el informe a un fichero en formato PDF
   */
  void exportToPDF() {
    obj_->exportToPDF();
  }

  /**
  Exporta el informe a un fichero en formato PDF y lo envia por correo eléctronico
  */
  void sendEMailPDF() {
    obj_->sendEMailPDF();
  }

  /**
  Guarda como plantilla de estilo SVG
  */
  void saveSVGStyle() {
    obj_->saveSVGStyle();
  }

  /**
  Guarda la página actual como plantilla de estilo SVG simplificada ( sólo los campos de datos )
  */
  void saveSimpleSVGStyle() {
    obj_->saveSimpleSVGStyle();
  }

  /**
  Carga y aplica una plantilla de estilo SVG
  */
  void loadSVGStyle() {
    obj_->exportFileCSVData();
  }

  /**
  Establece si el visor debe cerrarse automáticamente tras imprimir el informe

  @param b TRUE para cierre automático, FALSE para desactivar cierre automático
  */
  void setAutoClose(const bool b) {
    obj_->setAutoClose(b);
  }

  /**
  Establece la resolucion de la impresora

  @param dpi Resolucion en puntos por pulgada
  */
  void setResolution(int dpi) {
    obj_->setResolution(dpi);
  }
  void setPixel(int relDpi) {
    obj_->setPixel(relDpi);
  }
  void setDefaults() {
    obj_->setDefaults();
  }

  /**
  Actualizar el informe.

  Emite la señal requestUpdateReport y llama a updateDisplay.
  Si el visor tiene un motor de informes ejecuta de nuevo la consulta y el renderizado.
  */
  void updateReport() {
    obj_->updateReport();
  }

  /**
  Actualiza el visor, redibujando la coleccion de paginas en pantalla
  */
  void  updateDisplay() {
    obj_->updateDisplay();
  }

  /**
  Establece el nombre del estilo
  */
  void setStyleName(const QString &style) {
    obj_->setStyleName(style);
  }

  /**
  Visor básico de Kugar
  */
  MReportViewer *rptViewer() {
    return obj_->rptViewer();
  }

  /**
  Establece el motor de informes a usar por visor
  */
  void setReportEngine(FLReportEngine *r) {
    obj_->setReportEngine(r);
  }

  /**
  Incrusta el visor básico de Kugar en la capa principal de un objeto widget padre ( generalmente un QFrame )
  */
  void rptViewerEmbedInParent(QWidget *parentFrame) {
    obj_->rptViewerEmbedInParent(parentFrame);
  }

  /**
  Establece directamente la coleccion de paginas a visualizar.
  No seran visibles hasta que no se ejecute updateDisplay.

  Al estableder la colección de paginas la clase pasa a ser un mero
  visualizador de esas páginas, es decir, no tiene un motor de informes
  asignado y los métodos específicos que llaman al motor de informes no
  tendran efecto (renderReport, csvData, etc..)
  */
  void setReportPages(FLReportPages *pgs) {
    obj_->setReportPages(pgs);
  }

  /**
  Metodos proporcionados por ergonomia. Son un enlace a los
  mismos métodos que proporciona FLReportPages, para manejar
  la coleccion de paginas del visor
  */
  FLPicture *getCurrentPage() {
    return obj_->getCurrentPage();
  }
  FLPicture *getFirstPage() {
    return obj_->getFirstPage();
  }
  FLPicture *getPreviousPage() {
    return obj_->getPreviousPage();
  }
  FLPicture *getNextPage() {
    return obj_->getNextPage();
  }
  FLPicture *getLastPage() {
    return obj_->getLastPage();
  }
  FLPicture *getPageAt(uint i) {
    return obj_->getPageAt(i);
  }
  void clearPages() {
    obj_->clearPages();
  }
  void appendPage() {
    obj_->appendPage();
  }
  int getCurrentIndex() {
    return obj_->getCurrentIndex();
  }
  void setCurrentPage(int idx) {
    obj_->setCurrentPage(idx);
  }
  void setPageSize(int s) {
    obj_->setPageSize(s);
  }
  void setPageOrientation(int o) {
    obj_->setPageOrientation(o);
  }
  void setPageDimensions(QSize dim) {
    obj_->setPageDimensions(dim);
  }
  int pageSize() {
    return obj_->pageSize();
  }
  int pageOrientation() {
    return obj_->pageOrientation();
  }
  QSize pageDimensions() {
    return obj_->pageDimensions();
  }
  int pageCount() {
    return obj_->pageCount();
  }

  /**
  Devuelve un objeto hijo del visor

  @param objName. Nombre del objeto hijo
  @return Objeto hijo
  */
  QObject *child(const QString &objName) {
    if (!obj_)
      return 0;
    return obj_->child(objName);
  }

  /**
  Para desactivar las acciones por defecto para imprimir y exportar

  Esto es util cuando se quieren capturar la señales que disparan
  los botones del formulario, desactivar lo que hacen por defecto,
  y susituirlo por funcionalidad especifica. Por ejemplo para mostrar un
  dialogo de impresion personalizado.
  */
  void disableSlotsPrintExports(bool disablePrints = true, bool disableExports = true) {
    obj_->disableSlotsPrintExports(disablePrints, disableExports);
  }

  void setName(const QString &n) {
    obj_->setName(n);
  }

  FLReportViewer *obj() {
    return obj_;
  }

signals:

  /**
  Señal emitida cuando se va a actualizar el informe
  */
  void requestUpdateReport();

private:

  void connects() const {
    connect(obj_, SIGNAL(requestUpdateReport()), this, SIGNAL(requestUpdateReport()));
    connect(obj_, SIGNAL(destroyed()), this, SIGNAL(destroyed()));
  }

  FLReportViewer *obj_;
};

//! Manejador de procesos de sistema
/**
 Permite arrancar y gestionar procesos de consola.

 @author InfoSiAL S.L.
 */
class  FLProcessInterface : public QProcess
{

  Q_OBJECT

public:

  /**
   Constructor
   */
  FLProcessInterface() : obj_(0) {
    obj_ = new QProcess();
    connects();
  };

  /**
   Constructor
   */
  FLProcessInterface(const QString &arg0) : obj_(0) {
    obj_ = new QProcess(arg0);
    connects();
  };

  /**
   Destructor
   */
  ~FLProcessInterface() {
    if (obj_)
      obj_->deleteLater();
  }

public slots:

  /**
   Arranca el comando definido en el parámetro arg0 del constructor

   @return true si el comando se ejecutó, false en caso contrario
   */
  bool start() {
    return obj_->start();
  }

  /**
   Establece el directorio donde se ejecuta el proceso
   */
  void setWorkingDirectory(const QString &path) {
    QDir dir(path);
    obj_->setWorkingDirectory(dir);
  }

  /**
   Establece los argumentos para el proceso

   @param args Argumentos del proceso: comando + argumentos
   */
  void setArguments(const QStringList &args) {
    obj_->setArguments(args);
  }

  /**
   Añade un argumento al proceso

   @param arg Argumento a añadir el proceso
   */
  void addArgument(const QString &arg) {
    obj_->addArgument(arg);
  }

  /**
   Indica si el proceso está en ejecución

   @return true si está en ejecución, false en caso contrario
   */
  bool isRunning() {
    return obj_->isRunning();
  }

  /**
   Devuelve la salida estándar del proceso

   @return Array binario con el contenido de la salida estándar del proceso
   */
  QByteArray readStdout() {
    return obj_->readStdout();
  }

  void emitExited() {
    emit exited();
  }

signals:

  /**
   Señal emitida cuando ha terminado el proceso
   */
  void exited();

private:

  void connects() {
    disconnect(obj_, SIGNAL(processExited()), this, SLOT(emitExited()));
    connect(obj_, SIGNAL(processExited()), this, SLOT(emitExited()));
  }

  QProcess *obj_;
};

class  FLPosPrinterInterface : public QObject
{

  Q_OBJECT
  Q_ENUMS(PaperWidth)

public:

  enum PaperWidth {P57_5MM, P69_5MM, P76MM, NPaperWidth = P76MM};

  /**
   constructor
   */
  FLPosPrinterInterface() : obj_(0) {
    obj_ = new FLPosPrinter();
  }

  /**
   destructor
   */
  ~FLPosPrinterInterface() {
    if (obj_)
      delete obj_;
  }

public slots:

  int paperWidth() {
    return obj_->paperWidth();
  }

  void setPaperWidth(int pw) {
    obj_->setPaperWidth((FLPosPrinter::PaperWidth)pw);
  }

  const QString &printerName() {
    return obj_->printerName();
  }

  void setPrinterName(const QString &pName) {
    obj_->setPrinterName(pName);
  }

  void cleanup() {
    obj_->cleanup();
  }

  void flush() {
    obj_->flush();
  }

  void send(const QString &str, const int col = -1, const int row = -1) {
    obj_->send(str, col, row);
  }

private:

  FLPosPrinter *obj_;
};

//! Cliente SMTP para correo electrónico
/**
 Cliente SMTP para correo electrónico

 Esta clase es utilizada para componer un mensaje de correo electrónico y
 enviarlo mediante el protocolo SMTP.

 @author InfoSiAL, S.L.
 */
class  FLSmtpClientInterface : public QObject
{
  Q_OBJECT

public:

  /**
   Constructor
   */
  FLSmtpClientInterface() : QObject(0) {
    obj_ = new FLSmtpClient(this);
    connects();
  }

  /**
   Constructor
   */
  FLSmtpClientInterface(FLSmtpClient *obj) : QObject(obj), obj_(0) {
    obj_ = obj;
  }

  /**
   Destructor
   */
  ~FLSmtpClientInterface() {}

public slots:

  /**
   Establece el remitente del mensaje

   @param from Dirección de correo del remitente
   */
  void setFrom(const QString &from) {
    obj_->setFrom(from);
  }

  /**
   Establece el destinatario del mensaje

   @param to Dirección de correo del destinatario
   */
  void setTo(const QString &to) {
    obj_->setTo(to);
  }

  /**
   Establece el asunto del mensaje

   @param subject Texto del asunto
   */
  void setSubject(const QString &subject) {
    obj_->setSubject(subject);
  }

  /**
   Establece el cuerpo del mensaje

   @param body Texto del cuerpo
   */
  void setBody(const QString &body) {
    obj_->setBody(body);
  }

  /**
   Adjunta un fichero al mensaje

   @param attach Ruta y nombre del fichero a adjuntar
   @param cid    Id para incrustar fichero en HTML
   */
  void addAttachment(const QString &attach, const QString &cid = QString::null) {
    obj_->addAttachment(attach, cid);
  }

  /**
   Añade una parte de texto al mensaje

   @param text     Texto a añadir
   @param mimeType Formato MIME del texto. Por defecto text/plain
   */
  void addTextPart(const QString &text, const QString &mimeType = "text/plain") {
    obj_->addTextPart(text, mimeType);
  }

  /**
   Servidor de correo saliente.

   Si no se indica un servidor de correo saliente intentará averiguarlo
   mediante los registros MX de los servidores de DNS.

   @param mS Nombre o direccion IP del servidor de correo saliente
   */
  void setMailServer(const QString &mS) {
    obj_->setMailServer(mS);
  }

  /**
   Tipo MIME

   Indica el tipo MIME del cuerpo del mensaje

   @param mT Tipo MIME
   */
  void setMimeType(const QString &mT) {
    obj_->setMimeType(mT);
  }

  /**
   Inicia el envío del mensaje
   */
  void startSend() {
    obj_->startSend();
  }

  void emitStatus(const QString &st) {
    emit status(st);
  }

  void emitSendStarted() {
    emit sendStarted();
  }

  void emitSendEnded() {
    emit sendEnded();
  }

  void emitSendTotalSteps(int ts) {
    emit sendTotalSteps(ts);
  }

  void emitSendStepNumber(int sn) {
    emit sendStepNumber(sn);
  }

  FLSmtpClient *obj() {
    return obj_;
  }

signals:

  /**
   Informa del estado del envío
   */
  void status(const QString &);

  /**
   Indica que se ha iniciado el envío del mensaje
   */
  void sendStarted();

  /**
   Indica que se ha finalizado el envío del mensaje
   */
  void sendEnded();

  /**
   Indica los pasos totales que se ejecutarán en el proceso de envío. Usado para diálogos de progreso.
   */
  void sendTotalSteps(int);

  /**
   Indica el número de paso que se va ejecutar. Usado para diálogos de progreso.
   */
  void sendStepNumber(int);

private:

  void connects() const;

  FLSmtpClient *obj_;
};

class  FLImageViewerInterface : public QObject
{

  Q_OBJECT

public:

  /**
   Constructor
   */
  FLImageViewerInterface() : QObject(0) {
    obj_ = new FLImageViewer();
  }

  /**
   Constructor
   */
  FLImageViewerInterface(FLImageViewer *obj) : QObject(obj), obj_(0) {
    obj_ = obj;
  }

  /**
   Destructor
   */
  ~FLImageViewerInterface() {}

public slots:

  bool loadImage(const QString &pathFile) {
    obj_->loadImage(pathFile);
  }

  void openFile() {
    obj_->openFile();
  }

  void show() {
    obj_->show();
  }

  void close() {
    obj_->close();
  }

  QString fileName() {
    return obj_->filename;
  }

  void saveImage(int f) {
    obj_->saveImage(f);
  }

  void savePixmap(int f) {
    obj_->savePixmap(f);
  }

  void setImage(const QVariant &v) {
    obj_->setImage(QImage(v.toCString()));
  }

  void setImage(const QPixmap &p) {
    obj_->setImage(p.convertToImage());
  }

private:

  FLImageViewer *obj_;
};

class  FLSerialPortInterface : public QObject
{
  Q_OBJECT
  Q_ENUMS(BaudRateType DataBitsType ParityType StopBitsType FlowType)

public:

  enum BaudRateType {
    BAUD50, //POSIX ONLY
    BAUD75, //POSIX ONLY
    BAUD110,
    BAUD134, //POSIX ONLY
    BAUD150, //POSIX ONLY
    BAUD200, //POSIX ONLY
    BAUD300,
    BAUD600,
    BAUD1200,
    BAUD1800, //POSIX ONLY
    BAUD2400,
    BAUD4800,
    BAUD9600,
    BAUD14400, //WINDOWS ONLY
    BAUD19200,
    BAUD38400,
    BAUD56000, //WINDOWS ONLY
    BAUD57600,
    BAUD76800, //POSIX ONLY
    BAUD115200,
    BAUD128000, //WINDOWS ONLY
    BAUD256000 //WINDOWS ONLY
  };

  enum DataBitsType {
    DATA_5, //only for compatibility
    DATA_6,
    DATA_7,
    DATA_8
  };

  enum ParityType {
    PAR_NONE,
    PAR_ODD,
    PAR_EVEN,
    PAR_MARK, //WINDOWS ONLY
    PAR_SPACE //WINDOWS directly, POSIX simulated
  };

  enum StopBitsType {
    STOP_1,
    STOP_1_5, //WINDOWS ONLY
    STOP_2
  };

  enum FlowType {
    FLOW_OFF,
    FLOW_HARDWARE,
    FLOW_XONXOFF
  };

  FLSerialPortInterface(const QString &name = QString::null) : QObject(0) {
    obj_ = new QextSerialPort(name);
  }

  ~FLSerialPortInterface() {
    delete obj_;
  }

public slots:

  bool open(int mode = 0) {
    return obj_->open(mode);
  }
  void close() {
    obj_->close();
  }
  void flush() {
    obj_->flush();
  }

  QIODevice::Offset size() const {
    return obj_->size();
  }

  Q_LONG readBlock(char *data, Q_ULONG maxSize) {
    return obj_->readBlock(data, maxSize);
  }
  Q_LONG writeBlock(const QString &data, Q_ULONG size) {
    return obj_->writeBlock(data, size);
  }
  void writeText(const QString &text) {
    writeBlock(text.latin1(), text.length());
  }

  int getch() {
    return obj_->getch();
  }
  int putch(int ch) {
    return obj_->putch(ch);
  }
  int ungetch(int ch) {
    return obj_->ungetch(ch);
  }

  Q_ULONG bytesAvailable() const {
    return obj_->bytesAvailable();
  }

  void setName(const QString &name) {
    obj_->setName(name);
  }
  QString name() {
    return obj_->name();
  }

  void setBaudRate(BaudRateType b) {
    obj_->setBaudRate((QextSerialPort::BaudRateType) b);
  }
  void setDataBits(DataBitsType d) {
    obj_->setDataBits((QextSerialPort::DataBitsType) d);
  }
  void setParity(ParityType p) {
    obj_->setParity((QextSerialPort::ParityType) p);
  }
  void setStopBits(StopBitsType s) {
    obj_->setStopBits((QextSerialPort::StopBitsType) s);
  }
  void setFlowControl(FlowType f) {
    obj_->setFlowControl((QextSerialPort::FlowType) f);
  }
  void setTimeout(int sec, int msec) {
    obj_->setTimeout(sec, msec);
  }

  BaudRateType baudRate() const {
    return (FLSerialPortInterface::BaudRateType) obj_->baudRate();
  }
  DataBitsType setDataBits() const {
    return (FLSerialPortInterface::DataBitsType) obj_->setDataBits();
  }
  ParityType setParity() const {
    return (FLSerialPortInterface::ParityType) obj_->setParity();
  }
  StopBitsType setStopBits() const {
    return (FLSerialPortInterface::StopBitsType) obj_->setStopBits();
  }
  FlowType setFlowControl() const {
    return (FLSerialPortInterface::FlowType) obj_->setFlowControl();
  }

  void setRts(bool set = TRUE) {
    obj_->setRts(set);
  }
  void setDtr(bool set = TRUE) {
    obj_->setDtr(set);
  }

  bool cts() const {
    return obj_->cts();
  }
  bool dsr() const {
    return obj_->dsr();
  }
  bool dcd() const {
    return obj_->dcd();
  }
  bool ri() const {
    return obj_->ri();
  }

private:

  QextSerialPort *obj_;
};

//! Intefaz para QSignalMapper
/**
Ejemplo:

\code
....
class interna {
    var ctx:Object;
    function interna( context ) { this.ctx = context; }
    function init() { this.ctx.interna_init(); }

    var signalMapper;
    function dispatcher(name) { this.ctx.interna_dispatcher(name); }
}
...
...
function interna_init()
{
  var sigMap = this.iface.signalMapper = new AQSignalMapper(this.obj());
  connect(sigMap, "mapped(QString)", this.iface,  "dispatcher()");

  var list = new AQObjectQueryList(this.obj(),
                                   "QPushButton", "", true, true);
  var obj;
  while((obj = list.current()) != undefined) {
    connect(obj, "clicked()", sigMap, "map()");
    sigMap.setMapping(obj, obj.name);
    list.next();
  }
}

function interna_dispatcher(name)
{
  debug ("dispatcher " + name);
  switch (name) {
    case "pbClear":
      ...
      break;
    case "pbFilter":
      ...
      break;
    ...
    ... etc ... etc ... etc ...
  }
}
...
...
\endcode

@author InfoSiAL, S.L.
*/
class AQSignalMapper : public QSignalMapper
{
  Q_OBJECT

public:

  AQSignalMapper(QObject *parent) :
    QSignalMapper(parent) {
    QObject::setName("AQSignalMapper" +
                     QDateTime::currentDateTime().toString("ddMMyyyyhhmmsszzz"));
  }

public slots:

  void setMapping(QObject *sender, int identifier) {
    QSignalMapper::setMapping(sender, identifier);
  }
  void setMapping(QObject *sender, const QString &identifier) {
    QSignalMapper::setMapping(sender, identifier);
  }
  void removeMappings(QObject *sender) {
    QSignalMapper::removeMappings(sender);
  }
};

//! Wrapper para QObject::queryList
/**
Ejemplo:

\code
....
class interna {
    var ctx:Object;
    function interna( context ) { this.ctx = context; }
    function init() { this.ctx.interna_init(); }

    var signalMapper;
    function dispatcher(name) { this.ctx.interna_dispatcher(name); }
}
...
...
function interna_init()
{
  var sigMap = this.iface.signalMapper = new AQSignalMapper(this.obj());
  connect(sigMap, "mapped(QString)", this.iface,  "dispatcher()");

  var list = new AQObjectQueryList(this.obj(),
                                   "QPushButton", "", true, true);
  var obj;
  while((obj = list.current()) != undefined) {
    connect(obj, "clicked()", sigMap, "map()");
    sigMap.setMapping(obj, obj.name);
    list.next();
  }
}

function interna_dispatcher(name)
{
  debug ("dispatcher " + name);
  switch (name) {
    case "pbClear":
      ...
      break;
    case "pbFilter":
      ...
      break;
    ...
    ... etc ... etc ... etc ...
  }
}
...
...
\endcode

@author InfoSiAL, S.L.
*/
class AQObjectQueryList : public QObject
{
  Q_OBJECT

public:

  AQObjectQueryList(QObject *obj,
                    const QString &inheritsClass,
                    const QString &objName,
                    bool regexpMatch,
                    bool recursiveSearch) :
    QObject(0), l_(0) {
    if (obj) {
      l_ = obj->queryList(inheritsClass.isEmpty() ? 0 : inheritsClass.latin1(),
                          objName.isEmpty() ? 0 : objName.latin1(),
                          regexpMatch, recursiveSearch);
      l_->first();
    }
  }

  AQObjectQueryList(QObject *obj) :
    QObject(0), l_(0) {
    if (obj) {
      l_ = obj->queryList();
      l_->first();
    }
  }

  ~AQObjectQueryList() {
    delete l_;
  }

public slots:

  QObject *first() const {
    if (!l_) return 0;
    return l_->first();
  }

  QObject *last() const {
    if (!l_) return 0;
    return l_->last();
  }

  QObject *next() const {
    if (!l_) return 0;
    return l_->next();
  }

  QObject *prev() const {
    if (!l_) return 0;
    return l_->prev();
  }

  QObject *current() const {
    if (!l_) return 0;
    return l_->current();
  }

  bool isValid() const {
    if (!l_) return false;
    return (l_->current() != 0);
  }

private:

  QObjectList *l_;
};

//! Fábrica de objetos para Qt Script
/**
 Fábrica de objetos para Qt Script.

 @author InfoSiAL S.L.
 */
class FLObjectFactory : public QSObjectFactory
{
public:

  /**
  constructor.
  */
  FLObjectFactory();

  /**
  destructor.
  */
  ~FLObjectFactory();

  /**
  Devuelve el objeto interfaz equivalente al objeto especificado

  @param. className. Nombre de la clase del objeto especificado
  @param. arguments. Lista de argumentos para construir el objeto interfaz
  */
  QObject *create(const QString &className, const QSArgumentList &arguments, QObject *);

private:

  FLUtilInterface *util_;
  QObject *voidObject_;
};

#endif
