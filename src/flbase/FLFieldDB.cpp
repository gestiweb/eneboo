/***************************************************************************
 FLFieldDB.cpp
 -------------------
 begin                : Wed Jul 4 2001
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

#include <math.h>

#include <qbuffer.h>
#include <qcolordialog.h>
#include <qfontdatabase.h>
#include <qcombobox.h>

#include "FLFieldDB.h"
#include "FLSqlCursor.h"
#include "FLSqlQuery.h"
#include "FLFormSearchDB.h"
#include "FLFormRecordDB.h"
#include "FLTableMetaData.h"
#include "FLFieldMetaData.h"
#include "FLRelationMetaData.h"
#include "FLManager.h"
#include "FLAction.h"
#include "FLTableDB.h"
#include "FLDataTable.h"
#include "FLUtil.h"
#include "FLApplication.h"
#include "FLSqlDatabase.h"
#include "FLSqlConnections.h"
#include "vdatepopup.h"
#include "FLSettings.h"

FLLineEdit::FLLineEdit(QWidget *parent, const char *name) :
  QLineEdit(parent, name),
  type(QVariant::Invalid), partDecimal(0), autoSelect(true)
{
}

QString FLLineEdit::text() const
{
  QString text(QLineEdit::text());
  if (text.isEmpty())
    return text;

  bool ok = false;
  bool minus = false;

  switch (type) {
    case QVariant::Double: {
      if (text.left(1) == "-") {
        minus = true;
        text = text.mid(1);
      }

      double val = aqApp->localeSystem().toDouble(text, &ok);
      if (ok)
        text.setNum(val, 'f', partDecimal);

      if (minus)
        text.prepend('-');
    }
    break;
    case QVariant::UInt: {
      uint val = aqApp->localeSystem().toUInt(text, &ok);
      if (ok)
        text.setNum(val);
    }
    break;
    case QVariant::Int: {
      int val = aqApp->localeSystem().toInt(text, &ok);
      if (ok)
        text.setNum(val);
    }
    break;
  }

  return text;
}

void FLLineEdit::setText(const QString &text, bool checkfocus)
{
  if (checkfocus) {
    if (text.isEmpty() || hasFocus()) {
      QLineEdit::setText(text);
      return;
    }
  }
  bool ok = false;
  QString s(text);
  bool minus = false;

  switch (type) {
    case QVariant::Double: {
      if (s.left(1) == "-") {
        minus = true;
        s = s.mid(1);
      }

      double val = s.toDouble(&ok);
      if (ok)
        s = aqApp->localeSystem().toString(val, 'f', partDecimal);

      if (minus)
        s.prepend('-');
    }
    break;
    case QVariant::UInt: {
      uint val = s.toUInt(&ok);
      if (ok)
        s = aqApp->localeSystem().toString(val);
    }
    break;
    case QVariant::Int: {
      int val = s.toInt(&ok);
      if (ok)
        s = aqApp->localeSystem().toString(val);
    }
    break;
  }

  QLineEdit::setText(s);

}

void FLLineEdit::setText(const QString &text)
{
    this->setText(text, true);
}

void FLLineEdit::focusOutEvent(QFocusEvent *f)
{
#ifndef Q_OS_WIN32
  const QValidator *v = validator();
  if (v) {
    QString s(text());
    v->fixup(s);
    setText(s);
  } else if (type == QVariant::Double || type == QVariant::Int || type == QVariant::UInt)
    setText(text());
#else
  if (type == QVariant::Double || type == QVariant::Int || type == QVariant::UInt)
    setText(text());
#endif
  QLineEdit::focusOutEvent(f);
}

void FLLineEdit::focusInEvent(QFocusEvent *f)
{
  if (type == QVariant::Double || type == QVariant::Int || type == QVariant::UInt) {
    blockSignals(true);
    QString s(text());
    const QValidator *v = validator();
    if (v) {
      int pos = 0;
      v->validate(s, pos);
    }
    QLineEdit::setText(s);
    blockSignals(false);
  }
  if (autoSelect && selectedText().isEmpty() && !isReadOnly())
    selectAll();
  QLineEdit::focusInEvent(f);
}

FLPixmapView::FLPixmapView(QWidget *parent) :
  QScrollView(parent), autoScaled_(false), path_(QString::null)
{
  viewport()->setBackgroundMode(PaletteBase);
}

void FLPixmapView::setPixmap(const QPixmap &pix)
{
  QApplication::setOverrideCursor(waitCursor);
  pixmap_ = pix;
  if (!autoScaled_)
    resizeContents(pixmap_.size().width(), pixmap_.size().height());
  viewport()->repaint(false);
  QApplication::restoreOverrideCursor();
}

void FLPixmapView::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
  p->fillRect(cx, cy, cw, ch, colorGroup().brush(QColorGroup::Background));

  if (autoScaled_) {
    QImage img;
    int newWidth = width() - 2;
    int newHeight = height() - 2;

    if (!pixmapView_.isNull() && pixmapView_.width() == newWidth && pixmapView_.height()
        == newHeight)
      return;

    img = pixmap_;
    if (img.width() > newWidth || img.height() > newHeight)
      pixmapView_.convertFromImage(img.scale(newWidth, newHeight, QImage::ScaleMin));
    else
      pixmapView_.convertFromImage(img);

    if (!pixmapView_.isNull())
      p->drawPixmap((width() / 2) - (pixmapView_.width() / 2), (height() / 2)
                    - (pixmapView_.height() / 2), pixmapView_);
  } else {
    if (!pixmap_.isNull())
      p->drawPixmap((width() / 2) - (pixmap_.width() / 2), (height() / 2) - (pixmap_.height() / 2),
                    pixmap_);
  }
}

void FLPixmapView::previewUrl(const QUrl &u)
{
  if (u.isLocalFile()) {
    QString path = u.path();

    if (path != path_) {
      path_ = path;
      QImage img(path_);

      if (img.isNull())
        return;

      QPixmap pix;

      QApplication::setOverrideCursor(waitCursor);
      pix.convertFromImage(img);
      QApplication::restoreOverrideCursor();

      if (!pix.isNull())
        setPixmap(pix);
    }
  }
}

void FLPixmapView::clear()
{
  setPixmap(QPixmap());
}

QPixmap FLPixmapView::pixmap()
{
  return pixmap_;
}

void FLPixmapView::setAutoScaled(const bool autoScaled)
{
  autoScaled_ = autoScaled;
}

void FLDateEdit::fix()
{
  if (!date().isNull())
    QDateEdit::fix();
}

FLFieldDB::FLFieldDB(QWidget *parent, const char *name) :
  FLWidgetFieldDB(parent, name), editor_(0), fieldName_(QString::null), tableName_(QString::null),
  actionName_(QString::null), foreignField_(QString::null), fieldRelation_(QString::null),
  filter_(QString::null), cursor_(0), cursorAux(0), cursorInit(false), cursorAuxInit(false),
  topWidget_(0), showed(false), showAlias_(true), datePopup_(0), dateFrame_(0),
  datePickerOn_(false), autoComPopup_(0), autoComFrame_(0), accel_(0), keepDisabled_(false),
  editorImg_(0), pbAux_(0), pbAux2_(0), pbAux3_(0), pbAux4_(0), fieldAlias_(QString::null),
  showEditor_(true), fieldMapValue_(0), autoCompMode_(OnDemandF4), timerAutoComp_(0),
  textFormat_(Qt::AutoText), initNotNullColor_(false)
{

  pushButtonDB->setFlat(true);
  setFocusProxy(pushButtonDB);

  // Silix //Aulla : Se carga valor desde clave local
  maxPixImages_ = FLSettings::readEntry("ebcomportamiento/maxPixImages","").toInt();
  if(!maxPixImages_)
  	maxPixImages_ = 600;
  topWidget_ = topLevelWidget();

  if (topWidget_ && !topWidget_->inherits("FLFormDB")) {
    QWidget *topWid = parentWidget();
    while (topWid && !topWid->inherits("FLFormDB"))
      topWid = topWid->parentWidget();
    topWidget_ = topWid;
  }

  if (!topWidget_) {
#ifdef FL_DEBUG
    qWarning(tr("FLFieldDB : El widget de nivel superior deber ser de la clase FLFormDB o heredar de ella - %1").arg(name));
#endif
  } else
    cursor_ = ::qt_cast<FLFormDB *>(topWidget_)->cursor();

  if (!name)
    setName("FLFieldDB");

  cursorBackup_ = 0;
  partDecimal_ = -1;
}

bool FLFieldDB::eventFilter(QObject *obj, QEvent *ev)
{
  if (ev->type() == QEvent::KeyPress) {
    QKeyEvent *k = static_cast<QKeyEvent *>(ev);

    bool timerActive = false;
    if (autoComFrame_ && autoComFrame_->isVisible()) {
      if (k->key() == Key_Down && autoComPopup_) {
        autoComPopup_->setQuickFocus();
#ifdef Q_OS_WIN32
        autoComPopup_->grabKeyboard();
#endif
        return true;
      }

#ifdef Q_OS_WIN32
      if (editor_)
        editor_->releaseKeyboard();
      if (autoComPopup_)
        autoComPopup_->releaseKeyboard();
#endif
      autoComFrame_->hide();
      if (::qt_cast<QLineEdit *>(editor_) && k->key() == Key_BackSpace) {
        ::qt_cast<QLineEdit *>(editor_)->backspace();
      }
      if (!timerAutoComp_) {
        timerAutoComp_ = new QTimer(this, QObject::name());
        connect(timerAutoComp_, SIGNAL(timeout()), SLOT(toggleAutoCompletion()));
      } else
        timerAutoComp_->stop();
      if (k->key() != Key_Enter && k->key() != Key_Return) {
        timerActive = true;
        timerAutoComp_->start(500, true);
      } else {
        QTimer::singleShot(0, this, SLOT(autoCompletionUpdateValue()));
        return true;
      }
    }

    if (!timerActive && autoCompMode_ == AlwaysAuto &&
        (!autoComFrame_ || !autoComFrame_->isVisible())) {
      if (k->key() == Key_BackSpace || k->key() == Key_Delete ||
          (k->key() >= Key_Space && k->key() <= Key_ydiaeresis)) {
        if (!timerAutoComp_) {
          timerAutoComp_ = new QTimer(this, QObject::name());
          connect(timerAutoComp_, SIGNAL(timeout()), SLOT(toggleAutoCompletion()));
        } else
          timerAutoComp_->stop();
        if (k->key() != Key_Enter && k->key() != Key_Return) {
          timerAutoComp_->start(500, true);
        } else {
          QTimer::singleShot(0, this, SLOT(autoCompletionUpdateValue()));
          return true;
        }
      }
    }

    if (::qt_cast<FLLineEdit *>(obj)) {
      if (k->key() == Key_F4) {
        emit keyF4Pressed();
        return true;
      }
    } else if (::qt_cast<QTextEdit *>(obj)) {
      if (k->key() == Key_F4) {
        emit keyF4Pressed();
        return true;
      }
      return false;
    }

    if (k->key() == Key_Enter || k->key() == Key_Return) {
      focusNextPrevChild(true);
      emit keyReturnPressed();
      return true;
    }
    if (k->key() == Key_Up) {
      focusNextPrevChild(false);
      return true;
    }
    if (k->key() == Key_Down) {
      focusNextPrevChild(true);
      return true;
    }
    if (k->key() == Key_F2) {
      emit keyF2Pressed();
      return true;
    }

    return false;
  } else if (ev->type() == QEvent::MouseButtonRelease && obj == textLabelDB
             && ((QMouseEvent *) ev)->button() == Qt::LeftButton) {
    emit labelClicked();
  } else {
    return false;
  }
}

void FLFieldDB::updateValue(const QDate &d)
{
  if (!cursor_)
    return;
  bool isNull = (!d.isValid() || d.isNull());
  if (!cursor_->bufferIsNull(fieldName_)) {
    if (d == cursor_->valueBuffer(fieldName_).toDate())
      return;
  } else if (isNull)
    return;
  if (isNull)
    cursor_->setValueBuffer(fieldName_, QDate());
  else
    cursor_->setValueBuffer(fieldName_, d);
}

void FLFieldDB::updateValue(const QTime &t)
{
  if (!cursor_)
    return;
  bool isNull = (!t.isValid() || t.isNull());
  if (!cursor_->bufferIsNull(fieldName_)) {
    if (t == cursor_->valueBuffer(fieldName_).toTime())
      return;
  } else if (isNull)
    return;
  if (isNull)
    cursor_->setValueBuffer(fieldName_, QTime());
  else
    cursor_->setValueBuffer(fieldName_, t);
}

void FLFieldDB::updateValue(bool b)
{
  if (!cursor_)
    return;
  if (!cursor_->bufferIsNull(fieldName_))
    if (b == cursor_->valueBuffer(fieldName_).toBool())
      return;
  cursor_->setValueBuffer(fieldName_, QVariant(b, 0));
}

void FLFieldDB::updateValue(const QString &t)
{
  if (!cursor_)
    return;

  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return;
  FLFieldMetaData *field = tMD->field(fieldName_);
  if (!field)
    return;

  bool ol = field->hasOptionsList();
  QString tAux(t);

  if (ol && editor_)
    tAux = field->optionsList()[::qt_cast<QComboBox *>(editor_)->currentItem()];

  if (!cursor_->bufferIsNull(fieldName_)) {
    if (tAux == cursor_->valueBuffer(fieldName_).toString()) {
      return;
    }
  } else if (tAux.isEmpty())
    return;

  QString s(tAux);
  if (field->type() == QVariant::String && !ol) {
    if (s.startsWith(" ")) {
      disconnect(cursor_, SIGNAL(bufferChanged(const QString &)), this,
                 SLOT(refreshQuick(const QString &)));
      cursor_->setValueBuffer(fieldName_, s.remove(0, 1));
      connect(cursor_, SIGNAL(bufferChanged(const QString &)), this,
              SLOT(refreshQuick(const QString &)));
      return;
    }
    s.remove("\\");
    s.replace("'", "\'");
  }

  if (editor_ && (field->type() == QVariant::Double ||
                  field->type() == QVariant::Int ||
                  field->type() == QVariant::UInt)) {
    s = ::qt_cast<FLLineEdit *>(editor_)->text();
  }

  if (s.isEmpty())
    cursor_->setValueBuffer(fieldName_, QVariant());
  else
    cursor_->setValueBuffer(fieldName_, s);

  if (isVisible() && hasFocus() && field->type() == QVariant::String &&
      field->length() == s.length())
    focusNextPrevChild(true);
}

void FLFieldDB::updateValue()
{
  if (!cursor_)
    return;
  QTextEdit *ted = ::qt_cast<QTextEdit *>(editor_);
  if (!ted)
    return;
  QString t(ted->text());
  if (!cursor_->bufferIsNull(fieldName_)) {
    if (t == cursor_->valueBuffer(fieldName_).toString())
      return;
  } else if (t.isEmpty())
    return;
  t.remove("\\");
  t.replace("'", "\'");
  if (t.isEmpty())
    cursor_->setValueBuffer(fieldName_, QVariant());
  else
    cursor_->setValueBuffer(fieldName_, t);
}

void FLFieldDB::setTextFormat(int f)
{
  textFormat_ = (Qt::TextFormat) f;
  QTextEdit *ted = ::qt_cast<QTextEdit *>(editor_);
  if (ted)
    ted->setTextFormat(textFormat_);
}

int FLFieldDB::textFormat() const
{
  QTextEdit *ted = ::qt_cast<QTextEdit *>(editor_);
  if (ted)
    return ted->textFormat();
  return textFormat_;
}

void FLFieldDB::setEchoMode(int m)
{
  QLineEdit *led = ::qt_cast<QLineEdit *>(editor_);
  if (led)
    led->setEchoMode((QLineEdit::EchoMode) m);
}

int FLFieldDB::echoMode() const
{
  QLineEdit *led = ::qt_cast<QLineEdit *>(editor_);
  if (led)
    return led->echoMode();
  return QLineEdit::Normal;
}

void FLFieldDB::setValue(const QVariant &cv)
{
  if (!cursor_)
    return;

  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return;

  FLFieldMetaData *field = tMD->field(fieldName_);
  if (!field) {
#ifdef FL_DEBUG
    qWarning(tr("FLFieldDB::setValue() : No existe el campo ") + fieldName_);
#endif
    return;
  }

  QVariant v(cv);
  if (field->hasOptionsList()) {
    int idxItem = -1;
    if (v.type() == QVariant::String)
      idxItem = field->optionsList().findIndex(v.toString());
    if (idxItem == -1)
      idxItem = v.toInt();
    ::qt_cast<QComboBox *>(editor_)->setCurrentItem(idxItem);
    updateValue(::qt_cast<QComboBox *>(editor_)->currentText());
    return;
  }

  int type = field->type();
  QVariant::Type fltype = FLFieldMetaData::flDecodeType(type);
  if (v.type() == QVariant::Bool && fltype != QVariant::Bool) {
    if (type == QVariant::Double || type == QVariant::Int || type == QVariant::UInt)
      v = 0;
    else
      v.clear();
  }

  if (v.type() == QVariant::String && v.toString().isEmpty())
    if (type == QVariant::Double || type == QVariant::Int || type == QVariant::UInt)
      v.clear();

  bool isNull = (!v.isValid() || v.isNull());

  if (isNull && !field->allowNull()) {
    QVariant defVal(field->defaultValue());
    if (defVal.isValid() && !defVal.isNull()) {
      v = defVal;
      isNull = false;
    }
  }

  v.cast(fltype);

  switch (type) {
    case QVariant::UInt:
    case QVariant::Int:
    case QVariant::String:
      if (editor_) {
        bool doHome = (::qt_cast<FLLineEdit *>(editor_)->text().isEmpty());
        ::qt_cast<FLLineEdit *>(editor_)->setText(isNull ? QString() : v.toString(), false);
        if (doHome)
          ::qt_cast<FLLineEdit *>(editor_)->home(false);
      }
      break;
    case QVariant::StringList:
      if (!editor_)
        return;
      ::qt_cast<QTextEdit *>(editor_)->setText(isNull ? QString() : v.toString(), false);
      break;
    case QVariant::Double:
      if (editor_) {
        QString s;
        if (!isNull)
          s.setNum(v.toDouble(), 'f', partDecimal_ != -1 ? partDecimal_ : field->partDecimal());
        ::qt_cast<FLLineEdit *>(editor_)->setText(s, false);
      }
      break;
    case FLFieldMetaData::Serial:
      if (editor_)
        ::qt_cast<FLSpinBox *>(editor_)->setValue(isNull ? 0 : v.toUInt());
      break;
    case QVariant::Pixmap:
      if (editorImg_) {
        QCString cs;
        if (!isNull)
          cs = v.toCString();
        if (cs.isEmpty()) {
          editorImg_->clear();
          return;
        }
        QPixmap pix;
        if (!QPixmapCache::find(cs.left(100), pix)) {
          pix.loadFromData(cs);
          QPixmapCache::insert(cs.left(100), pix);
        }
        if (!pix.isNull())
          editorImg_->setPixmap(pix);
        else
          editorImg_->clear();
      }
      break;
    case QVariant::Date:
      if (editor_)
        ::qt_cast<FLDateEdit *>(editor_)->setDate(isNull ? QDate() : v.toDate());
      break;
    case QVariant::Time:
      if (editor_)
        ::qt_cast<QTimeEdit *>(editor_)->setTime(isNull ? QTime() : v.toTime());
      break;
    case QVariant::Bool:
      if (editor_ && !isNull)
        ::qt_cast<QCheckBox *>(editor_)->setChecked(v.toBool());
      break;
  }
}

QVariant FLFieldDB::value()
{
  if (!cursor_)
    return QVariant();

  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return QVariant();

  FLFieldMetaData *field = tMD->field(fieldName_);
  if (!field) {
#ifdef FL_DEBUG
    qWarning(tr("FLFieldDB::value() : No existe el campo ") + fieldName_);
#endif
    return QVariant();
  }

  QVariant v;

  if (field->hasOptionsList()) {
    v = ::qt_cast<QComboBox *>(editor_)->currentItem();
    v.cast(QVariant::Int);
    return v;
  }

  int type = field->type();
  QVariant::Type fltype = FLFieldMetaData::flDecodeType(type);
  if (cursor_->bufferIsNull(fieldName_))
    if (type == QVariant::Double || type == QVariant::Int || type == QVariant::UInt)
      return 0;

  switch (type) {
    case QVariant::UInt:
    case QVariant::Int:
    case QVariant::Double:
    case QVariant::String:
    case QVariant::StringList:
      if (editor_) {
        FLLineEdit *ed_ = ::qt_cast<FLLineEdit *>(editor_);
        if (ed_)
          v = ed_->text();
      }
      break;
    case FLFieldMetaData::Serial:
      if (editor_) {
        FLSpinBox *ed_ = ::qt_cast<FLSpinBox *>(editor_);
        if (ed_)
          v = ed_->value();
      }
      break;
    case QVariant::Pixmap:
      v = cursor_->valueBuffer(fieldName_);
      break;
    case QVariant::Date:
      if (editor_)
        v = ::qt_cast<FLDateEdit *>(editor_)->date().toString(Qt::ISODate);
      break;
    case QVariant::Time:
      if (editor_)
        v = ::qt_cast<QTimeEdit *>(editor_)->time().toString(Qt::ISODate);
      break;
    case QVariant::Bool:
      if (editor_)
        v = QVariant(::qt_cast<QCheckBox *>(editor_)->isChecked(), 0);
      break;
  }

  v.cast(fltype);
  return v;
}

void FLFieldDB::selectAll()
{
  if (!cursor_)
    return;
  if (!cursor_->metadata())
    return;
  FLFieldMetaData *field = cursor_->metadata()->field(fieldName_);
  if (!field)
    return;
  switch (field->type()) {
    case QVariant::UInt:
    case QVariant::Int:
    case QVariant::Double:
    case QVariant::String:
      if (editor_)
        ::qt_cast<FLLineEdit *>(editor_)->selectAll();
      break;
    case FLFieldMetaData::Serial:
      if (editor_)
        ::qt_cast<FLSpinBox *>(editor_)->selectAll();
      break;
  }
}

void FLFieldDB::initCursor()
{
  if (!tableName_.isEmpty() && foreignField_.isEmpty() && fieldRelation_.isEmpty()) {
    cursorBackup_ = cursor_;
    if (cursor_) {
      cursor_ = new FLSqlCursor(tableName_, true,
                                cursor_->db()->connectionName(), 0, 0, this);
    } else {
      if (!topWidget_)
        return;
      cursor_ = new FLSqlCursor(tableName_, true,
                                FLSqlConnections::database()->connectionName(),
                                0, 0, this);
    }
    cursor_->setModeAccess(FLSqlCursor::BROWSE);
    if (showed)
      disconnect(cursor_, SIGNAL(cursorUpdated()), this, SLOT(refresh()));
    connect(cursor_, SIGNAL(cursorUpdated()), this, SLOT(refresh()));
    return;
  } else {
    if (cursorBackup_) {
      disconnect(cursor_, SIGNAL(cursorUpdated()), this, SLOT(refresh()));
      cursor_ = cursorBackup_;
      cursorBackup_ = 0;
    }
  }

  if (!cursor_)
    return;

  if (!cursor_->metadata())
    return;

  if (tableName_.isEmpty() || foreignField_.isEmpty() || fieldRelation_.isEmpty()) {
    if (!foreignField_.isEmpty() && !fieldRelation_.isEmpty()) {
      if (showed)
        disconnect(cursor_, SIGNAL(bufferChanged(const QString &)), this,
                   SLOT(refresh(const QString &)));
      connect(cursor_, SIGNAL(bufferChanged(const QString &)), this, SLOT(refresh(const QString &)));
    }
    if (showed) {
      disconnect(cursor_, SIGNAL(newBuffer()), this, SLOT(refresh()));
      disconnect(cursor_, SIGNAL(bufferChanged(const QString &)), this,
                 SLOT(refreshQuick(const QString &)));
    }
    connect(cursor_, SIGNAL(newBuffer()), this, SLOT(refresh()));
    connect(cursor_, SIGNAL(bufferChanged(const QString &)), this,
            SLOT(refreshQuick(const QString &)));
    return;
  }

  if (!cursorAux) {
    if (cursorAuxInit)
      return;

    FLTableMetaData *tMD = cursor_->db()->manager()->metadata(tableName_);
    if (!tMD)
      return;

    disconnect(cursor_, SIGNAL(newBuffer()), this, SLOT(refresh()));
    disconnect(cursor_, SIGNAL(bufferChanged(const QString &)), this,
               SLOT(refreshQuick(const QString &)));

    cursorAux = cursor_;
    QString curName(cursor_->metadata()->name());

    FLRelationMetaData *rMD = tMD->relation(fieldRelation_, foreignField_, curName);
    if (!rMD) {
      bool checkIntegrity = false;
      FLRelationMetaData *testM1 = cursor_->metadata()->relation(foreignField_, fieldRelation_,
                                                                 tableName_);
      if (testM1)
        checkIntegrity = (testM1->cardinality() == FLRelationMetaData::RELATION_M1);
      FLFieldMetaData *fMD = tMD->field(fieldRelation_);
      if (fMD) {
        rMD = new FLRelationMetaData(curName, foreignField_, FLRelationMetaData::RELATION_1M,
                                     false, false, checkIntegrity);
        fMD->addRelationMD(rMD);
#ifdef FL_DEBUG
        qWarning(tr("FLFieldDB : La relación entre la tabla del formulario ( %1 ) y la tabla ( %2 ) de este campo ( %3 ) no existe, pero sin embargo se han indicado los campos de relación( %4, %5 )")
                 .arg(curName).arg(tableName_).arg(fieldName_).arg(fieldRelation_).arg(foreignField_));
        qWarning(tr("FLFieldDB : Creando automáticamente %1.%2 --1M--> %3.%4")
                 .arg(tableName_).arg(fieldRelation_).arg(curName).arg(foreignField_));
#endif
      }
#ifdef FL_DEBUG
      else {
        qWarning(tr("FLFieldDB : El campo ( %1 ) indicado en la propiedad fieldRelation no se encuentra en la tabla ( %2 )")
                 .arg(fieldRelation_).arg(tableName_));
      }
#endif
    }

    cursor_ = new FLSqlCursor(tableName_, false, cursor_->db()->connectionName(), cursorAux, rMD,
                              this);
    if (!cursor_) {
      cursor_ = cursorAux;
      if (showed) {
        disconnect(cursor_, SIGNAL(newBuffer()), this, SLOT(refresh()));
        disconnect(cursor_, SIGNAL(bufferChanged(const QString &)), this,
                   SLOT(refreshQuick(const QString &)));
      }
      connect(cursor_, SIGNAL(newBuffer()), this, SLOT(refresh()));
      connect(cursor_, SIGNAL(bufferChanged(const QString &)), this,
              SLOT(refreshQuick(const QString &)));
      cursorAux = 0;
      if (tMD && !tMD->inCache()) {
        delete tMD;
      }
      return;
    } else {
      if (showed)
        disconnect(cursorAux, SIGNAL(newBuffer()), this, SLOT(setNoShowed()));
      connect(cursorAux, SIGNAL(newBuffer()), this, SLOT(setNoShowed()));
    }

    cursor_->setModeAccess(FLSqlCursor::BROWSE);
    if (showed) {
      disconnect(cursor_, SIGNAL(newBuffer()), this, SLOT(refresh()));
      disconnect(cursor_, SIGNAL(bufferChanged(const QString &)), this,
                 SLOT(refreshQuick(const QString &)));
    }
    connect(cursor_, SIGNAL(newBuffer()), this, SLOT(refresh()));
    connect(cursor_, SIGNAL(bufferChanged(const QString &)), this,
            SLOT(refreshQuick(const QString &)));

    cursorAuxInit = true;
    cursor_->append(cursor_->db()->db()->recordInfo(tableName_).find(fieldName_));
    cursor_->append(cursor_->db()->db()->recordInfo(tableName_).find(fieldRelation_));
    if (tMD && !tMD->inCache()) {
      delete tMD;
    }
  }
}

void FLFieldDB::initEditor()
{
  if (!cursor_)
    return;

  if (editor_) {
    delete editor_;
    editor_ = 0;
  }

  if (editorImg_) {
    delete editorImg_;
    editorImg_ = 0;
  }

  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return;
  FLFieldMetaData *field = tMD->field(fieldName_);
  if (!field)
    return;

  int type = field->type();
  int len = field->length();
  int partInteger = field->partInteger();
  int partDecimal = partDecimal_ > -1 ? partDecimal_ : field->partDecimal();
  QString rX = field->regExpValidator();
  bool ol = field->hasOptionsList();

  QString rt;
  if (field->relationM1())
    if (field->relationM1()->foreignTable() != tMD->name())
      rt = field->relationM1()->foreignTable();

  bool hasPushButtonDB = false;
  fieldAlias_ = field->alias();

  textLabelDB->setFont(qApp->font());
  if (showAlias_ && type != QVariant::Pixmap && type != QVariant::Bool) {
    if (!field->allowNull() && field->editable())
      textLabelDB->setText(fieldAlias_ + QString::fromLatin1("*"));
    else
      textLabelDB->setText(fieldAlias_);
  } else
    textLabelDB->hide();

  if (!rt.isEmpty()) {
    hasPushButtonDB = true;
    FLTableMetaData *tmd = cursor_->db()->manager()->metadata(rt);
    if (!tmd)
      pushButtonDB->setDisabled(true);
    if (tmd && !tmd->inCache())
      delete tmd;
  }

  initMaxSize_ = maximumSize();
  initMinSize_ = minimumSize();

  switch (type) {
    case QVariant::UInt:
    case QVariant::Int:
    case QVariant::Double:
    case QVariant::String:
      if (ol) {
        editor_ = new QComboBox(true, this, "editor");

        ::qt_cast<QComboBox *>(editor_)->setEditable(false);
        ::qt_cast<QComboBox *>(editor_)->setAutoCompletion(true);
        ::qt_cast<QComboBox *>(editor_)->setFont(qApp->font());
        if (cursor_->modeAccess() != FLSqlCursor::BROWSE)
          if (!field->allowNull())
            ::qt_cast<QComboBox *>(editor_)->setPaletteBackgroundColor(notNullColor());

        QStringList olTranslated;
        QStringList olNoTranslated(field->optionsList());
        int countOl = olNoTranslated.count();
        for (int i = 0; i < countOl; ++i)
          olTranslated << FLUtil::translate("MetaData", olNoTranslated[i]);
        ::qt_cast<QComboBox *>(editor_)->insertStringList(olTranslated);

        editor_->installEventFilter(this);
        if (showed)
          disconnect(editor_, SIGNAL(activated(const QString &)), this,
                     SLOT(updateValue(const QString &)));
        connect(editor_, SIGNAL(activated(const QString &)), this,
                SLOT(updateValue(const QString &)));
      } else {
        editor_ = new FLLineEdit(this, "editor");

        ::qt_cast<FLLineEdit *>(editor_)->setFont(qApp->font());
        ::qt_cast<FLLineEdit *>(editor_)->type = type;
        ::qt_cast<FLLineEdit *>(editor_)->partDecimal = partDecimal;
        if (cursor_->modeAccess() != FLSqlCursor::BROWSE)
          if (!field->allowNull())
            ::qt_cast<FLLineEdit *>(editor_)->setPaletteBackgroundColor(notNullColor());
        editor_->installEventFilter(this);

        if (type == QVariant::Double) {
          ::qt_cast<FLLineEdit *>(editor_)->setValidator(
            new FLDoubleValidator(0, pow(10, partInteger) - 1, partDecimal, editor_)
          );
          ::qt_cast<FLLineEdit *>(editor_)->setAlignment(Qt::AlignRight);
        } else {
          if (type == QVariant::UInt || type == QVariant::Int) {
            if (type == QVariant::UInt) {
              ::qt_cast<FLLineEdit *>(editor_)->setValidator(
                new FLUIntValidator(0, ((int) pow(10, partInteger) - 1), editor_)
              );
            } else {
              ::qt_cast<FLLineEdit *>(editor_)->setValidator(
                new FLIntValidator(((int)(pow(10, partInteger) - 1) * (-1)),
                                   ((int) pow(10, partInteger) - 1), editor_)
              );
            }
            ::qt_cast<FLLineEdit *>(editor_)->setAlignment(Qt::AlignRight);
          } else {
            ::qt_cast<FLLineEdit *>(editor_)->setMaxLength(len);
            if (!rX.isEmpty()) {
              QRegExp r(rX);
              ::qt_cast<FLLineEdit *>(editor_)->setValidator(new QRegExpValidator(r, editor_));
            }
            ::qt_cast<FLLineEdit *>(editor_)->setAlignment(Qt::AlignLeft);

            connect(this, SIGNAL(keyF4Pressed()), this, SLOT(toggleAutoCompletion()));
            if (autoCompMode_ == OnDemandF4) {
              QToolTip::add(editor_, tr("Para completado automático pulsar F4"));
              QWhatsThis::add(editor_, tr("Para completado automático pulsar F4"));
            } else if (autoCompMode_ == AlwaysAuto) {
              QToolTip::add(editor_, tr("Completado automático permanente activado"));
              QWhatsThis::add(editor_, tr("Completado automático permanente activado"));
            } else {
              QToolTip::add(editor_, tr("Completado automático desactivado"));
              QWhatsThis::add(editor_, tr("Completado automático desactivado"));
            }
          }
        }

        if (showed) {
          disconnect(editor_, SIGNAL(lostFocus()), this, SLOT(emitLostFocus()));
          disconnect(editor_, SIGNAL(textChanged(const QString &)), this,
                     SLOT(updateValue(const QString &)));
          disconnect(editor_, SIGNAL(textChanged(const QString &)), this,
                     SLOT(emitTextChanged(const QString &)));
        }
        connect(editor_, SIGNAL(lostFocus()), this, SLOT(emitLostFocus()));
        connect(editor_, SIGNAL(textChanged(const QString &)), this,
                SLOT(updateValue(const QString &)));
        connect(editor_, SIGNAL(textChanged(const QString &)), this,
                SLOT(emitTextChanged(const QString &)));
        if (hasPushButtonDB) {
          if (showed) {
            disconnect(this, SIGNAL(keyF2Pressed()), pushButtonDB, SLOT(animateClick()));
            disconnect(this, SIGNAL(labelClicked()), this, SLOT(openFormRecordRelation()));
          }
          connect(this, SIGNAL(keyF2Pressed()), pushButtonDB, SLOT(animateClick()));
          connect(this, SIGNAL(labelClicked()), this, SLOT(openFormRecordRelation()));
          textLabelDB->installEventFilter(this);
          QFont tlF = textLabelDB->font();
          tlF.setUnderline(true);
          textLabelDB->setFont(tlF);
          QColor cB("blue");
          textLabelDB->setPaletteForegroundColor(cB);
          textLabelDB->setCursor(QCursor::PointingHandCursor);
        }
      }

      editor_-> setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 7, (QSizePolicy::SizeType) 0,
                                          editor_->sizePolicy().hasHeightForWidth()));
      FLWidgetFieldDBLayout->addWidget(editor_);
      break;

    case FLFieldMetaData::Serial:
      editor_ = new FLSpinBox(this, "editor");

      ::qt_cast<FLSpinBox *>(editor_)->setFont(qApp->font());
      ::qt_cast<FLSpinBox *>(editor_)->setMaxValue(((int) pow(10, partInteger) - 1));

      editor_-> setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 7, (QSizePolicy::SizeType) 0,
                                          editor_->sizePolicy().hasHeightForWidth()));
      FLWidgetFieldDBLayout->addWidget(editor_);
      editor_->installEventFilter(this);

      if (showed)
        disconnect(editor_, SIGNAL(valueChanged(const QString &)), this,
                   SLOT(updateValue(const QString &)));
      connect(editor_, SIGNAL(valueChanged(const QString &)), this,
              SLOT(updateValue(const QString &)));
      break;

    case QVariant::Pixmap:
      if (cursor_->modeAccess() != FLSqlCursor::BROWSE) {
        FLWidgetFieldDBLayout->setDirection(QBoxLayout::Down);
        editorImg_ = new FLPixmapView(this);
        editorImg_->setFocusPolicy(QWidget::NoFocus);
        editorImg_->setSizePolicy(sizePolicy());
        editorImg_->setMaximumSize(initMaxSize_);
        editorImg_->setMinimumSize(initMinSize_);
        editorImg_->setAutoScaled(true);
        FLWidgetFieldDBLayout->addWidget(editorImg_);

        if (!pbAux3_) {
          QSpacerItem *spcBut = new QSpacerItem(20, 20, QSizePolicy::Expanding,
                                                QSizePolicy::Minimum);
          lytButtons->addItem(spcBut);
          pbAux3_ = new QPushButton(this, "pbAux3");
          pbAux3_->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed,
                                             pbAux3_->sizePolicy().hasHeightForWidth()));
          pbAux3_->setMinimumSize(QSize(22, 22));
          pbAux3_->setFocusPolicy(QPushButton::NoFocus);
          pbAux3_->setIconSet(QIconSet(QPixmap::fromMimeSource("file_open.png")));
          pbAux3_->setText(QString::null);
          QToolTip::add(pbAux3_, tr("Abrir fichero de imagen"));
          QWhatsThis::add(pbAux3_, tr("Abrir fichero de imagen"));
          lytButtons->addWidget(pbAux3_);
          if (showed)
            disconnect(pbAux3_, SIGNAL(clicked()), this, SLOT(searchPixmap()));
          connect(pbAux3_, SIGNAL(clicked()), this, SLOT(searchPixmap()));
          if (!hasPushButtonDB) {
            if (showed)
              disconnect(this, SIGNAL(keyF2Pressed()), pbAux3_, SLOT(animateClick()));
            connect(this, SIGNAL(keyF2Pressed()), pbAux3_, SLOT(animateClick()));
            pbAux3_->setFocusPolicy(QPushButton::StrongFocus);
            pbAux3_->installEventFilter(this);
          }
        }

        // Silix
        if (!pbAux4_) {
          pbAux4_ = new QPushButton(this, "pbAux4");
          pbAux4_->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed,
                                             pbAux4_->sizePolicy().hasHeightForWidth()));
          pbAux4_->setMinimumSize(QSize(22, 22));
          pbAux4_->setFocusPolicy(QPushButton::NoFocus);
          pbAux4_->setIconSet(QIconSet(QPixmap::fromMimeSource("paste.png")));
          pbAux4_->setText(QString::null);
          QToolTip::add(pbAux4_, tr("Pegar imagen desde el portapapeles"));
          QWhatsThis::add(pbAux4_, tr("Pegar imagen desde el portapapeles"));
          lytButtons->addWidget(pbAux4_);
          if (showed)
            disconnect(pbAux4_, SIGNAL(clicked()), this, SLOT(setPixmapFromClipboard()));
          connect(pbAux4_, SIGNAL(clicked()), this, SLOT(setPixmapFromClipboard()));
        }

        if (!pbAux_) {
          pbAux_ = new QPushButton(this, "pbAux");
          pbAux_->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed,
                                            pbAux_->sizePolicy().hasHeightForWidth()));
          pbAux_->setMinimumSize(QSize(22, 22));
          pbAux_->setFocusPolicy(QPushButton::NoFocus);
          pbAux_->setIconSet(QIconSet(QPixmap::fromMimeSource("eraser.png")));
          pbAux_->setText(QString::null);
          QToolTip::add(pbAux_, tr("Borrar imagen"));
          QWhatsThis::add(pbAux_, tr("Borrar imagen"));
          lytButtons->addWidget(pbAux_);
          if (showed)
            disconnect(pbAux_, SIGNAL(clicked()), this, SLOT(clearPixmap()));
          connect(pbAux_, SIGNAL(clicked()), this, SLOT(clearPixmap()));
        }

        if (!pbAux2_) {
          pbAux2_ = new QPushButton(this, "pbAux2");
          QPopupMenu *savepixmap = new QPopupMenu(pbAux2_);
          QStrList fmt = QImage::outputFormats();
          int id = 0;
          for (const char *f = fmt.first(); f; f = fmt.next(), ++id)
            savepixmap->insertItem(f, id);
          pbAux2_->setPopup(savepixmap);
          pbAux2_->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed,
                                             pbAux2_->sizePolicy().hasHeightForWidth()));
          pbAux2_->setMinimumSize(QSize(22, 22));
          pbAux2_->setFocusPolicy(QPushButton::NoFocus);
          pbAux2_->setIconSet(QIconSet(QPixmap::fromMimeSource("filesaveas.png")));
          pbAux2_->setText(QString::null);
          QToolTip::add(pbAux2_, tr("Guardar imagen como..."));
          QWhatsThis::add(pbAux_, tr("Guardar imagen como.."));
          lytButtons->addWidget(pbAux2_);
          if (showed)
            disconnect(savepixmap, SIGNAL(activated(int)), this, SLOT(savePixmap(int)));
          connect(savepixmap, SIGNAL(activated(int)), this, SLOT(savePixmap(int)));
        }

        if (hasPushButtonDB)
          pushButtonDB->installEventFilter(this);
      }
      break;

    case QVariant::Date:
      editor_ = new FLDateEdit(this, "editor");

      ::qt_cast<FLDateEdit *>(editor_)->setFont(qApp->font());

      editor_-> setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 7, (QSizePolicy::SizeType) 0,
                                          editor_->sizePolicy().hasHeightForWidth()));
      FLWidgetFieldDBLayout->insertWidget(1, editor_);

      ::qt_cast<FLDateEdit *>(editor_)->setOrder(QDateEdit::DMY);
      ::qt_cast<FLDateEdit *>(editor_)->setAutoAdvance(true);
      ::qt_cast<FLDateEdit *>(editor_)->setSeparator("-");
      editor_->installEventFilter(this);

      if (cursor_->modeAccess() != FLSqlCursor::BROWSE) {
        if (!pbAux_) {
          pbAux_ = new QPushButton(this, "pbAux");
          pbAux_->setFlat(true);
          pbAux_->setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 0, (QSizePolicy::SizeType) 0,
                                            0, 0, pbAux_->sizePolicy().hasHeightForWidth()));
          pbAux_->setMinimumSize(QSize(25, 25));
          pbAux_->setMaximumSize(QSize(25, 25));
          pbAux_->setFocusPolicy(QPushButton::NoFocus);
          pbAux_->setIconSet(QIconSet(QPixmap::fromMimeSource("date.png")));
          pbAux_->setText(QString::null);
          QToolTip::add(pbAux_, tr("Seleccionar fecha (F2)"));
          QWhatsThis::add(pbAux_, tr("Seleccionar fecha (F2)"));
          lytButtons->addWidget(pbAux_);
          if (showed) {
            disconnect(pbAux_, SIGNAL(clicked()), this, SLOT(toggleDatePicker()));
            disconnect(this, SIGNAL(keyF2Pressed()), pbAux_, SLOT(animateClick()));
          }
          connect(pbAux_, SIGNAL(clicked()), this, SLOT(toggleDatePicker()));
          connect(this, SIGNAL(keyF2Pressed()), pbAux_, SLOT(animateClick()));
        }
      }

      if (showed)
        disconnect(editor_, SIGNAL(valueChanged(const QDate &)), this,
                   SLOT(updateValue(const QDate &)));
      connect(editor_, SIGNAL(valueChanged(const QDate &)), this, SLOT(updateValue(const QDate &)));
      if (cursor_->modeAccess() == FLSqlCursor::INSERT && !field->allowNull()) {
        QVariant defVal(field->defaultValue());
        if (!defVal.isValid() || defVal.isNull())
          ::qt_cast<FLDateEdit *>(editor_)->setDate(QDate::currentDate());
        else
          ::qt_cast<FLDateEdit *>(editor_)->setDate(defVal.toDate());
      }
      break;

    case QVariant::Time: {
      editor_ = new QTimeEdit(this, "editor");

      ::qt_cast<QTimeEdit *>(editor_)->setFont(qApp->font());
      ::qt_cast<QTimeEdit *>(editor_)->setAutoAdvance(true);

      editor_-> setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 7, (QSizePolicy::SizeType) 0,
                                          editor_->sizePolicy().hasHeightForWidth()));
      FLWidgetFieldDBLayout->addWidget(editor_);
      editor_->installEventFilter(this);

      if (showed)
        disconnect(editor_, SIGNAL(valueChanged(const QTime &)), this,
                   SLOT(updateValue(const QTime &)));
      connect(editor_, SIGNAL(valueChanged(const QTime &)), this, SLOT(updateValue(const QTime &)));
      if (cursor_->modeAccess() == FLSqlCursor::INSERT && !field->allowNull()) {
        QVariant defVal(field->defaultValue());
        if (!defVal.isValid() || defVal.isNull())
          ::qt_cast<QTimeEdit *>(editor_)->setTime(QTime::currentTime());
        else
          ::qt_cast<QTimeEdit *>(editor_)->setTime(defVal.toTime());
      }
    }
    break;

    case QVariant::StringList: {
      QTextEdit *ted = new QTextEdit(this, "editor");
      editor_ = ted;

      ted->setFont(qApp->font());
      ted->setTabChangesFocus(true);

      editor_->setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 7, QSizePolicy::Expanding,
                                         editor_->sizePolicy().hasHeightForWidth()));

      ted->setTextFormat(textFormat_);

      if (textFormat_ == Qt::RichText && cursor_->modeAccess() != FLSqlCursor::BROWSE) {
        FLWidgetFieldDBLayout->setDirection(QBoxLayout::Down);
        FLWidgetFieldDBLayout->remove(textLabelDB);
        AQTextEditBar *textEditTab_ = new AQTextEditBar(this, "textEditTab_", textLabelDB);
        textEditTab_->doConnections(ted);
        FLWidgetFieldDBLayout->addWidget(textEditTab_);
      }

      FLWidgetFieldDBLayout->addWidget(editor_);
      editor_->installEventFilter(this);

      if (showed)
        disconnect(editor_, SIGNAL(textChanged()), this, SLOT(updateValue()));
      connect(editor_, SIGNAL(textChanged()), this, SLOT(updateValue()));

      connect(this, SIGNAL(keyF4Pressed()), this, SLOT(toggleAutoCompletion()));
      if (autoCompMode_ == OnDemandF4) {
        QToolTip::add(editor_, tr("Para completado automático pulsar F4"));
        QWhatsThis::add(editor_, tr("Para completado automático pulsar F4"));
      } else if (autoCompMode_ == AlwaysAuto) {
        QToolTip::add(editor_, tr("Completado automático permanente activado"));
        QWhatsThis::add(editor_, tr("Completado automático permanente activado"));
      } else {
        QToolTip::add(editor_, tr("Completado automático desactivado"));
        QWhatsThis::add(editor_, tr("Completado automático desactivado"));
      }
    }
    break;

    case QVariant::Bool:
      editor_ = new QCheckBox(this, "editor");

      ::qt_cast<QCheckBox *>(editor_)->setText(tMD->fieldNameToAlias(fieldName_));
      ::qt_cast<QCheckBox *>(editor_)->setFont(qApp->font());
      editor_->installEventFilter(this);

      editor_->setMinimumWidth(fontMetrics().width(tMD->fieldNameToAlias(fieldName_))
                               + fontMetrics().maxWidth() * 2);
      editor_->setSizePolicy(QSizePolicy((QSizePolicy::SizeType) 7, (QSizePolicy::SizeType) 0,
                                         editor_->sizePolicy().hasHeightForWidth()));
      FLWidgetFieldDBLayout->addWidget(editor_);

      if (showed)
        disconnect(editor_, SIGNAL(toggled(bool)), this, SLOT(updateValue(bool)));
      connect(editor_, SIGNAL(toggled(bool)), this, SLOT(updateValue(bool)));
      break;
  }

  if (editor_) {
    editor_->setFocusPolicy(QWidget::StrongFocus);
    setFocusProxy(editor_);
    setTabOrder(pushButtonDB, editor_);
    if (hasPushButtonDB) {
      pushButtonDB->setFocusPolicy(QWidget::NoFocus);
      QToolTip::add(editor_, tr("Para buscar un valor en la tabla relacionada pulsar F2"));
      QWhatsThis::add(editor_, tr("Para buscar un valor en la tabla relacionada pulsar F2"));
    }
  } else if (editorImg_) {
    editorImg_->setFocusPolicy(QWidget::NoFocus);
    if (hasPushButtonDB)
      pushButtonDB->setFocusPolicy(QWidget::StrongFocus);
  }

  if (!hasPushButtonDB)
    pushButtonDB->hide();

  if (initMaxSize_.width() < 80)
    setShowEditor(false);
  else
    setShowEditor(showEditor_);

}

void FLFieldDB::setPartDecimal(int d)
{
  FLLineEdit *editor = ::qt_cast<FLLineEdit *>(editor_);
  if (editor && editor->type == QVariant::Double) {
    partDecimal_ = d;
    editor->partDecimal = d;
    refreshQuick(fieldName_);
    editor->setText(editor->text(), false);
  }
}

void FLFieldDB::openFormRecordRelation()
{
  if (!cursor_)
    return;

  if (fieldName_.isEmpty())
    return;
  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return;

  FLFieldMetaData *field = tMD->field(fieldName_);
  if (!field)
    return;

  if (!field->relationM1()) {
#ifdef FL_DEBUG
    qWarning("FLFieldDB : " + tr("El campo de búsqueda debe tener una relación M1"));
#endif
    return;
  }

  FLSqlCursor *c = 0;
  FLFieldMetaData *fMD = field->associatedField();
  FLAction *a = 0;

  QVariant v(cursor_->valueBuffer(field->name()));
  if (v.toString().isEmpty() || (fMD && cursor_->bufferIsNull(fMD->name()))) {
    QMessageBox::warning(qApp->focusWidget(), tr("Aviso"),
                         tr("Debe indicar un valor para %1").arg(field->alias()), QMessageBox::Ok,
                         0, 0);
    return;
  }
  FLManager *mng = cursor_->db()->manager();
  c = new FLSqlCursor(field->relationM1()->foreignTable(), true, cursor_->db()->connectionName());
  c->select(mng->formatAssignValue(field->relationM1()->foreignField(), field,
                                   v, true));
  if (c->size() <= 0)
    return;
  c->next();

  if (actionName_.isEmpty())
    a = mng->action(field->relationM1()->foreignTable());
  else
    a = mng->action(actionName_);

  c->setAction(a);

  int modeAccess = cursor_->modeAccess();
  if (modeAccess == FLSqlCursor::INSERT || modeAccess == FLSqlCursor::DEL)
    modeAccess = FLSqlCursor::EDIT;
  c->openFormInMode(modeAccess, false);
}

void FLFieldDB::searchValue()
{
  if (!cursor_)
    return;

  if (fieldName_.isEmpty())
    return;
  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return;
  FLFieldMetaData *field = tMD->field(fieldName_);
  if (!field)
    return;

  if (!field->relationM1()) {
#ifdef FL_DEBUG
    qWarning("FLFieldDB : " + tr("El campo de búsqueda debe tener una relación M1"));
#endif
    return;
  }

  FLFormSearchDB *f = 0;
  FLSqlCursor *c = 0;
  FLFieldMetaData *fMD = field->associatedField();
  FLAction *a = 0;

  if (fMD) {
    if (!fMD->relationM1()) {
#ifdef FL_DEBUG
      qWarning("FLFieldDB : " + tr("El campo asociado debe tener una relación M1"));
#endif
      return;
    }

    QVariant v(cursor_->valueBuffer(fMD->name()));
    if (v.toString().isEmpty() || cursor_->bufferIsNull(fMD->name())) {
      QMessageBox::warning(qApp->focusWidget(), tr("Aviso"),
                           tr("Debe indicar un valor para %1").arg(fMD->alias()), QMessageBox::Ok,
                           0, 0);
      return;
    }
    FLManager *mng = cursor_->db()->manager();
    c = new FLSqlCursor(fMD->relationM1()->foreignTable(), true, cursor_->db()->connectionName());
    c->select(mng->formatAssignValue(fMD->relationM1()->foreignField(), fMD,
                                     v, true));
    if (c->size() > 0)
      c->next();

    if (actionName_.isEmpty())
      a = mng->action(field->relationM1()->foreignTable());
    else {
      a = mng->action(actionName_);
      a->setTable(field->relationM1()->foreignTable());
    }

    f = new FLFormSearchDB(c, a->name(), topWidget_);
  } else {
    FLManager *mng = cursor_->db()->manager();
    if (actionName_.isEmpty()) {
      a = mng->action(field->relationM1()->foreignTable());
      if (!a)
        return;
    } else {
      a = mng->action(actionName_);
      if (!a)
        return;
      a->setTable(field->relationM1()->foreignTable());
    }

    c = new FLSqlCursor(a->table(), true, cursor_->db()->connectionName());
    f = new FLFormSearchDB(c, a->name(), topWidget_);
  }

  f->setMainWidget();

  QObjectList *lObjs = f->queryList("FLTableDB");
  QObject *obj = lObjs->first();
  delete lObjs;
  FLTableDB *objTdb = ::qt_cast<FLTableDB *>(obj);
  if (fMD && objTdb) {
    objTdb->setTableName(field->relationM1()->foreignTable());
    objTdb->setFieldRelation(field->associatedFieldFilterTo());
    objTdb->setForeignField(fMD->relationM1()->foreignField());
    if (fMD->relationM1()->foreignTable() == tMD->name())
      objTdb->setReadOnly(true);
  }

  f->setFilter(filter_);

  if (f->mainWidget()) {
    if (objTdb) {
      QVariant curValue(value());
      if (field->type() == QVariant::String && !curValue.toString().isEmpty()) {
        objTdb->setInitSearch(curValue.toString());
        objTdb->putFirstCol(field->relationM1()->foreignField());
      }
      QTimer::singleShot(0, objTdb->lineEditSearch, SLOT(setFocus()));
    }
    QVariant v(f->exec(field->relationM1()->foreignField()));
    if (v.isValid() && !v.isNull()) {
      setValue(QVariant());
      setValue(v);
    }
  }

  f->close();

  if (c) {
    disconnect(c, 0, 0, 0);
    c->deleteLater();
  }
}

void FLFieldDB::searchPixmap()
{
  if (!cursor_ || !editorImg_)
    return;

  if (fieldName_.isEmpty())
    return;
  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return;
  FLFieldMetaData *field = tMD->field(fieldName_);
  if (!field)
    return;

  if (field->type() == QVariant::Pixmap) {
    QFileDialog *fd = new QFileDialog(this, 0, true);
    FLPixmapView *p = new FLPixmapView(fd);

    p->setAutoScaled(true);
    fd->setContentsPreviewEnabled(TRUE);
    fd->setContentsPreview(p, p);
    fd->setPreviewMode(QFileDialog::Contents);
    fd->setCaption(tr("Elegir archivo"));
    fd->setFilter("*");

    QString filename;
    if (fd->exec() == QDialog::Accepted)
      filename = fd->selectedFile();
    if (filename.isEmpty())
      return;
    setPixmap(filename);
  }
}

void FLFieldDB::setPixmap(const QString &filename)
{
  QImage img(filename);

  if (img.isNull())
    return;

  QApplication::setOverrideCursor(waitCursor);

  QPixmap pix;
  QCString s;
  QBuffer buffer(s);

  // Silix
  if (img.width() <= maxPixImages_ && img.height() <= maxPixImages_)
    pix.convertFromImage(img);
  else {
    int newWidth, newHeight;
    if (img.width() < img.height()) {
      newHeight = maxPixImages_;
      newWidth = qRound(newHeight * img.width() / img.height());
    } else {
      newWidth = maxPixImages_;
      newHeight = qRound(newWidth * img.height() / img.width());
    }
    pix.convertFromImage(img.scale(newWidth, newHeight, QImage::ScaleMin));
  }

  QApplication::restoreOverrideCursor();

  if (pix.isNull())
    return;

  editorImg_->setPixmap(pix);

  QApplication::setOverrideCursor(waitCursor);

  buffer.open(IO_WriteOnly);
  pix.save(&buffer, "XPM");

  QApplication::restoreOverrideCursor();

  if (s.isEmpty())
    return;

  if (!QPixmapCache::find(s.left(100)))
    QPixmapCache::insert(s.left(100), pix);

  updateValue(QString(s));
}

// Silix
void FLFieldDB::setPixmapFromPixmap(const QPixmap &pixmap, const int w, const int h)
{
  if (pixmap.isNull())
    return;

  QApplication::setOverrideCursor(waitCursor);

  QPixmap pix;
  QCString s;
  QBuffer buffer(s);

  QImage img = pixmap.convertToImage();
  if (w != 0 && h != 0)
    pix.convertFromImage(img.scale(w, h, QImage::ScaleMin));
  else
    pix.convertFromImage(img);

  QApplication::restoreOverrideCursor();

  if (pix.isNull())
    return;

  editorImg_->setPixmap(pix);

  QApplication::setOverrideCursor(waitCursor);

  buffer.open(IO_WriteOnly);
  pix.save(&buffer, "XPM");

  QApplication::restoreOverrideCursor();

  if (s.isEmpty())
    return;

  if (!QPixmapCache::find(s.left(100)))
    QPixmapCache::insert(s.left(100), pix);

  updateValue(QString(s));
}

// Silix
void FLFieldDB::setPixmapFromClipboard()
{
  QImage img = QApplication::clipboard()->image();
  if (img.isNull())
    return;

  QApplication::setOverrideCursor(waitCursor);

  QPixmap pix;
  QCString s;
  QBuffer buffer(s);

  // Silix
  if (img.width() <= maxPixImages_ && img.height() <= maxPixImages_)
    pix.convertFromImage(img);
  else {
    int newWidth, newHeight;
    if (img.width() < img.height()) {
      newHeight = maxPixImages_;
      newWidth = qRound(newHeight * img.width() / img.height());
    } else {
      newWidth = maxPixImages_;
      newHeight = qRound(newWidth * img.height() / img.width());
    }
    pix.convertFromImage(img.scale(newWidth, newHeight, QImage::ScaleMin));
  }

  QApplication::restoreOverrideCursor();

  if (pix.isNull())
    return;

  editorImg_->setPixmap(pix);

  QApplication::setOverrideCursor(waitCursor);

  buffer.open(IO_WriteOnly);
  pix.save(&buffer, "XPM");

  QApplication::restoreOverrideCursor();

  if (s.isEmpty())
    return;

  if (!QPixmapCache::find(s.left(100)))
    QPixmapCache::insert(s.left(100), pix);

  updateValue(QString(s));
}



// Silix
void FLFieldDB::savePixmap(const QString &filename, const char *format)
{
  if (editorImg_) {
    if (!filename.isEmpty()) {
      QPixmap pix;
      QApplication::setOverrideCursor(waitCursor);
      pix.loadFromData(value().toCString());
      if (!pix.isNull())
        if (!pix.save(filename, format))
          QMessageBox::warning(this, tr("Error"), tr("Error guardando fichero"));
      QApplication::restoreOverrideCursor();
    }
  }
}

// Silix
QPixmap FLFieldDB::pixmap()
{
  QPixmap pix;
  pix.loadFromData(value().toCString());
  return pix;
}

void FLFieldDB::setFilter(const QString &f)
{
  if (filter_ != f) {
    filter_ = f;
    setMapValue();
  }
}

QString FLFieldDB::filter() const
{
  return filter_;
}

QString FLFieldDB::fieldName() const
{
  return fieldName_;
}

QString FLFieldDB::actionName() const
{
  return actionName_;
}

QString FLFieldDB::tableName() const
{
  return tableName_;
}

QString FLFieldDB::foreignField() const
{
  return foreignField_;
}

QString FLFieldDB::fieldRelation() const
{
  return fieldRelation_;
}

QString FLFieldDB::fieldAlias() const
{
  return fieldAlias_;
}

void FLFieldDB::refreshQuick(const QString &fN)
{
  if (fN.isEmpty() || fN != fieldName_ || !cursor_)
    return;

  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return;
  FLFieldMetaData *field = tMD->field(fieldName_);
  if (!field)
    return;
  if (field->outTransaction())
    return;

  int type = field->type();

  if (type != QVariant::Pixmap && !editor_)
    return;

  QVariant v(cursor_->valueBuffer(fieldName_));
  bool null = cursor_->bufferIsNull(fieldName_);
  int partDecimal = partDecimal_ != -1 ? partDecimal_ : field->partDecimal();
  bool ol = field->hasOptionsList();

  switch (type) {
    case QVariant::Double:
      if (v.toDouble() == ::qt_cast<FLLineEdit *>(editor_)->text().toDouble())
        return;
      disconnect(editor_, SIGNAL(textChanged(const QString &)), this,
                 SLOT(updateValue(const QString &)));
      {
        QString s;
        if (!null)
          s.setNum(v.toDouble(), 'f', partDecimal);
        ::qt_cast<FLLineEdit *>(editor_)->setText(s, false);
      }
      connect(editor_, SIGNAL(textChanged(const QString &)), this,
              SLOT(updateValue(const QString &)));
      break;

    case QVariant::String: {
      bool doHome = false;
      if (ol) {
        if (v.toString() == ::qt_cast<QComboBox *>(editor_)->currentText())
          return;
      } else {
        if (v.toString() == ::qt_cast<FLLineEdit *>(editor_)->text())
          return;
        doHome = (::qt_cast<FLLineEdit *>(editor_)->text().isEmpty());
      }
      disconnect(editor_, SIGNAL(textChanged(const QString &)), this,
                 SLOT(updateValue(const QString &)));
      if (v.isValid() && !v.isNull()) {
        if (ol)
          ::qt_cast<QComboBox *>(editor_)->setCurrentItem(
            field->optionsList().findIndex(
              v.toString()));
        else
          ::qt_cast<FLLineEdit *>(editor_)->setText(v.toString(), false);
      } else {
        if (ol)
          ::qt_cast<QComboBox *>(editor_)->setCurrentItem(0);
        else
          ::qt_cast<FLLineEdit *>(editor_)->setText(QString::null, false);
      }
      if (!ol && doHome)
        ::qt_cast<FLLineEdit *>(editor_)->home(false);
      connect(editor_, SIGNAL(textChanged(const QString &)), this,
              SLOT(updateValue(const QString &)));
    }
    break;

    case QVariant::UInt:
      if (v.toUInt() == ::qt_cast<FLLineEdit *>(editor_)->text().toUInt())
        return;
      disconnect(editor_, SIGNAL(textChanged(const QString &)), this,
                 SLOT(updateValue(const QString &)));
      {
        QString s;
        if (!null)
          s.setNum(v.toUInt());
        ::qt_cast<FLLineEdit *>(editor_)->setText(s, false);
      }
      connect(editor_, SIGNAL(textChanged(const QString &)), this,
              SLOT(updateValue(const QString &)));
      break;

    case QVariant::Int:
      if (v.toInt() == ::qt_cast<FLLineEdit *>(editor_)->text().toInt())
        return;
      disconnect(editor_, SIGNAL(textChanged(const QString &)), this,
                 SLOT(updateValue(const QString &)));
      {
        QString s;
        if (!null)
          s.setNum(v.toInt());
        ::qt_cast<FLLineEdit *>(editor_)->setText(s, false);
      }
      connect(editor_, SIGNAL(textChanged(const QString &)), this,
              SLOT(updateValue(const QString &)));
      break;

    case FLFieldMetaData::Serial:
      if (v.toInt() == ::qt_cast<FLSpinBox *>(editor_)->value())
        return;
      disconnect(editor_, SIGNAL(valueChanged(const QString &)), this,
                 SLOT(updateValue(const QString &)));
      ::qt_cast<FLSpinBox *>(editor_)->setValue(v.toInt());
      connect(editor_, SIGNAL(valueChanged(const QString &)), this,
              SLOT(updateValue(const QString &)));
      break;

    case QVariant::Pixmap: {
      if (!editorImg_) {
        editorImg_ = new FLPixmapView(this);
        editorImg_->setFocusPolicy(QWidget::NoFocus);
        editorImg_->setSizePolicy(sizePolicy());
        editorImg_->setMaximumSize(initMaxSize_);
        editorImg_->setMinimumSize(initMinSize_);
        editorImg_->setAutoScaled(true);
        FLWidgetFieldDBLayout->addWidget(editorImg_);
        if (field->visible())
          editorImg_->show();
      }
      QCString cs;
      if (!null)
        cs = v.toCString();
      if (cs.isEmpty()) {
        editorImg_->clear();
        return;
      }
      QPixmap pix;
      if (!QPixmapCache::find(cs.left(100), pix)) {
        pix.loadFromData(cs);
        QPixmapCache::insert(cs.left(100), pix);
      }
      if (pix.isNull())
        editorImg_->clear();
      else
        editorImg_->setPixmap(pix);
    }
    break;

    case QVariant::Date:
      if (v.toDate() == ::qt_cast<FLDateEdit *>(editor_)->date())
        return;
      disconnect(editor_, SIGNAL(valueChanged(const QDate &)), this,
                 SLOT(updateValue(const QDate &)));
      ::qt_cast<FLDateEdit *>(editor_)->setDate(v.toDate());
      connect(editor_, SIGNAL(valueChanged(const QDate &)), this, SLOT(updateValue(const QDate &)));
      break;

    case QVariant::Time:
      if (v.toTime() == ::qt_cast<QTimeEdit *>(editor_)->time())
        return;
      disconnect(editor_, SIGNAL(valueChanged(const QTime &)), this,
                 SLOT(updateValue(const QTime &)));
      ::qt_cast<QTimeEdit *>(editor_)->setTime(v.toTime());
      connect(editor_, SIGNAL(valueChanged(const QTime &)), this, SLOT(updateValue(const QTime &)));
      break;

    case QVariant::StringList:
      if (v.toString() == ::qt_cast<QTextEdit *>(editor_)->text())
        return;
      disconnect(editor_, SIGNAL(textChanged()), this, SLOT(updateValue()));
      ::qt_cast<QTextEdit *>(editor_)->setText(v.toString());
      connect(editor_, SIGNAL(textChanged()), this, SLOT(updateValue()));
      break;

    case QVariant::Bool:
      if (v.toBool() == ::qt_cast<QCheckBox *>(editor_)->isChecked())
        return;
      disconnect(editor_, SIGNAL(toggled(bool)), this, SLOT(updateValue(bool)));
      ::qt_cast<QCheckBox *>(editor_)->setChecked(v.toBool());
      connect(editor_, SIGNAL(toggled(bool)), this, SLOT(updateValue(bool)));
      break;
  }
}

void FLFieldDB::refresh(const QString &fN)
{
  if (!cursor_)
    return;

  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return;

  QVariant v;
  bool null;
  if (fN.isEmpty()) {
    v = cursor_->valueBuffer(fieldName_);
    null = cursor_->bufferIsNull(fieldName_);
  } else {
    if (!cursorAux && fN.lower() == fieldRelation_.lower()) {
      if (cursor_->bufferIsNull(fieldRelation_))
        return;

      FLManager *mng = cursor_->db()->manager();
      FLFieldMetaData *field = tMD->field(fieldRelation_);
      FLTableMetaData *tmd = mng->metadata(field->relationM1()->foreignTable());
      if (!tmd)
        return;
      if (topWidget_ && !topWidget_->isShown() && cursor_->modeAccess() != FLSqlCursor::INSERT) {
        if (tmd && !tmd->inCache())
          delete tmd;
        return;
      }
      if (!field) {
        if (tmd && !tmd->inCache())
          delete tmd;
        return;
      }

      if (!field->relationM1()) {
#ifdef FL_DEBUG
        qWarning("FLFieldDB : " + tr("El campo de la relación debe estar relacionado en M1"));
#endif
        if (tmd && !tmd->inCache())
          delete tmd;
        return;
      }

      QVariant v(cursor_->valueBuffer(fieldRelation_));
      FLSqlQuery q(0, cursor_->db()->connectionName());
      q.setForwardOnly(true);
      q.setTablesList(field->relationM1()->foreignTable());
      q.setSelect(foreignField_ + "," + field->relationM1()->foreignField());
      q.setFrom(field->relationM1()->foreignTable());

      QString where(mng->formatAssignValue(field->relationM1()->foreignField(),
                                           field, v, true));
      QString filterAc(cursor_->filterAssoc(fieldRelation_, tmd));

      if (!filterAc.isEmpty()) {
        if (where.isEmpty())
          where = filterAc;
        else
          where += QString::fromLatin1(" AND ") + filterAc;
      }

      if (filter_.isEmpty())
        q.setWhere(where);
      else
        q.setWhere(filter_ + QString::fromLatin1(" AND ") + where);

      if (q.exec() && q.next()) {
        QVariant v0(q.value(0)), v1(q.value(1));
        if (v0 != value())
          setValue(v0);
        if (v1 != v)
          cursor_->setValueBuffer(fieldRelation_, v1);
      }
      if (tmd && !tmd->inCache())
        delete tmd;
    }
    return;
  }

  FLFieldMetaData *field = tMD->field(fieldName_);
  if (!field)
    return;

  int type = field->type();

  if (type != QVariant::Pixmap && !editor_)
    return;

  int modeAccess = cursor_->modeAccess();
  int partDecimal = partDecimal_ != -1 ? partDecimal_ : field->partDecimal();
  bool ol = field->hasOptionsList();

  bool fDis = (keepDisabled_ || cursor_->fieldDisabled(fieldName_) ||
               (modeAccess == FLSqlCursor::EDIT &&
                (field->isPrimaryKey() || tMD->fieldListOfCompoundKey(fieldName_)))
               || !field->editable() || modeAccess == FLSqlCursor::BROWSE);
  setDisabled(fDis);

  switch (type) {
    case QVariant::Double:
      disconnect(editor_, SIGNAL(textChanged(const QString &)), this,
                 SLOT(updateValue(const QString &)));
      {
        QString s;
        if (!null)
          s.setNum(v.toDouble(), 'f', partDecimal);
        ::qt_cast<FLLineEdit *>(editor_)->setText(s);
      }
      connect(editor_, SIGNAL(textChanged(const QString &)), this,
              SLOT(updateValue(const QString &)));
      break;

    case QVariant::String: {
      bool doHome = false;
      if (!ol)
        doHome = (::qt_cast<FLLineEdit *>(editor_)->text().isEmpty());
      disconnect(editor_, SIGNAL(textChanged(const QString &)), this,
                 SLOT(updateValue(const QString &)));
      if (!null) {
        if (ol)
          ::qt_cast<QComboBox *>(editor_)->setCurrentItem(
            field->optionsList().findIndex(
              v.toString()));
        else
          ::qt_cast<FLLineEdit *>(editor_)->setText(v.toString());
      } else {
        if (ol)
          ::qt_cast<QComboBox *>(editor_)->setCurrentItem(0);
        else
          ::qt_cast<FLLineEdit *>(editor_)->setText(QString::null);
      }
      if (!ol && doHome)
        ::qt_cast<FLLineEdit *>(editor_)->home(false);
      connect(editor_, SIGNAL(textChanged(const QString &)), this,
              SLOT(updateValue(const QString &)));
    }
    break;

    case QVariant::UInt:
      disconnect(editor_, SIGNAL(textChanged(const QString &)), this,
                 SLOT(updateValue(const QString &)));
      {
        QString s;
        if (!null)
          s.setNum(v.toUInt());
        ::qt_cast<FLLineEdit *>(editor_)->setText(s);
      }
      connect(editor_, SIGNAL(textChanged(const QString &)), this,
              SLOT(updateValue(const QString &)));
      break;

    case QVariant::Int:
      disconnect(editor_, SIGNAL(textChanged(const QString &)), this,
                 SLOT(updateValue(const QString &)));
      {
        QString s;
        if (!null)
          s.setNum(v.toInt());
        ::qt_cast<FLLineEdit *>(editor_)->setText(s);
      }
      connect(editor_, SIGNAL(textChanged(const QString &)), this,
              SLOT(updateValue(const QString &)));
      break;

    case FLFieldMetaData::Serial:
      disconnect(editor_, SIGNAL(valueChanged(const QString &)), this,
                 SLOT(updateValue(const QString &)));
      ::qt_cast<FLSpinBox *>(editor_)->setValue(0);
      connect(editor_, SIGNAL(valueChanged(const QString &)), this,
              SLOT(updateValue(const QString &)));
      break;

    case QVariant::Pixmap:
      if (!editorImg_) {
        editorImg_ = new FLPixmapView(this);
        editorImg_->setFocusPolicy(QWidget::NoFocus);
        editorImg_->setSizePolicy(sizePolicy());
        editorImg_->setMaximumSize(initMaxSize_);
        editorImg_->setMinimumSize(initMinSize_);
        editorImg_->setAutoScaled(true);
        FLWidgetFieldDBLayout->addWidget(editorImg_);
        if (field->visible())
          editorImg_->show();
      }
      if (modeAccess == FLSqlCursor::BROWSE)
        setDisabled(false);
      if (field->visible()) {
        QCString cs;
        if (!null)
          cs = v.toCString();
        if (cs.isEmpty()) {
          editorImg_->clear();
          return;
        }
        QPixmap pix;
        if (!QPixmapCache::find(cs.left(100), pix)) {
          pix.loadFromData(cs);
          QPixmapCache::insert(cs.left(100), pix);
        }
        if (!pix.isNull()) {
          editorImg_->setPixmap(pix);
        } else
          editorImg_->clear();
      }
      if (modeAccess == FLSqlCursor::BROWSE)
        pushButtonDB->setDisabled(true);
      break;

    case QVariant::Date:
      if (cursor_->modeAccess() == FLSqlCursor::INSERT && null && !field->allowNull()) {
        QVariant defVal(field->defaultValue());
        if (!defVal.isValid() || defVal.isNull())
          ::qt_cast<FLDateEdit *>(editor_)->setDate(QDate::currentDate());
        else
          ::qt_cast<FLDateEdit *>(editor_)->setDate(defVal.toDate());
      } else {
        disconnect(editor_, SIGNAL(valueChanged(const QDate &)), this,
                   SLOT(updateValue(const QDate &)));
        ::qt_cast<FLDateEdit *>(editor_)->setDate(v.toDate());
        connect(editor_, SIGNAL(valueChanged(const QDate &)), this,
                SLOT(updateValue(const QDate &)));
      }
      break;

    case QVariant::Time:
      if (cursor_->modeAccess() == FLSqlCursor::INSERT && null && !field->allowNull()) {
        QVariant defVal(field->defaultValue());
        if (!defVal.isValid() || defVal.isNull())
          ::qt_cast<QTimeEdit *>(editor_)->setTime(QTime::currentTime());
        else
          ::qt_cast<QTimeEdit *>(editor_)->setTime(defVal.toTime());
      } else {
        disconnect(editor_, SIGNAL(valueChanged(const QTime &)), this,
                   SLOT(updateValue(const QTime &)));
        ::qt_cast<QTimeEdit *>(editor_)->setTime(v.toTime());
        connect(editor_, SIGNAL(valueChanged(const QTime &)), this,
                SLOT(updateValue(const QTime &)));
      }
      break;

    case QVariant::StringList:
      disconnect(editor_, SIGNAL(textChanged()), this, SLOT(updateValue()));
      ::qt_cast<QTextEdit *>(editor_)->setText(v.toString());
      connect(editor_, SIGNAL(textChanged()), this, SLOT(updateValue()));
      break;

    case QVariant::Bool:
      disconnect(editor_, SIGNAL(toggled(bool)), this, SLOT(updateValue(bool)));
      ::qt_cast<QCheckBox *>(editor_)->setChecked(v.toBool());
      connect(editor_, SIGNAL(toggled(bool)), this, SLOT(updateValue(bool)));
      break;
  }

  if (!field->visible()) {
    if (editor_)
      editor_->hide();
    else if (editorImg_)
      editorImg_->hide();
    setDisabled(true);
  }
}

void FLFieldDB::setTableName(const QString &fT)
{
  tableName_ = fT;
  if (showed) {
    if (topWidget_) {
      initCursor();
      initEditor();
    } else
      initFakeEditor();
  }
}

void FLFieldDB::setForeignField(const QString &fN)
{
  foreignField_ = fN;
  if (showed) {
    if (topWidget_) {
      initCursor();
      initEditor();
    } else
      initFakeEditor();
  }
}

void FLFieldDB::setFieldRelation(const QString &fN)
{
  fieldRelation_ = fN;
  if (showed) {
    if (topWidget_) {
      initCursor();
      initEditor();
    } else
      initFakeEditor();
  }
}

void FLFieldDB::setFieldAlias(const QString &alias)
{
  if (!alias.isEmpty()) {
    fieldAlias_ = alias;
    if (showAlias_)
      textLabelDB->setText(fieldAlias_);
  }
}

void FLFieldDB::setFieldName(const QString &fN)
{
  fieldName_ = fN;
  if (showed) {
    if (topWidget_) {
      initCursor();
      initEditor();
    } else
      initFakeEditor();
  }
}

void FLFieldDB::setActionName(const QString &aN)
{
  actionName_ = aN;
  if (showed && !topWidget_)
    initFakeEditor();
}

FLDoubleValidator::FLDoubleValidator(QObject *parent, const char *name) :
  QDoubleValidator(parent, name)
{
}

FLDoubleValidator::FLDoubleValidator(double bottom, double top, int decimals, QObject *parent,
                                     const char *name) :
  QDoubleValidator(bottom, top, decimals, parent, name)
{
}

QValidator::State FLDoubleValidator::validate(QString &input, int &i) const
{
  if (input.isEmpty())
    return QValidator::Acceptable;
  input.replace(",", ".");

  QValidator::State state = QDoubleValidator::validate(input, i);

  if (state == QValidator::Invalid || state == QValidator::Intermediate) {
    QString s(input.right(input.length() - 1));
    if (input.left(1) == "-" &&
        (QDoubleValidator::validate(s, i) == QValidator::Acceptable || s.isEmpty())) {
      state = QValidator::Acceptable;
    } else
      state =  QValidator::Invalid;
  } else
    state = QValidator::Acceptable;

  if (aqApp->commaSeparator() == ',')
    input.replace(".", ",");
  else
    input.replace(",", ".");

  return state;
}

FLIntValidator::FLIntValidator(int minimum, int maximum, QObject *parent, const char *name) :
  QIntValidator(minimum, maximum, parent, name)
{
}

QValidator::State FLIntValidator::validate(QString &input, int &i) const
{
  if (input.isEmpty())
    return QValidator::Acceptable;
  QValidator::State state = QIntValidator::validate(input, i);
  if (state == QValidator::Invalid || state == QValidator::Intermediate) {
    QString s = input.right(input.length() - 1);

    if (input.left(1) == "-" && (QIntValidator::validate(s, i) == QValidator::Acceptable
                                 || s.isEmpty()))
      return QValidator::Acceptable;
    return QValidator::Invalid;
  } else
    return QValidator::Acceptable;
}

FLUIntValidator::FLUIntValidator(int minimum, int maximum, QObject *parent, const char *name) :
  QIntValidator(minimum, maximum, parent, name)
{
}

QValidator::State FLUIntValidator::validate(QString &input, int &i) const
{
  if (input.isEmpty())
    return QValidator::Acceptable;
  QIntValidator iV(0, 1000000000, 0);
  QValidator::State state = iV.validate(input, i);
  if (state == QValidator::Intermediate)
    state = QValidator::Invalid;
  return state;
}

void FLFieldDB::emitLostFocus()
{
  emit lostFocus();
}

void FLFieldDB::showEvent(QShowEvent *e)
{
  showWidget();
  QWidget::showEvent(e);
}

void FLFieldDB::showWidget()
{
  if (!showed) {
    if (topWidget_) {
      refresh();
      if (cursorAux && cursor_ && cursor_->bufferIsNull(fieldName_)) {
        if (!cursorAux->bufferIsNull(foreignField_)) {
          FLManager *mng = cursor_->db()->manager();
          FLTableMetaData *tMD = mng->metadata(tableName_);
          if (tMD) {
            QVariant v(cursorAux->valueBuffer(foreignField_));

            FLSqlQuery q(0, cursor_->db()->connectionName());
            q.setForwardOnly(true);
            q.setTablesList(tableName_);
            q.setSelect(fieldName_);
            q.setFrom(tableName_);

            QString where(mng->formatAssignValue(tMD->field(fieldRelation_), v, true));
            QString filterAc(cursorAux->filterAssoc(foreignField_, tMD));

            if (!filterAc.isEmpty()) {
              if (where.isEmpty())
                where = filterAc;
              else
                where += QString::fromLatin1(" AND ") + filterAc;
            }

            if (filter_.isEmpty())
              q.setWhere(where);
            else
              q.setWhere(filter_ + QString::fromLatin1(" AND ") + where);
            if (q.exec() && q.next())
              setValue(q.value(0));
            if (!tMD->inCache())
              delete tMD;
          }
        }
      }
    } else
      initFakeEditor();
    showed = true;
  }
}

void FLFieldDB::setNoShowed()
{
  if (!foreignField_.isEmpty() && !fieldRelation_.isEmpty()) {
    showed = false;
    if (isVisible())
      showWidget();
  }
}

FLSqlCursor *FLFieldDB::cursor()
{
  return cursor_;
}

bool FLFieldDB::showAlias() const
{
  return showAlias_;
}

void FLFieldDB::setShowAlias(const bool value)
{
  if (showAlias_ != value) {
    showAlias_ = value;
    if (showAlias_)
      textLabelDB->show();
    else
      textLabelDB->hide();
  }
}

void FLFieldDB::setMapValue(const QString &v)
{
  fieldMapValue_ = ::qt_cast<FLFieldDB *>(sender());
  mapValue_ = v;
  setMapValue();
}

void FLFieldDB::setMapValue()
{
  if (!fieldMapValue_ || !cursor_)
    return;
  FLTableMetaData *tMD = cursor_->metadata();
  if (!tMD)
    return;

  QString fSN = fieldMapValue_->fieldName();
  FLFieldMetaData *field = tMD->field(fieldName_);
  FLFieldMetaData *fieldSender = tMD->field(fSN);

  if (!field || !fieldSender)
    return;

  if (field->relationM1()) {
    if (field->relationM1()->foreignTable() != tMD->name()) {
      FLManager *mng = cursor_->db()->manager();
      QString rt(field->relationM1()->foreignTable());
      QString fF(fieldMapValue_->foreignField());
      FLSqlQuery q(0, cursor_->db()->connectionName());
      q.setForwardOnly(true);
      q.setTablesList(rt);
      q.setSelect(field->relationM1()->foreignField() + "," + fF);
      q.setFrom(rt);

      QString where(mng->formatAssignValue(fF, fieldSender, mapValue_, true));
      FLTableMetaData *assocTmd = mng->metadata(rt);
      QString filterAc(cursor_->filterAssoc(fF, assocTmd));
      if (assocTmd && !assocTmd->inCache())
        delete assocTmd;

      if (!filterAc.isEmpty()) {
        if (where.isEmpty())
          where = filterAc;
        else
          where += QString::fromLatin1(" AND ") + filterAc;
      }

      if (filter_.isEmpty())
        q.setWhere(where);
      else
        q.setWhere(filter_ + QString::fromLatin1(" AND ") + where);

      if (q.exec() && q.next()) {
        setValue(QVariant());
        setValue(q.value(0));
      }
    }
  }
}

void FLFieldDB::emitKeyF2Pressed()
{
  emit keyF2Pressed();
}

void FLFieldDB::emitLabelClicked()
{
  emit labelClicked();
}

void FLFieldDB::emitTextChanged(const QString &t)
{
  emit textChanged(t);
}

void FLFieldDB::toggleDatePicker()
{
  if (!dateFrame_) {
    dateFrame_ = new QVBox(this, "dateFrame", WType_Popup);
    dateFrame_->setFrameStyle(QFrame::PopupPanel | QFrame::Raised);
    dateFrame_->setFixedSize(200, 200);
    dateFrame_->setLineWidth(3);
    dateFrame_->hide();
    if (!datePopup_) {
      datePopup_ = new VDatePopup(dateFrame_, QDate::currentDate());
      connect(datePopup_, SIGNAL(dateSelected(const QDate &)), ::qt_cast<FLDateEdit *>(editor_),
              SLOT(setDate(const QDate &)));
    }
  }

  if (!datePickerOn_) {
    QPoint tmpPoint = mapToGlobal(pbAux_->geometry().bottomRight());
    dateFrame_->setGeometry(tmpPoint.x() - 207, tmpPoint.y(), 200, 200);
    QDate date = ::qt_cast<FLDateEdit *>(editor_)->date();
    if (date.isValid()) {
      datePopup_->setDate(date);
    } else {
      datePopup_->setDate(QDate::currentDate());
    }
    datePickerOn_ = true;
    dateFrame_->show();
  }

  if (!dateFrame_->isVisible())
    datePickerOn_ = false;
}

void FLFieldDB::clearPixmap()
{
  if (editorImg_) {
    editorImg_->clear();
    cursor_->setValueBuffer(fieldName_, QVariant());
  }
}

void FLFieldDB::savePixmap(int f)
{
  if (editorImg_) {
    QStrList fmtl = QImage::outputFormats();
    const char *fmt = fmtl.at(f);
    QString ext = QString(fmt).lower();
    QString filename = "imagen." + ext;
    QString savefilename = QFileDialog::getSaveFileName(filename.lower(), "*." + ext, this,
                                                        filename, tr("Guardar imagen como"));
    if (!savefilename.isEmpty()) {
      QPixmap pix;
      QApplication::setOverrideCursor(waitCursor);
      pix.loadFromData(value().toCString());
      if (!pix.isNull())
        if (!pix.save(savefilename, fmt))
          QMessageBox::warning(this, tr("Error"), tr("Error guardando fichero"));
      QApplication::restoreOverrideCursor();
    }
  }
}

void FLFieldDB::toggleAutoCompletion()
{
  if (autoCompMode_ == NeverAuto)
    return;

  if (!autoComFrame_ && cursor_) {
    autoComFrame_ = new QVBox(this, "autoComFrame", WType_Popup);
    autoComFrame_->setFrameStyle(QFrame::PopupPanel | QFrame::Raised);
    autoComFrame_->setLineWidth(1);
    autoComFrame_->hide();

    if (!autoComPopup_) {
      FLTableMetaData *tMD = cursor_->metadata();
      FLFieldMetaData *field = tMD ? tMD->field(fieldName_) : 0;

      if (field) {
        autoComPopup_ = new FLDataTable(autoComFrame_, "autoComPopup", true);
        FLSqlCursor *cur;

        if (!field->relationM1()) {
          if (!fieldRelation_.isEmpty() && !foreignField_.isEmpty()) {
            autoComFieldName_ = foreignField_;

            FLFieldMetaData *fRel = tMD ? tMD->field(fieldRelation_) : 0;
            if (!fRel) {
              return;
            }
            autoComFieldRelation_ = fRel->relationM1()->foreignField();
            cur = new FLSqlCursor(fRel->relationM1()->foreignTable(), false,
                                  cursor_->db()->connectionName(), 0, 0, autoComFrame_);
            tMD = cur->metadata();
            field = tMD ? tMD->field(autoComFieldName_) : field;
          } else {
            autoComFieldName_ = fieldName_;
            autoComFieldRelation_ = QString::null;
            cur = new FLSqlCursor(tMD->name(), false, cursor_->db()->connectionName(), 0, 0,
                                  autoComFrame_);
          }
        } else {
          autoComFieldName_ = field->relationM1()->foreignField();
          autoComFieldRelation_ = QString::null;
          cur = new FLSqlCursor(field->relationM1()->foreignTable(), false,
                                cursor_->db()->connectionName(), 0, 0, autoComFrame_);
          tMD = cur->metadata();
          field = tMD ? tMD->field(autoComFieldName_) : field;
        }

        cur->append(QSqlFieldInfo(autoComFieldName_, FLFieldMetaData::flDecodeType(field->type()),
                                  -1, field->length(), -1));

        QStringList fieldsNames = QStringList::split(',', tMD->fieldsNames());
        for (QStringList::Iterator it = fieldsNames.begin(); it != fieldsNames.end(); ++it) {
          if (!cur->QSqlCursor::field((*it))) {
            field = tMD->field(*it);
            if (field)
              cur->append(QSqlFieldInfo(field->name(),
                                        FLFieldMetaData::flDecodeType(field->type()), -1,
                                        field->length(), -1, QVariant(), 0, true));
          }
        }

        if (!autoComFieldRelation_.isEmpty() && topWidget_) {
          QObjectList *l = static_cast<QObject *>(topWidget_)->queryList("FLFieldDB");
          QObjectListIt itf(*l);
          FLFieldDB *fdb = 0;
          while ((fdb = static_cast<FLFieldDB *>(itf.current())) != 0) {
            ++itf;
            if (fdb->fieldName() == autoComFieldRelation_)
              break;
          }
          delete l;

          if (fdb && !fdb->filter().isEmpty())
            cur->setMainFilter(fdb->filter());
        }
        autoComPopup_->setFLSqlCursor(cur);
        autoComPopup_->setTopMargin(0);
        autoComPopup_->setLeftMargin(0);
        autoComPopup_->horizontalHeader()->hide();
        autoComPopup_->verticalHeader()->hide();

        connect(cur, SIGNAL(newBuffer()), this, SLOT(autoCompletionUpdateValue()));
        connect(autoComPopup_, SIGNAL(recordChoosed()), this, SLOT(autoCompletionUpdateValue()));
      }
    }
  }

  if (autoComPopup_) {
    FLSqlCursor *cur = autoComPopup_->cursor();
    FLTableMetaData *tMD = cur->metadata();
    FLFieldMetaData *field = tMD ? tMD->field(autoComFieldName_) : 0;

    if (field) {
      QString filter(cursor_->db()->manager()->formatAssignValueLike(field, value(), true));
      cur->setFilter(filter);
      autoComPopup_->setFilter(filter);
      autoComPopup_->setSort(QStringList() << autoComFieldName_ + " ASC");
      autoComPopup_->QDataTable::refresh();
    }

    if (!autoComFrame_->isVisible() && cur->size() > 1) {
      QPoint tmpPoint;
      if (showAlias_)
        tmpPoint = mapToGlobal(textLabelDB->geometry().bottomLeft());
      else if (pushButtonDB && pushButtonDB->isShown())
        tmpPoint = mapToGlobal(pushButtonDB->geometry().bottomLeft());
      else
        tmpPoint = mapToGlobal(editor_->geometry().bottomLeft());
      int frameWidth = width();
      if (frameWidth < autoComPopup_->width())
        frameWidth = autoComPopup_->width();
      if (frameWidth < autoComFrame_->width())
        frameWidth = autoComFrame_->width();
      autoComFrame_->setGeometry(tmpPoint.x(), tmpPoint.y(), frameWidth, 300);
      autoComFrame_->show();
      autoComFrame_->setFocus();
    } else if (autoComFrame_->isVisible() && cur->size() == 1)
      autoComFrame_->hide();

    cur->first();
  }
}

void FLFieldDB::autoCompletionUpdateValue()
{
  if (!autoComPopup_ || !autoComFrame_)
    return;

  FLSqlCursor *cur = autoComPopup_->cursor();
  if (!cur || !cur->isValid())
    return;

  if (::qt_cast<FLDataTable *>(sender())) {
    setValue(cur->valueBuffer(autoComFieldName_));
    autoComFrame_->hide();
#ifdef Q_OS_WIN32
    if (editor_)
      editor_->releaseKeyboard();
    if (autoComPopup_)
      autoComPopup_->releaseKeyboard();
#endif
  } else if (::qt_cast<QTextEdit *>(editor_)) {
    setValue(cur->valueBuffer(autoComFieldName_));
  } else {
    FLLineEdit *ed = ::qt_cast<FLLineEdit *>(editor_);
    if (autoComFrame_->isVisible() && !ed->hasFocus()) {
      if (!autoComPopup_->hasFocus()) {
        QString cval(cur->valueBuffer(autoComFieldName_).toString());
        QString val(ed->text());
        ed->autoSelect = false;
        ed->setText(cval);
        ed->QLineEdit::setFocus();
        ed->setCursorPosition(cval.length());
        ed->cursorBackward(true, cval.length() - val.length());
#ifdef Q_OS_WIN32
        ed->grabKeyboard();
#endif
      } else {
        setValue(cur->valueBuffer(autoComFieldName_));
      }
    } else if (!autoComFrame_->isVisible()) {
      QString cval(cur->valueBuffer(autoComFieldName_).toString());
      QString val(ed->text());
      ed->autoSelect = false;
      ed->setText(cval);
      ed->QLineEdit::setFocus();
      ed->setCursorPosition(cval.length());
      ed->cursorBackward(true, cval.length() - val.length());
    }
  }
  if (!autoComFieldRelation_.isEmpty() && !autoComFrame_->isVisible()) {
    cursor_->setValueBuffer(fieldRelation_, cur->valueBuffer(autoComFieldRelation_));
  }
}

int FLFieldDB::insertAccel(const QString &key)
{
  if (!accel_) {
    accel_ = new QAccel(editor_);
    connect(accel_, SIGNAL(activated(int)), this, SLOT(emitActivatedAccel(int)));
  }

  int id = accel_->findKey(QKeySequence(key));

  if (id != -1)
    return id;

  id = accel_->insertItem(QKeySequence(key));
  return id;
}

void FLFieldDB::removeAccel(int id)
{
  if (!accel_)
    return;
  accel_->removeItem(id);
}

void FLFieldDB::emitActivatedAccel(int id)
{
  if (editor_ && editor_->hasFocus()) {
    emit activatedAccel(id);
  }
}

void FLFieldDB::setKeepDisabled(const bool keep)
{
  keepDisabled_ = keep;
}

bool FLFieldDB::showEditor() const
{
  return showEditor_;
}

void FLFieldDB::setShowEditor(const bool show)
{
   	showEditor_ = show;
    	QWidget *ed = 0;
	if (editor_)
    		ed = editor_;
    	else if (editorImg_)
		ed = editorImg_;
		
    	if (ed) 
    		if (show)
			ed->show();
     		else
			ed->hide();	
}

void FLFieldDB::initFakeEditor()
{
  bool hasPushButtonDB = (tableName_.isEmpty() && foreignField_.isEmpty()
                          && fieldRelation_.isEmpty());
  fieldAlias_ = (fieldName_.isEmpty() ? tr("Error: fieldName vacio") : fieldName_);

  if (!editor_) {
    editor_ = new QLineEdit(this);
    editor_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    textLabelDB->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    FLWidgetFieldDBLayout->addWidget(editor_);
    editor_->setFocusPolicy(QWidget::StrongFocus);
    setFocusProxy(editor_);
    setTabOrder(pushButtonDB, editor_);
    editor_->show();
  }

  textLabelDB->setText(fieldAlias_);
  if (showAlias_)
    textLabelDB->show();
  else
    textLabelDB->hide();

  if (hasPushButtonDB) {
    pushButtonDB->setFocusPolicy(QWidget::NoFocus);
    pushButtonDB->show();
  } else
    pushButtonDB->hide();

  QString prty;
  if (!tableName_.isEmpty())
    prty += "tN:" + tableName_ + ",";
  if (!foreignField_.isEmpty())
    prty += "fF:" + foreignField_ + ",";
  if (!fieldRelation_.isEmpty())
    prty += "fR:" + fieldRelation_ + ",";
  if (!actionName_.isEmpty())
    prty += "aN:" + actionName_ + ",";
  if (!prty.isEmpty()) {
    ::qt_cast<QLineEdit *>(editor_)->setText(prty);
    ::qt_cast<QLineEdit *>(editor_)->home(false);
  }

  if (maximumSize().width() < 80)
    setShowEditor(false);
  else
    setShowEditor(showEditor_);
}

void FLFieldDB::setEnabled(bool enable)
{
  if (enable)
    clearWState(WState_ForceDisabled);
  else
    setWState(WState_ForceDisabled);

  if (!isTopLevel() && parentWidget() &&
      !parentWidget()->isEnabled() && enable)
    return; // nothing we can do

  if (enable) {
    if (testWState(WState_Disabled)) {
      clearWState(WState_Disabled);
      enabledChange(!enable);
      if (children()) {
        QObjectListIt it(*children());
        QWidget *w;
        while ((w = (QWidget *)it.current()) != 0) {
          ++it;
          if (w->isWidgetType() &&
              !w->testWState(WState_ForceDisabled)) {
            QLineEdit *le = ::qt_cast<QLineEdit *>(w);
            if (le) {
              bool allowNull = true;
              FLTableMetaData *tMD = cursor_->metadata();
              if (tMD) {
                FLFieldMetaData *field = tMD->field(fieldName_);
                if (field && !field->allowNull())
                  allowNull = false;
              }
//               QColor cFg = qApp->palette().color(QPalette::Active, QColorGroup::Text);
              QColor cBg;
              if (allowNull)
                cBg = qApp->palette().color(QPalette::Active, QColorGroup::Base);
              else
                cBg = notNullColor();

              le->setDisabled(false);
              le->setReadOnly(false);
              le->setCursor(QCursor::ibeamCursor);
//               le->setPaletteForegroundColor(cFg);
              le->setPaletteBackgroundColor(cBg);
              le->setFocusPolicy(QWidget::StrongFocus);
              continue;
            }

            QTextEdit *te = ::qt_cast<QTextEdit *>(w);
            if (te) {
//               QColor cFg = qApp->palette().color(QPalette::Active, QColorGroup::Text);
              QColor cBg = qApp->palette().color(QPalette::Active, QColorGroup::Base);

              te->setDisabled(false);
              te->setReadOnly(false);
              te->viewport()->setCursor(QCursor::ibeamCursor);
//               te->setPaletteForegroundColor(cFg);
              te->setPaletteBackgroundColor(cBg);
              te->setFocusPolicy(QWidget::WheelFocus);
              continue;
            }

            w->setEnabled(true);
          }
        }
      }
    }
  } else {
    if (!testWState(WState_Disabled)) {
      if (focusWidget() == this) {
        bool parentIsEnabled = (!parentWidget() || parentWidget()->isEnabled());
        if (!parentIsEnabled || !focusNextPrevChild(TRUE))
          clearFocus();
      }
      setWState(WState_Disabled);
      enabledChange(!enable);

      if (children()) {
        QObjectListIt it(*children());
        QWidget *w;
        while ((w = (QWidget *)it.current()) != 0) {
          ++it;
          if (w->isWidgetType() && w->isEnabled()) {
            QLineEdit *le = ::qt_cast<QLineEdit *>(w);
            if (le) {
//               QColor cFg = qApp->palette().color(QPalette::Disabled, QColorGroup::Text);
              QColor cBg = qApp->palette().color(QPalette::Disabled, QColorGroup::Background);

              le->setDisabled(false);
              le->setReadOnly(true);
              le->setCursor(QCursor::ibeamCursor);
//               le->setPaletteForegroundColor(cFg);
              le->setPaletteBackgroundColor(cBg);
              le->setFocusPolicy(QWidget::NoFocus);
              continue;
            }

            QTextEdit *te = ::qt_cast<QTextEdit *>(w);
            if (te) {
//               QColor cFg = qApp->palette().color(QPalette::Disabled, QColorGroup::Text);
              QColor cBg = qApp->palette().color(QPalette::Disabled, QColorGroup::Background);

              te->setDisabled(false);
              te->setReadOnly(true);
              te->viewport()->setCursor(QCursor::ibeamCursor);
//               te->setPaletteForegroundColor(cFg);
              te->setPaletteBackgroundColor(cBg);
              te->setFocusPolicy(QWidget::NoFocus);
              continue;
            }

            if (w == textLabelDB && pushButtonDB) {
              w->setDisabled(false);
              continue;
            }

            w->setEnabled(false);
            w->clearWState(WState_ForceDisabled);
          }
        }
      }
    }
  }
#if defined(Q_WS_X11)
  if (testWState(WState_OwnCursor)) {
    // enforce the windows behavior of clearing the cursor on
    // disabled widgets
    extern void qt_x11_enforce_cursor(QWidget * w);   // defined in qwidget_x11.cpp
    qt_x11_enforce_cursor(this);
  }
#endif
}

AQTextEditBar::AQTextEditBar(QWidget *parent, const char *name, QLabel *lb) :
  QWidget(parent, name), ted_(0)
{
  layout_ = new QHBoxLayout(this);

  if (lb && !lb->isHidden()) {
    lb_ = new QLabel(lb->text(), this, "lb_");
    layout_->addWidget(lb_);
  }

  layout_->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding,
                                   QSizePolicy::Minimum));

  pbBold_ = new QPushButton(this, "pbBold_");
  pbBold_->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed,
                                     pbBold_->sizePolicy().hasHeightForWidth()));
  pbBold_->setMinimumSize(QSize(22, 22));
  pbBold_->setFocusPolicy(QPushButton::NoFocus);
  pbBold_->setIconSet(QIconSet(QPixmap::fromMimeSource("font-bold.png")));
  pbBold_->setText(QString::null);
  QToolTip::add(pbBold_, tr("Negrita"));
  QWhatsThis::add(pbBold_, tr("Negrita"));
  pbBold_->setToggleButton(true);
  connect(pbBold_, SIGNAL(clicked()), this, SLOT(textBold()));
  layout_->addWidget(pbBold_);

  pbItal_ = new QPushButton(this, "pbItal_");
  pbItal_->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed,
                                     pbItal_->sizePolicy().hasHeightForWidth()));
  pbItal_->setMinimumSize(QSize(22, 22));
  pbItal_->setFocusPolicy(QPushButton::NoFocus);
  pbItal_->setIconSet(QIconSet(QPixmap::fromMimeSource("font-italic.png")));
  pbItal_->setText(QString::null);
  QToolTip::add(pbItal_, tr("Cursiva"));
  QWhatsThis::add(pbItal_, tr("Cursiva"));
  pbItal_->setToggleButton(true);
  connect(pbItal_, SIGNAL(clicked()), this, SLOT(textItalic()));
  layout_->addWidget(pbItal_);

  pbUnde_ = new QPushButton(this, "pbUnde_");
  pbUnde_->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed,
                                     pbUnde_->sizePolicy().hasHeightForWidth()));
  pbUnde_->setMinimumSize(QSize(22, 22));
  pbUnde_->setFocusPolicy(QPushButton::NoFocus);
  pbUnde_->setIconSet(QIconSet(QPixmap::fromMimeSource("font-underline.png")));
  pbUnde_->setText(QString::null);
  QToolTip::add(pbUnde_, tr("Subrayado"));
  QWhatsThis::add(pbUnde_, tr("Subrayado"));
  pbUnde_->setToggleButton(true);
  connect(pbUnde_, SIGNAL(clicked()), this, SLOT(textUnderline()));
  layout_->addWidget(pbUnde_);

  pbColor_ = new QPushButton(this, "pbColor_");
  pbColor_->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed,
                                      pbColor_->sizePolicy().hasHeightForWidth()));
  pbColor_->setMinimumSize(QSize(22, 22));
  pbColor_->setFocusPolicy(QPushButton::NoFocus);
  QPixmap pix(16, 16);
  pix.fill(black);
  pbColor_->setIconSet(pix);
  pbColor_->setText(QString::null);
  QToolTip::add(pbColor_, tr("Color"));
  QWhatsThis::add(pbColor_, tr("Color"));
  connect(pbColor_, SIGNAL(clicked()), this, SLOT(textColor()));
  layout_->addWidget(pbColor_);

  comboFont_ = new QComboBox(true, this, "comboFont_");
  QFontDatabase db;
  comboFont_->insertStringList(db.families());
  comboFont_->lineEdit()->setText(QApplication::font().family());
  connect(comboFont_, SIGNAL(activated(const QString &)),
          this, SLOT(textFamily(const QString &)));
  layout_->addWidget(comboFont_);

  comboSize_ = new QComboBox(true, this, "comboSize_");
  QValueList<int> sizes = db.standardSizes();
  QValueList<int>::Iterator it = sizes.begin();
  for (; it != sizes.end(); ++it)
    comboSize_->insertItem(QString::number(*it));
  connect(comboSize_, SIGNAL(activated(const QString &)),
          this, SLOT(textSize(const QString &)));
  comboSize_->lineEdit()->setText(QString::number(QApplication::font().pointSize()));
  layout_->addWidget(comboSize_);
}

void AQTextEditBar::doConnections(QTextEdit *e)
{
  ted_ = e;
  if (!ted_)
    return;
  connect(e, SIGNAL(currentFontChanged(const QFont &)),
          this, SLOT(fontChanged(const QFont &)));
  connect(e, SIGNAL(currentColorChanged(const QColor &)),
          this, SLOT(colorChanged(const QColor &)));
}

void AQTextEditBar::textBold()
{
  if (!ted_)
    return;
  ted_->setBold(pbBold_->isOn());
}

void AQTextEditBar::textItalic()
{
  if (!ted_)
    return;
  ted_->setItalic(pbItal_->isOn());
}

void AQTextEditBar::textUnderline()
{
  if (!ted_)
    return;
  ted_->setUnderline(pbUnde_->isOn());
}

void AQTextEditBar::textColor()
{
  if (!ted_)
    return;
  QColor col = QColorDialog::getColor(ted_->color(), this);
  if (!col.isValid())
    return;
  ted_->setColor(col);
  QPixmap pix(16, 16);
  pix.fill(col);
  pbColor_->setIconSet(pix);
}

void AQTextEditBar::textFamily(const QString &f)
{
  if (!ted_)
    return;
  ted_->setFamily(f);
  ted_->viewport()->setFocus();
}

void AQTextEditBar::textSize(const QString &p)
{
  if (!ted_)
    return;
  ted_->setPointSize(p.toInt());
  ted_->viewport()->setFocus();
}

void AQTextEditBar::fontChanged(const QFont &f)
{
  pbBold_->setOn(f.bold());
  pbItal_->setOn(f.italic());
  pbUnde_->setOn(f.underline());
  comboFont_->lineEdit()->setText(f.family());
  comboSize_->lineEdit()->setText(QString::number(f.pointSize()));
}

void AQTextEditBar::colorChanged(const QColor &c)
{
  QPixmap pix(16, 16);
  pix.fill(c);
  pbColor_->setIconSet(pix);
}
