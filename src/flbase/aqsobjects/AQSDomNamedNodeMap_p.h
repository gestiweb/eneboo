/***************************************************************************
 AQSDomNamedNodeMap_p.h
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

#ifndef AQSDOMNAMEDNODEMAP_P_H_
#define AQSDOMNAMEDNODEMAP_P_H_

#include "AQSDomNode_p.h"

// @AQ_PTR_INDIRECTION@

class AQSDomNamedNodeMap : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DomNamedNodeMap, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QDomNode *namedItem(const QString &) const;
  QDomNode *setNamedItem(QDomNode *);
  QDomNode *setNamedItem(AQSDomNode *);
  QDomNode *removeNamedItem(const QString &);
  QDomNode *item(int) const;
  QDomNode *namedItemNS(const QString &, const QString &) const;
  QDomNode *setNamedItemNS(QDomNode *);
  QDomNode *setNamedItemNS(AQSDomNode *);
  QDomNode *removeNamedItemNS(const QString &, const QString &);
  uint length() const;
  uint count() const;
  bool contains(const QString &) const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomNamedNodeMap*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDomNamedNodeMap;
    if (sgt == QString::fromLatin1("QDomNamedNodeMap*"))
      return new QDomNamedNodeMap(*argValue<QDomNamedNodeMap *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomNamedNodeMap*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QDomNode *AQSDomNamedNodeMap::namedItem(const QString &arg0) const
{
  AQ_CALL_RET_PTR(namedItem(arg0), QDomNode);
}
inline QDomNode *AQSDomNamedNodeMap::setNamedItem(QDomNode *arg0)
{
  AQ_CALL_RET_PTR(setNamedItem(*arg0), QDomNode);
}
inline QDomNode *AQSDomNamedNodeMap::setNamedItem(AQSDomNode *arg0)
{
  AQ_CALL_RET_PTR(setNamedItem(*arg0), QDomNode);
}
inline QDomNode *AQSDomNamedNodeMap::removeNamedItem(const QString &arg0)
{
  AQ_CALL_RET_PTR(removeNamedItem(arg0), QDomNode);
}
inline QDomNode *AQSDomNamedNodeMap::item(int arg0) const
{
  AQ_CALL_RET_PTR(item(arg0), QDomNode);
}
inline QDomNode *AQSDomNamedNodeMap::namedItemNS(const QString &arg0,  const QString &arg1) const
{
  AQ_CALL_RET_PTR(namedItemNS(arg0, arg1), QDomNode);
}
inline QDomNode *AQSDomNamedNodeMap::setNamedItemNS(QDomNode *arg0)
{
  AQ_CALL_RET_PTR(setNamedItemNS(*arg0), QDomNode);
}
inline QDomNode *AQSDomNamedNodeMap::setNamedItemNS(AQSDomNode *arg0)
{
  AQ_CALL_RET_PTR(setNamedItemNS(*arg0), QDomNode);
}
inline QDomNode *AQSDomNamedNodeMap::removeNamedItemNS(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET_PTR(removeNamedItemNS(arg0, arg1), QDomNode);
}
inline uint AQSDomNamedNodeMap::length() const
{
  AQ_CALL_RET_V(length(), uint);
}
inline uint AQSDomNamedNodeMap::count() const
{
  AQ_CALL_RET_V(count(), uint);
}
inline bool AQSDomNamedNodeMap::contains(const QString &arg0) const
{
  AQ_CALL_RET_V(contains(arg0), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDOMNAMEDNODEMAP_P_H_ */
// @AQSVOIDPTR@
