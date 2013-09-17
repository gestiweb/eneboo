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
#include "qsoperations.h"
#include "qstypes.h"
#include "qsfunction.h"
#include "qsenv.h"
#include "qslexer.h"

#include <QList>

QSArrayClass::QSArrayClass( QSClass *b )
        : QSWritableClass( b )
{
    addMember( REG_USTR( QLatin1String( "toString" ) ), QSMember( &QSArrayClass::join ) );
    addMember( REG_USTR( QLatin1String( "toLocaleString" ) ), QSMember( &QSArrayClass::join ) ); // ###
    addMember( REG_USTR( QLatin1String( "concat" ) ), QSMember( &concat ) );
    addMember( REG_USTR( QLatin1String( "join" ) ), QSMember( &QSArrayClass::join ) );
    addMember( REG_USTR( QLatin1String( "pop" ) ), QSMember( &pop ) );
    addMember( REG_USTR( QLatin1String( "push" ) ), QSMember( &push ) );
    addMember( REG_USTR( QLatin1String( "reverse" ) ), QSMember( &reverse ) );
    addMember( REG_USTR( QLatin1String( "shift" ) ), QSMember( &shift ) );
    addMember( REG_USTR( QLatin1String( "slice" ) ), QSMember( &slice ) );
    addMember( REG_USTR( QLatin1String( "sort" ) ), QSMember( &sort ) );
    addMember( REG_USTR( QLatin1String( "splice" ) ), QSMember( &splice ) );
    addMember( REG_USTR( QLatin1String( "unshift" ) ), QSMember( &unshift ) );
    int lengthIndex = addVariableMember( QSLexer::length_, AttributeNone );
    // setLength and length functions assume that lengthIndex is null
    Q_ASSERT( !lengthIndex );
    lengthIndex = 0;
}

/*!
  \reimp
*/
QSObject QSArrayClass::cast( const QSList &args ) const
{
    return construct( args );
}


QSObject QSArrayClass::construct( const QSList &args ) const
{
    QSArray result( env() );
    unsigned int len = 0;
    QSListIterator it = args.begin();
    // a single argument might denote the array size
    if ( args.size() == 1 && it->isNumber() ) {
        len = it->toUInt32();
        if ( it->toNumber() != double( len ) ) {
            throwError( RangeError, QLatin1String( "Invalid array length." ) );
            return createUndefined();
        }
        setLength( &result, len );
    } else {
        // initialize array
        len = args.size();
        for ( unsigned int u = 0; it != args.end(); ++it, ++u )
            result.put( NEW_USTRNUM( u ), *it );
    }

    // array size
    return result;
}

QSArray::QSArray( const QSEnv *env )
: QSObject( env->arrayClass() )
{
    setVal( new QSInstanceData( 1, env->createUndefined() ) );
    env->registerShared( shVal() );
    QSArrayClass::setLength( this, 0 );
}

void QSArrayClass::write( QSObject *objPtr, const QSMember &mem,
                          const QSObject &val ) const
{
    if ( mem.name() == QSLexer::length_ ) {
        uint oldLen = length( objPtr );
        uint newLen = val.toUInt32();
        if ( val.toNumber() != double( newLen ) ) {
            throwError( RangeError, QLatin1String( "Invalid array length." ) );
            return ;
        }
        // shrink array if necessary
        for ( uint u = newLen; u < oldLen; ++u ) {
            //QString pr = QString::number( u );
            if ( objPtr->hasProperty( HASH_USTRNUM( u ) ) )
                //  deleteProperty( pr ); // ####
                objPtr->put( NEW_USTRNUM( u ), createUndefined() );
        }
        setLength( objPtr, newLen );
    } else {
        QSWritableClass::write( objPtr, mem, val );
        bool ok;
        uint idx = USTR( mem.name() ).toUInt( &ok );
        if ( ok ) {
            uint len = length( objPtr );
            if ( idx >= len )
                setLength( objPtr, idx + 1 );
        }
    }
}

