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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "qsnodes.h"
#include "grammar.h"
#include "qslexer.h"
#include "qsengine.h"
#include "qslookup.h"
#include "qsinternal.h"
#include "qslexer.lut.h"

#include <qlocale.h>

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define shiftWindowsLineBreak() if( current == '\r' && next1 == '\n' ) shift( 1 );

QHash<uint, QString> QSLexer::ustrHash;
QHash<uint, qint16> QSLexer::ustrRefs;
QHash<QString, uint> QSLexer::ustrRegs;
QVarLengthArray<uint, MAX_USTR_NUM> QSLexer::ustrNumbers;

int QSLexer::count_ = 0;
int QSLexer::instances_ = 0;

uint QSLexer::undefined_ = 0;
uint QSLexer::null_ = 0;
uint QSLexer::true_ = 0;
uint QSLexer::false_ = 0;
uint QSLexer::function_ = 0;
uint QSLexer::noop_ = 0;
uint QSLexer::anony_ = 0;
uint QSLexer::args_ = 0;
uint QSLexer::object_ = 0;
uint QSLexer::boolean_ = 0;
uint QSLexer::number_ = 0;
uint QSLexer::string_ = 0;
uint QSLexer::length_ = 0;
uint QSLexer::zero_ = 0;
uint QSLexer::application_ = 0;
uint QSLexer::global_ = 0;
uint QSLexer::lastindex_ = 0;

QSLexer::QSLexer()
    : pattern( 0 ), flags( 0 ), yylineno( 0 ), runlineno( 0 ),
    size8( INIT_SIZE_BUFFER8 ), size16( INIT_SIZE_BUFFER16 ), restrKeyword( false ),
    stackToken( -1 ), pos( 0 ),
    code( 0 ), length( 0 ),
    bol( true ), current( 0 ), next1( 0 ),
    next2( 0 ), next3( 0 ), prog( 0 )
{
  buffer8.resize( INIT_SIZE_BUFFER8 );
  buffer16.resize( INIT_SIZE_BUFFER16 );

  if ( !instances_ ) {
    ustrNumbers.resize( MAX_USTR_NUM );
    for ( int i = 0; i < MAX_USTR_NUM; ++i ) {
      ustrNumbers[ i ] = NEW_USTR( QString::number( i ) );
      REF_USTR( ustrNumbers[ i ] );
    }

    undefined_ = NEW_USTR( QLatin1String( "undefined" ) );
    REF_USTR( undefined_ );
    null_ = NEW_USTR( QLatin1String( "null" ) );
    REF_USTR( null_ );
    true_ = NEW_USTR( QLatin1String( "true" ) );
    REF_USTR( true_ );
    false_ = NEW_USTR( QLatin1String( "false" ) );
    REF_USTR( false_ );
    function_ = NEW_USTR( QLatin1String( "function" ) );
    REF_USTR( function_ );
    noop_ = NEW_USTR( QLatin1String( "###aqnoop" ) );
    REF_USTR( noop_ );
    anony_ = NEW_USTR( QLatin1String( "[anonymous]" ) );
    REF_USTR( anony_ );
    args_ = NEW_USTR( QLatin1String( "arguments" ) );
    REF_USTR( args_ );
    object_ = NEW_USTR( QLatin1String( "object" ) );
    REF_USTR( object_ );
    boolean_ = NEW_USTR( QLatin1String( "boolean" ) );
    REF_USTR( boolean_ );
    number_ = NEW_USTR( QLatin1String( "number" ) );
    REF_USTR( number_ );
    string_ = NEW_USTR( QLatin1String( "string" ) );
    REF_USTR( string_ );
    length_ = NEW_USTR( QLatin1String( "length" ) );
    REF_USTR( length_ );
    zero_ = NEW_USTR( QLatin1String( "0" ) );
    REF_USTR( zero_ );
    application_ = NEW_USTR( QLatin1String( "Application" ) );
    REF_USTR( application_ );
    global_ = NEW_USTR( QLatin1String( "global" ) );
    REF_USTR( global_ );
    lastindex_ = NEW_USTR( QLatin1String( "lastIndex" ) );
    REF_USTR( lastindex_ );
  }

  ++instances_;
}

