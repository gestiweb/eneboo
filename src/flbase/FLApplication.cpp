/***************************************************************************
                          FLApplication.cpp
                         -------------------
begin                : Mon Jul 2 2001
copyright            : (C) 2001-2005 by InfoSiAL, S.L.
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

#include "FLApplication.h"
#include "FLAbout.h"
#include "FLHelpWindow.h"
#include "FLFormDB.h"
#include "FLObjectFactory.h"
#include "FLWidgetAction.h"
#include "FLAction.h"
#include "FLTranslator.h"
#include "FLAccessControlLists.h"
#include "FLSqlDatabase.h"
#include "FLSqlSavePoint.h"
#include "FLSqlConnections.h"
#include "FLDiskCache.h"
#include "FLMemCache.h"
#include "FLTranslations.h"
#include "FLSettings.h"
#include "AQSObjectFactory.h"
#include "AQSWrapperFactory.h"
#include "metatranslator.h"

#include "AQConfig.h"

// Uso interno
class FLWorkspace : public QWorkspace
{
public:

  FLWorkspace(QWidget *parent = 0, const char *name = 0);

protected:

  void paintEvent(QPaintEvent *pe);

private:

  QImage logo;
  QColor fColor, pColor;
};

class FLWidget : public QWidget
{
public:

  FLWidget(QWidget *parent = 0, const char *name = 0,  WFlags f = 0);

protected:

  void paintEvent(QPaintEvent *pe);

private:

  QImage logo;
  QColor fColor, pColor;
};

class FLTimerDbLock : public QTimer
{
public:

  FLTimerDbLock(QObject *parent = 0, const char *name = 0)
    : QTimer(parent, name) {}

  int dbLocksLapsus_;
  int dbLocksChecks_;
  bool dbLocksShowWarn_;
  QString dbLocksMsgWarn_;
  QString dbLocksConn_;
};

class FLPopupWarn : public QWhatsThis
{
public:

  FLPopupWarn(QWidget *widget)
    : QWhatsThis(widget) {}

  bool clicked(const QString &href);
  QMap<QString, QSArgumentList> scriptCalls_;
};

bool FLPopupWarn::clicked(const QString &href)
{
  if (!href.isEmpty()) {
    QString h(href);
    if (href.contains(':'))
      h = href.section(':', 1, 1);
    if (h.contains('.') == 1)
      aqApp->call(h.section('.', 1, 1),  scriptCalls_[href], h.section('.', 0, 0));
    else
      aqApp->call(h, scriptCalls_[href], 0);
  }
  return false;
}

FLApplication::FLApplication(int &argc, char **argv) :
  AQ_IMPL_APP(argc, argv),
  pWorkspace(0), mainWidget_(0), container(0), toolBox(0),
  toggleBars(0), project_(0), wb_(0), dictMainWidgets(0),
  formAlone_(false), notExit_(false), acl_(0), popupWarn_(0),
  noGUI_(false), mngLoader_(0), sysTr_(0), initializing_(false),
  flFactory_(0)
{
  aqApp = this;

  dictMainWidgets = new QDict <QWidget>(37);

  AQConfig::init(this);
  FLMemCache::init();
  FLDiskCache::init();
  flFactory_ = new FLObjectFactory;
  timeUser_ = QDateTime::currentDateTime();
  multiLangEnabled_ = false;
  multiLangId_ = QString(QTextCodec::locale()).left(2).upper();

  QObject::setName("aqApp");
}

FLApplication::~FLApplication()
{
  FLSqlConnections::finish();
  delete dictMainWidgets;

#ifdef FL_DEBUG
  qWarning("*************************************************");
  qWarning("FLSqlQuery::countRefQuery");
  qWarning("*************************************************");
  qWarning(QString::number(FLSqlQuery::countRefQuery));
  qWarning("*************************************************");
  qWarning("FLSqlQuery::countRefQuery");
  qWarning("*************************************************");

  qWarning("*************************************************");
  qWarning("FLSqlCursor::countRefCursor");
  qWarning("*************************************************");
  qWarning(QString::number(FLSqlCursor::countRefCursor));
  qWarning("*************************************************");
  qWarning("FLSqlCursor::countRefCursor");
  qWarning("*************************************************");
#endif
}

bool FLApplication::eventFilter(QObject *obj, QEvent *ev)
{
  if (initializing_)
    return QApplication::eventFilter(obj, ev);

  if (formAlone_) {
    if (ev->type() == QEvent::Close) {
      generalExit();
      return true;
    }
    return QApplication::eventFilter(obj, ev);
  }

#if defined(Q_OS_WIN32)
  if (obj != mainWidget_ && !::qt_cast<QMainWindow *>(obj))
    return QApplication::eventFilter(obj, ev);

  QWidget *aw = (pWorkspace ? pWorkspace->activeWindow() : 0);
  if (aw && aw != obj && ev->type() != QEvent::Resize && ev->type() != QEvent::Close) {
    obj->removeEventFilter(this);
    if (ev->type() == QEvent::WindowActivate) {
      if (obj == container)
        activateModule("");
      else
        activateModule(obj->name());
    }
    if (pWorkspace && notify(pWorkspace->activeWindow(), ev)) {
      obj->installEventFilter(this);
      return true;
    }
    obj->installEventFilter(this);
  }
#endif
  FLApplicationInterface *ap2 = new FLApplicationInterface(this);
  switch (ev->type()) {
    case QEvent::KeyPress:
      if (obj == container) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(ev);
        if (ke->key() == Key_W && (ke->state() & (ControlButton | AltButton)) && ap2->isDebuggerMode()) {
          openQSWorkbench();
          return true;
        }
      }
      if (obj == mainWidget_) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(ev);
        if (ke->key() == Key_Shift && (ke->state() & ControlButton)) {
          activateModule("");
          return true;
        }
        if (ke->key() == Key_Q && (ke->state() & ControlButton)) {
          generalExit();
          return true;
        }
        if (ke->key() == Key_W && (ke->state() & (ControlButton | AltButton))) {
          openQSWorkbench();
          return true;
        }
        if (ke->key() == Key_Escape) {
          static_cast<QWidget *>(obj)->hide();
          return true;
        }
      }
      break;
    case QEvent::Close:
      if (obj == container) {
        generalExit();
        return true;
      } else {
        static_cast<QWidget *>(obj)->hide();
        return true;
      }
      break;
    case QEvent::WindowActivate:
      if (obj == container) {
        activateModule("");
        return true;
      } else {
        activateModule(obj->name());
        return true;
      }
      break;
    case QEvent::MouseButtonPress:
      if (modulesMenu) {
        QMouseEvent *me = static_cast<QMouseEvent *>(ev);
        if (me->button() == RightButton) {
          modulesMenu->popup(QCursor::pos());
          return true;
        } else
          return false;
      } else
        return false;
      break;
  }

  return QApplication::eventFilter(obj, ev);
}

void FLApplication::init(const QString &n, const QString &callFunction,
                         const QString &arguments, bool quitAfterCall, bool noMax)
{
  initializing_ = true;

  container = new QMainWindow(0);
  container->setName("container");
  container->setIcon(QPixmap::fromMimeSource("icono_abanq.png"));
  container->setCaption("Eneboo " AQ_VERSION);

  FLDiskCache::init(this);
#ifndef QSDEBUGGER
  AQ_DISKCACHE_CLR();
#endif

  if (n.isEmpty() || (!callFunction.isEmpty() && quitAfterCall)) {
    windowMenu = new QPopupMenu(container, "windowMenu");
    windowCascadeAction = new QAction(tr("Cascada"), tr("Cascada"), QKeySequence(), container);
    windowCascadeAction->setIconSet(QIconSet(QPixmap::fromMimeSource("cascada.png")));
    windowCascadeAction->addTo(windowMenu);
    windowTileAction = new QAction(tr("Mosaico"), tr("Mosaico"), QKeySequence(), container);
    windowTileAction->setIconSet(QIconSet(QPixmap::fromMimeSource("mosaico.png")));
    windowTileAction->addTo(windowMenu);
    windowCloseAction = new QAction(tr("Cerrar"), tr("Cerrar"), QKeySequence(), container);
    windowCloseAction->setIconSet(QIconSet(QPixmap::fromMimeSource("cerrar.png")));
    windowCloseAction->addTo(windowMenu);

    modulesMenu = new QPopupMenu(container, "modulesMenu");
    modulesMenu->setCheckable(false);

    QWidget *w = new QWidget(container, "widgetContainer");
    QVBoxLayout *vL = new QVBoxLayout(w);
    exitButton = new QPushButton(QPixmap::fromMimeSource("exit.png"), tr("Salir"), w, "pbSalir");
    exitButton->setAccel(QKeySequence(tr("Ctrl+Q")));
    exitButton->setFocusPolicy(QWidget::NoFocus);
    QToolTip::add(exitButton, tr("Salir de la aplicación (Ctrl+Q)"));
    QWhatsThis::add(exitButton, tr("Salir de la aplicación (Ctrl+Q)"));
    connect(exitButton, SIGNAL(clicked()), this, SLOT(generalExit()));
    toolBox = new QToolBox(w, "toolBox");
    vL->addWidget(exitButton);
    vL->addWidget(toolBox);
    container->setCentralWidget(w);
  }

  qInitNetworkProtocols();

#ifdef QSDEBUGGER
  project_ = new QSProject(this, db()->database());
#else
  project_ = new QSProject(0, db()->database());
#endif

  AQ_SET_MNGLOADER

  db()->manager()->init();
  mngLoader_->init();

  if (n.isEmpty() || (!callFunction.isEmpty() && quitAfterCall)) {
    initStyles();
    initMenuBar();
  }

  db()->manager()->loadTables();
  mngLoader_->loadKeyFiles();
  mngLoader_->loadAllIdModules();
  mngLoader_->loadIdAreas();

  acl_ = new FLAccessControlLists();
  acl_->init();

  loadScripts();
  mngLoader_->setShaLocalFromGlobal();
  loadTranslations();

  QSInterpreter *i = project_->interpreter();
  if (i) {
    i->addObjectFactory(flFactory_);
    i->addObjectFactory(new AQSObjectFactory);
    i->addWrapperFactory(new AQSWrapperFactory);
    i->addObjectFactory(new QSInputDialogFactory);
    i->addObjectFactory(new QSUtilFactory);
#ifdef FL_DEBUGGER
    i->setErrorMode( QSInterpreter::AskForDebug );
#else
    i->setErrorMode( QSInterpreter::Notify );
#endif
  } else {
    // Failed loading QSA.
  }

  if (!callFunction.isEmpty()) {

    QStringList argumentList = QStringList::split(':', arguments, false);
    QSArgumentList arglist;
    for (QStringList::Iterator it = argumentList.begin(); it != argumentList.end(); ++it) {
      arglist.append(QSArgument(*it));
    }
    call(callFunction, arglist, 0);
    if (quitAfterCall) {
      if (!db()->driverName().isEmpty())
        FLVar::clean();
      mngLoader_->finish();
      db()->manager()->finish();
      QTimer::singleShot(3000, this, SLOT(quit()));
    }
  }
  

  if (!quitAfterCall) {
    if (n.isEmpty()) {
      call("init", QSArgumentList(), "sys");
      initToolBox();
      readState();
      container->installEventFilter(this);
    } else {
      FLFormDB *f = new FLFormDB(n, 0, (noMax ? 0 : Qt::WStyle_Customize));
      formAlone_ = true;
      f->installEventFilter(this);
      f->setMainWidget();
      QApplication::setMainWidget(f);
      if (f->mainWidget()) {
        if (noMax) {
          f->show();
        } else {
          f->showMaximized();
        }
      }
      else {
        f->close();
      }
    }
  }

  AQ_UNSET_MNGLOADER

  initializing_ = false;
}


int FLApplication::initfcgi()
{
  initializing_ = true;

  container = new QMainWindow(0);
  container->setName("container");
  container->setCaption("Eneboo " AQ_VERSION);

  FLDiskCache::init(this);
#ifndef QSDEBUGGER
  AQ_DISKCACHE_CLR();
#endif

  qInitNetworkProtocols();

#ifdef QSDEBUGGER
  project_ = new QSProject(this, db()->database());
#else
  project_ = new QSProject(0, db()->database());
#endif

  AQ_SET_MNGLOADER

  db()->manager()->init();
  mngLoader_->init();

  db()->manager()->loadTables();
  mngLoader_->loadKeyFiles();
  mngLoader_->loadAllIdModules();
  mngLoader_->loadIdAreas();

  acl_ = new FLAccessControlLists();
  acl_->init();

  loadScripts();
  mngLoader_->setShaLocalFromGlobal();
  loadTranslations();

  QSInterpreter *i = project_->interpreter();
  if (i) {
    i->addObjectFactory(flFactory_);
    i->addObjectFactory(new AQSObjectFactory);
    i->addWrapperFactory(new AQSWrapperFactory);
    i->addObjectFactory(new QSInputDialogFactory);
    i->addObjectFactory(new QSUtilFactory);
#ifdef FL_DEBUGGER
    i->setErrorMode( QSInterpreter::AskForDebug );
#else
    i->setErrorMode( QSInterpreter::Notify );
#endif
  } else {
    // Failed loading QSA.
  }

/*
    QStringList argumentList = QStringList::split(':', arguments, false);
    QSArgumentList arglist;
    for (QStringList::Iterator it = argumentList.begin(); it != argumentList.end(); ++it) {
      arglist.append(QSArgument(*it));
    }
    call(callFunction, arglist, 0);

  if (!db()->driverName().isEmpty())
    FLVar::clean();
  mngLoader_->finish();
  db()->manager()->finish();
  QTimer::singleShot(3000, this, SLOT(quit()));
  */
  AQ_UNSET_MNGLOADER

  initializing_ = false;
    
}

