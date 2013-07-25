/****************************************************************************
** $Id: qsinternal.cpp  1.1.5   edited 2006-12-04T16:37:07$
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

#include "qsarray_object.h"
#include "qscheck.h"
#include "qsclass.h"
#include "qsdate_object.h"
#include "qsdebugger.h"
#include "qsengine.h"
#include "qsenv.h"
#include "qserror_object.h"
#include "qsfuncref.h"
#include "qsinternal.h"
#include "qslexer.h"
#include "qsmath_object.h"
#include "qsnodes.h"
#include "qsobject.h"
#include "qsobject_object.h"
#include "qsoperations.h"
#include "qsregexp_object.h"
#include "qstypes.h"

#include <qapplication.h>
#include <qregexp.h>
#if defined (QT_THREAD_SUPPORT)
#include <private/qmutexpool_p.h>
#endif
#include <stdio.h>

extern int qsyyparse();

QString qs_format_error(const QString &err)
{
  if (err.startsWith(QString::fromLatin1("parse error,"))) {
    QString copy = err.mid(13);
    copy.replace(QString::fromLatin1("ABSTRACT"), QString::fromLatin1("abstract"));
    copy.replace(QString::fromLatin1("BREAK"), QString::fromLatin1("break"));
    copy.replace(QString::fromLatin1("CASE"), QString::fromLatin1("case"));
    copy.replace(QString::fromLatin1("CATCH"), QString::fromLatin1("catch"));
    copy.replace(QString::fromLatin1("CLASS"), QString::fromLatin1("class"));
    copy.replace(QString::fromLatin1("CONSTRUCTOR"), QString::fromLatin1("constructor"));
    copy.replace(QString::fromLatin1("CONTINUE"), QString::fromLatin1("continue"));
    copy.replace(QString::fromLatin1("DEFAULT"), QString::fromLatin1("default"));
    copy.replace(QString::fromLatin1("DO"), QString::fromLatin1("do"));
    copy.replace(QString::fromLatin1("ELSE"), QString::fromLatin1("else"));
    copy.replace(QString::fromLatin1("EXTENDS"), QString::fromLatin1("extends"));
    copy.replace(QString::fromLatin1("falseTOKEN"), QString::fromLatin1("false"));
    copy.replace(QString::fromLatin1("FINAL"), QString::fromLatin1("final"));
    copy.replace(QString::fromLatin1("FINALLY"), QString::fromLatin1("finally"));
    copy.replace(QString::fromLatin1("FOR"), QString::fromLatin1("'for"));
    copy.replace(QString::fromLatin1("FUNCTION"), QString::fromLatin1("function"));
    copy.replace(QString::fromLatin1("IDENT"), QString::fromLatin1("identifier"));
    copy.replace(QString::fromLatin1("IF"), QString::fromLatin1("if"));
    copy.replace(QString::fromLatin1("IMPORT"), QString::fromLatin1("import"));
    copy.replace(QString::fromLatin1("INSTANCEOF"), QString::fromLatin1("instanceof"));
    copy.replace(QString::fromLatin1("IS"), QString::fromLatin1("is"));
    copy.replace(QString::fromLatin1("NEW"), QString::fromLatin1("new"));
    copy.replace(QString::fromLatin1("NULLTOKEN"), QString::fromLatin1("null"));
    copy.replace(QString::fromLatin1("PACKAGE"), QString::fromLatin1("package"));
    copy.replace(QString::fromLatin1("PRIVATE"), QString::fromLatin1("private"));
    copy.replace(QString::fromLatin1("PUBLIC"), QString::fromLatin1("public"));
    copy.replace(QString::fromLatin1("QS_CONST"), QString::fromLatin1("const"));
    copy.replace(QString::fromLatin1("QS_DELETE"), QString::fromLatin1("delete"));
    copy.replace(QString::fromLatin1("QS_IN"), QString::fromLatin1("in"));
    copy.replace(QString::fromLatin1("QS_VOID"), QString::fromLatin1("void"));
    copy.replace(QString::fromLatin1("RETURN"), QString::fromLatin1("return"));
    copy.replace(QString::fromLatin1("STATIC"), QString::fromLatin1("static"));
    copy.replace(QString::fromLatin1("SWITCH"), QString::fromLatin1("switch"));
    copy.replace(QString::fromLatin1("THIS"), QString::fromLatin1("this"));
    copy.replace(QString::fromLatin1("THROW"), QString::fromLatin1("throw"));
    copy.replace(QString::fromLatin1("TRUETOKEN"), QString::fromLatin1("true"));
    copy.replace(QString::fromLatin1("TRY"), QString::fromLatin1("try"));
    copy.replace(QString::fromLatin1("TYPEOF"), QString::fromLatin1("typeof"));
    copy.replace(QString::fromLatin1("VAR"), QString::fromLatin1("var"));
    copy.replace(QString::fromLatin1("WITH"), QString::fromLatin1("with"));
    copy.replace(QString::fromLatin1("RESERVED"), QString::fromLatin1("reserved keyword"));
    return copy;
  }
  return err;
}

QSObject QSArgumentsClass::construct(FunctionImp *func, const QSList *args) const
{
  Q_UNUSED(func);
  Q_UNUSED(args);

#if 0
  QSObject obj(createWritable());
  obj.put("callee", Function(func), DontEnum);
  if (args) {
    obj.put("length", createNumber(args->size()), DontEnum);
    QSListIterator arg = args->begin();
    for (int i = 0; arg != args->end(); arg++, i++)
      obj.put(QSString::from(i), *arg, DontEnum);
  }
  return obj;
#else
  return createWritable();
#endif
}

int QSEngineImp::instances = 0;

class ExecutionStack
{
public:
  ExecutionStack()
    : progNode(0L),
      firstNode(0L),
      prev(0L) {}
  ExecutionStack *push() {
    ExecutionStack *s = new ExecutionStack();
    s->prev = this;
    return s;
  }
  ExecutionStack *pop() {
    ExecutionStack *s = prev;
    if (this)
      delete this;
    return s;
  }
  void deallocateNodes() {
#if QSNODES_ALLOC_DEBUG == 2
    printf("*********************\n");
#ifdef AQ_ENABLE_NODELIST
    //    QSNode *it = QSNode::nodeFirstNode();
    //    while (it) {
    //      printf("%p -- ", it->prev);
    //      it = it->next;
    //    }
    //    printf(" %p\n", it);
    printf("1 - %p %p %p\n", progNode, firstNode, QSNode::nodeFirstNode());
#endif
    if (firstNode)
      printf("1 - firstNode::refs => %d\n", firstNode->refs());
    printf("1 - QSNode::qsNodeCount => %d\n", QSNode::qsNodeCount);
#endif
    if (progNode == firstNode)
      firstNode = 0L;
    if (progNode && progNode->deref()) {
      delete progNode;
      progNode = 0L;
    }
    if (firstNode && firstNode->deref()) {
      delete firstNode;
      firstNode = 0L;
    }
#ifdef AQ_ENABLE_NODELIST
    firstNode = QSNode::nodeFirstNode();
#else
    firstNode = QSProgramNode::nodeProgLast();
#endif
#if QSNODES_ALLOC_DEBUG == 2
#ifdef AQ_ENABLE_NODELIST
    printf("2 - %p %p %p\n", progNode, firstNode, QSNode::nodeFirstNode());
#endif
    if (firstNode)
      printf("2 - firstNode::refs => %d\n", firstNode->refs());
#ifdef AQ_ENABLE_NODELIST
    //    it = QSNode::nodeFirstNode();
    //    while (it) {
    //      printf("%p -- ", it->prev);
    //      it = it->next;
    //    }
    //    printf(" %p\n", it);
#endif
    printf("2 - QSNode::qsNodeCount => %d\n", QSNode::qsNodeCount);
    printf("#####################\n");
#endif
  }
  QSProgramNode *progNode;
  QSNode *firstNode;
  ExecutionStack *prev;
};

inline QSProgramNode *QSEngineImp::progNode() const
{
  return stack->progNode;
}

inline void QSEngineImp::setProgNode(QSProgramNode *p)
{
  stack->progNode = p;
}

inline QSNode *QSEngineImp::firstNode() const
{
  return stack->firstNode;
}

inline void QSEngineImp::setFirstNode(QSNode *n)
{
  stack->firstNode = n;
}

QSEngineImp::QSEngineImp(QSEngine *s)
  : scr(s),
    initialized(false),
    en(0),
    glob(0)
#ifdef QSDEBUGGER
    , dbg(0L)
#endif
{
#if defined (QT_THREAD_SUPPORT) && QT_VERSION >= 0x030300
  QMutexLocker locker(qt_global_mutexpool ? qt_global_mutexpool->get(qApp) : 0);
#endif
  ++instances;
  // are we the first interpreter instance ? Initialize some stuff
  if (instances == 1)
    globalInit();
  stack = new ExecutionStack();
}

QSEngineImp::~QSEngineImp()
{
#if defined (QT_THREAD_SUPPORT) && QT_VERSION >= 0x030300
  QMutexLocker locker(qt_global_mutexpool ? qt_global_mutexpool->get(qApp) : 0);
#endif

#ifdef QSDEBUGGER
  attachDebugger(0L);
#endif

  clear();
  delete stack;
  stack = 0L;

  // are we the last of our kind ? Free global stuff.
  if (instances == 1)
    globalClear();
  --instances;

#ifdef QSNODES_ALLOC_DEBUG
  printf("QSEngineImp::instances => %d\n", QSEngineImp::instances);
#endif
}

void QSEngineImp::globalInit()
{
  (void)new QSLexer();
}

void QSEngineImp::globalClear()
{
  delete QSLexer::lexer();
}

void QSEngineImp::init()
{
  errType = 0;
  errLines.clear();
  errMsgs.clear();
  retVal.invalidate();
#ifdef QSDEBUGGER
  if (dbg)
    dbg->clear();
#endif

  if (en)
    en->setExecutionMode(QSEnv::Normal);

  if (!initialized) {
    en = new QSEnv(scr);
    glob = new Global(scr);
    glob->init();
    recursion = 0;
    initialized = true;
#ifdef QSDEBUGGER
    sid = -1;
#endif
  }

  if (scr->timeoutTrigger())
    scr->timeoutTrigger()->start();
}

void QSEngineImp::clear()
{
  if (initialized) {
    // To correctly free references.
    if (retVal.isValid())
      retVal.objectType()->deref(&retVal);
    retVal.invalidate();

    en->clearScopeChain();
    delete glob;
    glob = 0;
    delete en;
    en = 0;

    setProgNode(QSProgramNode::nodeProgLast());
#ifdef AQ_ENABLE_NODELIST
    setFirstNode(QSNode::nodeFirstNode());
#else
    setFirstNode(QSProgramNode::nodeProgLast());
#endif
    stack->deallocateNodes();

#ifdef QSNODES_ALLOC_DEBUG
    printf("QSNode::qsNodeCount => %d (Before garbage collection)\n", QSNode::qsNodeCount);
#endif

#ifndef AQ_NO_GARBAGE_COLLECTION
#ifdef AQ_ENABLE_NODELIST
    QSNode *n =  QSNode::nodeFirstNode();
    if (n) {
      QPtrList<QSNode> garbage;
      const QSNode *tmp;
      while ((tmp = n)) {
        n = n->next;
        garbage.append(tmp);
      }
      for (n = garbage.first(); n; n = garbage.next()) {
        if (n->refs()) {
          n->deref();
          break;
        }
      }
      garbage.clear();
      n =  QSNode::nodeFirstNode();
      while ((tmp = n)) {
        n = n->next;
        garbage.append(tmp);
      }
      for (n = garbage.first(); n; n = garbage.next()) {
        if (!n->refs())
          delete n;
      }
    }
#endif
#ifdef QSNODES_ALLOC_DEBUG
    else
      printf("Can not do garbage collection, first node is null\n");
#endif
#else
#ifdef QSNODES_ALLOC_DEBUG
    printf("Garbage Collection Disabled\n");
#endif
#endif

#ifdef QSNODES_ALLOC_DEBUG
    printf("QSNode::qsNodeCount => %d (After garbage collection)\n", QSNode::qsNodeCount);
#endif

#ifdef QSDEBUGGER
    sid = -1;
#endif
    initialized = false;
  }
}

#if defined (QT_THREAD_SUPPORT) && QT_VERSION >= 0x030300
#define QS_MUTEX_LOCK if (mutex) mutex->lock()
#define QS_MUTEX_UNLOCK if (mutex) mutex->unlock()
#else
#define QS_MUTEX_LOCK
#define QS_MUTEX_UNLOCK
#endif

bool QSEngineImp::evaluate(const QString &code, const QSObject *thisV,
                           bool onlyCheckSyntax, int checkMode, int lineZero)
{
#if defined (QT_THREAD_SUPPORT) && QT_VERSION >= 0x030300
  QMutex *mutex = qt_global_mutexpool ? qt_global_mutexpool->get(qApp) : 0;
#endif
  QS_MUTEX_LOCK;

  init();

#ifdef QSDEBUGGER
  incrSourceId();
  if (debugger())
    debugger()->setSourceId(sid);
#endif

  if (recursion > 7) {
#ifdef QSNODES_ALLOC_DEBUG
    printf("QSEngineImp::recursion => %d : Breaking out of recursion\n",
           "recursion");
#endif
    QS_MUTEX_UNLOCK;
    return true;
  } else if (recursion > 0) {
#ifdef QSNODES_ALLOC_DEBUG
    printf("QSEngineImp::recursion => % d\n", recursion);
#endif
    stack = stack->push();
  }

  Q_ASSERT(QSLexer::lexer());
  {
    QSLexer::lexer()->setCode(code,
#ifdef QSDEBUGGER
                              sourceId(),
#else
                              0,
#endif
                              lineZero);
#ifdef AQ_ENABLE_NODELIST
    QSNode::setNodeFirstNode(firstNode());
    int parseError = qsyyparse();
    setFirstNode(QSNode::nodeFirstNode());
    setProgNode(QSProgramNode::nodeProgLast());
#else
    int parseError = qsyyparse();
    setFirstNode(QSProgramNode::nodeProgLast());
    setProgNode(QSProgramNode::nodeProgLast());
#endif

    if (parseError || QSLexer::lexer()->lexerState() == QSLexer::Bad) {
      errType = QSErrParseError;
      int l = QSLexer::lexer()->lineNo();
      errLines.append(l);
      errMsgs.append(QString::fromLatin1("Parse Error: ")
                     + qs_format_error(QSLexer::lexer()->errorMessage()));
      stack->deallocateNodes();
      QS_MUTEX_UNLOCK;
      return false;
    }
  }

  Q_ASSERT(progNode());

  QSCheckData sem(env(), env()->globalClass());
  if (thisV) {
    Q_ASSERT(thisV->isValid());
    sem.enterClass((QSClass *)thisV->objectType());
    env()->pushScope(*thisV);
  }

  sem.setGlobalStatementsForbidden(checkMode & QSEngine::DisallowGlobal);
  progNode()->check(&sem);
  if (sem.hasError()) {
    errType = sem.errorCode();
    errLines = sem.errorLines();
    errMsgs = sem.errorMessages();
    stack->deallocateNodes();
    QS_MUTEX_UNLOCK;
    return false;
  }

  if (onlyCheckSyntax) {
    stack->deallocateNodes();
    QS_MUTEX_UNLOCK;
    return true;
  }

  env()->clearException();

  ++recursion;

  QS_MUTEX_UNLOCK;
  QSObject res = progNode()->execute(env());
  QS_MUTEX_LOCK;

  --recursion;

  if (env()->isExceptionMode()) {
    QSObject err = env()->exception();
    errType = 99; /* TODO */
    errLines.append(QSErrorClass::errorLine(&err));
    errMsgs.append(QSErrorClass::errorName(&err) +
                   QString::fromLatin1(". ") +
                   QSErrorClass::errorMessage(&err));
