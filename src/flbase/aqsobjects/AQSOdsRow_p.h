/***************************************************************************
 AQSOdsRow_p.h
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

#ifndef AQSODSROW_P_H_
#define AQSODSROW_P_H_

#include "AQSOdsStyle_p.h"
#include "AQSOdsAutoChart_p.h"
#include "AQSOdsImage_p.h"
#include "AQSOdsColor_p.h"

// @AQ_PTR_INDIRECTION@

class AQSOdsRow : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(OdsRow, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void close();
  AQOdsRow *opIn(AQOdsChart *, uint = 0, uint = 0);
  AQOdsRow *opIn(AQSOdsChart *, uint = 0, uint = 0);
  AQOdsRow *opIn(AQOdsImage *, uint = 0, uint = 0);
  AQOdsRow *opIn(AQSOdsImage *, uint = 0, uint = 0);
  AQOdsRow *opIn(AQOdsStyle *);
  AQOdsRow *opIn(AQSOdsStyle *);
  AQOdsRow *opIn(const QString &, uint = 0, uint = 0);
  AQOdsRow *opIn(double, uint = 0, uint = 0);
  AQOdsRow *coveredCell();
  AQOdsRow *addBgColor(AQOdsColor *);
  AQOdsRow *addBgColor(AQSOdsColor *);
  AQOdsRow *addFgColor(AQOdsColor *);
  AQOdsRow *addFgColor(AQSOdsColor *);
  AQOdsRow *setFixedPrecision(uint);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("AQOdsSheet*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("AQOdsSheet*"))
      return new AQOdsRow(*argValue<AQOdsSheet *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("AQOdsSheet*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSOdsRow::close()
{
  AQ_CALL_VOID(close());
}
inline AQOdsRow *AQSOdsRow::opIn(AQOdsChart *arg0,  uint arg1,  uint arg2)
{
  AQ_CALL_RET_PTR(opIn(*arg0, arg1, arg2), AQOdsRow);
}
inline AQOdsRow *AQSOdsRow::opIn(AQSOdsChart *arg0,  uint arg1,  uint arg2)
{
  AQ_CALL_RET_PTR(opIn(*arg0, arg1, arg2), AQOdsRow);
}
inline AQOdsRow *AQSOdsRow::opIn(AQOdsImage *arg0,  uint arg1,  uint arg2)
{
  AQ_CALL_RET_PTR(opIn(*arg0, arg1, arg2), AQOdsRow);
}
inline AQOdsRow *AQSOdsRow::opIn(AQSOdsImage *arg0,  uint arg1,  uint arg2)
{
  AQ_CALL_RET_PTR(opIn(*arg0, arg1, arg2), AQOdsRow);
}
inline AQOdsRow *AQSOdsRow::opIn(AQOdsStyle *arg0)
{
  AQ_CALL_RET_PTR(opIn(*arg0), AQOdsRow);
}
inline AQOdsRow *AQSOdsRow::opIn(AQSOdsStyle *arg0)
{
  AQ_CALL_RET_PTR(opIn(*arg0), AQOdsRow);
}
inline AQOdsRow *AQSOdsRow::opIn(const QString &arg0,  uint arg1,  uint arg2)
{
  AQ_CALL_RET_PTR(opIn(arg0, arg1, arg2), AQOdsRow);
}
inline AQOdsRow *AQSOdsRow::opIn(double arg0,  uint arg1,  uint arg2)
{
  AQ_CALL_RET_PTR(opIn(arg0, arg1, arg2), AQOdsRow);
}
inline AQOdsRow *AQSOdsRow::coveredCell()
{
  AQ_CALL_RET_PTR(coveredCell(), AQOdsRow);
}
inline AQOdsRow *AQSOdsRow::addBgColor(AQOdsColor *arg0)
{
  AQ_CALL_RET_PTR(addBgColor(*arg0), AQOdsRow);
}
inline AQOdsRow *AQSOdsRow::addBgColor(AQSOdsColor *arg0)
{
  AQ_CALL_RET_PTR(addBgColor(*arg0), AQOdsRow);
}
inline AQOdsRow *AQSOdsRow::addFgColor(AQOdsColor *arg0)
{
  AQ_CALL_RET_PTR(addFgColor(*arg0), AQOdsRow);
}
inline AQOdsRow *AQSOdsRow::addFgColor(AQSOdsColor *arg0)
{
  AQ_CALL_RET_PTR(addFgColor(*arg0), AQOdsRow);
}
inline AQOdsRow *AQSOdsRow::setFixedPrecision(uint arg0)
{
  AQ_CALL_RET_PTR(setFixedPrecision(arg0), AQOdsRow);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSODSROW_P_H_ */
// @AQOBJECT_VOID@
