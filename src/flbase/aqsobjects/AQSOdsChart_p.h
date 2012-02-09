/***************************************************************************
 AQSOdsChart_p.h
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

#ifndef AQSODSCHART_P_H_
#define AQSODSCHART_P_H_

#include "AQSOdsSeries_p.h"
#include "AQSOdsCentimeters_p.h"

// @AQ_PTR_INDIRECTION@

class AQSOdsChart : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(OdsChart, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void addRange(AQOdsCellRange *);
  void addRange(AQSOdsCellRange *);
  void addSeries(AQOdsSeries *);
  void addSeries(AQSOdsSeries *);
  void setTitle(const QString &);
  void setSubtitle(const QString &);
  void setXAxisLabel(const QString &);
  void setYAxisLabel(const QString &);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[3].append(QString::fromLatin1("QString,AQOdsLength*,AQOdsLength*"));
    candidates[3].append(QString::fromLatin1("QString,AQOdsCentimeters*,AQOdsCentimeters*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QString,AQOdsLength*,AQOdsLength*"))
      return new AQOdsChart(*(argValue<QString *>(args[0])),
                            *argValue<AQOdsLength *>(args[1]),
                            *argValue<AQOdsLength *>(args[2]));
    if (sgt == QString::fromLatin1("QString,AQOdsCentimeters*,AQOdsCentimeters*"))
      return new AQOdsChart(*(argValue<QString *>(args[0])),
                            *argValue<AQOdsCentimeters *>(args[1]),
                            *argValue<AQOdsCentimeters *>(args[2]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[3].append(QString::fromLatin1("QString,AQOdsLength*,AQOdsLength*"));
    candidates[3].append(QString::fromLatin1("QString,AQOdsCentimeters*,AQOdsCentimeters*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSOdsChart::addRange(AQOdsCellRange *arg0)
{
  AQ_CALL_VOID(addRange(*arg0));
}
inline void AQSOdsChart::addRange(AQSOdsCellRange *arg0)
{
  AQ_CALL_VOID(addRange(*arg0));
}
inline void AQSOdsChart::addSeries(AQOdsSeries *arg0)
{
  AQ_CALL_VOID(addSeries(*arg0));
}
inline void AQSOdsChart::addSeries(AQSOdsSeries *arg0)
{
  AQ_CALL_VOID(addSeries(*arg0));
}
inline void AQSOdsChart::setTitle(const QString &arg0)
{
  AQ_CALL_VOID(setTitle(arg0));
}
inline void AQSOdsChart::setSubtitle(const QString &arg0)
{
  AQ_CALL_VOID(setSubtitle(arg0));
}
inline void AQSOdsChart::setXAxisLabel(const QString &arg0)
{
  AQ_CALL_VOID(setXAxisLabel(arg0));
}
inline void AQSOdsChart::setYAxisLabel(const QString &arg0)
{
  AQ_CALL_VOID(setYAxisLabel(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSODSCHART_P_H_ */
// @AQOBJECT_VOID@
