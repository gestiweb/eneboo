/***************************************************************************
 AQSColor_p.h
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

#ifndef AQSCOLOR_P_H_
#define AQSCOLOR_P_H_

#include "AQSVoidPtr_p.h"
#include <qcolor.h>

// @AQ_PTR_INDIRECTION@

class AQSColor : public AQSVoidPtr
{
  Q_OBJECT

  Q_PROPERTY(QColor toVariant READ toVariant);

  AQ_DECLARE_AQS_VOID(Color, VoidPtr);

public:
  QColor toVariant() const {
    if (!o_)
      return QColor();
    return *o_;
  }

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  bool isValid() const;
  QString name() const;
  void setNamedColor(const QString &);
  QRgb rgb() const;
  void setRgb(int, int, int);
  void setRgb(QRgb);
  int red() const;
  int green() const;
  int blue() const;
  void setHsv(int, int, int);
  QColor *light(int = 150) const;
  QColor *dark(int = 200) const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QRgb"));
    candidates[2].append(QString::fromLatin1("QRgb,uint"));
    candidates[3].append(QString::fromLatin1("int,int,int"));
    candidates[4].append(QString::fromLatin1("int,int,int,QColor::Spec"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[1].append(QString::fromLatin1("QColor*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QColor;
    if (sgt == QString::fromLatin1("QRgb"))
      return new QColor(*(argValue<QRgb *>(args[0])));
    if (sgt == QString::fromLatin1("QRgb,uint"))
      return new QColor(*(argValue<QRgb *>(args[0])),
                        args[1].variant().toUInt());
    if (sgt == QString::fromLatin1("int,int,int"))
      return new QColor(args[0].variant().toInt(),
                        args[1].variant().toInt(),
                        args[2].variant().toInt());
    if (sgt == QString::fromLatin1("int,int,int,QColor::Spec"))
      return new QColor(args[0].variant().toInt(),
                        args[1].variant().toInt(),
                        args[2].variant().toInt(),
                        static_cast<QColor::Spec>(args[3].variant().toUInt()));
    if (sgt == QString::fromLatin1("QString"))
      return new QColor(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QColor*"))
      return new QColor(*argValue<QColor *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QRgb"));
    candidates[2].append(QString::fromLatin1("QRgb,uint"));
    candidates[3].append(QString::fromLatin1("int,int,int"));
    candidates[4].append(QString::fromLatin1("int,int,int,QColor::Spec"));
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[1].append(QString::fromLatin1("QColor*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline bool AQSColor::isValid() const
{
  AQ_CALL_RET_V(isValid(), bool);
}
inline QString AQSColor::name() const
{
  AQ_CALL_RET_V(name(), QString);
}
inline void AQSColor::setNamedColor(const QString &arg0)
{
  AQ_CALL_VOID(setNamedColor(arg0));
}
inline QRgb AQSColor::rgb() const
{
  AQ_CALL_RET_V(rgb(), QRgb);
}
inline void AQSColor::setRgb(int arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(setRgb(arg0, arg1, arg2));
}
inline void AQSColor::setRgb(QRgb arg0)
{
  AQ_CALL_VOID(setRgb(arg0));
}
inline int AQSColor::red() const
{
  AQ_CALL_RET_V(red(), int);
}
inline int AQSColor::green() const
{
  AQ_CALL_RET_V(green(), int);
}
inline int AQSColor::blue() const
{
  AQ_CALL_RET_V(blue(), int);
}
inline void AQSColor::setHsv(int arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(setHsv(arg0, arg1, arg2));
}
inline QColor *AQSColor::light(int arg0) const
{
  AQ_CALL_RET_PTR(light(arg0), QColor);
}
inline QColor *AQSColor::dark(int arg0) const
{
  AQ_CALL_RET_PTR(dark(arg0), QColor);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCOLOR_P_H_ */
// @AQSVOIDPTR@