void FLApplication::addObjectFactory(QSObjectFactory *newObjectFactory) {
	QSInterpreter *i = project_->interpreter();
	if (i) {
		if (newObjectFactory == NULL) {
			qDebug("ERROR: newObjectFactory == NULL!");
			return;
		}
		i->addObjectFactory(newObjectFactory);
	}
}


QString FLApplication::callfcgi(const QString &callFunction, QStringList argumentList) {    
    initializing_ = true;
    AQ_SET_MNGLOADER
    QSArgumentList arglist;
    for (QStringList::Iterator it = argumentList.begin(); it != argumentList.end(); ++it) {
      arglist.append(QSArgument(*it));
    }
    QSArgument ret = call(callFunction, arglist, 0);
    // return ret.typeName();
    QVariant v = ret.variant();
    AQ_UNSET_MNGLOADER
    initializing_ = false;
    return v.asString();        
}

void FLApplication::endfcgi() {    
    initializing_ = true;

  AQ_SET_MNGLOADER
  
  if (!db()->driverName().isEmpty()) FLVar::clean();
  mngLoader_->finish();
  db()->manager()->finish();
  QTimer::singleShot(10, this, SLOT(quit()));
  AQ_UNSET_MNGLOADER
    initializing_ = false;

  
}



void FLApplication::openQSWorkbench()
{
#ifdef QSDEBUGGER
  if (project_) {
    if (!wb_)
      wb_ = new QSWorkbench(project_, 0, db()->database());
    wb_->open();
    wb_->widget()->raise();
  }
#endif
}


void FLApplication::initMainWidget()
{
  if (!mainWidget_ || !container)
    return;

  QMainWindow *mw = ::qt_cast<QMainWindow *>(mainWidget_);
  if (mw) {
    mw->menuBar()->insertItem(tr("&Ventana"), windowMenu);
    mw->setCentralWidget(0);
  }

  initView();
  initActions();
  initToolBar();
  initStatusBar();

  readStateModule();
}

void FLApplication::showMainWidget(QWidget *w)
{
  if (!container) {
    if (w)
      w->show();
    return;
  }

  QWidget *focusW = focusWidget();

  if (w == container || !w) {
    if (container->isMinimized())
      container->showNormal();
    else if (!container->isVisible()) {
      container->setFont(font());
      container->show();
    }
    if (focusW && focusW->isA("QMainWindow") && focusW != container)
      container->setFocus();
    if (!container->isActiveWindow()) {
      container->raise();
      container->setActiveWindow();
    }
    container->setCaption("Eneboo " AQ_VERSION);
    return;
  }

  if (w->isMinimized())
    w->showNormal();
  else if (!w->isVisible()) {
    w->show();
    w->setFont(font());
  }
  if (focusW && focusW->isA("QMainWindow") && focusW != w)
    w->setFocus();
  if (!w->isActiveWindow()) {
    w->raise();
    w->setActiveWindow();
  }

  QMainWindow *mw = ::qt_cast<QMainWindow *>(w);
  if (mw) {
    QWidget *view_back = mw->centralWidget();
    if (view_back) {
      pWorkspace = static_cast<FLWorkspace *>(view_back->child(mw->name(), "QWorkspace"));
      view_back->show();
    }
  }

  AQ_SET_MNGLOADER

  setCaptionMainWidget("");
  QString descripArea = mngLoader_->idAreaToDescription(mngLoader_->activeIdArea());
  w->setIcon(mngLoader_->iconModule(w->name()));
  toolBox->setCurrentItem(static_cast<QToolBar *>(toolBox->child(descripArea, "QToolBar")));

  AQ_UNSET_MNGLOADER
}

void FLApplication::setMainWidget(QWidget *w)
{
  if (!container)
    return;

  if (w == container || !w) {
    QApplication::setMainWidget(container);
    mainWidget_ = 0;
    return;
  }

  QApplication::setMainWidget(w);
  mainWidget_ = w;

  QMainWindow *mw = ::qt_cast<QMainWindow *>(mainWidget_);
  if (!mw)
    return;

  QAction *a, *b;
  if (toggleBars) {
    QObject *tgb = container->child("agToggleBars");
    a = ::qt_cast<QAction *>(tgb->child("Herramientas"));
    b = ::qt_cast<QAction *>(tgb->child("Estado"));
    QToolBar *tB = ::qt_cast<QToolBar *>(mw->child("toolBar"));
    if (tB)
      a->setOn(tB->isShown());
    b->setOn(mw->statusBar()->isShown());
  }
}

void FLApplication::makeStyle(const QString &style)
{
  QPalette p = palette();
  setStyle(style);
  setPalette(p, true);
  initToolBox();
  FLSettings::writeEntry("style", style);
}

void FLApplication::chooseFont()
{
  setFont(QFontDialog::getFont(0, font(), mainWidget()), true);
  QWidgetList *list = allWidgets();
  QWidgetListIt it(*list);
  QWidget *w;

  while ((w = it.current()) != 0) {
    ++it;
    w->setFont(font());
  }

  FLSettings::writeEntry("font/family", font().family());
  FLSettings::writeEntry("font/pointSize", font().pointSize());
  FLSettings::writeEntry("font/bold", font().bold());
  FLSettings::writeEntry("font/italic", font().italic());
  FLSettings::writeEntry("font/underline", font().underline());
  FLSettings::writeEntry("font/strikeOut", font().strikeOut());
}

void FLApplication::showStyles()
{
  if (style)
    style->exec(QCursor::pos());
}

void FLApplication::showToggleBars()
{
  if (toggleBars)
    toggleBars->exec(QCursor::pos());
}

