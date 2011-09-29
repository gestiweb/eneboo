/***************************************************************************
 AQSIconDragEvent_p.h
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

#ifndef AQSICONDRAGEVENT_P_H_
#define AQSICONDRAGEVENT_P_H_

#include "AQSEvent_p.h"
#include <qevent.h>

class AQSIconDragEvent : public AQSEvent
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(IconDragEvent, Event);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool isAccepted() const;
  void accept();
  void ignore();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QIconDragEvent;
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSIconDragEvent::isAccepted() const
{
  AQ_CALL_RET_V(isAccepted(), bool);
}
inline void AQSIconDragEvent::accept()
{
  AQ_CALL_VOID(accept());
}
inline void AQSIconDragEvent::ignore()
{
  AQ_CALL_VOID(ignore());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSICONDRAGEVENT_P_H_ */
// @AQSVOIDPTR@
