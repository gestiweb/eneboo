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

#include <WBorderLayout>
#include <WFitLayout>
#include <WAccordionLayout>
#include <WTree>
#include <WTreeNode>

#include <Ext/Panel>
#include <Ext/ToolBar>
#include <Ext/TabWidget>
#include <Ext/Button>

#include <QSignalMapper>
#include <QStyle>
#include <QMenu>

#include <AQUi/AQUiUtil.h>
#include <AQUi/AQUiManager.h>
#include <AQUi/AQInvokerThreadContext.h>
#include <AQUi/AQFormDB.h>
#include <AQUi/AQAction.h>
#include <AQCore/AQCoreUtil.h>
#include <AQCore/AQActionMD.h>
#include <AQCore/AQTableMD.h>
#include <AQCore/AQDiskCache.h>
#include <AQSql/AQSqlCursor.h>
#include <AQSql/AQManager.h>
#include <AQSql/AQManagerModules.h>
#include <AQXml/AQXbelReader.h>
#include <AQXml/AQXbelWriter.h>

#include "../../gui/mainwindow/AQ3MainWindow.h"
#include "../wtqt/WQApplication.h"
#include "../webgets/WQMenu.h"
#include "../webgets/WQTree.h"
#include "../webgets/WQAction.h"
#include "../AQApplication.h"
#include "AQTabView.h"
#include "AQMainWindow.h"
#include "AQMainWindow_p.h"

AQMainWindowPrivate::AQMainWindowPrivate() :
    WQWidgetPrivate()
{}

AQMainWindowPrivate::~AQMainWindowPrivate()
{}

void AQMainWindowPrivate::setupUi()
{
  ww_ = new Ext::Panel;
  lay_ = new WBorderLayout;
  ww_->setBorder( false );
  ww_->setLayout( lay_ );
}

void AQMainWindowPrivate::initTabView()
{
  AQ_Q( AQMainWindow );
  tabView_ = new AQTabView( q, false, false );
  tabView_->QObject::setObjectName( q->QObject::objectName() + "_tabView" );
  q->addWQWidget( tabView_ );
}

void AQMainWindowPrivate::initDocks()
{
  Ext::Panel * dockView = new Ext::Panel;
  WAccordionLayout * ly = new WAccordionLayout;
  dockView->setLayout( ly );
  dockView->resize( 200, WLength() );
  dockView->setResizable( true );
  dockView->setBorder( false );
  dockView->setCollapsible( true );
  dockView->setAnimate( true );

  dockModules_ = new Ext::Panel;
  dockModules_->setLayout( new WFitLayout );
  dockModules_->setTitle( toWString( AQCoreUtil::translate( "AQMainWindow", "Módulos" ) ) );
  dockModules_->setBorder( false );
  dockModules_->setAutoScrollBars( true );
  ly->addWidget( dockModules_ );

  dockMarks_ = new Ext::Panel;
  dockMarks_->setLayout( new WFitLayout );
  dockMarks_->setTitle( toWString( AQCoreUtil::translate( "AQMainWindow", "Marcadores" ) ) );
  dockMarks_->setBorder( false );
  dockMarks_->setAutoScrollBars( true );
  ly->addWidget( dockMarks_ );

  dockRecent_ = new Ext::Panel;
  dockRecent_->setLayout( new WFitLayout );
  dockRecent_->setTitle( toWString( AQCoreUtil::translate( "AQMainWindow", "Reciente" ) ) );
  dockRecent_->setBorder( false );
  dockRecent_->setAutoScrollBars( true );
  ly->addWidget( dockRecent_ );

  lay_->addWidget( dockView, WBorderLayout::West );
}