/*!
  \reimp
 
  Equivalent to join() with a "," seperator.
*/

QString QSArrayClass::toString( const QSObject *obj ) const
{
    return joinInternal( *obj, QLatin1String( "," ) );
}

QVariant QSArrayClass::toVariant( const QSObject *obj, QVariant::Type t ) const
{
    QSObject v = *obj;
    int len = length( obj );
    QSPropertyMap *propMap = properties( obj );

    if ( !propMap ) {
        switch ( t ) {
        case QVariant::StringList:
        case QVariant::Map:
        case QVariant::String:
        case QVariant::List:
            return QVariant( t, ( void * ) 0 );
        default:
            return QVariant();
        }
    }

    if ( t == QVariant::StringList ) {
        QStringList l;
        for ( int i = 0; i < len; ++i )
            l << v.get( HASH_USTRNUM( i ) ).toString();
        QVariant var = l;
        return var;
    } else if ( t == QVariant::Map ) {
        QVariantMap m;
        for ( QSPropertyMap::ConstIterator it = propMap->begin();
                it != propMap->end(); ++it )
            m.insert( USTR( it.key() ), ( *it ).object.toVariant( QVariant::Invalid ) );
        QVariant var = m;
        return var;
    } else if ( t == QVariant::String ) {
        QStringList l;
        for ( int i = 0; i < len; ++i )
            l << v.get( HASH_USTRNUM( i ) ).toString();
        return QVariant( l.join( QLatin1String( "," ) ) );
    } else {
        QVariantList l;
        QVariantMap m;
        bool conversion_ok = true;
        for ( QSPropertyMap::ConstIterator it = propMap->begin(); it != propMap->end(); ++it ) {
            if ( conversion_ok ) {
                int index = USTR( it.key() ).toInt( &conversion_ok );
                if ( conversion_ok ) {
                    // Pad to the right index
                    while ( l.size() <= index ) l << QVariant();
                    l[ index ] = ( *it ).object.toVariant( QVariant::Invalid );
                }
            }
            m[ USTR( it.key() ) ] = ( *it ).object.toVariant( QVariant::Invalid );
        }
        QVariant var;
        if ( conversion_ok )
            var = l;
        else
            var = m;

        return var;
    }
}

/*!
  Returns the length of array object \a a.
 */

uint QSArrayClass::length( const QSObject *a )
{
    Q_ASSERT( a && a->isA( a->env() ->arrayClass() ) );
    return ( uint ) ( ( QSInstanceData* ) a->shVal() ) ->value( 0 ) ->toNumber();
}

/*!
  Sets the length property of array object \a a to \a l.
 */

void QSArrayClass::setLength( QSObject *a, uint l )
{
    ( ( QSInstanceData* ) a->shVal() ) ->setValue( 0, a->env() ->createNumber( l ) );
}


#if QS_MAX_STACK>0
static int joinInternalRecursionDepth = 0;
#endif 
/*!
  Returns a string concatenating all members of this array seperated
  by \a sep.
*/

QString QSArrayClass::joinInternal( const QSObject &obj, const QString &sep )
{
#if QS_MAX_STACK>0
    if ( ++joinInternalRecursionDepth == QS_MAX_STACK ) {
        Q_ASSERT( obj.isValid() );
        obj.env() ->throwError( RangeError,
                                QLatin1String( "Internal recursion level maxed out in: "
                                               "QSArrayClass::joinInternal" ), -1 );
        --joinInternalRecursionDepth;
        return QString();
    }
#endif
    uint len = length( &obj );
    QString str;
    for ( uint i = 0; i < len; ++i ) {
        if ( i >= 1 )
            str += sep;
        QSObject o = obj.get( HASH_USTRNUM( i ) );
        if ( !o.isUndefined() && !o.isNull() )
            str += o.toString();
    }
#if QS_MAX_STACK>0
    joinInternalRecursionDepth--;
#endif
    return str;
}

