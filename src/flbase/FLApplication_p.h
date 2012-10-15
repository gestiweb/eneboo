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
   bajo  los  t�rminos  de  la  Licencia  P�blica General de GNU   en  su
   versi�n 2, publicada  por  la  Free  Software Foundation.
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
#include <qlocale.h>
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
class QUrlOperator;
class QNetworkOperation;

/**
Clase aplicaci�n para AbanQ.

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
  Inicializa la aplicaci�n.

  Realiza acciones de inicializaci�n.
  Es indispensable ejecutar este m�todo antes de utilizar las clases de la librer�a flbase.

  Si se indica el nombre de una acci�n s�lo muestra el formulario maestro asociado a esta como
  �nica interfaz, sin el selector de m�dulos para activar otros m�dulos. Esta forma de inicializaci�n
  es �til para aplicaciones que s�lo necesitan un fomulario simple para interactuar, como puede ser
  un tpv o un terminal industrial en una planta de producci�n. Al cerrar este formulario se cierra
  la aplicaci�n.

  @param n    Nombre de la acci�n cuyo formulario maestro se desea que sea la �nica ventana de aplicaci�n
  @param callFunction Nombre de la funci�n que se desea llamar al inicio. Ejemplo; flfactppal.init
  @param arguments  Cadena con los argumentos separados por dos puntos, para la funci�n indicada en callFunction.Ejemplo: 10:cliente2:false
  @param quitAfterCall Si es TRUE sale de la aplicaci�n despu�s de ejecutar la funci�n indicada en callFunction
  */
  virtual void init(const QString &n = QString::null, const QString &callFunction = QString::null,
                    const QString &arguments = QString::null, bool quitAfterCall = false, bool noMax = false);

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
  Ejecuta una funci�n del script en el contexto de un objeto.

  @param function Nombre de la funci�n.
  @param arguments Argumentos de la funci�n.
  @param context Objeto contexto para el script.
  */
  QSArgument call(const QString &function,
                  const QSArgumentList &arguments = QSArgumentList(),
                  QObject *objectContext = 0) const;

  /**
  Ejecuta una funci�n del script en el contexto de un objeto.

  @param function Nombre de la funci�n.
  @param arguments Argumentos de la funci�n.
  @param context Nombre del objeto contexto para el script.
  */
  QSArgument call(const QString &function,
                  const QSArgumentList &arguments = QSArgumentList(),
                  const QString &nameObjectContext = QString::null) const {
    return call(function, arguments, project_->object(nameObjectContext));
  }

  /**
  Establece el t�tulo de la ventana principal.

  El t�tulo de la ventana principal siempre tendr� una parte fija establecida
  autom�ticamente por el motor y se le a�adira el texto que se quiera establecer

  @text Texto a establecer en el t�tulo de la ventana principal
  */
  void setCaptionMainWidget(const QString &text);

  /**
  Obtiene el �ltimo texto establecido con FLApplication::setCaptionMainWidget para el t�tulo de la ventana principal
  */
  QString lastTextCaption() const {
    return lastTextCaption_;
  }

  /**
  Establece la posibilidad de poder salir o no de la aplicaci�n.
  */
  void setNotExit(const bool &b);

  /**
  Establece que la ejecuci�n es en batch, sin interfaz gr�fica de usuario
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
  Obtiene si la ejecuci�n es en batch, sin interfaz gr�fica de usuario
  */
  bool noGUI() const;

  /**
  A�ade c�digo script al objeto sys, para poder ejecutarlo din�micamente.

  @param code C�digo script
  @param scriptEntryFunction Funcion del c�digo del script a ejecutar despues de evaluarlo
  @return TRUE si se a�adi� el c�digo y este no tiene errores.
  */
  bool addSysCode(const QString &code, const QString &scriptEntryFunction = QString::null);

  /**
  Establece el nombre de la funcion de entrada tras evaluar el c�digo
  */
  void setScriptEntryFunction(const QString &scriptEntryFunction);

  /**
  Obtiene la lista de pesta�as que contiene un control QTabWidget

  @param wN Nombre del formulario con el formato [Form|Record]<nombre de la acci�n>
  @param n Nombre del tabWidget

  @return Cadena que contiene la lista con formato nomPesta�a1/descPesta�a1*nomPesta�a2/descPesta�a2*...
  */
  QString getTabWidgetPages(const QString &wN, const QString &n);

  /**
  Obtiene una lista de controles de clase c para el formulario wN

  @param wN Nombre del formulario con el formato [Form|Record]<nombre de la acci�n>
  @param c Nombre de la clase del objeto que se busca

  @return Cadena que contiene la lista con formato nombreObjeto1/descObjeto1*nomObjeto2/descObj2*...
  */
  QString getWidgetList(const QString &wN, const QString &c);

  /**
  @return Base de datos principal
  */
  FLSqlDatabase *db();

  /**
  Para activar/desactivar la detecci�n de posibles bloqueos de una conexi�n a base
  de datos sobre el resto.

  Al activar la detecci�n se inicia un cron�metro que peri�dicamente comprueba bloqueos,
  si existe un bloqueo se emite la se�al databaseLockDetected(), y se muestra un mensaje en el
  caso que se indique hacerlo. Al desactivarla se para este cron�metro y por tanto las
  comprobaciones peri�dicas.

  La detecci�n de bloqueos solo funciona en las bases de datos que lo soportan,
  ver FLSqlDatabase::canDetectLocks().

  @param  on              True activado False desactivado
  @param  msecLapsus      Lapso de tiempo que debe transcurrir entre cada comprobaci�n, por defecto 30 seg.
  @param  limChecks       Numero l�mite de comprobaciones a realizar, al llegar a este l�mite el cron�metro
                          para. Si es -1 no hay limite.
  @param  showWarn        True para mostrar un cuadro de di�logo con el aviso de bloqueo
  @param  msgWarn         Texto a mostrar en el di�logo de aviso, si es vac�o mostrar� uno est�ndar
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
  Para guardar y obtener la geometr�a de los formularios.

  Ver mapGeometryForms_
  */
  void saveGeometryForm(const QString &name, const QRect &geo);
  QRect geometryForm(const QString &name) const;

  /**
  @return La ventana principal de un m�dulo
  */
  QWidget *modMainWidget(const QString &idModulo) const;

