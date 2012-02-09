/***************************************************************************
 AQSCanvasSprite_p.h
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

#ifndef AQSCANVASSPRITE_P_H_
#define AQSCANVASSPRITE_P_H_

#include "AQSCanvasItem_p.h"
#include "AQSCanvasPixmapArray_p.h"

class AQSCanvasSprite : public AQSCanvasItem
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(CanvasSprite, CanvasItem);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setSequence(QCanvasPixmapArray *);
  void setSequence(AQSCanvasPixmapArray *);
  virtual void move(double, double, int);
  void setFrame(int);
  virtual void setFrameAnimation(uint = QCanvasSprite::Cycle, int = 1, int = 0);
  int frame() const;
  int frameCount() const;
  virtual int rtti() const;
  virtual QRect boundingRect() const;
  int width() const;
  int height() const;
  int leftEdge() const;
  int topEdge() const;
  int rightEdge() const;
  int bottomEdge() const;
  int leftEdge(int) const;
  int topEdge(int) const;
  int rightEdge(int) const;
  int bottomEdge(int) const;
  QCanvasPixmap *image() const;
  virtual QCanvasPixmap *imageAdvanced() const;
  QCanvasPixmap *image(int) const;
  virtual void advance(int);
  virtual void draw(QPainter &);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QCanvasPixmapArray*,QCanvas*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QCanvasPixmapArray*,QCanvas*"))
      return new QCanvasSprite(argValue<QCanvasPixmapArray *>(args[0]),
                               argValue<QCanvas *>(args[1]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QCanvasPixmapArray*,QCanvas*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSCanvasSprite::setSequence(QCanvasPixmapArray *arg0)
{
  AQ_CALL_VOID(setSequence(arg0));
}
inline void AQSCanvasSprite::setSequence(AQSCanvasPixmapArray *arg0)
{
  AQ_CALL_VOID(setSequence(*arg0));
}
inline void AQSCanvasSprite::move(double arg0,  double arg1,  int arg2)
{
  AQ_CALL_VOID(move(arg0, arg1, arg2));
}
inline void AQSCanvasSprite::setFrame(int arg0)
{
  AQ_CALL_VOID(setFrame(arg0));
}
inline void AQSCanvasSprite::setFrameAnimation(uint arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(setFrameAnimation(static_cast<QCanvasSprite::FrameAnimationType>(arg0), arg1, arg2));
}
inline int AQSCanvasSprite::frame() const
{
  AQ_CALL_RET_V(frame(), int);
}
inline int AQSCanvasSprite::frameCount() const
{
  AQ_CALL_RET_V(frameCount(), int);
}
inline int AQSCanvasSprite::rtti() const
{
  AQ_CALL_RET_V(rtti(), int);
}
inline QRect AQSCanvasSprite::boundingRect() const
{
  AQ_CALL_RET_V(boundingRect(), QRect);
}
inline int AQSCanvasSprite::width() const
{
  AQ_CALL_RET_V(width(), int);
}
inline int AQSCanvasSprite::height() const
{
  AQ_CALL_RET_V(height(), int);
}
inline int AQSCanvasSprite::leftEdge() const
{
  AQ_CALL_RET_V(leftEdge(), int);
}
inline int AQSCanvasSprite::topEdge() const
{
  AQ_CALL_RET_V(topEdge(), int);
}
inline int AQSCanvasSprite::rightEdge() const
{
  AQ_CALL_RET_V(rightEdge(), int);
}
inline int AQSCanvasSprite::bottomEdge() const
{
  AQ_CALL_RET_V(bottomEdge(), int);
}
inline int AQSCanvasSprite::leftEdge(int arg0) const
{
  AQ_CALL_RET_V(leftEdge(arg0), int);
}
inline int AQSCanvasSprite::topEdge(int arg0) const
{
  AQ_CALL_RET_V(topEdge(arg0), int);
}
inline int AQSCanvasSprite::rightEdge(int arg0) const
{
  AQ_CALL_RET_V(rightEdge(arg0), int);
}
inline int AQSCanvasSprite::bottomEdge(int arg0) const
{
  AQ_CALL_RET_V(bottomEdge(arg0), int);
}
inline QCanvasPixmap *AQSCanvasSprite::image() const
{
  AQ_CALL_RET(image());
}
inline QCanvasPixmap *AQSCanvasSprite::imageAdvanced() const
{
  AQ_CALL_RET(imageAdvanced());
}
inline QCanvasPixmap *AQSCanvasSprite::image(int arg0) const
{
  AQ_CALL_RET(image(arg0));
}
inline void AQSCanvasSprite::advance(int arg0)
{
  AQ_CALL_VOID(advance(arg0));
}
inline void AQSCanvasSprite::draw(QPainter &arg0)
{
  AQ_CALL_VOID(draw(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCANVASSPRITE_P_H_ */
// @AQSVOIDPTR@
