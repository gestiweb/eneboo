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

#include <qstring.h>
#include <qdatastream.h>
#include <mimepart.h>
#include <indexclass.h>
#include <errno.h>
#include <mailfolder.h>
#include <messagedevice.h>

IndexClass::IndexClass(MailFolder *parent, QString id)
        : _parent(parent),
        _id(id),
        _descriptorOffset(0),
        _descriptorLength(0),
        _uniblockOffset(0),
        _uniblockLength(0),
        _multipartOnly(0),
        _unreadMark(0),
        _lock(false)
{
    _parentID="";
    _messageID="";
}

IndexClass::IndexClass(const IndexClass & source)
{
    _id=source._id;
    _parentID=source._parentID;
    _messageID=source._messageID;
    _descriptorOffset=source._descriptorOffset;
    _descriptorLength=source._descriptorLength;
    _uniblockOffset=source._uniblockOffset;
    _uniblockLength=source._uniblockLength;
    _multipartOnly=source._multipartOnly;
    _unreadMark=source._unreadMark;
    _lock = false;

    QPtrList<MimePart> temp = source._partList;

    for (unsigned i=0;i<temp.count();i++)
        _partList.append(new MimePart(*temp.at(i)));
}

IndexClass::~IndexClass()
{
    _partList.setAutoDelete(true);
    _partList.clear();
}


QString IndexClass::url() const
{
    QString serverUrl;

    serverUrl = _parent->name() + "/" + getID();

    return serverUrl;
}


MailFolder *IndexClass::getParentFolder() const
{
    return _parent;
}

void IndexClass::setParentFolder(MailFolder *parent)
{
    _parent=parent;
}

QString IndexClass::getDataFilename() const
{
    if (_parent)
        return _parent->getMessagesFileName();
    else
        return QString::null;
}

QString IndexClass::getDescriptorFileName() const
{
    if (_parent)
        return _parent->getDescriptorFileName();
    else
        return QString::null;
}

QString IndexClass::getID() const
{
    return _id;
}

void IndexClass::setID(const QString &id)
{
    _id=id;
}

QString IndexClass::getParentID() const
{
    return _parentID;
}

void IndexClass::setParentID(const QString &parentID)
{
    _parentID=parentID;
}

QString IndexClass::getMessageID() const
{
    return _messageID;
}

void IndexClass::setMessageID(const QString &messageID)
{
    _messageID=messageID;
}

bool IndexClass::getUnreadMark()
{
    return _unreadMark;
}

void IndexClass::setUnreadMark(bool unreadMark)
{
    _unreadMark=unreadMark;
}

int IndexClass::addPart(MimePart *part, bool uniblockRelative)
{
    if (uniblockRelative)
        part->offset+=_uniblockOffset;
    _partList.append(part);

    return _partList.count()-1;
}

bool IndexClass::removePart(int partIndex)
{
    return _partList.remove(partIndex);
}

MimePart* IndexClass::getPartAt(int partIndex)
{
    return _partList.at(partIndex);
}

int IndexClass::getPartCount() const
{
    return _partList.count();
}

long IndexClass::getDescriptorOffset() const
{
    return _descriptorOffset;
}

void IndexClass::setDescriptorOffset(long descriptorOffset)
{
    _descriptorOffset=descriptorOffset;
}

long IndexClass::getDescriptorLength() const
{
    return _descriptorLength;
}

void IndexClass::setDescriptorLength(long descriptorLength)
{
    _descriptorLength=descriptorLength;
}

long IndexClass::getUniblockOffset() const
{
    return _uniblockOffset;
}

void IndexClass::setUniblockOffset(long uniblockOffset, bool bParts)
{
    if (bParts) {
        long relativeOffset = 0L;
        MimePart * part = 0L;

        for (QPtrListIterator<MimePart> it(_partList); it.current(); ++it) {
            part = it.current();

            if ( part->offset || part->length ) {
                relativeOffset = part->offset - _uniblockOffset;
                part->offset = uniblockOffset + relativeOffset;
            } else {
                qDebug("Hmm, saving an unfinished mail!");
            }
        }
    }

    _uniblockOffset = uniblockOffset;
}

long IndexClass::getUniblockLength() const
{
    return _uniblockLength;
}

void IndexClass::setUniblockLength(long uniblockLength)
{
    _uniblockLength=uniblockLength;
}

int IndexClass::getSize()
{
    if (_multipartOnly) {
        int _size=0;
        for (unsigned i=0;i<_partList.count();i++)
            _size+=_partList.at(i)->length;
        return _size;
    } else {
        return _uniblockLength;
    }
}

bool IndexClass::isMultipartOnly()
{
    return _multipartOnly;
}

void IndexClass::setMultipartOnly(bool multipartOnly)
{
    _multipartOnly=multipartOnly;
}

bool IndexClass::isLocked()
{
    return _lock;
}

void IndexClass::lock()
{
    _lock = true;
}

void IndexClass::unlock()
{
    _lock = false;
}

QDataStream& operator>>(QDataStream &stream, IndexClass &index)
{
    stream>>index._id;
    stream>>index._parentID;
    stream>>index._messageID;
    stream>>index._descriptorOffset;
    stream>>index._descriptorLength;
    stream>>index._uniblockOffset;
    stream>>index._uniblockLength;
    stream>>index._multipartOnly;
    stream>>index._unreadMark;
    int partCount;
    stream>>partCount;
    index._partList.clear();

    for (int i=0;i<partCount;i++) {
        MimePart *part=new MimePart;
        stream>>(*part);
        index._partList.append(part);
    }

    return stream;
}

QDataStream& operator<<(QDataStream &stream, IndexClass &index)
{
    stream<<index._id;
    stream<<index._parentID;
    stream<<index._messageID;
    stream<<index._descriptorOffset;
    stream<<index._descriptorLength;
    stream<<index._uniblockOffset;
    stream<<index._uniblockLength;
    stream<<index._multipartOnly;
    stream<<index._unreadMark;

    int partCount=index.getPartCount();
    stream<<partCount;

    for (int i=0;i<partCount;i++) {
        MimePart &part=*index.getPartAt(i);
        stream<<part;
    }

    return stream;
}

bool IndexClass::isFullMessage() const
{
    if ( !getPartCount() )
        return false;
    else
        return true;
}




