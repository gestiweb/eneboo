/***************************************************************************
                            FLApplication_p.h
                          -------------------
 begin                : Mon Jul 2 2001
 copyright            : (C) 2001-2005 by InfoSiAL S.L.
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

#ifndef FLAPPLICATION_P_H
#define FLAPPLICATION_P_H

#include <qapplication.h>
#include <qworkspace.h>
#include <qwhatsthis.h>
#include <qtooltip.h>
#include <qmessagebox.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qptrlist.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qaction.h>
#include <qstatusbar.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qsignalmapper.h>
#include <qdict.h>
#include <qstylefactory.h>
#include <qstyle.h>
#include <qfontdialog.h>
#include <qmenubar.h>
#include <qpainter.h>
#include <qimage.h>
#include <qtimer.h>
#include <qdockarea.h>
#include <qdockwindow.h>
#include <qpushbutton.h>
#include <qsinterpreter.h>
#include <qsproject.h>
#include <qsargument.h>
#include <qtoolbox.h>
#include <qsinputdialogfactory.h>
#include <qsutilfactory.h>
#include <qtextcodec.h>
#include <qtextedit.h>
#include <qprinter.h>
#include <qpaintdevicemetrics.h>
#include <qsimplerichtext.h>
#include <qpalette.h>
#include <qmutex.h>
#include <qgroupbox.h>
#include <qnetwork.h>
#include "../qt/include/aqapplication.h"

#define AQ_SET_MNGLOADER                 \
  bool noMngLoader = (mngLoader_ == 0);  \
  if (noMngLoader)                       \
    mngLoader_ = db()->managerModules();

#define AQ_UNSET_MNGLOADER               \
  if (noMngLoader)                       \
    mngLoader_ = 0;

class QSWorkbench;
class FLAccessControlLists;
class FLSqlDatabase;
class FLWorkspace;
class FLPopupWarn;
class FLManagerModules;

/**
Clase aplicación para AbanQ.

@author InfoSiAL S.L.
*/
class FLApplication: public AQ_DECL_APP
{
  Q_OBJECT

public:

  /**
  constructor
  */
  FLApplication(int &argc, char **argv);

  /**
  destructor
  */
  virtual ~FLApplication();

  /**
  Inicializa la aplicación.

  Realiza acciones de inicialización.
  Es indispensable ejecutar este método antes de utilizar las clases de la librería flbase.

  Si se indica el nombre de una acción sólo muestra el formulario maestro asociado a esta como
  única interfaz, sin el selector de módulos para activar otros módulos. Esta forma de inicialización
  es útil para aplicaciones que sólo necesitan un fomulario simple para interactuar, como puede ser
  un tpv o un terminal industrial en una planta de producción. Al cerrar este formulario se cierra
  la aplicación.

  @param n    Nombre de la acción cuyo formulario maestro se desea que sea la única ventana de aplicación
  @param callFunction Nombre de la función que se desea llamar al inicio. Ejemplo; flfactppal.init
  @param arguments  Cadena con los argumentos separados por dos puntos, para la función indicada en callFunction.Ejemplo: 10:cliente2:false
  @param quitAfterCall Si es TRUE sale de la aplicación después de ejecutar la función indicada en callFunction
  */
  virtual void init(const QString &n = QString::null, const QString &callFunction = QString::null,
                    const QString &arguments = QString::null, bool quitAfterCall = false, bool noMax = false);

  virtual int initfcgi();
  virtual void endfcgi();

  virtual QString callfcgi(const QString &callFunction, QStringList arguments);

  /**
  Para obtener el espacio de trabajo
  */
  FLWorkspace *workspace() const;

  /**
  Para establecer el widget pricipal.

  @param w Widget principal
  */
  void setMainWidget(QWidget *w);

  /**
  Para mostrar el widget principal del formulario.

  @param w Widget principal
  */
  void showMainWidget(QWidget *w);

  /**
  Para obtener el proyecto de scripts.

  @return Objeto QSProject correspondiente al proyecto para manejar scripts
  */
  QSProject *project() const {
    return project_;
  }