QSLexer::~QSLexer()
{
  --instances_;
#ifdef QSNODES_ALLOC_DEBUG

  if ( !instances_ ) {
    for ( QHash<uint, QString>::const_iterator it = ustrHash.begin(); it != ustrHash.end(); ++it )
      qWarning() << it.key() << it.value();
    qWarning() << ustrHash.size() << ustrHash.capacity();
  }
#endif
}

/*!
  Sets the code to be parsed.
  \a lineno specifies which line which is to be interpreted
  as line number zero. \a id specifies the unique identification
  number for this source code unit.
*/
void QSLexer::setCode( const QString &c, int id, int lineno )
{
  errmsg = QString();
  yylineno = -lineno;
  sid = id;
  restrKeyword = false;
  delimited = false;
  stackToken = -1;
  pos = 0;
  code = c.unicode();
  length = c.length();
#ifndef QTSCRIPT_PURE_ECMA

  bol = true;
#endif

  // read first characters
  current = ( length > 0 ) ? code[ 0 ].unicode() : 0;
  next1 = ( length > 1 ) ? code[ 1 ].unicode() : 0;
  next2 = ( length > 2 ) ? code[ 2 ].unicode() : 0;
  next3 = ( length > 3 ) ? code[ 3 ].unicode() : 0;
}

inline void QSLexer::shift( uint p )
{
  while ( p-- ) {
    ++pos;
    current = next1;
    next1 = next2;
    next2 = next3;
    next3 = ( pos + 3 < length ) ? code[ pos + 3 ].unicode() : 0;
  }
}

inline void QSLexer::setDone( State s )
{
  state = s;
  done = true;
}

