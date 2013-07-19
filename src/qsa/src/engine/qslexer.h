/****************************************************************************
** $Id: qslexer.h  1.1.5   edited 2006-02-23T15:39:57$
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

#ifndef QSLEXER_H
#define QSLEXER_H

#include <qintdict.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qlocale.h>

class QSLexer
{
public:
  enum State {
    Start,
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
    Bad,
    KeywordNumber
  };

  enum ParenthesesState {
    IgnoreParentheses,
    CountParentheses,
    BalancedParentheses
  };

  QSLexer();
  ~QSLexer();

  static QSLexer *lexer() {
    return lx;
  }

  void setCode(const QString &c, int id, int lineno = 0);
  int lex();

  int lineNo() const {
    return yylineno + 1;
  }

  int sourceId() const {
    return sid;
  }

  bool prevTerminator() const {
    return terminator;
  }

  QString pattern, flags;

  bool scanRegExp();

  State lexerState() const {
    return state;
  }

  QString errorMessage() const {
    return errmsg;
  }
  void setErrorMessage(const QString &err) {
    errmsg = err;
  }
  void setErrorMessage(const char *err) {
    setErrorMessage(QString::fromLatin1(err));
  }

private:
  int yylineno;
  bool done;
  char *buffer8;
  QChar *buffer16;
  uint size8, size16;
  uint pos8, pos16;
  bool terminator;
  bool restrKeyword;
  // encountered delimiter like "'" and "}" on last run
  bool delimited;
  int stackToken;

  State state;
  void setDone(State s) {
    state = s;
    done = TRUE;
  }

  uint pos;

  bool isWhiteSpace() const {
    return (current == ' ' || current == '\t' ||
            current == 0x0b || current == 0x0c);
  }

  bool isLineTerminator() const {
    return (current == '\n' || current == '\r');
  }

  bool isHexDigit(ushort c) const {
    return ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'f') ||
            (c >= 'A' && c <= 'F'));
  }

  bool isOctalDigit(ushort c) const {
    return (c >= '0' && c <= '7');
  }

  ushort singleEscape(ushort c) const {
    switch (c) {
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

  ushort convertOctal(ushort c1, ushort c2,
                      ushort c3) const {
    return ((c1 - '0') * 64 + (c2 - '0') * 8 + c3 - '0');
  }

public:
  static uchar convertHex(ushort c) {
    if (c >= '0' && c <= '9')
      return static_cast<uchar>(c - '0');
    else if (c >= 'a' && c <= 'f')
      return static_cast<uchar>(c - 'a' + 10);
    else
      return static_cast<uchar>(c - 'A' + 10);
  }

  static uchar convertHex(ushort c1, ushort c2) {
    return ((convertHex(c1) << 4) + convertHex(c2));
  }

  static QChar convertUnicode(ushort c1, ushort c2,
                              ushort c3, ushort c4) {
    return QChar((convertHex(c3) << 4) + convertHex(c4),
                 (convertHex(c1) << 4) + convertHex(c2));
  }

  static bool isIdentLetter(ushort c) {
    /* TODO: allow other legitimate unicode chars */
    return ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c == '$' || c == '_'));
  }

  static bool isDecimalDigit(ushort c) {
    return (c >= '0' && c <= '9');
  }

  void clearUstr() {
    if (ustrHash.count() > 401)
      ustrHash.clear();
    ustrList.clear();
  }

private:

  static QSLexer *lx;
  static int sid;

  QIntDict<const QString> ustrHash;
  QPtrList<const QString> ustrList;

  uint hashUstr(QChar *s, uint len) {
    uint hash = 0;
    for (int i = 0; i < len; ++i) {
      hash += s[i].unicode();
      hash += (hash << 10);
      hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
  }

  const QString *newUstr(QChar *s, uint len) {
    const QString *us = 0;
    if (len < 50) {
      uint hash = hashUstr(s, len);
      us = ustrHash.find(hash);
      if (!us) {
        us = new QString(s, len);
        ustrHash.insert(hash, us);
      }
    } else {
      us = new QString(s, len);
      ustrList.append(us);
    }
    return us;
  }

  void syncProhibitAutomaticSemicolon() {
    if (parenthesesState == BalancedParentheses) {
      // we have seen something like "if (foo)", which means we should
      // never insert an automatic semicolon at this point, since it would
      // then be expanded into an empty statement (ECMA-262 7.9.1)
      prohibitAutomaticSemicolon = true;
      parenthesesState = IgnoreParentheses;
    } else {
      prohibitAutomaticSemicolon = false;
    }
  }

  const QChar *code;
  uint length;
  int yycolumn;
  int bol;

  // current and following unicode characters
  ushort current, next1, next2, next3;

  QString errmsg;
  QLocale localeC;

  ParenthesesState parenthesesState;
  int parenthesesCount;
  bool prohibitAutomaticSemicolon;
};

#endif
