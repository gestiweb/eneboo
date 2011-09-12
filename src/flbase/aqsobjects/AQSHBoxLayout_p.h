/***************************************************************************
 AQSHBoxLayout_p.h
 -------------------
 begin                : 07/02/2011
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

#ifndef AQSHBOXLAYOUT_P_H_
#define AQSHBOXLAYOUT_P_H_

#include "AQSBoxLayout_p.h"
#include <qlayout.h>

class AQSHBoxLayout : public AQSBoxLayout
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(HBoxLayout, BoxLayout);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,int"));
    candidates[3].append(QString::fromLatin1("QWidget*,int,int"));
    candidates[4].append(QString::fromLatin1("QWidget*,int,int,QString"));
    candidates[1].append(QString::fromLatin1("QLayout*"));
    candidates[2].append(QString::fromLatin1("QLayout*,int"));
    candidates[3].append(QString::fromLatin1("QLayout*,int,QString"));
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("int"));
    candidates[2].append(QString::fromLatin1("int,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QHBoxLayout(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,int"))
      return new QHBoxLayout(argValue<QWidget *>(args[0]),
                             args[1].variant().toInt());
    if (sgt == QString::fromLatin1("QWidget*,int,int"))
      return new QHBoxLayout(argValue<QWidget *>(args[0]),
                             args[1].variant().toInt(),
                             args[2].variant().toInt());
    if (sgt == QString::fromLatin1("QWidget*,int,int,QString"))
      return new QHBoxLayout(argValue<QWidget *>(args[0]),
                             args[1].variant().toInt(),
                             args[2].variant().toInt(),
                             *(argValue<QString *>(args[3])));
    if (sgt == QString::fromLatin1("QLayout*"))
      return new QHBoxLayout(argValue<QLayout *>(args[0]));
    if (sgt == QString::fromLatin1("QLayout*,int"))
      return new QHBoxLayout(argValue<QLayout *>(args[0]),
                             args[1].variant().toInt());
    if (sgt == QString::fromLatin1("QLayout*,int,QString"))
      return new QHBoxLayout(argValue<QLayout *>(args[0]),
                             args[1].variant().toInt(),
                             *(argValue<QString *>(args[2])));
    if (sgt.isEmpty())
      return new QHBoxLayout;
    if (sgt == QString::fromLatin1("int"))
      return new QHBoxLayout(args[0].variant().toInt());
    if (sgt == QString::fromLatin1("int,QString"))
      return new QHBoxLayout(args[0].variant().toInt(),
                             *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,int"));
    candidates[3].append(QString::fromLatin1("QWidget*,int,int"));
    candidates[4].append(QString::fromLatin1("QWidget*,int,int,QString"));
    candidates[1].append(QString::fromLatin1("QLayout*"));
    candidates[2].append(QString::fromLatin1("QLayout*,int"));
    candidates[3].append(QString::fromLatin1("QLayout*,int,QString"));
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("int"));
    candidates[2].append(QString::fromLatin1("int,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSHBOXLAYOUT_P_H_ */
// @AQSOBJECT@
