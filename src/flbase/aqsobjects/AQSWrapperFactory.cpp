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

AQSWrapperFactory::AQSWrapperFactory()
{
  d = new AQSWrapperFactoryPrivate;

  AQ_GEN_REG_WRAP

  //### Remove in AbanQ v3
#define AQ_REG_COMPAT_FL(Class) \
  registerWrapper(AQ_QUOTEME(FL##Class), AQ_QUOTEME(AQS##Class))
#define AQ_REG_COMPAT_FL2(FClass,Class) \
  registerWrapper(AQ_QUOTEME(FL##FClass), AQ_QUOTEME(AQS##Class))

  AQ_REG_COMPAT_FL(SqlDatabase);
  AQ_REG_COMPAT_FL(Manager);
  AQ_REG_COMPAT_FL(ManagerModules);
  AQ_REG_COMPAT_FL(Action);
  AQ_REG_COMPAT_FL(SqlCursor);
  AQ_REG_COMPAT_FL(SqlQuery);
  AQ_REG_COMPAT_FL(FieldDB);
  AQ_REG_COMPAT_FL(TableDB);
  AQ_REG_COMPAT_FL(FormDB);
  AQ_REG_COMPAT_FL(FormRecordDB);
  AQ_REG_COMPAT_FL(FormSearchDB);
  AQ_REG_COMPAT_FL2(DataTable, DataTableDB);
  AQ_REG_COMPAT_FL2(RelationMetaData, RelationMD);
  AQ_REG_COMPAT_FL2(FieldMetaData, FieldMD);
  AQ_REG_COMPAT_FL2(TableMetaData, TableMD);
  //### Remove in AbanQ v3
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
  if (className == AQ_QUOTEME(FL##Class)) \
    return new AQS##Class(static_cast<QObject *>(ptr))
#define AQ_CRE_COMPAT_FL2(FClass,Class) \
  if (className == AQ_QUOTEME(FL##FClass)) \
    return new AQS##Class(static_cast<QObject *>(ptr))

  AQ_CRE_COMPAT_FL(SqlDatabase);
  AQ_CRE_COMPAT_FL(Manager);
  AQ_CRE_COMPAT_FL(ManagerModules);
  AQ_CRE_COMPAT_FL(ManagerModules);
  AQ_CRE_COMPAT_FL(SqlCursor);
  AQ_CRE_COMPAT_FL(SqlQuery);
  AQ_CRE_COMPAT_FL(FieldDB);
  AQ_CRE_COMPAT_FL(TableDB);
  AQ_CRE_COMPAT_FL(FormDB);
  AQ_CRE_COMPAT_FL(FormRecordDB);
  AQ_CRE_COMPAT_FL(FormSearchDB);
  AQ_CRE_COMPAT_FL2(DataTable, DataTableDB);
  AQ_CRE_COMPAT_FL2(RelationMetaData, RelationMD);
  AQ_CRE_COMPAT_FL2(FieldMetaData, FieldMD);
  AQ_CRE_COMPAT_FL2(TableMetaData, TableMD);
  //### Remove in AbanQ v3

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
