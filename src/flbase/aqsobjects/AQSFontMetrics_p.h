/***************************************************************************
 AQSFontMetrics_p.h
 -------------------
 begin                : 18/02/2011
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

#ifndef AQSFONTMETRICS_P_H_
#define AQSFONTMETRICS_P_H_

#include "AQSVoidPtr_p.h"
#include <qfontmetrics.h>

// @AQ_PTR_INDIRECTION@

class AQSFontMetrics : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(FontMetrics, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int ascent() const;
  int descent() const;
  int height() const;
  int leading() const;
  int lineSpacing() const;
  int minLeftBearing() const;
  int minRightBearing() const;
  int maxWidth() const;
  bool inFont(QChar) const;
  int leftBearing(QChar) const;
  int rightBearing(QChar) const;
  int width(const QString &, int = -1) const;
  int width(QChar) const;
  int charWidth(const QString &, int) const;
  QRect boundingRect(const QString &, int = -1) const;
  QRect boundingRect(QChar) const;
  QRect boundingRect(int, int, int, int, int, const QString &, int = -1, int = 0, int* = 0, QTextParag ** = 0) const;
  QSize size(int, const QString &, int = -1, int = 0, int* = 0, QTextParag ** = 0) const;
  int underlinePos() const;
  int overlinePos() const;
  int strikeOutPos() const;
  int lineWidth() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QFont"));
    candidates[2].append(QString::fromLatin1("QFont,QFont::Script"));
    candidates[1].append(QString::fromLatin1("QFontMetrics*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QFont"))
      return new QFontMetrics(*(argValue<QFont *>(args[0])));
    if (sgt == QString::fromLatin1("QFont,QFont::Script"))
      return new QFontMetrics(*(argValue<QFont *>(args[0])),
                              static_cast<QFont::Script>(args[1].variant().toUInt()));
    if (sgt == QString::fromLatin1("QFontMetrics*"))
      return new QFontMetrics(*argValue<QFontMetrics *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QFont"));
    candidates[2].append(QString::fromLatin1("QFont,QFont::Script"));
    candidates[1].append(QString::fromLatin1("QFontMetrics*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSFontMetrics::ascent() const
{
  AQ_CALL_RET_V(ascent(), int);
}
inline int AQSFontMetrics::descent() const
{
  AQ_CALL_RET_V(descent(), int);
}
inline int AQSFontMetrics::height() const
{
  AQ_CALL_RET_V(height(), int);
}
inline int AQSFontMetrics::leading() const
{
  AQ_CALL_RET_V(leading(), int);
}
inline int AQSFontMetrics::lineSpacing() const
{
  AQ_CALL_RET_V(lineSpacing(), int);
}
inline int AQSFontMetrics::minLeftBearing() const
{
  AQ_CALL_RET_V(minLeftBearing(), int);
}
inline int AQSFontMetrics::minRightBearing() const
{
  AQ_CALL_RET_V(minRightBearing(), int);
}
inline int AQSFontMetrics::maxWidth() const
{
  AQ_CALL_RET_V(maxWidth(), int);
}
inline bool AQSFontMetrics::inFont(QChar arg0) const
{
  AQ_CALL_RET_V(inFont(arg0), bool);
}
inline int AQSFontMetrics::leftBearing(QChar arg0) const
{
  AQ_CALL_RET_V(leftBearing(arg0), int);
}
inline int AQSFontMetrics::rightBearing(QChar arg0) const
{
  AQ_CALL_RET_V(rightBearing(arg0), int);
}
inline int AQSFontMetrics::width(const QString &arg0,  int arg1) const
{
  AQ_CALL_RET_V(width(arg0, arg1), int);
}
inline int AQSFontMetrics::width(QChar arg0) const
{
  AQ_CALL_RET_V(width(arg0), int);
}
inline int AQSFontMetrics::charWidth(const QString &arg0,  int arg1) const
{
  AQ_CALL_RET_V(charWidth(arg0, arg1), int);
}
inline QRect AQSFontMetrics::boundingRect(const QString &arg0,  int arg1) const
{
  AQ_CALL_RET_V(boundingRect(arg0, arg1), QRect);
}
inline QRect AQSFontMetrics::boundingRect(QChar arg0) const
{
  AQ_CALL_RET_V(boundingRect(arg0), QRect);
}
inline QRect AQSFontMetrics::boundingRect(int arg0,  int arg1,  int arg2,  int arg3,  int arg4,  const QString &arg5,  int arg6,  int arg7,  int *arg8,  QTextParag **arg9) const
{
  AQ_CALL_RET_V(boundingRect(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9), QRect);
}
inline QSize AQSFontMetrics::size(int arg0,  const QString &arg1,  int arg2,  int arg3,  int *arg4,  QTextParag **arg5) const
{
  AQ_CALL_RET_V(size(arg0, arg1, arg2, arg3, arg4, arg5), QSize);
}
inline int AQSFontMetrics::underlinePos() const
{
  AQ_CALL_RET_V(underlinePos(), int);
}
inline int AQSFontMetrics::overlinePos() const
{
  AQ_CALL_RET_V(overlinePos(), int);
}
inline int AQSFontMetrics::strikeOutPos() const
{
  AQ_CALL_RET_V(strikeOutPos(), int);
}
inline int AQSFontMetrics::lineWidth() const
{
  AQ_CALL_RET_V(lineWidth(), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSFONTMETRICS_P_H_ */
// @AQSVOIDPTR@
