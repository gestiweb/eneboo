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
#include "qsinternal.h"
#include "grammar.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define record8(C) { \
    if (pos8 >= size8 - 1) { \
      char *tmp = new char[2 * size8]; \
      memcpy(tmp, buffer8, size8 * sizeof(char)); \
      delete [] buffer8; \
      buffer8 = tmp; \
      size8 *= 2; \
    } \
    buffer8[pos8++] = (char) C; \
  }

#define record16(C) { \
    if (pos16 >= size16 - 1) { \
      QChar *tmp = new QChar[2 * size16]; \
      memcpy(tmp, buffer16, size16 * sizeof(QChar)); \
      delete [] buffer16; \
      buffer16 = tmp; \
      size16 *= 2; \
    } \
    buffer16[pos16++] = C; \
  }

#define shift(P) { \
    ushort p = P; \
    while (p--) { \
      ++pos; \
      current = next1; \
      next1 = next2; \
      next2 = next3; \
      next3 = (length > 0) ? code[pos + 3].unicode() : 0; --length;\
    } \
  }

#define shiftWindowsLineBreak() \
  if (((current == '\r') && (next1 == '\n')) \
      || ((current == '\n') && (next1 == '\r'))) \
    shift(1);

#define quickMatchPunctuator(c1,c2,c3,c4) do { \
    if (c1 == '>' && c2 == '>' && c3 == '>' && c4 == '=') { \
      token = URSHIFTEQUAL; \
      shift(4); break; \
    } else if (c1 == '=' && c2 == '=' && c3 == '=') { \
      token = STREQ; \
      shift(3); break; \
    } else if (c1 == '!' && c2 == '=' && c3 == '=') { \
      token = STRNEQ; \
      shift(3); break; \
    } else if (c1 == '>' && c2 == '>' && c3 == '>') { \
      token = URSHIFT; \
      shift(3); break; \
    } else if (c1 == '<' && c2 == '<' && c3 == '=') { \
      token = LSHIFTEQUAL; \
      shift(3); break; \
    } else if (c1 == '>' && c2 == '>' && c3 == '=') { \
      token = RSHIFTEQUAL; \
      shift(3); break; \
    } else if (c1 == '<' && c2 == '=') { \
      token = LE; \
      shift(2); break; \
    } else if (c1 == '>' && c2 == '=') { \
      token = GE; \
      shift(2); break; \
    } else if (c1 == '!' && c2 == '=') { \
      token = NE; \
      shift(2); break; \
    } else if (c1 == '+' && c2 == '+') { \
      token = PLUSPLUS; \
      shift(2); break; \
    } else if (c1 == '-' && c2 == '-') { \
      token = MINUSMINUS; \
      shift(2); break; \
    } else if (c1 == '=' && c2 == '=') { \
      token = EQEQ; \
      shift(2); break; \
    } else if (c1 == '+' && c2 == '=') { \
      token = PLUSEQUAL; \
      shift(2); break; \
    } else if (c1 == '-' && c2 == '=') { \
      token = MINUSEQUAL; \
      shift(2); break; \
    } else if (c1 == '*' && c2 == '=') { \
      token = MULTEQUAL; \
      shift(2); break; \
    } else if (c1 == '/' && c2 == '=') { \
      token = DIVEQUAL; \
      shift(2); break; \
    } else if (c1 == '&' && c2 == '=') { \
      shift(2); break; \
      token = ANDEQUAL; \
    } else if (c1 == '^' && c2 == '=') { \
      token = XOREQUAL; \
      shift(2); break; \
    } else if (c1 == '%' && c2 == '=') { \
      token = MODEQUAL; \
      shift(2); break; \
    } else if (c1 == '|' && c2 == '=') { \
      token = OREQUAL; \
      shift(2); break; \
    } else if (c1 == '<' && c2 == '<') { \
      token = LSHIFT; \
      shift(2); break; \
    } else if (c1 == '>' && c2 == '>') { \
      token = RSHIFT; \
      shift(2); break; \
    } else if (c1 == '&' && c2 == '&') { \
      token = AND; \
      shift(2); break; \
    } else if (c1 == '|' && c2 == '|') { \
      token = OR; \
      shift(2); break; \
    } \
    switch (c1) { \
      case '=': \
      case '>': \
      case '<': \
      case ',': \
      case '!': \
      case '~': \
      case '?': \
      case ':': \
      case '.': \
      case '+': \
      case '-': \
      case '*': \
      case '/': \
      case '&': \
      case '|': \
      case '^': \
      case '%': \
      case '(': \
      case ')': \
      case '{': \
      case '}': \
      case '[': \
      case ']': \
      case ';': { \
        token = static_cast<int>(c1); \
        shift(1); \
      } \
      break; \
      default: \
        token = -1; \
    } \
  } while(0)