#ifdef QSDEBUGGER
int QSLexer::lex( YYSTYPE *lvalp, YYLTYPE *llocp )
#else
int QSLexer::lex( YYSTYPE *lvalp, YYLTYPE * )
#endif
{
  int token = 0;
  state = Start;
  ushort stringType = 0; // either single or double quotes
  pos8 = pos16 = hash_ = 0;
  done = false;
  terminator = false;

  // did we push a token on the stack previously ?
  // (after an automatic semicolon insertion)
  if ( stackToken >= 0 )
  {
    setDone( Other );
    token = stackToken;
    stackToken = 0;
  }

  while ( !done )
  {
    switch ( state ) {
    case Start:
      if ( isWhiteSpace() ) {
        // do nothing
      } else if ( current == '/' && next1 == '/' ) {
        shift( 1 );
        state = InSingleLineComment;
      } else if ( current == '/' && next1 == '*' ) {
        shift( 1 );
        state = InMultiLineComment;
      } else if ( current == 0 ) {
        if ( !terminator && !delimited ) {
          // automatic semicolon insertion if program incomplete
          token = ';';
          stackToken = 0;
          setDone( Other );
        } else
          setDone( Eof );
      } else if ( isLineTerminator() ) {
        shiftWindowsLineBreak();
        ++yylineno;
        bol = true;
        terminator = true;
        if ( restrKeyword ) {
          token = ';';
          setDone( Other );
        }
      } else if ( current == '"' || current == '\'' ) {
        state = InString;
        stringType = static_cast<unsigned short>( current );
      } else if ( isIdentLetter( current ) ) {
        record16( current );
        state = InIdentifier;
      } else if ( current == '0' ) {
        record8( current );
        state = InNum0;
      } else if ( isDecimalDigit( current ) ) {
        record8( current );
        state = InNum;
      } else if ( current == '.' && isDecimalDigit( next1 ) ) {
        record8( current );
        state = InDecimal;
      } else {
        token = matchPunctuator( current, next1, next2, next3 );
        if ( token != -1 )
          setDone( Other );
        else {
          setDone( Bad );
          errmsg = "Illegal character";
        }
      }
      break;
    case InString:
      if ( current == stringType ) {
        shift( 1 );
        setDone( String );
      } else if ( current == 0 || isLineTerminator() ) {
        setDone( Bad );
        errmsg = "Unclosed string at end of line";
      } else if ( current == '\\' ) {
        state = InEscapeSequence;
      } else {
        record16( current );
      }
      break;
      // Escape Sequences inside of strings
    case InEscapeSequence:
      if ( isOctalDigit( current ) ) {
        if ( current >= '0' && current <= '3' &&
             isOctalDigit( next1 ) && isOctalDigit( next2 ) ) {
          record16( convertOctal( current, next1, next2 ) );
          shift( 2 );
          state = InString;
        } else if ( isOctalDigit( current ) &&
                    isOctalDigit( next1 ) ) {
          record16( convertOctal( '0', current, next1 ) );
          shift( 1 );
          state = InString;
        } else if ( isOctalDigit( current ) ) {
          record16( convertOctal( '0', '0', current ) );
          state = InString;
        } else {
          setDone( Bad );
          errmsg = "Illegal escape squence";
        }
      } else if ( current == 'x' )
        state = InHexEscape;
      else if ( current == 'u' )
        state = InUnicodeEscape;
      else {
        record16( singleEscape( static_cast<unsigned short>( current ) ) );
        state = InString;
      }
      break;
    case InHexEscape:
      if ( isHexDigit( current ) && isHexDigit( next1 ) ) {
        state = InString;
        record16( convertHex( current, next1 ) );
        shift( 1 );
      } else if ( current == stringType ) {
        record16( 'x' );
        shift( 1 );
        setDone( String );
      } else {
        record16( 'x' );
        record16( current );
        state = InString;
      }
      break;
    case InUnicodeEscape:
      if ( isHexDigit( current ) && isHexDigit( next1 ) &&
           isHexDigit( next2 ) && isHexDigit( next3 ) ) {
        record16( convertUnicode( current, next1, next2, next3 ) );
        shift( 3 );
        state = InString;
      } else if ( current == stringType ) {
        record16( 'u' );
        shift( 1 );
        setDone( String );
      } else {
        setDone( Bad );
        errmsg = "Illegal unicode escape sequence";
      }
      break;
    case InSingleLineComment:
      if ( isLineTerminator() ) {
        shiftWindowsLineBreak();
        ++yylineno;
        terminator = true;
#ifndef QTSCRIPT_PURE_ECMA

        bol = true;
#endif

        if ( restrKeyword ) {
          token = ';';
          setDone( Other );
        } else
          state = Start;
      } else if ( current == 0 ) {
        setDone( Eof );
      }
      break;
    case InMultiLineComment:
      if ( current == 0 ) {
        setDone( Bad );
        errmsg = "Unclosed comment at end of file";
      } else if ( isLineTerminator() ) {
        shiftWindowsLineBreak();
        ++yylineno;
      } else if ( current == '*' && next1 == '/' ) {
        state = Start;
        shift( 1 );
      }
      break;
    case InIdentifier:
      if ( isIdentLetter( current ) || isDecimalDigit( current ) ) {
        record16( current );
        break;
      }
      setDone( Identifier );
      break;
    case InNum0:
      if ( current == 'x' || current == 'X' ) {
        record8( current );
        state = InHex;
      } else if ( current == '.' ) {
        record8( current );
        state = InDecimal;
      } else if ( current == 'e' || current == 'E' ) {
        record8( current );
        state = InExponentIndicator;
      } else if ( isOctalDigit( current ) ) {
        record8( current );
        state = InOctal;
      } else if ( isDecimalDigit( current ) ) {
        record8( current );
        state = InDecimal;
      } else {
        setDone( Number );
      }
      break;
    case InHex:
      if ( isHexDigit( current ) )
        record8( current );
      else
        setDone( Hex );
      break;
    case InOctal:
      if ( isOctalDigit( current ) ) {
        record8( current );
      } else if ( isDecimalDigit( current ) ) {
        record8( current );
        state = InDecimal;
      } else {
        setDone( Octal );
      }
      break;
    case InNum:
      if ( isDecimalDigit( current ) ) {
        record8( current );
      } else if ( current == '.' ) {
        record8( current );
        state = InDecimal;
      } else if ( current == 'e' || current == 'E' ) {
        record8( current );
        state = InExponentIndicator;
      } else {
        setDone( Number );
      }
      break;
    case InDecimal:
      if ( isDecimalDigit( current ) ) {
        record8( current );
      } else if ( current == 'e' || current == 'E' ) {
        record8( current );
        state = InExponentIndicator;
      } else {
        setDone( Number );
      }
      break;
    case InExponentIndicator:
      if ( current == '+' || current == '-' ) {
        record8( current );
      } else if ( isDecimalDigit( current ) ) {
        record8( current );
        state = InExponent;
      } else {
        setDone( Bad );
        errmsg = "Illegal syntax for exponential number";
      }
      break;
    case InExponent:
      if ( isDecimalDigit( current ) ) {
        record8( current );
      } else {
        setDone( Number );
      }
      break;
    default:
      Q_ASSERT( !"Unhandled state in switch statement" );
    }

    // move on to the next character
    if ( !done )
      shift( 1 );
    if ( state != Start && state != InSingleLineComment )
      bol = false;
  }

  // no identifiers allowed directly after numeric literal, e.g. "3in" is bad
  if ( ( state == Number || state == Octal || state == Hex )
       && isIdentLetter( current ) )
  {
    state = Bad;
    errmsg = "Identifier cannot start with numeric literal";
  }

  // terminate string
  buffer8[ pos8 ] = '\0';

  double dval = 0;
  if ( state == Number )
  {
    dval = QLocale::c().toDouble( QLatin1String( buffer8.constData() ) );
  } else if ( state == Hex )
  { // scan hex numbers
    // TODO: support long uint
    uint i;
    sscanf( buffer8.constData(), "%x", &i );
    dval = i;
    state = Number;
  } else if ( state == Octal )
  {   // scan octal number
    uint ui;
    sscanf( buffer8.constData(), "%o", &ui );
    dval = ui;
    state = Number;
  }

  restrKeyword = false;
  delimited = false;
#ifdef QSDEBUGGER
  llocp->first_line = yylineno; // ???
  llocp->last_line = yylineno;
#endif

  switch ( state )
  {
  case Eof:
    return 0;
  case Other:
    if ( token == '}' || token == ';' )
      delimited = true;
    return token;
  case Identifier:
    if ( ( token = QSLookup::find( &mainTable, buffer16.constData(), pos16 ) ) < 0 ) {
      lvalp->ustr = newUstr( buffer16.constData(), pos16, hash_ );
      return IDENT;
    }
    if ( token == CONTINUE || token == BREAK ||
         token == RETURN || token == THROW )
      restrKeyword = true;
    return token;
  case String:
    lvalp->ustr = newUstr( buffer16.constData(), pos16, hash_ );
    return STRING;
  case Number:
    lvalp->dval = dval;
    return NUMBER;
  case Bad:
    return -1;
  default:
    Q_ASSERT( !"unhandled numeration value in switch" );
    return -1;
  }
}

