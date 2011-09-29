/***************************************************************************
 AQSColorDrag_p.h
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

#ifndef AQSCOLORDRAG_P_H_
#define AQSCOLORDRAG_P_H_

#include "AQSStoredDrag_p.h"
#include "AQSColor_p.h"

class AQSColorDrag : public AQSStoredDrag
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(ColorDrag, StoredDrag);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  void setColor(QColor *);
  void setColor(AQSColor *);
  bool canDecode(QMimeSource *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QColor*"));
    candidates[2].append(QString::fromLatin1("QColor*,QWidget*"));
    candidates[3].append(QString::fromLatin1("QColor*,QWidget*,QString"));
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QColor*"))
      return new QColorDrag(*argValue<QColor *>(args[0]));
    if (sgt == QString::fromLatin1("QColor*,QWidget*"))
      return new QColorDrag(*argValue<QColor *>(args[0]),
                            argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("QColor*,QWidget*,QString"))
      return new QColorDrag(*argValue<QColor *>(args[0]),
                            argValue<QWidget *>(args[1]),
                            *(argValue<QString *>(args[2])));
    if (sgt.isEmpty())
      return new QColorDrag;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QColorDrag(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QColorDrag(argValue<QWidget *>(args[0]),
                            *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QColor*"));
    candidates[2].append(QString::fromLatin1("QColor*,QWidget*"));
    candidates[3].append(QString::fromLatin1("QColor*,QWidget*,QString"));
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSColorDrag::setColor(QColor *arg0)
{
  AQ_CALL_VOID(setColor(*arg0));
}
inline void AQSColorDrag::setColor(AQSColor *arg0)
{
  AQ_CALL_VOID(setColor(*arg0));
}
inline bool AQSColorDrag::canDecode(QMimeSource *arg0)
{
  AQ_CALL_RET_V(canDecode(arg0), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSCOLORDRAG_P_H_ */
// @AQSOBJECT@
