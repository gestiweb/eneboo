/***************************************************************************
 AQSBrush_p.h
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

#ifndef AQSBRUSH_P_H_
#define AQSBRUSH_P_H_

#include "AQSPixmap_p.h"
#include <qbrush.h>

// @AQ_PTR_INDIRECTION@

class AQSBrush : public AQSVoidPtr
{
  Q_OBJECT

  Q_ENUMS(BrushStyle)

  Q_PROPERTY(QBrush toVariant READ toVariant);

  AQ_DECLARE_AQS_VOID(Brush, VoidPtr);

public:
  QBrush toVariant() const {
    if (!o_)
      return QBrush();
    return *o_;
  }

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  uint style() const;
  void setStyle(BrushStyle);
  QColor *color() const;
  void setColor(QColor *);
  void setColor(AQSColor *);
  QPixmap *pixmap() const;
  void setPixmap(QPixmap *);
  void setPixmap(AQSPixmap *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("Qt::BrushStyle"));
    candidates[1].append(QString::fromLatin1("QColor*"));
    candidates[2].append(QString::fromLatin1("QColor*,Qt::BrushStyle"));
    candidates[2].append(QString::fromLatin1("QColor*,QPixmap*"));
    candidates[1].append(QString::fromLatin1("QBrush*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QBrush;
    if (sgt == QString::fromLatin1("Qt::BrushStyle"))
      return new QBrush(static_cast<Qt::BrushStyle>(args[0].variant().toUInt()));
    if (sgt == QString::fromLatin1("QColor*"))
      return new QBrush(*argValue<QColor *>(args[0]));
    if (sgt == QString::fromLatin1("QColor*,Qt::BrushStyle"))
      return new QBrush(*argValue<QColor *>(args[0]),
                        static_cast<Qt::BrushStyle>(args[1].variant().toUInt()));
    if (sgt == QString::fromLatin1("QColor*,QPixmap*"))
      return new QBrush(*argValue<QColor *>(args[0]),
                        *argValue<QPixmap *>(args[1]));
    if (sgt == QString::fromLatin1("QBrush*"))
      return new QBrush(*argValue<QBrush *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("Qt::BrushStyle"));
    candidates[1].append(QString::fromLatin1("QColor*"));
    candidates[2].append(QString::fromLatin1("QColor*,Qt::BrushStyle"));
    candidates[2].append(QString::fromLatin1("QColor*,QPixmap*"));
    candidates[1].append(QString::fromLatin1("QBrush*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline uint AQSBrush::style() const
{
  AQ_CALL_RET_V(style(), uint);
}
inline void AQSBrush::setStyle(BrushStyle arg0)
{
  AQ_CALL_VOID(setStyle(arg0));
}
inline QColor *AQSBrush::color() const
{
  AQ_CALL_RET_PTR(color(), QColor);
}
inline void AQSBrush::setColor(QColor *arg0)
{
  AQ_CALL_VOID(setColor(*arg0));
}
inline void AQSBrush::setColor(AQSColor *arg0)
{
  AQ_CALL_VOID(setColor(*arg0));
}
inline QPixmap *AQSBrush::pixmap() const
{
  AQ_CALL_RET(pixmap());
}
inline void AQSBrush::setPixmap(QPixmap *arg0)
{
  AQ_CALL_VOID(setPixmap(*arg0));
}
inline void AQSBrush::setPixmap(AQSPixmap *arg0)
{
  AQ_CALL_VOID(setPixmap(*arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSBRUSH_P_H_ */
// @AQSVOIDPTR@
