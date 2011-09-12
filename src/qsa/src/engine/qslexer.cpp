/****************************************************************************
** $Id: qslexer.cpp  1.1.5   edited 2006-05-22T18:01:36$
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "qslexer.h"
#include "qsengine.h"
#include "qsnodes.h"
#include "qslookup.h"
#include "qsinternal.h"
#include "grammar.h"
#include "qslexer.lut.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define shiftWindowsLineBreak() if( current == '\r' && next1 == '\n' ) shift( 1 );


#ifdef QSDEBUGGER
extern YYLTYPE qsyylloc;  // global bison variable holding token info
#endif

QSLexer *QSLexer::lx = 0;
int QSLexer::sid;

// a bridge for yacc from the C world to C++
int qsyylex()
{
  return QSLexer::lexer()->lex();
}

QSLexer::QSLexer()
  : yylineno(1),
    size8(128), size16(128), restrKeyword(false),
    stackToken(-1), pos(0),
    code(0), length(0),
    bol(true),
    current(0), next1(0), next2(0), next3(0),
    localeC(QLocale::C)
{
#if YYDEBUG == 1
  extern int qsyydebug;
  qsyydebug = 1;
#endif

  Q_ASSERT(lx == 0);
  lx = this;

  // allocate space for read buffers
  buffer8 = new char[size8];
  buffer16 = new QChar[size16];

  ustrHash.resize(199);
  ustrHash.setAutoDelete(true);
}

QSLexer::~QSLexer()
{
  delete [] buffer8;
  delete [] buffer16;

  if (lx == this)
    lx = 0;
}

/*!
  Sets the code to be parsed.
  \a lineno specifies which line which is to be interpreted
  as line number zero. \a id specifies the unique identification
  number for this source code unit.
*/
void QSLexer::setCode(const QString &c, int id, int lineno)
{
  errmsg = QString::null;
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
  current = (length > 0) ? code[0].unicode() : 0;
  next1 = (length > 1) ? code[1].unicode() : 0;
  next2 = (length > 2) ? code[2].unicode() : 0;
  next3 = (length > 3) ? code[3].unicode() : 0;
}

