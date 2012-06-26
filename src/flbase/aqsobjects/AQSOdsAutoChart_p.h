/***************************************************************************
 AQSOdsAutoChart_p.h
 -------------------
 begin                : 16/12/2011
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

#ifndef AQSODSAUTOCHART_P_H_
#define AQSODSAUTOCHART_P_H_

#include "AQSOdsChart_p.h"
#include "AQSOdsSheet_p.h"

// @AQ_PTR_INDIRECTION@

class AQSOdsAutoChart : public AQSOdsChart
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(OdsAutoChart, OdsChart);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[4].append(QString::fromLatin1("QString,AQOdsLength*,AQOdsLength*,AQOdsSheet*"));
    candidates[4].append(QString::fromLatin1("QString,AQOdsCentimeters*,AQOdsCentimeters*,AQOdsSheet*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QString,AQOdsLength*,AQOdsLength*,AQOdsSheet*"))
      return new AQOdsAutoChart(*(argValue<QString *>(args[0])),
                                *argValue<AQOdsLength *>(args[1]),
                                *argValue<AQOdsLength *>(args[2]),
                                *argValue<AQOdsSheet *>(args[3]));
    if (sgt == QString::fromLatin1("QString,AQOdsCentimeters*,AQOdsCentimeters*,AQOdsSheet*"))
      return new AQOdsAutoChart(*(argValue<QString *>(args[0])),
                                *argValue<AQOdsCentimeters *>(args[1]),
                                *argValue<AQOdsCentimeters *>(args[2]),
                                *argValue<AQOdsSheet *>(args[3]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[4].append(QString::fromLatin1("QString,AQOdsLength*,AQOdsLength*,AQOdsSheet*"));
    candidates[4].append(QString::fromLatin1("QString,AQOdsCentimeters*,AQOdsCentimeters*,AQOdsSheet*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSODSAUTOCHART_P_H_ */
// @AQOBJECT_VOID@
