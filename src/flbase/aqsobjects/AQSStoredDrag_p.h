/***************************************************************************
 AQSStoredDrag_p.h
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

#ifndef AQSSTOREDDRAG_P_H_
#define AQSSTOREDDRAG_P_H_

#include "AQSDragObject_p.h"

class AQSStoredDrag : public AQSDragObject
{
  Q_OBJECT

  AQ_DECLARE_AQS_OBJECT(StoredDrag, DragObject);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual void setEncodedData(QByteArray *);
  virtual void setEncodedData(AQSByteArray *);
  virtual QByteArray *encodedData(const char *) const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,QString"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt == QString::fromLatin1("QString"))
      return new QStoredDrag(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QString,QWidget*"))
      return new QStoredDrag(*(argValue<QString *>(args[0])),
                             argValue<QWidget *>(args[1]));
    if (sgt == QString::fromLatin1("QString,QWidget*,QString"))
      return new QStoredDrag(*(argValue<QString *>(args[0])),
                             argValue<QWidget *>(args[1]),
                             *(argValue<QString *>(args[2])));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[2].append(QString::fromLatin1("QString,QWidget*"));
    candidates[3].append(QString::fromLatin1("QString,QWidget*,QString"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline void AQSStoredDrag::setEncodedData(QByteArray *arg0)
{
  AQ_CALL_VOID(setEncodedData(*arg0));
}
inline void AQSStoredDrag::setEncodedData(AQSByteArray *arg0)
{
  AQ_CALL_VOID(setEncodedData(*arg0));
}
inline QByteArray *AQSStoredDrag::encodedData(const char *arg0) const
{
  AQ_CALL_RET_PTR(encodedData(arg0), QByteArray);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSSTOREDDRAG_P_H_ */
// @AQSOBJECT@
