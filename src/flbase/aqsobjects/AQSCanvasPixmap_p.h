/***************************************************************************
 AQSCanvasPixmap_p.h
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

#ifndef AQSCANVASPIXMAP_P_H_
#define AQSCANVASPIXMAP_P_H_

#include "AQSPixmap_p.h"
#include <qcanvas.h>

class AQSCanvasPixmap : public AQSPixmap
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(CanvasPixmap, Pixmap);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  int offsetX() const;
  int offsetY() const;
  void setOffset(int, int);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[1].append(QString::fromLatin1("QImage*"));
    candidates[2].append(QString::fromLatin1("QPixmap*,QPoint"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QString"))
      return new QCanvasPixmap(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QImage*"))
      return new QCanvasPixmap(*argValue<QImage *>(args[0]));
    if (sgt == QString::fromLatin1("QPixmap*,QPoint"))
      return new QCanvasPixmap(*argValue<QPixmap *>(args[0]),
                               *(argValue<QPoint *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[1].append(QString::fromLatin1("QImage*"));
    candidates[2].append(QString::fromLatin1("QPixmap*,QPoint"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline int AQSCanvasPixmap::offsetX() const
{
  AQ_CALL_RET_V(offsetX(), int);
}
inline int AQSCanvasPixmap::offsetY() const
{
  AQ_CALL_RET_V(offsetY(), int);
}
inline void AQSCanvasPixmap::setOffset(int arg0,  int arg1)
{
  AQ_CALL_VOID(setOffset(arg0, arg1));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCANVASPIXMAP_P_H_ */
// @AQSVOIDPTR@