QSObject QSArrayClass::toPrimitive( const QSObject *obj,
                                    const QSClass * ) const
{
    return createString( toString( obj ) );
}

QSObject QSArrayClass::concat( QSEnv *env )
{
    const QSList * args = env->arguments();
    QSArray result( env );
    int n = 0, len = 0;
    QSObject obj = env->thisValue();
    QSListIterator it = args->begin();
    for ( ;; ) {
        if ( obj.isA( "Array" ) ) {
            len = length( &obj );
            for ( int k = 0; k < len; ++k ) {
                uint p = HASH_USTRNUM( k );
                if ( obj.hasProperty( p ) ) {
                    result.put( NEW_USTRNUM( n ), obj.get( p ) );
                }
                ++n;
            }
        } else {
            result.put( NEW_USTRNUM( n ), obj );
            ++n;
        }
        if ( it == args->end() )
            break;
        obj = it++;
    }
    result.put( QSLexer::length_, env->createNumber( n ) );
    return result;
}

/*!
  Returns a string contain all elements of array \a o joined by
  the first element in \a args or ',' if unspecified.
 */

QSObject QSArrayClass::join( QSEnv *env )
{
    QString separator = env->arg( 0 ).isDefined() ?
                        env->arg( 0 ).toString() : QLatin1String( "," );
    return env->createString( NEW_USTR( joinInternal( env->thisValue(), separator ) ) );
}

/*!
  Pop the last element of array \a o and return it. Returns undefined
  in case the array is empty.
*/

QSObject QSArrayClass::pop( QSEnv *env )
{
    QSObject t = env->thisValue();
    uint len = length( &t );
    if ( len == 0 ) {
        return env->createUndefined();
    } else {
        QSObject obj = env->thisValue();
        uint str = HASH_USTRNUM( len - 1 );
        QSObject result = obj.get( str );
        obj.deleteProperty( str );
        setLength( &obj, len - 1 );
        return result;
    }
}

/*!
  Push all \a args to the end of array \a o.
 */

QSObject QSArrayClass::push( QSEnv *env )
{
    QSObject obj = env->thisValue();
    uint len = length( &obj );
    for ( int n = 0; n < env->numArgs(); ++n )
        obj.put( NEW_USTRNUM( len + n ), env->arg( n ) );
    len += env->numArgs();
    setLength( &obj, len );
    return env->createNumber( len );
}

/*!
  Reverse the order of elements in array \a o and return the array.
*/

QSObject QSArrayClass::reverse( QSEnv *env )
{
    QSObject o = env->thisValue();
    uint len = length( &o );
    uint middle = len / 2;
    QSObject thisObj = env->thisValue();
    for ( uint k = 0; k < middle; ++k ) {
        uint str = NEW_USTRNUM( k );
        uint str2 = NEW_USTRNUM( len - k - 1 );
        QSObject obj = thisObj.get( str );
        QSObject obj2 = thisObj.get( str2 );
        if ( thisObj.hasProperty( str2 ) ) {
            if ( thisObj.hasProperty( str ) ) {
                thisObj.put( str, obj2 );
                thisObj.put( str2, obj );
            } else {
                thisObj.put( str, obj2 );
                thisObj.deleteProperty( str2 );
            }
        } else {
            if ( thisObj.hasProperty( str ) ) {
                thisObj.deleteProperty( str );
                thisObj.put( str2, obj );
            } else {
                // why delete something that's not there ? Strange.
                thisObj.deleteProperty( str );
                thisObj.deleteProperty( str2 );
            }
        }
    }
    return thisObj;
}

/*!
  Removes the first element from array \a o and returns it.
*/