signals:

  /**
  Se�al emitida cuando se ha detectado un bloqueo en la conexion a la base de datos
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
  Abre en el MDI el formulario maestro del tipo FLFormDB asociado a una acci�n.

  @param n Nombre de la acci�n asociada al formulario maestro
  @param p Imagen QPixmap que sirve de icono para el formulario
  */
  void openMasterForm(const QString &n, const QPixmap &p);

  /**
  Ejecuta la funci�n "main" del script "scriptform" de una acci�n.

  @param n Nombre de la acci�n
  */
  void execMainScript(const QString &n);

  /**
  Activa un m�dulo dado.

  El activar un m�dulo significa mostrarlo en la ventana principal.
  Si es la primera vez que se activa se inicializa, se ejecuta
  la funci�n "init" de su script principal ([idM].qs) y se guarda un puntero
  a su formulario principal ([idM].ui) en FLApplication::dictMainWidgets.
  La segunda activaci�n y sucesiva ya no se ejecutan las inicializaciones,
  y s�lo se muestra el formulario principal, recuper�ndolo de
  FLApplication::dictMainWidgets.

  @param idM Identificador del m�dulo a activar
  */
  void activateModule(const QString &idM);

  /**
  Inicia/Reinicia la caja de herramientas.
  Tambien reinicializa el men� emergente de m�dulos.
  */
  void initToolBox();

  /**
  Carga todos los scripts de todos los m�dulos instalados.

  Reinicializa el proyecto para scripts descargando todos los
  scripts existentes.
  */
  void loadScripts();

  /**
  Carga todas las traducciones de todos los m�dulos instalados.

  Si una traduccion ya existe para un m�dulo la actualiza.
  */
  void loadTranslations();

  /**
  Traducci�n multiidioma de una cadena texto

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
  Activa/Desactiva la traducci�n din�mica multiidioma

  @param enable   Si es TRUE activa a partir de ese momento la traducci�n din�mica multiidioma.
                  Importante: Todos los mensajes ser�n traducidos mediante el traductor "multilang" y
                  al lenguaje activado, ignorando las traducciones existentes locales.
                  Si es FALSE desactiva la traducci�n din�mica y los mensajes volver�n a traducirse
                  utilizando el traductor para el idioma local.
  @param langid   Identificador del idioma a utilizar para la traducci�n multiidioma. Si 'enable' es FALSE
                  o es vacio este par�metro ser� ignorado, y se seguir� utilizando el �ltimo idioma establecido.
  */
  void setMultiLang(bool enable = true, const QString &langid = QString::null);

  /**
  @return Si la traducci�n din�mica multiidioma est� activada
  */
  bool multiLangEnabled() const {
    return multiLangEnabled_;
  }

  /**
  @return El identificador del idioma actualmente establedico para la traducci�n din�mica multiidioma
  */
  QString multiLangId() const {
    return multiLangId_;
  }

  /**
  Muestra el documento contenido de una p�gina html
  @param url URL de la p�gina html a presentar
  */
  void showDocPage(const QString &url);

  /**
  Reinicializa la aplicaci�n.

  Vuelve a cargar las traducciones, los scripts y reinicializa
  la caja de herramientas.
  */
  virtual void reinit();

  /**
  Abre el entorno integrado de desarrollo de scripts QSA Workbench
  */
  void openQSWorkbench();

  /**
  Evalua el c�digo de todos los scripts del proyecto QSA

  Si se ha establecido una funcion de entrada con setScriptEntryFunction(),
  justo antes de llamar a este m�todo, esta es invocada tras la evaluacion.
  */
  void evaluateProject();

  /**
  Establece el tipo de fuente para la aplicaci�n
  */
  void chooseFont();

  /**
  Muestra el dialogo "Acerca de"
  */
  void aboutAbanQ();
  /**
  Muestra cuadro de dialogo para configurar la carga estatica desde el disco local
  */
  void staticLoaderSetup();

  /** Uso interno */
  QTranslator *createSysTranslator(const QString &lang, bool loadDefault = false);

  /** Uso interno */
  void callScriptEntryFunction();
   
  /**
  Muestra la venta de ayuda con el �ndice
  */
  void helpIndex();
  
  /**
  Muestra la url http://www.eneboo.org
  */
  void urlEneboo();
  
  QLocale localeSystem() const {
    return localeSystem_;
  }
  QChar commaSeparator() const {
    return commaSeparator_;
  }

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
  Guarda el estado del m�dulo activo
  */
  void writeStateModule();

  /**
  Lee el estado del m�dulo activo
  */
  void readStateModule();

  /**
  Filtro de eventos
  */
  bool eventFilter(QObject *obj, QEvent *ev);

