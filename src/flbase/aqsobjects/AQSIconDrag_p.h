/***************************************************************************
 AQSIconDrag_p.h
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

#ifndef AQSICONDRAG_P_H_
#define AQSICONDRAG_P_H_

#include "AQSDragObject_p.h"
#include "AQSIconDragItem_p.h"
#include <qiconview.h>

class AQSIconDrag : public AQSDragObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(IconDrag, DragObject);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void append(QIconDragItem *, const QRect &, const QRect &);
  void append(AQSIconDragItem *, const QRect &, const QRect &);
  virtual QByteArray *encodedData(const char *) const;
  bool canDecode(QMimeSource *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QIconDrag(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QIconDrag(argValue<QWidget *>(args[0]),
                           *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSIconDrag::append(QIconDragItem *arg0,  const QRect &arg1,  const QRect &arg2)
{
  AQ_CALL_VOID(append(*arg0, arg1, arg2));
}
inline void AQSIconDrag::append(AQSIconDragItem *arg0,  const QRect &arg1,  const QRect &arg2)
{
  AQ_CALL_VOID(append(*arg0, arg1, arg2));
}
inline QByteArray *AQSIconDrag::encodedData(const char *arg0) const
{
  AQ_CALL_RET_PTR(encodedData(arg0), QByteArray);
}
inline bool AQSIconDrag::canDecode(QMimeSource *arg0)
{
  AQ_CALL_RET_V(canDecode(arg0), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSICONDRAG_P_H_ */
// @AQSOBJECT@