inline bool QSLexer::isWhiteSpace() const
{
  return ( current == ' ' || current == '\t' ||
           current == 0x0b || current == 0x0c );
}

inline bool QSLexer::isLineTerminator() const
{
  return ( current == '\n' || current == '\r' );
}

inline bool QSLexer::isHexDigit( ushort c ) const
{
  return ( ( c >= '0' && c <= '9' ) ||
           ( c >= 'a' && c <= 'f' ) ||
           ( c >= 'A' && c <= 'F' ) );
}

inline bool QSLexer::isOctalDigit( ushort c ) const
{
  return ( c >= '0' && c <= '7' );
}

inline int QSLexer::matchPunctuator( ushort c1, ushort c2,
                                     ushort c3, ushort c4 )
{
  if ( c1 == '>' && c2 == '>' && c3 == '>' && c4 == '=' ) {
    shift( 4 );
    return URSHIFTEQUAL;
  } else if ( c1 == '=' && c2 == '=' && c3 == '=' ) {
    shift( 3 );
    return STREQ;
  } else if ( c1 == '!' && c2 == '=' && c3 == '=' ) {
    shift( 3 );
    return STRNEQ;
  } else if ( c1 == '>' && c2 == '>' && c3 == '>' ) {
    shift( 3 );
    return URSHIFT;
  } else if ( c1 == '<' && c2 == '<' && c3 == '=' ) {
    shift( 3 );
    return LSHIFTEQUAL;
  } else if ( c1 == '>' && c2 == '>' && c3 == '=' ) {
    shift( 3 );
    return RSHIFTEQUAL;
  } else if ( c1 == '<' && c2 == '=' ) {
    shift( 2 );
    return LE;
  } else if ( c1 == '>' && c2 == '=' ) {
    shift( 2 );
    return GE;
  } else if ( c1 == '!' && c2 == '=' ) {
    shift( 2 );
    return NE;
  } else if ( c1 == '+' && c2 == '+' ) {
    shift( 2 );
    return PLUSPLUS;
  } else if ( c1 == '-' && c2 == '-' ) {
    shift( 2 );
    return MINUSMINUS;
  } else if ( c1 == '=' && c2 == '=' ) {
    shift( 2 );
    return EQEQ;
  } else if ( c1 == '+' && c2 == '=' ) {
    shift( 2 );
    return PLUSEQUAL;
  } else if ( c1 == '-' && c2 == '=' ) {
    shift( 2 );
    return MINUSEQUAL;
  } else if ( c1 == '*' && c2 == '=' ) {
    shift( 2 );
    return MULTEQUAL;
  } else if ( c1 == '/' && c2 == '=' ) {
    shift( 2 );
    return DIVEQUAL;
  } else if ( c1 == '&' && c2 == '=' ) {
    shift( 2 );
    return ANDEQUAL;
  } else if ( c1 == '^' && c2 == '=' ) {
    shift( 2 );
    return XOREQUAL;
  } else if ( c1 == '%' && c2 == '=' ) {
    shift( 2 );
    return MODEQUAL;
  } else if ( c1 == '|' && c2 == '=' ) {
    shift( 2 );
    return OREQUAL;
  } else if ( c1 == '<' && c2 == '<' ) {
    shift( 2 );
    return LSHIFT;
  } else if ( c1 == '>' && c2 == '>' ) {
    shift( 2 );
    return RSHIFT;
  } else if ( c1 == '&' && c2 == '&' ) {
    shift( 2 );
    return AND;
  } else if ( c1 == '|' && c2 == '|' ) {
    shift( 2 );
    return OR;
  }

  switch ( c1 ) {
  case '=':
  case '>':
  case '<':
  case ',':
  case '!':
  case '~':
  case '?':
  case ':':
  case '.':
  case '+':
  case '-':
  case '*':
  case '/':
  case '&':
  case '|':
  case '^':
  case '%':
  case '(':
  case ')':
  case '{':
  case '}':
  case '[':
  case ']':
  case ';':
    shift( 1 );
    return static_cast<int>( c1 );
  default:
    return -1;
  }
}

