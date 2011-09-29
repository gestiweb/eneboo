/***************************************************************************
                          FLAccessControl.cpp
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

#include "FLAccessControl.h"

FLAccessControl::FLAccessControl() :
  name_(QString::null), user_(QString::null),
  perm_(QString::null), acosPerms_(0) {}

FLAccessControl::~FLAccessControl()
{
  if (acosPerms_) {
    acosPerms_->clear();
    delete acosPerms_;
  }
}

void FLAccessControl::clear()
{
  name_ = QString::null;
  user_ = QString::null;
  perm_ = QString::null;
  if (acosPerms_) {
    acosPerms_->clear();
    delete acosPerms_;
    acosPerms_ = 0;
  }
}

QString FLAccessControl::type()
{
  return QString::null;
}

void FLAccessControl::set(QDomElement *e)
{
  if (!e)
    return;

  if (acosPerms_) {
    acosPerms_->clear();
    delete acosPerms_;
  }

  acosPerms_ = new QDict < QString >(31);
  acosPerms_->setAutoDelete(true);

  perm_ = e->attribute("perm");

  QDomNode no = e->firstChild();

  while (!no.isNull()) {
    QDomElement e = no.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "name") {
        name_ = e.text();
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "user") {
        user_ = e.text();
        no = no.nextSibling();
        continue;
      }
      if (e.tagName() == "aco") {
        acosPerms_->replace(e.text(), new QString(e.attribute("perm")));
        no = no.nextSibling();
        continue;
      }
    }
    no = no.nextSibling();
  }
}

void FLAccessControl::get(QDomDocument *d)
{
  if (this->type().isEmpty() || !d)
    return;

  QDomElement root = d->firstChild().toElement();
  QDomElement e = d->createElement(this->type());
  e.setAttribute("perm", perm_);
  root.appendChild(e);

  QDomElement name = d->createElement("name");
  e.appendChild(name);
  QDomText n = d->createTextNode(name_);
  name.appendChild(n);

  QDomElement user = d->createElement("user");
  e.appendChild(user);
  QDomText u = d->createTextNode(user_);
  user.appendChild(u);

  if (acosPerms_) {
    QDictIterator < QString > it(*acosPerms_);
    for (; it.current(); ++it) {
      QDomElement aco = d->createElement("aco");
      aco.setAttribute("perm", *(*it));
      e.appendChild(aco);
      QDomText t = d->createTextNode(it.currentKey());
      aco.appendChild(t);
    }
  }
}

void FLAccessControl::setAcos(const QStringList &acos)
{
  if (acos.isEmpty())
    return;

  if (acosPerms_) {
    acosPerms_->clear();
    delete acosPerms_;
  }

  acosPerms_ = new QDict < QString > (31);
  acosPerms_->setAutoDelete(true);

  QString nameAcos;
  for (QStringList::const_iterator it = acos.begin(); it != acos.end(); ++it) {
    nameAcos = *it;
    ++it;
    acosPerms_->replace(nameAcos, new QString(*it));
  }
}

QStringList FLAccessControl::getAcos()
{
  QStringList acos;

  if (acosPerms_) {
    QDictIterator < QString > it(*acosPerms_);
    for (; it.current(); ++it) {
      acos << it.currentKey();
      acos << *(*it);
    }
  }

  return acos;
}

