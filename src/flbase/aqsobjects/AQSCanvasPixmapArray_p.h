/***************************************************************************
 AQSCanvasPixmapArray_p.h
 -------------------
 begin                : 06/10/2011
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

#ifndef AQSCANVASPIXMAPARRAY_P_H_
#define AQSCANVASPIXMAPARRAY_P_H_

#include "AQSCanvasPixmap_p.h"

class AQSCanvasPixmapArray : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(CanvasPixmapArray, VoidPtr);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool readPixmaps(const QString &, int = 0);
  bool readCollisionMasks(const QString &);
  bool isValid() const;
  QCanvasPixmap *image(int) const;
  void setImage(int, QCanvasPixmap *);
  void setImage(int, AQSCanvasPixmap *);
  uint count() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,int"));
    candidates[1].append(QString::fromLatin1("QValueList<QPixmap>"));
    candidates[2].append(QString::fromLatin1("QValueList<QPixmap>,QPointArray*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QCanvasPixmapArray;
    if (sgt == QString::fromLatin1("QString"))
      return new QCanvasPixmapArray(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,int"))
      return new QCanvasPixmapArray(*(argValue<QString *>(args[0])),
                                    args[1].variant().toInt());
    if (sgt == QString::fromLatin1("QValueList<QPixmap>"))
      return new QCanvasPixmapArray(*(argValue<QValueList<QPixmap>*>(args[0])));
    if (sgt == QString::fromLatin1("QValueList<QPixmap>,QPointArray*"))
      return new QCanvasPixmapArray(*(argValue<QValueList<QPixmap>*>(args[0])),
                                    *argValue<QPointArray *>(args[1]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,int"));
    candidates[1].append(QString::fromLatin1("QValueList<QPixmap>"));
    candidates[2].append(QString::fromLatin1("QValueList<QPixmap>,QPointArray*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSCanvasPixmapArray::readPixmaps(const QString &arg0,  int arg1)
{
  AQ_CALL_RET_V(readPixmaps(arg0, arg1), bool);
}
inline bool AQSCanvasPixmapArray::readCollisionMasks(const QString &arg0)
{
  AQ_CALL_RET_V(readCollisionMasks(arg0), bool);
}
inline bool AQSCanvasPixmapArray::isValid() const
{
  AQ_CALL_RET_V(isValid(), bool);
}
inline QCanvasPixmap *AQSCanvasPixmapArray::image(int arg0) const
{
  AQ_CALL_RET(image(arg0));
}
inline void AQSCanvasPixmapArray::setImage(int arg0,  QCanvasPixmap *arg1)
{
  AQ_CALL_VOID(setImage(arg0, arg1));
}
inline void AQSCanvasPixmapArray::setImage(int arg0, AQSCanvasPixmap *arg1)
{
  AQ_CALL_VOID(setImage(arg0, *arg1));
}
inline uint AQSCanvasPixmapArray::count() const
{
  AQ_CALL_RET_V(count(), uint);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCANVASPIXMAPARRAY_P_H_ */
// @AQSVOIDPTR@
