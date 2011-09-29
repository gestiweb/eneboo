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

#include <stdio.h>

#include <qtextstream.h>
#include <qcstring.h>
#include <qstringlist.h>
#include <qstrlist.h>
#include <qvaluelist.h>

#include <messageclass.h>
#include <addressclass.h>
#include <addresslistclass.h>
#include <dateclass.h>
#include <mimepart.h>
#include <headerclass.h>
#include <mimecodec.h>

MessageClass::MessageClass(const QCString & c)
{
    if ( c.isEmpty() )
        return;

    int iLength = c.length();
    QCString header( getPartHeader(c, 0, iLength) );

    // process the main header
    parseHeader(header);

    processPart(c, 0, iLength);
}

MessageClass::MessageClass(const HeaderClass &h)
        : Content_Type("text/plain"),
        Content_Transfer_Encoding("8bit")
{
    importHeader(h);
}

MessageClass::~MessageClass()
{
    partList.setAutoDelete(true);
    partList.clear();
}

void MessageClass::parseHeader(const QCString &h)
{
    QCString field, value, hline, sender, resent, xpriority, inrep, ref;
    QTextStream txt(h, IO_ReadOnly);

    while (!txt.atEnd()) {
        hline=txt.readLine();
        int pos=hline.find(":");
        if (pos!=-1) {
            field=hline.mid(0, pos).lower();
            value=hline.mid(pos+2);
            // this shouldn't be necessary, since more than one space is a protocol violation
            stripSpaces(value);
            stripComments(value);

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

    // strip "<>" from Message_ID
    stripParanthesis(Message_ID);

    // process In_Reply_To and References
    QStrList tlist;

    if (!inrep.isEmpty()) {
        chunk(tlist, inrep);
        if (tlist.count())
            In_Reply_To=tlist.at(0);
    }

    if (!ref.isEmpty()) {
        chunk(tlist, ref);
        for (unsigned int i=0;i<tlist.count();i++)
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
    split(tokens, ';', Content_Type);

    if (tokens.count())
        Content_Type=tokens.at(0);

    // a digest message is something really special, that's why we test it here
    if (Content_Type.lower().contains("multipart/digest"))
        digest=true;
    else
        digest=false;

    // check some redundant values
    if ( !( From.email || From.getTranslated_name() ) && !sender.isEmpty())
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
        Priority = Priority.lower();
        if ( Priority!="normal" &&
                Priority!="high" && Priority!="highest" &&
                Priority!="lowest" && Priority!="low" )
            Priority="normal";

        QCString k( Priority[0] );
        k=k.upper();
        Priority.replace(0, 1, k);
    }

    // give some default values for the very important fields
    QCString def="unknown", defd="Mon, 1 Jan 1999 0:0:0 UTF";
    if ( From.getOriginalAddress().isEmpty() )
        From = AddressClass(def);
    if ( To.toQCString().isEmpty() )
        To = AddressListClass(def);
    if ( Subject.isEmpty() )
        Subject = "(no subject)";
    if ( !Date.day_of_week)
        Date=DateClass(defd);
    if ( Content_Type.isEmpty() )
        Content_Type="text/plain"; // this is needed, for MIME-1.0 compliance
    if ( Content_Transfer_Encoding.isEmpty() )
        Content_Transfer_Encoding="8bit"; // so is this
}

void MessageClass::stripQuotes(QCString &s)
{
    if ( !s.isEmpty() ) {
        if (s.data()[0]=='"')
            s.remove(0, 1);

        if (s.data()[s.length()-1]=='"')
            s.truncate(s.length()-1);
    }
}

void MessageClass::stripParanthesis(QCString &s)
{
    if ( !s.isEmpty() ) {
        if (s.data()[0]=='<')
            s.remove(0, 1);
        if (s.data()[s.length()-1]=='>')
            s.truncate(s.length()-1);
    }
}

void MessageClass::stripSpaces(QCString &s)
{
    s = s.simplifyWhiteSpace();
}

void MessageClass::stripComments(QCString &s)
{
    int nesting=0;
    bool qnesting=false;
    char * ptrS = s.data();
    unsigned int lengthS = s.length();
    QCString t(lengthS);

    char c = 0;
    for (unsigned int i=0; i<lengthS; i++) {
        c = ptrS[i];

        if (c=='"' && !nesting)
            qnesting=!qnesting;
        if (c=='(' && !qnesting)
            nesting++;
        if (nesting==0 || qnesting)
            t+=c;
        if (c==')' && nesting && !qnesting)
            nesting--;
    }

    s = t.left(t.length());
}

void MessageClass::split(QStrList & t, char c, const QCString &s)
{
    QCString token;
    unsigned uLength = s.length();
    char * ptrS = s.data();

    t.clear();

    for (unsigned int i=0; i<uLength; i++) {
        if (ptrS[i] == c) {
            stripSpaces(token);
            t.append(token);
            token.truncate(0);
        } else
            token += ptrS[i];
    }
    if (!token.isEmpty()) {
        stripSpaces(token);
        t.append(token);
    }
}

void MessageClass::chunk(QStrList &t, const QCString &c)
{
    QCString token;
    unsigned uLength = c.length();
    char cTemp;
    char * ptrS = c.data();

    t.clear();

    for (unsigned i=0; i<uLength; i++) {
        cTemp = ptrS[i];

        if (cTemp=='>' && !token.isEmpty()) {
            t.append(token);
            token.truncate(0);
        } else if (cTemp == '<')
            token.truncate(0);
        else
            token += cTemp;
    }
}

QCString MessageClass::getPartHeader(const QCString &s, int offset, int length)
{
    // isolate and unfold the header
    QTextIStream txt(s);
    txt.device()->at(offset);

    QCString header = (const char *)txt.readLine();
    QCString hline = header;

    // this should add more speed
    header.resize(2048);

    while
    (
        !hline.isEmpty()
        && !txt.atEnd()
        && int(txt.device()->at()) < offset+length
    ) {
        hline = (const char *)txt.readLine();
        if (!hline.isEmpty()) {
            if (!(QChar(hline.data()[0]).isSpace()))
                header.append("\n");
            else
                header.append(" ");

            header.append(hline);
        }
    }

    return header;
}

void MessageClass::processPart(const QCString &s, int offset, int length, bool related)
{
    QCString header = getPartHeader(s, offset, length);

    // some of the default values are required for MIME 1.0 compliance
    QCString field, value, encoding("8bit");
    QCString boundary, protocol, name("attachment.dat");
    QCString cid("unknown"), hline, charset("us-ascii");
    QCString type = digest ? "message/rfc822" : "text/plain";
    QCString disposition;

    QTextIStream txt(header);

    // get the part header fields
    int pos = 0;
    while (!txt.atEnd()) {
        hline = txt.readLine();

        pos=hline.find(':');
        if (pos!=-1) {
            field=hline.left(pos).lower();
            value=hline.mid(pos+2);
            // this shouldn't be necessary, since more than one space
            // is a protocol violation - but who knows..
            stripSpaces(value);
            stripComments(value);

            // check for known fields
            if (field=="content-type")
                type=value;
            else if (field=="content-transfer-encoding")
                encoding=value;
            else if (field=="content-id")
                cid=value;
            else if (field=="content-disposition")
                disposition=value;
        }
    }

    // additional parsing for cid, boudary, charset or attachment name

    QStrList tokens;
    split(tokens, ';', type);

    if (tokens.count())
        type=tokens.at(0);
    else
        type="text/plain";

    // search for additional part attributes
    QCString spfield;
    QCString compare;
    for (unsigned int i=0;i<tokens.count();i++) {
        spfield = tokens.at(i);
        compare = spfield.left(9).lower();

        if (compare.left(8) == "charset=") {
            charset=spfield.mid(8);
            stripSpaces(charset);
            stripQuotes(charset);
        } else if (compare.left(9) == "boundary=") {
            boundary=spfield.mid(9);
            stripQuotes(boundary);
        } else if (compare.left(9) == "protocol=") {
            protocol=spfield.mid(9);
            stripQuotes(protocol);
        } else if (!disposition.isEmpty() &&
                   compare.left(5) == "name=") {
            name=spfield.mid(5);
            stripQuotes(name);
        }
    }

    split(tokens, ';', cid);
    if (tokens.count()) {
        cid=tokens.at(0);
        stripParanthesis(cid);
    }

    // get the part offset
    QTextIStream parttxt(s);
    parttxt.device()->at(offset);
    int pOffset;
    QCString pLine=(const char *)parttxt.readLine();
    while (!pLine.isEmpty()) {
        pLine=(const char *)parttxt.readLine();
    }
    pOffset=parttxt.device()->at();

    // get the sub-part list, scanning for the boundary;
    QValueList<int> partOffset, partLength;

    // check the boundary
    if (!boundary.isEmpty()) {
        boundary.prepend("--");
        QCString end_boundary = (const char*)boundary;
        end_boundary.append("--");

        int iBoundaryLength = boundary.length();
        int iEndBoundaryLength = end_boundary.length();
        int iBegin = s.find(boundary, offset + header.length());
        int iEnd = 0;

        while ( iBegin != -1 ) {
            iEnd = s.find(boundary, iBegin + iBoundaryLength);

            if ( iEnd != -1 &&
                    iEnd <= offset + length - iEndBoundaryLength ) {
                // found a valid part
                // Please don't forget the new line char to the end !
                iBegin = s.find('\n', iBegin) + 1;
                partOffset.append(iBegin);
                partLength.append(iEnd - iBegin - 1);
                iBegin = iEnd;

                if ( s.mid(iEnd, iEndBoundaryLength) == end_boundary )
                    iBegin = -1; // we found the end
            } else {
                qDebug("If we got on this branch there is either a mangled mail or a parser failure");
#ifdef AETHERA_DEBUG

                qDebug("%*.*s", s.length(), s.length(), (const char *)s);
#endif

                iBegin = -1;
            }
        }
    }

    if (partOffset.count()>0) {
        // check for the special case "multipart/related"
        processPart(s, partOffset[0], partLength[0]);

        if (type.lower().contains("multipart/related")) {
            for (unsigned int i=1;i<partOffset.count();i++)
                processPart(s, partOffset[i], partLength[i], true); // these are all related parts
        } else
            for (unsigned int i=1;i<partOffset.count();i++)
                processPart(s, partOffset[i], partLength[i]);
    } else {
        // either one simple part, or there was no boundary (broken mail), so dump it in one part
        QCString partOffset=(const char *)QString::number(pOffset);
        QCString partLength=(const char *)QString::number(length+offset-pOffset);
        QCString partType("unknown");

        type=type.lower();
        stripSpaces(type);

        if (digest) {
            if (type=="message/rfc822") {
                partType = "digest";
            }
        } else {
            if (related) {
                partType = "related";
            } else {
                // If it is an attachment
                // "inline" means it must be in the view part (not like attachment)
                // TODO: view all inline parts (not only the text parts)
                if (  disposition.lower().contains("attachment") ||
                        type.contains("octet-stream") ||
                        ( disposition.lower().contains("inline") && !type.contains("text/") ) ) {
                    partType = "attachment";
                } else if ( type.contains("text/") ) {
                    if ( type.contains("text/plain") ) {
                        partType = "text";
                        type = "text/plain";
                    } else if (type.contains("text/html")) {
                        partType = "text";
                        type = "text/html";
                    } else if ( type.contains("calendar") ) {
                        if ( name.isEmpty() || "attachment.dat" == name )
                            name = "calendar.ics";
                        partType = "attachment";
                    } else if ( type.contains("card") ) {
                        if ( name.isEmpty() || "attachment.dat" == name )
                            name = "card.vcf";
                        partType = "attachment";
                    } else {
                        partType = "text";
                    }
                } else {
                    if ( type.contains("application/pgp-signature") ||
                            ( type.contains("multipart/signed") &&
                              protocol.contains("application/pgp-signature") )
                       ) {
                        //TODO: Add support for PGP/GPG.
                        if ( name.isEmpty() || "attachment.dat" == name ) {
                            name = "signature.pgp";
                        }
                    } else {
                        //            qDebug("aethera: message with unknown type!");
                        //            qDebug("Type: %s",(const char*)type);
                    }

                    partType = "attachment";
                }
            }
        }

        if ( "unknown" != partType ) {
            // V VERSION TYPE MIME ENCODING OFFSET LENGTH CHARSET CID NAME
            QCString p("V ");
            p.append( PART_VERSION_TXT );
            p.append(" ");
            p.append(partType);
            p.append(" ");
            p.append(type);
            p.append(" ");
            p.append(encoding);
            p.append(" ");
            p.append(partOffset);
            p.append(" ");
            p.append(partLength);
            p.append(" ");
            p.append(charset);
            p.append(" ");
            p.append(cid);
            p.append(" ");
            QCString p_name = QCString( MIMECodec::encodeBase64(name) );
            p.append(p_name);
            partList.append(new MimePart(p));
        } else {
            qDebug("MessageClass error: unknown part.");
        }
    }
}

void MessageClass::importHeader(const HeaderClass &h)
{
    From=h.From;
    Reply_To=h.Reply_To;
    To=h.To;
    Cc=h.Cc;
    Bcc=h.Bcc;
    Subject=h.Subject;
    Message_ID=h.Message_ID;
    In_Reply_To=h.In_Reply_To;
    Organization=h.Organization;
    Status=h.Status;
    Priority=h.Priority;
    Received=h.Received;
    References=h.References;
    Date=h.Date;
    Content_Type=h.Content_Type;
    Content_Transfer_Encoding=h.Content_Transfer_Encoding;
}

HeaderClass MessageClass::header()
{
    HeaderClass h;
    h.From=From;
    h.Reply_To=Reply_To;
    h.To=To;
    h.Cc=Cc;
    h.Bcc=Bcc;
    h.Subject=Subject;
    h.Message_ID=Message_ID;
    h.In_Reply_To=In_Reply_To;
    h.Organization=Organization;
    h.Status=Status;
    h.Priority=Priority;
    h.Received=Received;
    h.References=References;
    h.Date=Date;
    h.Content_Type=Content_Type;
    h.Content_Transfer_Encoding=Content_Transfer_Encoding;
    return h;
}
