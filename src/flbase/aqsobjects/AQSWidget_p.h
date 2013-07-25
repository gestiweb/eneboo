/***************************************************************************
 AQSWidget_p.h
 -------------------
 begin                : 29/01/2011
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

#ifndef AQSWIDGET_P_H_
#define AQSWIDGET_P_H_

#include "AQSObject_p.h"
#include <qwidget.h>

class AQSWidget : public AQSObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Widget, Object);

public slots:
  QPaintDevice *paintDevice() const {
    return static_cast<QPaintDevice *>(o_);
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QWidget *parentWidget(bool = FALSE) const;
  uint testWFlags(WFlags) const;
  QWidget *childAt(int, int, bool = FALSE) const;
  QWidget *childAt(const QPoint &, bool = FALSE) const;
  QFontMetrics *fontMetrics() const;
  void setFixedSize(const QSize &);
  void setFixedSize(int, int);
  void setFixedWidth(int);
  void setFixedHeight(int);
  QLayout *layout() const;
  void setSizePolicy(uint, uint, bool = FALSE);
  virtual void reparent(QWidget *, WFlags, const QPoint &, bool = FALSE);
  void reparent(QWidget *, const QPoint &, bool = FALSE);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QWidget;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QWidget(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QWidget(argValue<QWidget *>(args[0]),
                         *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new QWidget(argValue<QWidget *>(args[0]),
                         *(argValue<QString *>(args[1])),
                         static_cast<WFlags>(args[2].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QWidget *AQSWidget::parentWidget(bool arg0) const
{
  AQ_CALL_RET(parentWidget(arg0));
}
inline uint AQSWidget::testWFlags(WFlags arg0) const
{
  AQ_CALL_RET_V(testWFlags(arg0), uint);
}
inline QWidget *AQSWidget::childAt(int arg0,  int arg1,  bool arg2) const
{
  AQ_CALL_RET(childAt(arg0, arg1, arg2));
}
inline QWidget *AQSWidget::childAt(const QPoint &arg0,  bool arg1) const
{
  AQ_CALL_RET(childAt(arg0, arg1));
}
inline QFontMetrics *AQSWidget::fontMetrics() const
{
  AQ_CALL_RET_PTR(fontMetrics(), QFontMetrics);
}
inline void AQSWidget::setFixedSize(const QSize &arg0)
{
  AQ_CALL_VOID(setFixedSize(arg0));
}
inline void AQSWidget::setFixedSize(int arg0,  int arg1)
{
  AQ_CALL_VOID(setFixedSize(arg0, arg1));
}
inline void AQSWidget::setFixedWidth(int arg0)
{
  AQ_CALL_VOID(setFixedWidth(arg0));
}
inline void AQSWidget::setFixedHeight(int arg0)
{
  AQ_CALL_VOID(setFixedHeight(arg0));
}
inline QLayout *AQSWidget::layout() const
{
  AQ_CALL_RET(layout());
}
inline void AQSWidget::setSizePolicy(uint arg0,  uint arg1,  bool arg2)
{
  AQ_CALL_VOID(setSizePolicy(static_cast<QSizePolicy::SizeType>(arg0), static_cast<QSizePolicy::SizeType>(arg1), arg2));
}
inline void AQSWidget::reparent(QWidget *arg0,  WFlags arg1,  const QPoint &arg2,  bool arg3)
{
  AQ_CALL_VOID(reparent(arg0, arg1, arg2, arg3));
}
inline void AQSWidget::reparent(QWidget *arg0,  const QPoint &arg1,  bool arg2)
{
  AQ_CALL_VOID(reparent(arg0, arg1, arg2));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSWIDGET_P_H_ */
// @AQSWIDGET@