int QSLexer::lex()
{
  int token = 0;
  state = Start;
  ushort stringType = 0; // either single or double quotes
  pos8 = pos16 = 0;
  done = false;
  terminator = false;

  // did we push a token on the stack previously ?
  // (after an automatic semicolon insertion)
  if (stackToken >= 0) {
    setDone(Other);
    token = stackToken;
    stackToken = 0;
  }

  while (!done) {
    switch (state) {
      case Start:
        if (isWhiteSpace()) {
          // do nothing
        } else if (current == '/' && next1 == '/') {
          shift(1);
          state = InSingleLineComment;
        } else if (current == '/' && next1 == '*') {
          shift(1);
          state = InMultiLineComment;
        } else if (current == 0) {
          if (!terminator && !delimited) {
            // automatic semicolon insertion if program incomplete
            token = ';';
            stackToken = 0;
            setDone(Other);
          } else
            setDone(Eof);
        } else if (isLineTerminator()) {
          shiftWindowsLineBreak();
          yylineno++;
          bol = true;
          terminator = true;
          if (restrKeyword) {
            token = ';';
            setDone(Other);
          }
        } else if (current == '"' || current == '\'') {
          state = InString;
          stringType = current;
        } else if (isIdentLetter(current)) {
          record16(current);
          state = InIdentifier;
        } else if (current == '0') {
          record8(current);
          state = InNum0;
        } else if (isDecimalDigit(current)) {
          record8(current);
          state = InNum;
        } else if (current == '.' && isDecimalDigit(next1)) {
          record8(current);
          state = InDecimal;
        } else {
          token = matchPunctuator(current, next1, next2, next3);
          if (token != -1)
            setDone(Other);
          else {
            setDone(Bad);
            errmsg = "Illegal character";
          }
        }
        break;
      case InString:
        if (current == stringType) {
          shift(1);
          setDone(String);
        } else if (current == 0 || isLineTerminator()) {
          setDone(Bad);
          errmsg = "Unclosed string at end of line";
        } else if (current == '\\') {
          state = InEscapeSequence;
        } else {
          record16(current);
        }
        break;
        // Escape Sequences inside of strings
      case InEscapeSequence:
        if (isOctalDigit(current)) {
          if (current >= '0' && current <= '3' &&
              isOctalDigit(next1) && isOctalDigit(next2)) {
            record16(convertOctal(current, next1, next2));
            shift(2);
            state = InString;
          } else if (isOctalDigit(current) &&
                     isOctalDigit(next1)) {
            record16(convertOctal('0', current, next1));
            shift(1);
            state = InString;
          } else if (isOctalDigit(current)) {
            record16(convertOctal('0', '0', current));
            state = InString;
          } else {
            setDone(Bad);
            errmsg = "Illegal escape squence";
          }
        } else if (current == 'x')
          state = InHexEscape;
        else if (current == 'u')
          state = InUnicodeEscape;
        else {
          record16(singleEscape(current));
          state = InString;
        }
        break;
      case InHexEscape:
        if (isHexDigit(current) && isHexDigit(next1)) {
          state = InString;
          record16(convertHex(current, next1));
          shift(1);
        } else if (current == stringType) {
          record16('x');
          shift(1);
          setDone(String);
        } else {
          record16('x');
          record16(current);
          state = InString;
        }
        break;
      case InUnicodeEscape:
        if (isHexDigit(current) && isHexDigit(next1) &&
            isHexDigit(next2) && isHexDigit(next3)) {
          record16(convertUnicode(current, next1, next2, next3));
          shift(3);
          state = InString;
        } else if (current == stringType) {
          record16('u');
          shift(1);
          setDone(String);
        } else {
          setDone(Bad);
          errmsg = "Illegal unicode escape sequence";
        }
        break;
      case InSingleLineComment:
        if (isLineTerminator()) {
          shiftWindowsLineBreak();
          yylineno++;
          terminator = true;
#ifndef QTSCRIPT_PURE_ECMA
          bol = true;
#endif
          if (restrKeyword) {
            token = ';';
            setDone(Other);
          } else
            state = Start;
        } else if (current == 0) {
          setDone(Eof);
        }
        break;
      case InMultiLineComment:
        if (current == 0) {
          setDone(Bad);
          errmsg = "Unclosed comment at end of file";
        } else if (isLineTerminator()) {
          shiftWindowsLineBreak();
          yylineno++;
        } else if (current == '*' && next1 == '/') {
          state = Start;
          shift(1);
        }
        break;
      case InIdentifier:
        if (isIdentLetter(current) || isDecimalDigit(current)) {
          record16(current);
          break;
        }
        setDone(Identifier);
        break;
      case InNum0:
        if (current == 'x' || current == 'X') {
          record8(current);
          state = InHex;
        } else if (current == '.') {
          record8(current);
          state = InDecimal;
        } else if (current == 'e' || current == 'E') {
          record8(current);
          state = InExponentIndicator;
        } else if (isOctalDigit(current)) {
          record8(current);
          state = InOctal;
        } else if (isDecimalDigit(current)) {
          record8(current);
          state = InDecimal;
        } else {
          setDone(Number);
        }
        break;
      case InHex:
        if (isHexDigit(current))
          record8(current);
        else
          setDone(Hex);
        break;
      case InOctal:
        if (isOctalDigit(current)) {
          record8(current);
        } else if (isDecimalDigit(current)) {
          record8(current);
          state = InDecimal;
        } else {
          setDone(Octal);
        }
        break;
      case InNum:
        if (isDecimalDigit(current)) {
          record8(current);
        } else if (current == '.') {
          record8(current);
          state = InDecimal;
        } else if (current == 'e' || current == 'E') {
          record8(current);
          state = InExponentIndicator;
        } else if (current == 'k') {
          shift(1);
          setDone(KeywordNumber);
        } else {
          setDone(Number);
        }
        break;
      case InDecimal:
        if (isDecimalDigit(current)) {
          record8(current);
        } else if (current == 'e' || current == 'E') {
          record8(current);
          state = InExponentIndicator;
        } else {
          setDone(Number);
        }
        break;
      case InExponentIndicator:
        if (current == '+' || current == '-') {
          record8(current);
        } else if (isDecimalDigit(current)) {
          record8(current);
          state = InExponent;
        } else {
          setDone(Bad);
          errmsg = "Illegal syntax for exponential number";
        }
        break;
      case InExponent:
        if (isDecimalDigit(current)) {
          record8(current);
        } else {
          setDone(Number);
        }
        break;
      default:
        Q_ASSERT(!"Unhandled state in switch statement");
    }

    // move on to the next character
    if (!done)
      shift(1);
    if (state != Start && state != InSingleLineComment)
      bol = false;
  }

  // no identifiers allowed directly after numeric literal, e.g. "3in" is bad
  if ((state == Number || state == Octal || state == Hex)
      && isIdentLetter(current)) {
    state = Bad;
    errmsg = "Identifier cannot start with numeric literal";
  }

  // terminate string
  buffer8[pos8] = '\0';

  double dval = 0;
  if (state == Number) {
    dval = localeC.toDouble(buffer8);
  } else if (state == Hex) {   // scan hex numbers
    // TODO: support long uint
    uint i;
    sscanf(buffer8, "%x", &i);
    dval = i;
    state = Number;
  } else if (state == Octal) {     // scan octal number
    uint ui;
    sscanf(buffer8, "%o", &ui);
    dval = ui;
    state = Number;
  }

  restrKeyword = false;
  delimited = false;
#ifdef QSDEBUGGER
  qsyylloc.first_line = yylineno; // ???
  qsyylloc.last_line = yylineno;
#endif

  switch (state) {
    case Eof:
      return 0;
    case Other:
      if (token == '}' || token == ';')
        delimited = true;
      return token;
    case Identifier:
      if ((token = QSLookup::find(&mainTable, buffer16, pos16)) < 0) {
        /* TODO: close leak on parse error. same holds true for String */
        qsyylval.ustr = newUstr(buffer16, pos16);
        return IDENT;
      }
      if (token == CONTINUE || token == BREAK ||
          token == RETURN || token == THROW)
        restrKeyword = true;
      return token;
    case KeywordNumber:
      token = localeC.toInt(buffer8);
      if (token == CONTINUE || token == BREAK ||
          token == RETURN || token == THROW)
        restrKeyword = true;
      return token;
    case String:
      qsyylval.ustr = newUstr(buffer16, pos16);
      return STRING;
    case Number:
      qsyylval.dval = dval;
      return NUMBER;
    case Bad:
      return -1;
    default:
      Q_ASSERT(!"unhandled numeration value in switch");
      return -1;
  }
}