QSObject QSArrayClass::shift( QSEnv *env )
{
    QSObject obj = env->thisValue();
    uint len = length( &obj );
    if ( len == 0 )
        return env->createUndefined();

    QSObject result = obj.get( QSLexer::zero_ );
    for ( uint k = 1; k < len; ++k ) {
        uint str = NEW_USTRNUM( k );
        uint str2 = NEW_USTRNUM( k - 1 );
        if ( obj.hasProperty( str ) )
            obj.put( str2, obj.get( str ) );
        else
            obj.deleteProperty( str2 );
    }
    obj.deleteProperty( HASH_USTRNUM( len - 1 ) );
    setLength( &obj, len - 1 );
    return result;
}

/*!
  Returns the elements \a args[0] up to (but excluding) \a args[1]
  from array \a o.
  ### meaning of negative indices
 */

QSObject QSArrayClass::slice( QSEnv *env )
{
    QSObject obj = env->thisValue();
    QSArray result( env );
    uint len = length( &obj );
    int begin = env->arg( 0 ).toUInt32();
    int end = len;
    if ( !env->arg( 1 ).isUndefined() )
        end = env->arg( 1 ).toUInt32();
    if ( end < 0 )
        end += len;
    // safety tests
    if ( begin < 0 || end < 0 || begin >= end ) {
        result.put( QSLexer::length_, env->createNumber( 0 ) );
        return result;
    }
    for ( uint k = 0; k < ( uint ) end - begin; ++k ) {
        uint str = NEW_USTRNUM( k + begin );
        uint str2 = NEW_USTRNUM( k );
        if ( obj.hasProperty( str ) )
            result.put( str2, obj.get( str ) );
    }
    result.put( QSLexer::length_, end - begin );
    return result;
}

static void quickSort( QSObject *obj, int start, int end )
{
    if ( end - start < 1 )
        return ;

    int pIndex = start + ( end - start ) / 2;

    QSObject pivot = obj->get( NEW_USTRNUM( pIndex ) );
    obj->put( NEW_USTRNUM( pIndex ), obj->get( NEW_USTRNUM( end ) ) );
    QString pivotString = pivot.toString();

    int low = start, high = end - 1;
    QSObject lObj, hObj;

    while ( low < high ) {
        while ( ( lObj = obj->get( NEW_USTRNUM( low ) ) ).toString() < pivotString
                && low < high )
            ++low;

        while ( ( hObj = obj->get( NEW_USTRNUM( high ) ) ).toString() > pivotString
                && high > low )
            --high;

        if ( low < high ) {
            obj->put( NEW_USTRNUM( low ), hObj );
            obj->put( NEW_USTRNUM( high ), lObj );
            ++low;
            --high;
        }
    }

    if ( obj->get( NEW_USTRNUM( low ) ).toString() < pivotString )
        ++low;

    obj->put( NEW_USTRNUM( end ), obj->get( NEW_USTRNUM( low ) ) );
    obj->put( NEW_USTRNUM( low ), pivot );

    quickSort( obj, start, low );
    quickSort( obj, low + 1, end );
}


static inline int compare( const QSObject &a, const QSObject &b, QSObject *sort )
{
    QSList l;
    l.append( a );
    l.append( b );
    return int( sort->execute( l ).toNumber() );
}


static void quickSort( QSObject *obj, int start, int end, QSObject *sort )
{
    if ( end - start < 1 )
        return ;

    int pIndex = start + ( end - start ) / 2;
    QSObject pivot = obj->get( NEW_USTRNUM( pIndex ) );
    obj->put( NEW_USTRNUM( pIndex ), obj->get( NEW_USTRNUM( end ) ) );

    int low = start, high = end - 1;

    QSObject lObj, hObj;

    while ( low < high ) {

        while ( compare( lObj = obj->get( NEW_USTRNUM( low ) ), pivot, sort ) < 0
                && low < high )
            ++low;

        while ( compare( hObj = obj->get( NEW_USTRNUM( high ) ), pivot, sort ) > 0
                && high > low )
            --high;

        if ( low < high ) {
            obj->put( NEW_USTRNUM( low ), hObj );
            obj->put( NEW_USTRNUM( high ), lObj );
            ++low;
            --high;
        }
    }

    if ( compare( obj->get( NEW_USTRNUM( low ) ), pivot, sort ) < 0 )
        ++low;

    obj->put( NEW_USTRNUM( end ), obj->get( NEW_USTRNUM( low ) ) );
    obj->put( NEW_USTRNUM( low ), pivot );

    quickSort( obj, start, low, sort );
    quickSort( obj, low + 1, end, sort );
}


