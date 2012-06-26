/***************************************************************************
 AQSTextCodec_p.h
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

#ifndef AQSTEXTCODEC_P_H_
#define AQSTEXTCODEC_P_H_

#include "AQSByteArray_p.h"
#include <qtextcodec.h>

class AQSTextCodec : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(TextCodec, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual const char *name() const;
  virtual const char *mimeName() const;
  virtual int mibEnum() const;
  virtual QTextDecoder *makeDecoder() const;
  virtual QTextEncoder *makeEncoder() const;
  virtual QString toUnicode(const char *, int) const;
  virtual QCString fromUnicode(const QString &, int &) const;
  QCString fromUnicode(const QString &) const;
  QString toUnicode(QByteArray *, int) const;
  QString toUnicode(AQSByteArray *, int) const;
  QString toUnicode(QByteArray *) const;
  QString toUnicode(AQSByteArray *) const;
  QString toUnicode(const QCString &, int) const;
  QString toUnicode(const QCString &) const;
  QString toUnicode(const char *) const;
  virtual bool canEncode(QChar) const;
  virtual bool canEncode(const QString &) const;
  virtual int heuristicContentMatch(const char *, int) const;
  virtual int heuristicNameMatch(const char *) const;

protected:
  static void *construct(const QSArgumentList &args) {
    return 0;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline const char *AQSTextCodec::name() const
{
  AQ_CALL_RET(name());
}
inline const char *AQSTextCodec::mimeName() const
{
  AQ_CALL_RET(mimeName());
}
inline int AQSTextCodec::mibEnum() const
{
  AQ_CALL_RET_V(mibEnum(), int);
}
inline QTextDecoder *AQSTextCodec::makeDecoder() const
{
  AQ_CALL_RET(makeDecoder());
}
inline QTextEncoder *AQSTextCodec::makeEncoder() const
{
  AQ_CALL_RET(makeEncoder());
}
inline QString AQSTextCodec::toUnicode(const char *arg0,  int arg1) const
{
  AQ_CALL_RET_V(toUnicode(arg0, arg1), QString);
}
inline QCString AQSTextCodec::fromUnicode(const QString &arg0,  int &arg1) const
{
  AQ_CALL_RET_V(fromUnicode(arg0, arg1), QCString);
}
inline QCString AQSTextCodec::fromUnicode(const QString &arg0) const
{
  AQ_CALL_RET_V(fromUnicode(arg0), QCString);
}
inline QString AQSTextCodec::toUnicode(QByteArray *arg0,  int arg1) const
{
  AQ_CALL_RET_V(toUnicode(*arg0, arg1), QString);
}
inline QString AQSTextCodec::toUnicode(AQSByteArray *arg0,  int arg1) const
{
  AQ_CALL_RET_V(toUnicode(*arg0, arg1), QString);
}
inline QString AQSTextCodec::toUnicode(QByteArray *arg0) const
{
  AQ_CALL_RET_V(toUnicode(*arg0), QString);
}
inline QString AQSTextCodec::toUnicode(AQSByteArray *arg0) const
{
  AQ_CALL_RET_V(toUnicode(*arg0), QString);
}
inline QString AQSTextCodec::toUnicode(const QCString &arg0,  int arg1) const
{
  AQ_CALL_RET_V(toUnicode(arg0, arg1), QString);
}
inline QString AQSTextCodec::toUnicode(const QCString &arg0) const
{
  AQ_CALL_RET_V(toUnicode(arg0), QString);
}
inline QString AQSTextCodec::toUnicode(const char *arg0) const
{
  AQ_CALL_RET_V(toUnicode(arg0), QString);
}
inline bool AQSTextCodec::canEncode(QChar arg0) const
{
  AQ_CALL_RET_V(canEncode(arg0), bool);
}
inline bool AQSTextCodec::canEncode(const QString &arg0) const
{
  AQ_CALL_RET_V(canEncode(arg0), bool);
}
inline int AQSTextCodec::heuristicContentMatch(const char *arg0,  int arg1) const
{
  AQ_CALL_RET_V(heuristicContentMatch(arg0, arg1), int);
}
inline int AQSTextCodec::heuristicNameMatch(const char *arg0) const
{
  AQ_CALL_RET_V(heuristicNameMatch(arg0), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTEXTCODEC_P_H_ */
// @AQSVOIDPTR@
