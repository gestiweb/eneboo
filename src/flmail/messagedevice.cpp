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

// QT
#include <qdatastream.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qcstring.h>
#include <qtl.h>
#include <qtextcodec.h>

// Aethera
#include <addresslistclass.h>
#include <indexclass.h>
#include <localmailfolder.h>
#include <mimecodec.h>
//#include <texthtml.h>
#include <messagedevice.h>
#include <messagedescriptor.h>
#include <mimepart.h>
#include <charsets.h>

#include "AQConfig.h"

MessageDevice::MessageDevice(IndexClass * index)
        : _index(index)
{
    Q_ASSERT(0 != _index);
}

MessageDevice::~MessageDevice()
{
}

int MessageDevice::getPartCount()
{
    return _index->getPartCount();
}

int MessageDevice::createPart()
{
    // create part
    MimePart *_part = new MimePart();
    _index->setMultipartOnly(true);
    int partIndex=_index->addPart(_part, false);

    // save index
    _index->getParentFolder()->saveIndex(_index);

    //ServerNotifier::thisInstance()->objectChanged(_index->getParentFolder()->name()+"/"+_index->getID());

    return partIndex;
}

bool MessageDevice::deletePart(int partIndex)
{
    if ( _index->removePart(partIndex) ) {
        _index->setMultipartOnly(true);

        QByteArray data;
        saveMessage(-1, data);

        //ServerNotifier::thisInstance()->objectDeleted(_index->getParentFolder()->name()+"/"+_index->getID()+ATTR_SEP+QString::number(partIndex));
        //ServerNotifier::thisInstance()->objectChanged(_index->getParentFolder()->name()+"/"+_index->getID());

        return true;
    } else {
        return false;
    }
}

bool MessageDevice::getPartData(int partIndex, QByteArray &data)
{
    data.resize(0);
    if (partIndex<_index->getPartCount() && partIndex>=0) {
        MimePart *part=_index->getPartAt(partIndex);

        // debug
        // qDebug("messagedevice: reading part data (part %d, bytes %d, enc %s)", partIndex, part->length, (const char *)part->encoding);

        if (part->length>0) {
            QFile dataFile(_index->getDataFilename());
            data.resize(part->length+1);

            if (dataFile.open(IO_ReadOnly)) {
                dataFile.at(part->offset);
                dataFile.readBlock(data.data(), part->length);
                dataFile.close();

                data[(int)part->length]=0;
                data=MIMECodec::decode(data, part->encoding);

                return true;
            } else {
                data.resize(0);

                // debug
                qDebug("messagedevice: could not open file");

                return false;
            }
        } else {
            // debug
            qDebug("messagedevice: empty part");

            data.resize(0);
            return true;
        }
    } else {
        // debug
        qDebug("messagedevice: part out of scope (%d parts registered)", _index->getPartCount());

        data.resize(0);
        return false;
    }
}

bool MessageDevice::setPartData(int partIndex, const QByteArray &data)
{
    if ( partIndex < _index->getPartCount() &&
            partIndex >= 0 &&
            !data.isEmpty() ) {
        return saveMessage(partIndex, data);
    }

    return false;
}

MimePart& MessageDevice::operator[](int partIndex)
{
    return *(_index->getPartAt(partIndex));
}

void MessageDevice::loadDescriptor(bool fast)
{
    if ( _index->getDescriptorLength() ) {
        QFile dataFile(_index->getDescriptorFileName());
        if (dataFile.open(IO_ReadOnly)) {
            dataFile.at(_index->getDescriptorOffset());
            QDataStream stream(&dataFile);
            _descriptor.setCoreLoad(fast);
            stream>>_descriptor;
            _descriptor.setCoreLoad(false); // better safe than sorry
            dataFile.close();
        }
    }
}

MessageDescriptor & MessageDevice::getDescriptor()
{
    return _descriptor;
}

void MessageDevice::saveDescriptor()
{
    QFile dataFile(_index->getDescriptorFileName());
    if (dataFile.open(IO_WriteOnly|IO_Append)) {
        _index->setDescriptorOffset(dataFile.at());
        QDataStream stream(&dataFile);
        stream<<_descriptor;
        _index->setDescriptorLength(dataFile.at()-_index->getDescriptorOffset());
        dataFile.close();

        _index->getParentFolder()->saveIndex(_index);
    }
}

IndexClass* MessageDevice::index()
{
    return _index;
}