void FLApplication::initToolBox()
{
  
  if (!toolBox || !modulesMenu)
    return;

  modulesMenu->clear();
  FLApplicationInterface *ap2 = new FLApplicationInterface(this);
  while (toolBox->count()) {
    QWidget *item = toolBox->item(0);
    if (item) {
      if (item->isA("QToolBar"))
        static_cast<QToolBar *>(item)->clear();
      toolBox->removeItem(item);
      delete item;
    }
  }

  AQ_SET_MNGLOADER

  QStringList listAreas = mngLoader_->listIdAreas(), listModules;
  QString descripArea, descripModule;
  QToolBar *newAreaBar;
  FLWidgetAction *newModuleAction;
  QWidget *w;
  int c = 65;


  for (QStringList::Iterator itA = listAreas.begin(); itA != listAreas.end(); ++itA) {
    descripArea = mngLoader_->idAreaToDescription(*itA);
    newAreaBar = new QToolBar(tr(descripArea), container, toolBox, false, descripArea);
    newAreaBar->setFrameStyle(QFrame::NoFrame);
    newAreaBar->setOrientation(Qt::Vertical);
    newAreaBar->boxLayout()->setSpacing(3);
    toolBox->addItem(newAreaBar, tr(descripArea));

    QActionGroup *ag = new QActionGroup(newAreaBar, descripArea);
    ag->setMenuText(descripArea);
    ag->setUsesDropDown(true);

    listModules = mngLoader_->listIdModules(*itA);
    listModules.sort();
    for (QStringList::Iterator itM = listModules.begin(); itM != listModules.end(); ++itM, ++c) {
      if (QChar(c) == 'Q')
        ++c;
        
#ifdef FL_QUICK_CLIENT
      if (*itM == "sys")
        continue;
#endif
        
    if (*itM == "sys" && ap2->isDebuggerMode()) {
       
        descripModule = QString(QChar(c)) + QString::fromLatin1(": ") +
                        tr("Cargar Paquete de Módulos");
        newModuleAction = new FLWidgetAction(descripModule, descripModule, descripModule,
                                             QKeySequence(QString("Ctrl+Shift+") + QString(QChar(c))),
                                             newAreaBar, *itM);
        newModuleAction->setIconSet(QPixmap::fromMimeSource("load.png"));
        newModuleAction->setIdModule(*itM);
        newModuleAction->addTo(newAreaBar);
        ag->add(newModuleAction);
        connect(newModuleAction, SIGNAL(activated()), this, SLOT(loadModules()));

        ++c;
       
       /* descripModule = QString(QChar(c)) + QString::fromLatin1(": ") +
                        tr("Exportar Módulos a Disco");
        newModuleAction = new FLWidgetAction(descripModule, descripModule, descripModule,
                                             QKeySequence(QString("Ctrl+Shift+") + QString(QChar(c))),
                                             newAreaBar, *itM);
        newModuleAction->setIconSet(QPixmap::fromMimeSource("export.png"));
        newModuleAction->setIdModule(*itM);
        newModuleAction->addTo(newAreaBar);
        ag->add(newModuleAction);
        connect(newModuleAction, SIGNAL(activated()), this, SLOT(exportModules()));

        ++c; */
       /* descripModule = QString(QChar(c)) + QString::fromLatin1(": ") +
                        tr("Importar Módulos desde Disco");
        newModuleAction = new FLWidgetAction(descripModule, descripModule, descripModule,
                                             QKeySequence(QString("Ctrl+Shift+") + QString(QChar(c))),
                                             newAreaBar, *itM);
        newModuleAction->setIconSet(QPixmap::fromMimeSource("import.png"));
        newModuleAction->setIdModule(*itM);
        newModuleAction->addTo(newAreaBar);
        ag->add(newModuleAction);
        connect(newModuleAction, SIGNAL(activated()), this, SLOT(importModules()));

        ++c; */
       /* descripModule = QString(QChar(c)) + QString::fromLatin1(": ") +
                        tr("Actualización y Soporte");
        newModuleAction = new FLWidgetAction(descripModule, descripModule, descripModule,
                                             QKeySequence(QString("Ctrl+Shift+") + QString(QChar(c))),
                                             newAreaBar, *itM);
        newModuleAction->setIconSet(QPixmap::fromMimeSource("settings.png"));
        newModuleAction->setIdModule(*itM);
        newModuleAction->addTo(newAreaBar);
        ag->add(newModuleAction);
        connect(newModuleAction, SIGNAL(activated()), this, SLOT(updateAbanQ()));
        ++c;
        */
#ifdef QSDEBUGGER  /// Si compilamos el debugger nos aparece este apartado del menu.

  if (ap2->isDebuggerMode())
                                  {        
        descripModule = QString(QChar(c)) + QString::fromLatin1(": ") +
                        tr("QSA WorkBench");
        newModuleAction = new FLWidgetAction(descripModule, descripModule, descripModule,
                                             QKeySequence(QString("Ctrl+Shift+") + QString(QChar(c))),
                                             newAreaBar, *itM);
        newModuleAction->setIconSet(QPixmap::fromMimeSource("bug.png"));
        newModuleAction->setIdModule(*itM);
        newModuleAction->addTo(newAreaBar);
        ag->add(newModuleAction);
        connect(newModuleAction, SIGNAL(activated()), this, SLOT(openQSWorkbench()));
                                  }
#endif

#ifndef QSDEBUGGER

#else
        ++c;
#endif
      }

      descripModule = QString(QChar(c)) + ": " + mngLoader_->idModuleToDescription(*itM);
      newModuleAction = new FLWidgetAction(tr(descripModule),
                                           tr(descripModule),
                                           tr(descripModule),
                                           QKeySequence(QString("Ctrl+Shift+") + QString(QChar(c))),
                                           newAreaBar,
                                           (*itM));
      newModuleAction->setIconSet(mngLoader_->iconModule(*itM));
      newModuleAction->setIdModule(*itM);
      newModuleAction->addTo(newAreaBar);
      ag->add(newModuleAction);
      connect(newModuleAction, SIGNAL(activated()), this, SLOT(activateModule()));
    }

    w = new FLWidget(newAreaBar);
    w->setBackgroundMode(Qt::NoBackground);
    newAreaBar->setStretchableWidget(w);
    ag->addTo(modulesMenu);
  }

  AQ_UNSET_MNGLOADER

  QToolBar *configToolBar = new QToolBar(tr("Configuracion"), container, toolBox, false, "Configuracion");
  configToolBar->setFrameStyle(QFrame::NoFrame);
  configToolBar->setOrientation(Qt::Vertical);
  configToolBar->boxLayout()->setSpacing(3);
  toolBox->addItem(configToolBar, tr("Configuración"));

  FLWidgetAction *fontAction = new FLWidgetAction(tr("Fuente"), tr("Fuente"), tr("Fuente"), QKeySequence(), configToolBar, "fuente");
  fontAction->setIconSet(QPixmap::fromMimeSource("font.png"));
  connect(fontAction, SIGNAL(activated()), this, SLOT(chooseFont()));
  fontAction->addTo(configToolBar);

  FLWidgetAction *styleAction = new FLWidgetAction(tr("Estilo"), tr("Estilo"), tr("Estilo"), QKeySequence(), configToolBar, "estilo");
  styleAction->setIconSet(QPixmap::fromMimeSource("estilo.png"));
  styleAction->addTo(configToolBar);
  connect(styleAction, SIGNAL(activated()), this, SLOT(showStyles()));

  w = new FLWidget(configToolBar);
  w->setBackgroundMode(Qt::NoBackground);
  configToolBar->setStretchableWidget(w);

  QToolBar *helpToolBar = new QToolBar(tr("Ayuda"), container, toolBox, false, "Ayuda");
  helpToolBar->setFrameStyle(QFrame::NoFrame);
  helpToolBar->setOrientation(Qt::Vertical);
  helpToolBar->boxLayout()->setSpacing(3);
  toolBox->addItem(helpToolBar, tr("Ayuda"));

  FLWidgetAction *helpIndexAction = new FLWidgetAction(tr("Ayuda"), tr("Índice"), tr("Indice"), QKeySequence(), helpToolBar, "help");
  helpIndexAction->setIconSet(QPixmap::fromMimeSource("help_index.png"));
  connect(helpIndexAction, SIGNAL(activated()), this, SLOT(helpIndex()));
  helpIndexAction->addTo(helpToolBar);

  FLWidgetAction *aboutAction = new FLWidgetAction(tr("Ayuda"), tr("Acerca de Eneboo"), tr("&Acerca de Eneboo"), QKeySequence(), helpToolBar, "acercaeneboo");
  aboutAction->setIconSet(QPixmap::fromMimeSource("about.png"));
  connect(aboutAction, SIGNAL(activated()), this, SLOT(aboutAbanQ()));
  aboutAction->addTo(helpToolBar);

  FLWidgetAction *aboutQt = new FLWidgetAction(tr("Ayuda"), tr("Acerca de Qt"), tr("&Acerca Qt"), QKeySequence(), helpToolBar, "acercaqt");
  aboutQt->setIconSet(QPixmap::fromMimeSource("aboutqt.png"));
  connect(aboutQt, SIGNAL(activated()), this, SLOT(aboutQt()));
  aboutQt->addTo(helpToolBar);

  w = new FLWidget(helpToolBar);
  w->setBackgroundMode(Qt::NoBackground);
  helpToolBar->setStretchableWidget(w);

  if (acl_)
    acl_->process(container);
}

void FLApplication::initActions()
{
  if (::qt_cast<QMainWindow *>(mainWidget_) && pWorkspace) {
    connect(windowCascadeAction, SIGNAL(activated()), pWorkspace, SLOT(cascade()));
    connect(windowTileAction, SIGNAL(activated()), pWorkspace, SLOT(tile()));
    connect(windowCloseAction, SIGNAL(activated()), pWorkspace, SLOT(closeActiveWindow()));
  }
}

void FLApplication::initMenuBar()
{
  connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(windowMenuAboutToShow()));
}

void FLApplication::initToolBar()
{
  QMainWindow *mw = ::qt_cast<QMainWindow *>(mainWidget_);
  if (!mw)
    return;

  QToolBar *tb = ::qt_cast<QToolBar *>(mw->child("toolBar"));
  if (!tb)
    return;

  tb->setMovingEnabled(false);
  if (tb->child("automatic what's this? button"))
    return;
  tb->addSeparator();
  QWhatsThis::whatsThisButton(tb);

  if (!toggleBars) {
    QAction *a, *b;
    toggleBars = new QPopupMenu(container, "toggleBars");
    toggleBars->setCheckable(true);
    QActionGroup *ag = new QActionGroup(container, "agToggleBars", false);
    a = new QAction(tr("Barra de Herramientas"), QKeySequence(), ag, "Herramientas");
    a->setToggleAction(true);
    a->setOn(true);
    connect(a, SIGNAL(toggled(bool)), this, SLOT(toggleToolBar(bool)));
    b = new QAction(tr("Barra de Estado"), QKeySequence(), ag, "Estado");
    b->setToggleAction(true);
    b->setOn(true);
    connect(b, SIGNAL(toggled(bool)), this, SLOT(toggleStatusBar(bool)));
    ag->addTo(toggleBars);
  }

  mw->menuBar()->insertItem(tr("&Ver"), toggleBars);
  mw->menuBar()->insertItem(tr("&Módulos"), modulesMenu);
}

