/***************************************************************************
 AQSBoxLayout_p.h
 -------------------
 begin                : 07/02/2011
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

#ifndef AQSBOXLAYOUT_P_H_
#define AQSBOXLAYOUT_P_H_

#include "AQSLayout_p.h"
#include <qlayout.h>

class AQSBoxLayout : public AQSLayout
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(BoxLayout, Layout);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void addItem(QLayoutItem *);
  uint direction() const;
  void setDirection(uint);
  void addSpacing(int);
  void addStretch(int = 0);
  void addWidget(QWidget *, int = 0, int = 0);
  void addLayout(QLayout *, int = 0);
  void addStrut(int);
  void insertSpacing(int, int);
  void insertStretch(int, int = 0);
  void insertWidget(int, QWidget *, int = 0, int = 0);
  void insertLayout(int, QLayout *, int = 0);
  bool setStretchFactor(QWidget *, int);
  bool setStretchFactor(QLayout *, int);
  QSize sizeHint() const;
  QSize minimumSize() const;
  QSize maximumSize() const;
  bool hasHeightForWidth() const;
  int heightForWidth(int) const;
  uint expanding() const;
  void invalidate();
  bool activate();
  void setGeometry(const QRect &);
  int findWidget(QWidget *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QWidget*,QBoxLayout::Direction"));
    candidates[3].append(QString::fromLatin1("QWidget*,QBoxLayout::Direction,int"));
    candidates[4].append(QString::fromLatin1("QWidget*,QBoxLayout::Direction,int,int"));
    candidates[5].append(QString::fromLatin1("QWidget*,QBoxLayout::Direction,int,int,QString"));
    candidates[2].append(QString::fromLatin1("QLayout*,QBoxLayout::Direction"));
    candidates[3].append(QString::fromLatin1("QLayout*,QBoxLayout::Direction,int"));
    candidates[4].append(QString::fromLatin1("QLayout*,QBoxLayout::Direction,int,QString"));
    candidates[1].append(QString::fromLatin1("QBoxLayout::Direction"));
    candidates[2].append(QString::fromLatin1("QBoxLayout::Direction,int"));
    candidates[3].append(QString::fromLatin1("QBoxLayout::Direction,int,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QWidget*,QBoxLayout::Direction"))
      return new QBoxLayout(argValue<QWidget *>(args[0]),
                            static_cast<QBoxLayout::Direction>(args[1].variant().toUInt()));
    if (sgt == QString::fromLatin1("QWidget*,QBoxLayout::Direction,int"))
      return new QBoxLayout(argValue<QWidget *>(args[0]),
                            static_cast<QBoxLayout::Direction>(args[1].variant().toUInt()),
                            args[2].variant().toInt());
    if (sgt == QString::fromLatin1("QWidget*,QBoxLayout::Direction,int,int"))
      return new QBoxLayout(argValue<QWidget *>(args[0]),
                            static_cast<QBoxLayout::Direction>(args[1].variant().toUInt()),
                            args[2].variant().toInt(),
                            args[3].variant().toInt());
    if (sgt == QString::fromLatin1("QWidget*,QBoxLayout::Direction,int,int,QString"))
      return new QBoxLayout(argValue<QWidget *>(args[0]),
                            static_cast<QBoxLayout::Direction>(args[1].variant().toUInt()),
                            args[2].variant().toInt(),
                            args[3].variant().toInt(),
                            *(argValue<QString *>(args[4])));
    if (sgt == QString::fromLatin1("QLayout*,QBoxLayout::Direction"))
      return new QBoxLayout(argValue<QLayout *>(args[0]),
                            static_cast<QBoxLayout::Direction>(args[1].variant().toUInt()));
    if (sgt == QString::fromLatin1("QLayout*,QBoxLayout::Direction,int"))
      return new QBoxLayout(argValue<QLayout *>(args[0]),
                            static_cast<QBoxLayout::Direction>(args[1].variant().toUInt()),
                            args[2].variant().toInt());
    if (sgt == QString::fromLatin1("QLayout*,QBoxLayout::Direction,int,QString"))
      return new QBoxLayout(argValue<QLayout *>(args[0]),
                            static_cast<QBoxLayout::Direction>(args[1].variant().toUInt()),
                            args[2].variant().toInt(),
                            *(argValue<QString *>(args[3])));
    if (sgt == QString::fromLatin1("QBoxLayout::Direction"))
      return new QBoxLayout(static_cast<QBoxLayout::Direction>(args[0].variant().toUInt()));
    if (sgt == QString::fromLatin1("QBoxLayout::Direction,int"))
      return new QBoxLayout(static_cast<QBoxLayout::Direction>(args[0].variant().toUInt()),
                            args[1].variant().toInt());
    if (sgt == QString::fromLatin1("QBoxLayout::Direction,int,QString"))
      return new QBoxLayout(static_cast<QBoxLayout::Direction>(args[0].variant().toUInt()),
                            args[1].variant().toInt(),
                            *(argValue<QString *>(args[2])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[2].append(QString::fromLatin1("QWidget*,QBoxLayout::Direction"));
    candidates[3].append(QString::fromLatin1("QWidget*,QBoxLayout::Direction,int"));
    candidates[4].append(QString::fromLatin1("QWidget*,QBoxLayout::Direction,int,int"));
    candidates[5].append(QString::fromLatin1("QWidget*,QBoxLayout::Direction,int,int,QString"));
    candidates[2].append(QString::fromLatin1("QLayout*,QBoxLayout::Direction"));
    candidates[3].append(QString::fromLatin1("QLayout*,QBoxLayout::Direction,int"));
    candidates[4].append(QString::fromLatin1("QLayout*,QBoxLayout::Direction,int,QString"));
    candidates[1].append(QString::fromLatin1("QBoxLayout::Direction"));
    candidates[2].append(QString::fromLatin1("QBoxLayout::Direction,int"));
    candidates[3].append(QString::fromLatin1("QBoxLayout::Direction,int,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSBoxLayout::addItem(QLayoutItem *arg0)
{
  AQ_CALL_VOID(addItem(arg0));
}
inline uint AQSBoxLayout::direction() const
{
  AQ_CALL_RET_V(direction(), uint);
}
inline void AQSBoxLayout::setDirection(uint arg0)
{
  AQ_CALL_VOID(setDirection(static_cast<QBoxLayout::Direction>(arg0)));
}
inline void AQSBoxLayout::addSpacing(int arg0)
{
  AQ_CALL_VOID(addSpacing(arg0));
}
inline void AQSBoxLayout::addStretch(int arg0)
{
  AQ_CALL_VOID(addStretch(arg0));
}
inline void AQSBoxLayout::addWidget(QWidget *arg0,  int arg1,  int arg2)
{
  AQ_CALL_VOID(addWidget(arg0, arg1, arg2));
}
inline void AQSBoxLayout::addLayout(QLayout *arg0,  int arg1)
{
  AQ_CALL_VOID(addLayout(arg0, arg1));
}
inline void AQSBoxLayout::addStrut(int arg0)
{
  AQ_CALL_VOID(addStrut(arg0));
}
inline void AQSBoxLayout::insertSpacing(int arg0,  int arg1)
{
  AQ_CALL_VOID(insertSpacing(arg0, arg1));
}
inline void AQSBoxLayout::insertStretch(int arg0,  int arg1)
{
  AQ_CALL_VOID(insertStretch(arg0, arg1));
}
inline void AQSBoxLayout::insertWidget(int arg0,  QWidget *arg1,  int arg2,  int arg3)
{
  AQ_CALL_VOID(insertWidget(arg0, arg1, arg2, arg3));
}
inline void AQSBoxLayout::insertLayout(int arg0,  QLayout *arg1,  int arg2)
{
  AQ_CALL_VOID(insertLayout(arg0, arg1, arg2));
}
inline bool AQSBoxLayout::setStretchFactor(QWidget *arg0,  int arg1)
{
  AQ_CALL_RET_V(setStretchFactor(arg0, arg1), bool);
}
inline bool AQSBoxLayout::setStretchFactor(QLayout *arg0,  int arg1)
{
  AQ_CALL_RET_V(setStretchFactor(arg0, arg1), bool);
}
inline QSize AQSBoxLayout::sizeHint() const
{
  AQ_CALL_RET_V(sizeHint(), QSize);
}
inline QSize AQSBoxLayout::minimumSize() const
{
  AQ_CALL_RET_V(minimumSize(), QSize);
}
inline QSize AQSBoxLayout::maximumSize() const
{
  AQ_CALL_RET_V(maximumSize(), QSize);
}
inline bool AQSBoxLayout::hasHeightForWidth() const
{
  AQ_CALL_RET_V(hasHeightForWidth(), bool);
}
inline int AQSBoxLayout::heightForWidth(int arg0) const
{
  AQ_CALL_RET_V(heightForWidth(arg0), int);
}
inline uint AQSBoxLayout::expanding() const
{
  AQ_CALL_RET_V(expanding(), uint);
}
inline void AQSBoxLayout::invalidate()
{
  AQ_CALL_VOID(invalidate());
}
inline bool AQSBoxLayout::activate()
{
  AQ_CALL_RET_V(activate(), bool);
}
inline void AQSBoxLayout::setGeometry(const QRect &arg0)
{
  AQ_CALL_VOID(setGeometry(arg0));
}
inline int AQSBoxLayout::findWidget(QWidget *arg0)
{
  AQ_CALL_RET_V(findWidget(arg0), int);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSBOXLAYOUT_P_H_ */
// @AQSOBJECT@