bool MessageDevice::hasAttachments()
{
    for (int i=0;i<_index->getPartCount();i++)
        if (_index->getPartAt(i)->type=="attachment")
            return true;
    return false;
}

bool MessageDevice::hasText()
{
    return textPart();
}

bool MessageDevice::hasHtml()
{
    return htmlPart();
}

bool MessageDevice::textPart(IVList *textPartList)
{
    bool bFound = false;

    for (int i=0;i<_index->getPartCount();i++) {
        MimePart *part=_index->getPartAt(i);

        if ( ( part->type=="text" || part->type=="attachment" ) &&
                part->mimetype=="text/plain" ) {
            // if it is a test or we realy need the parts list
            if ( textPartList != NULL )
                textPartList->append(i);
            bFound=true;
        }
    }

    return(bFound);
}

int MessageDevice::messageText()
{
    for (int i=0;i<_index->getPartCount();i++) {
        MimePart *part=_index->getPartAt(i);

        if ( part->type=="text" &&
                ( part->mimetype=="text/plain" || part->mimetype=="text/html") ) {
            return( i );
        }
    }
    return( -1 );
}

bool MessageDevice::htmlPart(IVList *htmlPartList)
{
    bool bFound = false;

    for (int i=0;i<_index->getPartCount();i++) {
        MimePart *part=_index->getPartAt(i);

        if ( ( part->type=="text" || part->type=="attachment" ) &&
                part->mimetype=="text/html" ) {
            // if it is a test or we realy need the parts list
            if ( htmlPartList != NULL )
                htmlPartList->append(i);
            bFound=true;
        }
    }

    return(bFound);
}

QString MessageDevice::text(bool bAllTextParts)
{
    QString txt;

    IVList textParts;

    if ( textPart(&textParts) ) {
        for (IVList::Iterator it=textParts.begin(); it!=textParts.end(); ++it) {
            QCString text_;
            MimePart *part=_index->getPartAt(*it);

            if ( bAllTextParts==true ||
                    part->type!="attachment" ) {
                getPartData( *it , text_);

                if ( part->type == "attachment" )
                    txt += "\n-------- Attachment ---------\n";

                QTextCodec * codec = QTextCodec::codecForName( part->charset );
                if (codec) {
                    txt += codec->toUnicode( text_ );
                } else {
                    if ( "utf-8" == part->charset.lower() ) {
                        txt += QString::fromUtf8( text_.data() );
                    } else if ( "8bit" == part->encoding.lower() ) {
                        txt += QString::fromLocal8Bit( text_.data() );
                    } else { // 7 bit
                        txt += QString::fromLatin1( text_.data() );
                    }
                }
            }
        }
    }
    return txt;
}

QString MessageDevice::html(bool bAllHTMLParts)
{
    QString txt;
    QCString txtTemp;
    QCString htmlAttachment="\
                            <br>&nbsp;\
                            <p>-------- Attachment Part ---------\
                            <br>&nbsp;\
                            ";
    IVList textParts;
    IVList htmlParts;
    IVList allParts;

    if( htmlPart(&htmlParts) || textPart(&textParts) )
    {
        allParts=textParts+htmlParts;
        qHeapSort( allParts );

        for(IVList::Iterator it=allParts.begin(); it!=allParts.end(); ++it)
        {
            QCString text_;
            MimePart *part=_index->getPartAt(*it);

            if( bAllHTMLParts==true || part->type!="attachment" )
            {
                getPartData( *it , text_);
                txtTemp = text_;
                if( part->mimetype=="text/plain" )
                {
                    if( part->type=="attachment" )
                        txt += htmlAttachment;
                    //TextHtmlConvertor::ref()->text2html(txtTemp);
                }
                else //html
                {
                    if( part->type=="attachment" )
                        txt += htmlAttachment;
                    //TextHtmlConvertor::ref()->html2html(txtTemp);
                }

                // back to text_
                text_ = txtTemp;

                QTextCodec * codec = QTextCodec::codecForName( part->charset );
                if(codec)
                {
                    txt += codec->toUnicode( text_ );
                }
                else
                {
                    if( "utf-8" == part->charset.lower() )
                    {
                        txt += QString::fromUtf8( text_.data() );
                    }
                    else if( "8bit" == part->encoding.lower() )
                    {
                        txt += QString::fromLocal8Bit( text_.data() );
                    }
                    else // 7 bit
                    {
                        txt += QString::fromLatin1( text_.data() );
                    }
                }
            }
        }
    }
    return txt;
}

