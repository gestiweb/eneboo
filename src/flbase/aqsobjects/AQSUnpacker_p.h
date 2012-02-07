/***************************************************************************
 AQSUnpacker_p.h
 -------------------
 begin                : 08/03/2011
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

#ifndef AQSUNPACKER_P_H_
#define AQSUNPACKER_P_H_

#include "AQSByteArray_p.h"
#include "AQObjects.h"

class AQSUnpacker : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID_AQOBJECT(Unpacker, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QString getText() const;
  QByteArray getBinary() const;
  QStringList errorMessages() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QString"))
      return new AQUnpacker(*(argValue<QString *>(args[0])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSUnpacker::getText() const
{
  AQ_CALL_RET_V(getText(), QString);
}
inline QByteArray AQSUnpacker::getBinary() const
{
  AQ_CALL_RET_V(getBinary(), QByteArray);
}
inline QStringList AQSUnpacker::errorMessages() const
{
  AQ_CALL_RET_V(errorMessages(), QStringList);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSUNPACKER_P_H_ */
// @AQOBJECT_VOID@
