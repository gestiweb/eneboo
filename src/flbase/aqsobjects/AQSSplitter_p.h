/***************************************************************************
 AQSSplitter_p.h
 -------------------
 begin                : 05/11/2011
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

#ifndef AQSSPLITTER_P_H_
#define AQSSPLITTER_P_H_

#include "AQSFrame_p.h"
#include <qsplitter.h>

class AQSSplitter : public AQSFrame
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Splitter, Frame);

public slots:
  QValueList<QVariant> sizes() const {
    QVariantList ret;
    if (!o_)
      return ret;
    QValueList<int> l(o_->sizes());
    for (QValueList<int>::const_iterator it = l.begin(); it != l.end(); ++it)
      ret << *it;
    return ret;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setCollapsible(QWidget *, bool);
  virtual void setResizeMode(QWidget *, uint);
  void moveToFirst(QWidget *);
  void moveToLast(QWidget *);
  void refresh();
  void setSizes(QValueList<int>);
  int handleWidth() const;
  void setHandleWidth(int);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[1].append(QString::fromLatin1("Qt::Orientation"));
    candidates[2].append(QString::fromLatin1("Qt::Orientation,QWidget*"));
    candidates[3].append(QString::fromLatin1("Qt::Orientation,QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QSplitter;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QSplitter(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QSplitter(argValue<QWidget *>(args[0]),
                           *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("Qt::Orientation"))
      return new QSplitter(static_cast<Qt::Orientation>(args[0].variant().toUInt()));
    if (sgt == QString::fromLatin1("Qt::Orientation,QWidget*"))
      return new QSplitter(static_cast<Qt::Orientation>(args[0].variant().toUInt()),
                           argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("Qt::Orientation,QWidget*,QString"))
      return new QSplitter(static_cast<Qt::Orientation>(args[0].variant().toUInt()),
                           argValue<QWidget *>(args[1]),
                           *(argValue<QString *>(args[2])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[1].append(QString::fromLatin1("Qt::Orientation"));
    candidates[2].append(QString::fromLatin1("Qt::Orientation,QWidget*"));
    candidates[3].append(QString::fromLatin1("Qt::Orientation,QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSSplitter::setCollapsible(QWidget *arg0,  bool arg1)
{
  AQ_CALL_VOID(setCollapsible(arg0, arg1));
}
inline void AQSSplitter::setResizeMode(QWidget *arg0,  uint arg1)
{
  AQ_CALL_VOID(setResizeMode(arg0, static_cast<QSplitter::ResizeMode>(arg1)));
}
inline void AQSSplitter::moveToFirst(QWidget *arg0)
{
  AQ_CALL_VOID(moveToFirst(arg0));
}
inline void AQSSplitter::moveToLast(QWidget *arg0)
{
  AQ_CALL_VOID(moveToLast(arg0));
}
inline void AQSSplitter::refresh()
{
  AQ_CALL_VOID(refresh());
}
inline void AQSSplitter::setSizes(QValueList<int> arg0)
{
  AQ_CALL_VOID(setSizes(arg0));
}
inline int AQSSplitter::handleWidth() const
{
  AQ_CALL_RET_V(handleWidth(), int);
}
inline void AQSSplitter::setHandleWidth(int arg0)
{
  AQ_CALL_VOID(setHandleWidth(arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSPLITTER_P_H_ */
// @AQSWIDGET@