void FLApplication::initStatusBar()
{
  QMainWindow *mw = ::qt_cast<QMainWindow *>(mainWidget_);
  if (!mw)
    return;
  mw->statusBar()->message(tr("Listo."));
  mw->statusBar()->setSizeGripEnabled(false);
}

void FLApplication::initView()
{
  QMainWindow *mw = ::qt_cast<QMainWindow *>(mainWidget_);
  if (!mw)
    return;

  QWidget *view_back = mw->centralWidget();
  if (!view_back) {
    view_back = new QVBox(mainWidget_);
    static_cast<QVBox *>(view_back)->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    pWorkspace = new FLWorkspace(view_back, db()->managerModules()->activeIdModule());
    pWorkspace->setBackgroundMode(Qt::NoBackground);
    pWorkspace->setScrollBarsEnabled(true);
    mw->setCentralWidget(view_back);
  }
}

void FLApplication::initStyles()
{
  style = new QPopupMenu(container, "styles");
  style->setCheckable(true);

  QActionGroup *ag = new QActionGroup(container, 0);

  ag->setExclusive(true);
  QSignalMapper *styleMapper = new QSignalMapper(container);
  connect(styleMapper, SIGNAL(mapped(const QString &)), this, SLOT(makeStyle(const QString &)));

  QStringList list = QStyleFactory::keys();
  list.sort();
  QDict < int >stylesDict(17, false);

  for (QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
    QString styleStr = *it;
    QString styleAccel = styleStr;

    if (stylesDict[ styleAccel.left(1)]) {
      for (uint i = 0; i < styleAccel.length(); ++i) {
        if (!stylesDict[ styleAccel.mid(i, 1)]) {
          stylesDict.insert(styleAccel.mid(i, 1), (const int *) 1);
          styleAccel = styleAccel.insert(i, '&');
          break;
        }
      }
    } else {
      stylesDict.insert(styleAccel.left(1), (const int *) 1);
      styleAccel = "&" + styleAccel;
    }
    QAction *a = new QAction(styleStr, QIconSet(), styleAccel, 0, ag, 0,
                             ag->isExclusive());
    if (styleStr == FLSettings::readEntry("style", "QtCurve"))
      a->setOn(true);
    connect(a, SIGNAL(activated()), styleMapper, SLOT(map()));
    styleMapper->setMapping(a, a->text());
  }
  ag->addTo(style);
}

bool FLApplication::queryExit()
{
  if (notExit_)
    return false;
  return (QMessageBox::Yes == QMessageBox::question(
            aqApp->mainWidget(), tr("Salir..."),
            tr("¿ Quiere salir de la aplicación ?"),
            QMessageBox::Yes,
            QMessageBox::No |
            QMessageBox::Default |
            QMessageBox::Escape)
         );
}

void FLApplication::generalExit()
{
  if (wb_)
    wb_->close();

  if (queryExit()) {
    if (!formAlone_) {
      writeState();
      writeStateModule();
    }
    if (!db()->driverName().isEmpty())
      FLVar::clean();
    db()->managerModules()->finish();
    db()->manager()->finish();
    quit();
  }
}

QString FLApplication::getTabWidgetPages(const QString &wN, const QString &n)
{
  QString actionName;
  const FLAction *a;
  QWidget *w;
  if (wN.left(10) == "formRecord") {
    actionName = wN.mid(10, wN.length());
    a = db()->manager()->action(actionName);
    w = db()->managerModules()->createFormRecord(a, 0, 0, 0);
  } else if (wN.left(10) == "formSearch") {
    actionName = wN.mid(10, wN.length());
    a = db()->manager()->action(actionName);
    w = db()->managerModules()->createForm(a, 0, 0, 0);
  } else {
    actionName = wN.mid(4, wN.length());
    a = db()->manager()->action(actionName);
    w = db()->managerModules()->createForm(a, 0, 0, 0);
  }

  if (!w)
    return "";

  QTabWidget *tabW = static_cast<QTabWidget * >(w->child(n));
  if (!tabW)
    return "";

  QString s;
  QWidget *page = 0;
  for (int i = 0; i < tabW->count(); ++i) {
    page = tabW->page(i);
    s += QString(page->name()) + "/" + QString(page->caption()) + "*";
  }
  return s;
}

QString FLApplication::getWidgetList(const QString &wN, const QString &c)
{
  QString actionName;
  const FLAction *a;
  QWidget *w;
  if (wN.left(10) == "formRecord") {
    actionName = wN.mid(10, wN.length());
    a = db()->manager()->action(actionName);
    w = db()->managerModules()->createFormRecord(a, 0, 0, 0);
  } else if (wN.left(10) == "formSearch") {
    actionName = wN.mid(10, wN.length());
    a = db()->manager()->action(actionName);
    w = db()->managerModules()->createForm(a, 0, 0, 0);
  } else {
    actionName = wN.mid(4, wN.length());
    a = db()->manager()->action(actionName);
    w = db()->managerModules()->createForm(a, 0, 0, 0);
  }

  if (!w)
    return "";

  const QObjectList *oL = w->queryList(c);
  QObjectListIt it(*oL);
  QObject *o;
  QString s;
  QString name;
  QString fieldTable;

  while ((o = it.current()) != 0) {
    ++it;
    name = o->name();
    if (c == "FLFieldDB") {
      fieldTable = static_cast<FLFieldDB *>(o)->tableName();
      if (!fieldTable.isEmpty() && fieldTable != a->table())
        continue;
      s += name + "/" + static_cast<FLFieldDB *>(o)->fieldName() + "*";
    } else if (c == "FLTableDB") {
      s += name + "/" + static_cast<FLTableDB *>(o)->tableName() + "*";
    } else if (c == "QButton" || c == "Button") {
      if (name == "pushButtonDB" || name == "pbAux" || name == "qt_left_btn" || name == "qt_right_btn")
        continue;
      s += name + "/" + static_cast<QButton *>(o)->text() + "*";
    } else {
      if (name == "textLabelDB" || name == "componentDB" || name == "tab pages" ||
          name == "editor" || name == "FrameFind" || name == "TextLabelSearch" ||
          name == "TextLabelIn" || name == "lineEditSearch" || name == "in-combo" ||
          name == "voidTable")
        continue;
      if (o->isA("QGroupBox"))
        s += name + "/" + ((QGroupBox *) o)->title() + "*";
      else
        s += name + "/ " + "*";
    }
  }
  delete o;
  delete oL;
  delete w;
  return s;
}


void FLApplication::toggleToolBar(bool toggle)
{
  QMainWindow *mw = ::qt_cast<QMainWindow *>(mainWidget_);
  if (!mw)
    return;

  QToolBar *tb = ::qt_cast<QToolBar *>(mw->child("toolBar"));
  if (!tb)
    return;

  if (toggle)
    tb->show();
  else
    tb->hide();
}

void FLApplication::toggleStatusBar(bool toggle)
{
  QMainWindow *mw = ::qt_cast<QMainWindow *>(mainWidget_);
  if (!mw)
    return;

  if (toggle)
    mw->statusBar()->show();
  else
    mw->statusBar()->hide();
}

void FLApplication::aboutAbanQ()
{
  FLAbout *aboutAbanQ = new FLAbout(AQ_VERSION, 0);
  aboutAbanQ->exec();
  aboutAbanQ->deleteLater();
}

void FLApplication::helpIndex()
{
  FLHelpWindow *help = new FLHelpWindow(AQ_DATA + "/doc/index.html",
                                        ".", 0, "help viewer");
  help->show();
}

void FLApplication::statusHelpMsg(const QString &text)
{
  QMainWindow *mw = ::qt_cast<QMainWindow *>(mainWidget_);
  if (!mw)
    return;
  mw->statusBar()->message(text, 2000);
}

void FLApplication::windowMenuAboutToShow()
{
  if (!pWorkspace)
    return;

  windowMenu->clear();
  windowCascadeAction->addTo(windowMenu);
  windowTileAction->addTo(windowMenu);
  windowCloseAction->addTo(windowMenu);

  if (pWorkspace->windowList().isEmpty()) {
    windowCascadeAction->setEnabled(false);
    windowTileAction->setEnabled(false);
    windowCloseAction->setEnabled(false);
  } else {
    windowCascadeAction->setEnabled(true);
    windowTileAction->setEnabled(true);
    windowCloseAction->setEnabled(true);
    windowMenu->insertSeparator();
  }

  QWidgetList windows(pWorkspace->windowList());
  for (int i = 0; i < int(windows.count()); ++i) {
    int id = windowMenu->insertItem(windows.at(i)->caption(), this, SLOT(windowMenuActivated(int)));
    windowMenu->setItemParameter(id, i);
    windowMenu->setItemChecked(id, pWorkspace->activeWindow() == windows.at(i));
  }
}

void FLApplication::windowMenuActivated(int id)
{
  if (!pWorkspace)
    return;
  QWidget *w = pWorkspace->windowList().at(id);
  if (w)
    w->setFocus();
}

bool FLApplication::existsFormInMDI(const QString &id)
{
  if (id.isEmpty() || !pWorkspace)
    return false;
  if (!pWorkspace->windowList().isEmpty()) {
    QWidgetList windows = pWorkspace->windowList();
    for (int i = 0; i < int(windows.count()); ++i) {
      if (static_cast<FLFormDB *>(windows.at(i))->idMDI() == id) {
        windows.at(i)->showNormal();
        windows.at(i)->setFocus();
        return true;
      }
    }
  }
  return false;
}

void FLApplication::openMasterForm(const QString &n, const QPixmap & /*p*/)
{
  if (existsFormInMDI(n))
    return;

  bool showMaximized = false;
  if (pWorkspace && pWorkspace->activeWindow())
    showMaximized = pWorkspace->activeWindow()->isMaximized();
  FLFormDB *f = new FLFormDB(n, pWorkspace);

  f->setMainWidget();
  if (f->mainWidget()) {
    if ((pWorkspace && pWorkspace->windowList().isEmpty()) || showMaximized)
      f->showMaximized();
    else
      f->show();
  } else
    f->close();
}

