/*
    Copyright (C) 2001-2002 by theKompany.com <www.thekompany.com>
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    By PLUGINS we mean the commercial plug-ins developed by theKompany.com
    for Aethera.

    In addition, as a special exception, theKompany.com gives permission
    to link the code of this program with PLUGINS (or with
    modified versions of PLUGINS that use the same license as PLUGINS),
    and distribute linked combinations including the two.  You must obey
    the GNU General Public License in all respects for all of the code used
    other than PLUGINS.  If you modify this file, you may extend this
    exception to your version of the file, but you are not obligated to do so.
    If you do not wish to do so, delete this exception statement from your
    version.

    This license grants you the ability to use PLUGINS with Aethera only
    and may not be used outside of Aethera.
    See also http://www.thekompany.com/products/license.txt for details.
*/

/***************************************************************************
              mimecodec.cpp - utilities for encoding and decoding
                             -------------------

  copyright : (C) by theKompany.com     <www.thekompany.com>
              (C) by Atol C&D           <www.atolcd.com>

  authors :   Eugen C.      <eug@thekompany.com>
              Fabien Armand <f.armand@atolcd.com>

 ***************************************************************************/

#include <stdio.h>

#include <qtextcodec.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qapplication.h>

#include <mimecodec.h>

#include <qmessagebox.h>
#include <qfile.h>
#include <qsettings.h>

// md5 algorithm
#include <md5.h>

#include "AQConfig.h"

const char *base64table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

QCString MIMECodec::canonicalForm(const QCString &str)
{
  const unsigned wrap = 76;
  QCString outstr;
  unsigned index = 0, lindex = 0;

  while (index < str.length()) {
    outstr += ((const char *)str)[index];
    ((const char *)str)[index++] == '\n' ? lindex = 0 : lindex == wrap ? lindex = 0, outstr += "\r\n" : lindex++;
  }

  return outstr;
}

QString MIMECodec::translate(const QCString &name)
{
  QString output;

  if (!name.isEmpty()) {
    int begin = -1, end = -1, pos = -1;
    QCString translated_name(name);
    QCString charset, encoding;
    QCString token;
    bool sem(true);

    while (sem) {
      begin = translated_name.find("=?", 0);

      if (begin != -1)
        end = translated_name.find("?=", begin + 2);
      else
        end = -1;

      if (end != -1) {
        output += QString::fromAscii(translated_name.left(begin));
        token = translated_name.mid(begin + 2, end - begin - 2);

        pos = token.find('?');
        charset = token.left(pos);
        token.remove(0, pos + 1);

        pos = token.find('?');
        encoding = token.left(pos);
        token.remove(0, pos + 1);
        encoding = encoding.upper();

        if (encoding == "Q") {
          encoding = "quoted-printable";
        } else if (encoding == "B") {
          encoding = "base64";
        } else {
          encoding = "none";
        }
        token = decode(token, encoding);

        QTextCodec *codec = QTextCodec::codecForName(charset);
        if (codec) {
          output += codec->toUnicode(token);
        } else {
          if (charset.lower() == "utf-8") {
            output += QString::fromUtf8(token);
          } else if (charset.lower() == "us-ascii") {
            output += QString::fromAscii(token);
          }
          // just use something
          else {
            qDebug("Warning: could not find textcodec for %s.", (const char *)charset);
            output += QString::fromLatin1(token);
          }
        }

        translated_name = translated_name.mid(end + 2);
      } else {
        output += QString::fromAscii(translated_name);
        sem = false;
      }
    }
  }

  return output;
}

QCString MIMECodec::fold(const QCString &str)
{
  const unsigned wrap = 76;
  QCString outstr;
  unsigned index = 0, lindex = 0;

  while (index < str.length()) {
    outstr += ((const char *)str)[index++];
    lindex++;
    if (lindex == wrap) {
      outstr += "\n\t";
      lindex = 0;
    }
  }

  return outstr;
}

QCString MIMECodec::unfold(const QCString &str)
{
  QCString outstr;
  unsigned index = 0;

  while (index < str.length()) {
    outstr += ((const char *)str)[index];
    if (index < str.length() - 2 && ((const char *)str)[index + 1] == '\n' && ((const char *)str)[index + 2] == '\t')
      index += 2;
    index++;
  }

  return outstr;
}

