/***************************************************************************
 AQSPixmap_p.h
 -------------------
 begin                : 14/02/2011
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

#ifndef AQSPIXMAP_P_H_
#define AQSPIXMAP_P_H_

#include "AQSColor_p.h"
#include "AQSImage_p.h"
#include <qpixmap.h>

// @AQ_PTR_INDIRECTION@

class AQSPixmap : public AQSVoidPtr
{
  Q_OBJECT

  Q_PROPERTY(int width READ width)
  Q_PROPERTY(int height READ height)
  Q_PROPERTY(QSize size READ size)
  Q_PROPERTY(QRect rect READ rect)
  Q_PROPERTY(int depth READ depth)
  Q_PROPERTY(QPixmap toVariant READ toVariant);

  AQ_DECLARE_AQS_VOID(Pixmap, VoidPtr);

public:
  QPixmap toVariant() const {
    if (!o_)
      return QPixmap();
    return *o_;
  }

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

public slots:
  QPaintDevice *paintDevice() const {
    return static_cast<QPaintDevice *>(o_);
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool isNull() const;
  int width() const;
  int height() const;
  QSize size() const;
  QRect rect() const;
  int depth() const;
  void fill(const QColor& = Qt::white);
  void fill(QColor *);
  void fill(AQSColor *);
  void fill(const QWidget *, int, int);
  void fill(const QWidget *, const QPoint &);
  void resize(int, int);
  void resize(const QSize &);
  bool selfMask() const;
  bool hasAlpha() const;
  bool hasAlphaChannel() const;
  QPixmap *xForm(QWMatrix *) const;
  QPixmap *xForm(AQSWMatrix *) const;
  QImage *convertToImage() const;
  bool convertFromImage(QImage *, int = 0);
  bool convertFromImage(AQSImage *, int = 0);
  bool loadFromData(QByteArray *, const char* = 0, int = 0);
  bool loadFromData(AQSByteArray *, const char* = 0, int = 0);
  bool save(QIODevice *, const char *, int = -1) const;
  bool save(AQSIODevice *, const char *, int = -1) const;
  bool load(const QString &, const char *, int);
  bool save(const QString &, const char *, int = -1) const;
  int serialNumber() const;
  void setOptimization(uint);
  bool isQBitmap() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[2].append(QString::fromLatin1("int,int"));
    candidates[3].append(QString::fromLatin1("int,int,int"));
    candidates[4].append(QString::fromLatin1("int,int,int,QPixmap::Optimization"));
    candidates[1].append(QString::fromLatin1("QSize"));
    candidates[2].append(QString::fromLatin1("QSize,int"));
    candidates[3].append(QString::fromLatin1("QSize,int,QPixmap::Optimization"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QString"));
    candidates[3].append(QString::fromLatin1("QString,QString,QPixmap::ColorMode"));
    candidates[3].append(QString::fromLatin1("QString,QString,int"));
    candidates[1].append(QString::fromLatin1("QByteArray*"));
    candidates[1].append(QString::fromLatin1("QPixmap*"));
    candidates[1].append(QString::fromLatin1("QImage*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QPixmap;
    if (sgt == QString::fromLatin1("int,int"))
      return new QPixmap(args[0].variant().toInt(),
                         args[1].variant().toInt());
    if (sgt == QString::fromLatin1("int,int,int"))
      return new QPixmap(args[0].variant().toInt(),
                         args[1].variant().toInt(),
                         args[2].variant().toInt());
    if (sgt == QString::fromLatin1("int,int,int,QPixmap::Optimization"))
      return new QPixmap(args[0].variant().toInt(),
                         args[1].variant().toInt(),
                         args[2].variant().toInt(),
                         static_cast<QPixmap::Optimization>(args[3].variant().toUInt()));
    if (sgt == QString::fromLatin1("QSize"))
      return new QPixmap(*(argValue<QSize *>(args[0])));
    if (sgt == QString::fromLatin1("QSize,int"))
      return new QPixmap(*(argValue<QSize *>(args[0])),
                         args[1].variant().toInt());
    if (sgt == QString::fromLatin1("QSize,int,QPixmap::Optimization"))
      return new QPixmap(*(argValue<QSize *>(args[0])),
                         args[1].variant().toInt(),
                         static_cast<QPixmap::Optimization>(args[2].variant().toUInt()));
    if (sgt == QString::fromLatin1("QString"))
      return new QPixmap(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,QString"))
      return new QPixmap(*(argValue<QString *>(args[0])),
                         *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QString,QString,QPixmap::ColorMode"))
      return new QPixmap(*(argValue<QString *>(args[0])),
                         *(argValue<QString *>(args[1])),
                         static_cast<QPixmap::ColorMode>(args[2].variant().toUInt()));
    if (sgt == QString::fromLatin1("QString,QString,int"))
      return new QPixmap(*(argValue<QString *>(args[0])),
                         *(argValue<QString *>(args[1])),
                         args[2].variant().toInt());
    if (sgt == QString::fromLatin1("QByteArray*"))
      return new QPixmap(*argValue<QByteArray *>(args[0]));
    if (sgt == QString::fromLatin1("QPixmap*"))
      return new QPixmap(*argValue<QPixmap *>(args[0]));
    if (sgt == QString::fromLatin1("QImage*"))
      return new QPixmap(*argValue<QImage *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[2].append(QString::fromLatin1("int,int"));
    candidates[3].append(QString::fromLatin1("int,int,int"));
    candidates[4].append(QString::fromLatin1("int,int,int,QPixmap::Optimization"));
    candidates[1].append(QString::fromLatin1("QSize"));
    candidates[2].append(QString::fromLatin1("QSize,int"));
    candidates[3].append(QString::fromLatin1("QSize,int,QPixmap::Optimization"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QString"));
    candidates[3].append(QString::fromLatin1("QString,QString,QPixmap::ColorMode"));
    candidates[3].append(QString::fromLatin1("QString,QString,int"));
    candidates[1].append(QString::fromLatin1("QByteArray*"));
    candidates[1].append(QString::fromLatin1("QPixmap*"));
    candidates[1].append(QString::fromLatin1("QImage*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSPixmap::isNull() const
{
  AQ_CALL_RET_V(isNull(), bool);
}
inline int AQSPixmap::width() const
{
  AQ_CALL_RET_V(width(), int);
}
inline int AQSPixmap::height() const
{
  AQ_CALL_RET_V(height(), int);
}
inline QSize AQSPixmap::size() const
{
  AQ_CALL_RET_V(size(), QSize);
}
inline QRect AQSPixmap::rect() const
{
  AQ_CALL_RET_V(rect(), QRect);
}
inline int AQSPixmap::depth() const
{
  AQ_CALL_RET_V(depth(), int);
}
inline void AQSPixmap::fill(const QColor &arg0)
{
  AQ_CALL_VOID(fill(arg0));
}
inline void AQSPixmap::fill(QColor *arg0)
{
  AQ_CALL_VOID(fill(*arg0));
}
inline void AQSPixmap::fill(AQSColor *arg0)
{
  AQ_CALL_VOID(fill(*arg0));
}
inline void AQSPixmap::fill(const QWidget *arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(fill(arg0, arg1, arg2));
}
inline void AQSPixmap::fill(const QWidget *arg0,  const QPoint &arg1)
{
  AQ_CALL_VOID(fill(arg0, arg1));
}
inline void AQSPixmap::resize(int arg0,  int arg1)
{
  AQ_CALL_VOID(resize(arg0, arg1));
}
inline void AQSPixmap::resize(const QSize &arg0)
{
  AQ_CALL_VOID(resize(arg0));
}
inline bool AQSPixmap::selfMask() const
{
  AQ_CALL_RET_V(selfMask(), bool);
}
inline bool AQSPixmap::hasAlpha() const
{
  AQ_CALL_RET_V(hasAlpha(), bool);
}
inline bool AQSPixmap::hasAlphaChannel() const
{
  AQ_CALL_RET_V(hasAlphaChannel(), bool);
}
inline QPixmap *AQSPixmap::xForm(QWMatrix *arg0) const
{
  AQ_CALL_RET_PTR(xForm(*arg0), QPixmap);
}
inline QPixmap *AQSPixmap::xForm(AQSWMatrix *arg0) const
{
  AQ_CALL_RET_PTR(xForm(*arg0), QPixmap);
}
inline QImage *AQSPixmap::convertToImage() const
{
  AQ_CALL_RET_PTR(convertToImage(), QImage);
}
inline bool AQSPixmap::convertFromImage(QImage *arg0,  int arg1)
{
  AQ_CALL_RET_V(convertFromImage(*arg0, arg1), bool);
}
inline bool AQSPixmap::convertFromImage(AQSImage *arg0,  int arg1)
{
  AQ_CALL_RET_V(convertFromImage(*arg0, arg1), bool);
}
inline bool AQSPixmap::loadFromData(QByteArray *arg0,  const char *arg1,  int arg2)
{
  AQ_CALL_RET_V(loadFromData(*arg0, arg1, arg2), bool);
}
inline bool AQSPixmap::loadFromData(AQSByteArray *arg0,  const char *arg1,  int arg2)
{
  AQ_CALL_RET_V(loadFromData(*arg0, arg1, arg2), bool);
}
inline bool AQSPixmap::save(QIODevice *arg0,  const char *arg1,  int arg2) const
{
  AQ_CALL_RET_V(save(arg0, arg1, arg2), bool);
}
inline bool AQSPixmap::save(AQSIODevice *arg0,  const char *arg1,  int arg2) const
{
  AQ_CALL_RET_V(save(*arg0, arg1, arg2), bool);
}
inline bool AQSPixmap::load(const QString &arg0,  const char *arg1,  int arg2)
{
  AQ_CALL_RET_V(load(arg0, arg1, arg2), bool);
}
inline bool AQSPixmap::save(const QString &arg0,  const char *arg1,  int arg2) const
{
  AQ_CALL_RET_V(save(arg0, arg1, arg2), bool);
}
inline int AQSPixmap::serialNumber() const
{
  AQ_CALL_RET_V(serialNumber(), int);
}
inline void AQSPixmap::setOptimization(uint arg0)
{
  AQ_CALL_VOID(setOptimization(static_cast<QPixmap::Optimization>(arg0)));
}
inline bool AQSPixmap::isQBitmap() const
{
  AQ_CALL_RET_V(isQBitmap(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSPIXMAP_P_H_ */
// @AQSVOIDPTR@