void FLApplication::openDefaultForm()
{
  QObject *s = const_cast<QObject *>(sender());
  if (s && !s->inherits("QAction")) {
#ifdef FL_DEBUG
    qWarning(tr("FLApplication : El objeto que envia señales para abrir formularios debe ser de la clase QAction"));
#endif
    return;
  }
  QPixmap icon = static_cast<QAction *>(s)->iconSet().pixmap();
  openMasterForm(s->name(), icon);
}

void FLApplication::execMainScript(const QString &n)
{
  const FLAction *a = db()->manager()->action(n);
  if (!a) {
#ifdef FL_DEBUG
    qWarning(tr("FLApplication : No existe la acción %1").arg(n));
#endif

    return;
  }
  call("main", QSArgumentList(), "form" + a->name());
}

void FLApplication::execDefaultScript()
{
  QObject *s = const_cast<QObject *>(sender());
  if (s && !s->inherits("QAction")) {
#ifdef FL_DEBUG
    qWarning(tr("FLApplication : El objeto que envia señales para abrir scripts debe ser de la clase QAction"));
#endif
    return;
  }
  execMainScript(s->name());
}

void FLApplication::windowClose()
{
  if (!pWorkspace)
    return;
  pWorkspace->closeActiveWindow();
}

void FLApplication::writeState()
{
  FLSettings::writeEntry("MultiLang/Enabled", multiLangEnabled_);
  FLSettings::writeEntry("MultiLang/LangId", multiLangId_);

  if (container) {
    QStringList windowsOpened;
    QWidgetList *list = topLevelWidgets();
    QWidgetListIt it(*list);
    QWidget *w;
    if (initializing_) {
      while ((w = it.current()) != 0) {
        ++it;
        w->removeEventFilter(this);
        if (dictMainWidgets->find(w->name()) != 0) {
          if (w != container) {
            if (!w->isHidden())
              windowsOpened << w->name();
            w->hide();
          } else
            w->setDisabled(true);
        }
      }
    } else {
      while ((w = it.current()) != 0) {
        ++it;
        if (w != container && !w->isHidden() && dictMainWidgets->find(w->name()) != 0)
          windowsOpened << w->name();
      }
    }
    delete list;
    FLSettings::writeEntry("windowsOpened/Main", windowsOpened);

    FLSettings::writeEntry("Geometry/MainwindowMaximized", container->isMaximized());
    if (!container->isMaximized()) {
      FLSettings::writeEntry("Geometry/MainwindowX", container->x());
      FLSettings::writeEntry("Geometry/MainwindowY", container->y());
      FLSettings::writeEntry("Geometry/MainwindowWidth", container->width());
      FLSettings::writeEntry("Geometry/MainwindowHeight", container->height());
    }
  }

  for (QMap<QString, QRect>::const_iterator it = mapGeometryForms_.begin(); it != mapGeometryForms_.end(); ++it) {
    QRect r(*it);
    QString k("Geometry/" + it.key() + "/");

    FLSettings::writeEntry(k + "X", r.x());
    FLSettings::writeEntry(k + "Y", r.y());
    FLSettings::writeEntry(k + "Width", r.width());
    FLSettings::writeEntry(k + "Height", r.height());
  }
}

void FLApplication::writeStateModule()
{
  QString idM(db()->managerModules()->activeIdModule());
  if (!mainWidget_ || idM.isEmpty() || mainWidget_->name() != idM)
    return;

  if (::qt_cast<QMainWindow *>(mainWidget_) &&
      pWorkspace && !pWorkspace->windowList().isEmpty()) {
    QStringList windowsOpened;
    QWidgetList windows(pWorkspace->windowList());
    for (int i = 0; i < windows.count(); ++i)
      windowsOpened << static_cast<FLFormDB *>(windows.at(i))->idMDI();
    FLSettings::writeEntry("windowsOpened/" + idM, windowsOpened);
  } else
    FLSettings::writeEntry("windowsOpened/" + idM, QStringList());

  QString k("Geometry/" + idM);
  FLSettings::writeEntry(k + "/Maximized", mainWidget_->isMaximized());
  FLSettings::writeEntry(k + "/X", mainWidget_->x());
  FLSettings::writeEntry(k + "/Y", mainWidget_->y());
  FLSettings::writeEntry(k + "/Width", mainWidget_->width());
  FLSettings::writeEntry(k + "/Height", mainWidget_->height());
}

void FLApplication::readState()
{
  if (container) {
    QRect r(container->pos(), container->size());

    multiLangEnabled_ = FLSettings::readBoolEntry("MultiLang/Enabled", false);
    multiLangId_ = FLSettings::readEntry("MultiLang/LangId", QString(QTextCodec::locale()).left(2)).upper();

    if (!FLSettings::readBoolEntry("Geometry/MainwindowMaximized", false)) {
      r.setX(FLSettings::readNumEntry("Geometry/MainwindowX", r.x()));
      r.setY(FLSettings::readNumEntry("Geometry/MainwindowY", r.y()));
      r.setWidth(FLSettings::readNumEntry("Geometry/MainwindowWidth", r.width()));
      r.setHeight(FLSettings::readNumEntry("Geometry/MainwindowHeight", r.height()));

      QRect desk = QApplication::desktop()->availableGeometry(container);
      QRect inter = desk.intersect(r);
      container->resize(r.size());
      if (inter.width() * inter.height() > (r.width() * r.height() / 20))
        container->move(r.topLeft());
    } else
      container->resize(QApplication::desktop()->availableGeometry(container).size());

    QString activeIdModule(db()->managerModules()->activeIdModule());

#if defined (FL_QUICK_CLIENT)
    if (activeIdModule == "sys")
      activeIdModule = QString::null;
#endif

    QStringList windowsOpened(FLSettings::readListEntry("windowsOpened/Main"));
    if (!windowsOpened.isEmpty()) {
      QStringList::Iterator it(windowsOpened.end());
      do {
        --it;
#if defined (FL_QUICK_CLIENT)
        if ((*it) == "sys")
          continue;
#endif
        QWidget *w = 0;
        if ((*it) != activeIdModule && db()->managerModules()->listAllIdModules().contains(*it)) {
          w = dictMainWidgets->find(*it);
          if (!w) {
            w = db()->managerModules()->createUI(*it + ".ui", this, 0, *it);
            dictMainWidgets->replace(*it, w);
            setName(*it);
            if (acl_)
              acl_->process(w);
            setMainWidget(w);
            call("init", QSArgumentList(), *it);
            db()->managerModules()->setActiveIdModule(*it);
            setMainWidget(w);
            initMainWidget();
          }
        }
      } while (it != windowsOpened.begin());
    }

    QDictIterator<QWidget> itD(*dictMainWidgets);
    QWidget *w;
    while ((w = itD.current()) != 0) {
      ++itD;
      if (w->name() != activeIdModule) {
        w->installEventFilter(this);
        w->show();
        w->setFont(font());
        QMainWindow *mw = ::qt_cast<QMainWindow *>(w);
        if (!mw)
          continue;
        QWidget *view_back = mw->centralWidget();
        if (view_back) {
          pWorkspace = static_cast<FLWorkspace *>(view_back->child(mw->name(), "QWorkspace"));
          view_back->show();
        }
      }
    }

    if (!activeIdModule.isEmpty()) {
      container->show();
      container->setFont(font());
    }
    activateModule(activeIdModule);
  }
}

void FLApplication::readStateModule()
{
  QString idM(db()->managerModules()->activeIdModule());
#if defined (FL_QUICK_CLIENT)
  if (idM == "sys")
    return;
#endif
  if (!mainWidget_ || idM.isEmpty() || mainWidget_->name() != idM)
    return;

  if (::qt_cast<QMainWindow *>(mainWidget_)) {
    QStringList windowsOpened(FLSettings::readListEntry("windowsOpened/" + idM));
    if (!windowsOpened.isEmpty()) {
      QAction *act = 0;
      for (QStringList::Iterator it = windowsOpened.begin(); it != windowsOpened.end(); ++it) {
        act = ::qt_cast<QAction *>(mainWidget_->child((*it), "QAction"));
        if (act && act->isVisible())
          openMasterForm((*it), act->iconSet().pixmap());
      }
    }
  }

  QRect r(mainWidget_->pos(), mainWidget_->size());
  QString k("Geometry/" + idM);
  if (!FLSettings::readBoolEntry(k + "/Maximized", false)) {
    r.setX(FLSettings::readNumEntry(k + "/X", r.x()));
    r.setY(FLSettings::readNumEntry(k + "/Y", r.y()));
    r.setWidth(FLSettings::readNumEntry(k + "/Width", r.width()));
    r.setHeight(FLSettings::readNumEntry(k + "/Height", r.height()));
    QRect desk = QApplication::desktop()->availableGeometry(mainWidget_);
    QRect inter = desk.intersect(r);
    mainWidget_->resize(r.size());
    if (inter.width() * inter.height() > (r.width() * r.height() / 20))
      mainWidget_->move(r.topLeft());
  } else
    mainWidget_->resize(QApplication::desktop()->availableGeometry(mainWidget_).size());
}

void FLApplication::loadScripts()
{
  AQ_SET_MNGLOADER

  setOverrideCursor(Qt::WaitCursor);
  QStringList listModules(mngLoader_->listAllIdModules());
  for (QStringList::Iterator it = listModules.begin(); it != listModules.end(); ++it)
    loadScriptsFromModule(*it);
  restoreOverrideCursor();

  AQ_UNSET_MNGLOADER
}

