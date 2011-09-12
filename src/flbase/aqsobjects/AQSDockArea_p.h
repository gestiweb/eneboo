/***************************************************************************
 AQSDockArea_p.h
 -------------------
 begin                : 02/04/2011
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

#ifndef AQSDOCKAREA_P_H_
#define AQSDOCKAREA_P_H_

#include "AQSWidget_p.h"
#include <qdockarea.h>

class AQSDockArea : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(DockArea, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void moveDockWindow(QDockWindow *, const QPoint &, const QRect &, bool);
  void removeDockWindow(QDockWindow *, bool, bool, bool = TRUE);
  void moveDockWindow(QDockWindow *, int = -1);
  bool hasDockWindow(QDockWindow *, int* = 0);
  bool isEmpty() const;
  bool isDockWindowAccepted(QDockWindow *);
  void setAcceptDockWindow(QDockWindow *, bool);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("Qt::Orientation"));
    candidates[2].append(QString::fromLatin1("Qt::Orientation,QDockArea::HandlePosition"));
    candidates[3].append(QString::fromLatin1("Qt::Orientation,QDockArea::HandlePosition,QWidget*"));
    candidates[4].append(QString::fromLatin1("Qt::Orientation,QDockArea::HandlePosition,QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("Qt::Orientation"))
      return new QDockArea(static_cast<Qt::Orientation>(args[0].variant().toUInt()));
    if (sgt == QString::fromLatin1("Qt::Orientation,QDockArea::HandlePosition"))
      return new QDockArea(static_cast<Qt::Orientation>(args[0].variant().toUInt()),
                           static_cast<QDockArea::HandlePosition>(args[1].variant().toUInt()));
    if (sgt == QString::fromLatin1("Qt::Orientation,QDockArea::HandlePosition,QWidget*"))
      return new QDockArea(static_cast<Qt::Orientation>(args[0].variant().toUInt()),
                           static_cast<QDockArea::HandlePosition>(args[1].variant().toUInt()),
                           argValue<QWidget *>(args[2]));
    if (sgt == QString::fromLatin1("Qt::Orientation,QDockArea::HandlePosition,QWidget*,QString"))
      return new QDockArea(static_cast<Qt::Orientation>(args[0].variant().toUInt()),
                           static_cast<QDockArea::HandlePosition>(args[1].variant().toUInt()),
                           argValue<QWidget *>(args[2]),
                           *(argValue<QString *>(args[3])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("Qt::Orientation"));
    candidates[2].append(QString::fromLatin1("Qt::Orientation,QDockArea::HandlePosition"));
    candidates[3].append(QString::fromLatin1("Qt::Orientation,QDockArea::HandlePosition,QWidget*"));
    candidates[4].append(QString::fromLatin1("Qt::Orientation,QDockArea::HandlePosition,QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSDockArea::moveDockWindow(QDockWindow *arg0,  const QPoint &arg1,  const QRect &arg2,  bool arg3)
{
  AQ_CALL_VOID(moveDockWindow(arg0, arg1, arg2, arg3));
}
inline void AQSDockArea::removeDockWindow(QDockWindow *arg0,  bool arg1,  bool arg2,  bool arg3)
{
  AQ_CALL_VOID(removeDockWindow(arg0, arg1, arg2, arg3));
}
inline void AQSDockArea::moveDockWindow(QDockWindow *arg0,  int arg1)
{
  AQ_CALL_VOID(moveDockWindow(arg0, arg1));
}
inline bool AQSDockArea::hasDockWindow(QDockWindow *arg0,  int *arg1)
{
  AQ_CALL_RET_V(hasDockWindow(arg0, arg1), bool);
}
inline bool AQSDockArea::isEmpty() const
{
  AQ_CALL_RET_V(isEmpty(), bool);
}
inline bool AQSDockArea::isDockWindowAccepted(QDockWindow *arg0)
{
  AQ_CALL_RET_V(isDockWindowAccepted(arg0), bool);
}
inline void AQSDockArea::setAcceptDockWindow(QDockWindow *arg0,  bool arg1)
{
  AQ_CALL_VOID(setAcceptDockWindow(arg0, arg1));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDOCKAREA_P_H_ */
// @AQSWIDGET@
