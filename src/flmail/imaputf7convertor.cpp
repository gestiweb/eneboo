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
// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
// Copyright (C) 2002 Rik Hemsley (rikkus) <rik@kde.org>

// IMAP UTF7 codec adapted (read: nicked) from RFC 2192 example C code.

// changed by Fabien Armand for the Aethera project <far@atolcd.com>

#include <qregexp.h>
#include <defines.h>
#include <imaputf7convertor.h>


const char * const base64chars =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+,";
     
    
enum
  {
    UNDEFINED       = 64,
    MAXLINE         = 76,
    UTF16MASK       = 0x03FFUL,
    UTF16SHIFT      = 10,
    UTF16BASE       = 0x10000UL,
    UTF16HIGHSTART  = 0xD800UL,
    UTF16HIGHEND    = 0xDBFFUL,
    UTF16LOSTART    = 0xDC00UL,
    UTF16LOEND      = 0xDFFFUL
  };


 
QString ImapUTF7Convertor::imapUTF7ToUnicode(const QString & input)
  {
    unsigned char c, i, bitcount;
    unsigned long ucs4, utf16, bitbuf;
    unsigned char base64[256],utf8[6];
    unsigned long srcPtr = 0;

    QCString output;
    QCString src = input.ascii();
        
    // Initialize modified base64 decoding table.

    memset(base64, UNDEFINED, sizeof(base64));
    
  // for (i = 0; i < sizeof(base64chars); ++i)
  for (i = 0; i < 64; ++i)
    {
      base64[(int)base64chars[i]] = i;  
    }
    
       
    // Loop until end of string.
    while (srcPtr < src.length())
    {
      c = src[(int)srcPtr++];

      // Deal with literal characters and "&-".

      if (c != '&' || src[(int)srcPtr] == '-')
      {
        // Encode literally.

        output += c;
                
        // Skip over the '-' if this is an "&-" sequence.

        if (c == '&')
          srcPtr++;
      }
      else
      {
        // Convert modified UTF-7 -> UTF-16 -> UCS-4 -> UTF-8 -> HEX.
        bitbuf = 0;
        bitcount = 0;
        ucs4 = 0;
       
        while ((c = base64[(unsigned char) src[(int)srcPtr]]) != UNDEFINED)
        {
          
          ++srcPtr;
          bitbuf = (bitbuf << 6) | c;
          bitcount += 6;

          // Enough bits for an UTF-16 character ?

          if (bitcount >= 16)
          {
            bitcount -= 16;
            utf16 = (bitcount ? bitbuf >> bitcount : bitbuf) & 0xffff;

            // Convert UTF16 to UCS4.

            if (utf16 >= UTF16HIGHSTART && utf16 <= UTF16HIGHEND)
            {
              ucs4 = (utf16 - UTF16HIGHSTART) << UTF16SHIFT;
              continue;
            }
            else if (utf16 >= UTF16LOSTART && utf16 <= UTF16LOEND)
            {
              ucs4 += utf16 - UTF16LOSTART + UTF16BASE;
            }
            else
            {
              ucs4 = utf16;
            }

            // Convert UTF-16 range of UCS4 to UTF-8.

            if (ucs4 <= 0x7fUL)
            {
              utf8[0] = ucs4;
              i = 1;
            }
            else if (ucs4 <= 0x7ffUL)
            {
              utf8[0] = 0xc0 | (ucs4 >> 6);
              utf8[1] = 0x80 | (ucs4 & 0x3f);
              i = 2;
            }
            else if (ucs4 <= 0xffffUL)
            {
              utf8[0] = 0xe0 | (ucs4 >> 12);
              utf8[1] = 0x80 | ((ucs4 >> 6) & 0x3f);
              utf8[2] = 0x80 | (ucs4 & 0x3f);
              i = 3;
            }
            else
            {
              utf8[0] = 0xf0 | (ucs4 >> 18);
              utf8[1] = 0x80 | ((ucs4 >> 12) & 0x3f);
              utf8[2] = 0x80 | ((ucs4 >> 6) & 0x3f);
              i = 4;
            }

            // Copy it.
            for (c = 0; c < i; ++c)
            {
              output += utf8[c];              
            }
          }
        }

        // Skip over trailing '-' in modified UTF-7 encoding.

        if (src[(int)srcPtr] == '-')
          ++srcPtr;
      }
    }

    return QString::fromUtf8(output.data());
  }

