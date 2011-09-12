/***************************************************************************
 AQSVButtonGroup_p.h
 -------------------
 begin                : 06/02/2011
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

#ifndef AQSVBUTTONGROUP_P_H_
#define AQSVBUTTONGROUP_P_H_

#include "AQSButtonGroup_p.h"
#include <qvbuttongroup.h>

class AQSVButtonGroup : public AQSButtonGroup
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(VButtonGroup, ButtonGroup);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QVButtonGroup;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QVButtonGroup(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QVButtonGroup(argValue<QWidget *>(args[0]),
                               *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QString"))
      return new QVButtonGroup(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,QWidget*"))
      return new QVButtonGroup(*(argValue<QString *>(args[0])),
                               argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("QString,QWidget*,QString"))
      return new QVButtonGroup(*(argValue<QString *>(args[0])),
                               argValue<QWidget *>(args[1]),
                               *(argValue<QString *>(args[2])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSVBUTTONGROUP_P_H_ */
// @AQSWIDGET@
