/***************************************************************************
 AQSPen_p.h
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

#ifndef AQSPEN_P_H_
#define AQSPEN_P_H_

#include "AQSColor_p.h"
#include <qpen.h>

// @AQ_PTR_INDIRECTION@

class AQSPen : public AQSVoidPtr
{
  Q_OBJECT

  Q_PROPERTY(QPen toVariant READ toVariant);

  AQ_DECLARE_AQS_VOID(Pen, VoidPtr);

public:
  QPen toVariant() const {
    if (!o_)
      return QPen();
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
  void setStyle(uint);
  uint width() const;
  void setWidth(uint);
  QColor *color() const;
  void setColor(QColor *);
  void setColor(AQSColor *);
  uint capStyle() const;
  void setCapStyle(uint);
  uint joinStyle() const;
  void setJoinStyle(uint);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("Qt::PenStyle"));
    candidates[1].append(QString::fromLatin1("QColor*"));
    candidates[2].append(QString::fromLatin1("QColor*,uint"));
    candidates[3].append(QString::fromLatin1("QColor*,uint,Qt::PenStyle"));
    candidates[5].append(QString::fromLatin1("QColor*,uint,Qt::PenStyle,Qt::PenCapStyle,Qt::PenJoinStyle"));
    candidates[1].append(QString::fromLatin1("QPen*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QPen;
    if (sgt == QString::fromLatin1("Qt::PenStyle"))
      return new QPen(static_cast<Qt::PenStyle>(args[0].variant().toUInt()));
    if (sgt == QString::fromLatin1("QColor*"))
      return new QPen(*argValue<QColor *>(args[0]));
    if (sgt == QString::fromLatin1("QColor*,uint"))
      return new QPen(*argValue<QColor *>(args[0]),
                      args[1].variant().toUInt());
    if (sgt == QString::fromLatin1("QColor*,uint,Qt::PenStyle"))
      return new QPen(*argValue<QColor *>(args[0]),
                      args[1].variant().toUInt(),
                      static_cast<Qt::PenStyle>(args[2].variant().toUInt()));
    if (sgt == QString::fromLatin1("QColor*,uint,Qt::PenStyle,Qt::PenCapStyle,Qt::PenJoinStyle"))
      return new QPen(*argValue<QColor *>(args[0]),
                      args[1].variant().toUInt(),
                      static_cast<Qt::PenStyle>(args[2].variant().toUInt()),
                      static_cast<Qt::PenCapStyle>(args[3].variant().toUInt()),
                      static_cast<Qt::PenJoinStyle>(args[4].variant().toUInt()));
    if (sgt == QString::fromLatin1("QPen*"))
      return new QPen(*argValue<QPen *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("Qt::PenStyle"));
    candidates[1].append(QString::fromLatin1("QColor*"));
    candidates[2].append(QString::fromLatin1("QColor*,uint"));
    candidates[3].append(QString::fromLatin1("QColor*,uint,Qt::PenStyle"));
    candidates[5].append(QString::fromLatin1("QColor*,uint,Qt::PenStyle,Qt::PenCapStyle,Qt::PenJoinStyle"));
    candidates[1].append(QString::fromLatin1("QPen*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline uint AQSPen::style() const
{
  AQ_CALL_RET_V(style(), uint);
}
inline void AQSPen::setStyle(uint arg0)
{
  AQ_CALL_VOID(setStyle(static_cast<Qt::PenStyle>(arg0)));
}
inline uint AQSPen::width() const
{
  AQ_CALL_RET_V(width(), uint);
}
inline void AQSPen::setWidth(uint arg0)
{
  AQ_CALL_VOID(setWidth(arg0));
}
inline QColor *AQSPen::color() const
{
  AQ_CALL_RET_PTR(color(), QColor);
}
inline void AQSPen::setColor(QColor *arg0)
{
  AQ_CALL_VOID(setColor(*arg0));
}
inline void AQSPen::setColor(AQSColor *arg0)
{
  AQ_CALL_VOID(setColor(*arg0));
}
inline uint AQSPen::capStyle() const
{
  AQ_CALL_RET_V(capStyle(), uint);
}
inline void AQSPen::setCapStyle(uint arg0)
{
  AQ_CALL_VOID(setCapStyle(static_cast<Qt::PenCapStyle>(arg0)));
}
inline uint AQSPen::joinStyle() const
{
  AQ_CALL_RET_V(joinStyle(), uint);
}
inline void AQSPen::setJoinStyle(uint arg0)
{
  AQ_CALL_VOID(setJoinStyle(static_cast<Qt::PenJoinStyle>(arg0)));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSPEN_P_H_ */
// @AQSVOIDPTR@