protected slots:

  /**
  Lee el estado general
  */
  void readState();

  /**
  Sale de la aplicacion, pidiendo confirmaci�n
  */
  void generalExit();

  /**
  Abre el formulario por defecto para una acci�n.

  El uso normal es conectar la se�al activated() de una accion de FLWidgetApplication.ui
  a este slot. El nombre de esa acci�n debe tener correspondencia en [id_modulo].xml
  */
  void openDefaultForm();

  /**
  Ejecuta el script por defecto para una acci�n.

  El uso normal es conectar la se�al activated() de una accion de [id_modulo].ui
  a este slot. El nombre de esa acci�n debe tener correspondencia en [id_modulo].xml.
  De esta manera se ejecutar� la funci�n "main" del script indicado en la
  etiqueta "scriptform" de la acci�n, dentro de [id_modulo].xml
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
  Establece un stilo para la apariencia de la aplicaci�n
  */
  void makeStyle(const QString &);

  /**
  Muestra el men� emergente de estilos
  */
  void showStyles();

  /**
  Muestra el men� emergente para conmutar barras
  */
  void showToggleBars();

  /**
  Activa el m�dulo asociado a un FLWidgetAction.

  El �nico uso permitido, es conectar la se�al activated() de FLWidgetAction a este slot.
  Invoca a FLApplication::activateModule(cons QString & idM).
  */
  void activateModule();

  /**
  Reinicializa la aplicaci�n.

  Vuelve a cargar las traducciones, los scripts y reinicializa
  la caja de herramientas.
  */
  void reinitP();

  /**
  Realiza comprobacion para detectar bloqueos en la conexion a la base de datos.
  */
  void checkDatabaseLocks(QObject *timer = 0);

  /**
  Uso interno
  */
  void loadModules();
  void exportModules();
  void importModules();
  void updateAbanQ();
  void dumpDatabase();
  void checkForUpdate();
  void checkForUpdateFinish(QNetworkOperation * op);

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
  Carga los scripts asociados a un m�dulo.

  @param idM Identificador del m�dulo
  */
  void loadScriptsFromModule(const QString &idM);

  /**
  Carga la traducci�n asociada a un m�dulo.

  Si una traducci�n ya existe para un m�dulo la actualiza.

  @param idM Identificador del m�dulo
  @param lang Idioma de la traducci�n
  */
  void loadTranslationFromModule(const QString &idM, const QString &lang);

  /**
  Comprueba si una tabla ya est� abierta, mediante su formulario, en el MDI.

  Recorre la lista de formularios del MDI, tambi�n llamado espacio de trabajo (QWorkspace),
  y comprueba si el formulari ya est� abierto, en tal caso devuelve TRUE, y restaura el formulario

  @param id Identificador del  formulario. Corresponde con el nombre de la accion que lo abre
  @return TRUE si el formulario ya est� abierto y FALSE en caso contrario
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
  Bot�n salir
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
  Menu de m�dulos
  */
  QPopupMenu *modulesMenu;

  /**
  Proyecto para scripts
  */
  QSProject *project_;
  QSWorkbench *wb_;

  /**
  Diccionario para almacenar los formularios principales de los m�dulos
  activados.

  En el diccionario se almacenan los punteros a los formularios principales
  de los m�dulos que se van activando, utilizando como palabra clave
  el identificador de m�dulo correspondiente.
  */
  QDict<QWidget> *dictMainWidgets;

  /**
  Tiempo de sesion del usuario
  */
  QDateTime timeUser_;

  /**
  Guarda el �ltimo texto establecido con FLApplication::setCaptionMainWidget para el t�tulo de la ventana principal
  */
  QString lastTextCaption_;

  /**
  Indica que la aplicaci�n s�lo tiene como ventana principal un formulario maestro, ver FLApplication::init()
  */
  bool formAlone_;

  /**
  Indica que no se puede salir de la aplicaci�n por lo m�todos normales.
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
  Indica que la ejecuci�n es en batch, sin interfaz gr�fica de usuario
  */
  bool noGUI_;

  /**
  Almacena el nombre de la funcion de entrada tras evaluar el c�digo
  */
  QString scriptEntryFunction_;

  /**
  Auxiliar para mensajes de aviso emergentes
  **/
  FLPopupWarn *popupWarn_;

  /**
  Diccionario para almacenar la geometr�a de los formularios.

  Este diccionario se guarda en disco, en el fichero de configuraciones, al cerrar la aplicaci�n
  */
  QMap<QString, QRect> mapGeometryForms_;

  /**
  Informacion para la traducci�n din�mica multiidioma
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
  QLocale localeSystem_;
  QChar commaSeparator_;
  QUrlOperator *opCheckUpdate_;
  QString hashUpdate_;
};

#endif