  /**
  Obtiene el tiempo que inicio sesion el usuario actual.

  @return Regresa el tiempo de inicio de sesion.
  */
  const QDateTime &timeUser() const;

  /**
  Ejecuta una función del script en el contexto de un objeto.

  @param function Nombre de la función.
  @param arguments Argumentos de la función.
  @param context Objeto contexto para el script.
  */
  QSArgument call(const QString &function,
                  const QSArgumentList &arguments = QSArgumentList(),
                  QObject *objectContext = 0) const;

  /**
  Ejecuta una función del script en el contexto de un objeto.

  @param function Nombre de la función.
  @param arguments Argumentos de la función.
  @param context Nombre del objeto contexto para el script.
  */
  QSArgument call(const QString &function,
                  const QSArgumentList &arguments = QSArgumentList(),
                  const QString &nameObjectContext = QString::null) const {
    return call(function, arguments, project_->object(nameObjectContext));
  }

  /**
  Establece el título de la ventana principal.

  El título de la ventana principal siempre tendrá una parte fija establecida
  automáticamente por el motor y se le añadira el texto que se quiera establecer

  @text Texto a establecer en el título de la ventana principal
  */
  void setCaptionMainWidget(const QString &text);

  /**
  Obtiene el último texto establecido con FLApplication::setCaptionMainWidget para el título de la ventana principal
  */
  QString lastTextCaption() const {
    return lastTextCaption_;
  }

  /**
  Establece la posibilidad de poder salir o no de la aplicación.
  */
  void setNotExit(const bool &b);

  /**
  Establece que la ejecución es en batch, sin interfaz gráfica de usuario
  */
  void setNoGUI(const bool &b);

  /**
  Obtiene la lista de control de acceso actualmente instalada.
  */
  FLAccessControlLists *acl() const {
    return acl_;
  }

  /**
  Imprime el contenido de un editor de texto ( QTextEdit ).

  @param editor Objeto QTextEdit del que imprimir su contenido
  */
  void printTextEdit(QTextEdit *editor);

  /**
  Establece la ruta y el nombre del programa a utilizar para imprimir
  */
  void setPrintProgram(const QString &printProgram);

  /**
  Obtiene la ruta y el nombre del programa a utilizar para imprimir
  */
  const QString &printProgram() const;

  /**
  Obtiene si la ejecución es en batch, sin interfaz gráfica de usuario
  */
  bool noGUI() const;

  /**
  Añade código script al objeto sys, para poder ejecutarlo dinámicamente.

  @param code Código script
  @param scriptEntryFunction Funcion del código del script a ejecutar despues de evaluarlo
  @return TRUE si se añadió el código y este no tiene errores.
  */
  bool addSysCode(const QString &code, const QString &scriptEntryFunction = QString::null);

  /**
  Establece el nombre de la funcion de entrada tras evaluar el código
  */
  void setScriptEntryFunction(const QString &scriptEntryFunction);

  /**
  Obtiene la lista de pestañas que contiene un control QTabWidget

  @param wN Nombre del formulario con el formato [Form|Record]<nombre de la acción>
  @param n Nombre del tabWidget

  @return Cadena que contiene la lista con formato nomPestaña1/descPestaña1*nomPestaña2/descPestaña2*...
  */
  QString getTabWidgetPages(const QString &wN, const QString &n);

  /**
  Obtiene una lista de controles de clase c para el formulario wN

  @param wN Nombre del formulario con el formato [Form|Record]<nombre de la acción>
  @param c Nombre de la clase del objeto que se busca

  @return Cadena que contiene la lista con formato nombreObjeto1/descObjeto1*nomObjeto2/descObj2*...
  */
  QString getWidgetList(const QString &wN, const QString &c);