int QSLexer::matchPunctuator(ushort c1, ushort c2,
                             ushort c3, ushort c4)
{
  if (c1 == '>' && c2 == '>' && c3 == '>' && c4 == '=') {
    shift(4);
    return URSHIFTEQUAL;
  } else if (c1 == '=' && c2 == '=' && c3 == '=') {
    shift(3);
    return STREQ;
  } else if (c1 == '!' && c2 == '=' && c3 == '=') {
    shift(3);
    return STRNEQ;
  } else if (c1 == '>' && c2 == '>' && c3 == '>') {
    shift(3);
    return URSHIFT;
  } else if (c1 == '<' && c2 == '<' && c3 == '=') {
    shift(3);
    return LSHIFTEQUAL;
  } else if (c1 == '>' && c2 == '>' && c3 == '=') {
    shift(3);
    return RSHIFTEQUAL;
  } else if (c1 == '<' && c2 == '=') {
    shift(2);
    return LE;
  } else if (c1 == '>' && c2 == '=') {
    shift(2);
    return GE;
  } else if (c1 == '!' && c2 == '=') {
    shift(2);
    return NE;
  } else if (c1 == '+' && c2 == '+') {
    shift(2);
    return PLUSPLUS;
  } else if (c1 == '-' && c2 == '-') {
    shift(2);
    return MINUSMINUS;
  } else if (c1 == '=' && c2 == '=') {
    shift(2);
    return EQEQ;
  } else if (c1 == '+' && c2 == '=') {
    shift(2);
    return PLUSEQUAL;
  } else if (c1 == '-' && c2 == '=') {
    shift(2);
    return MINUSEQUAL;
  } else if (c1 == '*' && c2 == '=') {
    shift(2);
    return MULTEQUAL;
  } else if (c1 == '/' && c2 == '=') {
    shift(2);
    return DIVEQUAL;
  } else if (c1 == '&' && c2 == '=') {
    shift(2);
    return ANDEQUAL;
  } else if (c1 == '^' && c2 == '=') {
    shift(2);
    return XOREQUAL;
  } else if (c1 == '%' && c2 == '=') {
    shift(2);
    return MODEQUAL;
  } else if (c1 == '|' && c2 == '=') {
    shift(2);
    return OREQUAL;
  } else if (c1 == '<' && c2 == '<') {
    shift(2);
    return LSHIFT;
  } else if (c1 == '>' && c2 == '>') {
    shift(2);
    return RSHIFT;
  } else if (c1 == '&' && c2 == '&') {
    shift(2);
    return AND;
  } else if (c1 == '|' && c2 == '|') {
    shift(2);
    return OR;
  }

  switch (c1) {
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
      shift(1);
      return static_cast<int>(c1);
    default:
      return -1;
  }
}
