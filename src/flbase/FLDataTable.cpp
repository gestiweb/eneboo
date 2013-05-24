/***************************************************************************
                           FLDataTable.cpp
                         -------------------
begin                : Sun Jul 1 2001
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

#include "FLDataTable.h"
#include "FLSqlCursor.h"
#include "FLTableMetaData.h"
#include "FLFieldMetaData.h"
#include "FLUtil.h"
#include "FLApplication.h"
#include "FLSqlQuery.h"
#include "FLSqlDatabase.h"
#include "FLManager.h"
#include "FLObjectFactory.h"

void FLCheckBox::drawButton(QPainter *p)
{
  QRect rect, wrect(this->rect());
  rect.setRect((wrect.width() - 13) / 2, (wrect.height() - 13) / 2, 13, 13);

  if (state() == QButton::On) {
    QBrush bu(green, SolidPattern);
    p->fillRect(0, 0, wrect.width() - 1, wrect.height() - 1, bu);
  }

  QRect irect = QStyle::visualRect(rect, this);
  p->fillRect(irect, Qt::white);
  p->drawRect(irect);

  if (state() == QButton::On) {
    QPointArray a(7 * 2);
    int i, xx, yy;
    xx = irect.x() + 3;
    yy = irect.y() + 5;

    for (i = 0; i < 3; i++) {
      a.setPoint(2 * i,   xx, yy);
      a.setPoint(2 * i + 1, xx, yy + 2);
      xx++;
      yy++;
    }

    yy -= 2;
    for (i = 3; i < 7; i++) {
      a.setPoint(2 * i,   xx, yy);
      a.setPoint(2 * i + 1, xx, yy + 2);
      xx++;
      yy--;
    }

    p->drawLineSegments(a);
  }

  drawButtonLabel(p);
}

bool FLCheckBox::hitButton(const QPoint &pos) const
{
  return this->rect().contains(pos);
}

FLDataTable::FLDataTable(QWidget *parent, const char *name, bool popup)
  : QDataTable(parent, name),
    rowSelected(-1), colSelected(-1), cursor_(0), readonly_(false),
    editonly_(false), insertonly_(false), persistentFilter_(QString::null),
    refreshing_(false), refresh_timer_(false), popup_(popup), showAllPixmaps_(false)
{
  if (!name)
    setName("FLDataTable");
  bu_.setColor(qApp->palette().color(QPalette::Active, QColorGroup::Midlight));
  bu_.setStyle(SolidPattern);
  ok = QPixmap::fromMimeSource("unlock.png");
  no = QPixmap::fromMimeSource("lock.png");
}

FLDataTable::~FLDataTable() {}

void FLDataTable::selectRow(int r, int c)
{
  if (!cursor_ || !cursor_->metadata())
    return ;

  if (r < 0) {
    if (cursor_->isValid()) {
      rowSelected = cursor_->at();
      colSelected = currentColumn();
    } else {
      rowSelected = 0;
      colSelected = 0;
    }
  } else {
    rowSelected = r;
    colSelected = c;
  }

  QObject *snd = const_cast<QObject *>(sender());
  if (!snd || (snd && !snd ->isA("FLSqlCursor"))) {
    QWidget *sndw = ::qt_cast<QWidget *>(snd);
    if (sndw) {
      if (!sndw->hasFocus() || !sndw->isVisible()) {
        setCurrentCell(rowSelected, colSelected);
        return ;
      }
    }
    if (numRows() != cursor_->size())
      setNumRows(cursor_->size());
    cursor_->seek(rowSelected);
  }
  setCurrentCell(rowSelected, colSelected);
}

void FLDataTable::setFLSqlCursor(FLSqlCursor *c)
{
  if (c && c->metadata()) {
    if (!cursor_) {
      disconnect(this, SIGNAL(currentChanged(int, int)), this, SLOT(selectRow(int, int)));
      disconnect(this, SIGNAL(clicked(int, int, int, const QPoint &)), this, SLOT(selectRow(int, int)));
      connect(this, SIGNAL(currentChanged(int, int)), this, SLOT(selectRow(int, int)));
      connect(this, SIGNAL(clicked(int, int, int, const QPoint &)), this, SLOT(selectRow(int, int)));
    } else {
      disconnect(cursor_, SIGNAL(currentChanged(int)), this, SLOT(selectRow(int)));
      if (!popup_)
        disconnect(cursor_, SIGNAL(cursorUpdated()), this, SLOT(refresh()));
    }

    cursor_ = c;
    if (cursor_) {
      disconnect(cursor_, SIGNAL(currentChanged(int)), this, SLOT(selectRow(int)));
      if (!popup_)
        disconnect(cursor_, SIGNAL(cursorUpdated()), this, SLOT(refresh()));
      connect(cursor_, SIGNAL(currentChanged(int)), this, SLOT(selectRow(int)));
      if (!popup_)
        connect(cursor_, SIGNAL(cursorUpdated()), this, SLOT(refresh()));
    }

    QDataTable::setSqlCursor(static_cast<QSqlCursor *>(c), true, false);
  }
}

void FLDataTable::paintCell(QPainter *p, int row, int col, const QRect &cr,
                            bool selected, const QColorGroup &cg)
{
  QTable::paintCell(p, row, col, cr, selected, cg);

  FLTableMetaData *tMD;
  if (!cursor_ || !(tMD = cursor_->metadata()))
    return ;

  QSqlField *field = cursor_->field(indexOf(col));
  QString fName(field->name());
  int type = tMD->fieldType(fName);
  if (!showAllPixmaps_ && type == QVariant::Pixmap && row != rowSelected)
    return ;

  p->setPen(selected ? cg.highlightedText() : cg.text());

  lastTextPainted = QString::null;
  if (cursor_->QSqlCursor::seek(row)) {
    if (!selected && (row % 2))
      p->fillRect(0, 0, cr.width() - 1, cr.height() - 1, bu_);
    paintField(p, field, cr, selected);
  } else
    return;

  cursor_->QSqlCursor::seek(rowSelected);

  if (widthCols_.contains(fName)) {
    int wH = widthCols_[ fName ];
    if (wH > 0 && wH != columnWidth(col))
      QTable::setColumnWidth(col, wH);
    if (wH > 0)
      return;
  }

  int wC = columnWidth(col);
  int wH = fontMetrics().width(tMD->fieldNameToAlias(fName) +
                               QString::fromLatin1("W"));
  if (wH < wC)
    wH = wC;
  wC = fontMetrics().width(lastTextPainted) + fontMetrics().maxWidth();
  if (wC > wH) {
    QTable::setColumnWidth(col, wC);
#if defined(Q_OS_MACX) || defined(Q_OS_WIN32)
    QTable::repaintContents();
#endif
  }
}

static inline Qt::BrushStyle nametoBrushStyle(const QString &style)
{
  if (style.isEmpty() || style == "SolidPattern")
    return Qt::SolidPattern;
  if (style == "NoBrush")
    return Qt::NoBrush;
  if (style == "Dense1Pattern")
    return Qt::Dense1Pattern;
  if (style == "Dense2Pattern")
    return Qt::Dense2Pattern;
  if (style == "Dense3Pattern")
    return Qt::Dense3Pattern;
  if (style == "Dense4Pattern")
    return Qt::Dense4Pattern;
  if (style == "Dense5Pattern")
    return Qt::Dense5Pattern;
  if (style == "Dense6Pattern")
    return Qt::Dense6Pattern;
  if (style == "Dense7Pattern")
    return Qt::Dense7Pattern;
  if (style == "HorPattern")
    return Qt::HorPattern;
  if (style == "VerPattern")
    return Qt::VerPattern;
  if (style == "CrossPattern")
    return Qt::CrossPattern;
  if (style == "BDiagPattern")
    return Qt::BDiagPattern;
  if (style == "FDiagPattern")
    return Qt::FDiagPattern;
  if (style == "DiagCrossPattern")
    return Qt::DiagCrossPattern;
  return Qt::SolidPattern;
}

static inline Qt::PenStyle nametoPenStyle(const QString &style)
{
  if (style.isEmpty() || style == "SolidLine")
    return Qt::SolidLine;
  if (style == "NoPen")
    return Qt::NoPen;
  if (style == "DashLine")
    return Qt::DashLine;
  if (style == "DotLine")
    return Qt::DotLine;
  if (style == "DashDotLine")
    return Qt::DashDotLine;
  if (style == "DashDotDotLine")
    return Qt::DashDotDotLine;
  return Qt::SolidLine;
}

void FLDataTable::paintField(QPainter *p, const QSqlField *field,
                             const QRect &cr, bool selected)
{
  if (!field)
    return;

  FLTableMetaData *tMD = cursor_->metadata();
  FLFieldMetaData *fieldTMD = tMD->field(field->name());
  if (!fieldTMD)
    return;

  if (!fieldTMD->visible()) {
    QBrush bu(gray, DiagCrossPattern);
    p->fillRect(0, 0, cr.width() - 1, cr.height() - 1, bu);
    return;
  }

  QString bgColorName;
  QString fgColorName;
  QString brushStyle;
  QString penStyle;
  int type = fieldTMD->type();

  if (!functionGetColor_.isEmpty()) {
    QSArgumentList arglist;
    QSArgument ret;

    arglist.append(QSArgument(fieldTMD->name()));
    arglist.append(QSArgument(field->value()));
    arglist.append(QSArgument(FLSqlCursorInterface::sqlCursorInterface(cursor_)));
    arglist.append(QSArgument(selected));
    arglist.append(QSArgument(type));
    ret = aqApp->call(functionGetColor_, arglist, 0);

    if (ret.type() == QSArgument::Variant && ret.variant().type() == QVariant::List) {
      QValueList<QVariant> list(ret.variant().toList());

      if (!list.isEmpty()) {
        if (list.size() >= 1) {
          bgColorName = list[0].toString();
          if (list.size() >= 2) {
            fgColorName = list[1].toString();
            if (list.size() >= 3) {
              brushStyle = list[2].toString();
              if (list.size() >= 4)
                penStyle = list[3].toString();
            }
          }
        }

        QBrush bu(p->brush());
        if (!bgColorName.isEmpty()) {
          QColor bgColor(bgColorName);
          if (bgColor.isValid())
            bu.setColor(bgColor);
          else
            bgColorName = QString::null;
        }
        if (!brushStyle.isEmpty())
          bu.setStyle(nametoBrushStyle(brushStyle));

        QPen pen(p->pen());
        if (!fgColorName.isEmpty()) {
          QColor fgColor(fgColorName);
          if (fgColor.isValid())
            pen.setColor(fgColor);
          else
            fgColorName = QString::null;
        }
        if (!penStyle.isEmpty())
          pen.setStyle(nametoPenStyle(penStyle));

        if (!bgColorName.isEmpty() || !brushStyle.isEmpty()) {
          p->setBrush(bu);
          p->fillRect(0, 0, cr.width() - 1, cr.height() - 1, bu);
        }
        if (!fgColorName.isEmpty() || !penStyle.isEmpty())
          p->setPen(pen);
      }
    }
  }

  if (field->isNull() && type != QVariant::Bool)
    return;

  QString text;

  switch (type) {
    case QVariant::Double: {
      double fValue = field->value().toDouble();
      if (fValue < 0.0 && fgColorName.isEmpty()) {
        QPen pen(p->pen());
        pen.setColor(red);
        p->setPen(pen);
      }
      text = aqApp->localeSystem().toString(fValue, 'f', fieldTMD->partDecimal());
      p->drawText(2, 2, cr.width() - 4, cr.height() - 4,
                  Qt::AlignRight | Qt::AlignVCenter, text);
    }
    break;

    case FLFieldMetaData::Unlock: {
      if (field->value().toBool())
        p->drawPixmap((cr.width() - ok.width()) / 2, 2, ok,
                      0, 0, cr.width() - 4, cr.height() - 4);
      else
        p->drawPixmap((cr.width() - no.width()) / 2, 2, no,
                      0, 0, cr.width() - 4, cr.height() - 4);
    }
    break;

    case QVariant::String: {
      text = field->value().toString();
      if (fieldTMD->hasOptionsList()) {
        QStringList ol(fieldTMD->optionsList());
        if (!ol.contains(text)) {
          QVariant defVal(fieldTMD->defaultValue());
          if (defVal.isValid())
            text = defVal.toString();
          else
            text = ol.first();
        }
        text = FLUtil::translate("MetaData", text);
      }
      p->drawText(2, 2, cr.width() - 4, cr.height() - 4, fieldAlignment(field), text);
    }
    break;

    case QVariant::Int: {
      int fValue = field->value().toInt();
      if (fValue < 0 && fgColorName.isEmpty()) {
        QPen pen(p->pen());
        pen.setColor(red);
        p->setPen(pen);
      }
      text = aqApp->localeSystem().toString(fValue);
      p->drawText(2, 2, cr.width() - 4, cr.height() - 4,
                  Qt::AlignRight | Qt::AlignVCenter, text);
    }
    break;

    case FLFieldMetaData::Serial:
    case QVariant::UInt:
      text = aqApp->localeSystem().toString(field->value().toUInt());
      p->drawText(2, 2, cr.width() - 4, cr.height() - 4,
                  Qt::AlignRight | Qt::AlignVCenter, text);
      break;

    case QVariant::Pixmap: {
      QCString cs = cursor_->db()->manager()->fetchLargeValue(field->value().toString()).toCString();
      if (cs.isEmpty())
        return ;

      QPixmap pix;

      if (!QPixmapCache::find(cs.left(100), pix)) {
        pix.loadFromData(cs);
        QPixmapCache::insert(cs.left(100), pix);
      }
      if (!pix.isNull())
        p->drawPixmap(2, 2, pix, 0, 0, cr.width() - 4,
                      cr.height() - 4);
    }
    break;

    case QVariant::Date: {
      QDate d = field->value().toDate();

      text = d.toString("dd-MM-yyyy");
      p->drawText(2, 2, cr.width() - 4, cr.height() - 4,
                  fieldAlignment(field), text);
    }
    break;

    case QVariant::Time: {
      QTime t = field->value().toTime();

      text = t.toString("hh:mm:ss");
      p->drawText(2, 2, cr.width() - 4, cr.height() - 4,
                  fieldAlignment(field), text);
    }
    break;

    case QVariant::StringList:
      text = field->value().toString();
      p->drawText(2, 2, cr.width() - 4, cr.height() - 4,
                  Qt::AlignAuto | Qt::AlignTop, text.left(255) + "...");
      //p->drawText( 2, 2, cr.width() - 4, cr.height() - 4,
      //             Qt::AlignAuto | Qt::AlignTop, text );
      break;

    case QVariant::Bool: {
      if (fieldTMD->isCheck()) {
        int row = rowAt(cr.center().y()), col = columnAt(cr.center().x());
        int curAt = cursor_->at();
        FLCheckBox *chk = ::qt_cast<FLCheckBox *>(cellWidget(row, col));
        if (!chk) {
          chk = new FLCheckBox(this, row);
          setCellWidget(row, col, chk);
        } else
          disconnect(chk, SIGNAL(toggled(bool)), this, SLOT(setChecked(bool)));
        if (cursor_->QSqlCursor::seek(row)) {
          chk->setChecked(primarysKeysChecked_.contains(cursor_->QSqlCursor::value(tMD->primaryKey())));
          cursor_->QSqlCursor::seek(curAt);
        }
        connect(chk, SIGNAL(toggled(bool)), this, SLOT(setChecked(bool)));
      } else {
        if (field->value().toBool()) {
          QBrush bu(p->brush());
          if (bgColorName.isEmpty())
            bu.setColor(green);
          if (brushStyle.isEmpty())
            bu.setStyle(selected ? Dense4Pattern : SolidPattern);
          p->fillRect(0, 0, cr.width() - 1, cr.height() - 1, bu);
        } else {
          QBrush bu(p->brush());
          if (bgColorName.isEmpty())
            bu.setColor(red);
          if (brushStyle.isEmpty())
            bu.setStyle(selected ? Dense4Pattern : SolidPattern);
          p->fillRect(0, 0, cr.width() - 1, cr.height() - 1, bu);
        }
        if (!selected && fgColorName.isEmpty()) {
          QPen pen(p->pen());
          pen.setColor(darkBlue);
          p->setPen(pen);
        }
        text = field->value().toBool() ? tr("Sí") : tr("No");
        p->drawText(2, 2, cr.width() - 4, cr.height() - 4,
                    fieldAlignment(field), text);
      }
    }
    break;
  }
  lastTextPainted = text;
}

bool FLDataTable::eventFilter(QObject *o, QEvent *e)
{
  int r = currentRow(), c = currentColumn(), nr = numRows(), nc = numCols();

  switch (e->type()) {
    case QEvent::KeyPress: {
      QKeyEvent *ke = static_cast<QKeyEvent *>(e);

      if (ke->key() == Key_Escape && popup_ && parentWidget()) {
        parentWidget()->hide();
        return true;
      }

      if (ke->key() == Key_Insert)
        return true;

      if (ke->key() == Key_F2)
        return true;

      if (ke->key() == Key_Up && r == 0)
        return true;

      if (ke->key() == Key_Left && c == 0)
        return true;

      if (ke->key() == Key_Down && r == nr - 1)
        return true;

      if (ke->key() == Key_Right && c == nc - 1)
        return true;

      if ((ke->key() == Key_Enter || ke->key() == Key_Return) && r > -1) {
        emit recordChoosed();
        return true;
      }

      if (ke->key() == Key_A && !popup_)
        if (cursor_ && !readonly_ && !editonly_) {
          cursor_->insertRecord();
          return true;
        } else
          return false;

      if (ke->key() == Key_C && !popup_)
        if (cursor_ && !readonly_ && !editonly_) {
          cursor_->copyRecord();
          return true;
        } else
          return false;

      if (ke->key() == Key_M && !popup_)
        if (insertonly_)
          return false;
        else if (cursor_ && !readonly_) {
          cursor_->editRecord();
          return true;
        } else
          return false;

      if (ke->key() == Key_E || ke->key() == Key_Delete && !popup_)
        if (insertonly_)
          return false;
        else if (cursor_ && !readonly_ && !editonly_) {
          cursor_->deleteRecord();
          return true;
        } else
          return false;

      if (ke->key() == Key_V && !popup_)
        if (cursor_) {
          cursor_->browseRecord();
          return true;
        }
      return false;
    }
    break;
  }
  return QDataTable::eventFilter(o, e);
}

void FLDataTable::contentsContextMenuEvent(QContextMenuEvent *e)
{
  QTable::contentsContextMenuEvent(e);
}

void FLDataTable::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
  if (e->button() != LeftButton)
    return ;

  int tmpRow = rowAt(e->pos().y());
  int tmpCol = columnAt(e->pos().x());
  QTableItem *itm = item(tmpRow, tmpCol);

  if (itm && !itm->isEnabled())
    return ;

  emit doubleClicked(tmpRow, tmpCol, e->button(), e->pos());
  emit recordChoosed();
}

void FLDataTable::refresh()
{
  if (refreshing_) {
      if (!refresh_timer_) {
        QTimer::singleShot(500, this, SLOT(refresh()));
        refresh_timer_ = true;
      }
      return;
  }
  refreshing_ = true;
  refresh_timer_ = false;
  if (popup_) QDataTable::refresh();
  if (cursor_  && cursor_->metadata()) {
    cursor_->setFilter(persistentFilter_);
    FLSqlCursor *sndCursor = ::qt_cast<FLSqlCursor *>(sender());
    if (sndCursor) {
      setFilter(cursor_->curFilter());
      QDataTable::refresh();
      cursor_->QSqlCursor::seek(cursor_->atFrom());
      selectRow();
    } else {
      setFilter(cursor_->curFilter());
      QDataTable::refresh();
      selectRow();
    }
  }
  refreshing_ = false;
}

void FLDataTable::setFocus()
{
  if (!cursor_)
    return ;
  if (!hasFocus()) {
    setPaletteBackgroundColor(qApp->palette().color(QPalette::Active, QColorGroup::Base));
#ifndef FL_QUICK_CLIENT
    QDataTable::refresh();
#endif
  } else if (numRows() != cursor_->size())
    setNumRows(cursor_->size());
  QWidget::setFocus();
}

void FLDataTable::setQuickFocus()
{
  setPaletteBackgroundColor(qApp->palette().color(QPalette::Active, QColorGroup::Base));
  QWidget::setFocus();
}

void FLDataTable::focusOutEvent(QFocusEvent *)
{
  setPaletteBackgroundColor(qApp->palette().color(QPalette::Active, QColorGroup::Background));
}

void FLDataTable::setFLReadOnly(const bool mode)
{
  if (!cursor_)
    return ;
  cursor_->setEdition(!mode);
  readonly_ = mode;
}

void FLDataTable::setEditOnly(const bool mode)
{
  editonly_ = mode;
}

void FLDataTable::setInsertOnly(const bool mode)
{
  if (!cursor_)
    return ;
  cursor_->setEdition(!mode);
  insertonly_ = mode;
}

void FLDataTable::ensureRowSelectedVisible()
{
  if (rowSelected > -1) {
    if (!isUpdatesEnabled() || !viewport()->isUpdatesEnabled())
      return;
    int cw = columnWidth(colSelected);
    int margin = visibleHeight() / 2;
    int y = rowPos(rowSelected) + rowHeight(rowSelected) / 2;
    if (cw < visibleWidth())
      ensureVisible(columnPos(colSelected) + cw / 2, y, cw / 2, margin);
    else
      ensureVisible(columnPos(colSelected), y, 0, margin);
  }
}

void FLDataTable::setChecked(bool on)
{
  FLCheckBox *chk = ::qt_cast<FLCheckBox *>(sender());

  if (!chk || !cursor_ || !cursor_->metadata())
    return;

  int curAt = cursor_->at();
  int posAt = chk->row();

  if (cursor_->QSqlCursor::seek(posAt)) {
    QVariant primaryKeyValue(cursor_->QSqlCursor::value(cursor_->metadata()->primaryKey()));
    setPrimaryKeyChecked(primaryKeyValue, on);
    cursor_->QSqlCursor::seek(curAt);
  }
}

const QValueList<QVariant> FLDataTable::primarysKeysChecked() const
{
  return primarysKeysChecked_;
}

void FLDataTable::clearChecked()
{
  primarysKeysChecked_.clear();
}

void FLDataTable::setPrimaryKeyChecked(const QVariant &primaryKeyValue, bool on)
{
  if (on) {
    if (!primarysKeysChecked_.contains(primaryKeyValue)) {
      primarysKeysChecked_.append(primaryKeyValue);
      emit primaryKeyToggled(primaryKeyValue, true);
    }
  } else {
    if (primarysKeysChecked_.contains(primaryKeyValue)) {
      primarysKeysChecked_.remove(primaryKeyValue);
      emit primaryKeyToggled(primaryKeyValue, false);
    }
  }
}

void FLDataTable::setPersistentFilter(const QString &pFilter)
{
  persistentFilter_ = pFilter;
}

void FLDataTable::setColumnWidth(const QString &field, int w)
{
  widthCols_.insert(field, w);
}

void FLDataTable::handleError(const QSqlError &)
{
}

int FLDataTable::indexOf(uint i) const
{
  return QDataTable::indexOf(i);
}