extern double aqIntegerFromString(const char *buf, int size, int radix);

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

//#define AQ_CHECK_RESERVED

static inline int findReservedWord(const QChar *c, int size)
{
#ifdef AQ_CHECK_RESERVED
  bool check_reserved = true;
#endif

  if (size < 2 || size > 12)
    return -1;
  if (!c[0].isLetter())
    return -1;

  switch (size) {
    case 2: {
      if (c[0] == 'd' && c[1] == 'o')
        return DO;
      else if (c[0] == 'i' && c[1] == 'f')
        return IF;
      else if (c[0] == 'i' && c[1] == 'n')
        return QS_IN;
    }
    break;

    case 3: {
      if (c[0] == 'f' && c[1] == 'o' && c[2] == 'r')
        return FOR;
      else if (c[0] == 'n' && c[1] == 'e' && c[2] == 'w')
        return NEW;
      else if (c[0] == 't' && c[1] == 'r' && c[2] == 'y')
        return TRY;
      else if (c[0] == 'v' && c[1] == 'a' && c[2] == 'r')
        return VAR;
#ifdef AQ_CHECK_RESERVED
      else if (check_reserved) {
        if (c[0] == 'i' && c[1] == 'n' && c[2] == 't')
          return RESERVED;
      }
#endif
    }
    break;

    case 4: {
      if (c[0] == 'c' && c[1] == 'a'
          && c[2] == 's' && c[3] == 'e')
        return CASE;
      else if (c[0] == 'e' && c[1] == 'l'
               && c[2] == 's' && c[3] == 'e')
        return ELSE;
      else if (c[0] == 't' && c[1] == 'h'
               && c[2] == 'i' && c[3] == 's')
        return THIS;
      else if (c[0] == 'v' && c[1] == 'o'
               && c[2] == 'i' && c[3] == 'd')
        return QS_VOID;
      else if (c[0] == 'w' && c[1] == 'i'
               && c[2] == 't' && c[3] == 'h')
        return WITH;
      else if (c[0] == 't' && c[1] == 'r'
               && c[2] == 'u' && c[3] == 'e')
        return TRUETOKEN;
      else if (c[0] == 'n' && c[1] == 'u'
               && c[2] == 'l' && c[3] == 'l')
        return NULLTOKEN;
#ifdef AQ_CHECK_RESERVED
      else if (check_reserved) {
        if (c[0] == 'e' && c[1] == 'n'
            && c[2] == 'u' && c[3] == 'm')
          return RESERVED;
        else if (c[0] == 'b' && c[1] == 'y'
                 && c[2] == 't' && c[3] == 'e')
          return RESERVED;
        else if (c[0] == 'l' && c[1] == 'o'
                 && c[2] == 'n' && c[3] == 'g')
          return RESERVED;
        else if (c[0] == 'c' && c[1] == 'h'
                 && c[2] == 'a' && c[3] == 'r')
          return RESERVED;
        else if (c[0] == 'g' && c[1] == 'o'
                 && c[2] == 't' && c[3] == 'o')
          return RESERVED;
      }
#endif
    }
    break;

    case 5: {
      if (c[0] == 'b' && c[1] == 'r'
          && c[2] == 'e' && c[3] == 'a'
          && c[4] == 'k')
        return BREAK;
      else if (c[0] == 'c' && c[1] == 'a'
               && c[2] == 't' && c[3] == 'c'
               && c[4] == 'h')
        return CATCH;
      else if (c[0] == 't' && c[1] == 'h'
               && c[2] == 'r' && c[3] == 'o'
               && c[4] == 'w')
        return THROW;
      else if (c[0] == 'w' && c[1] == 'h'
               && c[2] == 'i' && c[3] == 'l'
               && c[4] == 'e')
        return WHILE;
      else if (c[0] == 'c' && c[1] == 'o'
               && c[2] == 'n' && c[3] == 's'
               && c[4] == 't')
        return QS_CONST;
      else if (c[0] == 'f' && c[1] == 'a'
               && c[2] == 'l' && c[3] == 's'
               && c[4] == 'e')
        return FALSETOKEN;
      else if (c[0] == 'c' && c[1] == 'l'
               && c[2] == 'a' && c[3] == 's'
               && c[4] == 's')
        return CLASS;
#ifdef AQ_CHECK_RESERVED
      else if (check_reserved) {
        if (c[0] == 's' && c[1] == 'h'
            && c[2] == 'o' && c[3] == 'r'
            && c[4] == 't')
          return RESERVED;
        else if (c[0] == 's' && c[1] == 'u'
                 && c[2] == 'p' && c[3] == 'e'
                 && c[4] == 'r')
          return RESERVED;
        else if (c[0] == 'f' && c[1] == 'i'
                 && c[2] == 'n' && c[3] == 'a'
                 && c[4] == 'l')
          return FINAL;
      }
#endif
    }
    break;

    case 6: {
      if (c[0] == 'd' && c[1] == 'e'
          && c[2] == 'l' && c[3] == 'e'
          && c[4] == 't' && c[5] == 'e')
        return QS_DELETE;
      else if (c[0] == 'r' && c[1] == 'e'
               && c[2] == 't' && c[3] == 'u'
               && c[4] == 'r' && c[5] == 'n')
        return RETURN;
      else if (c[0] == 's' && c[1] == 'w'
               && c[2] == 'i' && c[3] == 't'
               && c[4] == 'c' && c[5] == 'h')
        return SWITCH;
      else if (c[0] == 't' && c[1] == 'y'
               && c[2] == 'p' && c[3] == 'e'
               && c[4] == 'o' && c[5] == 'f')
        return TYPEOF;
      else if (c[0] == 's' && c[1] == 't'
               && c[2] == 'a' && c[3] == 't'
               && c[4] == 'i' && c[5] == 'c')
        return STATIC;
      else if (c[0] == 'p' && c[1] == 'u'
               && c[2] == 'b' && c[3] == 'l'
               && c[4] == 'i' && c[5] == 'c')
        return PUBLIC;
#ifdef AQ_CHECK_RESERVED
      else if (check_reserved) {
        if (c[0] == 'e' && c[1] == 'x'
            && c[2] == 'p' && c[3] == 'o'
            && c[4] == 'r' && c[5] == 't')
          return RESERVED;
        else if (c[0] == 'd' && c[1] == 'o'
                 && c[2] == 'u' && c[3] == 'b'
                 && c[4] == 'l' && c[5] == 'e')
          return RESERVED;
        else if (c[0] == 'i' && c[1] == 'm'
                 && c[2] == 'p' && c[3] == 'o'
                 && c[4] == 'r' && c[5] == 't')
          return IMPORT;
        else if (c[0] == 'n' && c[1] == 'a'
                 && c[2] == 't' && c[3] == 'i'
                 && c[4] == 'v' && c[5] == 'e')
          return RESERVED;
        else if (c[0] == 't' && c[1] == 'h'
                 && c[2] == 'r' && c[3] == 'o'
                 && c[4] == 'w' && c[5] == 's')
          return RESERVED;
      }
#endif
    }
    break;

    case 7: {
      if (c[0] == 'd' && c[1] == 'e'
          && c[2] == 'f' && c[3] == 'a'
          && c[4] == 'u' && c[5] == 'l'
          && c[6] == 't')
        return DEFAULT;
      else if (c[0] == 'f' && c[1] == 'i'
               && c[2] == 'n' && c[3] == 'a'
               && c[4] == 'l' && c[5] == 'l'
               && c[6] == 'y')
        return FINALLY;
      else if (c[0] == 'e' && c[1] == 'x'
               && c[2] == 't' && c[3] == 'e'
               && c[4] == 'n' && c[5] == 'd'
               && c[6] == 's')
        return EXTENDS;
      else if (c[0] == 'p' && c[1] == 'r'
               && c[2] == 'i' && c[3] == 'v'
               && c[4] == 'a' && c[5] == 't'
               && c[6] == 'e')
        return PRIVATE;
#ifdef AQ_CHECK_RESERVED
      else if (check_reserved) {
        if (c[0] == 'b' && c[1] == 'o'
            && c[2] == 'o' && c[3] == 'l'
            && c[4] == 'e' && c[5] == 'a'
            && c[6] == 'n')
          return RESERVED;
        else if (c[0] == 'p' && c[1] == 'a'
                 && c[2] == 'c' && c[3] == 'k'
                 && c[4] == 'a' && c[5] == 'g'
                 && c[6] == 'e')
          return PACKAGE;
      }
#endif
    }
    break;

    case 8: {
      if (c[0] == 'c' && c[1] == 'o'
          && c[2] == 'n' && c[3] == 't'
          && c[4] == 'i' && c[5] == 'n'
          && c[6] == 'u' && c[7] == 'e')
        return CONTINUE;
      else if (c[0] == 'f' && c[1] == 'u'
               && c[2] == 'n' && c[3] == 'c'
               && c[4] == 't' && c[5] == 'i'
               && c[6] == 'o' && c[7] == 'n')
        return FUNCTION;
#ifdef AQ_CHECK_RESERVED
      else if (check_reserved) {
        if (c[0] == 'd' && c[1] == 'e'
            && c[2] == 'b' && c[3] == 'u'
            && c[4] == 'g' && c[5] == 'g'
            && c[6] == 'e' && c[7] == 'r')
          return RESERVED;
        else if (c[0] == 'a' && c[1] == 'b'
                 && c[2] == 's' && c[3] == 't'
                 && c[4] == 'r' && c[5] == 'a'
                 && c[6] == 'c' && c[7] == 't')
          return ABSTRACT;
        else if (c[0] == 'v' && c[1] == 'o'
                 && c[2] == 'l' && c[3] == 'a'
                 && c[4] == 't' && c[5] == 'i'
                 && c[6] == 'l' && c[7] == 'e')
          return RESERVED;
      }
#endif
    }
    break;

    case 9: {
#ifdef AQ_CHECK_RESERVED
      if (check_reserved) {
        if (c[0] == 'i' && c[1] == 'n'
            && c[2] == 't' && c[3] == 'e'
            && c[4] == 'r' && c[5] == 'f'
            && c[6] == 'a' && c[7] == 'c'
            && c[8] == 'e')
          return RESERVED;
        else if (c[0] == 't' && c[1] == 'r'
                 && c[2] == 'a' && c[3] == 'n'
                 && c[4] == 's' && c[5] == 'i'
                 && c[6] == 'e' && c[7] == 'n'
                 && c[8] == 't')
          return RESERVED;
        else if (c[0] == 'p' && c[1] == 'r'
                 && c[2] == 'o' && c[3] == 't'
                 && c[4] == 'e' && c[5] == 'c'
                 && c[6] == 't' && c[7] == 'e'
                 && c[8] == 'd')
          return RESERVED;
      }
#endif
    }
    break;

    case 10: {
      if (c[0] == 'i' && c[1] == 'n'
          && c[2] == 's' && c[3] == 't'
          && c[4] == 'a' && c[5] == 'n'
          && c[6] == 'c' && c[7] == 'e'
          && c[8] == 'o' && c[9] == 'f')
        return INSTANCEOF;
#ifdef AQ_CHECK_RESERVED
      else if (check_reserved) {
        if (c[0] == 'i' && c[1] == 'm'
            && c[2] == 'p' && c[3] == 'l'
            && c[4] == 'e' && c[5] == 'm'
            && c[6] == 'e' && c[7] == 'n'
            && c[8] == 't' && c[9] == 's')
          return RESERVED;
      }
#endif
    }
    break;

    case 12: {
#ifdef AQ_CHECK_RESERVED
      if (check_reserved) {
        if (c[0] == 's' && c[1] == 'y'
            && c[2] == 'n' && c[3] == 'c'
            && c[4] == 'h' && c[5] == 'r'
            && c[6] == 'o' && c[7] == 'n'
            && c[8] == 'i' && c[9] == 'z'
            && c[10] == 'e' && c[11] == 'd')
          return RESERVED;
      }
#endif
    }
    break;

  }

  return -1;
}

