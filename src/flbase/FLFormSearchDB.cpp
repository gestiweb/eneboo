/***************************************************************************
 FLFormSearchDB.cpp
 -------------------
 begin                : Sun Nov 4 2001
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

#include "FLFormSearchDB.h"
#include "FLObjectFactory.h"
#include "FLSqlCursor.h"
#include "FLTableMetaData.h"
#include "FLAction.h"
#include "FLApplication.h"
#include "FLManagerModules.h"
#include "FLAccessControlLists.h"

FLFormSearchDB::FLFormSearchDB(const QString &actionName, QWidget *parent) :
  FLFormDB(parent, actionName, Qt::WStyle_Customize | Qt::WStyle_Maximize | Qt::WStyle_Title
           | Qt::WStyle_NormalBorder | Qt::WType_Dialog | Qt::WShowModal | Qt::WStyle_SysMenu),
  loop(false), acceptingRejecting_(false), inExec_(false)
{
  setFocusPolicy(QWidget::NoFocus);

  if (actionName.isEmpty()) {
    action_ = 0;
#ifdef FL_DEBUG
    qWarning(tr("FLFormSearchDB : Nombre de acción vacío"));
#endif
    return;
  } else
    action_ = FLSqlConnections::database()->manager()->action(actionName);
  if (!action_) {
#ifdef FL_DEBUG
    qWarning(tr("FLFormSearchDB : No existe la acción " + actionName));
#endif
    return;
  }
  cursor_ = new FLSqlCursor(action_->table(), true, "default", 0, 0, this);
  name_ = action_->name();

  initForm();
}

FLFormSearchDB::FLFormSearchDB(FLSqlCursor *cursor, const QString &actionName, QWidget *parent) :
  FLFormDB(parent, actionName, Qt::WStyle_Customize | Qt::WStyle_Maximize | Qt::WStyle_Title
           | Qt::WStyle_NormalBorder | Qt::WType_Dialog | Qt::WShowModal | Qt::WStyle_SysMenu),
  loop(false), acceptingRejecting_(false), inExec_(false)
{
  setFocusPolicy(QWidget::NoFocus);

  if (actionName.isEmpty())
    action_ = 0;
  else if (cursor)
    action_ = cursor->db()->manager()->action(actionName);
  else
    action_ = FLSqlConnections::database()->manager()->action(actionName);
  cursor_ = cursor;
  name_ = (action_ ? action_->name() : QString::null);

  initForm();
}

FLFormSearchDB::~FLFormSearchDB()
{
  if (cursor_ && !cursor_->aqWasDeleted()) {
    cursor_->restoreEditionFlag(this);
    cursor_->restoreBrowseFlag(this);
  }
}

void FLFormSearchDB::setFilter(const QString &f)
{
  if (!cursor_)
    return;
  QString previousF(cursor_->mainFilter());
  QString newF;
  if (previousF.isEmpty())
    newF = f;
  else if (previousF.contains(f))
    return;
  else
    newF = previousF + " AND " + f;
  cursor_->setMainFilter(newF);
}

QVariant FLFormSearchDB::exec(const QString &n)
{
  if (!cursor_)
    return QVariant();

  if (loop || inExec_) {
#ifdef FL_DEBUG
    qWarning(tr("FLFormSearchDB::exec(): Se ha detectado una llamada recursiva"));
#endif
    QWidget::show();
    if (initFocusWidget_)
      initFocusWidget_->setFocus();
    return QVariant();
  }

  inExec_ = true;
  acceptingRejecting_ = false;

  QWidget::show();
  if (initFocusWidget_)
    initFocusWidget_->setFocus();

  if (iface)
    aqApp->call("init", QSArgumentList(), iface);
  if (!isClosing_ && !aqApp->project()->interpreter()->hadError())
    QTimer::singleShot(0, this, SLOT(emitFormReady()));

  accepted_ = false;
  loop = true;
  if (!isClosing_ && !acceptingRejecting_)
    QApplication::eventLoop()->enterLoop();
  loop = false;

  clearWFlags(WShowModal);
  QVariant v;
  if (accepted_ && !n.isEmpty())
    v = cursor_->valueBuffer(n);
  else
    v = QVariant();

  inExec_ = false;

  return v;
}

void FLFormSearchDB::hide()
{
  if (isHidden())
    return;
  QWidget::hide();
  if (loop) {
    loop = false;
    QApplication::eventLoop()->exitLoop();
  }
}

void FLFormSearchDB::closeEvent(QCloseEvent *e)
{
  frameGeometry();
  if (focusWidget()) {
    FLFieldDB *fdb = ::qt_cast<FLFieldDB *>(focusWidget()->parentWidget());
    if (fdb && fdb->autoComFrame_ && fdb->autoComFrame_->isVisible()) {
      fdb->autoComFrame_->hide();
      return;
    }
  }

  if (cursor_ && pushButtonCancel) {
    if (!pushButtonCancel->isEnabled())
      return;
    isClosing_ = true;
    setCursor(0);
  } else
    isClosing_ = true;
  if (isShown())
    reject();
  if (isHidden()) {
    emit closed();
    QWidget::closeEvent(e);
    deleteLater();
  }
}

void FLFormSearchDB::setCursor(FLSqlCursor *c)
{
  if (c != cursor_ && cursor_ && oldCursorCtxt)
    cursor_->setContext(oldCursorCtxt);

  if (!c)
    return;

  if (cursor_) {
    disconnect(cursor_, SIGNAL(recordChoosed()), this, SLOT(accept()));
    disconnect(cursor_, SIGNAL(destroyed(QObject *)), this, SLOT(cursorDestroyed(QObject *)));
  }
  if (cursor_ && cursor_ != c) {
    cursor_->restoreEditionFlag(this);
    cursor_->restoreBrowseFlag(this);
  }
  cursor_ = c;
  cursor_->setEdition(false, this);
  cursor_->setBrowse(false, this);
  connect(cursor_, SIGNAL(recordChoosed()), this, SLOT(accept()));
  connect(cursor_, SIGNAL(destroyed(QObject *)), this, SLOT(cursorDestroyed(QObject *)));
  if (iface && cursor_) {
    oldCursorCtxt = cursor_->context();
    cursor_->setContext(iface);
  }
}

void FLFormSearchDB::setMainWidget(QWidget *w)
{
  if (!cursor_ || !w)
    return;

  if (showed) {
    if (mainWidget_ && mainWidget_ != w)
      initMainWidget(w);
  } else
    w->hide();

  if (layoutButtons)
    delete layoutButtons;

  if (layout)
    delete layout;

  w->setFont(qApp->font());

  QRect desk = QApplication::desktop()->availableGeometry(this);
  QRect geo = w->geometry();
  bool tooLarge = false;

  if (geo.width() > desk.width() || geo.height() > desk.height()) {
    QScrollView *sv = new QScrollView(this);
    sv->setResizePolicy(QScrollView::AutoOneFit);
    sv->hide();
    sv->addChild(w);
    layout = new QVBoxLayout(this, 5, 5, "vlay" + name_);
    layout->add(sv);
    sv->resize(size().expandedTo(desk.size()));
    layoutButtons = new QHBoxLayout(layout, 3, "hlay" + name_);
    connect(this, SIGNAL(formReady()), sv, SLOT(show()));
    tooLarge = true;
  } else {
    layout = new QVBoxLayout(this, 2, 3, "vlay" + name_);
    layout->add(w);
    layoutButtons = new QHBoxLayout(layout, 3, "hlay" + name_);
  }

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

  pushButtonAccept = new QPushButton(this, "pushButtonAccept");
  pushButtonAccept->setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 0, (QSizePolicy::SizeType) 0,
                                              0, 0,
                                              pushButtonAccept->sizePolicy().hasHeightForWidth()));
  pushButtonAccept->setMinimumSize(pbSize);
  pushButtonAccept->setMaximumSize(pbSize);
  QPixmap ok(QPixmap::fromMimeSource("button_ok.png"));
  pushButtonAccept->setIconSet(ok);
  pushButtonAccept->setFocusPolicy(QWidget::NoFocus);
  pushButtonAccept->setAccel(QKeySequence(Qt::Key_F10));
  pushButtonAccept->setDefault(true);
  QToolTip::add(pushButtonAccept, tr("Seleccionar registro actual y cerrar formulario (F10)"));
  QWhatsThis::add(pushButtonAccept, tr("Seleccionar registro actual y cerrar formulario (F10)"));
  layoutButtons->addWidget(pushButtonAccept);
  connect(pushButtonAccept, SIGNAL(clicked()), this, SLOT(accept()));

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
  QToolTip::add(pushButtonCancel, tr("Cerrar formulario sin seleccionar registro (Esc)"));
  QWhatsThis::add(pushButtonCancel, tr("Cerrar formulario sin seleccionar registro (Esc)"));
  layoutButtons->addItem(new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
  layoutButtons->addWidget(pushButtonCancel);
  connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));

  mainWidget_ = w;

  cursor_->setEdition(false);
  cursor_->setBrowse(false);
  connect(cursor_, SIGNAL(recordChoosed()), this, SLOT(accept()));

  if (!tooLarge) {
    int mWidth = mainWidget_->width();
    int mHeight = mainWidget_->height();
    QWidget *actWin = qApp->activeWindow();
    QRect screen = (actWin ? actWin->geometry() : qApp->mainWidget()->geometry());
    QPoint p = screen.center() - QPoint(mWidth / 2, mHeight / 2);

    if (p.x() + mWidth > desk.width())
      p.setX(desk.width() - mWidth);
    if (p.y() + mHeight > desk.height())
      p.setY(desk.height() - mHeight);
    if (p.x() < 0)
      p.setX(0);
    if (p.y() < 0)
      p.setY(0);
    move(p);
  }
}

void FLFormSearchDB::setMainWidget()
{
  FLFormDB::setMainWidget();
}

bool FLFormSearchDB::initScript()
{
  return false;
}

void FLFormSearchDB::accept()
{
  if (acceptingRejecting_)
    return;
  frameGeometry();
  if (cursor_) {
    disconnect(cursor_, SIGNAL(recordChoosed()), this, SLOT(accept()));
    accepted_ = true;
  }
  acceptingRejecting_ = true;
  hide();
}

void FLFormSearchDB::reject()
{
  if (acceptingRejecting_)
    return;
  frameGeometry();
  if (cursor_)
    disconnect(cursor_, SIGNAL(recordChoosed()), this, SLOT(accept()));
  acceptingRejecting_ = true;
  hide();
}

// Silix
void FLFormSearchDB::setCaptionWidget(const QString &text) {
  if (text.isEmpty())
    return;

  setCaption(text);
}

QString FLFormSearchDB::geoName() const
{
  return QString::fromLatin1("formSearch") + idMDI_;
}

QString FLFormSearchDB::formClassName() const
{
  return "FormSearchDB";
}

void FLFormSearchDB::show()
{
  exec();
}
