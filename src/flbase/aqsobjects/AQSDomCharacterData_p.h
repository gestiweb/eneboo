/***************************************************************************
 AQSDomCharacterData_p.h
 -------------------
 begin                : 16/02/2011
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

#ifndef AQSDOMCHARACTERDATA_P_H_
#define AQSDOMCHARACTERDATA_P_H_

#include "AQSDomNode_p.h"

// @AQ_PTR_INDIRECTION@

class AQSDomCharacterData : public AQSDomNode
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DomCharacterData, DomNode);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual QString substringData(unsigned long, unsigned long);
  virtual void appendData(const QString &);
  virtual void insertData(unsigned long, const QString &);
  virtual void deleteData(unsigned long, unsigned long);
  virtual void replaceData(unsigned long, unsigned long, const QString &);
  virtual uint length() const;
  virtual QString data() const;
  virtual void setData(const QString &);
  virtual uint nodeType() const;
  virtual bool isCharacterData() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomCharacterData*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDomCharacterData;
    if (sgt == QString::fromLatin1("QDomCharacterData*"))
      return new QDomCharacterData(*argValue<QDomCharacterData *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomCharacterData*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSDomCharacterData::substringData(unsigned long arg0,  unsigned long arg1)
{
  AQ_CALL_RET_V(substringData(arg0, arg1), QString);
}
inline void AQSDomCharacterData::appendData(const QString &arg0)
{
  AQ_CALL_VOID(appendData(arg0));
}
inline void AQSDomCharacterData::insertData(unsigned long arg0,  const QString &arg1)
{
  AQ_CALL_VOID(insertData(arg0, arg1));
}
inline void AQSDomCharacterData::deleteData(unsigned long arg0,  unsigned long arg1)
{
  AQ_CALL_VOID(deleteData(arg0, arg1));
}
inline void AQSDomCharacterData::replaceData(unsigned long arg0,  unsigned long arg1,  const QString &arg2)
{
  AQ_CALL_VOID(replaceData(arg0, arg1, arg2));
}
inline uint AQSDomCharacterData::length() const
{
  AQ_CALL_RET_V(length(), uint);
}
inline QString AQSDomCharacterData::data() const
{
  AQ_CALL_RET_V(data(), QString);
}
inline void AQSDomCharacterData::setData(const QString &arg0)
{
  AQ_CALL_VOID(setData(arg0));
}
inline uint AQSDomCharacterData::nodeType() const
{
  AQ_CALL_RET_V(nodeType(), uint);
}
inline bool AQSDomCharacterData::isCharacterData() const
{
  AQ_CALL_RET_V(isCharacterData(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDOMCHARACTERDATA_P_H_ */
// @AQSVOIDPTR@