QCString MessageDevice::rfc822Message()
{
    QFile mailFile(_index->getDataFilename());
    QCString headerLine;
    int dataLength=_index->getUniblockLength();

    mailFile.open(IO_ReadOnly);
    mailFile.at(_index->getUniblockOffset());

    QCString message(dataLength+1);
    mailFile.readBlock(message.data(), dataLength);
    message.data()[dataLength]=0;

    mailFile.close();

    return message;
}

QCString MessageDevice::rfc822Header()
{
    QCString header;

    if( 0 /*_index->isMultipartOnly()*/)
    {
        loadDescriptor(false);
        QDateTime currentDateTime=QDateTime::currentDateTime();

        // date
        header+="Date: "+(QCString)DateClass(currentDateTime)+"\n";

        // to
        header+="To: ";
        for(uint i=0;i<_descriptor.to.count();i++)
        {
            header+=_descriptor.to[i];
            if(i==_descriptor.to.count()-1)
                header+="\n";
            else
                header+=",\n\t";
        }

        // cc
        if(_descriptor.cc.count())
        {
            header+="Cc: ";
            for(uint i=0;i<_descriptor.cc.count();i++)
            {
                header+=_descriptor.cc[i];
                if(i==_descriptor.cc.count()-1)
                    header+="\n";
                else
                    header+=",\n\t";
            }
        }

        // from
        header+="From: "+(QCString)_descriptor.from+"\n";

        // reply-to
        if(!_descriptor.replyTo.isEmpty()) header+="Reply-To: "+_descriptor.replyTo+"\n";

        // subject
        header+="Subject: "+_descriptor.subject+"\n";

        // header-id
        header+="Message-ID: <"+_descriptor.messageID+">\n";

        // in-reply-to
        if(!_descriptor.inReplyTo.isEmpty()) header+="In-Reply-To: <"+_descriptor.inReplyTo+">\n";

        // references
        if(!_descriptor.references.isEmpty() && !_descriptor.inReplyTo.isEmpty())
            header+="References: <"+_descriptor.references.join("> <")+">\n";

        // priority
        if(!_descriptor.priority.isEmpty()) header+="Priority: "+_descriptor.priority+"\n";

        // x tags
        header+="X-Mailer: AbanQ "+QCString(AQ_VERSION)+"\n";
        header+="X-AbanQ-Generated: header (rfc822), reference id "+_descriptor.indexID+"\n";

        // MIME version
        header+="MIME-Version: 1.0\n";

        // content-type & content-transfer-encoding
        if(_index->getPartCount())
        {
            if(_index->getPartCount()==1)
            {
                MimePart *part=_index->getPartAt(0);

                header+="Content-Type: "+part->mimetype;
                if(!part->charset.isEmpty())
                    header+="; charset="+part->charset;
                header+="\n";
                header+="Content-Transfer-Encoding: "+part->encoding+"\n";
            }
            else
            {
                QCString time=(QCString)DateClass(currentDateTime);
                time=time.replace(QRegExp("\\s"), "_").lower();
                QCString boundary=QCString("\"-boundary");
                boundary+=time;
                boundary+=_descriptor.indexID+"\"";
                header+="Content-Type: multipart/related;\n\tboundary="+boundary+"\n";
            }
        }
    }
    else
    {
        QFile mailFile(_index->getDataFilename());
        int dataLength=_index->getUniblockLength();

        mailFile.open(IO_ReadOnly);
        mailFile.at(_index->getUniblockOffset());

        header.resize(dataLength+1);
        mailFile.readBlock(header.data(), dataLength);
        header.data()[dataLength]=0;

        mailFile.close();
    }

    return header;
}