void AQMainWindowPrivate::initTreeMenuModules()
{
  AQ_Q( AQMainWindow );

  menuAbanQ_ = new WQMenu( "AbanQ", q );
  menuHelp_ = new WQMenu( q->tr( "Ayuda" ), q );
  treeModules_ = new WQTree( q );
  treeRecent_ = new WQTree( q );
  Ext::ToolBar * tb = new Ext::ToolBar;
  WTree * tree = new WTree;

  QList<QActionGroup *> list( listAgModules() );
  for ( QList<QActionGroup *>::const_iterator it = list.begin(); it != list.end(); ++it ) {
    WQMenu * menuArea = menuAbanQ_->addMenu( folderIcon_, ( *it ) ->objectName() );
    menuArea->addActions( ( *it ) ->actions() );

    WQTree * treeArea = treeModules_->addTree( folderIcon_, ( *it ) ->objectName() );
    treeArea->addActions( ( *it ) ->actions() );
  }

  menuAbanQ_->addSeparator();
#ifdef AQ_DEBUG

  menuAbanQ_->addAction( actTests_ );
#endif

  menuAbanQ_->addAction( actExit_ );
  tb->addButton( "AbanQ", menuAbanQ_->wmenu() );

  menuHelp_->addAction( actAboutAbanQ_ );
  tb->addButton( toWString( q->tr( "Ayuda" ) ), menuHelp_->wmenu() );

  ww_->setTopToolBar( tb );

  WTreeNode * root = treeModules_->wtree();
  root->setNodeVisible( false );
  tree->setTreeRoot( root );
  tree->setStyleClass( "table" );
  dockModules_->layout() ->addWidget( tree );
  root->expand();

  root = treeRecent_->wtree();
  root->setNodeVisible( false );
  tree = new WTree;
  tree->setTreeRoot( root );
  tree->setStyleClass( "table" );
  dockRecent_->layout() ->addWidget( tree );

  AQ_WUI_CONNECT( menuAbanQ_->triggered, q, AQMainWindow, activateAction );
  AQ_WUI_CONNECT( menuHelp_->triggered, q, AQMainWindow, activateAction );
  AQ_WUI_CONNECT( treeModules_->triggered, q, AQMainWindow, activateAction );
  AQ_WUI_CONNECT( treeRecent_->triggered, q, AQMainWindow, activateAction );
}

void AQMainWindowPrivate::initGuiObjects()
{
  folderIcon_ = QIcon( ":/images/folder.png" );
  defaultIcon_ = QIcon( ":/images/icon.png" );
  actExit_->setIcon( QIcon( ":/images/exit.png" ) );
  actAboutAbanQ_->setIcon( defaultIcon_ );
#ifdef AQ_DEBUG

  actTests_->setIcon( QIcon( ":/images/icon.png" ) );
#endif
}

QActionGroup * AQMainWindowPrivate::agModule( const QString & idModule )
{
  if ( idModule.isEmpty() )
    return 0;

  AQ_Q( AQMainWindow );

  QActionGroup * ag = new QActionGroup( q );
  QAction * a;
  QStringList actsNames;
  QIcon actIcon;
  AQ3MainWindow * w = static_cast<AQ3MainWindow *> ( AQUiManager::createUI( idModule + ".ui", 0, db_ ) );

  if ( w ) {

    QList<QMenu *> listMenus = w->findChildren<QMenu *> ();

    for ( QList<QMenu *>::const_iterator it = listMenus.begin(); it != listMenus.end(); ++it ) {

      a = new QAction( ag );
      a->setEnabled( false );
      a->setIcon( folderIcon_ );
      a->setText( ( *it ) ->title().remove( "&" ) );
      QList<QAction *> listActions = ( *it ) ->actions();

      for ( QList<QAction *>::const_iterator it2 = listActions.begin(); it2 != listActions.end(); ++it2 ) {

        ( *it2 ) ->trigger();

        if ( !( *it2 ) ->isSeparator() && !actsNames.contains( ( *it2 ) ->objectName() ) ) {

          actIcon = ( *it2 ) ->icon();

          if ( actIcon.isNull() )
            actIcon = defaultIcon_;

          a = new QAction( actIcon, ( *it2 ) ->text(), ag );
          a->setObjectName( ( *it2 ) ->objectName() );
          actsNames << a->objectName();
          q->addAction( a );
        }
      }
    }

    QList<Q3ToolBar *> listBars = w->findChildren<Q3ToolBar *> ();

    for ( QList<Q3ToolBar *>::const_iterator it = listBars.begin(); it != listBars.end(); ++it ) {

      QList<QAction *> listActions = ( *it ) ->actions();

      for ( QList<QAction *>::const_iterator it2 = listActions.begin(); it2 != listActions.end(); ++it2 ) {

        ( *it2 ) ->trigger();

        if ( !( *it2 ) ->isSeparator() && !actsNames.contains( ( *it2 ) ->objectName() ) ) {

          actIcon = ( *it2 ) ->icon();

          if ( actIcon.isNull() )
            actIcon = defaultIcon_;

          a = new QAction( actIcon, ( *it2 ) ->text(), ag );
          a->setObjectName( ( *it2 ) ->objectName() );
          actsNames << a->objectName();
          q->addAction( a );
        }
      }
    }

    actsExecDefault_ << w->actsExecDefault;
    actsExecMain_ << w->actsExecMain;
    delete w;
  }

  return ag;
}

