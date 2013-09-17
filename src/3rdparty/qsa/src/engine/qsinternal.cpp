/****************************************************************************
**
** Copyright (C) 1992-2006 Trolltech AS. All rights reserved.
**
** This file is part of the QSA of the Qt Toolkit.
**
** For QSA Commercial License Holders (non-open source):
** 
** Licensees holding a valid Qt Script for Applications (QSA) License Agreement
** may use this file in accordance with the rights, responsibilities and
** obligations contained therein.  Please consult your licensing agreement or
** contact sales@trolltech.com if any conditions of the Licensing Agreement are
** not clear to you.
** 
** Further information about QSA licensing is available at:
** http://www.trolltech.com/products/qsa/licensing.html or by contacting
** info@trolltech.com.
** 
** 
** For Open Source Edition:  
** 
** This file may be used under the terms of the GNU General Public License
** version 2 as published by the Free Software Foundation and appearing in the
** file LICENSE.GPL included in the packaging of this file.  Please review the
** following information to ensure GNU General Public Licensing requirements
** will be met:  http://www.trolltech.com/products/qt/opensource.html 
** 
** If you are unsure which license is appropriate for your use, please review
** the following information:
** http://www.trolltech.com/products/qsa/licensing.html or contact the 
** sales department at sales@trolltech.com.

**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

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

#include <qmutex.h>
#include <qregexp.h>

#include <stdio.h>

#include <AQScript/AQParserQSA.h>

extern int qsyyparse( QSLexer * );

QString qs_format_error( const QString &err )
{
    if ( err.startsWith( QLatin1String( "parse error," ) ) ) {
        QString copy = err.mid( 13 );
        copy.replace( QLatin1String( "ABSTRACT" ), QLatin1String( "abstract" ) );
        copy.replace( QLatin1String( "BREAK" ), QLatin1String( "break" ) );
        copy.replace( QLatin1String( "CASE" ), QLatin1String( "case" ) );
        copy.replace( QLatin1String( "CATCH" ), QLatin1String( "catch" ) );
        copy.replace( QLatin1String( "CLASS" ), QLatin1String( "class" ) );
        copy.replace( QLatin1String( "CONSTRUCTOR" ), QLatin1String( "constructor" ) );
        copy.replace( QLatin1String( "CONTINUE" ), QLatin1String( "continue" ) );
        copy.replace( QLatin1String( "DEFAULT" ), QLatin1String( "default" ) );
        copy.replace( QLatin1String( "DO" ), QLatin1String( "do" ) );
        copy.replace( QLatin1String( "ELSE" ), QLatin1String( "else" ) );
        copy.replace( QLatin1String( "EXTENDS" ), QLatin1String( "extends" ) );
        copy.replace( QLatin1String( "falseTOKEN" ), QLatin1String( "false" ) );
        copy.replace( QLatin1String( "FINAL" ), QLatin1String( "final" ) );
        copy.replace( QLatin1String( "FINALLY" ), QLatin1String( "finally" ) );
        copy.replace( QLatin1String( "FOR" ), QLatin1String( "'for" ) );
        copy.replace( QLatin1String( "FUNCTION" ), QLatin1String( "function" ) );
        copy.replace( QLatin1String( "IDENT" ), QLatin1String( "identifier" ) );
        copy.replace( QLatin1String( "IF" ), QLatin1String( "if" ) );
        copy.replace( QLatin1String( "IMPORT" ), QLatin1String( "import" ) );
        copy.replace( QLatin1String( "INSTANCEOF" ), QLatin1String( "instanceof" ) );
        copy.replace( QLatin1String( "IS" ), QLatin1String( "is" ) );
        copy.replace( QLatin1String( "NEW" ), QLatin1String( "new" ) );
        copy.replace( QLatin1String( "NULLTOKEN" ), QLatin1String( "null" ) );
        copy.replace( QLatin1String( "PACKAGE" ), QLatin1String( "package" ) );
        copy.replace( QLatin1String( "PRIVATE" ), QLatin1String( "private" ) );
        copy.replace( QLatin1String( "PUBLIC" ), QLatin1String( "public" ) );
        copy.replace( QLatin1String( "QS_CONST" ), QLatin1String( "const" ) );
        copy.replace( QLatin1String( "QS_DELETE" ), QLatin1String( "delete" ) );
        copy.replace( QLatin1String( "QS_IN" ), QLatin1String( "in" ) );
        copy.replace( QLatin1String( "QS_VOID" ), QLatin1String( "void" ) );
        copy.replace( QLatin1String( "RETURN" ), QLatin1String( "return" ) );
        copy.replace( QLatin1String( "STATIC" ), QLatin1String( "static" ) );
        copy.replace( QLatin1String( "SWITCH" ), QLatin1String( "switch" ) );
        copy.replace( QLatin1String( "THIS" ), QLatin1String( "this" ) );
        copy.replace( QLatin1String( "THROW" ), QLatin1String( "throw" ) );
        copy.replace( QLatin1String( "trueTOKEN" ), QLatin1String( "true" ) );
        copy.replace( QLatin1String( "TRY" ), QLatin1String( "try" ) );
        copy.replace( QLatin1String( "TYPEOF" ), QLatin1String( "typeof" ) );
        copy.replace( QLatin1String( "VAR" ), QLatin1String( "var" ) );
        copy.replace( QLatin1String( "WITH" ), QLatin1String( "with" ) );
        copy.replace( QLatin1String( "RESERVED" ), QLatin1String( "reserved keyword" ) );
        return copy;
    }
    return err;
}

QSObject QSArgumentsClass::construct( FunctionImp * /*func*/,
                                      const QSList * /*args*/ ) const
{
    QSObject obj( createWritable() );
#if 0
    obj.put( "callee", Function( func ), DontEnum );
    if ( args ) {
        obj.put( "length", createNumber( args->size() ), DontEnum );
        QSListIterator arg = args->begin();
        for ( int i = 0; arg != args->end(); ++arg, ++i )
            obj.put( QSString::from( i ), *arg, DontEnum );
    }
#endif
    return obj;
}

