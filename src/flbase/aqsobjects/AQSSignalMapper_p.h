/***************************************************************************
 AQSSignalMapper_p.h
 -------------------
 begin                : 08/02/2011
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

#ifndef AQSSIGNALMAPPER_P_H_
#define AQSSIGNALMAPPER_P_H_

#include "AQSObject_p.h"
#include <qsignalmapper.h>

class AQSSignalMapper : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(SignalMapper, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setMapping(const QObject *, int);
  void setMapping(const QObject *, const QString &);
  void removeMappings(const QObject *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QObject*"))
      return new QSignalMapper(argValue<QObject *>(args[0]));
    if (sgt == QString::fromLatin1("QObject*,QString"))
      return new QSignalMapper(argValue<QObject *>(args[0]),
                               *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QObject*"));
    candidates[2].append(QString::fromLatin1("QObject*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSSignalMapper::setMapping(const QObject *arg0,  int arg1)
{
  AQ_CALL_VOID(setMapping(arg0, arg1));
}
inline void AQSSignalMapper::setMapping(const QObject *arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setMapping(arg0, arg1));
}
inline void AQSSignalMapper::removeMappings(const QObject *arg0)
{
  AQ_CALL_VOID(removeMappings(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSIGNALMAPPER_P_H_ */
// @AQSOBJECT@
