/***************************************************************************
 AQSHttpHeader_p.h
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

#ifndef AQSHTTPHEADER_P_H_
#define AQSHTTPHEADER_P_H_

#include "AQSVoidPtr_p.h"
#include <qhttp.h>

// @AQ_PTR_INDIRECTION@

class AQSHttpHeader : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(HttpHeader, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QString value(const QString &) const;
  void setValue(const QString &, const QString &);
  void removeValue(const QString &);
  QStringList keys() const;
  bool hasKey(const QString &) const;
  bool hasContentLength() const;
  uint contentLength() const;
  void setContentLength(int);
  bool hasContentType() const;
  QString contentType() const;
  void setContentType(const QString &);
  virtual QString toString() const;
  bool isValid() const;

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSHttpHeader::value(const QString &arg0) const
{
  AQ_CALL_RET_V(value(arg0), QString);
}
inline void AQSHttpHeader::setValue(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setValue(arg0, arg1));
}
inline void AQSHttpHeader::removeValue(const QString &arg0)
{
  AQ_CALL_VOID(removeValue(arg0));
}
inline QStringList AQSHttpHeader::keys() const
{
  AQ_CALL_RET_V(keys(), QStringList);
}
inline bool AQSHttpHeader::hasKey(const QString &arg0) const
{
  AQ_CALL_RET_V(hasKey(arg0), bool);
}
inline bool AQSHttpHeader::hasContentLength() const
{
  AQ_CALL_RET_V(hasContentLength(), bool);
}
inline uint AQSHttpHeader::contentLength() const
{
  AQ_CALL_RET_V(contentLength(), uint);
}
inline void AQSHttpHeader::setContentLength(int arg0)
{
  AQ_CALL_VOID(setContentLength(arg0));
}
inline bool AQSHttpHeader::hasContentType() const
{
  AQ_CALL_RET_V(hasContentType(), bool);
}
inline QString AQSHttpHeader::contentType() const
{
  AQ_CALL_RET_V(contentType(), QString);
}
inline void AQSHttpHeader::setContentType(const QString &arg0)
{
  AQ_CALL_VOID(setContentType(arg0));
}
inline QString AQSHttpHeader::toString() const
{
  AQ_CALL_RET_V(toString(), QString);
}
inline bool AQSHttpHeader::isValid() const
{
  AQ_CALL_RET_V(isValid(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSHTTPHEADER_P_H_ */
// @AQSVOIDPTR@
