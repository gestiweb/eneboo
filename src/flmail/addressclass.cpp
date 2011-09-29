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
/*
 * This file is part of Magellan <http://www.kAlliance.org/Magellan>
 *
 * Copyright (c) 1998-2000 Teodor Mihai <teddy@ireland.com>
 * Copyright (c) 1998-2000 Laur Ivan <laur.ivan@ul.ie>
 * Copyright (c) 1999-2000 Virgil Palanciuc <vv@ulise.cs.pub.ro>
 *
 * Requires the Qt widget libraries, available at no cost at
 * http://www.troll.no/
 *
 * Also requires the KDE libraries, available at no cost at
 * http://www.kde.org/
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <addressclass.h>
#include <mimecodec.h>
#include <charsets.h>

#include <qtextstream.h>
#include <qtextcodec.h>
#include <qregexp.h>
#include <qmessagebox.h>

AddressClass::AddressClass(const QCString &t)
{
    QCString temp = t;
    // Replace ", <, >, ',', ';' with space
    temp.replace( QRegExp("[\"<>,;]"), " ");
    temp = temp.simplifyWhiteSpace();

    QTextStream txt(temp, IO_ReadOnly);
    QCString token;
    txt>>token;

    // Ex: FirstName LastName <email@address.net>
    // Name = FirstName LastName
    // EMAIL = email@address.net
    // OBS: There aren't any words after the email part

    while (!token.isEmpty()) {
        // the email HAS to have '@'
        if ( -1 != token.find('@') ) {
            email = token;
            // nothing after the email part
            break;
        }

        // add a space between name parts
        if ( !name.isEmpty() )
            name += ' ';

        // add a new word to the name
        name.append(token);

        // next word
        txt>>token;
    }

    translateName();
}

AddressClass::AddressClass(const QString & str)
{
    QString temp = str;
    // Replace ", <, >, ',', ';' with space
    temp.replace( QRegExp("[\"<>,;]"), " ");
    temp = temp.simplifyWhiteSpace();

    QTextStream txt(temp, IO_ReadOnly);
    QString token;
    txt >> token;

    // Ex: FirstName LastName <email@address.net>
    // Name = FirstName LastName
    // EMAIL = email@address.net
    // OBS: There aren't any words after the email part

    while (!token.isEmpty()) {
        // the email HAS to have '@'
        if ( -1 != token.find('@') ) {
            email = token.utf8();
            // nothing after the email part
            break;
        }

        // add a space between name parts
        if ( !translated_name.isEmpty() )
            translated_name += ' ';

        // add a new word to the name
        translated_name.append(token);

        // next word
        txt >> token;
    }

    if ( !translated_name.isEmpty() ) {
        QTextCodec * codec = 0L;
        QCString ascii = translated_name.ascii();
        if ( translated_name == QString::fromAscii(ascii) &&
                CharSets::thisInstance()->isRFC822(ascii) ) {
            name = ascii;
        } else if ( translated_name != QString::fromUtf8(translated_name.utf8()) &&
                    (codec = CharSets::thisInstance()->getCodec(translated_name)) ) {
            name = QCString("=?") +
                   codec->mimeName() +
                   "?B?" +
                   MIMECodec::encodeBase64( codec->fromUnicode(translated_name), false, false) +
                   "?=";
        } else {
            name = QCString("=?utf-8?B?") +
                   MIMECodec::encodeBase64( translated_name.utf8(), false, false) +
                   "?=";
        }
    }
}

AddressClass::AddressClass() {}

AddressClass::AddressClass(const AddressClass & org)
{
    name = org.name;
    email = org.email;
    translated_name = org.translated_name;
}

AddressClass::~AddressClass() {}

bool AddressClass::operator==( const AddressClass& right ) const
{
    if ( name            == right.name &&
            email           == right.email &&
            translated_name == right.translated_name )
        return true;

    return false;
}

QCString AddressClass::getOriginalAddress() const
{
    QCString t;

    if ( !name.isEmpty() ) {
        int begin = 0, end = 0;
        begin = name.find("=?", 0);
        end = name.findRev("?=");
        if ( -1 != begin && -1 != end ) {
            t.append(name);
        } else {
            t.append("\"");
            t.append(name);
            t.append("\" ");
        }
        t.append( email.isEmpty() ? "" : " ");
    }

    if ( !email.isEmpty() ) {
        t.append("<");
        t.append(email);
        t.append(">");
    }

    return t;
}

void AddressClass::translateName()
{
    translated_name = MIMECodec::translate(name);
}

QTextStream& operator<<(QTextStream &s, const AddressClass &a)
{
    QCString t = a.getOriginalAddress();
    s << t;
    return s;
}

QString  AddressClass::getTranslated_name() const
{
    return translated_name;
}

void  AddressClass::setTranslated_name(const QString &set
                                      )
{
    translated_name = set
                      ;
}

QString AddressClass::fullAddress() const
{
    if ( !translated_name.isEmpty() ) {
        if ( email.isEmpty() ) {
            return QString("\"")+translated_name+QString("\"");
        } else {
            return QString("\"")+translated_name+QString("\" <")+QString(email)+QString(">");
        }
    } else
        return QString(email);
}

