/***************************************************************************
 AQSOdsSheet_p.h
 -------------------
 begin                : 15/12/2011
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

#ifndef AQSODSSHEET_P_H_
#define AQSODSSHEET_P_H_

#include "AQSOdsSpreadSheet_p.h"

// @AQ_PTR_INDIRECTION@

class AQSOdsSheet : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(OdsSheet, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void close();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("AQOdsSpreadSheet*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("AQOdsSpreadSheet*,QString"))
      return new AQOdsSheet(*argValue<AQOdsSpreadSheet *>(args[0]),
                            *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("AQOdsSpreadSheet*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSOdsSheet::close()
{
  AQ_CALL_VOID(close());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSODSSHEET_P_H_ */
// @AQOBJECT_VOID@
