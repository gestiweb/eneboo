/***************************************************************************
AQMainWindow.cpp
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

#include <qsworkbench.h>

#include <QSplashScreen>
#include <QTimer>
#include <QCloseEvent>
#include <QSignalMapper>
#include <QFocusEvent>
#include <QDockWidget>
#include <QTreeWidgetItem>
#include <QFileDialog>

#include <Q3ToolBar>

#include <AQCore/AQCore.h>
#include <AQCore/AQActionMD.h>
#include <AQCore/AQTableMD.h>
#include <AQCore/AQDiskCache.h>
#include <AQSql/AQSqlCursor.h>
#include <AQSql/AQManager.h>
#include <AQSql/AQManagerModules.h>
#include <AQXml/AQXbelReader.h>
#include <AQXml/AQXbelWriter.h>

#include "AQMainWindow.h"
#include "AQTabView.h"
#include "AQFormDB.h"
#include "AQ3MainWindow.h"
#include "AQActionInfo.h"
#include "AQTreeWidget.h"
#include "../AQApplication.h"
#include "../util/AQUiUtil.h"
#include "../component/AQConsole.h"
#include "../manager/AQUiManager.h"

AQMainWindow::AQMainWindow( QWidget * parent, AQSqlDatabase db )
    : QMainWindow( parent ),
    mapperActs( 0 ),
    tabView( 0 ),
    db_( db ),
    consoleCount( 0 ),
    dockModules( 0 ),
    treeModules( 0 )
{}

AQMainWindow::~AQMainWindow()
{
  finish();
}

QIcon AQMainWindow::actionIcon( const QString & idAction ) const
{
  QList<QAction *> list( actions() );
  QAction * a = 0;

  for ( QList<QAction *>::const_iterator it = list.begin(); it != list.end(); ++it ) {
    a = *it;

    if ( a->objectName() == idAction )
      break;
  }

  if ( a )
    return a->icon();

  return QIcon();
}

AQSqlDatabase AQMainWindow::db() const
{
  return db_;
}

AQFormDB * AQMainWindow::activateAction( const QString & idAction )
{
  return activateAction( AQAction::create( idAction, db_.connectionName() ) );
}

AQFormDB * AQMainWindow::activateAction( AQSharedAction aqAction )
{
  Q_ASSERT( aqAction );

  QString idAction( aqAction->action() ->name() );
  QHash<QString, AQActionInfo>::iterator it( actionInfoHash_.find( idAction ) );
  int countActs = ( it != actionInfoHash_.end() ? it.value().count : 0 );

  AQFormDB * form = new AQFormDB( tabView );

  form->setObjectName( aqAction->cursor() ->metadata() ->alias() + QString( "_%1" ).arg( ++countActs ) );
  form->setAQAction( aqAction );
  form->activateMasterUi();
  form->setIgnoreCloseEvent();

  connect( form, SIGNAL( closeRequest( AQFormDB * ) ), this, SLOT( closeForm( AQFormDB * ) ) );

  tabView->insertTab( tabView->count() + 1, form, form->objectName() );

  if ( countActs > 1 ) {
    it.value().widgets.insert( form->objectName(), form );
    it.value().count = countActs;
  } else {
    AQActionInfo actInfo;

    actInfo.widgets.insert( form->objectName(), form );
    actInfo.count = countActs;
    actionInfoHash_.insert( idAction, actInfo );
  }

  tabView->setCurrentWidget( form );
  form->setFocus();

  addRecentItem( idAction );

  if ( actsExecDefault.contains( idAction ) )
    aqWarn( tr ( "execDefaultScript( %1 ) " ).arg( idAction ) );
  if ( actsExecMain.contains( idAction ) )
    aqWarn( tr ( "execMainScript( %1 ) " ).arg( actsExecMain.at( actsExecMain.indexOf( idAction ) ) ) );

  return form;
}

void AQMainWindow::init()
{
  QSplashScreen splash( QPixmap( ":/images/splashabanq.png" ) );
  splash.show();
  AbanQ->processEvents();
  ui.setupUi( this );

  initTabView();
  initMenuStyles();
  initTreeMenuModules();
  initMenuStyleSheets();
  initDocks();
  initViewsMenu();

  statusBar() ->hide();
  show();
  splash.finish( this );
}

bool AQMainWindow::finish()
{
  saveTreeMarks();
  saveTreeRecent();

  QWidgetList list( tabView->widgets() );
  QWidget * w;

  for ( QWidgetList::const_iterator it = list.begin(); it != list.end(); ++it ) {
    w = *it;
    tabView->closeTab( w );
    QTimer::singleShot( 0, w, SLOT( deleteLater() ) );
  }

  return true;
}

void AQMainWindow::initTabView()
{
  tabView = new AQTabView( this );
  tabView->setObjectName( objectName() + "_tabView" );

  connect( tabView, SIGNAL( closeTabClicked() ), this, SLOT( closeTab() ) );
  connect( tabView, SIGNAL( newTabClicked() ), this, SLOT( newTab() ) );

  tabView->setTabPosition( QTabWidget::South );
  tabView->setTabShape( QTabWidget::Triangular );

  QTabBar * tabBar = tabView->findChild<QTabBar *>();

  if ( tabBar ) {
    tabBar->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( tabBar, SIGNAL( customContextMenuRequested( const QPoint& ) ),
             this, SLOT( openTabMenu( const QPoint& ) ) );
  }

  setCentralWidget( tabView );
}

void AQMainWindow::initMenuStyles()
{
  QMenu * menuStyle = ui.menuConfig->addMenu( QIcon( ":/images/estilo.png" ), tr( "&Estilo " ) );
  menuStyle->addActions( AbanQ->agStyles( this ) ->actions() );
}

void AQMainWindow::initMenuStyleSheets()
{
  QMenu * menuStyleSheets = ui.menuConfig->addMenu( QIcon( ":/images/css.png" ), tr( "&Hoja de estilo " ) );
  menuStyleSheets->addActions( AbanQ->agStyleSheets( this ) ->actions() );
}

void AQMainWindow::initTreeMenuModules()
{
  folderIcon.addPixmap( style() ->standardIcon( QStyle::SP_DirClosedIcon ).pixmap( 16, 16 ),
                        QIcon::Normal, QIcon::Off );
  folderIcon.addPixmap( style() ->standardIcon( QStyle::SP_DirOpenIcon ).pixmap( 16, 16 ),
                        QIcon::Normal, QIcon::On );
  defaultIcon.addPixmap( style() ->standardIcon( QStyle::SP_FileIcon ).pixmap( 16, 16 ) );

  treeMarks = new AQTreeWidget( this );
  treeMarks->setObjectName( objectName() + "_treeMarks" );
  treeMarks->headerItem() ->setHidden( true );
  treeMarks->setSelectionMode( QAbstractItemView::ExtendedSelection );
  treeMarks->setDragEnabled( true );
  treeMarks->setAcceptDrops( true );
  treeMarks->setDropIndicatorShown( true );
  treeMarks->setDragDropOverwriteMode( true );
  treeMarks->setContextMenuPolicy( Qt::CustomContextMenu );
  treeMarks->setColumnCount( 2 );
  treeMarks->hideColumn( 1 );

  treeRecent = new AQTreeWidget( this );
  treeRecent->setObjectName( objectName() + "_treeRecent" );
  treeRecent->headerItem() ->setHidden( true );
  treeRecent->setSelectionMode( QAbstractItemView::SingleSelection );
  treeRecent->setDragEnabled( true );
  treeRecent->setAcceptDrops( false );
  treeRecent->setDropIndicatorShown( true );
  treeRecent->setDragDropMode( QAbstractItemView::DragOnly );
  treeRecent->setColumnCount( 2 );
  treeRecent->hideColumn( 1 );

  treeModules = new AQTreeWidget( this );
  treeModules->setObjectName( objectName() + "_treeModules" );
  treeModules->headerItem() ->setHidden( true );
  treeModules->setSelectionMode( QAbstractItemView::SingleSelection );
  treeModules->setDragEnabled( true );
  treeModules->setAcceptDrops( false );
  treeModules->setDropIndicatorShown( true );
  treeModules->setDragDropMode( QAbstractItemView::DragOnly );
  treeModules->setColumnCount( 2 );
  treeModules->hideColumn( 1 );

  mapperActs = new QSignalMapper( this );
  QList<QActionGroup *> list( listAgModules() );
  for ( QList<QActionGroup *>::const_iterator it = list.begin(); it != list.end(); ++it ) {
    QMenu * menuArea = ui.menuAbanQ->addMenu( ( *it ) ->objectName() );
    menuArea->setIcon( folderIcon );
    menuArea->addActions( ( *it ) ->actions() );
    treeItems( treeModules, *it );
  }

  connect( mapperActs, SIGNAL( mapped( const QString & ) ), this,
           SLOT( activateAction( const QString & ) ) );
  connect( treeModules, SIGNAL( itemDoubleClicked ( QTreeWidgetItem *, int ) ),
           this, SLOT( activateItem( QTreeWidgetItem *, int ) ) );
  connect( treeMarks, SIGNAL( itemDoubleClicked ( QTreeWidgetItem *, int ) ),
           this, SLOT( activateItem( QTreeWidgetItem *, int ) ) );
  connect( treeRecent, SIGNAL( itemDoubleClicked ( QTreeWidgetItem *, int ) ),
           this, SLOT( activateItem( QTreeWidgetItem *, int ) ) );
  connect( treeMarks, SIGNAL( customContextMenuRequested( const QPoint& ) ),
           this, SLOT( openTreeMenu( const QPoint& ) ) );

  restoreTreeMarks();
  restoreTreeRecent();

  ui.menuAbanQ->addSeparator();
  ui.menuAbanQ->addAction( ui.actConsole );
  ui.menuAbanQ->addAction( ui.actWorkbench );
  ui.menuAbanQ->addAction( ui.actExit );
}

void AQMainWindow::initDocks()
{
  dockMarks = new QDockWidget( this );
  dockMarks->setObjectName( objectName() + "_dockMarks" );
  dockMarks->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  dockMarks->setWindowTitle( tr( "Marcadores" ) );
  dockMarks->setWidget( treeMarks );
  addDockWidget( Qt::LeftDockWidgetArea, dockMarks );

  dockRecent = new QDockWidget( this );
  dockRecent->setObjectName( objectName() + "_dockRecent" );
  dockRecent->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  dockRecent->setWindowTitle( tr( "Reciente" ) );
  dockRecent->setWidget( treeRecent );
  addDockWidget( Qt::LeftDockWidgetArea, dockRecent );

  dockModules = new QDockWidget( this );
  dockModules->setObjectName( objectName() + "_dockModules" );
  dockModules->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  dockModules->setWindowTitle( tr( "Módulos" ) );
  dockModules->setWidget( treeModules );
  addDockWidget( Qt::LeftDockWidgetArea, dockModules );
}

void AQMainWindow::initViewsMenu()
{
  QMenu * viewsMenu = createPopupMenu();
  if ( viewsMenu ) {
    viewsMenu->setTitle( tr( "&Vistas" ) );
    ui.menubar->insertMenu( ui.menuConfig->menuAction(), viewsMenu );
  }
}

void AQMainWindow::treeItems( AQTreeWidget * parent, QActionGroup * ag )
{
  QTreeWidgetItem * itemAg = new QTreeWidgetItem( parent );
  itemAg->setText( 0, ag->objectName() );
  itemAg->setIcon( 0, folderIcon );
  itemAg->setData( 0, Qt::UserRole, "folder" );
  itemAg->setFlags( itemAg->flags() ^ Qt::ItemIsDragEnabled ^ Qt::ItemIsSelectable );
  treeItems( itemAg, ag->actions() );
}

void AQMainWindow::treeItems( QTreeWidgetItem * parent, QList<QAction *> actions )
{
  QTreeWidgetItem * itemAct;
  for ( QList<QAction *>::const_iterator it = actions.begin(); it != actions.end(); ++it ) {
    itemAct = new QTreeWidgetItem( parent );
    itemAct->setText( 0, ( *it ) ->text().remove( "&" ) );
    itemAct->setFont( 0, ( *it ) ->font() );
    if ( ( *it ) ->menu() ) {
      itemAct->setIcon( 0, folderIcon );
      itemAct->setData( 0, Qt::UserRole, "folder" );
      itemAct->setFlags( itemAct->flags() ^ Qt::ItemIsDragEnabled ^ Qt::ItemIsSelectable );
      treeItems( itemAct, ( *it ) ->menu() ->actions() );
    } else {
      itemAct->setIcon( 0, ( *it ) ->icon() );
      itemAct->setText( 1, ( *it ) ->objectName() );
      itemAct->setData( 0, Qt::UserRole, "bookmark" );
      itemAct->setFlags( itemAct->flags() ^ Qt::ItemIsDropEnabled );
    }
    if ( !( *it ) ->isEnabled() )
      itemAct->setFlags( itemAct->flags() ^ Qt::ItemIsDragEnabled ^ Qt::ItemIsSelectable );
  }
}

void AQMainWindow::closeEvent( QCloseEvent * e )
{
  e->ignore();
  Q_EMIT closeRequest( this );
}

void AQMainWindow::focusInEvent( QFocusEvent * e )
{
  e->ignore();
  if ( tabView && tabView->currentWidget() )
    tabView->currentWidget() ->setFocus();
}

void AQMainWindow::on_actExit_triggered()
{
  Q_EMIT closeRequest( this );
}

void AQMainWindow::on_aboutQt_triggered()
{
  AbanQ->aboutQt();
}

void AQMainWindow::on_aboutAbanQ_triggered()
{
  AbanQ->aboutAbanQ();
}

void AQMainWindow::on_actConsole_triggered()
{
  newConsole();
}

void AQMainWindow::on_actWorkbench_triggered()
{
  db_.scriptEngine() ->workbench() ->open();
}

QActionGroup * AQMainWindow::agModule( const QString & idModule )
{
  if ( idModule.isEmpty() || !mapperActs )
    return 0;

  QActionGroup * ag = new QActionGroup( this );
  QAction * a;
  QStringList actsNames;
  QIcon actIcon;
  AQ3MainWindow * w = static_cast<AQ3MainWindow *>( AQUiManager::createUI( idModule + ".ui", 0, db_ ) );

  if ( w ) {
    QList<QMenu *> listMenus = w->findChildren<QMenu *>();
    for ( QList<QMenu *>::const_iterator it = listMenus.begin(); it != listMenus.end(); ++it ) {
      a = new QAction( ag );
      QFont font = a->font();
      font.setBold( true );
      font.setUnderline( true );
      a->setFont( font );
      a->setEnabled( false );
      a->setIcon( folderIcon );
      a->setText( ( *it ) ->title().remove( "&" ) );
      QList<QAction *> listActions = ( *it ) ->actions();
      for ( QList<QAction *>::const_iterator it2 = listActions.begin(); it2 != listActions.end(); ++it2 ) {
        ( *it2 ) ->trigger();
        if ( !( *it2 ) ->isSeparator() && !actsNames.contains( ( *it2 ) ->objectName() ) ) {
          actIcon = ( *it2 ) ->icon();
          if ( actIcon.isNull() )
            actIcon = defaultIcon;
          a = new QAction( actIcon, ( *it2 ) ->text(), ag );
          a->setObjectName( ( *it2 ) ->objectName() );
          connect( a, SIGNAL( triggered() ), mapperActs, SLOT( map() ) );
          mapperActs->setMapping( a, a->objectName() );
          actsNames << a->objectName();
          addAction( a );
        }
      }
    }

    QList<Q3ToolBar *> listBars = w->findChildren<Q3ToolBar *>();
    for ( QList<Q3ToolBar *>::const_iterator it = listBars.begin(); it != listBars.end(); ++it ) {
      QList<QAction *> listActions = ( *it ) ->actions();
      for ( QList<QAction *>::const_iterator it2 = listActions.begin(); it2 != listActions.end(); ++it2 ) {
        ( *it2 ) ->trigger();
        if ( !( *it2 ) ->isSeparator() && !actsNames.contains( ( *it2 ) ->objectName() ) ) {
          actIcon = ( *it2 ) ->icon();
          if ( actIcon.isNull() )
            actIcon = defaultIcon;
          a = new QAction( actIcon, ( *it2 ) ->text(), ag );
          a->setObjectName( ( *it2 ) ->objectName() );
          connect( a, SIGNAL( triggered() ), mapperActs, SLOT( map() ) );
          mapperActs->setMapping( a, a ->objectName() );
          actsNames << a ->objectName();
          addAction( a );
        }
      }
    }

    actsExecDefault << w->actsExecDefault;
    actsExecMain << w->actsExecMain;
    w->deleteLater();
  }

  return ag;
}

QActionGroup * AQMainWindow::agModules( const QString & idArea )
{
  if ( idArea.isEmpty() )
    return 0;

  QStringList list( db_.managerModules() ->listIdModules( idArea ) );

  if ( list.isEmpty() )
    return 0;

  if ( idArea == "sys" && list.size() == 1 )
    return 0;

  QActionGroup * ag = new QActionGroup( this ), * ag2;
  QAction * a;

  list.sort();
  for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
    if ( *it == "sys" )
      continue;
    a = new QAction( folderIcon, db_.managerModules() ->idModuleToDescription( *it ), ag );
    ag2 = agModule( *it );
    if ( ag2 ) {
      QMenu * actsMenu = new QMenu( this );
      actsMenu->addActions( ag2->actions() );
      a->setMenu( actsMenu );
    }
  }
  return ag;
}

QList<QActionGroup *> AQMainWindow::listAgModules()
{
  QList<QActionGroup *> ret;

  if ( !db_.managerModules() ->isReady() )
    return ret;

  QActionGroup * ag;
  QStringList list( db_.managerModules() ->listIdAreas() );

  list.sort();
  for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
    ag = agModules( *it );
    if ( ag ) {
      ag->setObjectName( db_.managerModules() ->idAreaToDescription( *it ) );
      ret.append( ag );
    }
  }
  return ret;
}

void AQMainWindow::newTab()
{
  openAbanQMenu( QCursor::pos() );
}

void AQMainWindow::newConsole()
{
  AQConsole * con = new AQConsole( tabView, db_ );
  connect( con, SIGNAL( closeRequest( AQConsole * ) ), this, SLOT( closeConsole( AQConsole * ) ) );
  ++consoleCount;
  tabView->insertTab( tabView->count() + 1, con, tr( "Consola_%1" ).arg( consoleCount ) );
  tabView->setCurrentWidget( con );
  con->show();
  con->setFocus();
}

void AQMainWindow::closeConsole( AQConsole * con )
{
  tabView->closeTab( con );
  QTimer::singleShot( 0, con, SLOT( deleteLater() ) );
}

void AQMainWindow::closeForm( AQFormDB * form )
{
  QString actionName( form->aqAction() ->action() ->name() );
  QHash<QString, AQActionInfo>::iterator it = actionInfoHash_.find( actionName );

  if ( it != actionInfoHash_.end() ) {
    it.value().widgets.remove( form->objectName() );
    if ( !it.value().widgets.size() )
      actionInfoHash_.remove( actionName );
  }

  tabView->closeTab( form );
  QTimer::singleShot( 0, form, SLOT( deleteLater() ) );
}

void AQMainWindow::closeTab()
{
  QWidget * w = tabView->currentWidget();
  AQConsole * con = qobject_cast<AQConsole *>( w );

  if ( con ) {
    closeConsole( con );
    return ;
  }

  AQFormDB * form = qobject_cast<AQFormDB *>( w );
  if ( form ) {
    closeForm( form );
    return ;
  }
}

void AQMainWindow::closeTab( QWidget * w )
{
  AQConsole * con = qobject_cast<AQConsole *>( w );
  if ( con ) {
    closeConsole( con );
    return ;
  }

  AQFormDB * form = qobject_cast<AQFormDB *>( w );
  if ( form ) {
    closeForm( form );
    return ;
  }
}

void AQMainWindow::openAbanQMenu( const QPoint & pos )
{
  ui.menuAbanQ->exec( pos );
}

void AQMainWindow::activateItem( QTreeWidgetItem * item, int )
{
  activateAction( item->text( 1 ) );
}

bool actionLessThan( QAction * a1, QAction * a2 )
{
  return a1->text() < a2->text();
}

void AQMainWindow::openTabMenu( const QPoint & pos )
{
  QTabBar * tabBar = tabView->findChild<QTabBar *>();
  int currTabIndex = -1;

  for ( int i = 0; i < tabBar->count(); ++i ) {
    if ( tabBar->tabRect( i ).contains( pos ) ) {
      currTabIndex = i;
      break;
    }
  }

  AQFormDB * form = qobject_cast<AQFormDB *>( tabView->widget( currTabIndex ) );
  if ( form ) {
    QString actionName( form->aqAction() ->action() ->name() );
    QMenu menu( tabBar );
    QAction * a;
    QSignalMapper mapper;
    setCurrentWidget( form );
    QHash<QString, QWidget *> widgets = actionInfoHash_.value( actionName ).widgets;

    if ( widgets.size() > 1 ) {
      QList<QAction *> actions;
      for ( QHash<QString, QWidget *>::const_iterator it = widgets.begin(); it != widgets.end(); ++it ) {
        if ( it.key() == form->objectName() )
          continue;
        a = new QAction( it.key(), &menu );
        connect( a, SIGNAL( triggered() ), &mapper, SLOT( map() ) );
        mapper.setMapping( a, it.value() );
        actions << a;
      }

      qSort( actions.begin(), actions.end(), actionLessThan );

      connect( &mapper, SIGNAL( mapped( QWidget * ) ), this, SLOT( setCurrentWidget( QWidget * ) ) );

      menu.addActions( actions );
      menu.addSeparator();

      a = new QAction( tr( "Cerrar otras pestañas de %1" ).arg( form->aqAction() ->cursor() ->metadata() ->alias() ), &menu );
      connect( a, SIGNAL( triggered() ), this, SLOT( closeOtherTabsOfAction() ) );
      menu.addAction( a );
    }

    if ( tabView->widgets().size() > 1 &&
         tabView->widgets().size() > actionInfoHash_.value( actionName ).widgets.size() ) {
      a = new QAction( tr( "Cerrar las otras pestañas" ), &menu );
      connect( a, SIGNAL( triggered() ), this, SLOT( closeOtherTabs() ) );
      menu.addAction( a );
    }

    a = new QAction( tr( "Cerrar pestaña" ), &menu );
    a->setIcon( QIcon( ":/images/close.png" ) );
    connect( a, SIGNAL( triggered() ), this, SLOT( closeTab() ) );
    menu.addAction( a );
    menu.exec( tabBar->mapToGlobal( pos ) );
  }
}

void AQMainWindow::setCurrentWidget( QWidget * w )
{
  tabView->setCurrentWidget( w );
}

void AQMainWindow::closeOtherTabs()
{
  QWidget * w = tabView->currentWidget();
  QWidgetList widgets = tabView->widgets();

  for ( QWidgetList::const_iterator it = widgets.begin(); it != widgets.end(); ++it ) {
    if ( *it == w )
      continue;
    closeTab( *it );
  }
}

void AQMainWindow::closeOtherTabsOfAction()
{
  AQFormDB * form = qobject_cast<AQFormDB *>( tabView->currentWidget() );

  if ( form ) {
    QHash<QString, QWidget *> widgets =
      actionInfoHash_.value( form->aqAction() ->action() ->name() ).widgets;

    if ( widgets.size() > 1 ) {
      QList<QAction *> actions;
      for ( QHash<QString, QWidget *>::const_iterator it = widgets.begin(); it != widgets.end(); ++it ) {
        if ( it.key() == form->objectName() )
          continue;
        closeTab( it.value() );
      }
    }
  }
}

void AQMainWindow::openTreeMenu( const QPoint & pos )
{
  lastPosTreeMenu = pos;
  QTreeWidgetItem * curItem = treeMarks->itemAt( pos );
  QMenu menu( treeMarks );
  QAction * a;

  a = new QAction( tr( "Añadir" ), &menu );
  a->setMenu( ui.menuAbanQ );
  menu.addAction( a );

  a = new QAction( tr( "Nueva carpeta" ), &menu );
  connect( a, SIGNAL( triggered() ), this, SLOT( newTreeFolder() ) );
  menu.addAction( a );

  if ( curItem ) {
    menu.addSeparator();

    a = new QAction( tr( "Renombrar" ), &menu );
    connect( a, SIGNAL( triggered() ), this, SLOT( renameTreeItem() ) );
    menu.addAction( a );

    a = new QAction( tr( "Eliminar" ), &menu );
    connect( a, SIGNAL( triggered() ), this, SLOT( deleteTreeItem() ) );
    menu.addAction( a );
  }

  menu.addSeparator();

  a = new QAction( tr( "Importar marcadores" ), &menu );
  connect( a, SIGNAL( triggered() ), this, SLOT( importTreeMarks() ) );
  menu.addAction( a );

  if ( treeMarks->topLevelItemCount() ) {
    a = new QAction( tr( "Exportar marcadores" ), &menu );
    connect( a, SIGNAL( triggered() ), this, SLOT( exportTreeMarks() ) );
    menu.addAction( a );

    menu.addSeparator();

    a = new QAction( tr( "Abrir todos" ), &menu );
    connect( a, SIGNAL( triggered() ), this, SLOT( openActionsOfTreeMarks() ) );
    menu.addAction( a );
  }

  disconnect( mapperActs, SIGNAL( mapped( const QString & ) ), this,
              SLOT( activateAction( const QString & ) ) );
  connect( mapperActs, SIGNAL( mapped( const QString & ) ), this,
           SLOT( addItemFromAction( const QString & ) ) );

  menu.exec( treeMarks->viewport() ->mapToGlobal( pos ) );

  disconnect( mapperActs, SIGNAL( mapped( const QString & ) ), this,
              SLOT( addItemFromAction( const QString & ) ) );
  connect( mapperActs, SIGNAL( mapped( const QString & ) ), this,
           SLOT( activateAction( const QString & ) ) );
}

void AQMainWindow::newTreeFolder()
{
  QTreeWidgetItem * curItem = treeMarks->itemAt( lastPosTreeMenu );

  if ( curItem && !curItem->flags().testFlag( Qt::ItemIsDropEnabled ) )
    curItem = curItem->parent();

  QTreeWidgetItem * item = new QTreeWidgetItem( curItem );

  item->setText( 0, tr( "Nueva carpeta" ) );
  item->setIcon( 0, folderIcon );
  item->setData( 0, Qt::UserRole, "folder" );
  item->setFlags( item->flags() | Qt::ItemIsEditable );

  if ( !curItem )
    treeMarks->addTopLevelItem( item );

  treeMarks->setCurrentItem( item );
  treeMarks->editItem( item );
}

void AQMainWindow::renameTreeItem()
{
  QTreeWidgetItem * item = treeMarks->currentItem();
  if ( item ) {
    Qt::ItemFlags flagsBack = item->flags();
    item->setFlags( item->flags() | Qt::ItemIsEditable );
    treeMarks->editItem( item );
    item->setFlags( flagsBack );
  }
}

void AQMainWindow::deleteTreeItem()
{
  QTreeWidgetItem * item = treeMarks->currentItem();

  if ( item && AQ_Yes == AQ_MSGBOX_QUES( tr( "Borrar Marcador" ), tr( "¿ Está seguro ?" ) ) ) {
    QTreeWidgetItem * itemParent = static_cast<QTreeWidgetItem *>( item->parent() );

    if ( !itemParent )
      treeMarks->takeTopLevelItem( treeMarks->indexOfTopLevelItem( item ) );
    else
      itemParent->removeChild( item );

    delete item;
  }
}

void AQMainWindow::addItemFromAction( const QString & idAction )
{
  QList<QAction *> list = actions();
  QAction * a = 0;

  for ( QList<QAction *>::const_iterator it = list.begin(); it != list.end(); ++it ) {
    if ( ( *it ) ->objectName() == idAction ) {
      a = *it;
      break;
    }
  }

  if ( a ) {
    QTreeWidgetItem * curItem = treeMarks->itemAt( lastPosTreeMenu );

    if ( curItem && !curItem->flags().testFlag( Qt::ItemIsDropEnabled ) )
      curItem = curItem->parent();

    QTreeWidgetItem * itemAct = new QTreeWidgetItem( curItem );

    itemAct->setText( 0, a->text().remove( "&" ) );
    itemAct->setIcon( 0, a->icon() );
    itemAct->setFont( 0, a->font() );
    itemAct->setText( 1, a->objectName() );
    itemAct->setData( 0, Qt::UserRole, "bookmark" );
    itemAct->setFlags( itemAct->flags() ^ Qt::ItemIsDropEnabled );

    if ( !curItem )
      treeMarks->addTopLevelItem( itemAct );

    treeMarks->setCurrentItem( itemAct );
  }
}

void AQMainWindow::addRecentItem( const QString & idAction )
{
  QList<QAction *> list = actions();
  QAction * a = 0;

  for ( QList<QAction *>::const_iterator it = list.begin(); it != list.end(); ++it ) {
    if ( ( *it ) ->objectName() == idAction ) {
      a = *it;
      break;
    }
  }

  if ( a ) {
    QTreeWidgetItemIterator it( treeRecent );

    while ( *it ) {
      if ( ( *it ) ->text( 1 ) == a->objectName() ) {
        delete ( treeRecent->takeTopLevelItem( treeRecent->indexOfTopLevelItem( *it ) ) );
        break;
      }
      ++it;
    }

    QTreeWidgetItem * itemAct = new QTreeWidgetItem;

    itemAct->setText( 0, a->text().remove( "&" ) );
    itemAct->setIcon( 0, a->icon() );
    itemAct->setFont( 0, a->font() );
    itemAct->setText( 1, a->objectName() );
    itemAct->setData( 0, Qt::UserRole, "bookmark" );
    treeRecent->insertTopLevelItem( 0, itemAct );

    if ( treeRecent->topLevelItemCount() >= 10 )
      delete ( treeRecent->takeTopLevelItem( 10 ) );

    treeRecent->setCurrentItem( itemAct );
  }
}

void AQMainWindow::saveTreeMarks()
{
  QString path( AQ_DISKCACHE_DIRPATH() + "/bookmarks/" + db_.database() + "/" );
  QString filename( path + treeMarks->objectName() + ".xbel" );
  QDir d( path );

  if ( !d.exists() && !d.mkpath( path ) )
    return ;

  exportTree( treeMarks, filename );
}

void AQMainWindow::restoreTreeMarks()
{
  QString path( AQ_DISKCACHE_DIRPATH() + "/bookmarks/" + db_.database() + "/" );
  QString filename( path + treeMarks->objectName() + ".xbel" );
  QDir d( path );

  if ( !d.exists() && !d.mkpath( path ) )
    return ;

  treeMarks->clear();
  importTree( treeMarks, filename );
}

void AQMainWindow::saveTreeRecent()
{
  QString path( AQ_DISKCACHE_DIRPATH() + "/bookmarks/" + db_.database() + "/" );
  QString filename( path + treeRecent->objectName() + ".xbel" );
  QDir d( path );

  if ( !d.exists() && !d.mkpath( path ) )
    return ;

  exportTree( treeRecent, filename );
}

void AQMainWindow::restoreTreeRecent()
{
  QString path( AQ_DISKCACHE_DIRPATH() + "/bookmarks/" + db_.database() + "/" );
  QString filename( path + treeRecent->objectName() + ".xbel" );
  QDir d( path );

  if ( !d.exists() && !d.mkpath( path ) )
    return ;

  treeRecent->clear();
  importTree( treeRecent, filename );
}

void AQMainWindow::exportTreeMarks()
{
  exportTree( treeMarks );
}

void AQMainWindow::importTreeMarks()
{
  if ( AQ_Yes == AQ_MSGBOX_QUES( tr( "Importar Marcadores" ),
                                 tr( "Esta acción borrará los marcadores actuales\ny "
                                     "los sustituirá por los importados\n¿ Desea continuar ?" ) ) )
    importTree( treeMarks );
}

void AQMainWindow::exportTree( AQTreeWidget * tree, const QString & filename )
{
  QString fileName( filename );

  if ( fileName.isEmpty() )
    fileName = QFileDialog::getSaveFileName( this, tr( "Guardar Fichero Bookmark" ),
               QDir::currentPath(),
               tr( "Ficheros XBEL(*.xbel *.xml)" ) );

  if ( fileName.isEmpty() )
    return ;

  QFile file( fileName );

  file.remove();

  if ( !file.open( QFile::WriteOnly | QFile::Text ) ) {
    AQ_MSGBOX_WARN( tr( "Bookmarks" ),
                    tr( "No puedo escribir fichero %1:\n%2." )
                    .arg( fileName )
                    .arg( file.errorString() ) );
    return ;
  }

  AQXbelWriter writer( tree );
  writer.writeFile( &file );
}

void AQMainWindow::importTree( AQTreeWidget * tree, const QString & filename )
{
  QString fileName( filename );

  if ( fileName.isEmpty() )
    fileName = QFileDialog::getOpenFileName( this, tr( "Abrir Fichero Bookmark" ),
               QDir::currentPath(),
               tr( "Ficheros XBEL(*.xbel *.xml)" ) );

  if ( fileName.isEmpty() )
    return ;

  QFile file( fileName );

  if ( !file.exists() )
    return ;

  if ( !file.open( QFile::ReadOnly | QFile::Text ) ) {
    AQ_MSGBOX_WARN( tr( "Bookmarks" ),
                    tr( "No puedo leer el fichero %1:\n%2." )
                    .arg( fileName )
                    .arg( file.errorString() ) );
    return ;
  }

  AQXbelReader reader( tree, this );

  if ( !reader.read( &file ) ) {
    AQ_MSGBOX_WARN( tr( "Bookmarks" ),
                    tr( "Error en fichero %1 en linea %2, columna %3:\n%4" )
                    .arg( fileName )
                    .arg( reader.lineNumber() )
                    .arg( reader.columnNumber() )
                    .arg( reader.errorString() ) );
  }
}

void AQMainWindow::openActionsOfTreeMarks()
{
  QTreeWidgetItemIterator it( treeMarks );
  while ( *it ) {
    activateAction( ( *it ) ->text( 1 ) );
    ++it;
  }
}