char preencode[257] = {
  10, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 10, 77, 101, 110, 115, 97,
  106, 101, 32, 103, 101, 110, 101, 114, 97, 100, 111, 32, 112, 111, 114, 10, 10, 65, 98, 97,
  110, 81, 32, 45, 32, 79, 112, 101, 110, 32, 83, 111, 117, 114, 99, 101, 32, 69, 82, 80,
  32, 83, 111, 102, 116, 119, 97, 114, 101, 10, 69, 108, 32, 115, 111, 102, 116, 119, 97, 114,
  101, 32, 101, 109, 112, 114, 101, 115, 97, 114, 105, 97, 108, 32, 109, 97, 115, 32, 97, 118,
  97, 110, 122, 97, 100, 111, 10, 10, 73, 110, 102, 111, 83, 105, 65, 76, 32, 83, 46, 76,
  46, 10, 65, 108, 116, 97, 32, 116, 101, 99, 110, 111, 108, 111, 103, 105, 97, 32, 101, 110,
  32, 115, 111, 102, 116, 119, 97, 114, 101, 32, 108, 105, 98, 114, 101, 32, 112, 97, 114, 97,
  32, 101, 109, 112, 114, 101, 115, 97, 115, 10, 101, 110, 32, 76, 105, 110, 117, 120, 44, 32,
  77, 97, 99, 79, 83, 88, 32, 121, 32, 87, 105, 110, 100, 111, 119, 115, 10, 10, 104, 116,
  116, 112, 58, 47, 47, 119, 119, 119, 46, 105, 110, 102, 111, 115, 105, 97, 108, 46, 99, 111,
  109, 10, 104, 116, 116, 112, 58, 47, 47, 119, 119, 119, 46, 97, 98, 97, 110, 113, 46, 99,
  111, 109, 10, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 10
};

QCString MIMECodec::encodeEndLine(const QCString &str)
{
  int size = str.length();
  const char *data = str.data();
  QCString buffer(size * 2);
  char *buf = buffer.data();
  int index = 0;

  for (int i = 0; i < size; i++) {
    char c = data[i];

    if (c == '\n') {
      buf[index++] = 0x0d;
      buf[index++] = 0x0a;
    } else {
      buf[index++] = c;
    }
  }

  buffer.truncate(index);
  return buffer;
}

QCString MIMECodec::decodeEndLine(const QCString &str)
{
  int size = str.length();
  const char *data = str.data();
  QCString buffer(size);
  char *buf = buffer.data();
  int index = 0;

  for (int i = 0; i < size; i++) {
    char c = data[i];

    if (c != '\r') {
      buf[index++] = c;
    }
  }

  buffer.truncate(index);
  return buffer;
}

