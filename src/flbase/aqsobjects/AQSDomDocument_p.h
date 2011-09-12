/***************************************************************************
 AQSDomDocument_p.h
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

#ifndef AQSDOMDOCUMENT_P_H_
#define AQSDOMDOCUMENT_P_H_

#include "AQSDomNode_p.h"
#include "AQSByteArray_p.h"

// @AQ_PTR_INDIRECTION@

class AQSDomDocument : public AQSDomNode
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(DomDocument, DomNode);

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QDomNode *createElement(const QString &);
  QDomNode *createDocumentFragment();
  QDomNode *createTextNode(const QString &);
  QDomNode *createComment(const QString &);
  QDomNode *createCDATASection(const QString &);
  QDomNode *createProcessingInstruction(const QString &, const QString &);
  QDomNode *createAttribute(const QString &);
  QDomNode *createEntityReference(const QString &);
  QDomNodeList *elementsByTagName(const QString &) const;
  QDomNode *importNode(QDomNode *, bool);
  QDomNode *importNode(AQSDomNode *, bool);
  QDomNode *createElementNS(const QString &, const QString &);
  QDomNode *createAttributeNS(const QString &, const QString &);
  QDomNodeList *elementsByTagNameNS(const QString &, const QString &);
  QDomNode *elementById(const QString &);
  QDomNode *doctype() const;
  QDomImplementation *implementation() const;
  QDomNode *documentElement() const;
  bool setContent(QByteArray *, bool, QString* = 0, int* = 0, int* = 0);
  bool setContent(AQSByteArray *, bool, QString* = 0, int* = 0, int* = 0);
  bool setContent(const QString &, bool, QString* = 0, int* = 0, int* = 0);
  bool setContent(QByteArray *, QString* = 0, int* = 0, int* = 0);
  bool setContent(AQSByteArray *, QString* = 0, int* = 0, int* = 0);
  bool setContent(const QString &, QString* = 0, int* = 0, int* = 0);
  virtual uint nodeType() const;
  virtual bool isDocument() const;
  QString toString() const;
  QString toString(int) const;

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[1].append(QString::fromLatin1("QDomDocumentType*"));
    candidates[1].append(QString::fromLatin1("QDomDocument*"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QDomDocument;
    if (sgt == QString::fromLatin1("QString"))
      return new QDomDocument(*(argValue<QString *>(args[0])));
    if (sgt == QString::fromLatin1("QDomDocumentType*"))
      return new QDomDocument(*argValue<QDomDocumentType *>(args[0]));
    if (sgt == QString::fromLatin1("QDomDocument*"))
      return new QDomDocument(*argValue<QDomDocument *>(args[0]));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QString"));
    candidates[1].append(QString::fromLatin1("QDomDocumentType*"));
    candidates[1].append(QString::fromLatin1("QDomDocument*"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QDomNode *AQSDomDocument::createElement(const QString &arg0)
{
  AQ_CALL_RET_PTR(createElement(arg0), QDomNode);
}
inline QDomNode *AQSDomDocument::createDocumentFragment()
{
  AQ_CALL_RET_PTR(createDocumentFragment(), QDomNode);
}
inline QDomNode *AQSDomDocument::createTextNode(const QString &arg0)
{
  AQ_CALL_RET_PTR(createTextNode(arg0), QDomNode);
}
inline QDomNode *AQSDomDocument::createComment(const QString &arg0)
{
  AQ_CALL_RET_PTR(createComment(arg0), QDomNode);
}
inline QDomNode *AQSDomDocument::createCDATASection(const QString &arg0)
{
  AQ_CALL_RET_PTR(createCDATASection(arg0), QDomNode);
}
inline QDomNode *AQSDomDocument::createProcessingInstruction(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET_PTR(createProcessingInstruction(arg0, arg1), QDomNode);
}
inline QDomNode *AQSDomDocument::createAttribute(const QString &arg0)
{
  AQ_CALL_RET_PTR(createAttribute(arg0), QDomNode);
}
inline QDomNode *AQSDomDocument::createEntityReference(const QString &arg0)
{
  AQ_CALL_RET_PTR(createEntityReference(arg0), QDomNode);
}
inline QDomNodeList *AQSDomDocument::elementsByTagName(const QString &arg0) const
{
  AQ_CALL_RET_PTR(elementsByTagName(arg0), QDomNodeList);
}
inline QDomNode *AQSDomDocument::importNode(QDomNode *arg0,  bool arg1)
{
  AQ_CALL_RET_PTR(importNode(*arg0, arg1), QDomNode);
}
inline QDomNode *AQSDomDocument::importNode(AQSDomNode *arg0,  bool arg1)
{
  AQ_CALL_RET_PTR(importNode(*arg0, arg1), QDomNode);
}
inline QDomNode *AQSDomDocument::createElementNS(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET_PTR(createElementNS(arg0, arg1), QDomNode);
}
inline QDomNode *AQSDomDocument::createAttributeNS(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET_PTR(createAttributeNS(arg0, arg1), QDomNode);
}
inline QDomNodeList *AQSDomDocument::elementsByTagNameNS(const QString &arg0,  const QString &arg1)
{
  AQ_CALL_RET_PTR(elementsByTagNameNS(arg0, arg1), QDomNodeList);
}
inline QDomNode *AQSDomDocument::elementById(const QString &arg0)
{
  AQ_CALL_RET_PTR(elementById(arg0), QDomNode);
}
inline QDomNode *AQSDomDocument::doctype() const
{
  AQ_CALL_RET_PTR(doctype(), QDomNode);
}
inline QDomImplementation *AQSDomDocument::implementation() const
{
  AQ_CALL_RET_PTR(implementation(), QDomImplementation);
}
inline QDomNode *AQSDomDocument::documentElement() const
{
  AQ_CALL_RET_PTR(documentElement(), QDomNode);
}
inline bool AQSDomDocument::setContent(QByteArray *arg0,  bool arg1,  QString *arg2,  int *arg3,  int *arg4)
{
  AQ_CALL_RET_V(setContent(*arg0, arg1, arg2, arg3, arg4), bool);
}
inline bool AQSDomDocument::setContent(AQSByteArray *arg0,  bool arg1,  QString *arg2,  int *arg3,  int *arg4)
{
  AQ_CALL_RET_V(setContent(*arg0, arg1, arg2, arg3, arg4), bool);
}
inline bool AQSDomDocument::setContent(const QString &arg0,  bool arg1,  QString *arg2,  int *arg3,  int *arg4)
{
  AQ_CALL_RET_V(setContent(arg0, arg1, arg2, arg3, arg4), bool);
}
inline bool AQSDomDocument::setContent(QByteArray *arg0,  QString *arg1,  int *arg2,  int *arg3)
{
  AQ_CALL_RET_V(setContent(*arg0, arg1, arg2, arg3), bool);
}
inline bool AQSDomDocument::setContent(AQSByteArray *arg0,  QString *arg1,  int *arg2,  int *arg3)
{
  AQ_CALL_RET_V(setContent(*arg0, arg1, arg2, arg3), bool);
}
inline bool AQSDomDocument::setContent(const QString &arg0,  QString *arg1,  int *arg2,  int *arg3)
{
  AQ_CALL_RET_V(setContent(arg0, arg1, arg2, arg3), bool);
}
inline uint AQSDomDocument::nodeType() const
{
  AQ_CALL_RET_V(nodeType(), uint);
}
inline bool AQSDomDocument::isDocument() const
{
  AQ_CALL_RET_V(isDocument(), bool);
}
inline QString AQSDomDocument::toString() const
{
  AQ_CALL_RET_V(toString(), QString);
}
inline QString AQSDomDocument::toString(int arg0) const
{
  AQ_CALL_RET_V(toString(arg0), QString);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSDOMDOCUMENT_P_H_ */
// @AQSVOIDPTR@
