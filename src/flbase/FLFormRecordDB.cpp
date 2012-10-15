/***************************************************************************
 FLFormRecordDB.cpp
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

#include "FLFormRecordDB.h"
#include "FLObjectFactory.h"
#include "FLSqlCursor.h"
#include "FLTableMetaData.h"
#include "FLAction.h"
#include "FLManager.h"
#include "FLApplication.h"
#include "FLManagerModules.h"
#include "FLAccessControlLists.h"

FLFormRecordDB::FLFormRecordDB(FLSqlCursor *cursor, const QString &actionName, QWidget *parent,
                               bool showAcceptContinue) :
  FLFormDB(parent, actionName, Qt::WStyle_Customize | Qt::WStyle_Maximize | Qt::WStyle_Title
           | Qt::WStyle_NormalBorder | Qt::WType_Dialog | Qt::WShowModal | Qt::WStyle_SysMenu),
  pushButtonAccept(0), pushButtonAcceptContinue(0), pushButtonFirst(0), pushButtonPrevious(0),
  pushButtonNext(0), pushButtonLast(0), showAcceptContinue_(showAcceptContinue), iface(0),
  accepting(false), initTransLevel(0), initialModeAccess(cursor ? cursor->modeAccess()
                                                         : FLSqlCursor::BROWSE)
{
  setFocusPolicy(QWidget::NoFocus);

  if (actionName.isEmpty())
    action_ = 0;
  else if (cursor)
    action_ = cursor->db()->manager()->action(actionName);
  else
    action_ = FLSqlConnections::database()->manager()->action(actionName);
  cursor_ = cursor;
  name_ = QString::null;

  initForm();
}

FLFormRecordDB::~FLFormRecordDB()
{
  if (iface && iface->obj() == this) {
    iface->finish();
    iface->setObj(0);
  }
}

void FLFormRecordDB::initForm()
{
  if (cursor_ && cursor_->metadata()) {
    if (action_) {
      cursor_->setAction(action_);
      idMDI_ = action_->name();
    }

    if (iface)
      iface->finish();
    setName("formRecord" + idMDI_);
    QSProject *p = aqApp->project();
    iface = static_cast<FLFormRecordDBInterface *>(p->object(name()));
    if (iface) {
      iface->setObj(this);
      if (!oldCursorCtxt)
        oldCursorCtxt = cursor_->context();
      cursor_->setContext(iface);
    }

    QString caption = cursor_->metadata()->alias();
    switch (cursor_->modeAccess()) {
      case FLSqlCursor::INSERT:
        cursor_->transaction();
        initTransLevel = FLSqlCursor::transactionLevel();
        setCaption(tr("Insertar ") + caption);
        break;

      case FLSqlCursor::EDIT:
        cursor_->transaction();
        initTransLevel = FLSqlCursor::transactionLevel();
        setCaption(tr("Editar ") + caption);
        break;

      case FLSqlCursor::DEL:
        break;

      case FLSqlCursor::BROWSE:
        cursor_->transaction();
        initTransLevel = FLSqlCursor::transactionLevel();
        setCaption(tr("Visualizar ") + caption);
        break;
    }
  } else
    setCaption(tr("No hay metadatos"));
}

void FLFormRecordDB::setMainWidget(QWidget *w)
{
  if (!cursor_ || !w)
    return;

  if (!cursor_->metadata())
    return;

  if (showed) {
    if (mainWidget_ && mainWidget_ != w)
      initMainWidget(w);
  } else
    w->hide();

  if (pushButtonAcceptContinue)
    pushButtonAcceptContinue->hide();

  if (pushButtonAccept)
    pushButtonAccept->hide();

  if (pushButtonCancel)
    pushButtonCancel->hide();

  if (pushButtonFirst)
    pushButtonFirst->hide();

  if (pushButtonPrevious)
    pushButtonPrevious->hide();

  if (pushButtonNext)
    pushButtonNext->hide();

  if (pushButtonLast)
    pushButtonLast->hide();

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

  layoutButtons->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

  if (cursor_->modeAccess() == FLSqlCursor::EDIT || cursor_->modeAccess() == FLSqlCursor::BROWSE) {
    if (!pushButtonFirst) {
      pushButtonFirst = new QPushButton(this, "pushButtonFirst");
      connect(pushButtonFirst, SIGNAL(clicked()), this, SLOT(firstRecord()));
    }
    pushButtonFirst->setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 0,
                                               (QSizePolicy::SizeType) 0, 0, 0,
                                               pushButtonFirst->sizePolicy().hasHeightForWidth()));
    pushButtonFirst->setMinimumSize(pbSize);
    pushButtonFirst->setMaximumSize(pbSize);
    QPixmap rld(QPixmap::fromMimeSource("first.png"));
    pushButtonFirst->setIconSet(rld);
    pushButtonFirst->setAccel(QKeySequence(Qt::Key_F5));
    QToolTip::add(pushButtonFirst, tr("Aceptar los cambios e ir al primer registro (F5)"));
    QWhatsThis::add(pushButtonFirst, tr("Aceptar los cambios e ir al primer registro (F5)"));
    pushButtonFirst->setFocusPolicy(QWidget::NoFocus);
    layoutButtons->addWidget(pushButtonFirst);
    pushButtonFirst->show();

    if (!pushButtonPrevious) {
      pushButtonPrevious = new QPushButton(this, "pushButtonPrevious");
      connect(pushButtonPrevious, SIGNAL(clicked()), this, SLOT(previousRecord()));
    }
    pushButtonPrevious->setSizePolicy(
      QSizePolicy(
        (QSizePolicy::SizeType) 0,
        (QSizePolicy::SizeType) 0,
        0,
        0,
        pushButtonPrevious->sizePolicy().hasHeightForWidth()));
    pushButtonPrevious->setMinimumSize(pbSize);
    pushButtonPrevious->setMaximumSize(pbSize);
    QPixmap rld2(QPixmap::fromMimeSource("previous.png"));
    pushButtonPrevious->setIconSet(rld2);
    pushButtonPrevious->setAccel(QKeySequence(Qt::Key_F6));
    QToolTip::add(pushButtonPrevious, tr("Aceptar los cambios e ir al registro anterior (F6)"));
    QWhatsThis::add(pushButtonPrevious, tr("Aceptar los cambios e ir al registro anterior (F6)"));
    pushButtonPrevious->setFocusPolicy(QWidget::NoFocus);
    layoutButtons->addWidget(pushButtonPrevious);
    pushButtonPrevious->show();

    if (!pushButtonNext) {
      pushButtonNext = new QPushButton(this, "pushButtonNext");
      connect(pushButtonNext, SIGNAL(clicked()), this, SLOT(nextRecord()));
    }
    pushButtonNext->setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 0, (QSizePolicy::SizeType) 0,
                                              0, 0,
                                              pushButtonNext->sizePolicy().hasHeightForWidth()));
    pushButtonNext->setMinimumSize(pbSize);
    pushButtonNext->setMaximumSize(pbSize);
    QPixmap rld3(QPixmap::fromMimeSource("next.png"));
    pushButtonNext->setIconSet(rld3);
    pushButtonNext->setAccel(QKeySequence(Qt::Key_F7));
    QToolTip::add(pushButtonNext, tr("Aceptar los cambios e ir al registro siguiente (F7)"));
    QWhatsThis::add(pushButtonNext, tr("Aceptar los cambios e ir al registro siguiente (F7)"));
    pushButtonNext->setFocusPolicy(QWidget::NoFocus);
    layoutButtons->addWidget(pushButtonNext);
    pushButtonNext->show();

    if (!pushButtonLast) {
      pushButtonLast = new QPushButton(this, "pushButtonLast");
      connect(pushButtonLast, SIGNAL(clicked()), this, SLOT(lastRecord()));
    }
    pushButtonLast->setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 0, (QSizePolicy::SizeType) 0,
                                              0, 0,
                                              pushButtonLast->sizePolicy().hasHeightForWidth()));
    pushButtonLast->setMinimumSize(pbSize);
    pushButtonLast->setMaximumSize(pbSize);
    QPixmap rld4(QPixmap::fromMimeSource("last.png"));
    pushButtonLast->setIconSet(rld4);
    pushButtonLast->setAccel(QKeySequence(Qt::Key_F8));
    QToolTip::add(pushButtonLast, tr("Aceptar los cambios e ir al último registro (F8)"));
    QWhatsThis::add(pushButtonLast, tr("Aceptar los cambios e ir al último registro (F8)"));
    pushButtonLast->setFocusPolicy(QWidget::NoFocus);
    layoutButtons->addWidget(pushButtonLast);
    pushButtonLast->show();
  }

  if (cursor_->modeAccess() != FLSqlCursor::BROWSE) {
    if (showAcceptContinue_) {
      if (!pushButtonAcceptContinue) {
        pushButtonAcceptContinue = new QPushButton(this, "pushButtonAcceptContinue");
        connect(pushButtonAcceptContinue, SIGNAL(clicked()), this, SLOT(acceptContinue()));
      }
      pushButtonAcceptContinue->setSizePolicy(
        QSizePolicy(
          (QSizePolicy::SizeType) 0,
          (QSizePolicy::SizeType) 0,
          0,
          0,
          pushButtonAcceptContinue->sizePolicy().hasHeightForWidth()));
      pushButtonAcceptContinue->setMinimumSize(pbSize);
      pushButtonAcceptContinue->setMaximumSize(pbSize);
      QPixmap rld(QPixmap::fromMimeSource("reload.png"));
      pushButtonAcceptContinue->setIconSet(rld);
      pushButtonAcceptContinue->setFocusPolicy(QWidget::NoFocus);
      pushButtonAcceptContinue->setAccel(QKeySequence(Qt::Key_F9));
      QToolTip::add(pushButtonAcceptContinue,
                    tr("Aceptar los cambios y continuar con la edición de un nuevo registro (F9)"));
      QWhatsThis::add(
        pushButtonAcceptContinue,
        tr("Aceptar los cambios y continuar con la edición de un nuevo registro (F9)"));
      layoutButtons->addWidget(pushButtonAcceptContinue);
      pushButtonAcceptContinue->show();
    }

    if (!pushButtonAccept) {
      pushButtonAccept = new QPushButton(this, "pushButtonAccept");
      connect(pushButtonAccept, SIGNAL(clicked()), this, SLOT(accept()));
    }
    pushButtonAccept->setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 0,
                                                (QSizePolicy::SizeType) 0, 0, 0,
                                                pushButtonAccept->sizePolicy().hasHeightForWidth()));
    pushButtonAccept->setMinimumSize(pbSize);
    pushButtonAccept->setMaximumSize(pbSize);
    QPixmap ok(QPixmap::fromMimeSource("button_ok.png"));
    pushButtonAccept->setIconSet(ok);
    pushButtonAccept->setFocusPolicy(QWidget::NoFocus);
    pushButtonAccept->setAccel(QKeySequence(Qt::Key_F10));
    QToolTip::add(pushButtonAccept, tr("Aceptar los cambios y cerrar formulario (F10)"));
    QWhatsThis::add(pushButtonAccept, tr("Aceptar los cambios y cerrar formulario (F10)"));
    layoutButtons->addWidget(pushButtonAccept);
    pushButtonAccept->show();
  }

  if (!pushButtonCancel) {
    pushButtonCancel = new QPushButton(this, "pushButtonCancel");
    connect(cursor_, SIGNAL(autoCommit()), this, SLOT(disablePushButtonCancel()));
    connect(pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
  }
  pushButtonCancel->setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 0, (QSizePolicy::SizeType) 0,
                                              0, 0,
                                              pushButtonCancel->sizePolicy().hasHeightForWidth()));
  pushButtonCancel->setMinimumSize(pbSize);
  pushButtonCancel->setMaximumSize(pbSize);
  QPixmap cancel(QPixmap::fromMimeSource("button_cancel.png"));
  pushButtonCancel->setIconSet(cancel);
  if (cursor_->modeAccess() != FLSqlCursor::BROWSE) {
    pushButtonCancel->setFocusPolicy(QWidget::NoFocus);
    pushButtonCancel->setAccel(4096);
    QToolTip::add(pushButtonCancel, tr("Cancelar los cambios y cerrar formulario (Esc)"));
    QWhatsThis::add(pushButtonCancel, tr("Cancelar los cambios y cerrar formulario (Esc)"));
  } else {
    QPixmap ok(QPixmap::fromMimeSource("button_cancel.png"));
    pushButtonCancel->setIconSet(ok);
    pushButtonCancel->setFocusPolicy(QWidget::StrongFocus);
    pushButtonCancel->setFocus();
    pushButtonCancel->setAccel(4096);
    QToolTip::add(pushButtonCancel, tr("Aceptar y cerrar formulario (Esc)"));
    QWhatsThis::add(pushButtonCancel, tr("Aceptar y cerrar formulario (Esc)"));
  }
  pushButtonCancel->setDefault(true);
  layoutButtons->addWidget(pushButtonCancel);
  pushButtonCancel->show();

#if defined(Q_OS_MACX)
  layoutButtons->addItem(new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed));
#endif

  mainWidget_ = w;
  mainWidget_->setFocusPolicy(QWidget::NoFocus);

  if (!tooLarge) {
    int mWidth = mainWidget_->width();
    int mHeight = mainWidget_->height();
    QPoint p = desk.center() - QPoint(mWidth / 2, mHeight / 2);

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

void FLFormRecordDB::initScript()
{
  if (iface && cursor_)
    aqApp->call("init", QSArgumentList(), iface);
}

void FLFormRecordDB::setMainWidget()
{
  if (!action_ || !cursor_)
    return;

  if (!cursor_->metadata())
    return;

  setMainWidget(cursor_->db()->managerModules()->createFormRecord(action_, this, this));
}

void FLFormRecordDB::setCursor(FLSqlCursor *c)
{
  if (!c)
    return;
  cursor_ = c;
  if (iface)
    iface->setObjCursor(c);
}

void FLFormRecordDB::afterCommitBuffer()
{
  if (iface)
    aqApp->call("afterCommitBuffer", QSArgumentList(), iface);
}

void FLFormRecordDB::afterCommitTransaction()
{
  if (iface)
    aqApp->call("afterCommitTransaction", QSArgumentList(), iface);
}

void FLFormRecordDB::acceptedForm()
{
  if (iface)
    aqApp->call("acceptedForm", QSArgumentList(), iface);
}

void FLFormRecordDB::canceledForm()
{
  if (iface)
    aqApp->call("canceledForm", QSArgumentList(), iface);
}

bool FLFormRecordDB::validateForm()
{
  FLTableMetaData *mtd = cursor_->metadata();
  if (!cursor_ || !mtd)
    return true;

  if (cursor_->modeAccess() == FLSqlCursor::EDIT && mtd->concurWarn()) {
    QStringList colFields(cursor_->concurrencyFields());

    if (!colFields.isEmpty()) {
      QString pKN(mtd->primaryKey());
      QString pKWhere(
        cursor_->db()->manager()->formatAssignValue(
          mtd->field(pKN), cursor_->valueBuffer(pKN)
        )
      );

      FLSqlQuery q(0, cursor_->db()->connectionName());
      q.setTablesList(mtd->name());
      q.setSelect(colFields.join(","));
      q.setFrom(mtd->name());
      q.setWhere(pKWhere);
      q.setForwardOnly(true);

      if (q.exec() && q.next()) {
        for (QStringList::const_iterator it = colFields.begin(); it != colFields.end(); ++it) {
          QString msg(
            tr("El campo '%1' con valor '%2' ha sido modificado\npor otro usuario con el valor '%3'")
            .arg(mtd->fieldNameToAlias(*it))
            .arg(cursor_->valueBuffer(*it).toString().left(50))
            .arg(q.value(*it).toString().left(50))
          );

          int res =
            QMessageBox::warning(
              qApp->focusWidget(),
              tr("Aviso de concurrencia"),
              msg + "\n\n" +
              tr("¿ Desea realmente modificar este campo ?") + "\n\n" +
              tr("Sí : Ignora el cambio del otro usuario y utiliza el valor que acaba de introducir\n") +
              tr("No : Respeta el cambio del otro usuario e ignora el valor que ha introducido\n") +
              tr("Cancelar : Cancela el guardado del registro y vuelve a la edición del registro\n\n"),
              QMessageBox::Yes | QMessageBox::Default, QMessageBox::No, QMessageBox::Cancel | QMessageBox::Escape
            );
          if (res == QMessageBox::Cancel)
            return false;
          if (res == QMessageBox::No)
            cursor_->setValueBuffer(*it, q.value(*it));
        }
      }
    }
  }

  if (iface && (cursor_->modeAccess() == FLSqlCursor::INSERT ||
                cursor_->modeAccess() == FLSqlCursor::EDIT)) {
    QVariant v(aqApp->call("validateForm", QSArgumentList(), iface).variant());
    if (v.isValid() && !v.toBool())
      return false;
  }

  return true;
}

void FLFormRecordDB::accept()
{
  if (accepting) {
    return;
  } else
    accepting = true;

  if (!cursor_) {
    close();
    return;
  }

  if (!validateForm()) {
    accepting = false;
    return;
  }

  if (cursor_->checkIntegrity()) {
    acceptedForm();
    cursor_->setActivatedCheckIntegrity(false);
    if (cursor_->commitBuffer()) {
      cursor_->setActivatedCheckIntegrity(true);
    } else {
      accepting = false;
      return;
    }
  } else {
    accepting = false;
    return;
  }

  afterCommitBuffer();

  accepted = true;
  close();
}

void FLFormRecordDB::acceptContinue()
{
  if (accepting)
    return;
  else
    accepting = true;

  if (!cursor_) {
    close();
    return;
  }

  if (!validateForm()) {
    accepting = false;
    return;
  }

  if (cursor_->checkIntegrity()) {
    acceptedForm();
    cursor_->setActivatedCheckIntegrity(false);
    if (cursor_->commitBuffer()) {
      cursor_->setActivatedCheckIntegrity(true);
      cursor_->commit();
      cursor_->setModeAccess(FLSqlCursor::INSERT);
      accepted = false;
      QString caption = cursor_->metadata()->alias();
      cursor_->transaction();
      setCaption(tr("Insertar ") + caption);
      if (initFocusWidget_)
        initFocusWidget_->setFocus();
      cursor_->refreshBuffer();
      initScript();
    }
  }
  accepting = false;
}

void FLFormRecordDB::reject()
{
  accepted = false;
  canceledForm();
  close();
}

void FLFormRecordDB::closeEvent(QCloseEvent *e)
{
  frameGeometry();
  if (focusWidget()) {
    FLFieldDB *fdb = ::qt_cast<FLFieldDB *>(focusWidget()->parentWidget());
    if (fdb && fdb->autoComFrame_ && fdb->autoComFrame_->isVisible()) {
      fdb->autoComFrame_->hide();
      return;
    }
  }

  if (cursor_) {
    int levels = FLSqlCursor::transactionLevel() - initTransLevel;
    if (levels > 0) {
      cursor_->rollbackOpened(
        levels, tr("Se han detectado transacciones no finalizadas en la última operación.\n"
                   "Se van a cancelar las transacciones pendientes.\n")
      );
    }
    if (accepted) {
      if (!cursor_->commit())
        return;
      afterCommitTransaction();
    } else {
      if (!cursor_->rollback())
        return;
      else
        cursor_->QSqlCursor::select();
    }
    emit closed();
    if (oldCursorCtxt)
      cursor_->setContext(oldCursorCtxt);
  } else
    emit closed();
  QWidget::closeEvent(e);
  deleteLater();
}

void FLFormRecordDB::firstRecord()
{
  if (cursor_ && cursor_->at() != 0) {
    if (!validateForm())
      return;
    if (cursor_->checkIntegrity()) {
      acceptedForm();
      cursor_->setActivatedCheckIntegrity(false);
      if (cursor_->commitBuffer()) {
        cursor_->setActivatedCheckIntegrity(true);
        cursor_->commit();
        cursor_->setModeAccess(initialModeAccess);
        accepted = false;
        QString caption = cursor_->metadata()->alias() + " [ "
                          + aqApp->lastTextCaption() + " ]";
        cursor_->transaction();
        cursor_->first();
        initScript();
      }
    }
  }
}

void FLFormRecordDB::nextRecord()
{
  if (cursor_ && cursor_->isValid()) {
    if (cursor_->at() == (cursor_->size() - 1)) {
      firstRecord();
      return;
    }
    if (!validateForm())
      return;
    if (cursor_->checkIntegrity()) {
      acceptedForm();
      cursor_->setActivatedCheckIntegrity(false);
      if (cursor_->commitBuffer()) {
        cursor_->setActivatedCheckIntegrity(true);
        cursor_->commit();
        cursor_->setModeAccess(initialModeAccess);
        accepted = false;
        QString caption = cursor_->metadata()->alias() + " [ "
                          + aqApp->lastTextCaption() + " ]";
        cursor_->transaction();
        cursor_->next();
        initScript();
      }
    }
  }
}

void FLFormRecordDB::previousRecord()
{
  if (cursor_ && cursor_->isValid()) {
    if (cursor_->at() == 0) {
      lastRecord();
      return;
    }
    if (!validateForm())
      return;
    if (cursor_->checkIntegrity()) {
      acceptedForm();
      cursor_->setActivatedCheckIntegrity(false);
      if (cursor_->commitBuffer()) {
        cursor_->setActivatedCheckIntegrity(true);
        cursor_->commit();
        cursor_->setModeAccess(initialModeAccess);
        accepted = false;
        QString caption = cursor_->metadata()->alias() + " [ "
                          + aqApp->lastTextCaption() + " ]";
        cursor_->transaction();
        cursor_->prev();
        initScript();
      }
    }
  }
}

void FLFormRecordDB::lastRecord()
{
  if (cursor_ && cursor_->at() != (cursor_->size() - 1)) {
    if (!validateForm())
      return;
    if (cursor_->checkIntegrity()) {
      acceptedForm();
      cursor_->setActivatedCheckIntegrity(false);
      if (cursor_->commitBuffer()) {
        cursor_->setActivatedCheckIntegrity(true);
        cursor_->commit();
        cursor_->setModeAccess(initialModeAccess);
        accepted = false;
        QString caption = cursor_->metadata()->alias() + " [ "
                          + aqApp->lastTextCaption() + " ]";
        cursor_->transaction();
        cursor_->last();
        initScript();
      }
    }
  }
}

void FLFormRecordDB::disablePushButtonCancel()
{
  if (pushButtonCancel)
    pushButtonCancel->setDisabled(true);
}