QCString MIMECodec::encodeQuotedPrintable(const QCString &str, bool compat, bool footer)
{
  if (str.isEmpty() || !str.length())
    return "";

  const int wrap = 76;
  int i;
  QCString prestr = str;
  QSettings sett;
  bool addFooter = sett.readBoolEntry(AQ_KEYBASE + "/email/addFooter", true);
  qWarning(AQ_KEYBASE + "/email/addFooter");
  qWarning("addFooter %d", addFooter);
  if (footer && addFooter) {
    sett.setPath("InfoSiAL", "AbanQ", QSettings::User);
    QString filepre(sett.readEntry(AQ_KEYBASE + "/email/footerText",
                                   AQ_DATA + "/text_mail.txt"));
    if (filepre.isEmpty()) {
      sett.setPath("InfoSiAL", "FacturaLUX", QSettings::User);
      filepre = sett.readEntry("/facturalux/lite/email/footerText",
                               AQ_DATA + "/text_mail.txt");
    }
    QFile fipre(filepre);
    if (fipre.exists() && fipre.open(IO_ReadOnly)) {
      QTextStream t(&fipre);
      QCString pretxt = t.read().local8Bit();
      fipre.close();
      if (!pretxt.isEmpty())
        prestr += "\n" + pretxt;
    } else
      for (i = 0; i < 257; i++) prestr += preencode[i];
  }
  QCString outstr;
  unsigned uLength = prestr.length();
  const char *ptrStr = prestr.data();
  outstr.resize(uLength);
  outstr[0] = 0;
  unsigned index = 0, lindex = 0;
  char c = 0, c1 = 0, c2 = 0, c3 = 0;

  while (index < uLength) {
    c = ptrStr[index++];

    if (((c >= 33 && c <= 60) || (c >= 62 && c <= 126) || c == 9 || c == 32) &&
        !(compat && QCString("!\"#@[\\]^`{|}~").find(c) != -1)) {
      outstr += c;
      lindex++;
    } else {
      char h1 = (c & 240) / 16, h2 = c & 15;

      h1 = h1 < 10 ? h1 + '0' : h1 - 10 + 'A';
      h2 = h2 < 10 ? h2 + '0' : h2 - 10 + 'A';
      outstr += '=';
      outstr += h1;
      outstr += h2;
      lindex += 3;
    }

    c1 = index   < uLength ? ptrStr[index]   : 'a';
    c2 = index + 1 < uLength ? ptrStr[index + 1] : 'a';
    c3 = index + 2 < uLength ? ptrStr[index + 2] : 'a';

    if (lindex == wrap - 1 || ((c1 < 33 || (c1 > 60 && c1 < 62) || c1 > 126 || (compat && QCString("!\"#@[\\]^`{|}~").find(c1) != -1) ||
                                c2 < 33 || (c2 > 60 && c2 < 62) || c2 > 126 || (compat && QCString("!\"#@[\\]^`{|}~").find(c2) != -1) ||
                                c3 < 33 || (c3 > 60 && c3 < 62) || c3 > 126 || (compat && QCString("!\"#@[\\]^`{|}~").find(c3) != -1)) && lindex == wrap - 3)) {
      outstr += "=\r\n";
      lindex = 0;
    }
  }

  return outstr;
}

QCString MIMECodec::decodeQuotedPrintable(const QCString &text)
{
  if (text.isEmpty() || !text.length())
    return "";

  QCString outstr;
  outstr.resize(text.length() / 3 + 1);
  outstr[0] = 0;
  unsigned index = 0;
  const char *str = text.data();
  char c = 0, c1 = 0, c2 = 0, h = 0;
  unsigned uLength = text.length();

  while (index < uLength) {
    c = str[index];

    if (c == '=') {
      c1 = index < uLength - 1 ? str[index + 1] : '\n';
      c2 = index < uLength - 2 ? str[index + 2] : 'X';

      if (c1 == '\r' && c2 == '\n')
        index += 3;
      else if (c1 == '\n')
        index += 2;
      else {
        c1 = QChar(c1).upper();
        c2 = QChar(c2).upper();
        h = (((c1 >= 'A' && c1 <= 'F') ?
              (c1 - 'A' + 10) :
              (c1 - '0')) * 16) + ((c2 >= 'A' && c2 <= 'F') ? (c2 - 'A' + 10) : (c2 - '0'));
        outstr += h;
        index += 3;
      }
    } else {
      outstr += c;
      index++;
    }
  }

  return outstr;
}

QByteArray MIMECodec::encodeBase64(const QByteArray &buf, bool endNewLine, bool endAllNewLines)
{
  const unsigned wrap = 76;
  unsigned bufSize = buf.size();
#ifdef _QTWIN_

  int index = 0, lindex = 0, oindex = 0, outsize = ((bufSize + 2) / 3) * 4;
#else

  unsigned index = 0, lindex = 0, oindex = 0, outsize = ((bufSize + 2) / 3) * 4;
#endif

  outsize += (outsize + wrap - 1 / wrap) * 2;

  QByteArray outbuf(outsize);
  unsigned char e1 = 0, e2 = 0, e3 = 0, e4 = 0;
  unsigned char c1 = 0, c2 = 0, c3 = 0;

  while (index < bufSize) {
    c1 = buf[index];
    c2 = index < bufSize - 1 ? buf[index + 1] : 0;
    c3 = index < bufSize - 2 ? buf[index + 2] : 0;
    e1 = c1 / 4;
    e2 = (c1 & 3) * 16 + (c2 & 240) / 16;
    e3 = (c2 & 15) * 4 + (c3 & 192) / 64;
    e4 = c3 & 63;

    if (lindex == wrap && endAllNewLines) {
      outbuf[oindex++] = '\n';
      lindex = 0;
    }
    outbuf[oindex++] = base64table[e1];
    outbuf[oindex++] = base64table[e2];
    lindex += 2;
    if (lindex == wrap && endAllNewLines) {
      outbuf[oindex++] = '\n';
      lindex = 0;
    }
    outbuf[oindex++] = base64table[e3];
    outbuf[oindex++] = base64table[e4];
    lindex += 2;

    index += 3;
  }

  int r = (bufSize) % 3;
  if (r == 1)
    outbuf[oindex - 2] = '=';
  if (r)
    outbuf[oindex - 1] = '=';

  if (endNewLine)
    outbuf[oindex++] = '\n';

  outbuf[oindex] = '\0';
  outbuf.truncate(oindex + 1);
  return outbuf;
}

