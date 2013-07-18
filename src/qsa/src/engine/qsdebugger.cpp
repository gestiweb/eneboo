/****************************************************************************
** $Id: qsdebugger.cpp  1.1.5   edited 2006-02-23T15:39:57$
**
** Copyright (C) 2001-2006 Trolltech AS.  All rights reserved.
**
** This file is part of the Qt Script for Applications framework (QSA).
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding a valid Qt Script for Applications license may use
** this file in accordance with the Qt Script for Applications License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about QSA Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
*****************************************************************************/

#ifdef QSDEBUGGER

#include "qsdebugger.h"
#include "qsengine.h"
#include "qsenv.h"
#include "qsinternal.h"
#include "qsnodes.h"
#include <qstring.h>

Debugger::Debugger(QSEngine *engine)
  : eng(0),
    sid(-1)
{
  attach(engine);
}

Debugger::~Debugger()
{
  detach();
}

void Debugger::attach(QSEngine *e)
{
  dmode = Disabled;
  if (e) {
    if (!eng || e->rep != eng->rep) {
      eng = e;
      eng->rep->attachDebugger(this);
    }
  } else {
    eng = 0L;
  }
  reset();
}

void Debugger::detach()
{
  reset();
  if (!eng)
    return;
  eng->rep->attachDebugger(0L);
  eng = 0L;
}

int Debugger::freeSourceId() const
{
  return eng ? eng->rep->sourceId() + 1 : -1;
}

bool Debugger::setBreakpoint(int id, int line)
{
  if (!eng)
    return false;
  return eng->rep->setBreakpoint(id, line, true);
}

bool Debugger::deleteBreakpoint(int id, int line)
{
  if (!eng)
    return false;
  return eng->rep->setBreakpoint(id, line, false);
}

void Debugger::clearAllBreakpoints(int id)
{
  if (!eng)
    return;
  eng->rep->setBreakpoint(id, -1, false);
}

bool Debugger::validBreakpoint(const QString &code, int line)
{
  if (code.isEmpty() || line < 0)
    return false;

  QSEngine e;
  if (e.checkSyntax(code, QSEngine::CheckNormal, false)) {
#ifdef AQ_ENABLE_NODELIST
    return QSNode::setBreakpoint(QSNode::nodeFirstNode(),
                                 e.rep->sourceId(),
                                 line, true);
#else
    return QSNode::setBreakpoint(QSProgramNode::nodeProgLast(),
                                 e.rep->sourceId(),
                                 line, true);
#endif
  }
  return false;
}

QString Debugger::varInfo(const QString &ident) const
{
  if (!eng)
    return QString();

  int dot = ident.find('.');
  if (dot < 0)
    dot = ident.length();
  QString sub = ident.mid(0, dot);
  QSObject obj;
  // resolve base
  if (sub == QString::fromLatin1("||Global||")) {
    obj = env()->globalObject();
  } else if (sub == QString::fromLatin1("||Activation||")) {
    obj = env()->currentScope();
  } else if (sub == QString::fromLatin1("this")) {
    obj = env()->thisValue();
  } else {
    obj = env()->resolveValue(ident);
    if (!obj.isValid())
      return QString();
  }
  // look up each part of a.b.c.
  while (dot < (int)ident.length()) {
    int olddot = dot;
    dot = ident.find('.', olddot + 1);
    if (dot < 0)
      dot = ident.length();
    sub = ident.mid(olddot + 1, dot - olddot - 1);
    obj = obj.get(sub);
    if (!obj.isDefined())
      break;
  }

  return sub + QString::fromLatin1("=") + obj.debugString();
}

bool Debugger::setVar(const QString &ident, const QSObject &value)
{
  if (!eng || ident == QString::fromLatin1("this"))
    return false;
  int dot = ident.find('.');
  QString sub = dot > 0 ? ident.mid(0, dot) : ident;
  QSObject base;
  // resolve base
  if (sub == QString::fromLatin1("||Global||")) {
    base = env()->globalObject();
  } else if (sub == QString::fromLatin1("||Activation||")) {
    base = env()->currentScope();
  } else if (sub == QString::fromLatin1("this")) {
    base = env()->thisValue();
  } else {
    // return false;
    ScopeChain::const_iterator it = env()->scope().begin();
    for (it = env()->scope().begin(); it != env()->scope().end(); ++it) {
      if ((*it).hasProperty(sub)) {
        base = *it;
        break;
      }
    }
    if (it == env()->scope().end())
      return false;
#if 0 // ###
    const QSList *chain = Context::current()->pScopeChain();
    QSListIterator scope = chain->begin();
    while (scope != chain->end()) {
      if (scope->hasProperty(sub)) {
        base = *scope;
        break;
      }
      scope++;
    }
    if (scope == chain->end())
      return false;
#endif
  }
  // look up each part of a.b.c.
  while (dot > 0) {
    int olddot = dot;
    dot = ident.find('.', olddot + 1);
    if (dot < 0) {
      sub = ident.mid(olddot + 1);
      break;
    }
    sub = ident.mid(olddot + 1, dot - olddot - 1);
    if (!base.hasProperty(sub))
      return false;
    base = base.get(sub);
  }

  base.put(sub, value);

  return true;
}

// called from the scripting engine each time a statement node is hit.
bool Debugger::hit(int line, bool breakPoint)
{
  l = line;
  if (!eng)
    return true;

  if (!breakPoint && (mode() == Continue || mode() == Disabled))
    return true;

  bool ret = stopEvent();
  eng->init();  // in case somebody used a different interpreter meanwhile
  return ret;
}

#endif
