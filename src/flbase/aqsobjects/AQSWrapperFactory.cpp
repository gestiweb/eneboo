/***************************************************************************
 AQSWrapperFactory.cpp
 -------------------
 begin                : 29/01/2011
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

#include "AQSWrapperFactory.h"
#include "AQS_g.h"
#include "AQSWrapperFactory_p.h"

#include "AQConfig.h"

QSWrapperFactory *globalAQSWrapper = 0;

AQSWrapperFactory::AQSWrapperFactory()
{
  d = new AQSWrapperFactoryPrivate;

  globalAQSWrapper = this;

  AQ_GEN_REG_WRAP

  //### Remove in AbanQ v3
#define AQ_REG_COMPAT_FL(Class) \
  registerWrapper(AQ_QUOTEME(FL##Class), AQ_QUOTEME(AQS##Class))
#define AQ_REG_COMPAT_FL2(FClass,Class) \
  registerWrapper(AQ_QUOTEME(FL##FClass), AQ_QUOTEME(AQS##Class))

  AQ_REG_COMPAT_FL(SqlDatabase);
  AQ_REG_COMPAT_FL(Manager);
  AQ_REG_COMPAT_FL(ManagerModules);
  AQ_REG_COMPAT_FL(SqlCursor);
  AQ_REG_COMPAT_FL(SqlQuery);
  AQ_REG_COMPAT_FL(FieldDB);
  AQ_REG_COMPAT_FL(TableDB);
  AQ_REG_COMPAT_FL(FormDB);
  AQ_REG_COMPAT_FL(FormRecordDB);
  AQ_REG_COMPAT_FL(FormSearchDB);
  AQ_REG_COMPAT_FL2(Action, ActionMD);
  AQ_REG_COMPAT_FL2(RelationMetaData, RelationMD);
  AQ_REG_COMPAT_FL2(FieldMetaData, FieldMD);
  AQ_REG_COMPAT_FL2(TableMetaData, TableMD);
  AQ_REG_COMPAT_FL2(DataTable, DataTableDB);
  AQ_REG_COMPAT_FL(LineEdit);
  AQ_REG_COMPAT_FL(DateEdit);
  AQ_REG_COMPAT_FL(SpinBox);
  AQ_REG_COMPAT_FL(Table);
  //### Remove in AbanQ v3

  // From wrap internal Qt classes
#define AQ_REG_INTERNAL_QT(QClass,Class) \
  registerWrapper(AQ_QUOTEME(Q##QClass), AQ_QUOTEME(Q##Class))

  AQ_REG_INTERNAL_QT(TableHeader, Header);
}

AQSWrapperFactory::~AQSWrapperFactory()
{
  delete d;
}

QObject *AQSWrapperFactory::create(const QString &className, void *ptr)
{
  QObject *ret = staticCreate(className, ptr);
  if (ret)
    return (ret == ptr ? 0 : ret);

  QString errMsg(
    QString::fromLatin1("Failed to create wrapper for class %1")
    .arg(className)
  );
  throwError(errMsg);
  qWarning(errMsg);

  return 0;
}

QObject *AQSWrapperFactory::staticCreate(const QString &className, void *ptr)
{
  AQ_GEN_CRE_WRAP

  //### Remove in AbanQ v3
#define AQ_CRE_COMPAT_FL(Class) \
  if (className == AQ_QUOTEME(FL##Class)) { \
    AQS##Class *aqo = new AQS##Class(ptr); \
    aqo->setWrap(); \
    return aqo; \
  }

#define AQ_CRE_COMPAT_FL2(FClass,Class) \
  if (className == AQ_QUOTEME(FL##FClass)) { \
    AQS##Class *aqo = new AQS##Class(ptr); \
    aqo->setWrap(); \
    return aqo; \
  }

#define AQ_CRE_COMPAT_FL_OBJ(Class) \
  if (className == AQ_QUOTEME(FL##Class)) { \
    QObject *o = static_cast<QObject *>(ptr); \
    if (qstrcmp(o->className(), AQ_QUOTEME(FL##Class)) != 0) return o; \
    AQS##Class *qo = new AQS##Class(o); \
    qo->setWrap(); \
    return qo; \
  }

#define AQ_CRE_COMPAT_FL2_OBJ(FClass,Class) \
  if (className == AQ_QUOTEME(FL##FClass)) { \
    QObject *o = static_cast<QObject *>(ptr); \
    if (qstrcmp(o->className(), AQ_QUOTEME(FL##FClass)) != 0) return o; \
    AQS##Class *qo = new AQS##Class(o); \
    qo->setWrap(); \
    return qo; \
  }

  AQ_CRE_COMPAT_FL(SqlDatabase);
  AQ_CRE_COMPAT_FL(Manager);
  AQ_CRE_COMPAT_FL(ManagerModules);
  AQ_CRE_COMPAT_FL_OBJ(SqlCursor);
  AQ_CRE_COMPAT_FL_OBJ(SqlQuery);
  AQ_CRE_COMPAT_FL_OBJ(FieldDB);
  AQ_CRE_COMPAT_FL_OBJ(TableDB);
  AQ_CRE_COMPAT_FL_OBJ(FormDB);
  AQ_CRE_COMPAT_FL_OBJ(FormRecordDB);
  AQ_CRE_COMPAT_FL_OBJ(FormSearchDB);
  AQ_CRE_COMPAT_FL2(Action, ActionMD);
  AQ_CRE_COMPAT_FL2(RelationMetaData, RelationMD);
  AQ_CRE_COMPAT_FL2(FieldMetaData, FieldMD);
  AQ_CRE_COMPAT_FL2_OBJ(TableMetaData, TableMD);
  AQ_CRE_COMPAT_FL2_OBJ(DataTable, DataTableDB);
  AQ_CRE_COMPAT_FL_OBJ(LineEdit);
  AQ_CRE_COMPAT_FL_OBJ(DateEdit);
  AQ_CRE_COMPAT_FL_OBJ(SpinBox);
  AQ_CRE_COMPAT_FL_OBJ(Table);
  //### Remove in AbanQ v3

  // From wrap internal Qt classes
#define AQ_CRE_INTERNAL_QT_OBJ(QClass,Class) \
  if (className == AQ_QUOTEME(Q##QClass)) { \
    QObject *o = static_cast<QObject *>(ptr); \
    if (qstrcmp(o->className(), AQ_QUOTEME(Q##QClass)) != 0) return o; \
    AQS##Class *qo = new AQS##Class(o); \
    qo->setWrap(); \
    return qo; \
  }

  AQ_CRE_INTERNAL_QT_OBJ(TableHeader, Header);

  return 0;
}

static inline void objectChildrenXml(AQSObject *o, QDomNode *doc,
                                     bool includeComplexTypes)
{
  const QObjectList *children = static_cast<QObject *>(o->o())->children();
  if (!children)
    return;
  QObjectListIt it(*children);
  QObject *obj;
  AQSObject *ito;
  QDomNode *itd;

  while ((obj = it.current())) {
    ++it;

    ito = ::qt_cast<AQSObject *>(AQSWrapperFactory::staticCreate(obj->className(), obj));
    if (!ito)
      continue;

    ito->setObjectName(obj->name());
    itd = ito->toXml(includeComplexTypes);
    objectChildrenXml(ito, itd, includeComplexTypes);
    doc->firstChild().appendChild(itd->firstChild());
    delete ito;
  }
}

QDomNode *AQS::toXml(AQSObject *o, bool includeChildren,
                     bool includeComplexTypes) const
{
  QDomNode *doc = o->toXml(includeComplexTypes);
  if (!includeChildren)
    return doc;
  objectChildrenXml(o, doc, includeComplexTypes);
  return doc;
}

QDomNode *AQS::toXml(QObject *o, bool includeChildren,
                     bool includeComplexTypes) const
{
  AQSObject *aqo = ::qt_cast<AQSObject *>(findAQSObject(o));
  if (!aqo)
    return 0;
  return toXml(aqo, includeChildren, includeComplexTypes);
}

QByteArray AQS::toHex(QByteArray *ba) const
{
  return ba ? byteArrayToHex(ba) : QByteArray();
}

QByteArray AQS::fromHex(QByteArray *ba) const
{
  return ba ? byteArrayFromHex(ba) : QByteArray();
}

QByteArray AQS::toBase64(QByteArray *ba) const
{
  return ba ? byteArrayToBase64(ba) : QByteArray();
}

QByteArray AQS::fromBase64(QByteArray *ba) const
{
  return ba ? byteArrayFromBase64(ba) : QByteArray();
}

QByteArray AQS::compress(QByteArray *ba) const
{
  return ba ? qCompress(*ba) : QByteArray();
}

QByteArray AQS::uncompress(QByteArray *ba) const
{
  return ba ? qUncompress(*ba) : QByteArray();
}

QByteArray AQS::encryptInternal(QByteArray *ba) const
{
  if (!ba)
    return QByteArray();

  AQ_ENC_KEY;
  AQ_ENC_KEYVI;

  QByteArray key(32);
  QByteArray vi(32);

  for (int i = 0; i < 32; ++i)
    key[i] = enckey[i];
  for (int i = 0; i < 32; ++i)
    vi[i] = enckeyvi[i];

  QByteArray bac(qCompress(*ba));
  return aes_256_encrypt(&bac, key, vi);
}

QByteArray AQS::decryptInternal(QByteArray *ba) const
{
  if (!ba)
    return QByteArray();

  AQ_ENC_KEY;
  AQ_ENC_KEYVI;

  QByteArray key(32);
  QByteArray vi(32);

  for (int i = 0; i < 32; ++i)
    key[i] = enckey[i];
  for (int i = 0; i < 32; ++i)
    vi[i] = enckeyvi[i];

  QByteArray bac(aes_256_decrypt(ba, key, vi));
  return qUncompress(bac);
}

QString AQS::sha1(QByteArray *ba) const
{
  if (!ba)
    return QByteArray();
  return FLUtil::usha1((uchar *)(const char *)(*ba), ba->size());
}

extern "C" {
  int xsltprocMain(int argc, char **argv);
  int xsltprocMemory(const char *xsltStr, int sizeXslt,
                     const char *xmlStr, int sizeXml,
                     char **out, int *len);
  int xsltprocMemoryFile(const char *xsltStr, int sizeXslt,
                         const char *filename,
                         char **out, int *len);
  int xsltprocFile(const char *xsltStr, int sizeXslt,
                   const char *filename,
                   const char *output);
}

int AQS::xsltproc(const QStringList &args) const
{
  if (args.isEmpty())
    return 0;

  int argc = args.size() + 1;
  char *argv[argc];

  argv[0] = "xsltproc";
  for (int i = 1; i < argc; ++i) {
    argv[i] = qstrdup((const char *)args[i - 1]);
  }

  int ret = xsltprocMain(argc, argv);
  delete [] argv;
  return ret;
}

int AQS::xsltproc(QByteArray *xslt, const QString &fileName,
                  const QString &output) const
{
  int ret;
  ret = xsltprocFile((const char *)xslt->data(), xslt->size(),
                     (const char *)fileName,
                     (const char *)output);
  return ret;
}

QByteArray AQS::xsltproc(QByteArray *xslt, QByteArray *xml) const
{
  int len;
  char *out = NULL;
  xsltprocMemory((const char *)xslt->data(), xslt->size(),
                 (const char *)xml->data(), xml->size(),
                 &out, &len);
  QByteArray ret;
  if (out != NULL) {
    ret.duplicate(out, len);
    delete out;
  }
  return ret;
}

QByteArray AQS::xsltproc(QByteArray *xslt, const QString &fileName) const
{
  int len;
  char *out = NULL;
  xsltprocMemoryFile((const char *)xslt->data(), xslt->size(),
                     (const char *)fileName,
                     &out, &len);
  QByteArray ret;
  if (out != NULL) {
    ret.duplicate(out, len);
    delete out;
  }
  return ret;
}

void AQS::setTabOrder(QWidget *first, QWidget *second)
{
  QWidget::setTabOrder(first, second);
}

void AQS::setTabOrder(AQSWidget *first, AQSWidget *second)
{
  QWidget::setTabOrder((QWidget *)first, (QWidget *)second);
}
