/***************************************************************************
 AQSEvent_p.h
 -------------------
 begin                : 29/01/2011
 copyright            : (C) 2003-2011 by InfoSiAL S.L.
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

#ifndef AQSEVENT_P_H_
#define AQSEVENT_P_H_

#include "AQSVoidPtr_p.h"

class AQSEvent : public AQSVoidPtr
{
  Q_OBJECT

  Q_PROPERTY(int type READ type)
  Q_PROPERTY(bool spontaneous READ spontaneous)
  Q_PROPERTY(QMap eventData READ eventData)

  AQ_DECLARE_AQS_VOID(Event, VoidPtr);

public:
  int type() const {
    AQ_CALL_RET_V(type(), int);
  }

  bool spontaneous() const {
    AQ_CALL_RET_V(spontaneous(), bool);
  }

  QMap<QString, QVariant> eventData() const {
    QMap<QString, QVariant> ret;
    if (!o_)
      return ret;

    switch (type()) {
      case QEvent::ContextMenu: {
        QContextMenuEvent *ev = static_cast<QContextMenuEvent *>(o_);
        ret["pos"] = ev->pos();
        ret["globalPos"] = ev->globalPos();
        ret["state"] = ev->state();
        ret["reason"] = ev->reason();
        break;
      }

      case QEvent::MouseMove:
      case QEvent::MouseButtonDblClick:
      case QEvent::MouseButtonRelease:
      case QEvent::MouseButtonPress: {
        QMouseEvent *ev = static_cast<QMouseEvent *>(o_);
        ret["pos"] = ev->pos();
        ret["globalPos"] = ev->globalPos();
        ret["state"] = ev->state();
        ret["button"] = ev->button();
        ret["stateAfter"] = ev->stateAfter();
        break;
      }

      case QEvent::Timer: {
        QTimerEvent *ev = static_cast<QTimerEvent *>(o_);
        ret["timerId"] = ev->timerId();
        break;
      }

      case QEvent::Wheel: {
        QWheelEvent *ev = static_cast<QWheelEvent *>(o_);
        ret["pos"] = ev->pos();
        ret["globalPos"] = ev->globalPos();
        ret["state"] = ev->state();
        ret["delta"] = ev->delta();
        ret["orientation"] = ev->orientation();
        break;
      }

      case QEvent::KeyRelease:
      case QEvent::KeyPress: {
        QKeyEvent *ev = static_cast<QKeyEvent *>(o_);
        ret["state"] = ev->state();
        ret["stateAfter"] = ev->stateAfter();
        ret["key"] = ev->key();
        ret["ascii"] = ev->ascii();
        ret["text"] = ev->text();
        ret["count"] = ev->count();
        ret["isAutoRepeat"] = ev->isAutoRepeat();
        break;
      }

      case QEvent::Paint: {
        QPaintEvent *ev = static_cast<QPaintEvent *>(o_);
        ret["rect"] = ev->rect();
        ret["erased"] = ev->erased();
        break;
      }

      case QEvent::Resize: {
        QResizeEvent *ev = static_cast<QResizeEvent *>(o_);
        ret["size"] = ev->size();
        ret["oldSize"] = ev->oldSize();
        break;
      }

      case QEvent::Move: {
        QMoveEvent *ev = static_cast<QMoveEvent *>(o_);
        ret["pos"] = ev->pos();
        ret["oldPos"] = ev->oldPos();
        break;
      }
    }
    return ret;
  }

  void setEvent(QEvent *ev) {
    o_ = ev;
  }

public slots:
  QTimerEvent *castTimerEvent() const {
    return static_cast<QTimerEvent *>(o_);
  }
  QMouseEvent *castMouseEvent() const {
    return static_cast<QMouseEvent *>(o_);
  }
  QWheelEvent *castWheelEvent() const {
    return static_cast<QWheelEvent *>(o_);
  }
  QKeyEvent *castKeyEvent() const {
    return static_cast<QKeyEvent *>(o_);
  }
  QFocusEvent *castFocusEvent() const {
    return static_cast<QFocusEvent *>(o_);
  }
  QPaintEvent *castPaintEvent() const {
    return static_cast<QPaintEvent *>(o_);
  }
  QMoveEvent *castMoveEvent() const {
    return static_cast<QMoveEvent *>(o_);
  }
  QResizeEvent *castResizeEvent() const {
    return static_cast<QResizeEvent *>(o_);
  }
  QCloseEvent *castCloseEvent() const {
    return static_cast<QCloseEvent *>(o_);
  }
  QIconDragEvent *castIconDragEvent() const {
    return static_cast<QIconDragEvent *>(o_);
  }
  QShowEvent *castShowEvent() const {
    return static_cast<QShowEvent *>(o_);
  }
  QHideEvent *castHideEvent() const {
    return static_cast<QHideEvent *>(o_);
  }
  QContextMenuEvent *castContextMenuEvent() const {
    return static_cast<QContextMenuEvent *>(o_);
  }
  QIMEvent *castIMEvent() const {
    return static_cast<QIMEvent *>(o_);
  }
  QDropEvent *castDropEvent() const {
    return static_cast<QDropEvent *>(o_);
  }
  QDragMoveEvent *castDragMoveEvent() const {
    return static_cast<QDragMoveEvent *>(o_);
  }
  QDragEnterEvent *castDragEnterEvent() const {
    return static_cast<QDragEnterEvent *>(o_);
  }
  QDragLeaveEvent *castDragLeaveEvent() const {
    return static_cast<QDragLeaveEvent *>(o_);
  }
  QChildEvent *castChildEvent() const {
    return static_cast<QChildEvent *>(o_);
  }
  QCustomEvent *castCustomEvent() const {
    return static_cast<QCustomEvent *>(o_);
  }
  QEvent *castEvent() const {
    return static_cast<QEvent *>(o_);
  }

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QEvent::Type"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QEvent::Type"))
      return new QEvent(static_cast<QEvent::Type>(args[0].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QEvent::Type"));
    return candidates;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSEVENT_P_H_ */

// @AQSVOIDPTR@

