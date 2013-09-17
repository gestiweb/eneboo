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

#ifndef QSLEXER_H
#define QSLEXER_H

#include <QDebug>
#include <QString>
#include <QVarLengthArray>
#include <QHash>

#define NEW_USTR(S)      QSLexer::newUstr(S)
#define NEW_USTRNUM(S)   QSLexer::newUstrNumber(S)
#define USTR(S)          QSLexer::ustr(S)
#define REF_USTR(S)      QSLexer::ref(S)
#define DEREF_USTR(S)    QSLexer::deref(S)
#define ERASE_USTR(S)    QSLexer::erase(S)
#define HASH_USTR(S)     QSLexer::hashUstr(S)
#define HASH_USTRNUM(S)  QSLexer::hashUstrNumber(S)
#define INDEX_USTR(S)    QSLexer::indexOfUstr(S)
#define REG_USTR(S)      QSLexer::registerUstr(S)

#define MAX_USTR_NUM  256
#define INIT_SIZE_BUFFER8 128
#define INIT_SIZE_BUFFER16 128

class QSProgramNode;
union YYSTYPE;
struct YYLTYPE;

class QSLexer
{

public:

  enum State { Start,
               Identifier,
               InIdentifier,
               InSingleLineComment,
               InMultiLineComment,
               InNum,
               InNum0,
               InHex,
               InOctal,
               InDecimal,
               InExponentIndicator,
               InExponent,
               Hex,
               Octal,
               Number,
               String,
               Eof,
               InString,
               InEscapeSequence,
               InHexEscape,
               InUnicodeEscape,
               Other,
               Bad };

  QSLexer();
  ~QSLexer();

  void setCode( const QString &c, int id, int lineno = 0 );
  int lex( YYSTYPE *lvalp, YYLTYPE *llocp );

  int lineNo() const
  {
    return yylineno + 1;
  }

  int sourceId() const
  {
    return sid;
  }

  bool prevTerminator() const
  {
    return terminator;
  }

  bool scanRegExp();
  uint pattern, flags;

  State lexerState() const
  {
    return state;
  }

  void setState( State st )
  {
    state = st;
  }

  QString errorMessage() const
  {
    return errmsg;
  }

  void setErrorMessage( const QString &err )
  {
    errmsg = err;
  }

  void setErrorMessage( const char *err )
  {
    setErrorMessage( QLatin1String( err ) );
  }

  void setRunLineNo( int l )
  {
    runlineno = l;
  }

  int runlineNo() const
  {
    return runlineno;
  }

private:

  int yylineno;
  int runlineno;
  bool done;
  QVarLengthArray<char, INIT_SIZE_BUFFER8 * 2> buffer8;
  QVarLengthArray<QChar, INIT_SIZE_BUFFER16 * 2> buffer16;
  uint size8, size16;
  uint pos8, pos16, hash_;
  bool terminator;
  bool restrKeyword;
  // encountered delimiter like "'" and "}" on last run
  bool delimited;
  int stackToken;

  State state;
  inline void setDone( State s );
  uint pos;
  inline void shift( uint p );

  inline bool isWhiteSpace() const;
  inline bool isLineTerminator() const;
  inline bool isHexDigit( ushort c ) const;
  inline bool isOctalDigit( ushort c ) const;

  inline int matchPunctuator( ushort c1, ushort c2,
                              ushort c3, ushort c4 );
  inline ushort singleEscape( ushort c ) const;
  inline ushort convertOctal( ushort c1, ushort c2,
                              ushort c3 ) const;
public:

  static uchar convertHex( ushort c )
  {
    if ( c >= '0' && c <= '9' )
      return static_cast<uchar>( c - '0' );
    else if ( c >= 'a' && c <= 'f' )
      return static_cast<uchar>( c - 'a' + 10 );
    else
      return static_cast<uchar>( c - 'A' + 10 );
  }

  static uchar convertHex( ushort c1, ushort c2 )
  {
    return ( ( convertHex( c1 ) << 4 ) + convertHex( c2 ) );
  }

  static QChar convertUnicode( ushort c1, ushort c2,
                               ushort c3, ushort c4 )
  {
    return QChar( ( convertHex( c3 ) << 4 ) + convertHex( c4 ),
                  ( convertHex( c1 ) << 4 ) + convertHex( c2 ) );
  }

  static bool isIdentLetter( ushort c )
  {
    /* TODO: allow other legitimate unicode chars */
    return ( ( c >= 'a' && c <= 'z' ) ||
             ( c >= 'A' && c <= 'Z' ) ||
             ( c == '$' || c == '_' ) );
  }

  static bool isDecimalDigit( ushort c )
  {
    return ( c >= '0' && c <= '9' );
  }

private:

  int sid;

  inline void record8( ushort c );
  inline void record16( QChar c );

  const QChar * code;
  uint length;
  int yycolumn;
  int bol;     // begin of line

  // current and following unicode characters
  ushort current, next1, next2, next3;

  struct keyword
  {
    const char *name;
    int token;
  };

  QString errmsg;

public:

  static QString ustr( uint i )
  {
    return ustrHash.value( i );
  }

  static void ref( uint i )
  {
    if ( i > 0 )
      ustrRefs[ i ] ++;
  }

  static void deref( uint i )
  {
    if ( i > 0 && !( --ustrRefs[ i ] ) )
      ustrHash.erase( ustrHash.find( i ) );
  }

  static void erase( uint i )
  {
    if ( i > 0 && !ustrRefs.value( i ) )
      ustrHash.erase( ustrHash.find( i ) );
  }

  static uint newUstrNumber( uint i )
  {
    if ( !i )
      return zero_;
    if ( i < MAX_USTR_NUM )
      return ustrNumbers[ i ];
    else
      return newUstr( QString::number( i ) );
  }

  static uint newUstr( const QString & s );

  static uint indexOfUstr( const QString & s );

  static uint hashUstrNumber( uint i )
  {
    if ( !i )
      return zero_;
    if ( i < MAX_USTR_NUM )
      return ustrNumbers[ i ];
    else
      return hashUstr( QString::number( i ) );
  }

  static uint hashUstr( const QString & s )
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
    return hash;
  }

  static uint registerUstr( const QString & s );

  void setLastNode( QSProgramNode * p )
  {
    prog = p;
  }

  QSProgramNode * lastNode() const
  {
    return prog;
  }

  static int count()
  {
    return count_++;
  }

  static QHash<uint, QString> & ustringHash()
  {
    return ustrHash;
  }

private:

  static uint inline newUstr( const QChar * c, uint len, uint h );

  static QHash<uint, QString> ustrHash;
  static QHash<uint, qint16> ustrRefs;
  static QHash<QString, uint> ustrRegs;
  static QVarLengthArray<uint, MAX_USTR_NUM> ustrNumbers;

  QSProgramNode * prog;
  static int count_;
  static int instances_;

public:

  static uint undefined_;
  static uint null_;
  static uint true_;
  static uint false_;
  static uint function_;
  static uint noop_;
  static uint anony_;
  static uint args_;
  static uint object_;
  static uint boolean_;
  static uint number_;
  static uint string_;
  static uint length_;
  static uint zero_;
  static uint application_;
  static uint global_;
  static uint lastindex_;
};

#endif