#ifdef QSDEBUGGER
    if (dbg)
      dbg->setSourceId(QSErrorClass::errorSourceId(&err));
#endif
    env()->clearException();
  } else {
    errType = 0;
    errLines.clear();
    errMsgs.clear();
    // catch return value
    retVal = res;
  }

  if (thisV)
    env()->popScope();

  stack->deallocateNodes();
  if (recursion > 0) {
    stack = stack->pop();
    Q_ASSERT(stack);
  }

  QS_MUTEX_UNLOCK;
  return !errType;
}

bool QSEngineImp::call(QSObject *scope, const QString &func,
                       const QSList &args)
{
  init();
  QSObject t;
  if (!scope || !scope->isValid()) {
    t = env()->globalObject();
    scope = &t;
  }

  QSObject v = scope->getQualified(func);
  Q_ASSERT(v.isValid());

  if (!v.isDefined()) {
    if (func != QString::fromLatin1("main")) {
      errType = ReferenceError;
      errMsgs.append(QString::fromLatin1("Unknown function: ") + func);
      errLines.append(0);
    }
    return false;
  }

  if (!v.isFunction()) {
    errType = TypeError;
    errMsgs.append(func + QString::fromLatin1(" is not a function. Call failed."));
    errLines.append(0);
    return false;
  }
  QSObject res = v.invoke(QSMember(), args);

  if (env()->isExceptionMode()) {
    QSObject err = env()->exception();
    errType = 99; /* TODO */
    errLines.append(QSErrorClass::errorLine(&err));
    errMsgs.append(QSErrorClass::errorName(&err) +
                   QString::fromLatin1(". ") +
                   QSErrorClass::errorMessage(&err));
#ifdef QSDEBUGGER
    if (dbg)
      dbg->setSourceId(QSErrorClass::errorSourceId(&err));
#endif
    env()->clearException();
    return false;
  } else {
    errType = 0;
    errLines.clear();
    errMsgs.clear();
    retVal = res;
    return true;
  }
}

#ifdef QSDEBUGGER
void QSEngineImp::attachDebugger(Debugger *d)
{
  Debugger *debugger = dbg;
  dbg = 0;
  if (debugger) {
    debugger->detach();
  }
  dbg = d;
}

bool QSEngineImp::setBreakpoint(int id, int line, bool set)
{
  init();
  return QSNode::setBreakpoint(firstNode(), id, line, set);
}
#endif
