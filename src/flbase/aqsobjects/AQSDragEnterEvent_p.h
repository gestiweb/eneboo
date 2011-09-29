/***************************************************************************
 AQSDragEnterEvent_p.h
 -------------------
 begin                : 22/02/2011
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

#ifndef AQSDRAGENTEREVENT_P_H_
#define AQSDRAGENTEREVENT_P_H_

#include "AQSDragMoveEvent_p.h"
#include <qevent.h>

class AQSDragEnterEvent : public AQSDragMoveEvent
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DragEnterEvent, DragMoveEvent);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QPoint"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QPoint"))
      return new QDragEnterEvent(*(argValue<QPoint *>(args[0])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QPoint"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDRAGENTEREVENT_P_H_ */
// @AQSVOIDPTR@