QSLexer::QSLexer()
  : yylineno(1),
    size8(32), size16(32768), restrKeyword(false),
    stackToken(-1), pos(0),
    code(0), length(0),
    bol(true),
    current(0), next1(0), next2(0), next3(0),
    localeC(QLocale::C),
    parenthesesState(IgnoreParentheses),
    parenthesesCount(0),
    prohibitAutomaticSemicolon(false)
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

  ustrHash.resize(401);
  ustrHash.setAutoDelete(true);
  ustrList.setAutoDelete(true);
}

QSLexer::~QSLexer()
{
  delete [] buffer8;
  delete [] buffer16;

  clearUstr();

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
  bol = true;

  // read first characters
  current = (length > 0) ? code[0].unicode() : 0;
  --length;
  next1 = (length > 0) ? code[1].unicode() : 0;
  --length;
  next2 = (length > 0) ? code[2].unicode() : 0;
  --length;
  next3 = (length > 0) ? code[3].unicode() : 0;
  --length;
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
          syncProhibitAutomaticSemicolon();
          if (!terminator && !delimited && !prohibitAutomaticSemicolon) {
            // automatic semicolon insertion if program incomplete
            token = ';';
            stackToken = 0;
            setDone(Other);
          } else {
            setDone(Eof);
          }
        } else if (isLineTerminator()) {
          shiftWindowsLineBreak();
          yylineno++;
          bol = true;
          terminator = true;
          syncProhibitAutomaticSemicolon();
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
          quickMatchPunctuator(current, next1, next2, next3);
          if (token != -1) {
            setDone(Other);
          } else {
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
          bol = true;

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
          record8(current)
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
  } else if (state == Hex) { // scan hex numbers
    dval = aqIntegerFromString(buffer8, pos8, 16);
    state = Number;
  } else if (state == Octal) { // scan octal number
    dval = aqIntegerFromString(buffer8, pos8, 8);
    state = Number;
  }

  restrKeyword = false;
  delimited = false;

  switch (parenthesesState) {
    case IgnoreParentheses:
      break;
    case CountParentheses:
      if (token == ')') {
        --parenthesesCount;
        if (parenthesesCount == 0)
          parenthesesState = BalancedParentheses;
      } else if (token == '(') {
        ++parenthesesCount;
      }
      break;
    case BalancedParentheses:
      parenthesesState = IgnoreParentheses;
      break;
  }

  switch (state) {
    case Eof:
      return 0;
    case Other:
      if (token == '}' || token == ';')
        delimited = true;
      return token;
    case Identifier:
      if ((token = findReservedWord(buffer16, pos16)) < 0) {
        /* TODO: close leak on parse error. same holds true for String */
        qsyylval.ustr = newUstr(buffer16, pos16);
        return IDENT;
      }
      if (token == CONTINUE || token == BREAK ||
          token == RETURN || token == THROW) {
        restrKeyword = true;
      } else if (token == IF || token == FOR
                 || token == WHILE || token == WITH) {
        parenthesesState = CountParentheses;
        parenthesesCount = 0;
      } else if (token == DO) {
        parenthesesState = BalancedParentheses;
      }
      return token;
    case KeywordNumber:
      token = 0;
      for (int i = 0; i < pos8; ++i)
        token = (token * 10) + (buffer8[i] - '0');
      if (token == CONTINUE || token == BREAK ||
          token == RETURN || token == THROW) {
        restrKeyword = true;
      } else if (token == IF || token == FOR
                 || token == WHILE || token == WITH) {
        parenthesesState = CountParentheses;
        parenthesesCount = 0;
      } else if (token == DO) {
        parenthesesState = BalancedParentheses;
      }
      return token;
    case String:
      qsyylval.ustr = newUstr(buffer16, pos16);
      return STRING;
    case Number:
      qsyylval.dval = dval;
      return NUMBER;
    case Bad:
      clearUstr();
      return -1;
    default:
      clearUstr();
      Q_ASSERT(!"unhandled numeration value in switch");
      return -1;
  }
}

bool QSLexer::scanRegExp()
{
  pos16 = 0;
  bool lastWasEscape = FALSE;

  while (1) {
    if (isLineTerminator() || current == 0)
      return FALSE;
    else if (current != '/' || lastWasEscape == TRUE) {
      record16(current);
      lastWasEscape = !lastWasEscape && (current == '\\');
    } else {
      pattern = QString(buffer16, pos16);
      pos16 = 0;
      shift(1);
      break;
    }
    shift(1);
  }

  while (isIdentLetter(current)) {
    record16(current);
    shift(1);
  }
  flags = QString(buffer16, pos16);

  return TRUE;
}
