/***************************************************************************
 AQSLabel_p.h
 -------------------
 begin                : 30/01/2011
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

#ifndef AQSLABEL_P_H_
#define AQSLABEL_P_H_

#include "AQSFrame_p.h"
#include "AQSPixmap_p.h"
#include "AQSPicture_p.h"
#include <qlabel.h>

class AQSLabel : public AQSFrame
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(Label, Frame);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QPicture *picture() const;
  void setBuddy(QWidget *);
  QWidget *buddy() const;
  virtual void setPixmap(QPixmap *);
  virtual void setPixmap(AQSPixmap *);
  virtual void setPicture(QPicture *);
  virtual void setPicture(AQSPicture *);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,QString"));
    candidates[4].append(QString::fromLatin1("QString,QWidget*,QString,WFlags"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,QWidget*"));
    candidates[4].append(QString::fromLatin1("QWidget*,QString,QWidget*,QString"));
    candidates[5].append(QString::fromLatin1("QWidget*,QString,QWidget*,QString,WFlags"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QWidget*"))
      return new QLabel(argValue<QWidget *>(args[0]));
    if (sgt == QString::fromLatin1("QWidget*,QString"))
      return new QLabel(argValue<QWidget *>(args[0]),
                        *(argValue<QString *>(args[1])));
    if (sgt == QString::fromLatin1("QWidget*,QString,WFlags"))
      return new QLabel(argValue<QWidget *>(args[0]),
                        *(argValue<QString *>(args[1])),
                        static_cast<WFlags>(args[2].variant().toUInt()));
    if (sgt == QString::fromLatin1("QString,QWidget*"))
      return new QLabel(*(argValue<QString *>(args[0])),
                        argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("QString,QWidget*,QString"))
      return new QLabel(*(argValue<QString *>(args[0])),
                        argValue<QWidget *>(args[1]),
                        *(argValue<QString *>(args[2])));
    if (sgt == QString::fromLatin1("QString,QWidget*,QString,WFlags"))
      return new QLabel(*(argValue<QString *>(args[0])),
                        argValue<QWidget *>(args[1]),
                        *(argValue<QString *>(args[2])),
                        static_cast<WFlags>(args[3].variant().toUInt()));
    if (sgt == QString::fromLatin1("QWidget*,QString,QWidget*"))
      return new QLabel(argValue<QWidget *>(args[0]),
                        *(argValue<QString *>(args[1])),
                        argValue<QWidget *>(args[2]));
    if (sgt == QString::fromLatin1("QWidget*,QString,QWidget*,QString"))
      return new QLabel(argValue<QWidget *>(args[0]),
                        *(argValue<QString *>(args[1])),
                        argValue<QWidget *>(args[2]),
                        *(argValue<QString *>(args[3])));
    if (sgt == QString::fromLatin1("QWidget*,QString,QWidget*,QString,WFlags"))
      return new QLabel(argValue<QWidget *>(args[0]),
                        *(argValue<QString *>(args[1])),
                        argValue<QWidget *>(args[2]),
                        *(argValue<QString *>(args[3])),
                        static_cast<WFlags>(args[4].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QWidget*"));
    candidates[2].append(QString::fromLatin1("QWidget*,QString"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,WFlags"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,QString"));
    candidates[4].append(QString::fromLatin1("QString,QWidget*,QString,WFlags"));
    candidates[3].append(QString::fromLatin1("QWidget*,QString,QWidget*"));
    candidates[4].append(QString::fromLatin1("QWidget*,QString,QWidget*,QString"));
    candidates[5].append(QString::fromLatin1("QWidget*,QString,QWidget*,QString,WFlags"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QPicture *AQSLabel::picture() const
{
  AQ_CALL_RET(picture());
}
inline void AQSLabel::setBuddy(QWidget *arg0)
{
  AQ_CALL_VOID(setBuddy(arg0));
}
inline QWidget *AQSLabel::buddy() const
{
  AQ_CALL_RET(buddy());
}
inline void AQSLabel::setPixmap(QPixmap *arg0)
{
  AQ_CALL_VOID(setPixmap(*arg0));
}
inline void AQSLabel::setPixmap(AQSPixmap *arg0)
{
  AQ_CALL_VOID(setPixmap(*arg0));
}
inline void AQSLabel::setPicture(QPicture *arg0)
{
  AQ_CALL_VOID(setPicture(*arg0));
}
inline void AQSLabel::setPicture(AQSPicture *arg0)
{
  AQ_CALL_VOID(setPicture(*arg0));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSLABEL_P_H_ */
// @AQSWIDGET@