inline ushort QSLexer::singleEscape( ushort c ) const
{
  switch ( c ) {
  case 'b':
    return 0x08;
  case 't':
    return 0x09;
  case 'n':
    return 0x0A;
  case 'v':
    return 0x0B;
  case 'f':
    return 0x0C;
  case 'r':
    return 0x0D;
  case '"':
    return 0x22;
  case '\'':
    return 0x27;
  case '\\':
    return 0x5C;
  default:
    return c;
  }
}

inline ushort QSLexer::convertOctal( ushort c1, ushort c2,
                                     ushort c3 ) const
{
  return static_cast<unsigned short>( ( c1 - '0' ) * 64 + ( c2 - '0' ) * 8 + c3 - '0' );
}

bool QSLexer::scanRegExp()
{
  pos16 = 0;
  bool lastWasEscape = false;

  while ( 1 ) {
    if ( isLineTerminator() || current == 0 )
      return false;
    else if ( current != '/' || lastWasEscape == true ) {
      record16( current );
      lastWasEscape = !lastWasEscape && ( current == '\\' );
    } else {
      pattern = newUstr( buffer16.constData(), pos16, hash_ );
      pos16 = hash_ = 0;
      shift( 1 );
      break;
    }
    shift( 1 );
  }

  while ( isIdentLetter( current ) ) {
    record16( current );
    shift( 1 );
  }
  flags = newUstr( buffer16.constData(), pos16, hash_ );

  return true;
}

