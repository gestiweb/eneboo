/***************************************************************************
 AQSFrame_p.h
 -------------------
 begin                : 30/01/2011
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

#ifndef AQSFRAME_P_H_
#define AQSFRAME_P_H_

#include "AQSWidget_p.h"
#include <qframe.h>

class AQSFrame : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Frame, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int frameStyle() const;
  void setFrameStyle(int);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QFrame;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QFrame(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QFrame(argValue<QWidget *>(args[0]),
                        *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new QFrame(argValue<QWidget *>(args[0]),
                        *(argValue<QString *>(args[1])),
                        static_cast<WFlags>(args[2].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSFrame::frameStyle() const
{
  AQ_CALL_RET_V(frameStyle(), int);
}
inline void AQSFrame::setFrameStyle(int arg0)
{
  AQ_CALL_VOID(setFrameStyle(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSFRAME_P_H_ */
// @AQSWIDGET@
