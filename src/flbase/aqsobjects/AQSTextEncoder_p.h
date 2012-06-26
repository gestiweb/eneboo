/***************************************************************************
 AQSTextEncoder_p.h
 -------------------
 begin                : 05/11/2011
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

#ifndef AQSTEXTENCODER_P_H_
#define AQSTEXTENCODER_P_H_

#include "AQSVoidPtr_p.h"
#include <qtextcodec.h>

class AQSTextEncoder : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(TextEncoder, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual QCString fromUnicode(const QString &, int &);

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QCString AQSTextEncoder::fromUnicode(const QString &arg0,  int &arg1)
{
  AQ_CALL_RET_V(fromUnicode(arg0, arg1), QCString);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTEXTENCODER_P_H_ */
// @AQSVOIDPTR@