  /**
  @return Base de datos principal
  */
  FLSqlDatabase *db();

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
                                const QString &connectionName = "default");

  /**
  Muestra un mensaje emergente con un aviso.

  @param  msgWarn     Texto a mostrar en el aviso
  @param  scriptCalls Diccionario de funciones de scripts y sus argumentos correspondientes a los
                      hiperenlaces incluidos en el mensaje
  */
  void popupWarn(const QString &msgWarn,
                 const QMap<QString, QSArgumentList> & scriptCalls);

  /**
  Muestra un mensaje emergente con un aviso.

  @param  msgWarn     Texto a mostrar en el aviso
  */
  void popupWarn(const QString &msgWarn);

  /**
  Para guardar y obtener la geometría de los formularios.

  Ver mapGeometryForms_
  */
  void saveGeometryForm(const QString &name, const QRect &geo);
  QRect geometryForm(const QString &name) const;

  /**
  @return La ventana principal de un módulo
  */
  QWidget *modMainWidget(const QString &idModulo) const;

signals:

  /**
  Señal emitida cuando se ha detectado un bloqueo en la conexion a la base de datos
  */
  void databaseLockDetected();

public slots:

  /**
  Para confimar salir de la aplicacion
  */
  bool queryExit();

  /**
  Cambia el mensaje de estado en toda la barra de estado temporalmente
  */
  void statusHelpMsg(const QString &text);

  /**
  Abre en el MDI el formulario maestro del tipo FLFormDB asociado a una acción.

  @param n Nombre de la acción asociada al formulario maestro
  @param p Imagen QPixmap que sirve de icono para el formulario
  */
  void openMasterForm(const QString &n, const QPixmap &p);

  /**
  Ejecuta la función "main" del script "scriptform" de una acción.

  @param n Nombre de la acción
  */
  void execMainScript(const QString &n);

  /**
  Activa un módulo dado.

  El activar un módulo significa mostrarlo en la ventana principal.
  Si es la primera vez que se activa se inicializa, se ejecuta
  la función "init" de su script principal ([idM].qs) y se guarda un puntero
  a su formulario principal ([idM].ui) en FLApplication::dictMainWidgets.
  La segunda activación y sucesiva ya no se ejecutan las inicializaciones,
  y sólo se muestra el formulario principal, recuperándolo de
  FLApplication::dictMainWidgets.

  @param idM Identificador del módulo a activar
  */
  void activateModule(const QString &idM);

  /**
  Inicia/Reinicia la caja de herramientas.
  Tambien reinicializa el menú emergente de módulos.
  */
  void initToolBox();

  /**
  Carga todos los scripts de todos los módulos instalados.

  Reinicializa el proyecto para scripts descargando todos los
  scripts existentes.
  */
  void loadScripts();

  /**
  Carga todas las traducciones de todos los módulos instalados.

  Si una traduccion ya existe para un módulo la actualiza.
  */
  void loadTranslations();

  /**
  Traducción multiidioma de una cadena texto

  @param s  Cadena de texto a traducir
  @param l  Identificador del idioma (ES, EN, IT, CA, etc..) al que traducir
  @return   La cadena de texto traducida al idioma indicado
  */
  QString trMulti(const QString &s, const QString &l);

  /**
  Reimplentacion de QApplication::translate
  */
  virtual QString translate(const char *context, const char *sourceText,
                            const char *comment = 0, Encoding encoding = DefaultCodec) const;

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
  void setMultiLang(bool enable = true, const QString &langid = QString::null);

  /**
  @return Si la traducción dinámica multiidioma está activada
  */
  bool multiLangEnabled() const {
    return multiLangEnabled_;
  }

  /**
  @return El identificador del idioma actualmente establedico para la traducción dinámica multiidioma
  */
  QString multiLangId() const {
    return multiLangId_;
  }

  /**
  Muestra el documento contenido de una página html
  @param url URL de la página html a presentar
  */
  void showDocPage(const QString &url);

  /**
  Reinicializa la aplicación.

  Vuelve a cargar las traducciones, los scripts y reinicializa
  la caja de herramientas.
  */
  virtual void reinit();

  /**
  Abre el entorno integrado de desarrollo de scripts QSA Workbench
  */
  void openQSWorkbench();

  /**
  Evalua el código de todos los scripts del proyecto QSA

  Si se ha establecido una funcion de entrada con setScriptEntryFunction(),
  justo antes de llamar a este método, esta es invocada tras la evaluacion.
  */
  void evaluateProject();

  /**
  Establece el tipo de fuente para la aplicación
  */
  void chooseFont();

  /**
  Muestra el dialogo "Acerca de"
  */
  void aboutAbanQ();

  /** Uso interno */
  QTranslator *createSysTranslator(const QString &lang, bool loadDefault = false);

  /** Uso interno */
  void callScriptEntryFunction();
   
  /**
  Muestra la venta de ayuda con el índice
  */
  void helpIndex();