Q_GLOBAL_STATIC( QMutex, qsa_engineimp_mutex );

QSEngineImp::QSEngineImp( QSEngine *s )
: scr( s ),
initialized( false ),
en( 0 ),
glob( 0 )
#ifdef QSDEBUGGER
, dbg( 0L )
#endif
, lx( 0 )
{
    QMutexLocker locker( qsa_engineimp_mutex() );
    globalInit();
}

QSEngineImp::~QSEngineImp()
{
    QMutexLocker locker( qsa_engineimp_mutex() );

#ifdef QSDEBUGGER
    attachDebugger( 0L );
#endif
    clear();
    globalClear();
}

void QSEngineImp::globalInit()
{
    lx = new QSLexer();

}

void QSEngineImp::globalClear()
{
    delete lx;
}

void QSEngineImp::mark()
{
    glob->mark();
    retVal.mark();
}

void QSEngineImp::init()
{
    errType = 0;
    errLines.clear();
    errMsgs.clear();
    retVal.invalidate();
#ifdef QSDEBUGGER
    if ( dbg )
        dbg->clear();
#endif

    if ( en )
        en->setExecutionMode( QSEnv::Normal );

    if ( !initialized ) {
        en = new QSEnv( scr );
        glob = new Global( scr );
        glob->init();
        firstN = 0L;
        recursion = 0;
        initialized = true;
#ifdef QSDEBUGGER
        sid = -1;
#endif

    }

    if ( scr->timeoutTrigger() ) {
        scr->timeoutTrigger() ->start();
    }
}

void QSEngineImp::clear()
{
    if ( initialized ) {
        // To correctly free references.
        if ( retVal.isValid() )
            retVal.objectType() ->deref( &retVal );
        retVal.invalidate();
        en->clearScopeChain();
        delete glob; glob = 0;
        delete en; en = 0;
#ifdef QSDEBUGGER
        sid = -1;
#endif
        initialized = false;
    }
}

#define QS_MUTEX_LOCK if (mutex) mutex->lock()
#define QS_MUTEX_UNLOCK if (mutex) mutex->unlock()

