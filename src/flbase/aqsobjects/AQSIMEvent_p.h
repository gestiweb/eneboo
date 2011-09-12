/***************************************************************************
 AQSIMEvent_p.h
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

#ifndef AQSIMEVENT_P_H_
#define AQSIMEVENT_P_H_

#include "AQSEvent_p.h"
#include <qevent.h>

class AQSIMEvent : public AQSEvent
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(IMEvent, Event);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  const QString &text() const;
  int cursorPos() const;
  bool isAccepted() const;
  void accept();
  void ignore();
  int selectionLength() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[3].append(QString::fromLatin1("QEvent::Type,QString,int"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QEvent::Type,QString,int"))
      return new QIMEvent(static_cast<QEvent::Type>(args[0].variant().toUInt()),
                          *(argValue<QString *>(args[1])),
                          args[2].variant().toInt());
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[3].append(QString::fromLatin1("QEvent::Type,QString,int"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline const QString &AQSIMEvent::text() const
{
  AQ_CALL_RET_V(text(), QString);
}
inline int AQSIMEvent::cursorPos() const
{
  AQ_CALL_RET_V(cursorPos(), int);
}
inline bool AQSIMEvent::isAccepted() const
{
  AQ_CALL_RET_V(isAccepted(), bool);
}
inline void AQSIMEvent::accept()
{
  AQ_CALL_VOID(accept());
}
inline void AQSIMEvent::ignore()
{
  AQ_CALL_VOID(ignore());
}
inline int AQSIMEvent::selectionLength() const
{
  AQ_CALL_RET_V(selectionLength(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSIMEVENT_P_H_ */
// @AQSVOIDPTR@
