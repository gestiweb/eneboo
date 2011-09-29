/***************************************************************************
 AQSTextDrag_p.h
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

#ifndef AQSTEXTDRAG_P_H_
#define AQSTEXTDRAG_P_H_

#include "AQSDragObject_p.h"

class AQSTextDrag : public AQSDragObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(TextDrag, DragObject);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual void setText(const QString &);
  virtual void setSubtype(const QCString &);
  bool canDecode(const QMimeSource *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,QString"));
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QString"))
      return new QTextDrag(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,QWidget*"))
      return new QTextDrag(*(argValue<QString *>(args[0])),
                           argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("QString,QWidget*,QString"))
      return new QTextDrag(*(argValue<QString *>(args[0])),
                           argValue<QWidget *>(args[1]),
                           *(argValue<QString *>(args[2])));
    if (sgt.isEmpty())
      return new QTextDrag;
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QTextDrag(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QTextDrag(argValue<QWidget *>(args[0]),
                           *(argValue<QString *>(args[1])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,QString"));
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSTextDrag::setText(const QString &arg0)
{
  AQ_CALL_VOID(setText(arg0));
}
inline void AQSTextDrag::setSubtype(const QCString &arg0)
{
  AQ_CALL_VOID(setSubtype(arg0));
}
inline bool AQSTextDrag::canDecode(const QMimeSource *arg0)
{
  AQ_CALL_RET_V(canDecode(arg0), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSTEXTDRAG_P_H_ */
// @AQSOBJECT@