bool QSEngineImp::evaluate( const QString &code, const QSObject *thisV,
                            bool onlyCheckSyntax, int checkMode, int lineZero )
{
    QMutex * mutex = qsa_engineimp_mutex();
    QS_MUTEX_LOCK;
    QSProgramNode *progNode = 0;

    init();

#ifdef QSDEBUGGER
    incrSourceId();
    if ( debugger() )
        debugger() ->setSourceId( sid );
#endif
    if ( recursion > 7 ) {
        qWarning( "QtScript: breaking out of recursion" );
        QS_MUTEX_UNLOCK;
        return true;
    }

    Q_ASSERT( lx );

    {
        int lxSid = 0;
#ifdef QSDEBUGGER
        lxSid = sourceId();
#endif
        lx->setCode( code, lxSid, lineZero );
        int parseError = qsyyparse( lx );
        progNode = lx->lastNode();

        if ( parseError || lx ->lexerState() == QSLexer::Bad ) {
            errType = QSErrParseError;
            errLines.append( lx ->lineNo() );
            errMsgs.append( QLatin1String( "Parse Error: " ) + qs_format_error( lx->errorMessage() ) );
            if ( progNode && progNode->deref() )
                delete progNode;
            QS_MUTEX_UNLOCK;
            return false;
        }
    }

    QSCheckData sem( env(), env() ->globalClass() );
    if ( thisV ) {
        Q_ASSERT( thisV->isValid() );
        sem.enterClass( const_cast<QSClass *>( thisV->objectType() ) );
        env() ->pushScope( *thisV );
    }
    sem.setGlobalStatementsForbidden( checkMode & QSEngine::DisallowGlobal );
    progNode->check( &sem );
    if ( sem.hasError() ) {
        errType = sem.errorCode();
        errLines = sem.errorLines();
        errMsgs = sem.errorMessages();
        if ( progNode->deref() )
            delete progNode;
        QS_MUTEX_UNLOCK;
        return false;
    }

    if ( onlyCheckSyntax ) {
        if ( progNode->deref() )
            delete progNode;
        QS_MUTEX_UNLOCK;
        return true;
    }

    env() ->clearException();

    ++recursion;

    Q_ASSERT( progNode );

    QS_MUTEX_UNLOCK;
    QSObject res = progNode->execute( env() );
    QS_MUTEX_LOCK;

    --recursion;

    if ( env() ->isExceptionMode( ) ) {
        QSObject err = env() ->exception();
        errType = 99;
        errLines.append( QSErrorClass::errorLine( &err ) );
        errMsgs.append( QSErrorClass::errorName( &err ) + QLatin1String( ". " ) +
                        QSErrorClass::errorMessage( &err ) );
#ifdef QSDEBUGGER
        if ( dbg )
            dbg->setSourceId( QSErrorClass::errorSourceId( &err ) );
#endif
        env() ->clearException();
    } else {
        errType = 0;
        errLines.clear();
        errMsgs.clear();

        // catch return value
        retVal = res;
    }

    if ( thisV )
        env() ->popScope();
    if ( progNode->deref() )
        delete progNode;

    QS_MUTEX_UNLOCK;
    return !errType;
}

