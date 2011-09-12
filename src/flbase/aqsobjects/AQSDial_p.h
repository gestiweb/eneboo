/***************************************************************************
 AQSDial_p.h
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

#ifndef AQSDIAL_P_H_
#define AQSDIAL_P_H_

#include "AQSWidget_p.h"
#include <qdial.h>

class AQSDial : public AQSWidget
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Dial, Widget);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual void setNotchTarget(double);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    candidates[4].append(QString::fromLatin1("int,int,int,int"));
    candidates[5].append(QString::fromLatin1("int,int,int,int,QWidget*"));
    candidates[6].append(QString::fromLatin1("int,int,int,int,QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDial;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QDial(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QDial(argValue<QWidget *>(args[0]),
                       *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new QDial(argValue<QWidget *>(args[0]),
                       *(argValue<QString *>(args[1])),
                       static_cast<WFlags>(args[2].variant().toUInt()));
    if (sgt == QString::fromLatin1("int,int,int,int"))
      return new QDial(args[0].variant().toInt(),
                       args[1].variant().toInt(),
                       args[2].variant().toInt(),
                       args[3].variant().toInt());
    if (sgt == QString::fromLatin1("int,int,int,int,QWidget*"))
      return new QDial(args[0].variant().toInt(),
                       args[1].variant().toInt(),
                       args[2].variant().toInt(),
                       args[3].variant().toInt(),
                       argValue<QWidget *>(args[4]));
    if (sgt == QString::fromLatin1("int,int,int,int,QWidget*,QString"))
      return new QDial(args[0].variant().toInt(),
                       args[1].variant().toInt(),
                       args[2].variant().toInt(),
                       args[3].variant().toInt(),
                       argValue<QWidget *>(args[4]),
                       *(argValue<QString *>(args[5])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    candidates[4].append(QString::fromLatin1("int,int,int,int"));
    candidates[5].append(QString::fromLatin1("int,int,int,int,QWidget*"));
    candidates[6].append(QString::fromLatin1("int,int,int,int,QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSDial::setNotchTarget(double arg0)
{
  AQ_CALL_VOID(setNotchTarget(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDIAL_P_H_ */
// @AQSWIDGET@
