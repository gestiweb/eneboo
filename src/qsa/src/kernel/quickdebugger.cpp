/****************************************************************************
** $Id: quickdebugger.cpp  1.1.4   edited 2006-02-23T15:39:57$
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

#include "quickdebugger.h"

#include <qregexp.h>

QuickDebugger::QuickDebugger(QSEngine *e)
  : Debugger(e), hadError(false), scopeLvl(-1)
{
}

int QuickDebugger::freeSourceId()
{
  return Debugger::freeSourceId();
}

/*!
 * Returns true if \a var could be found in the current context, false
 * otherwise. On success \a value and \a type are set to the respective
 * values.
 */

bool QuickDebugger::watch(const QString &var, QString &type, QString &value) const
{
  QString simple = var;
  simple.replace(QRegExp(QString::fromLatin1("\\[")), QString::fromLatin1("."));
  simple.replace(QRegExp(QString::fromLatin1("\\]")), QString::fromLatin1(""));
  QString info = varInfo(simple);
  if (info.isNull()) {
    value = type = QString::null;
    return false;
  }
  int eq = info.find('=');
  int sep = info.findRev(':');
  value = info.mid(eq + 1, sep - eq - 1);
  type = info.mid(sep + 1);
  return true;
}

/*!
 * Set \a var to \val. Returns true on success, false if \var couldn't
 * be found in the current context, isn't writable or couldn't be
 * parse as s simple data type. Those types are null, undefined, a boolean,
 * a string or number.
 */

bool QuickDebugger::setVariable(const QString &var, const QString &val)
{
  QString simple = var;
  simple.replace(QRegExp(QString::fromLatin1("\\[")), QString::fromLatin1("."));
  simple.replace(QRegExp(QString::fromLatin1("\\]")), QString::fromLatin1(""));
  QString v = val.lower();
  QSObject value;
  bool ok;
  double d;
  if (v == QString::fromLatin1("null"))
    value = env()->createNull();
  else if (v == QString::fromLatin1("undefined"))
    value = env()->createUndefined();
  else if (v == QString::fromLatin1("true"))
    value = env()->createBoolean(true);
  else if (v == QString::fromLatin1("false"))
    value = env()->createBoolean(false);
  else if (d = val.toDouble(&ok), ok)
    value = env()->createNumber(d);
  else {
    v = val.stripWhiteSpace();
    QChar c0 = v[ 0 ];
    if ((c0 == '"' || c0 == '\'') && v[(int)v.length() - 1 ] == c0)
      value = env()->createString(v.mid(1, v.length() - 2));
    else
      return false;

  }

  return setVar(simple, value);
}

bool QuickDebugger::stopEvent()
{
  bool ret = true;
  emit stopped(ret);
  return ret;
}

void QuickDebugger::callEvent(const QString &fn, const QString &s)
{
  if (cStack.count() > 0 && cStack[0].line == -1) {
    QuickDebuggerStackFrame &sf = cStack.first();
    sf.line = lineNumber();
    sf.sourceId = sourceId();
    sf.scope = env()->scope();
  }
  if (fn == QString::fromLatin1("(internal)")) {
    tmpCStack.push(false);
    return;
  }
  tmpCStack.push(true);
  QString str(s);
  QuickDebuggerStackFrame sf(
    QString().sprintf("#%03d: %s(%s)", cStack.count() + 1, fn.latin1(), s.latin1())
  );
  cStack.prepend(sf);
}

void QuickDebugger::returnEvent()
{
  if (tmpCStack.pop()) {
    cStack.remove(cStack.begin());
    if (mode() != Disabled && cStack.isEmpty() && exceptionStack.isEmpty())
      setMode(Disabled);
  }
}

QValueList<QuickDebuggerStackFrame> QuickDebugger::backtrace()
{
  if (hadError)
    return exceptionStack;
  if (cStack.count() > 1) {
    QuickDebuggerStackFrame &sf = cStack.first();
    sf.line = lineNumber();
    sf.sourceId = sourceId();
    sf.scope = env()->scope();
  }
  return cStack;
}

void QuickDebugger::storeExceptionStack()
{
  if (hadError)
    return;

  exceptionStack = cStack;
  exceptionScope = env()->scope();
  hadError = true;

  if (exceptionStack.count() == 0) {
    QuickDebuggerStackFrame sf(QString::fromLatin1("No function"), lineNumber(), sourceId());
    exceptionStack.append(sf);
  } else {
    QuickDebuggerStackFrame &sf = exceptionStack.first();
    sf.line = lineNumber();
    sf.sourceId = sourceId();
    sf.scope = exceptionScope;
  }
}

QString QuickDebugger::varInfo(const QString &ident) const
{
  if (scopeLvl != -1 &&
      (
        (hadError && scopeLvl < exceptionStack.count()) ||
        (!hadError && scopeLvl < cStack.count())
      )
     )
    return varInfoScope(ident);

  if (!hadError || exceptionScope.isEmpty())
    return Debugger::varInfo(ident);

  QSEnv *e = env();
  e->pushScopeBlock();

  ScopeChain::const_iterator it = exceptionScope.end();
  do {
    --it;
    e->pushScope(*it);
  } while (it != exceptionScope.begin());


  QString ret = Debugger::varInfo(ident);
  e->popScopeBlock();
  return ret;
}

QString QuickDebugger::varInfoScope(const QString &ident) const
{
  const QuickDebuggerStackFrame &sf = (hadError
                                       ? exceptionStack[scopeLvl]
                                       : cStack[scopeLvl]);
  const ScopeChain *chain = &sf.scope;

  QSEnv *e = env();
  e->pushScopeBlock();

  ScopeChain::const_iterator it = chain->end();
  do {
    --it;
    e->pushScope(*it);
  } while (it != chain->begin());


  QString ret = Debugger::varInfo(ident);
  e->popScopeBlock();
  return ret;
}

void QuickDebugger::clear()
{
  hadError = false;
  exceptionScope.clear();
  exceptionStack.clear();
}

void QuickDebugger::setMode(Mode m)
{
  if (m == mode())
    return;
  Debugger::setMode(m);
  emit modeChanged(m);
}

#endif