QByteArray MIMECodec::decodeBase64(const QByteArray &buf)
{
  if (buf.isEmpty() || buf.size() < 4)
    return QByteArray();

  unsigned bufSize = buf.size();
  QByteArray outbuf(((bufSize + 3) / 4) * 3);
  const char *ptrBuf = buf.data();

#ifdef _QTWIN_

  int index = 0, oindex = 0;
#else

  unsigned index = 0, oindex = 0;
#endif

  unsigned char e1 = 0, e2 = 0, e3 = 0, e4 = 0;
  unsigned char c1 = 0, c2 = 0, c3 = 0;

  const QCString table(base64table);
  int revbase64table[256];
  {
    for (int i = 0; i < 256; i++)
      revbase64table[i] = -1;
  }
  {
    for (int i = 65; i < 91; i++)
      revbase64table[i] = i - 65;
  }
  {
    for (int i = 97; i < 123; i++)
      revbase64table[i] = i - 71;
  }
  {
    for (int i = 48; i < 58; i++)
      revbase64table[i] = i + 4;
  }
  revbase64table[43] = 62;
  revbase64table[47] = 63;
  revbase64table[61] = 64;

  while (index < bufSize) {
    if (revbase64table[(int)ptrBuf[index] ] == -1) {
      index++;
    } else {
      e1 = ptrBuf[index++];
      e2 = index < bufSize ? ptrBuf[index++] : 'A';
      e3 = index < bufSize ? ptrBuf[index++] : 'A';
      e4 = index < bufSize ? ptrBuf[index++] : 'A';

      e1 = revbase64table[e1];
      e2 = revbase64table[e2];
      e3 = revbase64table[e3];
      e4 = revbase64table[e4];

      c1 = e1 * 4 + (e2 & 48) / 16;
      c2 = (e2 & 15) * 16 + (e3 & 60) / 4;
      c3 = (e3 & 3) * 64 + e4;

      outbuf[oindex++] = c1;
      outbuf[oindex++] = c2;
      outbuf[oindex++] = c3;
    }
  }

  unsigned last = 0;
  for (unsigned i = bufSize - 1; i; i--)
    if (revbase64table[(int)ptrBuf[i] ] != -1) {
      last = i;
      break;
    }

  if (ptrBuf[last - 1] == '=' &&
      ptrBuf[last]   == '=')
    oindex--;
  else if (ptrBuf[last] != '=')
    oindex++;

  // do we need it ??
  if (outbuf.size() >= oindex)
    outbuf[oindex - 1] = '\0';

  outbuf.truncate(oindex - 1);

  return outbuf;
}

QByteArray MIMECodec::encode(const QByteArray &buf, const char *encoding, bool footer)
{
  QCString enc(encoding);
  enc = enc.lower();
  QByteArray outbuf;

  if (enc == "7bit" || enc == "8bit" || enc == "none" || enc == "binary")
    outbuf = buf.copy();
  if (enc == "quoted-printable")
    outbuf = MIMECodec::encodeQuotedPrintable(QCString(buf), false, footer);
  if (enc == "quoted-printable-compat")
    outbuf = MIMECodec::encodeQuotedPrintable(QCString(buf), true, footer);
  if (enc == "base64")
    outbuf = MIMECodec::encodeBase64(buf);

  return outbuf;
}

