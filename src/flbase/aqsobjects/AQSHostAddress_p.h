/***************************************************************************
 AQSHostAddress_p.h
 -------------------
 begin                : 28/04/2011
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

#ifndef AQSHOSTADDRESS_P_H_
#define AQSHOSTADDRESS_P_H_

#include "AQSVoidPtr_p.h"
#include <qhostaddress.h>

// @AQ_PTR_INDIRECTION@

class AQSHostAddress : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(HostAddress, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setAddress(Q_UINT32);
  void setAddress(Q_UINT8 *);
  bool setAddress(const QString &);
  bool isIPv4Address() const;
  bool isIPv6Address() const;
  QString toString() const;
  bool isNull() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("Q_UINT32"));
    candidates[1].append(QString::fromLatin1("Q_UINT8*"));
    candidates[1].append(QString::fromLatin1("Q_IPV6ADDR"));
    candidates[1].append(QString::fromLatin1("QHostAddress*"));
    candidates[0].append(QString::null);
    candidates[0].append(QString::null);
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QHostAddress;
    if (sgt == QString::fromLatin1("Q_UINT32"))
      return new QHostAddress(*(argValue<Q_UINT32 *>(args[0])));
    if (sgt == QString::fromLatin1("Q_UINT8*"))
      return new QHostAddress(argValue<Q_UINT8 *>(args[0]));
    if (sgt == QString::fromLatin1("Q_IPV6ADDR"))
      return new QHostAddress(*(argValue<Q_IPV6ADDR *>(args[0])));
    if (sgt == QString::fromLatin1("QHostAddress*"))
      return new QHostAddress(*argValue<QHostAddress *>(args[0]));
    if (sgt.isEmpty())
      return new QHostAddress;
    if (sgt.isEmpty())
      return new QHostAddress;
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("Q_UINT32"));
    candidates[1].append(QString::fromLatin1("Q_UINT8*"));
    candidates[1].append(QString::fromLatin1("Q_IPV6ADDR"));
    candidates[1].append(QString::fromLatin1("QHostAddress*"));
    candidates[0].append(QString::null);
    candidates[0].append(QString::null);
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSHostAddress::setAddress(Q_UINT32 arg0)
{
  AQ_CALL_VOID(setAddress(arg0));
}
inline void AQSHostAddress::setAddress(Q_UINT8 *arg0)
{
  AQ_CALL_VOID(setAddress(arg0));
}
inline bool AQSHostAddress::setAddress(const QString &arg0)
{
  AQ_CALL_RET_V(setAddress(arg0), bool);
}
inline bool AQSHostAddress::isIPv4Address() const
{
  AQ_CALL_RET_V(isIPv4Address(), bool);
}
inline bool AQSHostAddress::isIPv6Address() const
{
  AQ_CALL_RET_V(isIPv6Address(), bool);
}
inline QString AQSHostAddress::toString() const
{
  AQ_CALL_RET_V(toString(), QString);
}
inline bool AQSHostAddress::isNull() const
{
  AQ_CALL_RET_V(isNull(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSHOSTADDRESS_P_H_ */
// @AQSVOIDPTR@