bool QSEngineImp::evaluateByteCode( const QByteArray &code, const QSObject *thisV,
                                    bool, int checkMode, int lineZero )
{
    QMutex * mutex = qsa_engineimp_mutex();
    QS_MUTEX_LOCK;
    QSProgramNode *progNode = 0;

    init();

#ifdef QSDEBUGGER
    incrSourceId();
    if ( debugger() )
        debugger() ->setSourceId( sid );
#endif
    if ( recursion > 7 ) {
        qWarning( "QtScript: breaking out of recursion" );
        QS_MUTEX_UNLOCK;
        return true;
    }

    Q_ASSERT( lx );

    {
        int lxSid = 0;
#ifdef QSDEBUGGER
        lxSid = sourceId();
#endif
        lx->setCode( QString(), lxSid, lineZero );
        AQParserQSA parser( lx );
        int parseError = parser.parseByteCode( code );
        progNode = lx->lastNode();

        if ( parseError || lx ->lexerState() == QSLexer::Bad ) {
            errType = QSErrParseError;
            errLines.append( lx ->lineNo() );
            errMsgs.append( QLatin1String( "Parse Error: " ) + qs_format_error( lx->errorMessage() ) );
            if ( progNode && progNode->deref() )
                delete progNode;
            QS_MUTEX_UNLOCK;
            return false;
        }
    }

    QSCheckData sem( env(), env() ->globalClass() );
    if ( thisV ) {
        Q_ASSERT( thisV->isValid() );
        sem.enterClass( const_cast<QSClass *>( thisV->objectType() ) );
        env() ->pushScope( *thisV );
    }
    sem.setGlobalStatementsForbidden( checkMode & QSEngine::DisallowGlobal );
    progNode->check( &sem );
    if ( sem.hasError() ) {
        errType = sem.errorCode();
        errLines = sem.errorLines();
        errMsgs = sem.errorMessages();
        if ( progNode->deref() )
            delete progNode;
        QS_MUTEX_UNLOCK;
        return false;
    }

    env() ->clearException();

    ++recursion;

    Q_ASSERT( progNode );

    QS_MUTEX_UNLOCK;
    QSObject res = progNode->execute( env() );
    QS_MUTEX_LOCK;

    --recursion;

    if ( env() ->isExceptionMode( ) ) {
        QSObject err = env() ->exception();
        errType = 99;
        errLines.append( QSErrorClass::errorLine( &err ) );
        errMsgs.append( QSErrorClass::errorName( &err ) + QLatin1String( ". " ) +
                        QSErrorClass::errorMessage( &err ) );
#ifdef QSDEBUGGER
        if ( dbg )
            dbg->setSourceId( QSErrorClass::errorSourceId( &err ) );
#endif
        env() ->clearException();
    } else {
        errType = 0;
        errLines.clear();
        errMsgs.clear();

        // catch return value
        retVal = res;
    }

    if ( thisV )
        env() ->popScope();
    if ( progNode->deref() )
        delete progNode;

    QS_MUTEX_UNLOCK;
    return !errType;
}

bool QSEngineImp::evaluateByteCode( QDataStream & in, const QSObject *thisV,
                                    bool, int checkMode, int lineZero )
{
    QMutex * mutex = qsa_engineimp_mutex();
    QS_MUTEX_LOCK;
    QSProgramNode *progNode = 0;

    init();

#ifdef QSDEBUGGER
    incrSourceId();
    if ( debugger() )
        debugger() ->setSourceId( sid );
#endif
    if ( recursion > 7 ) {
        qWarning( "QtScript: breaking out of recursion" );
        QS_MUTEX_UNLOCK;
        return true;
    }

    Q_ASSERT( lx );

    {
        int lxSid = 0;
#ifdef QSDEBUGGER
        lxSid = sourceId();
#endif
        lx->setCode( QString(), lxSid, lineZero );
        AQParserQSA parser( lx );
        int parseError = parser.parseByteCode( in );
        progNode = lx->lastNode();

        if ( !progNode || parseError || lx ->lexerState() == QSLexer::Bad ) {
            errType = QSErrParseError;
            errLines.append( lx ->lineNo() );
            errMsgs.append( QLatin1String( "Parse Error: " ) + qs_format_error( lx->errorMessage() ) );
            if ( progNode && progNode->deref() )
                delete progNode;
            QS_MUTEX_UNLOCK;
            return false;
        }
    }

    QSCheckData sem( env(), env() ->globalClass() );
    if ( thisV ) {
        Q_ASSERT( thisV->isValid() );
        sem.enterClass( const_cast<QSClass *>( thisV->objectType() ) );
        env() ->pushScope( *thisV );
    }
    sem.setGlobalStatementsForbidden( checkMode & QSEngine::DisallowGlobal );
    progNode->check( &sem );
    if ( sem.hasError() ) {
        errType = sem.errorCode();
        errLines = sem.errorLines();
        errMsgs = sem.errorMessages();
        if ( progNode->deref() )
            delete progNode;
        QS_MUTEX_UNLOCK;
        return false;
    }

    env() ->clearException();

    ++recursion;

    Q_ASSERT( progNode );

    QS_MUTEX_UNLOCK;
    QSObject res = progNode->execute( env() );
    QS_MUTEX_LOCK;

    --recursion;

    if ( env() ->isExceptionMode( ) ) {
        QSObject err = env() ->exception();
        errType = 99;
        errLines.append( QSErrorClass::errorLine( &err ) );
        errMsgs.append( QSErrorClass::errorName( &err ) + QLatin1String( ". " ) +
                        QSErrorClass::errorMessage( &err ) );
#ifdef QSDEBUGGER
        if ( dbg )
            dbg->setSourceId( QSErrorClass::errorSourceId( &err ) );
#endif
        env() ->clearException();
    } else {
        errType = 0;
        errLines.clear();
        errMsgs.clear();

        // catch return value
        retVal = res;
    }

    if ( thisV )
        env() ->popScope();
    if ( progNode->deref() )
        delete progNode;

    QS_MUTEX_UNLOCK;
    return !errType;
}