QActionGroup * AQMainWindowPrivate::agModules( const QString & idArea )
{
  if ( idArea.isEmpty() )
    return 0;

  QStringList list( db_.managerModules() ->listIdModules( idArea ) );

  if ( list.isEmpty() )
    return 0;

  if ( idArea == "sys" && list.size() == 1 )
    return 0;

  AQ_Q( AQMainWindow );

  QActionGroup * ag = new QActionGroup( q ), *ag2;
  WQAction * a;

  list.sort();
  for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
    if ( *it == "sys" )
      continue;
    a = new WQAction( folderIcon_, db_.managerModules() ->idModuleToDescription( *it ), ag );
    ag2 = agModule( *it );
    if ( ag2 ) {
      WQMenu * actsMenu = new WQMenu( q );
      actsMenu->addActions( ag2->actions() );
      a->setWMenu( actsMenu );

      WQTree * actsTree = new WQTree( q );
      actsTree->addActions( ag2->actions() );
      a->setWTree( actsTree );
    }
  }
  return ag;
}

QList<QActionGroup *> AQMainWindowPrivate::listAgModules()
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

void AQMainWindowPrivate::newConsole()
{
  //    AQConsole * con = new AQConsole( tabView_, db_ );
  //    connect( con, SIGNAL( closeRequest( AQConsole * ) ), this, SLOT( closeConsole( AQConsole * ) ) );
  //    ++consoleCount;
  //    tabView_->insertTab( tabView_->count() + 1, con, tr( "Consola_%1" ).arg( consoleCount ) );
  //    tabView_->setCurrentWidget( con );
  //    con->show();
  //    con->setFocus();
}

void AQMainWindowPrivate::tests()
{
  AQUiUtil util;

  util.createProgressDialog( "En bucle de progreso: Paso : 0", 300, "Prueba" );

  for ( uint p = 1; p < 300; ++p ) {
    util.setLabelText( QString( "En bucle de progreso: Paso : %1" ).arg( p ) );
    util.setProgress( p );
    for ( int i = 0; i < 1500; ++i ) {}
  }

  util.destroyProgressDialog();
}

AQMainWindow::AQMainWindow( WQWidget * parent ) :
    WQWidget( *new AQMainWindowPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( AQMainWindow );
  d->tabView_ = 0;
  d->db_ = AQSqlDatabase();
  d->consoleCount_ = 0;
  d->setupUi();
}

AQMainWindow::AQMainWindow( WQWidget * parent, AQSqlDatabase db ) :
    WQWidget( *new AQMainWindowPrivate, static_cast<WObject *> ( parent ) )
{
  AQ_D( AQMainWindow );
  d->tabView_ = 0;
  d->db_ = db;
  d->consoleCount_ = 0;
  d->setupUi();
}

WWidget * AQMainWindow::ww()
{
  AQ_D( AQMainWindow );
  return d->ww_;
}

void AQMainWindow::addWQWidget( WQWidget * w )
{
  if ( !w )
    return ;

  if ( !w->isWidget() ) {
    aqWarn( "AQMainWindow::addWQWidget !w->isWidget " << this << w );
    return ;
  }

  AQ_D( AQMainWindow );
  d->lay_->addWidget( w->ww(), WBorderLayout::Center );
}

QIcon AQMainWindow::actionIcon( const QString & idAction ) const
{
  QList<QAction *> list( actions() );
  QAction * a = 0;
  for ( QList<QAction *>::const_iterator it = list.begin(); it != list.end(); ++it ) {
    if ( ( *it ) ->objectName() == idAction ) {
      a = *it;
      break;
    }
  }
  if ( a )
    return a->icon();
  return QIcon();
}

const AQSqlDatabase & AQMainWindow::db() const
{
  AQ_D( const AQMainWindow );
  return d->db_;
}