inline void QSLexer::record8( ushort c )
{
  if ( pos8 >= size8 - 1 ) {
    size8 *= 2;
    buffer8.resize( size8 );
  }
  buffer8[ pos8++ ] = static_cast<char>( c );
}

inline void QSLexer::record16( QChar c )
{
  if ( pos16 >= size16 - 1 ) {
    size16 *= 2;
    buffer16.resize( size16 );
  }
  buffer16[ pos16++ ] = c;
  hash_ += c.unicode();
  hash_ += ( hash_ << 10 );
  hash_ ^= ( hash_ >> 6 );
}

inline uint QSLexer::newUstr( const QChar * c, uint len, uint h )
{
  if ( !len )
    return 0;
  h += ( h << 3 );
  h ^= ( h >> 11 );
  h += ( h << 15 );
  if ( !ustrHash.contains( h ) )
    ustrHash.insert( h, QString( c, len ) );
  return h;
}

uint QSLexer::newUstr( const QString & s )
{
  int len = s.length();
  if ( !len )
    return 0;
  uint hash = 0;
  for ( int i = 0; i < len; ++i ) {
    hash += s[ i ].unicode();
    hash += ( hash << 10 );
    hash ^= ( hash >> 6 );
  }
  hash += ( hash << 3 );
  hash ^= ( hash >> 11 );
  hash += ( hash << 15 );
  if ( !ustrHash.contains( hash ) )
    ustrHash.insert( hash, s );
  return hash;
}

uint QSLexer::indexOfUstr( const QString & s )
{
  int len = s.length();
  if ( !len )
    return 0;
  uint hash = 0;
  for ( int i = 0; i < len; ++i ) {
    hash += s[ i ].unicode();
    hash += ( hash << 10 );
    hash ^= ( hash >> 6 );
  }
  hash += ( hash << 3 );
  hash ^= ( hash >> 11 );
  hash += ( hash << 15 );
  return ( ustrHash.contains( hash ) ? hash : 0 );
}

uint QSLexer::registerUstr( const QString & s )
{
  uint value = ustrRegs.value( s );
  if ( !value ) {
    int len = s.length();
    if ( !len )
      return 0;
    for ( int i = 0; i < len; ++i ) {
      value += s[ i ].unicode();
      value += ( value << 10 );
      value ^= ( value >> 6 );
    }
    value += ( value << 3 );
    value ^= ( value >> 11 );
    value += ( value << 15 );
    if ( !ustrHash.contains( value ) )
      ustrHash.insert( value, s );
    ustrRegs.insert( s, value );
  }
  return value;
}