protected:

  QTranslator *createModTranslator(const QString &idM, const QString &lang,
                                   bool loadDefault = false) const;
  FLManagerModules *mngLoader_;
  QAsciiDict<bool> sysContexts_;
  QTranslator *sysTr_;

  /**
  Guarda el estado general
  */
  void writeState();

  /**
  Guarda el estado del módulo activo
  */
  void writeStateModule();

  /**
  Lee el estado del módulo activo
  */
  void readStateModule();

  /**
  Filtro de eventos
  */
  bool eventFilter(QObject *obj, QEvent *ev);

private slots:

  /**
  Lee el estado general
  */
  void readState();

  /**
  Sale de la aplicacion, pidiendo confirmación
  */
  void generalExit();

  /**
  Abre el formulario por defecto para una acción.

  El uso normal es conectar la señal activated() de una accion de FLWidgetApplication.ui
  a este slot. El nombre de esa acción debe tener correspondencia en [id_modulo].xml
  */
  void openDefaultForm();

  /**
  Ejecuta el script por defecto para una acción.

  El uso normal es conectar la señal activated() de una accion de [id_modulo].ui
  a este slot. El nombre de esa acción debe tener correspondencia en [id_modulo].xml.
  De esta manera se ejecutará la función "main" del script indicado en la
  etiqueta "scriptform" de la acción, dentro de [id_modulo].xml
  */
  void execDefaultScript();

  /**
  Conmuta la barra de herramientas entre visible y oculta
  */
  void toggleToolBar(bool toggle);

  /**
  Conmuta la barra de estado entre visible y oculta
  */
  void toggleStatusBar(bool toggle);

  /**
  Llamado cuando el menu Ventana es activado; regenera el menu ventana con todas
  las ventanas abiertas
  */
  void windowMenuAboutToShow();

  /**
  Activa el hijo MDI que se ha seleccionado en el menu Ventana
  */
  void windowMenuActivated(int id);

  /**
  Cierra a ventana activa en el espacio de trabajo
  */
  void windowClose();

  /**
  Establece un stilo para la apariencia de la aplicación
  */
  void makeStyle(const QString &);

  /**
  Muestra el menú emergente de estilos
  */
  void showStyles();

  /**
  Muestra el menú emergente para conmutar barras
  */
  void showToggleBars();

  /**
  Activa el módulo asociado a un FLWidgetAction.

  El único uso permitido, es conectar la señal activated() de FLWidgetAction a este slot.
  Invoca a FLApplication::activateModule(cons QString & idM).
  */
  void activateModule();

  /**
  Reinicializa la aplicación.

  Vuelve a cargar las traducciones, los scripts y reinicializa
  la caja de herramientas.
  */
  void reinitP();

  /**
  Realiza comprobacion para detectar bloqueos en la conexion a la base de datos.
  */
  void checkDatabaseLocks(QObject *timer = 0);

  /**
  Uso interno.
  Muestra cuadro de dialogo para configurar la carga estatica desde el disco local
  */
  void staticLoaderSetup();

  /**
  Uso interno
  */
  void loadModules();
  void exportModules();
  void importModules();
  void updateAbanQ();