void AQMainWindow::init()
{
  AQ_D( AQMainWindow );

#ifdef AQ_DEBUG

  d->actTests_ = new QAction( this );
  d->actTests_->setObjectName( "actTests" );
  d->actTests_->setText( tr( "Tests" ) );
#endif

  d->actExit_ = new QAction( this );
  d->actExit_->setObjectName( "actExit" );
  d->actExit_->setText( tr( "Salir" ) );

  d->actAboutAbanQ_ = new QAction( this );
  d->actAboutAbanQ_->setObjectName( "actAboutAbanQ" );
  d->actAboutAbanQ_->setText( tr( "Acerca de AbanQ" ) );

  AQ_INVOKER<AQMainWindowPrivate> ivk;
  ivk.invoke( d, &AQMainWindowPrivate::initGuiObjects );

  d->initDocks();
  d->initTreeMenuModules();
  d->initTabView();
}

void AQMainWindow::finish()
{
  AQ_D( AQMainWindow );

  saveTreeMarks();
  saveTreeRecent();

  if ( d->tabView_ ) {
    WQWidgetList list( d->tabView_->widgets() );
    for ( WQWidgetList::const_iterator it = list.begin(); it != list.end(); ++it ) {
      d->tabView_->closeTab( *it );
      delete ( *it );
    }
  }
}

void AQMainWindow::closeTab()
{}

void AQMainWindow::closeTab( WQWidget * )
{}

AQFormDB * AQMainWindowPrivate::createForm( const QString & idAction )
{
  AQSharedAction aqAction = AQAction::create( idAction, db_.connectionName() );

  Q_ASSERT( aqAction );

  aqAction->cursor() ->select( "" );
  aqAction->cursor() ->setModeAccess( AQSql::EDIT );
  aqAction->cursor() ->first();

  AQFormDB * form = new AQFormDB;
  QString formName( aqAction->cursor() ->metadata() ->alias() );

  form->setObjectName( formName );
  form->setAQAction( aqAction );
  form->activateRecordUi();
  //form->activateMasterUi();

  return form;
}

void AQMainWindow::activateAction( QAction * act )
{
  activateAction( act->objectName() );
}

void AQMainWindow::activateAction( const QString & idAction )
{
  if ( idAction.isEmpty() )
    return ;

  if ( idAction == "actExit" ) {
    Q_EMIT closeRequest( this );
    return ;
  }

  if ( idAction == "actAboutAbanQ" ) {
    AbanQ->aboutAbanQ();
    return ;
  }

  AQ_D( AQMainWindow );

#ifdef AQ_DEBUG

  if ( idAction == "actTests" ) {
    d->tests();
    return ;
  }
#endif

  addRecentItem( idAction );

  //AQ_INVOKER<AQMainWindowPrivate,AQFormDB *,QString> ivk;
  //AQFormDB * form = ivk.invoke( d, &AQMainWindowPrivate::createForm, idAction );
  AQFormDB * form = d->createForm( idAction );

  WQWidget * wqForm = WQWidget::create( form );

  d->tabView_->addTab( wqForm, form->objectName() );
  d->tabView_->setCurrentWidget( wqForm );
}

void AQMainWindow::activateItem( WTreeNode *, int )
{}

bool actionLessThan( QAction *, QAction * )
{
  return true;
}

void AQMainWindow::setCurrentWidget( WQWidget * )
{}

void AQMainWindow::closeOtherTabs()
{}

void AQMainWindow::closeOtherTabsOfAction()
{}

void AQMainWindow::newTreeFolder()
{}

void AQMainWindow::renameTreeItem()
{}

void AQMainWindow::deleteTreeItem()
{}

void AQMainWindow::addItemFromAction( const QString & )
{}

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

  AQ_D( AQMainWindow );

  if ( a ) {
    d->treeRecent_->addAction( a );
    list = d->treeRecent_->actions();
    if ( list.size() >= 10 )
      d->treeRecent_->removeAction( list.first() );
    d->treeRecent_->wtree() ->expand();
  }
}

void AQMainWindow::saveTreeMarks()
{}

void AQMainWindow::restoreTreeMarks()
{}

void AQMainWindow::saveTreeRecent()
{}

void AQMainWindow::restoreTreeRecent()
{}

void AQMainWindow::exportTreeMarks()
{}

void AQMainWindow::importTreeMarks()
{}

void AQMainWindow::exportTree( WQTree *, const QString & )
{}

void AQMainWindow::importTree( WQTree *, const QString & )
{}

void AQMainWindow::openActionsOfTreeMarks()
{}