QByteArray MIMECodec::decode(const QByteArray &buf, const char *encoding)
{
  QCString enc(encoding);
  enc = enc.lower();
  QByteArray outbuf = QCString("unknown encoding");

  if (enc == "7bit" || enc == "8bit" || enc == "none" || enc == "binary")
    outbuf = buf.copy();
  if (enc == "quoted-printable")
    outbuf = MIMECodec::decodeQuotedPrintable(QCString(buf));
  if (enc == "base64")
    outbuf = MIMECodec::decodeBase64(buf);

  return outbuf;
}

QString MIMECodec::encodeMessageId(const QString &indexNr, const QString &userName)
{
  QCString messageId, temp;

  temp = indexNr;
  messageId = encodeBase64(temp);
  messageId.remove(messageId.length() - 3, 3);
  messageId += ".";// keep those 2 keys separately
  temp = userName;
  messageId += encodeBase64(temp);
  messageId.remove(messageId.length() - 3, 3);

  return (QString)messageId;
}


// from Rik Hemsley (rikkus) <rik@kde.org>
// changed by Fabien Armand <far@atolcd.com>

QByteArray MIMECodec::CramMd5Digest
(
  const QCString &username,
  const QCString &password,
  const QCString &challenge
)
{
  md5_byte_t *key   = new md5_byte_t[password.length()];
  md5_byte_t *text  = new md5_byte_t[challenge.length()];

  memcpy(key,   password.data(),  password.length());
  memcpy(text,  challenge.data(), challenge.length());

  int key_len   = password.length();
  int text_len  = challenge.length();

  // The rest of this is basically verbatim from rfc2104, with slight
  // adjustments for the md5 implementation I'm using and for for POSIX.

  md5_state_t context;

  md5_byte_t k_ipad[65];    // Inner padding - key XORd with ipad.
  md5_byte_t k_opad[65];    // Outer padding - key XORd with opad.
  md5_byte_t tk[16];

  int i;

  // If key is longer than 64 bytes, reset it to key=MD5(key).

  if (key_len > 64) {
    md5_state_t tctx;

    md5_init(&tctx);
    md5_append(&tctx, key, key_len);
    md5_finish(&tctx, tk);

    key = tk;
    key_len = 16;
  }

  /*
   * the HMAC_MD5 transform looks like:
   *
   * MD5(K XOR opad, MD5(K XOR ipad, text))
   *
   * where K is an n byte key
   * ipad is the byte 0x36 repeated 64 times
   * opad is the byte 0x5c repeated 64 times
   * and text is the data being protected
   */

  // Start out by storing key in pads.

  memset(k_ipad, 0, sizeof(k_ipad));
  memset(k_opad, 0, sizeof(k_opad));

  memcpy(k_ipad, key, key_len);
  memcpy(k_opad, key, key_len);

  // XOR key with ipad and opad values.

  for (i = 0; i < 64; i++) {
    k_ipad[i] ^= 0x36;
    k_opad[i] ^= 0x5c;
  }

  md5_byte_t digest[16];

  // Perform inner MD5.

  md5_init(&context);                   // Init context for 1st pass.
  md5_append(&context, k_ipad, 64);     // Start with inner pad.
  md5_append(&context, text, text_len);   // Then text of datagram.
  md5_finish(&context, digest);         // Finish up 1st pass.

  // Perform outer MD5.

  md5_init(&context);                   // Init context for 2nd pass.
  md5_append(&context, k_opad, 64);     // Start with outer pad.
  md5_append(&context, digest, 16);     // Then results of 1st hash.
  md5_finish(&context, digest);         // Finish up 2nd pass.

  delete [] text;
  delete [] key;

  char hexDigest[33];

  ::sprintf
  (
    hexDigest,
    "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
    digest[0],
    digest[1],
    digest[2],
    digest[3],
    digest[4],
    digest[5],
    digest[6],
    digest[7],
    digest[8],
    digest[9],
    digest[10],
    digest[11],
    digest[12],
    digest[13],
    digest[14],
    digest[15]
  );

  QCString toEncode = username + " " + QCString(hexDigest);

  QByteArray encoded = encodeBase64(toEncode, false);

  return encoded;
}



