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

#ifndef QSLOOKUP_H
#define QSLOOKUP_H

#include <qstring.h>

#ifdef __i386__
#  if defined(Q_CC_GNU)
#if !defined(Q_CC_INTEL) && ((100*(__GNUC__ - 0) + 10*(__GNUC_MINOR__ - 0) + __GNUC_PATCHLEVEL__) >= 332)
#    define AQ_FASTCALL __attribute__((regparm(3)))
#else
#    define AQ_FASTCALL
#endif
#  elif defined(Q_CC_MSVC)
#    define AQ_FASTCALL __fastcall
#  else
#     define AQ_FASTCALL
#  endif
#else
#  define AQ_FASTCALL
#endif

struct QSHashEntry
{
    const char *s;
    int value;
    int attr;
    const QSHashEntry *next;
};

struct QSHashTable
{
    int type;
    int size;
    const QSHashEntry *entries;
    int hashSize;
};

#include <QVarLengthArray>
#include <string.h>

/**
 * @short Fast keyword lookup.
 */
class QSLookup
{
public:

    static int find( const struct QSHashTable *table,
                            const QChar *c, uint len ) AQ_FASTCALL
    {
        QVarLengthArray<char> ascii( len + 1 );
        uint i;
        for ( i = 0; !c->row() && i < len; ++i, ++c )
            ascii[ i ] = c->cell();
        ascii[ i ] = '\0';

        int h = hash( ascii.data() ) % table->hashSize;
        const QSHashEntry *e = &table->entries[ h ];

        if ( !e )
            return -1;
        if ( !e->s )
            return -1;

        do
        {
            if ( strcmp( ascii.data(), e->s ) == 0 )
                return e->value;
            e = e->next;
        } while ( e );

        return -1;
    }

    static int find( const struct QSHashTable *table, const QString &s ) AQ_FASTCALL 
    {
        return find( table, s.unicode(), s.length() );
    }

    static uint hash( const QChar *c, uint len ) AQ_FASTCALL
    {
        uint val = 0;
        for ( uint i = 0; i < len; ++i, ++c )
            val += c->cell();
        return val;
    }

    static uint hash( const QString &key ) AQ_FASTCALL
    {
        return hash( key.unicode(), key.length() );
    }

    static uint hash( const char *s ) AQ_FASTCALL
    {
        uint val = 0;
        while ( *s )
            val += *s++;
        return val;
    }
};

#endif