private:

  /**
  Inicia las acciones
  */
  void initActions();

  /**
  Inicia la barra de menus
  */
  void initMenuBar();

  /**
  Inicia la barra de herramientas
  */
  void initToolBar();

  /**
  Inicia la barra de estado
  */
  void initStatusBar();

  /**
  Inicia el espacio de trabajo
  */
  void initView();

  /**
  Inicia el submenu de estilos
  */
  void initStyles();

  /**
  Inicia el widget principal del formulario
  */
  void initMainWidget();

  /**
  Carga los scripts asociados a un módulo.

  @param idM Identificador del módulo
  */
  void loadScriptsFromModule(const QString &idM);

  /**
  Carga la traducción asociada a un módulo.

  Si una traducción ya existe para un módulo la actualiza.

  @param idM Identificador del módulo
  @param lang Idioma de la traducción
  */
  void loadTranslationFromModule(const QString &idM, const QString &lang);

  /**
  Comprueba si una tabla ya está abierta, mediante su formulario, en el MDI.

  Recorre la lista de formularios del MDI, también llamado espacio de trabajo (QWorkspace),
  y comprueba si el formulari ya está abierto, en tal caso devuelve TRUE, y restaura el formulario

  @param id Identificador del  formulario. Corresponde con el nombre de la accion que lo abre
  @return TRUE si el formulario ya está abierto y FALSE en caso contrario
  */
  bool existsFormInMDI(const QString &id);

  /**
  Uso interno
  */
  void clearProject();

protected:

  /**
  Marco MDI frame que engancha los widget hijo MDI, inicializado en FLApplication::initView()
  */
  FLWorkspace *pWorkspace;

  /**
  Widget principal del modulo actualmente activo
  */
  QWidget *mainWidget_;

  /**
  Contenedor principal
  */
  QMainWindow *container;

  /**
  Caja de herramientas
  */
  QToolBox *toolBox;

  /**
  Botón salir
  */
  QPushButton *exitButton;

  /**
  Menu ventana
  */
  QPopupMenu *windowMenu;
  QAction *windowCascadeAction;
  QAction *windowTileAction;
  QAction *windowCloseAction;

  /**
  Menu estilos
  */
  QPopupMenu *style;

  /**
  Menu de barras
  */
  QPopupMenu *toggleBars;

  /**
  Menu de módulos
  */
  QPopupMenu *modulesMenu;

  /**
  Proyecto para scripts
  */
  QSProject *project_;
  QSWorkbench *wb_;

  /**
  Diccionario para almacenar los formularios principales de los módulos
  activados.

  En el diccionario se almacenan los punteros a los formularios principales
  de los módulos que se van activando, utilizando como palabra clave
  el identificador de módulo correspondiente.
  */
  QDict<QWidget> *dictMainWidgets;

  /**
  Tiempo de sesion del usuario
  */
  QDateTime timeUser_;

  /**
  Guarda el último texto establecido con FLApplication::setCaptionMainWidget para el título de la ventana principal
  */
  QString lastTextCaption_;

  /**
  Indica que la aplicación sólo tiene como ventana principal un formulario maestro, ver FLApplication::init()
  */
  bool formAlone_;

  /**
  Indica que no se puede salir de la aplicación por lo métodos normales.
  */
  bool notExit_;

  /**
  Mantiene la lista de control de acceso actualmente instalada
  */
  FLAccessControlLists *acl_;

  /**
  Ruta y nombre del programa a utilizar para imprimir, p.e.: /usr/bin/kprinter
  */
  QString printProgram_;

  /**
  Indica que la ejecución es en batch, sin interfaz gráfica de usuario
  */
  bool noGUI_;

  /**
  Almacena el nombre de la funcion de entrada tras evaluar el código
  */
  QString scriptEntryFunction_;

  /**
  Auxiliar para mensajes de aviso emergentes
  **/
  FLPopupWarn *popupWarn_;

  /**
  Diccionario para almacenar la geometría de los formularios.

  Este diccionario se guarda en disco, en el fichero de configuraciones, al cerrar la aplicación
  */
  QMap<QString, QRect> mapGeometryForms_;

  /**
  Informacion para la traducción dinámica multiidioma
  */
  bool multiLangEnabled_;
  QString multiLangId_;

  /**
  Flag de inicializacion en curso
  */
  bool initializing_;

  /**
  Uso interno
  */
  QSObjectFactory *flFactory_;
};

#endif