bool QSEngineImp::call( QSObject *scope, const QString &func,
                        const QSList &args )
{
    init();
    QSObject t;
    if ( !scope || !scope->isValid() ) {
        t = env() ->globalObject();
        scope = &t;
    }

    uint idFunc = NEW_USTR( func ); REF_USTR( idFunc );
    QSObject v = scope->getQualified( idFunc );
    Q_ASSERT( v.isValid() );

    if ( !v.isDefined() ) {
        if ( func != QLatin1String( "main" ) ) {
            errType = ReferenceError;
            errMsgs.append( QLatin1String( "Unknown function: " ) + func );
            errLines.append( 0 );
        }
        DEREF_USTR( idFunc );
        return false;
    }

    if ( !v.isFunction() ) {
        errType = TypeError;
        errMsgs.append( func + QLatin1String( " is not a function. Call failed." ) );
        errLines.append( 0 );
        DEREF_USTR( idFunc );
        return false;
    }
    QSObject res = v.invoke( QSMember(), args );

    if ( env() ->isExceptionMode() ) {
        QSObject err = env() ->exception();
        errType = 99; /* TODO */
        errLines.append( QSErrorClass::errorLine( &err ) );
        errMsgs.append( QSErrorClass::errorName( &err ) + QLatin1String( ". " ) +
                        QSErrorClass::errorMessage( &err ) );
#ifdef QSDEBUGGER
        if ( dbg )
            dbg->setSourceId( QSErrorClass::errorSourceId( &err ) );
#endif
        env() ->clearException();
        DEREF_USTR( idFunc );
        return false;
    } else {
        errType = 0;
        errLines.clear();
        errMsgs.clear();

        // return value
        retVal = res;
        DEREF_USTR( idFunc );
        return true;
    }
}

#ifdef QSDEBUGGER
void QSEngineImp::attachDebugger( Debugger *d )
{
    Debugger * debugger = dbg;
    dbg = 0;
    if ( debugger ) {
        debugger->detach();
    }
    dbg = d;
}
bool QSEngineImp::setBreakpoint( int /*id*/, int /*line*/, bool /*set*/ )
{
    Q_ASSERT( !"Unsupported action..." );
    init();
    return false;
}
#endif

bool LabelStack::push( uint id )
{
    if ( !id || contains( id ) )
        return false;

    StackElm *newtos = new StackElm;
    newtos->id = id;
    newtos->prev = tos;
    tos = newtos;
    return true;
}

bool LabelStack::contains( uint id ) const
{
    if ( !id )
        return true;

    for ( StackElm * curr = tos; curr; curr = curr->prev )
        if ( curr->id == id )
            return true;

    return false;
}

bool LabelStack::contains( const QString & id ) const
{
    return contains( INDEX_USTR( id ) );
}

void LabelStack::pop()
{
    if ( tos ) {
        StackElm * prev = tos->prev;
        delete tos;
        tos = prev;
    }
}

LabelStack::~LabelStack()
{
    StackElm * prev;

    while ( tos ) {
        prev = tos->prev;
        delete tos;
        tos = prev;
    }
}
