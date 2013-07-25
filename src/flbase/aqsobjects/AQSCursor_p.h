/***************************************************************************
 AQSCursor_p.h
 -------------------
 begin                : 25/03/2013
 copyright            : (C) 2003-2013 by InfoSiAL S.L.
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

#ifndef AQSCURSOR_P_H_
#define AQSCURSOR_P_H_

#include "AQSVoidPtr_p.h"
#include <qcursor.h>

// @AQ_PTR_INDIRECTION@

class AQSCursor : public AQSVoidPtr
{
  Q_OBJECT

  Q_PROPERTY(QCursor toVariant READ toVariant);

  AQ_DECLARE_AQS_VOID(Cursor, VoidPtr);

public:
  QCursor toVariant() const {
    if (!o_)
      return QCursor();
    return *o_;
  }

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int shape() const;
  void setShape(int);
  const QBitmap *bitmap() const;
  const QBitmap *mask() const;
  QPoint hotSpot() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("int"));
    candidates[2].append(QString::fromLatin1("QBitmap,QBitmap"));
    candidates[3].append(QString::fromLatin1("QBitmap,QBitmap,int"));
    candidates[4].append(QString::fromLatin1("QBitmap,QBitmap,int,int"));
    candidates[1].append(QString::fromLatin1("QPixmap*"));
    candidates[2].append(QString::fromLatin1("QPixmap*,int"));
    candidates[3].append(QString::fromLatin1("QPixmap*,int,int"));
    candidates[1].append(QString::fromLatin1("QCursor*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QCursor;
    if (sgt == QString::fromLatin1("int"))
      return new QCursor(args[0].variant().toInt());
    if (sgt == QString::fromLatin1("QBitmap,QBitmap"))
      return new QCursor(*(argValue<QBitmap *>(args[0])),
                         *(argValue<QBitmap *>(args[1])));
    if (sgt == QString::fromLatin1("QBitmap,QBitmap,int"))
      return new QCursor(*(argValue<QBitmap *>(args[0])),
                         *(argValue<QBitmap *>(args[1])),
                         args[2].variant().toInt());
    if (sgt == QString::fromLatin1("QBitmap,QBitmap,int,int"))
      return new QCursor(*(argValue<QBitmap *>(args[0])),
                         *(argValue<QBitmap *>(args[1])),
                         args[2].variant().toInt(),
                         args[3].variant().toInt());
    if (sgt == QString::fromLatin1("QPixmap*"))
      return new QCursor(*argValue<QPixmap *>(args[0]));
    if (sgt == QString::fromLatin1("QPixmap*,int"))
      return new QCursor(*argValue<QPixmap *>(args[0]),
                         args[1].variant().toInt());
    if (sgt == QString::fromLatin1("QPixmap*,int,int"))
      return new QCursor(*argValue<QPixmap *>(args[0]),
                         args[1].variant().toInt(),
                         args[2].variant().toInt());
    if (sgt == QString::fromLatin1("QCursor*"))
      return new QCursor(*argValue<QCursor *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("int"));
    candidates[2].append(QString::fromLatin1("QBitmap,QBitmap"));
    candidates[3].append(QString::fromLatin1("QBitmap,QBitmap,int"));
    candidates[4].append(QString::fromLatin1("QBitmap,QBitmap,int,int"));
    candidates[1].append(QString::fromLatin1("QPixmap*"));
    candidates[2].append(QString::fromLatin1("QPixmap*,int"));
    candidates[3].append(QString::fromLatin1("QPixmap*,int,int"));
    candidates[1].append(QString::fromLatin1("QCursor*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSCursor::shape() const
{
  AQ_CALL_RET_V(shape(), int);
}
inline void AQSCursor::setShape(int arg0)
{
  AQ_CALL_VOID(setShape(arg0));
}
inline const QBitmap *AQSCursor::bitmap() const
{
  AQ_CALL_RET(bitmap());
}
inline const QBitmap *AQSCursor::mask() const
{
  AQ_CALL_RET(mask());
}
inline QPoint AQSCursor::hotSpot() const
{
  AQ_CALL_RET_V(hotSpot(), QPoint);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCURSOR_P_H_ */
// @AQSVOIDPTR@