void FLApplication::loadTranslations()
{
  if (closingDown())
    return;

  AQ_SET_MNGLOADER

  QValueList<QTranslator *> translatorsCopy;
  if (translators && !translators->empty()) {
    translatorsCopy = *translators;
    QValueListConstIterator<QTranslator *> it(translatorsCopy.begin());
    while (it != translatorsCopy.end()) {
      removeTranslator(*it);
      ++it;
    }
  }

  QString lang(QString(QTextCodec::locale()).left(2));
  QStringList listModules = mngLoader_->listAllIdModules();
  for (QStringList::Iterator it = listModules.begin(); it != listModules.end(); ++it)
    loadTranslationFromModule(*it, lang);

  if (!translatorsCopy.empty()) {
    QValueListConstIterator<QTranslator *> it(translatorsCopy.begin());
    FLTranslator *item;
    while (it != translatorsCopy.end()) {
      item = ::qt_cast<FLTranslator *> (*it);
      if (item) {
        if (item->sysTrans_)
          installTranslator(item);
        else
          item->deleteLater();
      }
      ++it;
    }
  }

  AQ_UNSET_MNGLOADER
}

QString FLApplication::trMulti(const QString &s, const QString &l)
{
  bool backMultiEnabled = multiLangEnabled_;
  QString ret(translate(l.upper() + QString::fromLatin1("_MULTILANG"), s));
  multiLangEnabled_ = backMultiEnabled;
  return ret;
}

QString FLApplication::translate(const char *context, const char *sourceText,
                                 const char *comment, Encoding encoding) const
{
  if (!sourceText)
    return QString::null;

  if (!multiLangEnabled_ && sysContexts_.find(context) != 0) {
    QString result(sysTr_->findMessage(context, sourceText, comment).translation());
    if (!result.isNull())
      return result;
  } else if (translators) {
    QValueList<QTranslator *>::iterator it;
    FLTranslator *mf;
    QString result;
    for (it = translators->begin(); it != translators->end(); ++it) {
      mf = ::qt_cast<FLTranslator *> (*it);
      if (mf) {
        if (multiLangEnabled_) {
          if (mf->multiLang_)
            result = mf->findMessage(multiLangId_ + QString::fromLatin1("_MULTILANG"),
                                     sourceText, comment).translation();
          else
            continue;
        } else if (!mf->multiLang_)
          result = mf->findMessage(context, sourceText, comment).translation();
        else
          continue;
      } else
        result = (*it)->findMessage(context, sourceText, comment).translation();
      if (!result.isNull())
        return result;
    }
  }

#ifndef QT_NO_TEXTCODEC
  if (encoding == UnicodeUTF8)
    return QString::fromUtf8(sourceText);
  else if (QTextCodec::codecForTr() != 0)
    return QTextCodec::codecForTr()->toUnicode(sourceText);
  else
#endif

    return QString::fromLatin1(sourceText);
}

void FLApplication::setMultiLang(bool enable, const QString &langid)
{
  multiLangEnabled_ = enable;
  if (enable && !langid.isEmpty())
    multiLangId_ = langid.upper();
}

QTranslator *FLApplication::createSysTranslator(const QString &lang, bool loadDefault)
{
  QString fileTs(QString::fromLatin1("sys.") + lang + QString::fromLatin1(".ts"));
  QString key(QString::fromLatin1("sys") + lang + QString::fromLatin1("ts"));
  bool ok = true;

  if (AQ_DISKCACHE_FILEPATH(key).isEmpty()) {
    QFile fi(AQ_DATA + QString::fromLatin1("/translations/") + fileTs);
    ok = false;
    if (fi.open(IO_ReadOnly)) {
      QTextStream t(&fi);
      ok = AQ_DISKCACHE_INS(key, t.read());
    }
  }

  if (ok) {
    bool multiLang = (lang == "multilang");
    FLTranslator *tor = new FLTranslator(0, QString::fromLatin1("sys_") + lang,
                                         multiLang, true);

    if (tor->loadTsContent(key)) {
      if (!multiLang) {
        FLTranslations tmpTr;
        MetaTranslator tmpTor;

        tmpTr.loadTsFile(tmpTor, AQ_DATA + QString::fromLatin1("/translations/") +
                         fileTs, false);
        QValueList<MetaTranslatorMessage> list(tmpTor.messages());
        QValueList<MetaTranslatorMessage>::const_iterator it(list.begin());

        const char *itCtx;
        while (it != list.end()) {
          itCtx = (*it).context();
          if (qstrcmp(itCtx, "MetaData") == 0 ||
              qstrcmp(itCtx, "Queries") == 0 ||
              qstrcmp(itCtx, "scripts") == 0) {
            ++it;
            continue;
          }
          if (sysContexts_.find(itCtx) == 0)
            sysContexts_.insert(itCtx, &multiLangEnabled_);
          ++it;
        }

        sysTr_ = tor;
      }
      return tor;
    }
  }

  return (loadDefault ? createSysTranslator("es") : 0);
}

QTranslator *FLApplication::createModTranslator(const QString &idM, const QString &lang,
                                                bool loadDefault) const
{
  QString fileTs(idM + "." + lang + ".ts");
  QString key(mngLoader_->shaOfFile(fileTs));
  bool ok = !key.isEmpty();

  if (ok && AQ_DISKCACHE_FILEPATH(key).isEmpty())
    ok = AQ_DISKCACHE_INS(key, mngLoader_->content(fileTs));

  if (ok) {
    FLTranslator *tor = new FLTranslator(0, idM + QString::fromLatin1("_") + lang,
                                         lang == "multilang");
    if (tor->loadTsContent(key))
      return tor;
  }

  return (loadDefault ? createModTranslator(idM, "es") : 0);
}

void FLApplication::loadTranslationFromModule(const QString &idM, const QString &lang)
{
  installTranslator(createModTranslator(idM, lang, true));
  installTranslator(createModTranslator(idM, "multilang"));
}

