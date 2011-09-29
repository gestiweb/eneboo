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

#include <headerclass.h>
#include <mimecodec.h>

#include <qtextstream.h>
#include <qstringlist.h>
#include <qvaluelist.h>

HeaderClass::HeaderClass(const char *c)
{
    if (!c)
        return;

    QCString msg(c), header, hline="a";

    // isolate and unfold the header
    QTextIStream htxt(msg);

    while (!hline.isEmpty() && !htxt.atEnd()) {
        hline=(const char *)htxt.readLine();
        if (!hline.isEmpty()) {
            if (!(QChar(hline[0]).isSpace()))
                header.append("\n");
            else
                header.append(" ");

            MessageClass::stripSpaces(hline);

            header.append(hline);
        }
    }

    // process the header
    QCString field, value, sender, resent, xpriority, inrep="", ref="";
    QTextStream txt(header, IO_ReadOnly);

    while (!txt.atEnd()) {
        hline=txt.readLine();
        int pos=hline.find(":");
        if (pos!=-1) {
            field=hline.mid(0, pos).lower();
            value=hline.mid(pos+2);
            MessageClass::stripSpaces(value); // this shouldn't be necessary, since more than one space is a protocol violation - but who knows..
            MessageClass::stripComments(value);

            switch ( field.data()[0] ) {
            case 'b':
                if (field=="bcc")
                    Bcc=AddressListClass(value);
                break;

            case 'c':
                if (field=="cc")
                    Cc=AddressListClass(value);
                else if (field=="content-type")
                    Content_Type=value;
                else if (field=="content-transfer-encoding")
                    Content_Transfer_Encoding=value;
                break;

            case 'd':
                if (field=="date")
                    Date=DateClass(value);
                break;

            case 'f':
                if (field=="from")
                    From=AddressClass(value);
                break;

            case 'i':
                if (field=="in-reply-to")
                    inrep=value;
                break;

            case 'm':
                if (field=="message-id")
                    Message_ID=value;
                break;

            case 'o':
                if (field=="organization")
                    Organization=value;
                break;

            case 'p':
                if (field=="priority")
                    Priority=value;
                break;

            case 'r':
                if (field=="received")
                    Received.append(value);
                else if (field=="reply-to")
                    Reply_To=AddressClass(value);
                else if (field=="references")
                    ref=value;
                else if (field=="resent-from")
                    resent=value;
                break;

            case 's':
                if (field=="subject")
                    Subject=value;
                else if (field=="status")
                    Status=value;
                else if (field=="sender")
                    sender=value;
                break;

            case 't':
                if (field=="to")
                    To=AddressListClass(value);
                break;

            case 'x':
                if (field=="x-priority" || field=="x-msmail-priority") {
                    xpriority=value;
                }
                break;

            default:
                break;
            }
        }
    }

    // MessageClass::strip "<>" from Message_ID
    MessageClass::stripParanthesis(Message_ID);

    // process In_Reply_To and References
    QStrList tlist;

    if (!inrep.isEmpty()) {
        MessageClass::chunk(tlist, inrep);
        if (tlist.count())
            In_Reply_To=tlist.at(0);
    }

    if (!ref.isEmpty()) {
        MessageClass::chunk(References, ref);
        for (unsigned i=1;i<tlist.count();i++)
            References.append(tlist.at(i));
    }
    if (!In_Reply_To.isEmpty() && References.count())
        while (References.remove((const char *)In_Reply_To))
            ;

    // assign sensible values to Status
    if (Status=="O" || Status=="R")
        Status="Read";
    else
        Status="New";

    // parse the "Content-Type" structured field
    QStrList tokens;
    MessageClass::split(tokens, ';', Content_Type);

    Content_Type=tokens.at(0);

    // check some redundant values
    if ( !( From.email || From.getTranslated_name() ) &&!sender.isEmpty())
        From=AddressClass(sender);
    if ( !( From.email || From.getTranslated_name() ) && !resent.isEmpty())
        From=AddressClass(resent);
    if (!Priority && !xpriority.isEmpty())
        Priority=xpriority;

    // check and beautify the priority value, as some mailers use numbers
    int pr=Priority.toInt();
    if (pr>0 && pr<6)
        switch (pr) {
        case 5:
            Priority="Lowest";
            break;
        case 4:
            Priority="Low";
            break;
        case 3:
            Priority="Normal";
            break;
        case 2:
            Priority="High";
            break;
        case 1:
            Priority="Highest";
        }
    else {
        Priority=Priority.lower();
        if ( Priority!="normal" &&
                Priority!="high" && Priority!="highest" &&
                Priority!="lowest" && Priority!="low" )
            Priority="normal";

        QString k=QChar(Priority[0]);
        k=k.upper();
        Priority.replace(0, 1, (const char *)k);
    }

    // give some default values for the very important fields
    QCString def="unknown", defd="Mon, 1 Jan 1999 0:0:0 UTF";
    if ( !( From.email || From.getTranslated_name() ) )
        From=AddressClass(def);
    if (!(QCString)To)
        To=AddressListClass(def);
    if (!Subject)
        Subject="Not specified";
    if (!Date.day_of_week)
        Date=DateClass(defd);
    if (!Content_Type)
        Content_Type="text/plain"; // this is needed, for MIME-1.0 compliance
    if (!Content_Transfer_Encoding)
        Content_Transfer_Encoding="8bit"; // so is this
}

HeaderClass::~HeaderClass() {}

void HeaderClass::importMessage(MessageClass &) {}

HeaderClass::HeaderClass(MessageClass &m)
{
    importMessage(m);
}





