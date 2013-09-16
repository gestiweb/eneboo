/***************************************************************************
 FLFormDB.cpp
 -------------------
 begin                : Sat Jul 28 2001
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

#include <qsworkbench.h>

#include "FLFormDB.h"
#include "FLSqlCursor.h"
#include "FLTableMetaData.h"
#include "FLObjectFactory.h"
#include "FLAction.h"
#include "FLManager.h"
#include "FLApplication.h"
#include "FLManagerModules.h"
#include "FLAccessControlLists.h"
#include "FLSqlConnections.h"

FLFormDB::FLFormDB(QWidget *parent, const char *name, WFlags f) :
  QWidget(parent ? parent : aqApp->mainWidget(), name, f),
  cursor_(0), layout(0), mainWidget_(0), layoutButtons(0), pushButtonCancel(0), showed(false),
  iface(0), oldCursorCtxt(0), isClosing_(false), initFocusWidget_(0), oldFormObj(0),
  accepted_(false)
{
#ifdef QSDEBUGGER
  pushButtonDebug = 0;
#endif
}

FLFormDB::FLFormDB(const QString &actionName, QWidget *parent, WFlags f) :
  QWidget(parent ? parent : aqApp->mainWidget(), actionName, f),
  layout(0), mainWidget_(0), layoutButtons(0), pushButtonCancel(0), showed(false), iface(0),
  oldCursorCtxt(0), isClosing_(false), initFocusWidget_(0), oldFormObj(0),
  accepted_(false)
{
#ifdef QSDEBUGGER
  pushButtonDebug = 0;
#endif

  setFocusPolicy(QWidget::NoFocus);

  if (actionName.isEmpty()) {
    action_ = 0;
#ifdef FL_DEBUG
    qWarning(tr("FLFormDB : Nombre de acción vacío"));
#endif
    return;
  } else
    action_ = FLSqlConnections::database()->manager()->action(actionName);
  if (!action_) {
#ifdef FL_DEBUG
    qWarning(tr("FLFormDB : No existe la acción " + actionName));
#endif
    return;
  }
  cursor_ = new FLSqlCursor(action_->table(), true, "default", 0, 0, this);
  name_ = action_->name();

  initForm();
}

FLFormDB::FLFormDB(FLSqlCursor *cursor, const QString &actionName, QWidget *parent, WFlags f) :
  QWidget(parent ? parent : aqApp->mainWidget(), actionName, f),
  cursor_(cursor), layout(0), mainWidget_(0), layoutButtons(0), pushButtonCancel(0),
  showed(false), iface(0), oldCursorCtxt(0), isClosing_(false), initFocusWidget_(0),
  oldFormObj(0), accepted_(false)
{
  setFocusPolicy(QWidget::NoFocus);

  if (actionName.isEmpty())
    action_ = 0;
  else if (cursor)
    action_ = cursor->db()->manager()->action(actionName);
  else
    action_ = FLSqlConnections::database()->manager()->action(actionName);
  name_ = (action_ ? action_->name() : QString::null);

  initForm();
}

FLFormDB::~FLFormDB()
{
  unbindIface();
}

bool FLFormDB::close()
{
  if (isClosing_)
    return true;
  isClosing_ = true;
  isClosing_ = QWidget::close();
}

void FLFormDB::initForm()
{
  if (cursor_ && cursor_->metadata()) {
    QString caption;
    if (action_) {
      cursor_->setAction(action_);
      caption = action_->caption();
      if (!action_->description().isEmpty())
        QWhatsThis::add(this, action_->description());
      idMDI_ = action_->name();
    }

    if (caption.isEmpty())
      caption = cursor_->metadata()->alias();
    setCaption(caption);

    bindIface();
    setCursor(cursor_);
  } else
    setCaption(tr("No hay metadatos"));
}

void FLFormDB::setMainWidget(QWidget *w)
{
  if (!cursor_ || !w)
    return;

  if (showed) {
    if (mainWidget_ && mainWidget_ != w)
      initMainWidget(w);
  } else
    w->hide();

  if (layout)
    delete layout;

  w->setFont(qApp->font());
  layout = new QVBoxLayout(this, 2, 3, "vlay" + name_);
  layout->add(w);
  layoutButtons = new QHBoxLayout(layout, 3, "hlay" + name_);

  QSize pbSize(22, 22);

  QToolButton *wt = QWhatsThis::whatsThisButton(this);
  wt->setIconSet(QPixmap::fromMimeSource("about.png"));
  layoutButtons->addWidget(wt);
  wt->show();

#ifdef QSDEBUGGER
  pushButtonDebug = new QPushButton(this, "pushButtonDebug");
  pushButtonDebug->setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 0, (QSizePolicy::SizeType) 0, 0, 0,
                                             pushButtonDebug->sizePolicy().hasHeightForWidth()));
  pushButtonDebug->setMinimumSize(pbSize);
  pushButtonDebug->setMaximumSize(pbSize);
  QPixmap qsa(QPixmap::fromMimeSource("bug.png"));
  pushButtonDebug->setIconSet(qsa);
  pushButtonDebug->setAccel(QKeySequence(Qt::Key_F3));
  QToolTip::add(pushButtonDebug, tr("Abrir Depurador (F3)"));
  QWhatsThis::add(pushButtonDebug, tr("Abrir Depurador (F3)"));
  pushButtonDebug->setFocusPolicy(QWidget::NoFocus);
  layoutButtons->addWidget(pushButtonDebug);
  connect(pushButtonDebug, SIGNAL(clicked()), this, SLOT(debugScript()));
#endif

  layoutButtons->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

  pushButtonCancel = new QPushButton(this, "pushButtonCancel");
  pushButtonCancel->setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 0, (QSizePolicy::SizeType) 0,
                                              0, 0,
                                              pushButtonCancel->sizePolicy().hasHeightForWidth()));
  pushButtonCancel->setMinimumSize(pbSize);
  pushButtonCancel->setMaximumSize(pbSize);
  QPixmap cancel(QPixmap::fromMimeSource("button_cancel.png"));
  pushButtonCancel->setIconSet(cancel);
  pushButtonCancel->setFocusPolicy(QWidget::NoFocus);
  pushButtonCancel->setAccel(QKeySequence(tr("Esc")));
  QToolTip::add(pushButtonCancel, tr("Cerrar formulario (Esc)"));
  QWhatsThis::add(pushButtonCancel, tr("Cerrar formulario (Esc)"));
  layoutButtons->addWidget(pushButtonCancel);
  connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(close()));
  pushButtonCancel->show();

  mainWidget_ = w;
}

bool FLFormDB::initScript()
{
  if (iface) {
    aqApp->call("init", QSArgumentList(), iface);
    return true;
  }
  return false;
}

void FLFormDB::accept()
{
}

void FLFormDB::reject()
{
}

void FLFormDB::setMainWidget()
{
  if (!action_)
    return;
  if (cursor_)
    setMainWidget(cursor_->db()->managerModules()->createForm(action_, this, this));
  else
    setMainWidget(FLSqlConnections::database()->managerModules()->createForm(action_, this, this));
}

void FLFormDB::setMainWidget(const QString &uiFileName)
{
  if (cursor_)
    setMainWidget(cursor_->db()->managerModules()->createUI(uiFileName, this, this));
  else
    setMainWidget(FLSqlConnections::database()->managerModules()->createUI(uiFileName, this, this));
}

void FLFormDB::closeEvent(QCloseEvent *e)
{
  frameGeometry();
  if (focusWidget()) {
    FLFieldDB *fdb = ::qt_cast<FLFieldDB *>(focusWidget()->parentWidget());
    if (fdb && fdb->autoComFrame_ && fdb->autoComFrame_->isVisible()) {
      fdb->autoComFrame_->hide();
      return;
    }
  }

  setCursor(0);

  emit closed();
  QWidget::closeEvent(e);
  deleteLater();
}

void FLFormDB::hideEvent(QHideEvent *h)
{
  QWidget *pW = parentWidget();
  if (pW && pW->isA("QWorkspaceChild")) {
    QRect geo(pW->x(), pW->y(), pW->width(), pW->height());
    if (isMinimized()) {
      geo.setWidth(1);
      aqApp->saveGeometryForm(geoName(), geo);
    } else if (isMaximized()) {
      geo.setWidth(9999);
      aqApp->saveGeometryForm(geoName(), geo);
    } else
      aqApp->saveGeometryForm(geoName(), geo);
  } else {
    QRect geo(x(), y(), width(), height());
    aqApp->saveGeometryForm(geoName(), geo);
  }
}

void FLFormDB::showEvent(QShowEvent *e)
{
  if (!showed && mainWidget_) {
    showed = true;
    if (cursor_ && iface) {
      QVariant v(aqApp->call("preloadMainFilter", QSArgumentList(), iface).variant());
      if (v.isValid() && v.type() == QVariant::String)
        cursor_->setMainFilter(v.toString(), false);
      v = aqApp->call("preloadOrder", QSArgumentList(), iface).variant();
      if (v.isValid() && v.type() == QVariant::String)
        cursor_->setOrder(QStringList::split(',', v.toString()));
    }
    initMainWidget();
    callInitScript();
  }
  if (!isIfaceBind())
    bindIface();
}

void FLFormDB::callInitScript()
{
  if (!initScript())
    return;
  if (!isClosing_ && !aqApp->project()->interpreter()->hadError())
    QTimer::singleShot(0, this, SLOT(emitFormReady()));
}

void FLFormDB::initMainWidget(QWidget *w)
{
  QWidget *mWidget = w ? w : mainWidget_;
  if (mWidget) {
    QObjectList *l = static_cast<QObject *>(mWidget)->queryList("FLTableDB");
    QObjectListIt itt(*l);
    FLTableDB *tdb;
    while ((tdb = static_cast<FLTableDB *>(itt.current())) != 0) {
      ++itt;
      tdb->initCursor();
    }
    delete l;

    l = static_cast<QObject *>(mWidget)->queryList("FLFieldDB");
    QObjectListIt itf(*l);
    FLFieldDB *fdb;
    while ((fdb = static_cast<FLFieldDB *>(itf.current())) != 0) {
      ++itf;
      fdb->initCursor();
      fdb->initEditor();
      if (fdb->aqFirstTabStop == 1)
        initFocusWidget_ = fdb;
    }

    while (mWidget->parentWidget() && mWidget->parentWidget() != this)
      mWidget = mWidget->parentWidget();
    mWidget->show();

    FLAccessControlLists *acl = aqApp->acl();
    if (acl)
      acl->process(this);

    QWidget *pW = parentWidget();

    if (pW && pW->isA("QWorkspaceChild")) {
      QRect geo(aqApp->geometryForm(geoName()));
      if (geo.width() == 9999) {
        pW->resize(size().expandedTo(mWidget->size()));
        pW->showMaximized();
      } else if (geo.width() == 1) {
        pW->resize(size().expandedTo(mWidget->size()));
        pW->showMinimized();
      } else if (geo.isValid()) {
        QRect desk = QApplication::desktop()->availableGeometry(this);
        QRect inter = desk.intersect(geo);
        pW->resize(geo.size());
        if (inter.width() * inter.height() > (geo.width() * geo.height() / 20))
          pW->move(geo.topLeft());
      } else
        pW->resize(size().expandedTo(mWidget->size()));
    } else {
      QRect geo(aqApp->geometryForm(geoName()));
      if (geo.width() == 9999) {
        resize(size().expandedTo(mWidget->size()));
        showMaximized();
      } else if (geo.width() == 1) {
        resize(size().expandedTo(mWidget->size()));
        showMinimized();
      } else if (geo.isValid()) {
        QRect desk = QApplication::desktop()->availableGeometry(this);
        QRect inter = desk.intersect(geo);
        resize(geo.size());
        if (inter.width() * inter.height() > (geo.width() * geo.height() / 20))
#if defined(Q_OS_WIN32)
          setGeometry(geo);
#else
          move(geo.topLeft());
#endif
      } else
        resize(size().expandedTo(mWidget->size()));
    }

    if (!initFocusWidget_) {
      itf.toFirst();
      while ((fdb = static_cast<FLFieldDB *>(itf.current())) != 0) {
        ++itf;
        if (fdb->isEnabled()) {
          initFocusWidget_ = fdb;
          break;
        }
      }
      if (!initFocusWidget_)
        initFocusWidget_ = static_cast<QWidget *>(mWidget->focusWidget());
      if (initFocusWidget_)
        initFocusWidget_->setFocus();
    }

    delete l;

    QWidget *focWid = qApp->focusWidget();
    if (focWid) {
      QWidget *topWidget = focWid->topLevelWidget();
      if (topWidget && !topWidget->inherits("FLFormDB")) {
        QWidget *topWid = focWid->parentWidget();
        while (topWid && !topWid->inherits("FLFormDB"))
          topWid = topWid->parentWidget();
        topWidget = topWid;
      }
      if (topWidget != this)
        setFocus();
    } else
      setFocus();
  }
}

void FLFormDB::setCursor(FLSqlCursor *c)
{
  if (c != cursor_ && cursor_ && oldCursorCtxt)
    cursor_->setContext(oldCursorCtxt);

  if (!c)
    return;

  if (cursor_)
    disconnect(cursor_, SIGNAL(destroyed(QObject *)), this, SLOT(cursorDestroyed(QObject *)));
  cursor_ = c;
  connect(cursor_, SIGNAL(destroyed(QObject *)), this, SLOT(cursorDestroyed(QObject *)));
  if (iface && cursor_) {
    oldCursorCtxt = cursor_->context();
    cursor_->setContext(iface);
  }
}

QImage FLFormDB::snapShot()
{
  QPixmap pix = QPixmap::grabWidget(this);
  return pix.convertToImage();
}

void FLFormDB::saveSnapShot(const QString &pathFile)
{
  QFile fi(pathFile);
  if (!fi.open(IO_WriteOnly)) {
#ifdef FL_DEBUG
    qWarning("FLFormDB : " + tr("Error I/O al intentar escribir el fichero %1").arg(pathFile));
#endif
    return;
  }
  snapShot().save(&fi, "PNG");
}

void FLFormDB::showForDocument()
{
  showed = true;
  mainWidget_->show();
  resize(size().expandedTo(mainWidget_->size()));
  QWidget::show();
}

void FLFormDB::setMaximized()
{
  setWindowState(windowState() | WindowMaximized);
}

void FLFormDB::emitFormReady()
{
  emit formReady();
}

void FLFormDB::debugScript()
{
  QSScript *scr = script();
  if (!scr)
    return;
  aqApp->openQSWorkbench();
  QSWorkbench *wb = aqApp->workbench();
  if (wb)
    wb->showScript(scr);
}

QSScript *FLFormDB::script() const
{
  FLFormDBInterface *ifc = ::qt_cast<FLFormDBInterface *>(iface);
  if (ifc)
    return ifc->script();
  return 0;
}

QString FLFormDB::formName() const
{
  return QString::fromLatin1("form") + idMDI_;
}

QString FLFormDB::geoName() const
{
  return formName();
}

void FLFormDB::bindIface()
{
  QSProject *p = aqApp->project();
  if (!p)
    return;

  setName(formName());
  QObject *o = p->object(name());

  if (o != iface && iface && oldFormObj)
    static_cast<FLFormDBInterface *>(iface)->setObj(oldFormObj);
  iface = o;

  FLFormDBInterface *ifc = ::qt_cast<FLFormDBInterface *>(iface);
  if (!ifc)
    return;

  if (ifc->obj() != this) {
    if (oldFormObj) {
      disconnect(oldFormObj, SIGNAL(destroyed()),
                 this, SLOT(oldFormObjDestroyed()));
    }
    oldFormObj = ifc->obj();
    if (oldFormObj) {
      connect(oldFormObj, SIGNAL(destroyed()),
              this, SLOT(oldFormObjDestroyed()));
    }
    ifc->setObj(this);
  }
}

void FLFormDB::unbindIface()
{
  FLFormDBInterface *ifc = ::qt_cast<FLFormDBInterface *>(iface);
  if (!ifc)
    return;
  if (ifc->obj() == this)
    ifc->setObj(oldFormObj);
}

bool FLFormDB::isIfaceBind() const
{
  FLFormDBInterface *ifc = ::qt_cast<FLFormDBInterface *>(iface);
  if (!ifc)
    return true;
  return (ifc->obj() == this);
}

void FLFormDB::oldFormObjDestroyed()
{
  oldFormObj = 0;
}

void FLFormDB::cursorDestroyed(QObject *obj)
{
  if (!obj || obj != cursor_)
    return;
  cursor_ = 0;
}

void FLFormDB::focusInEvent(QFocusEvent *f)
{
  QWidget::focusInEvent(f);
  if (!isIfaceBind())
    bindIface();
}

QString FLFormDB::formClassName() const
{
  return "FormDB";
}

void FLFormDB::show()
{
  QWidget::show();
}

QVariant FLFormDB::exec(const QString &)
{
  QWidget::show();
  return QVariant();
}