bool MessageDevice::saveMessage(int iPart, const QByteArray & data)
{
    QCString message, line;
    loadDescriptor(false);

//  qDebug("Saving the message from parts");

    QDateTime currentDateTime=QDateTime::currentDateTime();

    // date
    message="Date: "+(QCString)DateClass(currentDateTime)+"\n";

    // to
    AddressListClass toAdd(_descriptor.to.join(", "), true);
    QStringList toList = toAdd.toQStringList();

    line="To: ";

    for (uint i=0;i<toList.count();i++)
    {
        line+=toList[i];
        if (i<toList.count()-1) {
            line+=", ";
            if (line.length()+toList[i+1].length()>75) line+="\n\t";
        }
    }

    line+="\n";

    message+=line;

    // cc
    if (_descriptor.cc.count())
    {
        AddressListClass ccAdd(_descriptor.cc.join(", "), true);
        QStringList ccList = ccAdd.toQStringList();

        line="Cc: ";
        for (uint i=0;i<ccList.count();i++) {
            line+=ccList[i];
            if (i<ccList.count()-1) {
                line+=", ";
                if (line.length()+ccList[i+1].length()>75) line+="\n\t";
            }
        }

        line+="\n";

        message+=line;
    }

    // from
    AddressClass fromAdd( _descriptor.from );
    message+="From: "+ fromAdd.getOriginalAddress() +"\n";

    // reply-to
    if ( !_descriptor.replyTo.isEmpty() )
    {
        AddressClass replytoAdd( _descriptor.replyTo );
        message += "Reply-To: " + replytoAdd.getOriginalAddress() + "\n";
    }

    // subject
    // if it cannot be converted to UTF8 then we use encoded subject
    QTextCodec * codec = 0L;
    QCString subj = _descriptor.subject.utf8();
    QCString ascii = _descriptor.subject.ascii();
    if ( _descriptor.subject == QString::fromAscii(ascii) &&
            CharSets::thisInstance()->isRFC822(ascii) )
    {
        subj = ascii;
    } else if ( _descriptor.subject != QString::fromUtf8( subj ) &&
                (codec = CharSets::thisInstance()->getCodec(_descriptor.subject)) )
    {
        subj = QCString("=?") +
               codec->mimeName() +
               "?B?" +
               MIMECodec::encodeBase64(codec->fromUnicode(_descriptor.subject), false, false) +
               "?=";
    } else
    {
        subj = QCString("=?utf-8?B?") +
               MIMECodec::encodeBase64(subj, false, false) +
               "?=";
    }

    message += "Subject: " + subj + "\n";

    // header-id
    message+="Message-ID: <"+_descriptor.messageID+">\n";

    // in-reply-to
    if (!_descriptor.inReplyTo.isEmpty())
    {
        message+="In-Reply-To: <"+_descriptor.inReplyTo+">\n";
    }

    // references
    if (_descriptor.references.count() && !_descriptor.inReplyTo.isEmpty())
        message+="References: <"+_descriptor.references.join("> <")+">\n";

    // priority
    if (!_descriptor.priority.isEmpty()) message+="Priority: "+_descriptor.priority+"\n";

    // x tags
    message+="X-Mailer: AbanQ "+QCString(AQ_VERSION)+"\n";
    message+="X-AbanQ-Generated: header (rfc822), reference id "+_descriptor.indexID+"\n";

    // MIME version
    message+="MIME-Version: 1.0\n";

    // Add the extra information
    // The lines has to be finished with \n
    if ( !_descriptor.extraInfo.isEmpty() )
    {
        if ( "HEADER_EXTRA " == _descriptor.extraInfo.left(13) ) {
            QCString extraLines = _descriptor.extraInfo.mid(13).ascii();
            if ( !extraLines.isEmpty() ) {
                message += extraLines;
            }
        }
    }

//  qDebug("Parts = %u", _index->getPartCount());

    // content-type & content-transfer-encoding
    if (_index->getPartCount())
    {
        if (_index->getPartCount()==1) {
            MimePart *part=_index->getPartAt(0);

            message+="Content-Type: "+part->mimetype;
            if (!part->charset.isEmpty())
                message+="; charset="+part->charset;
            message+="\n";
            message+="Content-Transfer-Encoding: "+part->encoding+"\n\n";

            QByteArray buf;

            if ( !iPart && !data.isEmpty() ) {
                buf = MIMECodec::encode(data, part->encoding);
            } else {
                QFile mailFile(_index->getDataFilename());
                if ( mailFile.open(IO_ReadOnly) ) {
                    mailFile.at(part->offset);

                    buf.resize(part->length+1);
                    mailFile.readBlock(buf.data(), part->length);
                    buf.data()[part->length] = 0;
                    mailFile.close();
                }
            }

            part->offset = message.length();
            message.append( buf.data() );
            part->length = message.length() - part->offset;
        } else {
            QCString time=(QCString)DateClass(currentDateTime);
            time=time.replace(QRegExp("\\s"), "_").lower();
            QCString boundary=QCString("-boundary");
            boundary+=time;
            boundary+=_descriptor.indexID;
            message+="Content-Type: multipart/related;\n\tboundary=\""+boundary+"\"\n";

            QFile mailFile(_index->getDataFilename());

            if ( !mailFile.open(IO_ReadOnly) ) {
                qDebug("No reading rights for %s", _index->getDataFilename().latin1());

                return false;
            }
            
            int lastText = 0;
            for (int i=0;i<_index->getPartCount();i++) {
              if (_index->getPartAt(i)->type=="text")
                lastText = i;
            }
            for (int i=0;i<_index->getPartCount();i++) {
                MimePart *part=_index->getPartAt(i);

                if ( part ) {
                    message+="\n\n--"+boundary+"\n";

                    line="Content-Type: ";
                    line+=part->mimetype;

                    if (part->type=="text" && !part->charset.isEmpty())
                        line += QCString("; charset=")+part->charset;
                    else if (part->type=="attachment") {
                        if (part->name.isEmpty()) part->name="attachment.dat";

                        QCString attachmentName=QCString("; name=")+"\""+part->name+"\"";

                        if ((line.length()+attachmentName.length())>75) line+="\n\t";

                        line+=attachmentName;
                    }
                    line+="\n";
                    message+=line;

                    message+="Content-Transfer-Encoding: "+part->encoding+"\n";

                    if ( !part->cid.isEmpty())
                      message+="Content-ID: <"+part->cid+">\n";

                    if (part->type=="attachment") {
                        line="Content-Disposition: attachment; ";
                        QCString attachmentFilename=QCString("filename=")+"\""+part->name+"\""+"\n";

                        if ((line.length()+attachmentFilename.length())>75) line+="\n\t";

                        line+=attachmentFilename;

                        line+="\n";

                        message+=line;
                    } else message+="\n";

                    QByteArray buf;

                    if ( iPart == i && !data.isEmpty() ) {
                        buf = MIMECodec::encode(data, part->encoding, (i == lastText));
                    } else {
                        mailFile.at(part->offset);

                        buf.resize(part->length+1);
                        mailFile.readBlock(buf.data(), part->length);
                        buf.data()[part->length] = 0;
                    }

                    part->offset = message.length();
                    message.append( buf.data() );
                    part->length = message.length() - part->offset;
                }
            }
            mailFile.close();

            message+="\n--"+boundary+"--\n";
        }
    }

    QFile mailFile(_index->getDataFilename());

    // append the message
    if ( !mailFile.open(IO_WriteOnly|IO_Append) )
    {
        qDebug("No writing rights for %s", _index->getDataFilename().latin1());
        return false;
    }

    QCString mboxInfo;
    mboxInfo = "From - " + QDateTime::currentDateTime().toString() + "\r\n";
    mailFile.writeBlock((const char *)mboxInfo, mboxInfo.length());

    long mailOffset = mailFile.at();

    _index->setUniblockOffset(mailOffset);
    mailFile.writeBlock(message.data(), message.length());
    _index->setUniblockLength( mailFile.at() - mailOffset );

    mailFile.writeBlock("\r\n", 2);
    mailFile.close();

    loadDescriptor();
    _descriptor.size = 0;
    _descriptor.attachments = false;

    {
        for (int i=0;i<_index->getPartCount();i++) {
            MimePart *part=_index->getPartAt(i);

            if ( part ) {
                part->offset += mailOffset;
                _descriptor.size += part->length;
                if ( "attachment" == part->type ) {
                    _descriptor.attachments = true;
                }
            }
        }
    }

    saveDescriptor();

//  ServerNotifier::thisInstance()->objectChanged(_index->getParentFolder()->name()+"/"+_index->getID());

    return true;
}

QByteArray MessageDevice::getEntryCacheBlock()
{
    QByteArray data;
    QDataStream stream(data, IO_WriteOnly);

    loadDescriptor(true);

    Q_UINT8 attachments=false;
    for (int i=0;i<_index->getPartCount() && !attachments;i++)
        if (_index->getPartAt(i)->type=="attachment")
            attachments=true;

    stream<<_descriptor.status;
    stream<<_descriptor.extendedStatus;
    stream<<_descriptor.flag;
    stream<<attachments;
    stream<<_descriptor.red;
    stream<<_descriptor.green;
    stream<<_descriptor.blue;
    stream<<_descriptor.from;
    stream<<_descriptor.to;
    stream<<_descriptor.subject;
    stream<<_descriptor.sentDate;
    stream<<_descriptor.receivedDate;
    stream<<_descriptor.scheduling;
    stream<<_descriptor.priority;
    stream<<_descriptor.parentIndexID;
    stream<<_index->getSize();
    stream<<_descriptor.contentType;
    stream<<_descriptor.inReplyTo;
    stream<<_descriptor.references;

    return data;
}

