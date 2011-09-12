/***************************************************************************
 AQSImage_p.h
 -------------------
 begin                : 22/02/2011
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

#ifndef AQSIMAGE_P_H_
#define AQSIMAGE_P_H_

#include "AQSWMatrix_p.h"
#include "AQSByteArray_p.h"
#include "AQSIODevice_p.h"
#include <qimage.h>

// @AQ_PTR_INDIRECTION@

class AQSImage : public AQSVoidPtr
{
  Q_OBJECT

  Q_PROPERTY(QImage toVariant READ toVariant);

  AQ_DECLARE_AQS_VOID(Image, VoidPtr);

public:
  QImage toVariant() const {
    if (!o_)
      return QImage();
    return *o_;
  }

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QImage *copy() const;
  QImage *copy(int, int, int, int, int = 0) const;
  QImage *copy(const QRect &) const;
  bool isNull() const;
  int width() const;
  int height() const;
  QSize size() const;
  QRect rect() const;
  int depth() const;
  int numColors() const;
  uint bitOrder() const;
  QRgb color(int) const;
  void setColor(int, QRgb);
  void setNumColors(int);
  bool hasAlphaBuffer() const;
  void setAlphaBuffer(bool);
  bool allGray() const;
  bool isGrayscale() const;
  int numBytes() const;
  int bytesPerLine() const;
  bool create(int, int, int, int = 0, uint = QImage::IgnoreEndian);
  bool create(const QSize &, int, int = 0, uint = QImage::IgnoreEndian);
  void reset();
  void fill(uint);
  void invertPixels(bool = TRUE);
  QImage *convertDepth(int) const;
  QImage *convertDepthWithPalette(int, QRgb *, int, int = 0) const;
  QImage *convertDepth(int, int) const;
  QImage *convertBitOrder(uint) const;
  QImage *smoothScale(int, int, uint = QImage::ScaleFree) const;
  QImage *smoothScale(const QSize &, uint = QImage::ScaleFree) const;
  QImage *scale(int, int, uint = QImage::ScaleFree) const;
  QImage *scale(const QSize &, uint = QImage::ScaleFree) const;
  QImage *scaleWidth(int) const;
  QImage *scaleHeight(int) const;
  QImage *xForm(QWMatrix *) const;
  QImage *xForm(AQSWMatrix *) const;
  QImage *createAlphaMask(int = 0) const;
  QImage *createHeuristicMask(bool = TRUE) const;
  QImage *mirror() const;
  QImage *mirror(bool, bool) const;
  QImage *swapRGB() const;
  bool loadFromData(QByteArray *, const char* = 0);
  bool loadFromData(AQSByteArray *, const char* = 0);
  bool save(QIODevice *, const char *, int = -1) const;
  bool save(AQSIODevice *, const char *, int = -1) const;
  bool load(const QString &, const char* = 0);
  bool save(const QString &, const char *, int = -1) const;
  bool valid(int, int) const;
  int pixelIndex(int, int) const;
  QRgb pixel(int, int) const;
  void setPixel(int, int, uint);
  int dotsPerMeterX() const;
  int dotsPerMeterY() const;
  void setDotsPerMeterX(int);
  void setDotsPerMeterY(int);
  QPoint offset() const;
  void setOffset(const QPoint &);
  QStringList textLanguages() const;
  QStringList textKeys() const;
  QString text(const char *, const char* = 0) const;
  void setText(const char *, const char *, const QString &);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[3].append(QString::fromLatin1("int,int,int"));
    candidates[4].append(QString::fromLatin1("int,int,int,int"));
    candidates[5].append(QString::fromLatin1("int,int,int,int,QImage::Endian"));
    candidates[2].append(QString::fromLatin1("QSize,int"));
    candidates[3].append(QString::fromLatin1("QSize,int,int"));
    candidates[4].append(QString::fromLatin1("QSize,int,int,QImage::Endian"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QString"));
    candidates[1].append(QString::fromLatin1("QByteArray*"));
    candidates[1].append(QString::fromLatin1("QImage*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QImage;
    if (sgt == QString::fromLatin1("int,int,int"))
      return new QImage(args[0].variant().toInt(),
                        args[1].variant().toInt(),
                        args[2].variant().toInt());
    if (sgt == QString::fromLatin1("int,int,int,int"))
      return new QImage(args[0].variant().toInt(),
                        args[1].variant().toInt(),
                        args[2].variant().toInt(),
                        args[3].variant().toInt());
    if (sgt == QString::fromLatin1("int,int,int,int,QImage::Endian"))
      return new QImage(args[0].variant().toInt(),
                        args[1].variant().toInt(),
                        args[2].variant().toInt(),
                        args[3].variant().toInt(),
                        static_cast<QImage::Endian>(args[4].variant().toUInt()));
    if (sgt == QString::fromLatin1("QSize,int"))
      return new QImage(*(argValue<QSize *>(args[0])),
                        args[1].variant().toInt());
    if (sgt == QString::fromLatin1("QSize,int,int"))
      return new QImage(*(argValue<QSize *>(args[0])),
                        args[1].variant().toInt(),
                        args[2].variant().toInt());
    if (sgt == QString::fromLatin1("QSize,int,int,QImage::Endian"))
      return new QImage(*(argValue<QSize *>(args[0])),
                        args[1].variant().toInt(),
                        args[2].variant().toInt(),
                        static_cast<QImage::Endian>(args[3].variant().toUInt()));
    if (sgt == QString::fromLatin1("QString"))
      return new QImage(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,QString"))
      return new QImage(*(argValue<QString *>(args[0])),
                        *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QByteArray*"))
      return new QImage(*argValue<QByteArray *>(args[0]));
    if (sgt == QString::fromLatin1("QImage*"))
      return new QImage(*argValue<QImage *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[3].append(QString::fromLatin1("int,int,int"));
    candidates[4].append(QString::fromLatin1("int,int,int,int"));
    candidates[5].append(QString::fromLatin1("int,int,int,int,QImage::Endian"));
    candidates[2].append(QString::fromLatin1("QSize,int"));
    candidates[3].append(QString::fromLatin1("QSize,int,int"));
    candidates[4].append(QString::fromLatin1("QSize,int,int,QImage::Endian"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QString"));
    candidates[1].append(QString::fromLatin1("QByteArray*"));
    candidates[1].append(QString::fromLatin1("QImage*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QImage *AQSImage::copy() const
{
  AQ_CALL_RET_PTR(copy(), QImage);
}
inline QImage *AQSImage::copy(int arg0,  int arg1,  int arg2,  int arg3,  int arg4) const
{
  AQ_CALL_RET_PTR(copy(arg0, arg1, arg2, arg3, arg4), QImage);
}
inline QImage *AQSImage::copy(const QRect &arg0) const
{
  AQ_CALL_RET_PTR(copy(arg0), QImage);
}
inline bool AQSImage::isNull() const
{
  AQ_CALL_RET_V(isNull(), bool);
}
inline int AQSImage::width() const
{
  AQ_CALL_RET_V(width(), int);
}
inline int AQSImage::height() const
{
  AQ_CALL_RET_V(height(), int);
}
inline QSize AQSImage::size() const
{
  AQ_CALL_RET_V(size(), QSize);
}
inline QRect AQSImage::rect() const
{
  AQ_CALL_RET_V(rect(), QRect);
}
inline int AQSImage::depth() const
{
  AQ_CALL_RET_V(depth(), int);
}
inline int AQSImage::numColors() const
{
  AQ_CALL_RET_V(numColors(), int);
}
inline uint AQSImage::bitOrder() const
{
  AQ_CALL_RET_V(bitOrder(), uint);
}
inline QRgb AQSImage::color(int arg0) const
{
  AQ_CALL_RET_V(color(arg0), QRgb);
}
inline void AQSImage::setColor(int arg0,  QRgb arg1)
{
  AQ_CALL_VOID(setColor(arg0, arg1));
}
inline void AQSImage::setNumColors(int arg0)
{
  AQ_CALL_VOID(setNumColors(arg0));
}
inline bool AQSImage::hasAlphaBuffer() const
{
  AQ_CALL_RET_V(hasAlphaBuffer(), bool);
}
inline void AQSImage::setAlphaBuffer(bool arg0)
{
  AQ_CALL_VOID(setAlphaBuffer(arg0));
}
inline bool AQSImage::allGray() const
{
  AQ_CALL_RET_V(allGray(), bool);
}
inline bool AQSImage::isGrayscale() const
{
  AQ_CALL_RET_V(isGrayscale(), bool);
}
inline int AQSImage::numBytes() const
{
  AQ_CALL_RET_V(numBytes(), int);
}
inline int AQSImage::bytesPerLine() const
{
  AQ_CALL_RET_V(bytesPerLine(), int);
}
inline bool AQSImage::create(int arg0,  int arg1,  int arg2,  int arg3,  uint arg4)
{
  AQ_CALL_RET_V(create(arg0, arg1, arg2, arg3, static_cast<QImage::Endian>(arg4)), bool);
}
inline bool AQSImage::create(const QSize &arg0,  int arg1,  int arg2,  uint arg3)
{
  AQ_CALL_RET_V(create(arg0, arg1, arg2, static_cast<QImage::Endian>(arg3)), bool);
}
inline void AQSImage::reset()
{
  AQ_CALL_VOID(reset());
}
inline void AQSImage::fill(uint arg0)
{
  AQ_CALL_VOID(fill(arg0));
}
inline void AQSImage::invertPixels(bool arg0)
{
  AQ_CALL_VOID(invertPixels(arg0));
}
inline QImage *AQSImage::convertDepth(int arg0) const
{
  AQ_CALL_RET_PTR(convertDepth(arg0), QImage);
}
inline QImage *AQSImage::convertDepthWithPalette(int arg0,  QRgb *arg1,  int arg2,  int arg3) const
{
  AQ_CALL_RET_PTR(convertDepthWithPalette(arg0, arg1, arg2, arg3), QImage);
}
inline QImage *AQSImage::convertDepth(int arg0,  int arg1) const
{
  AQ_CALL_RET_PTR(convertDepth(arg0, arg1), QImage);
}
inline QImage *AQSImage::convertBitOrder(uint arg0) const
{
  AQ_CALL_RET_PTR(convertBitOrder(static_cast<QImage::Endian>(arg0)), QImage);
}
inline QImage *AQSImage::smoothScale(int arg0,  int arg1,  uint arg2) const
{
  AQ_CALL_RET_PTR(smoothScale(arg0, arg1, static_cast<QImage::ScaleMode>(arg2)), QImage);
}
inline QImage *AQSImage::smoothScale(const QSize &arg0,  uint arg1) const
{
  AQ_CALL_RET_PTR(smoothScale(arg0, static_cast<QImage::ScaleMode>(arg1)), QImage);
}
inline QImage *AQSImage::scale(int arg0,  int arg1,  uint arg2) const
{
  AQ_CALL_RET_PTR(scale(arg0, arg1, static_cast<QImage::ScaleMode>(arg2)), QImage);
}
inline QImage *AQSImage::scale(const QSize &arg0,  uint arg1) const
{
  AQ_CALL_RET_PTR(scale(arg0, static_cast<QImage::ScaleMode>(arg1)), QImage);
}
inline QImage *AQSImage::scaleWidth(int arg0) const
{
  AQ_CALL_RET_PTR(scaleWidth(arg0), QImage);
}
inline QImage *AQSImage::scaleHeight(int arg0) const
{
  AQ_CALL_RET_PTR(scaleHeight(arg0), QImage);
}
inline QImage *AQSImage::xForm(QWMatrix *arg0) const
{
  AQ_CALL_RET_PTR(xForm(*arg0), QImage);
}
inline QImage *AQSImage::xForm(AQSWMatrix *arg0) const
{
  AQ_CALL_RET_PTR(xForm(*arg0), QImage);
}
inline QImage *AQSImage::createAlphaMask(int arg0) const
{
  AQ_CALL_RET_PTR(createAlphaMask(arg0), QImage);
}
inline QImage *AQSImage::createHeuristicMask(bool arg0) const
{
  AQ_CALL_RET_PTR(createHeuristicMask(arg0), QImage);
}
inline QImage *AQSImage::mirror() const
{
  AQ_CALL_RET_PTR(mirror(), QImage);
}
inline QImage *AQSImage::mirror(bool arg0,  bool arg1) const
{
  AQ_CALL_RET_PTR(mirror(arg0, arg1), QImage);
}
inline QImage *AQSImage::swapRGB() const
{
  AQ_CALL_RET_PTR(swapRGB(), QImage);
}
inline bool AQSImage::loadFromData(QByteArray *arg0,  const char *arg1)
{
  AQ_CALL_RET_V(loadFromData(*arg0, arg1), bool);
}
inline bool AQSImage::loadFromData(AQSByteArray *arg0,  const char *arg1)
{
  AQ_CALL_RET_V(loadFromData(*arg0, arg1), bool);
}
inline bool AQSImage::save(QIODevice *arg0,  const char *arg1,  int arg2) const
{
  AQ_CALL_RET_V(save(arg0, arg1, arg2), bool);
}
inline bool AQSImage::save(AQSIODevice *arg0,  const char *arg1,  int arg2) const
{
  AQ_CALL_RET_V(save(*arg0, arg1, arg2), bool);
}
inline bool AQSImage::load(const QString &arg0,  const char *arg1)
{
  AQ_CALL_RET_V(load(arg0, arg1), bool);
}
inline bool AQSImage::save(const QString &arg0,  const char *arg1,  int arg2) const
{
  AQ_CALL_RET_V(save(arg0, arg1, arg2), bool);
}
inline bool AQSImage::valid(int arg0,  int arg1) const
{
  AQ_CALL_RET_V(valid(arg0, arg1), bool);
}
inline int AQSImage::pixelIndex(int arg0,  int arg1) const
{
  AQ_CALL_RET_V(pixelIndex(arg0, arg1), int);
}
inline QRgb AQSImage::pixel(int arg0,  int arg1) const
{
  AQ_CALL_RET_V(pixel(arg0, arg1), QRgb);
}
inline void AQSImage::setPixel(int arg0,  int arg1,  uint arg2)
{
  AQ_CALL_VOID(setPixel(arg0, arg1, arg2));
}
inline int AQSImage::dotsPerMeterX() const
{
  AQ_CALL_RET_V(dotsPerMeterX(), int);
}
inline int AQSImage::dotsPerMeterY() const
{
  AQ_CALL_RET_V(dotsPerMeterY(), int);
}
inline void AQSImage::setDotsPerMeterX(int arg0)
{
  AQ_CALL_VOID(setDotsPerMeterX(arg0));
}
inline void AQSImage::setDotsPerMeterY(int arg0)
{
  AQ_CALL_VOID(setDotsPerMeterY(arg0));
}
inline QPoint AQSImage::offset() const
{
  AQ_CALL_RET_V(offset(), QPoint);
}
inline void AQSImage::setOffset(const QPoint &arg0)
{
  AQ_CALL_VOID(setOffset(arg0));
}
inline QStringList AQSImage::textLanguages() const
{
  AQ_CALL_RET_V(textLanguages(), QStringList);
}
inline QStringList AQSImage::textKeys() const
{
  AQ_CALL_RET_V(textKeys(), QStringList);
}
inline QString AQSImage::text(const char *arg0,  const char *arg1) const
{
  AQ_CALL_RET_V(text(arg0, arg1), QString);
}
inline void AQSImage::setText(const char *arg0,  const char *arg1,  const QString &arg2)
{
  AQ_CALL_VOID(setText(arg0, arg1, arg2));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSIMAGE_P_H_ */
// @AQSVOIDPTR@
