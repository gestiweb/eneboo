/***************************************************************************
 AQSDomNode_p.h
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

#ifndef AQSDOMNODE_P_H_
#define AQSDOMNODE_P_H_

#include "AQSTextStream_p.h"

// @AQ_PTR_INDIRECTION@

class AQSDomNode : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DomNode, VoidPtr);

protected:
  void specializedInternalFinish() {
    if (!wrap_)
      delete o_;
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  virtual QDomNode *insertBefore(QDomNode *, QDomNode *);
  virtual QDomNode *insertBefore(QDomNode *, AQSDomNode *);
  virtual QDomNode *insertBefore(AQSDomNode *, QDomNode *);
  virtual QDomNode *insertBefore(AQSDomNode *, AQSDomNode *);
  virtual QDomNode *insertAfter(QDomNode *, QDomNode *);
  virtual QDomNode *insertAfter(QDomNode *, AQSDomNode *);
  virtual QDomNode *insertAfter(AQSDomNode *, QDomNode *);
  virtual QDomNode *insertAfter(AQSDomNode *, AQSDomNode *);
  virtual QDomNode *replaceChild(QDomNode *, QDomNode *);
  virtual QDomNode *replaceChild(QDomNode *, AQSDomNode *);
  virtual QDomNode *replaceChild(AQSDomNode *, QDomNode *);
  virtual QDomNode *replaceChild(AQSDomNode *, AQSDomNode *);
  virtual QDomNode *removeChild(QDomNode *);
  virtual QDomNode *removeChild(AQSDomNode *);
  virtual QDomNode *appendChild(QDomNode *);
  virtual QDomNode *appendChild(AQSDomNode *);
  virtual bool hasChildNodes() const;
  virtual QDomNode *cloneNode(bool = TRUE) const;
  virtual void normalize();
  virtual bool isSupported(const QString &, const QString &) const;
  virtual QString nodeName() const;
  virtual uint nodeType() const;
  virtual QDomNode *parentNode() const;
  virtual QDomNodeList *childNodes() const;
  virtual QDomNode *firstChild() const;
  virtual QDomNode *lastChild() const;
  virtual QDomNode *previousSibling() const;
  virtual QDomNode *nextSibling() const;
  virtual QDomNamedNodeMap *attributes() const;
  virtual QDomDocument *ownerDocument() const;
  virtual QString namespaceURI() const;
  virtual QString localName() const;
  virtual bool hasAttributes() const;
  virtual QString nodeValue() const;
  virtual void setNodeValue(const QString &);
  virtual QString prefix() const;
  virtual void setPrefix(const QString &);
  virtual bool isAttr() const;
  virtual bool isCDATASection() const;
  virtual bool isDocumentFragment() const;
  virtual bool isDocument() const;
  virtual bool isDocumentType() const;
  virtual bool isElement() const;
  virtual bool isEntityReference() const;
  virtual bool isText() const;
  virtual bool isEntity() const;
  virtual bool isNotation() const;
  virtual bool isProcessingInstruction() const;
  virtual bool isCharacterData() const;
  virtual bool isComment() const;
  QDomNode *namedItem(const QString &) const;
  bool isNull() const;
  void clear();
  QDomAttr *toAttr();
  QDomCDATASection *toCDATASection();
  QDomDocumentFragment *toDocumentFragment();
  QDomDocument *toDocument();
  QDomDocumentType *toDocumentType();
  QDomElement *toElement();
  QDomEntityReference *toEntityReference();
  QDomText *toText();
  QDomEntity *toEntity();
  QDomNotation *toNotation();
  QDomProcessingInstruction *toProcessingInstruction();
  QDomCharacterData *toCharacterData();
  QDomComment *toComment();
  void save(QTextStream *, int) const;
  void save(AQSTextStream *, int) const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomNode*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDomNode;
    if (sgt == QString::fromLatin1("QDomNode*"))
      return new QDomNode(*argValue<QDomNode *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QDomNode*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QDomNode *AQSDomNode::insertBefore(QDomNode *arg0,  QDomNode *arg1)
{
  AQ_CALL_RET_PTR(insertBefore(*arg0, *arg1), QDomNode);
}
inline QDomNode *AQSDomNode::insertBefore(QDomNode *arg0, AQSDomNode *arg1)
{
  AQ_CALL_RET_PTR(insertBefore(*arg0, *arg1), QDomNode);
}
inline QDomNode *AQSDomNode::insertBefore(AQSDomNode *arg0,  QDomNode *arg1)
{
  AQ_CALL_RET_PTR(insertBefore(*arg0, *arg1), QDomNode);
}
inline QDomNode *AQSDomNode::insertBefore(AQSDomNode *arg0, AQSDomNode *arg1)
{
  AQ_CALL_RET_PTR(insertBefore(*arg0, *arg1), QDomNode);
}
inline QDomNode *AQSDomNode::insertAfter(QDomNode *arg0,  QDomNode *arg1)
{
  AQ_CALL_RET_PTR(insertAfter(*arg0, *arg1), QDomNode);
}
inline QDomNode *AQSDomNode::insertAfter(QDomNode *arg0, AQSDomNode *arg1)
{
  AQ_CALL_RET_PTR(insertAfter(*arg0, *arg1), QDomNode);
}
inline QDomNode *AQSDomNode::insertAfter(AQSDomNode *arg0,  QDomNode *arg1)
{
  AQ_CALL_RET_PTR(insertAfter(*arg0, *arg1), QDomNode);
}
inline QDomNode *AQSDomNode::insertAfter(AQSDomNode *arg0, AQSDomNode *arg1)
{
  AQ_CALL_RET_PTR(insertAfter(*arg0, *arg1), QDomNode);
}
inline QDomNode *AQSDomNode::replaceChild(QDomNode *arg0,  QDomNode *arg1)
{
  AQ_CALL_RET_PTR(replaceChild(*arg0, *arg1), QDomNode);
}
inline QDomNode *AQSDomNode::replaceChild(QDomNode *arg0, AQSDomNode *arg1)
{
  AQ_CALL_RET_PTR(replaceChild(*arg0, *arg1), QDomNode);
}
inline QDomNode *AQSDomNode::replaceChild(AQSDomNode *arg0,  QDomNode *arg1)
{
  AQ_CALL_RET_PTR(replaceChild(*arg0, *arg1), QDomNode);
}
inline QDomNode *AQSDomNode::replaceChild(AQSDomNode *arg0, AQSDomNode *arg1)
{
  AQ_CALL_RET_PTR(replaceChild(*arg0, *arg1), QDomNode);
}
inline QDomNode *AQSDomNode::removeChild(QDomNode *arg0)
{
  AQ_CALL_RET_PTR(removeChild(*arg0), QDomNode);
}
inline QDomNode *AQSDomNode::removeChild(AQSDomNode *arg0)
{
  AQ_CALL_RET_PTR(removeChild(*arg0), QDomNode);
}
inline QDomNode *AQSDomNode::appendChild(QDomNode *arg0)
{
  AQ_CALL_RET_PTR(appendChild(*arg0), QDomNode);
}
inline QDomNode *AQSDomNode::appendChild(AQSDomNode *arg0)
{
  AQ_CALL_RET_PTR(appendChild(*arg0), QDomNode);
}
inline bool AQSDomNode::hasChildNodes() const
{
  AQ_CALL_RET_V(hasChildNodes(), bool);
}
inline QDomNode *AQSDomNode::cloneNode(bool arg0) const
{
  AQ_CALL_RET_PTR(cloneNode(arg0), QDomNode);
}
inline void AQSDomNode::normalize()
{
  AQ_CALL_VOID(normalize());
}
inline bool AQSDomNode::isSupported(const QString &arg0,  const QString &arg1) const
{
  AQ_CALL_RET_V(isSupported(arg0, arg1), bool);
}
inline QString AQSDomNode::nodeName() const
{
  AQ_CALL_RET_V(nodeName(), QString);
}
inline uint AQSDomNode::nodeType() const
{
  AQ_CALL_RET_V(nodeType(), uint);
}
inline QDomNode *AQSDomNode::parentNode() const
{
  AQ_CALL_RET_PTR(parentNode(), QDomNode);
}
inline QDomNodeList *AQSDomNode::childNodes() const
{
  AQ_CALL_RET_PTR(childNodes(), QDomNodeList);
}
inline QDomNode *AQSDomNode::firstChild() const
{
  AQ_CALL_RET_PTR(firstChild(), QDomNode);
}
inline QDomNode *AQSDomNode::lastChild() const
{
  AQ_CALL_RET_PTR(lastChild(), QDomNode);
}
inline QDomNode *AQSDomNode::previousSibling() const
{
  AQ_CALL_RET_PTR(previousSibling(), QDomNode);
}
inline QDomNode *AQSDomNode::nextSibling() const
{
  AQ_CALL_RET_PTR(nextSibling(), QDomNode);
}
inline QDomNamedNodeMap *AQSDomNode::attributes() const
{
  AQ_CALL_RET_PTR(attributes(), QDomNamedNodeMap);
}
inline QDomDocument *AQSDomNode::ownerDocument() const
{
  AQ_CALL_RET_PTR(ownerDocument(), QDomDocument);
}
inline QString AQSDomNode::namespaceURI() const
{
  AQ_CALL_RET_V(namespaceURI(), QString);
}
inline QString AQSDomNode::localName() const
{
  AQ_CALL_RET_V(localName(), QString);
}
inline bool AQSDomNode::hasAttributes() const
{
  AQ_CALL_RET_V(hasAttributes(), bool);
}
inline QString AQSDomNode::nodeValue() const
{
  AQ_CALL_RET_V(nodeValue(), QString);
}
inline void AQSDomNode::setNodeValue(const QString &arg0)
{
  AQ_CALL_VOID(setNodeValue(arg0));
}
inline QString AQSDomNode::prefix() const
{
  AQ_CALL_RET_V(prefix(), QString);
}
inline void AQSDomNode::setPrefix(const QString &arg0)
{
  AQ_CALL_VOID(setPrefix(arg0));
}
inline bool AQSDomNode::isAttr() const
{
  AQ_CALL_RET_V(isAttr(), bool);
}
inline bool AQSDomNode::isCDATASection() const
{
  AQ_CALL_RET_V(isCDATASection(), bool);
}
inline bool AQSDomNode::isDocumentFragment() const
{
  AQ_CALL_RET_V(isDocumentFragment(), bool);
}
inline bool AQSDomNode::isDocument() const
{
  AQ_CALL_RET_V(isDocument(), bool);
}
inline bool AQSDomNode::isDocumentType() const
{
  AQ_CALL_RET_V(isDocumentType(), bool);
}
inline bool AQSDomNode::isElement() const
{
  AQ_CALL_RET_V(isElement(), bool);
}
inline bool AQSDomNode::isEntityReference() const
{
  AQ_CALL_RET_V(isEntityReference(), bool);
}
inline bool AQSDomNode::isText() const
{
  AQ_CALL_RET_V(isText(), bool);
}
inline bool AQSDomNode::isEntity() const
{
  AQ_CALL_RET_V(isEntity(), bool);
}
inline bool AQSDomNode::isNotation() const
{
  AQ_CALL_RET_V(isNotation(), bool);
}
inline bool AQSDomNode::isProcessingInstruction() const
{
  AQ_CALL_RET_V(isProcessingInstruction(), bool);
}
inline bool AQSDomNode::isCharacterData() const
{
  AQ_CALL_RET_V(isCharacterData(), bool);
}
inline bool AQSDomNode::isComment() const
{
  AQ_CALL_RET_V(isComment(), bool);
}
inline QDomNode *AQSDomNode::namedItem(const QString &arg0) const
{
  AQ_CALL_RET_PTR(namedItem(arg0), QDomNode);
}
inline bool AQSDomNode::isNull() const
{
  AQ_CALL_RET_V(isNull(), bool);
}
inline void AQSDomNode::clear()
{
  AQ_CALL_VOID(clear());
}
inline QDomAttr *AQSDomNode::toAttr()
{
  AQ_CALL_RET_PTR(toAttr(), QDomAttr);
}
inline QDomCDATASection *AQSDomNode::toCDATASection()
{
  AQ_CALL_RET_PTR(toCDATASection(), QDomCDATASection);
}
inline QDomDocumentFragment *AQSDomNode::toDocumentFragment()
{
  AQ_CALL_RET_PTR(toDocumentFragment(), QDomDocumentFragment);
}
inline QDomDocument *AQSDomNode::toDocument()
{
  AQ_CALL_RET_PTR(toDocument(), QDomDocument);
}
inline QDomDocumentType *AQSDomNode::toDocumentType()
{
  AQ_CALL_RET_PTR(toDocumentType(), QDomDocumentType);
}
inline QDomElement *AQSDomNode::toElement()
{
  AQ_CALL_RET_PTR(toElement(), QDomElement);
}
inline QDomEntityReference *AQSDomNode::toEntityReference()
{
  AQ_CALL_RET_PTR(toEntityReference(), QDomEntityReference);
}
inline QDomText *AQSDomNode::toText()
{
  AQ_CALL_RET_PTR(toText(), QDomText);
}
inline QDomEntity *AQSDomNode::toEntity()
{
  AQ_CALL_RET_PTR(toEntity(), QDomEntity);
}
inline QDomNotation *AQSDomNode::toNotation()
{
  AQ_CALL_RET_PTR(toNotation(), QDomNotation);
}
inline QDomProcessingInstruction *AQSDomNode::toProcessingInstruction()
{
  AQ_CALL_RET_PTR(toProcessingInstruction(), QDomProcessingInstruction);
}
inline QDomCharacterData *AQSDomNode::toCharacterData()
{
  AQ_CALL_RET_PTR(toCharacterData(), QDomCharacterData);
}
inline QDomComment *AQSDomNode::toComment()
{
  AQ_CALL_RET_PTR(toComment(), QDomComment);
}
inline void AQSDomNode::save(QTextStream *arg0,  int arg1) const
{
  AQ_CALL_VOID(save(*arg0, arg1));
}
inline void AQSDomNode::save(AQSTextStream *arg0,  int arg1) const
{
  AQ_CALL_VOID(save(*arg0, arg1));
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDOMNODE_P_H_ */
// @AQSVOIDPTR@
