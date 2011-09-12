/***************************************************************************
 AQSHttpResponseHeader_p.h
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

#ifndef AQSHTTPRESPONSEHEADER_P_H_
#define AQSHTTPRESPONSEHEADER_P_H_

#include "AQSHttpHeader_p.h"

// @AQ_PTR_INDIRECTION@

class AQSHttpResponseHeader : public AQSHttpHeader
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(HttpResponseHeader, HttpHeader);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int statusCode() const;
  QString reasonPhrase() const;
  virtual int majorVersion() const;
  virtual int minorVersion() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QHttpResponseHeader*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QHttpResponseHeader;
    if (sgt == QString::fromLatin1("QHttpResponseHeader*"))
      return new QHttpResponseHeader(*argValue<QHttpResponseHeader *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QHttpResponseHeader*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSHttpResponseHeader::statusCode() const
{
  AQ_CALL_RET_V(statusCode(), int);
}
inline QString AQSHttpResponseHeader::reasonPhrase() const
{
  AQ_CALL_RET_V(reasonPhrase(), QString);
}
inline int AQSHttpResponseHeader::majorVersion() const
{
  AQ_CALL_RET_V(majorVersion(), int);
}
inline int AQSHttpResponseHeader::minorVersion() const
{
  AQ_CALL_RET_V(minorVersion(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSHTTPRESPONSEHEADER_P_H_ */
// @AQSVOIDPTR@
