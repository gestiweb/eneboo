/***************************************************************************
 AQSNetworkProtocol_p.h
 -------------------
 begin                : 21/03/2011
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

#ifndef AQSNETWORKPROTOCOL_P_H_
#define AQSNETWORKPROTOCOL_P_H_

#include "AQSUrlOperator_p.h"

class AQSNetworkProtocol : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(NetworkProtocol, Object);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual void setUrl(QUrlOperator *);
  virtual void setAutoDelete(bool, int = 10000);
  bool autoDelete() const;
  virtual int supportedOperations() const;
  virtual void addOperation(QNetworkOperation *);
  QUrlOperator *url() const;
  QNetworkOperation *operationInProgress() const;
  virtual void clearOperationQueue();
  virtual void stop();

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QNetworkProtocol;
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSNetworkProtocol::setUrl(QUrlOperator *arg0)
{
  AQ_CALL_VOID(setUrl(arg0));
}
inline void AQSNetworkProtocol::setAutoDelete(bool arg0,  int arg1)
{
  AQ_CALL_VOID(setAutoDelete(arg0, arg1));
}
inline bool AQSNetworkProtocol::autoDelete() const
{
  AQ_CALL_RET_V(autoDelete(), bool);
}
inline int AQSNetworkProtocol::supportedOperations() const
{
  AQ_CALL_RET_V(supportedOperations(), int);
}
inline void AQSNetworkProtocol::addOperation(QNetworkOperation *arg0)
{
  AQ_CALL_VOID(addOperation(arg0));
}
inline QUrlOperator *AQSNetworkProtocol::url() const
{
  AQ_CALL_RET(url());
}
inline QNetworkOperation *AQSNetworkProtocol::operationInProgress() const
{
  AQ_CALL_RET(operationInProgress());
}
inline void AQSNetworkProtocol::clearOperationQueue()
{
  AQ_CALL_VOID(clearOperationQueue());
}
inline void AQSNetworkProtocol::stop()
{
  AQ_CALL_VOID(stop());
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSNETWORKPROTOCOL_P_H_ */
// @AQSOBJECT@