void FLApplication::loadScriptsFromModule(const QString &idM)
{
  QDomDocument doc("scripts");

  QString actionsXml(mngLoader_->contentCached(idM + QString::fromLatin1(".xml")));
  if (idM == "sys") {
    QString actionsXmlPlus(mngLoader_->contentCached("plus_sys.xml"));
    if (!actionsXmlPlus.isEmpty())
      actionsXml.replace("</ACTIONS>", actionsXmlPlus);
  }

  if (!FLUtil::domDocumentSetContent(doc, actionsXml)) {
#ifdef FL_DEBUG
    qWarning("FLApplication : " +
             QApplication::tr("Error al cargar las acciones del módulo %1").arg(idM));
#endif
    return;
  }

  QString extqs(".qs");
  QString stream(mngLoader_->contentCode(idM + extqs));
  if (idM == "sys") {
    QString streamPlus(mngLoader_->contentCode("plus_sys.qs"));
    if (!streamPlus.isEmpty())
      stream += streamPlus;
  }

  FLApplicationInterface *ap = static_cast<FLApplicationInterface *>(project_->object(idM));
  if (!ap) {
    ap = new FLApplicationInterface(this);
    ap->setName(idM);
    project_->addObject(ap);
    QSScript *scr = project_->createScript(ap, stream);
    scr->setBaseFileName(idM + extqs);
  } else {
    QSScript *scr = project_->script(ap);
    if (scr) {
      scr->setCode(stream);
    } else {
      scr = project_->createScript(ap, stream);
      scr->setBaseFileName(idM + extqs);
    }
  }

  QDomNode no(doc.documentElement().firstChild());
  QString scriptForm, scriptFormRecord, name;
  QString preFm("form");
  QString preFs("formSearch");
  QString preFr("formRecord");
  int matchs;

  while (!no.isNull()) {
    QDomElement e(no.toElement());
    if (!e.isNull()) {
      if (e.tagName() == "action") {
        QDomNode no2(e.firstChild());
        scriptForm = scriptFormRecord = name = stream = QString::null;
        matchs = 0;

        while (!no2.isNull()) {
          QDomElement e2(no2.toElement());
          if (!e2.isNull()) {
            if (e2.tagName() == "name") {
              name = e2.text();
              no2 = no2.nextSibling();
              if (++matchs == 3)
                break;
              continue;
            }
            if (e2.tagName() == "scriptformrecord") {
              scriptFormRecord = e2.text() + extqs;
              no2 = no2.nextSibling();
              if (++matchs == 3)
                break;
              continue;
            }
            if (e2.tagName() == "scriptform") {
              scriptForm = e2.text() + extqs;
              no2 = no2.nextSibling();
              if (++matchs == 3)
                break;
              continue;
            }
          }
          no2 = no2.nextSibling();
        }

        if (!scriptForm.isEmpty()) {
          FLFormDBInterface *f = static_cast<FLFormDBInterface *>(project_->object(preFm + name));
          if (!f) {
            f = new FLFormDBInterface(0);
            f->setName(preFm + name);
            project_->addObject(f);
            stream = mngLoader_->contentCode(scriptForm);
            QSScript *scr = project_->createScript(f, stream);
            scr->setBaseFileName(scriptForm);
            f->setScript(scr);
          } else {
            QSScript *scr = f->script();
            if (!scr) {
              scr = project_->script(f);
              if (scr)
                f->setScript(scr);
              else {
                stream = mngLoader_->contentCode(scriptForm);
                scr = project_->createScript(f, stream);
                scr->setBaseFileName(scriptForm);
                f->setScript(scr);
              }
            } else {
              stream = mngLoader_->contentCode(scriptForm);
              if (QString::compare(scr->code(), stream))
                scr->setCode(stream);
            }
          }

          FLFormSearchDBInterface *fs = static_cast<FLFormSearchDBInterface *>(project_->object(preFs + name));
          if (!fs) {
            fs = new FLFormSearchDBInterface(0);
            fs->setName(preFs + name);
            project_->addObject(fs);
            if (stream.isEmpty())
              stream = mngLoader_->contentCode(scriptForm);
            QSScript *scr = project_->createScript(fs, stream);
            scr->setBaseFileName(scriptForm);
            fs->setScript(scr);
          } else {
            QSScript *scr = fs->script();
            if (!scr) {
              scr = project_->script(fs);
              if (scr)
                fs->setScript(scr);
              else {
                if (stream.isEmpty())
                  stream = mngLoader_->contentCode(scriptForm);
                scr = project_->createScript(fs, stream);
                scr->setBaseFileName(scriptForm);
                fs->setScript(scr);
              }
            } else {
              if (stream.isEmpty())
                stream = mngLoader_->contentCode(scriptForm);
              if (QString::compare(scr->code(), stream))
                scr->setCode(stream);
            }
          }
        }

        if (!scriptFormRecord.isEmpty()) {
          FLFormRecordDBInterface *fr = static_cast<FLFormRecordDBInterface *>(project_->object(preFr + name));
          if (!fr) {
            fr = new FLFormRecordDBInterface(0);
            fr->setName(preFr + name);
            project_->addObject(fr);
            stream = mngLoader_->contentCode(scriptFormRecord);
            QSScript *scr = project_->createScript(fr, stream);
            scr->setBaseFileName(scriptFormRecord);
            fr->setScript(scr);
          } else {
            QSScript *scr = fr->script();
            if (!scr) {
              scr = project_->script(fr);
              if (scr)
                fr->setScript(scr);
              else {
                stream = mngLoader_->contentCode(scriptFormRecord);
                scr = project_->createScript(fr, stream);
                scr->setBaseFileName(scriptFormRecord);
                fr->setScript(scr);
              }
            } else {
              stream = mngLoader_->contentCode(scriptFormRecord);
              if (QString::compare(scr->code(), stream))
                scr->setCode(stream);
            }
          }
        }

        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }
}

void FLApplication::activateModule(const QString &idM)
{
#if defined (FL_QUICK_CLIENT)
  if (idM == "sys")
    return;
#endif
  writeStateModule();

  QWidget *w = 0;
  if (db()->managerModules()->listAllIdModules().contains(idM)) {
    w = dictMainWidgets->find(idM);
    if (!w) {
      w = db()->managerModules()->createUI(idM + QString::fromLatin1(".ui"),
                                           this, 0, idM);
      if (!w)
        return;
      dictMainWidgets->replace(idM, w);
      setName(idM);
      if (acl_)
        acl_->process(w);
      setMainWidget(w);
      call("init", QSArgumentList(), idM);
      w->removeEventFilter(this);
      db()->managerModules()->setActiveIdModule(idM);
      setMainWidget(w);
      initMainWidget();
      showMainWidget(w);
      w->installEventFilter(this);
      return;
    }
  }

  if (!w)
    db()->managerModules()->setActiveIdModule("");
  else
    db()->managerModules()->setActiveIdModule(idM);
  setMainWidget(w);
  showMainWidget(w);
}

void FLApplication::activateModule()
{
  FLWidgetAction *s = ::qt_cast<FLWidgetAction *>(sender());
  if (!s) {
#ifdef FL_DEBUG
    qWarning(tr("FLApplication : El objeto que envia señales activar módulos debe ser de la clase FLWidgetAction"));
#endif
    return;
  }
  QString shaG(db()->managerModules()->shaGlobal());
  if (shaG == "error")
    return;
  if (db()->managerModules()->shaLocal() != shaG)
    reinit();
  QString idM(s->idModule());
#if defined (FL_QUICK_CLIENT)
  if (idM == "sys")
    return;
#endif
  activateModule(idM);
}

void FLApplication::reinit()
{
  if (initializing_)
    return;

  initializing_ = true;
  writeState();
  writeStateModule();

  QTimer::singleShot(0, this, SLOT(reinitP()));
}

void FLApplication::clearProject()
{
  if (wb_) {
    wb_->close();
    wb_->deleteLater();
    wb_ = 0;
  }

  if (!project_)
    return;

  QSInterpreter *i = project_->interpreter();
  if (i) {
    i->stopExecution();
    i->clear();
  }

  project_->blockSignals(true);
  project_->clearObjects();

  QObjectList probjList(project_->objects());
  QPtrListIterator<QObject> itPrObj(probjList);
  QObject *prObj;
  while ((prObj = itPrObj()) != 0) {
    if (!prObj->aqWasDeleted())
      delete prObj;
  }
  QPtrList<QSScript> prscrList(project_->scripts());
  QPtrListIterator<QSScript> itScrObj(prscrList);
  QSScript *scrObj;
  while ((scrObj = itScrObj()) != 0) {
    if (!scrObj->aqWasDeleted())
      delete scrObj;
  }
  project_->blockSignals(false);
}

void FLApplication::reinitP()
{
  db()->managerModules()->finish();
  db()->manager()->finish();
  setMainWidget(0);
  db()->managerModules()->setActiveIdModule("");

  dictMainWidgets->setAutoDelete(true);
  dictMainWidgets->clear();
  dictMainWidgets->setAutoDelete(false);

  clearProject();

  db()->manager()->init();
  db()->managerModules()->init();
  db()->manager()->cleanupMetaData();
  FLDomNodeInterface::cleanupDomNodesCache();

  if (acl_)
    acl_->init();

  loadScripts();
  db()->managerModules()->setShaLocalFromGlobal();

  call("init", QSArgumentList(), "sys");
  initToolBox();
  readState();

  if (container) {
    container->installEventFilter(this);
    container->setDisabled(false);
  }

  callScriptEntryFunction();

  initializing_ = false;
}

void FLApplication::showDocPage(const QString &url)
{
  FLHelpWindow *docPage = new FLHelpWindow(url, ".", 0, "help viewer");
  docPage->show();
}

FLWorkspace *FLApplication::workspace() const
{
  return pWorkspace;
}

const QDateTime &FLApplication::timeUser() const
{
  return timeUser_;
}


static QMap<QString, QString> aqCallsCache_;

QSArgument FLApplication::call(const QString &function,
                               const QSArgumentList &arguments,
                               QObject *objectContext) const
{
  QSInterpreter *i = project_->interpreter();
  QString fKey;

  if (!objectContext) {
    fKey = function;
    QMap<QString, QString>::const_iterator it(aqCallsCache_.find(fKey));

    if (it != aqCallsCache_.end())
      return ((*it).isEmpty() ? QSArgument() : i->call(*it, arguments));

    if (!i->hasFunction(function)) {
      QString dot(".");
      QString dotIface(".iface.");
      QStringList sL(QStringList::split('.', function));
      QStringList classes(i->classes(sL[ 0 ]));

      for (QStringList::const_iterator it = classes.begin(); it != classes.end(); ++it) {
        if (i->hasFunction(sL[ 0 ] + dot + *it + dot + sL[ 1 ])) {
          QString fn(sL[ 0 ] + dotIface + sL[ 1 ]);
          aqCallsCache_.insert(fKey, fn);
          return i->call(fn, arguments);
        }
      }
    } else {
      aqCallsCache_.insert(fKey, function);
      return i->call(function, arguments);
    }
  } else {
    QString objName(objectContext->name());
    fKey = objName + function;
    QMap<QString, QString>::const_iterator it(aqCallsCache_.find(fKey));

    if (it != aqCallsCache_.end())
      return ((*it).isEmpty() ? QSArgument() : i->call(*it, arguments, objectContext));

    QString dot(".");

    if (!i->hasFunction(objName + dot + function)) {
      QString iface("iface.");
      QStringList classes(i->classes(objectContext));
      for (QStringList::const_iterator it = classes.begin(); it != classes.end(); ++it) {
        if (i->hasFunction(objName + dot + *it + dot + function)) {
          QString fn(iface + function);
          aqCallsCache_.insert(fKey, fn);
          return i->call(fn, arguments, objectContext);
        }
      }
    } else {
      aqCallsCache_.insert(fKey, function);
      return i->call(function, arguments, objectContext);
    }
  }

  aqCallsCache_.insert(function, QString::null);
  return QSArgument();
}

void FLApplication::setCaptionMainWidget(const QString &text)
{
  if (!text.isEmpty())
    lastTextCaption_ = text;

  if (!mainWidget_) {
    QWidget *mwi = aqApp->mainWidget();
    if (mwi) {
      QString bd(db()->driverNameToDriverAlias(db()->driverName()));
      mwi->setCaption(QString::fromLatin1("Eneboo " AQ_VERSION) +
                      "  - [ " + lastTextCaption_ + " ] - [" + bd + " [*] " +
                      db()->database() + " [*] " + db()->user() + "]");
    }
    return;
  }

  AQ_SET_MNGLOADER

  QString bd(db()->driverNameToDriverAlias(db()->driverName()));
  QString descripArea(mngLoader_->idAreaToDescription(mngLoader_->activeIdArea()));
  QString descripModule(mngLoader_->idModuleToDescription(mainWidget_->name()));
  mainWidget_->setCaption(descripArea + "::" + descripModule +
                          "  - [ " + lastTextCaption_ + " ] - [" + bd + " [*] " +
                          db()->database() + " [*] " + db()->user() + "]");

  AQ_UNSET_MNGLOADER
}


void FLApplication::setNotExit(const bool &b)
{
  notExit_ = b;
}

void FLApplication::setNoGUI(const bool &b)
{
  noGUI_ = b;
}

void FLApplication::printTextEdit(QTextEdit *editor)
{
  if (!editor || !mainWidget_)
    return;

#if defined(Q_OS_WIN32)
  QPrinter printer(QPrinter::HighResolution);
#else
  QPrinter printer(QPrinter::ScreenResolution);
#endif
  printer.setFullPage(true);
  if (printer.setup(mainWidget_)) {
    QPainter p(&printer);
    if (!p.device())
      return;
    QPaintDeviceMetrics metrics(p.device());
    int dpiy = metrics.logicalDpiY();
    int margin = (int)((2 / 2.54) * dpiy);
    QRect view(margin, margin, metrics.width() - 2 * margin, metrics.height() - 2 * margin);
    QFont font(editor->QWidget::font());
    font.setPointSize(10);

    QSimpleRichText richText(editor->text(), font, editor->context(), editor->styleSheet(),
                             editor->mimeSourceFactory(), view.height());
    richText.setWidth(&p, view.width());
    int page = 1;
    do {
      richText.draw(&p, margin, margin, view, mainWidget_->colorGroup());
      view.moveBy(0, view.height());
      p.translate(0 , -view.height());
      p.setFont(font);
      p.drawText(view.right() - p.fontMetrics().width(QString::number(page)),
                 view.bottom() + p.fontMetrics().ascent() + 5, QString::number(page));
      if (view.top() - margin >= richText.height())
        break;
      printer.newPage();
      ++page;
    } while (true);
  }
}

void FLApplication::setPrintProgram(const QString &printProgram)
{
  printProgram_ = printProgram;
}

const QString &FLApplication::printProgram() const
{
  return printProgram_;
}

bool FLApplication::noGUI() const
{
  return noGUI_;
}

FLSqlDatabase *FLApplication::db()
{
  return FLSqlConnections::database();
}

bool FLApplication::addSysCode(const QString &code, const QString &scriptEntryFunction)
{
  if (code.isEmpty() || !project_->interpreter()->checkSyntax(code))
    return false;

  QString sha1(FLUtil::sha1(code));
  QSScript *scr = project_->script("sys");
  QString scrCode(scr->code());
  QString addCode = "\n\n// " + sha1 + "\n" + code + "\n";

  if (!scrCode.contains(sha1))
    scrCode += addCode;

  if (!project_->interpreter()->checkSyntax(scrCode))
    return false;

  db()->managerModules()->setContent("plus_sys.qs", "sys", addCode);

  if (!scriptEntryFunction.isEmpty())
    scriptEntryFunction_ = scriptEntryFunction;

  return true;
}

void FLApplication::setScriptEntryFunction(const QString &scriptEntryFunction)
{
  scriptEntryFunction_ = scriptEntryFunction;
}

void FLApplication::setDatabaseLockDetection(bool on, int msecLapsus, int limChecks,
                                             bool showWarn, const QString &msgWarn, const QString &connectionName)
{

  QString timerId("TimerDbLock_" + connectionName);
  FLTimerDbLock *timerDbLock = ::qt_cast<FLTimerDbLock *>(child(timerId, "FLTimerDbLock", false));

  if (timerDbLock) {
    timerDbLock->stop();
    if (on) {
      QString defaultMsgWarn(tr("<p><img source=\"locked.png\" align=\"right\"><b><u>Bloqueo Detectado</u></b><br><br>"
                                "Su transacción actual está bloqueando a otros usuarios que han solicitado "
                                "acceder a datos que acaba de crear o modificar pero que aún no han sido "
                                "aceptados.<br><br>Si le es posible postponer la operación que está realizando se "
                                "recomienda guardar los cambios que ha efectuado aceptando todos los formularios "
                                "y esperar unos segundos antes de continuar."));
      QString moreInfo(tr("<br><br><em><a href=\"sys.statusDbLocksDialog\">Más información</a></em></p>"));

      timerDbLock->dbLocksChecks_ = limChecks;
      timerDbLock->dbLocksLapsus_ = (msecLapsus < 0 ? 0 : msecLapsus);
      timerDbLock->dbLocksShowWarn_ = showWarn;
      timerDbLock->dbLocksMsgWarn_ = (!msgWarn.isEmpty() ? msgWarn : defaultMsgWarn) + moreInfo;
      timerDbLock->dbLocksConn_ = connectionName;
      if (timerDbLock->dbLocksLapsus_)
        timerDbLock->start(timerDbLock->dbLocksLapsus_);
      else
        checkDatabaseLocks(timerDbLock);
    } else
      timerDbLock->deleteLater();
  } else if (on) {
    QString defaultMsgWarn(tr("<p><img source=\"locked.png\" align=\"right\"><b><u>Bloqueo Detectado</u></b><br><br>"
                              "Su transacción actual está bloqueando a otros usuarios que han solicitado "
                              "acceder a datos que acaba de crear o modificar pero que aún no han sido "
                              "aceptados.<br><br>Si le es posible postponer la operación que está realizando se "
                              "recomienda guardar los cambios que ha efectuado aceptando todos los formularios "
                              "y esperar unos segundos antes de continuar."));
    QString moreInfo(tr("<br><br><em><a href=\"sys.statusDbLocksDialog\">Más información</a></em></p>"));

    timerDbLock = new FLTimerDbLock(this, timerId);
    timerDbLock->dbLocksChecks_ = limChecks;
    timerDbLock->dbLocksLapsus_ = (msecLapsus < 0 ? 0 : msecLapsus);
    timerDbLock->dbLocksShowWarn_ = showWarn;
    timerDbLock->dbLocksMsgWarn_ = (!msgWarn.isEmpty() ? msgWarn : defaultMsgWarn) + moreInfo;
    timerDbLock->dbLocksConn_ = connectionName;
    connect(timerDbLock, SIGNAL(timeout()), this, SLOT(checkDatabaseLocks()));
    if (timerDbLock->dbLocksLapsus_)
      timerDbLock->start(timerDbLock->dbLocksLapsus_);
    else
      checkDatabaseLocks(timerDbLock);
  }
}

void FLApplication::popupWarn(const QString &msgWarn,
                              const QMap<QString, QSArgumentList> & scriptCalls)
{
  if (!popupWarn_)
    popupWarn_ = new FLPopupWarn(container);
  popupWarn_->scriptCalls_ = scriptCalls;
  QWhatsThis::display(msgWarn, QApplication::desktop()->mapToGlobal(QPoint(5, 5)), container);
}

void FLApplication::popupWarn(const QString &msgWarn)
{
  QWhatsThis::display(msgWarn, QApplication::desktop()->mapToGlobal(QPoint(5, 5)), container);
  processEvents(50);
}

void FLApplication::checkDatabaseLocks(QObject *timer)
{
  FLTimerDbLock *timerDbLock = ::qt_cast<FLTimerDbLock *>(timer ? timer : sender());
  if (timerDbLock) {
    timerDbLock->stop();

    FLSqlDatabase *database = FLSqlConnections::database(timerDbLock->dbLocksConn_);
    bool showed = false;

    if (database && database->canDetectLocks()) {
      QStringList locks(database->detectLocks());
      if (!locks.isEmpty() || timer) {
        if (timerDbLock->dbLocksShowWarn_) {
          showed = true;
          QMap<QString, QSArgumentList> scrCall;
          scrCall["sys.statusDbLocksDialog"] = QSArgumentList(locks);
          popupWarn(timerDbLock->dbLocksMsgWarn_, scrCall);
        }
        emit databaseLockDetected();
      }
    }
    if (timerDbLock->dbLocksChecks_ != 0) {
      if (timerDbLock->dbLocksChecks_ > 0)
        timerDbLock->dbLocksChecks_--;
      if (showed)
        timerDbLock->start(60000);
      else
        timerDbLock->start(timerDbLock->dbLocksLapsus_);
    } else
      timerDbLock->deleteLater();
  }
}

void FLApplication::saveGeometryForm(const QString &name, const QRect &geo)
{
  mapGeometryForms_[ name ] = geo;
}

QRect FLApplication::geometryForm(const QString &name) const
{
  QRect r;

  if (!mapGeometryForms_.contains(name)) {
    QString k("Geometry/" + name + "/");
    QRect rconfig;
    bool ok = false;

    rconfig.setX(FLSettings::readNumEntry(k + "X", 0, &ok));
    if (ok) {
      rconfig.setY(FLSettings::readNumEntry(k + "Y"));
      rconfig.setWidth(FLSettings::readNumEntry(k + "Width"));
      rconfig.setHeight(FLSettings::readNumEntry(k + "Height"));
      r = rconfig;
    }
  } else
    r = mapGeometryForms_[ name ];

  return r;
}

void FLApplication::staticLoaderSetup()
{
  db()->managerModules()->staticLoaderSetup();
}

void FLApplication::loadModules()
{
  call("sys.loadModules", QSArgumentList(), 0);
}

void FLApplication::exportModules()
{
  call("sys.exportModules", QSArgumentList(), 0);
}

void FLApplication::importModules()
{
  call("sys.importModules", QSArgumentList(), 0);
}

void FLApplication::updateAbanQ()
{
  call("sys.updateAbanQ", QSArgumentList(), 0);
}

QWidget *FLApplication::modMainWidget(const QString &idModulo) const
{
  QWidget *w = dictMainWidgets->find(idModulo);
  if (!w) {
    QWidgetList *list = QApplication::topLevelWidgets();
    QWidgetListIt it(*list);
    while ((w = it.current())) {
      ++it;
      if (QString::fromLatin1(w->name()) == idModulo)
        break;
    }
    delete list;
  }
  if (!w && aqApp->mainWidget())
    w = static_cast<QWidget *>(aqApp->mainWidget()->child(idModulo, "QWidget"));
  return w;
}

void FLApplication::evaluateProject()
{
  if (project_) {
    project_->commitEditorContents();
    QTimer::singleShot(0, project_, SLOT(evaluate()));
    QTimer::singleShot(0, this, SLOT(callScriptEntryFunction()));
  }
}

void FLApplication::callScriptEntryFunction()
{
  if (!scriptEntryFunction_.isEmpty()) {
    call(scriptEntryFunction_, QSArgumentList(), aqApp);
    scriptEntryFunction_ = QString::null;
  }
}

FLWorkspace::FLWorkspace(QWidget *parent, const char *name) : QWorkspace(parent, name)
{
  logo = QImage::fromMimeSource("mini_logo_abanq.png");
  fColor.setRgb(AQ_RGB_LOGO);
  pColor.setRgb(164, 170, 180);
}

void FLWorkspace::paintEvent(QPaintEvent *pe)
{
  QPainter p(this);
  p.fillRect(pe->rect(), fColor);

  int dx = width() - logo.width();
  int dy = height() - logo.height();
  p.drawImage(QPoint(dx, dy), logo);

  p.setPen(pColor);
  p.setBrush(pColor);
  //p.drawRect(dx + 23, 0, 2, dy);

  p.end();

  QWidget::paintEvent(pe);
}

FLWidget::FLWidget(QWidget *parent, const char *name,  WFlags f) : QWidget(parent, name, f)
{
  logo = QImage::fromMimeSource("mini_logo_abanq.png");
  fColor.setRgb(AQ_RGB_LOGO);
  pColor.setRgb(164, 170, 180);
}

void FLWidget::paintEvent(QPaintEvent *pe)
{
  QPainter p(this);
  p.fillRect(pe->rect(), fColor);

  int dx = width() - logo.width();
  int dy = height() - logo.height();
  p.drawImage(QPoint(dx, dy), logo);

  p.setPen(pColor);
  p.setBrush(pColor);
  //p.drawRect(dx + 23, 0, 2, dy);

  p.end();

  QWidget::paintEvent(pe);
}
