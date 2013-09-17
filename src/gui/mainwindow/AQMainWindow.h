/***************************************************************************
AQMainWindow.h
-------------------
begin                : 26/08/2007
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

#ifndef AQMAINWINDOW_H_
#define AQMAINWINDOW_H_

#include <uic/ui_AQMainWindow.h>

#include <AQSql/AQSqlDatabase.h>
#include <AQUi/AQAction.h>

#include <QHash>

class AQTabView;
class AQConsole;
class AQFormDB;
class AQActionInfo;
class AQTreeWidget;
class QTreeWidgetItem;
class QSignalMapper;

/**
Ventanas principales de AbanQ
 
Una ventana principal está asociada a la conexión de una
base de datos. Es el contenedor principal donde se muestran
los componentes de la interfaz de usuario.
 
Ofrece varios métodos para manejar la acciones de
AbanQ (AQAction), menús, formularios, etc..
 
@author InfoSiAL S.L.
*/
class AQMainWindow : public QMainWindow
{
  Q_OBJECT

public:

  /**
  Constructor

  @param parent Ventana padre
  @param db     Base de datos
  */
  AQMainWindow( QWidget * parent, AQSqlDatabase db );

  /**
  Destructor
  */
  ~AQMainWindow();

  /**
  @return Icono de la acción indicada. Puede ser nulo si la acción
          no está definida en la interfaz de usuario.
  */
  QIcon actionIcon( const QString & idAction ) const;

  /**
  @return Base de datos de la ventan principal
  */
  AQSqlDatabase db() const;

protected:

  /**
  Captura de eventos
  */
  void closeEvent( QCloseEvent * e );
  void focusInEvent ( QFocusEvent * e );

Q_SIGNALS:

  /**
  Señal emitida cuando la ventana solicita ser cerrada. El objeto que la
  contiene, generalmente del tipo (AQApplication), es quien realiza la accion
  de cerrarla
  */
  void closeRequest( AQMainWindow * win );

public Q_SLOTS:

  /**
  Activa la acción indicada abriendo el formulario asociado en una nueva pestaña

  @param idAction Nombre de la accion a activar. A partir de él construye el objeto 
                  AQAction correspondiente
  @return El formulario abierto. La ventana principal mantiene la propiedad sobre el
          formulario y puede ser invalidado en cualquier momento.
  */
  AQFormDB * activateAction( const QString & idAction );

  /**
  Hace lo mismo que el método anterior pero acepta un objeto AQAction ya construido
  en vez del nombre de la acción.
  */
  AQFormDB * activateAction( AQSharedAction aqAction );

private Q_SLOTS:

  /**
  Uso interno
  */
  void init();
  bool finish();
  void newTab();
  void closeConsole( AQConsole * con );
  void closeForm( AQFormDB * form );
  void closeTab();
  void closeTab( QWidget * w );
  void closeOtherTabs();
  void closeOtherTabsOfAction();
  void openAbanQMenu( const QPoint & pos );
  void openTabMenu( const QPoint & pos );
  void openTreeMenu( const QPoint & pos );
  void setCurrentWidget( QWidget * w );
  void newTreeFolder();
  void renameTreeItem();
  void deleteTreeItem();
  void addItemFromAction( const QString & idAction );
  void addRecentItem( const QString & idAction );
  void saveTreeMarks();
  void restoreTreeMarks();
  void saveTreeRecent();
  void restoreTreeRecent();
  void exportTreeMarks();
  void importTreeMarks();
  void exportTree( AQTreeWidget * tree, const QString & filename = QString() );
  void importTree( AQTreeWidget * tree, const QString & filename = QString() );
  void openActionsOfTreeMarks();

  void on_actExit_triggered();
  void on_aboutQt_triggered();
  void on_aboutAbanQ_triggered();
  void on_actConsole_triggered();
  void on_actWorkbench_triggered();
  void activateItem( QTreeWidgetItem * item, int );

private:

  Q_DISABLE_COPY( AQMainWindow )

  friend class AQApplication;

  /**
  Uso interno
  */
  QActionGroup * agModule( const QString & idModule );
  QActionGroup * agModules( const QString & idArea );
  QList<QActionGroup *> listAgModules();
  void treeItems( AQTreeWidget * parent, QActionGroup * ag );
  void treeItems( QTreeWidgetItem * parent, QList<QAction *> actions );

  void newConsole();
  void initTabView();
  void initMenuStyles();
  void initTreeMenuModules();
  void initMenuStyleSheets();
  void initDocks();
  void initViewsMenu();

  Ui::AQMainWindow ui;
  QSignalMapper * mapperActs;
  AQTabView * tabView;
  AQSqlDatabase db_;
  QStringList actsExecDefault;
  QStringList actsExecMain;
  int consoleCount;
  QDockWidget * dockModules;
  AQTreeWidget * treeModules;
  QDockWidget * dockMarks;
  AQTreeWidget * treeMarks;
  QDockWidget * dockRecent;
  AQTreeWidget * treeRecent;
  QIcon folderIcon;
  QIcon defaultIcon;
  QHash<QString, AQActionInfo> actionInfoHash_;
  QPoint lastPosTreeMenu;
};

#endif /*AQMAINWINDOW_H_*/