QString ImapUTF7Convertor::unicodeToIMAPUTF7(const QString & input)
  {
    unsigned int utf8pos, utf8total, c, utf7mode, bitstogo, utf16flag;
    unsigned long ucs4, bitbuf;

    QCString src = input.utf8();
    QString output;

    ulong srcPtr  = 0;
    utf7mode      = 0;
    utf8total     = 0;
    bitstogo      = 0;
    utf8pos       = 0;
    bitbuf        = 0;
    ucs4          = 0;

    while (srcPtr < src.length())
    {
      c = (unsigned char) src[(int)srcPtr++];

      // Normal character ?

      if (c >= ' ' && c <= '~')
      {
        // Switch out of UTF-7 mode.

        if (utf7mode)
        {
          if (bitstogo)
          {
            output += base64chars[(bitbuf << (6 - bitstogo)) & 0x3F];
            bitstogo = 0;
          }
          output += '-';
          utf7mode = 0;
        }
        output += c;

        // Encode '&' as '&-'.

        if (c == '&')
        {
          output += '-';
        }
        continue;
      }

      // Switch to UTF-7 mode.

      if (!utf7mode)
      {
        output += '&';
        utf7mode = 1;
      }

      // Encode US-ASCII characters as themselves.
      if (c < 0x80)
      {
        ucs4 = c;
        utf8total = 1;
      }
      else if (utf8total)
      {
        // Save UTF8 bits into UCS4.

        ucs4 = (ucs4 << 6) | (c & 0x3FUL);

        if (++utf8pos < utf8total)
        {
          continue;
        }
      }
      else
      {
        utf8pos = 1;

        if (c < 0xE0)
        {
          utf8total = 2;
          ucs4 = c & 0x1F;
        }
        else if (c < 0xF0)
        {
          utf8total = 3;
          ucs4 = c & 0x0F;
        }
        else
        {
          // XXX: Can't convert UTF8 sequences longer than 4.

          utf8total = 4;
          ucs4 = c & 0x03;
        }
        continue;
      }

      // Loop to split ucs4 into two utf16 chars if necessary.

      utf8total = 0;

      do
      {
        if (ucs4 >= UTF16BASE)
        {
          ucs4 -= UTF16BASE;
          bitbuf = (bitbuf << 16) | ((ucs4 >> UTF16SHIFT) + UTF16HIGHSTART);
          ucs4 = (ucs4 & UTF16MASK) + UTF16LOSTART;
          utf16flag = 1;
        }
        else
        {
          bitbuf = (bitbuf << 16) | ucs4;
          utf16flag = 0;
        }
        bitstogo += 16;

        // Spew out base64.

        while (bitstogo >= 6)
        {
          bitstogo -= 6;

          output +=
            base64chars[(bitstogo ? (bitbuf >> bitstogo) : bitbuf) & 0x3F];
        }
      }
      while (utf16flag);
    }

    // If in UTF-7 mode, finish in ASCII.

    if (utf7mode)
    {
      if (bitstogo)
      {
        output += base64chars[(bitbuf << (6 - bitstogo)) & 0x3F];
      }

      output += '-';
    }

    // Replace all " with \"

    output.replace(QRegExp("\""), "\\\"");

    // Replace all \ with \\.

    output.replace(QRegExp("\\"), "\\\\");

    return output;
  }

QString ImapUTF7Convertor::dateToString(const QDate & d)
  {
    return
      QString("%1-%2-%3")
      .arg(d.day())
      .arg(d.shortMonthName(d.month()))
      .arg(d.year())
      ;
  }

