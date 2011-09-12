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
              mimecodec.h - utilities for encoding and decoding
                             -------------------

  copyright : (C) by theKompany.com     <www.thekompany.com>
              (C) by Atol C&D           <www.atolcd.com>

  authors :   Eugen C.      <eug@thekompany.com>
              Fabien Armand <f.armand@atolcd.com>

 ***************************************************************************/

#ifndef __MIMECodec__
#define __MIMECodec__

#include <qcstring.h>

namespace MIMECodec
{
    /** wrap a text without CR-LF chars on the 76 character boundary.*/
    FL_EXPORT QCString canonicalForm(const QCString &);

    /** translate a MIME-encoded text.*/
    FL_EXPORT QString translate(const QCString &);

    /** MIME-fold text (used for headers).*/
    FL_EXPORT QCString fold(const QCString &);

    /** unfold MIME-folded text.*/
    FL_EXPORT QCString unfold(const QCString &);

    /** encode UNIX text (cr-ended) into MIME text (crlf-ended).*/
    FL_EXPORT QCString encodeEndLine(const QCString &);

    /** decode MIME text (crlf-ended) into UNIX text (cr-ended).*/
    FL_EXPORT QCString decodeEndLine(const QCString &);

    /** encode text in quoted-printable format (the bool parameter forces extra MIME gateway compatibility).*/
    FL_EXPORT QCString encodeQuotedPrintable(const QCString &, bool =false, bool =false);

    /** decode quoted-printable encoded text.*/
    FL_EXPORT QCString decodeQuotedPrintable(const QCString &);

    /** encode binary data in base64 format.*/
    FL_EXPORT QByteArray encodeBase64(const QByteArray &, bool endNewLine=true, bool endAllNewLines=true);

    /** decode base64-encoded binary data.*/
    FL_EXPORT QByteArray decodeBase64(const QByteArray &);

    /** encode binary data using the given encoding name ("base64", "quoted-printable", "7bit", "8bit", "binary", "none", "quoted-printable-compat" <- this can be used only for encoding).*/
    FL_EXPORT QByteArray encode(const QByteArray &, const char *, bool =false);   // buffer, encoding

    /** decode binary data using encoding name.*/
    FL_EXPORT QByteArray decode(const QByteArray &, const char *); // encoded buffer, encoding

    /** encode the message id.
      * It has a unique number/string and the user name like parameter.
      */
    FL_EXPORT QString encodeMessageId(const QString &, const QString &);

    /** create a cram-md5-digest using username, password, challenge
      * from Rik Hemsley (rikkus) <rik@kde.org>
      */
    FL_EXPORT QByteArray CramMd5Digest(const QCString&,const QCString&,const QCString&);
};

#endif
