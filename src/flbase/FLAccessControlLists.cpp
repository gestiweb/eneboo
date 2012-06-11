/***************************************************************************
                        FLAccessControlLists.cpp
                          -------------------
 begin                : Fri Oct 10 2005
 copyright            : (C) 2004-2005 by InfoSiAL S.L.
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

#include <qdom.h>
#include <qdict.h>
#include <qstringlist.h>
#include <qprogressdialog.h>

#include "FLAccessControl.h"
#include "FLAccessControlFactory.h"
#include "FLAccessControlLists.h"
#include "FLUtil.h"
#include "FLTableMetaData.h"
#include "FLManagerModules.h"
#include "FLApplication.h"
#include "FLSqlQuery.h"
#include "FLSqlDatabase.h"
#include "FLSqlConnections.h"

FLAccessControlLists::FLAccessControlLists() : name_(QString::null), accessControlList_(0) {}

FLAccessControlLists::~FLAccessControlLists()
{
  if (accessControlList_) {
    accessControlList_->clear();
    delete accessControlList_;
  }
}

void FLAccessControlLists::init()
{
  init(FLSqlConnections::database()->managerModules()->content("acl.xml"));
}

void FLAccessControlLists::init(const QString &aclXml)
{
  QDomDocument doc("ACL");

  if (accessControlList_) {
    accessControlList_->clear();
    delete accessControlList_;
    accessControlList_ = 0;
  }

  if (!FLUtil::domDocumentSetContent(doc, aclXml)) {
#ifdef FL_DEBUG
    qWarning("FLAccessControlLists : " +
             QApplication::tr("Lista de control de acceso vacia o errónea."));
#endif
    return;
  }

  accessControlList_ = new QDict < FLAccessControl >();
  accessControlList_->setAutoDelete(true);

  QDomElement docElem = doc.documentElement();
  QDomNode no = docElem.firstChild();

  while (!no.isNull()) {
    QDomElement e = no.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "name") {
        name_ = e.text();
        no = no.nextSibling();
        continue;
      }

      FLAccessControl *ac = FLAccessControlFactory::create(e.tagName());
      if (ac) {
        ac->set(&e);
        accessControlList_->replace(ac->type() + QString::fromLatin1("::") +
                                    ac->name() + QString::fromLatin1("::") +
                                    ac->user(), ac);
//	qWarning("FLAccessControlLists::set: " +
//           ac->type() + " :: " + ac->name() + " :: " + ac->user() );
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }
}

void FLAccessControlLists::process(QObject *obj)
{
  if (!obj || !accessControlList_ || obj->aqWasDeleted())
    return;

  if (accessControlList_->isEmpty())
    return;

  QString type(FLAccessControlFactory::type(obj));
  QString name(obj->name());
  QString user(FLSqlConnections::database()->user());

//#ifdef FL_DEBUG
//  qWarning("FLAccessControlLists::process: " +
//           type + " :: " + name + " :: " + user);
//#endif

  if (type.isEmpty() || name.isEmpty() || user.isEmpty())
    return;
  FLAccessControl *ac = (*accessControlList_)[type +
                                              QString::fromLatin1("::") + name +
                                              QString::fromLatin1("::") + user];
  if (ac)
    ac->processObject(obj);
}

void FLAccessControlLists::installACL(const QString &idacl)
{
  QDomDocument doc("ACL");

  QDomElement root = doc.createElement("ACL");
  doc.appendChild(root);

  QDomElement name = doc.createElement("name");
  root.appendChild(name);
  QDomText n = doc.createTextNode(idacl);
  name.appendChild(n);

  FLSqlQuery q;

  q.setTablesList("flacs");
  q.setSelect("idac,tipo,nombre,iduser,idgroup,degrupo,permiso");
  q.setFrom("flacs");
  q.setWhere(QString::fromLatin1("idacl='") + idacl + QString::fromLatin1("'"));
  q.setOrderBy("prioridad DESC,tipo");
  q.setForwardOnly(true);

  if (q.exec()) {
    uint step = 0;
    QProgressDialog progress(QApplication::tr("Instalando control de acceso..."), 0,
                             q.size(), 0, 0, true);
    progress.setCaption(QApplication::tr("Instalando ACL"));
    progress.setMinimumDuration(0);
    progress.setProgress(++step);
    while (q.next()) {
      makeRule(&q, &doc);
      progress.setProgress(++step);
    }
    FLSqlConnections::database()->managerModules()->setContent("acl.xml", "sys",
                                                               doc.toString());
  }
}

void FLAccessControlLists::makeRule(FLSqlQuery *q, QDomDocument *d)
{
  if (!q || !d)
    return;

  if (q->value(5).toBool())
    makeRuleGroup(q, d, q->value(4).toString());
  else
    makeRuleUser(q, d, q->value(3).toString());
}

void FLAccessControlLists::makeRuleUser(FLSqlQuery *q, QDomDocument *d, const QString &iduser)
{
  if (iduser.isEmpty() || !q || !d)
    return;

  FLAccessControl *ac = FLAccessControlFactory::create(q->value(1).toString());

  if (ac) {
    ac->setName(q->value(2).toString());
    ac->setUser(iduser);
    ac->setPerm(q->value(6).toString());

    FLSqlQuery qAcos;

    qAcos.setTablesList("flacos");
    qAcos.setSelect("nombre,permiso");
    qAcos.setFrom("flacos");
    qAcos.setWhere(QString::fromLatin1("idac='") + q->value(0).toString() +
                   QString::fromLatin1("'"));
    qAcos.setForwardOnly(true);

    QStringList acos;

    if (qAcos.exec()) {
      while (qAcos.next()) {
        acos << qAcos.value(0).toString();
        acos << qAcos.value(1).toString();
      }
    }

    ac->setAcos(acos);
    ac->get(d);

    delete ac;
  }
}

void FLAccessControlLists::makeRuleGroup(FLSqlQuery *q, QDomDocument *d, const QString &idgroup)
{
  if (idgroup.isEmpty() || !q || !d)
    return;

  FLSqlQuery qU;

  qU.setTablesList("flusers");
  qU.setSelect("iduser");
  qU.setFrom("flusers");
  qU.setWhere(QString::fromLatin1("idgroup='") + idgroup + QString::fromLatin1("'"));
  qU.setForwardOnly(true);

  if (qU.exec())
    while (qU.next())
      makeRuleUser(q, d, qU.value(0).toString());
}
