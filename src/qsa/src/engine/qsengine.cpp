/****************************************************************************
** $Id: qsengine.cpp  1.1.5   edited 2006-02-23T15:39:57$
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

#include "qsengine.h"
#include "qsenv.h"
#include "qserrors.h"
#include "qsfuncref.h"
#include "qsinternal.h"
#include "qslexer.h"
#include "qsnodes.h"
#include "qstypes.h"

#if defined (QT_THREAD_SUPPORT)
#include <private/qmutexpool_p.h>
#endif

QSEngine::QSEngine(QObject *parent, const char *name)
  : QObject(parent, name), timer(0)
{
  rep = new QSEngineImp(this);
  rep->init();
}

QSEngine::~QSEngine()
{
  delete rep;
  delete timer;

#if QS_DEBUG_MEM == 1
  printf("QSList::count: %d\n", QSList::count);
  printf("QSObject::count: %d\n", QSObject::count);
#endif
}

void QSEngine::init()
{
  rep->init();
}

int QSEngine::recursion() const
{
  return rep->recursion;
}

bool QSEngine::evaluate(const char *code)
{
  return rep->evaluate(QString::fromLatin1(code));
}

bool QSEngine::evaluate(const QString &code)
{
  return rep->evaluate(code);
}

bool QSEngine::evaluate(const QSObject &thisV, const QString &code, int linezero)
{
  return rep->evaluate(code, thisV.isValid() ? &thisV : 0, linezero);
}

bool QSEngine::call(const QString &func, const QSList &args)
{
  return rep->call(0, func, args);
}

bool QSEngine::call(QSObject *scope, const QString &func,
                    const QSList &args)
{
  return rep->call(scope, func, args);
}

void QSEngine::clear()
{
  rep->clear();
}

QSObject QSEngine::returnValue() const
{
  return rep->retVal;
}

void QSEngine::setReturnValue(const QSObject &r)
{
  rep->retVal = r;
}

int QSEngine::errorType() const
{
  return rep->errType;
}

QValueList<uint> QSEngine::errorLines() const
{
  return rep->errLines;
}

QStringList QSEngine::errorMessages() const
{
  return rep->errMsgs;
}

#if 0
extern int qsyyparse();
extern QString qs_format_error(const QString &str);
#endif

bool QSEngine::checkSyntax(const QString &code, int checkMode,
                           bool deleteNodes)
{
  return rep->evaluate(code, 0, true, checkMode);

#if 0
  Q_UNUSED(checkMode);
  Q_ASSERT(QSLexer::lexer());

#if defined (QT_THREAD_SUPPORT) && QT_VERSION >= 0x030300
  QMutexLocker locker(qt_global_mutexpool ? qt_global_mutexpool->get(QSLexer::lexer()) : 0);
#endif

  rep->errType = 0;
  rep->errLines.clear();
  rep->errMsgs.clear();

  QSLexer::lexer()->setCode(code,
#ifdef QSDEBUGGER
                            rep->sourceId()
#else
                            0
#endif
                           );

  int parseError = qsyyparse();
  QSNode *lastNode = QSProgramNode::nodeProgLast();
  bool success = true;

  if (parseError) {
    rep->errType = QSErrParseError;
    int l = QSLexer::lexer()->lineNo();
    rep->errLines.append(l);
    rep->errMsgs.append(QString::fromLatin1("Parse Error: ")
                        + qs_format_error(QSLexer::lexer()->errorMessage()));
    success = false;
  }

  if (deleteNodes && lastNode && lastNode->deref())
    delete lastNode;

  return success;
#endif
}

static QSObject debugFunction(QSEnv *env)
{
#ifndef AQ_NO_DEBUG_FUN
  qDebug("---> %s", env->arg(0).toString().local8Bit().data());
#endif
  return env->createUndefined();
}

static QSObject printFunction(QSEnv *env)
{
#ifndef AQ_NO_PRINT_FUN
  qDebug("%s", env->arg(0).toString().local8Bit().data());
#endif
  return env->createUndefined();
}

void QSEngine::enableDebug()
{
  rep->init();

  QSMember debug(&debugFunction);
  env()->globalClass()->addMember(QString::fromLatin1("debug"), debug);

  QSMember print(&printFunction);
  env()->globalClass()->addMember(QString::fromLatin1("print"), print);
}

/*!
  \internal
*/

void QSEngine::warn(const QString &msg, int l)
{
  emit warning(msg, l);
}

/*!
  \internal
*/

void QSEngine::requestPackage(const QString &package, QString &err)
{
  emit packageRequest(package, err);
}

QSEnv *QSEngine::env() const
{
  return rep->env();
}

#ifdef QSDEBUGGER
Debugger *QSEngine::debugger() const
{
  return rep->debugger();
}
#endif

/*!
  Register the type described by \a c with the engine. Ownership of the
  type object is passed on. The interpreter will delete all registered
  types on destruction or a call to clear().

  \sa type()
*/

void QSEngine::registerType(QSClass *c)
{
  QSClass *glob = env()->globalClass();
  glob->addStaticVariableMember(c->identifier(),
                                env()->typeClass()->createType(c),
                                AttributeExecutable);
}

