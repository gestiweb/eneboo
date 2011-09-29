/***************************************************************************
 AQSDropEvent_p.h
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

#ifndef AQSDROPEVENT_P_H_
#define AQSDROPEVENT_P_H_

#include "AQSEvent_p.h"
#include <qevent.h>

class AQSDropEvent : public AQSEvent
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DropEvent, Event);

public slots:
  QMimeSource *mimeSource() const {
    return static_cast<QMimeSource *>(o_);
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QPoint pos() const;
  bool isAccepted() const;
  void accept(bool = TRUE);
  void ignore();
  bool isActionAccepted() const;
  void acceptAction(bool = TRUE);
  void setAction(uint);
  uint action() const;
  QWidget *source() const;
  virtual const char *format(int = 0) const;
  virtual QByteArray *encodedData(const char *) const;
  virtual bool provides(const char *) const;
  void setPoint(const QPoint &);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QPoint"));
    candidates[2].append(QString::fromLatin1("QPoint,QEvent::Type"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QPoint"))
      return new QDropEvent(*(argValue<QPoint *>(args[0])));
    if (sgt == QString::fromLatin1("QPoint,QEvent::Type"))
      return new QDropEvent(*(argValue<QPoint *>(args[0])),
                            static_cast<QEvent::Type>(args[1].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QPoint"));
    candidates[2].append(QString::fromLatin1("QPoint,QEvent::Type"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QPoint AQSDropEvent::pos() const
{
  AQ_CALL_RET_V(pos(), QPoint);
}
inline bool AQSDropEvent::isAccepted() const
{
  AQ_CALL_RET_V(isAccepted(), bool);
}
inline void AQSDropEvent::accept(bool arg0)
{
  AQ_CALL_VOID(accept(arg0));
}
inline void AQSDropEvent::ignore()
{
  AQ_CALL_VOID(ignore());
}
inline bool AQSDropEvent::isActionAccepted() const
{
  AQ_CALL_RET_V(isActionAccepted(), bool);
}
inline void AQSDropEvent::acceptAction(bool arg0)
{
  AQ_CALL_VOID(acceptAction(arg0));
}
inline void AQSDropEvent::setAction(uint arg0)
{
  AQ_CALL_VOID(setAction(static_cast<QDropEvent::Action>(arg0)));
}
inline uint AQSDropEvent::action() const
{
  AQ_CALL_RET_V(action(), uint);
}
inline QWidget *AQSDropEvent::source() const
{
  AQ_CALL_RET(source());
}
inline const char *AQSDropEvent::format(int arg0) const
{
  AQ_CALL_RET(format(arg0));
}
inline QByteArray *AQSDropEvent::encodedData(const char *arg0) const
{
  AQ_CALL_RET_PTR(encodedData(arg0), QByteArray);
}
inline bool AQSDropEvent::provides(const char *arg0) const
{
  AQ_CALL_RET_V(provides(arg0), bool);
}
inline void AQSDropEvent::setPoint(const QPoint &arg0)
{
  AQ_CALL_VOID(setPoint(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDROPEVENT_P_H_ */
// @AQSVOIDPTR@