/*!
  Sort the array \a o. If an argument is given it is used as a
  compare function.
*/
QSObject QSArrayClass::sort( QSEnv *env )
{
    QSObject sortFunction;
    bool useSortFunction = env->arg( 0 ).isDefined();
    if ( useSortFunction ) {
        sortFunction = env->arg( 0 );
        if ( !sortFunction.isExecutable() )
            useSortFunction = false;
    }

    QSObject obj = env->thisValue();

    uint len = length( &obj );
    if ( len == 0 )
        return env->createUndefined();

    if ( useSortFunction ) {
        quickSort( &obj, 0, len - 1, &sortFunction );
    } else {
        quickSort( &obj, 0, len - 1 );
    }

    return obj;
}

/*! Array.splice(start, deleteCount [,item1 [,item2 [, ...]]])
  \a deleteCount elements of array \a o at index \a start are
  replaced by the following arguments.
 */

QSObject QSArrayClass::splice( QSEnv *env )
{
    QSArray result( env );
    QSObject obj = env->thisValue();

    int k;
    int len = length( &obj );
    int start = env->arg( 0 ).toInteger();
    int del = env->arg( 1 ).toInteger();
    start = ( start < 0 ) ? qMax( len + start, 0 ) : qMin( len, start );
    del = qMin( qMax( del, 0 ), len - start );
    for ( k = 0; k < del; ++k ) {
        uint index = NEW_USTRNUM( k + start );
        if ( obj.hasProperty( index ) )
            result.put( NEW_USTRNUM( k ), obj.get( index ) );
    }
    int add = env->numArgs() - 2;
    if ( add > del ) {
        for ( k = len - del; k > start; --k ) {
            uint s39 = NEW_USTRNUM( k + del - 1 );
            uint s40 = NEW_USTRNUM( k + add - 1 );
            if ( obj.hasProperty( s39 ) )          // 42
                obj.put( s40, obj.get( s39 ) );
            else  // 45
                obj.deleteProperty( s40 );
        }
    } else {
        for ( k = start; k < len - del; ++k ) {
            uint s22 = NEW_USTRNUM( k + del );
            uint s23 = NEW_USTRNUM( k + add );
            if ( obj.hasProperty( s22 ) )          // 25
                obj.put( s23, obj.get( s22 ) );
            else // 28
                obj.deleteProperty( s23 );
        } // 31
        for ( k = len; k > len - del + add; --k )
            obj.deleteProperty( NEW_USTRNUM( k - 1 ) );
    }
    for ( k = start; k < start + add; ++k )
        obj.put( NEW_USTRNUM( k ), env->arg( k - start + 2 ) );

    // Update length
    len = len - del + add;
    setLength( &obj, len < 0 ? 0 : len );

    return result;
}

/*!
  Prepend each element to the array \a o preserving order.
 */

QSObject QSArrayClass::unshift( QSEnv *env )
{
    QSObject obj = env->thisValue();
    uint len = length( &obj );
    int offset = env->numArgs();
    for ( uint k = len + offset - 1; ( int ) k >= offset; --k ) {
        uint str = NEW_USTRNUM( k );
        uint str2 = NEW_USTRNUM( k - offset );
        if ( obj.hasProperty( str2 ) )
            obj.put( str, obj.get( str2 ) );
        else
            obj.deleteProperty( str );
    }
    for ( int n = 0; n < offset; ++n )
        obj.put( NEW_USTRNUM( n ), env->arg( n ) );
    len += offset;
    setLength( &obj, len );
    return env->createNumber( len );
}
