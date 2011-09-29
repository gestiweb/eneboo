/***************************************************************************
 AQSDomElement_p.h
 -------------------
 begin                : 14/02/2011
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

#ifndef AQSDOMELEMENT_P_H_
#define AQSDOMELEMENT_P_H_

#include "AQSDomAttr_p.h"

// @AQ_PTR_INDIRECTION@

class AQSDomElement : public AQSDomNode
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DomElement, DomNode);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QString attribute(const QString &, const QString& = QString::null) const;
  void setAttribute(const QString &, const QString &);
  void setAttribute(const QString &, int);
  void setAttribute(const QString &, uint);
  void setAttribute(const QString &, long);
  void setAttribute(const QString &, ulong);
  void setAttribute(const QString &, double);
  void removeAttribute(const QString &);
  QDomAttr *attributeNode(const QString &);
  QDomAttr *setAttributeNode(QDomAttr *);
  QDomAttr *setAttributeNode(AQSDomAttr *);
  QDomAttr *removeAttributeNode(QDomAttr *);
  QDomAttr *removeAttributeNode(AQSDomAttr *);
  virtual QDomNodeList *elementsByTagName(const QString &) const;
  bool hasAttribute(const QString &) const;
  QString attributeNS(const QString, const QString &, const QString &) const;
  void setAttributeNS(const QString, const QString &, const QString &);
  void setAttributeNS(const QString, const QString &, int);
  void setAttributeNS(const QString, const QString &, uint);
  void setAttributeNS(const QString, const QString &, long);
  void setAttributeNS(const QString, const QString &, ulong);
  void setAttributeNS(const QString, const QString &, double);
  void removeAttributeNS(const QString &, const QString &);
  QDomAttr *attributeNodeNS(const QString &, const QString &);
  QDomAttr *setAttributeNodeNS(QDomAttr *);
  QDomAttr *setAttributeNodeNS(AQSDomAttr *);
  virtual QDomNodeList *elementsByTagNameNS(const QString &, const QString &) const;
  bool hasAttributeNS(const QString &, const QString &) const;
  QString tagName() const;
  void setTagName(const QString &);
  virtual QDomNamedNodeMap *attributes() const;
  virtual uint nodeType() const;
  virtual bool isElement() const;
  QString text() const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomElement*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDomElement;
    if (sgt == QString::fromLatin1("QDomElement*"))
      return new QDomElement(*argValue<QDomElement *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomElement*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSDomElement::attribute(const QString &arg0,  const QString &arg1) const
{
  AQ_CALL_RET_V(attribute(arg0, arg1), QString);
}
inline void AQSDomElement::setAttribute(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_VOID(setAttribute(arg0, arg1));
}
inline void AQSDomElement::setAttribute(const QString &arg0,  int arg1)
{
  AQ_CALL_VOID(setAttribute(arg0, arg1));
}
inline void AQSDomElement::setAttribute(const QString &arg0,  uint arg1)
{
  AQ_CALL_VOID(setAttribute(arg0, arg1));
}
inline void AQSDomElement::setAttribute(const QString &arg0,  long arg1)
{
  AQ_CALL_VOID(setAttribute(arg0, arg1));
}
inline void AQSDomElement::setAttribute(const QString &arg0,  ulong arg1)
{
  AQ_CALL_VOID(setAttribute(arg0, arg1));
}
inline void AQSDomElement::setAttribute(const QString &arg0,  double arg1)
{
  AQ_CALL_VOID(setAttribute(arg0, arg1));
}
inline void AQSDomElement::removeAttribute(const QString &arg0)
{
  AQ_CALL_VOID(removeAttribute(arg0));
}
inline QDomAttr *AQSDomElement::attributeNode(const QString &arg0)
{
  AQ_CALL_RET_PTR(attributeNode(arg0), QDomAttr);
}
inline QDomAttr *AQSDomElement::setAttributeNode(QDomAttr *arg0)
{
  AQ_CALL_RET_PTR(setAttributeNode(*arg0), QDomAttr);
}
inline QDomAttr *AQSDomElement::setAttributeNode(AQSDomAttr *arg0)
{
  AQ_CALL_RET_PTR(setAttributeNode(*arg0), QDomAttr);
}
inline QDomAttr *AQSDomElement::removeAttributeNode(QDomAttr *arg0)
{
  AQ_CALL_RET_PTR(removeAttributeNode(*arg0), QDomAttr);
}
inline QDomAttr *AQSDomElement::removeAttributeNode(AQSDomAttr *arg0)
{
  AQ_CALL_RET_PTR(removeAttributeNode(*arg0), QDomAttr);
}
inline QDomNodeList *AQSDomElement::elementsByTagName(const QString &arg0) const
{
  AQ_CALL_RET_PTR(elementsByTagName(arg0), QDomNodeList);
}
inline bool AQSDomElement::hasAttribute(const QString &arg0) const
{
  AQ_CALL_RET_V(hasAttribute(arg0), bool);
}
inline QString AQSDomElement::attributeNS(const QString arg0,  const QString &arg1,  const QString &arg2) const
{
  AQ_CALL_RET_V(attributeNS(arg0, arg1, arg2), QString);
}
inline void AQSDomElement::setAttributeNS(const QString arg0,  const QString &arg1,  const QString &arg2)
{
  AQ_CALL_VOID(setAttributeNS(arg0, arg1, arg2));
}
inline void AQSDomElement::setAttributeNS(const QString arg0,  const QString &arg1,  int arg2)
{
  AQ_CALL_VOID(setAttributeNS(arg0, arg1, arg2));
}
inline void AQSDomElement::setAttributeNS(const QString arg0,  const QString &arg1,  uint arg2)
{
  AQ_CALL_VOID(setAttributeNS(arg0, arg1, arg2));
}
inline void AQSDomElement::setAttributeNS(const QString arg0,  const QString &arg1,  long arg2)
{
  AQ_CALL_VOID(setAttributeNS(arg0, arg1, arg2));
}
inline void AQSDomElement::setAttributeNS(const QString arg0,  const QString &arg1,  ulong arg2)
{
  AQ_CALL_VOID(setAttributeNS(arg0, arg1, arg2));
}
inline void AQSDomElement::setAttributeNS(const QString arg0,  const QString &arg1,  double arg2)
{
  AQ_CALL_VOID(setAttributeNS(arg0, arg1, arg2));
}
inline void AQSDomElement::removeAttributeNS(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_VOID(removeAttributeNS(arg0, arg1));
}
inline QDomAttr *AQSDomElement::attributeNodeNS(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET_PTR(attributeNodeNS(arg0, arg1), QDomAttr);
}
inline QDomAttr *AQSDomElement::setAttributeNodeNS(QDomAttr *arg0)
{
  AQ_CALL_RET_PTR(setAttributeNodeNS(*arg0), QDomAttr);
}
inline QDomAttr *AQSDomElement::setAttributeNodeNS(AQSDomAttr *arg0)
{
  AQ_CALL_RET_PTR(setAttributeNodeNS(*arg0), QDomAttr);
}
inline QDomNodeList *AQSDomElement::elementsByTagNameNS(const QString &arg0,  const QString &arg1) const
{
  AQ_CALL_RET_PTR(elementsByTagNameNS(arg0, arg1), QDomNodeList);
}
inline bool AQSDomElement::hasAttributeNS(const QString &arg0,  const QString &arg1) const
{
  AQ_CALL_RET_V(hasAttributeNS(arg0, arg1), bool);
}
inline QString AQSDomElement::tagName() const
{
  AQ_CALL_RET_V(tagName(), QString);
}
inline void AQSDomElement::setTagName(const QString &arg0)
{
  AQ_CALL_VOID(setTagName(arg0));
}
inline QDomNamedNodeMap *AQSDomElement::attributes() const
{
  AQ_CALL_RET_PTR(attributes(), QDomNamedNodeMap);
}
inline uint AQSDomElement::nodeType() const
{
  AQ_CALL_RET_V(nodeType(), uint);
}
inline bool AQSDomElement::isElement() const
{
  AQ_CALL_RET_V(isElement(), bool);
}
inline QString AQSDomElement::text() const
{
  AQ_CALL_RET_V(text(), QString);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDOMELEMENT_P_H_ */
// @AQSVOIDPTR@
