/***************************************************************************
 AQSDomText_p.h
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

#ifndef AQSDOMTEXT_P_H_
#define AQSDOMTEXT_P_H_

#include "AQSDomCharacterData_p.h"

// @AQ_PTR_INDIRECTION@

class AQSDomText : public AQSDomCharacterData
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DomText, DomCharacterData);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QDomText *splitText(int);
  virtual uint nodeType() const;
  virtual bool isText() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomText*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDomText;
    if (sgt == QString::fromLatin1("QDomText*"))
      return new QDomText(*argValue<QDomText *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomText*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QDomText *AQSDomText::splitText(int arg0)
{
  AQ_CALL_RET_PTR(splitText(arg0), QDomText);
}
inline uint AQSDomText::nodeType() const
{
  AQ_CALL_RET_V(nodeType(), uint);
}
inline bool AQSDomText::isText() const
{
  AQ_CALL_RET_V(isText(), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDOMTEXT_P_H_ */
// @AQSVOIDPTR@
