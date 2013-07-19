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
                          mailfolder.cpp  -  the mail folder interface
                             -------------------
    begin                : Mon Mar  5 16:16:00 EET 2001
    copyright            : (C) 2001 by theKompany.com (www.thekompany.com)
    author               : Eugen Constantinescu
    email                : eug@thekompany.com
 ***************************************************************************/

#include "mailfolder.h"
#include "AQConfig.h"

#include <indexclass.h>
#include <messagedevice.h>
#include <messagedescriptor.h>
#include <messagefactory.h>
#include <mimecodec.h>


#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <quuid.h>

// this isn't used, right ?
AQMAIL_EXPORT unsigned long MailFolder::lastIndexID = 0;

AQMAIL_EXPORT QPtrList<class MailFolder> MailFolder::folderList;
AQMAIL_EXPORT bool MailFolder::autoExpunge=true;

MailFolder::MailFolder(const QString &_storageDevice)
        : storageDevice(_storageDevice),
        folderType(Normal),
        folderProtocol(POP3),
        messagesPlace(LOCAL),
        accessRights(ALL),
        unread(0)
{
    folderList.append(this);

    setupFiles();

    indexCollection.setAutoDelete(true);

    // Set the messages list to auto-delete the elements
    syncMessages.setAutoDelete(true);
};

MailFolder::~MailFolder()
{
    // remove the folder from the list
    folderList.removeRef(this);
}

void MailFolder::setupFiles()
{
    indexFileName=storageDevice+"/index";
    descriptorFileName=storageDevice+"/descriptor";

    if (storageDevice.contains("sysmail_deleted")) {
        folderType=MailFolder::Deleted;
        messagesPlace=MailFolder::LOCAL;
        folderProtocol=MailFolder::MBOX;
        accessRights=MailFolder::ALL;
    } else if (storageDevice.contains("sysmail_drafts")) {
        folderType=MailFolder::Drafts;
        messagesPlace=MailFolder::LOCAL;
        folderProtocol=MailFolder::MBOX;
        accessRights=MailFolder::ALL;
    } else if (storageDevice.contains("sysmail_inbox")) {
        folderType=MailFolder::Inbox;
        messagesPlace=MailFolder::LOCAL;
        folderProtocol=MailFolder::MBOX;
        accessRights=MailFolder::ALL;
    } else if (storageDevice.contains("sysmail_outbox")) {
        folderType=MailFolder::Outbox;
        messagesPlace=MailFolder::LOCAL;
        folderProtocol=MailFolder::MBOX;
        accessRights=MailFolder::ALL;
    } else if (storageDevice.contains("sysmail_sent")) {
        folderType=MailFolder::Sent;
        messagesPlace=MailFolder::LOCAL;
        folderProtocol=MailFolder::MBOX;
        accessRights=MailFolder::ALL;
    } else if (storageDevice.contains("sysmail")) {
        folderType=MailFolder::Normal;
        messagesPlace=MailFolder::LOCAL;
        folderProtocol=MailFolder::MBOX;
        accessRights=MailFolder::ALL;
    } else if (storageDevice.contains("imap_")) {
        folderType=MailFolder::Normal;
        messagesPlace=MailFolder::REMOTE;
        folderProtocol=MailFolder::IMAP4;
        accessRights=MailFolder::ALL;
    } else {
        folderType=MailFolder::Normal;
        messagesPlace=MailFolder::LOCAL;
        folderProtocol=MailFolder::MBOX;
        accessRights=MailFolder::ALL;
    }
};

int MailFolder::getFolderType() const
{
    return folderType;
}

int MailFolder::getFolderProtocol() const
{
    return folderProtocol;
}

QString MailFolder::getStorageDevice() const
{
    return storageDevice;
}

QString MailFolder::createMessage()
{
    // create index, set id and add to collection
    IndexClass *index = new IndexClass(this);
    if (!index) {
        return QString::null;
    }

    syncIndexID(index);
    QString newUUID = index->getID();
    indexCollection.insert(newUUID, index);

    // this is probably a dynamic-generated message, multipart by default
    index->setMultipartOnly(true);

    // create message device
    MessageDevice *dev=new MessageDevice(index);

    // add data (index id etc)
    MessageDescriptor &descriptor=dev->getDescriptor();
    descriptor.indexID = newUUID;

    // create a temporary id for the message
    // encode it
    descriptor.messageID = descriptor.indexID;
    descriptor.uid = descriptor.messageID;

    descriptor.status="New";

    // write message id and unread mark into index
    index->setMessageID(descriptor.messageID);
    index->setUnreadMark(true);

    // update folder stats
    if (index->getUnreadMark())
        unread++;

    // save index
    dev->saveDescriptor();

    // cleanup
    delete dev;

    // notify client
    //ServerNotifier::thisInstance()->objectCreated(name() + "/" + newUUID);

    return newUUID;
}

void MailFolder::saveIndex()
{
    // open file
    QFile dataFile(indexFileName);
    dataFile.open(IO_WriteOnly);
    QDataStream stream(&dataFile);

    // save index version
    stream<<QString(AQ_VERSION);

    // save indexes
    QDictIterator<IndexClass> it(indexCollection);

    IndexClass *index;
    while ((index=it.current())) {
        stream<<(*index);
        ++it;
    }

    // close file
    dataFile.close();
    //  qDebug("Folder %s(%p) has finished saving successfully.", (const char *)name(), this);
};

void MailFolder::saveIndex(IndexClass *index)
{
    // open file
    QFile dataFile(indexFileName);
    QDataStream stream;
    stream.setDevice(&dataFile);

    // check to see if this is the first index and if so save the index version
    switch ( indexCollection.count() ) {
    case 0:
        // remove the file if there are not mails
        dataFile.remove();
        break;
    case 1:
        dataFile.open(IO_WriteOnly);
        stream<<QString(AQ_VERSION);
        // save index
        stream<<(*index);
        // close file
        dataFile.close();
        break;
    default:
        dataFile.open(IO_Append|IO_WriteOnly);
        // save index
        stream<<(*index);
        // close file
        dataFile.close();
        break;
    }
}

int MailFolder::getUnread() const
{
    return unread;
}

void MailFolder::setUnread(int _unread)
{
    unread=_unread;
}

void MailFolder::incrementUnread()
{
    unread++;
}

void MailFolder::decrementUnread()
{
    unread--;
}

void MailFolder::setAutoExpunge(bool expunge)
{
    autoExpunge=expunge;
}

bool MailFolder::getAutoExpunge() const
{
    return autoExpunge;
}

int MailFolder::getMessageCount() const
{
    return indexCollection.count();
}

QStringList MailFolder::getEntryList(const QString url) const
{
    QStringList t;
    QDictIterator<IndexClass> it(indexCollection);

    if ( url.isEmpty() ) {
        while (it.current()) {
            t.append(it.currentKey());
            ++it;
        }
    } else {
        while (it.current()) {
            t.append(url + "/" + it.currentKey());
            ++it;
        }
    }

    return t;
}

QDict<IndexClass>& MailFolder::indexIDict()
{
    return indexCollection;
}

QString MailFolder::name() const
{
    return storageDevice.mid(storageDevice.findRev('/')+1);
}

QString MailFolder::nextIndexID()
{
    return QUuid::createUuid().toString();
}

void MailFolder::syncIndexID(IndexClass *idx)
{
    idx->setID(nextIndexID());
}

void MailFolder::crossReferenceIndexSet(IndexClass *idx)
{
    /*if(Settings::instance()->readBoolEntry("/Aethera/Threading/Complete references"))
    {
      MessageDevice *dev=new MessageDevice(idx);
      dev->loadDescriptor();
      QString parentIndexID=dev->getDescriptor().parentIndexID;
      delete dev;
      
      if(!parentIndexID.isEmpty())
      {
        // reparent and cross references for all items under parentIndexID
        QDictIterator<IndexClass> it(indexCollection);
        
        IndexClass *t;
        while((t=it.current()))
        {
          if(t->getParentID()==parentIndexID)
          {
            reparentIndex(t);
            crossReferenceIndex(t);
          }
        
          ++it;
        }
      }
    }
    else
    {*/
    crossReferenceIndex(idx);
    /*}*/
}

void MailFolder::crossReferenceIndex(IndexClass *idx)
{
    // load message descriptor
    MessageDevice *dev=new MessageDevice(idx);
    dev->loadDescriptor();
    MessageDescriptor &descriptor=dev->getDescriptor();

    for (unsigned int i=0;i<descriptor.references.count();i++) {
        // find if reffered message exists in Aethera
        QString relatedIndexID=translateGlobalMessageID(descriptor.references[i]);

        if (!relatedIndexID.isNull()) {
            // ...it does, so append the index id
            if (descriptor.relatedIndexIDList.find(relatedIndexID)==descriptor.relatedIndexIDList.end())
                descriptor.relatedIndexIDList.append(relatedIndexID);

            // load related message descriptor
            MessageDevice *relatedDev=new MessageDevice(findIndex(relatedIndexID));
            relatedDev->loadDescriptor(false);
            MessageDescriptor &relatedDescriptor=relatedDev->getDescriptor();
            bool saveRelated=false;

            // if the parent descriptor doesn't have this message in the references list then append it
            if (relatedDescriptor.references.find(idx->getMessageID())==relatedDescriptor.references.end()) {
                relatedDescriptor.references.append(idx->getMessageID());
                saveRelated=true;
            }

            // add the current message
            if (relatedDescriptor.relatedIndexIDList.find(idx->getID())==relatedDescriptor.relatedIndexIDList.end()) {
                relatedDescriptor.relatedIndexIDList.append(idx->getID());
                saveRelated=true;
            }

            // save related descriptor
            if (saveRelated)
                relatedDev->saveDescriptor();

            // cleanup
            delete relatedDev;
        }
    }

    // cleanup
    delete dev;
}

void MailFolder::reparentIndex(IndexClass *idx)
{
    // load message descriptor
    MessageDevice *dev=new MessageDevice(idx);
    dev->loadDescriptor();
    MessageDescriptor &descriptor=dev->getDescriptor();

    // check if this message has the "In-Reply-To" field which maps to a message in this folder
    QString parentIndexID=translateLocalMessageID(descriptor.inReplyTo);
    if (!parentIndexID.isNull()) {
        // we found a valid parent
        descriptor.parentIndexID=parentIndexID;

        // save descriptor
        dev->saveDescriptor();

        // add message to the parent reference list
        MessageDevice *parentDevice=new MessageDevice(indexCollection[parentIndexID]);
        parentDevice->loadDescriptor(false);
        MessageDescriptor &parentDescriptor=parentDevice->getDescriptor();

        if (parentDescriptor.references.find(idx->getMessageID())==parentDescriptor.references.end()) {
            parentDescriptor.references.append(idx->getMessageID());
            if (parentDescriptor.relatedIndexIDList.find(idx->getID())==parentDescriptor.relatedIndexIDList.end())
                parentDescriptor.relatedIndexIDList.append(idx->getID());

            parentDevice->saveDescriptor();
        }

        delete parentDevice;

        idx->setParentID(parentIndexID);
        saveIndex(idx);

        // notify client
        //ServerNotifier::thisInstance()->hierarchyChanged(name()+"/"+idx->getID());
    }
    /*else
    {
      // no "true" parent was found, try heuristics
      QString threadBy
        =Settings::instance()->readEntry("/Aethera/Threading/Thread by");

      bool threadAnyway
        =Settings::instance()->readBoolEntry("/Aethera/Threading/Thread anyway");

      if(threadBy=="Subject")
      {
        // if we still have to parent, resort to the other method if the user wants that
        if(threadAnyway && !reparentBySubject(idx))
          reparentByRelated(idx);
      }
      else
      {
        // if we still have to parent, resort to the other method if the user wants that
        if(threadAnyway && !reparentByRelated(idx))
          reparentBySubject(idx);
      }
    }*/

    // cleanup
    delete dev;
}

bool MailFolder::reparentByRelated(IndexClass *idx)
{
    // load descriptor
    MessageDevice *dev=new MessageDevice(idx);
    dev->loadDescriptor();
    MessageDescriptor &descriptor=dev->getDescriptor();

    QString relatedIndexID, firstRelatedIndexID=QString::null;
    bool res=false;

    // scan the "Related-To" list for a related message that lives in the same folder and matches the subject
    for (unsigned int i=0;i<descriptor.references.count();i++) {
        relatedIndexID=translateLocalMessageID(descriptor.references[i]);

        if (!relatedIndexID.isNull()) {
            // set fallback to the first related message in the same folder in case subject matching fails
            if (firstRelatedIndexID.isNull())
                firstRelatedIndexID=relatedIndexID;

            bool found=false;

            // load prospective parent descriptor
            MessageDevice *parentDev=new MessageDevice(indexCollection[relatedIndexID]);
            parentDev->loadDescriptor();

            if (subjectThreadMatch(parentDev->getDescriptor().subject, descriptor.subject))
                found=true;

            delete parentDev;

            if (found)
                break;
        }

        relatedIndexID=QString::null;
    }

    if (relatedIndexID.isNull()) {
        // no related messages match the subject, therefore use the first one
        relatedIndexID=firstRelatedIndexID;
    }

    if (!relatedIndexID.isNull()) {
        // eventually we found something, save it
        descriptor.parentIndexID=relatedIndexID;
        dev->saveDescriptor();

        // add message to the parent reference list
        MessageDevice *parentDevice=new MessageDevice(indexCollection[relatedIndexID]);
        parentDevice->loadDescriptor(false);
        MessageDescriptor &parentDescriptor=parentDevice->getDescriptor();

        if (parentDescriptor.references.find(idx->getMessageID())==parentDescriptor.references.end()) {
            parentDescriptor.references.append(idx->getMessageID());

            if (parentDescriptor.relatedIndexIDList.find(idx->getID())==parentDescriptor.relatedIndexIDList.end())
                parentDescriptor.relatedIndexIDList.append(idx->getID());

            parentDevice->saveDescriptor();
        }

        delete parentDevice;
        res=true;

        idx->setParentID(relatedIndexID);
        saveIndex(idx);

        // notify client
        //ServerNotifier::thisInstance()->hierarchyChanged(name()+"/"+idx->getID());
    }

    // cleanup
    delete dev;

    return res;
}

bool MailFolder::reparentBySubject(IndexClass *idx)
{
    // load descriptor
    MessageDevice *dev=new MessageDevice(idx);
    dev->loadDescriptor();
    MessageDescriptor &descriptor=dev->getDescriptor();

    bool res=false;

    // scan the whole folder for a parent that matches the subject and was received after the current message
    QDictIterator<IndexClass> it(indexCollection);
    IndexClass *t;
    while ((t=it.current())) {
        // load prospective parent descriptor
        MessageDevice *parentDev=new MessageDevice(t);
        parentDev->loadDescriptor();
        MessageDescriptor &parentDescriptor=parentDev->getDescriptor();

        if (subjectThreadMatch(parentDescriptor.subject, descriptor.subject) &&
                DateClass((QCString)parentDescriptor.receivedDate).toQDateTime()<=DateClass((QCString)descriptor.receivedDate).toQDateTime()) {
            descriptor.parentIndexID=it.currentKey();
            res=true;
        }

        delete parentDev;

        ++it;
    }

    if (res) {
        dev->saveDescriptor();

        // load parent descriptor
        MessageDevice *parentDevice=new MessageDevice(indexCollection[descriptor.parentIndexID]);
        parentDevice->loadDescriptor(false);
        MessageDescriptor &parentDescriptor=parentDevice->getDescriptor();

        // add message to the parent reference list
        if (parentDescriptor.references.find(idx->getMessageID())==parentDescriptor.references.end()) {
            parentDescriptor.references.append(idx->getMessageID());

            if (parentDescriptor.relatedIndexIDList.find(idx->getID())==parentDescriptor.relatedIndexIDList.end())
                parentDescriptor.relatedIndexIDList.append(idx->getID());

            parentDevice->saveDescriptor();
        }

        delete parentDevice;

        idx->setParentID(descriptor.parentIndexID);
        saveIndex(idx);

        // notify client
        //ServerNotifier::thisInstance()->hierarchyChanged(name()+"/"+idx->getID());
    }

    // cleanup
    delete dev;

    return res;
}

bool MailFolder::subjectThreadMatch(QString /*parentSubject*/, QString /*sonSubject*/)
{
    /*QStringList tokens=
      Settings::instance()->readEntry("/Aethera/Threading/Thread reply tokens");

    // set some default tokens if none were found
    if(!tokens.count())
    {
      tokens.append("i^\\s*re:\\s*");
      tokens.append("i^\\s*\\[re:\\]\\s*");
    }

    for(unsigned int i=0;i<tokens.count();i++)
    {
      bool cs=true;
      if(tokens[i][0]=='i')
      {
        cs=false;
        tokens[i]=tokens[i].mid(1);
      }
      
      QRegExp rexp(tokens[i]);
      rexp.setCaseSensitive(cs);
      
      int len;
      if(rexp.match(parentSubject, 0, &len)!=-1)
      {
        if(parentSubject.mid(len).stripWhiteSpace().lower().find(sonSubject.stripWhiteSpace().lower())==0)
          return true;
      }
    }*/

    return false;
}

QString MailFolder::translateLocalMessageID(QString messageID)
{
    if ( messageID.isEmpty() )
        return QString::null;

    QDictIterator<IndexClass> it(indexCollection);

    IndexClass *t;

    while ((t=it.current())) {
        if (t->getMessageID()==messageID)
            return it.currentKey();
        ++it;
    }

    return QString::null;
}

QString MailFolder::translateGlobalMessageID(QString messageID)
{
    QString indexID;
    for (unsigned int i=0;i<folderList.count();i++) {
        indexID=folderList.at(i)->translateLocalMessageID(messageID);
        if (!indexID.isEmpty())
            return indexID;
    }

    return QString::null;
}

IndexClass *MailFolder::findIndex(const QString &id)
{
    IndexClass * index = 0;

    for (unsigned int i=0; i<folderList.count(); i++) {
        if ((index=folderList.at(i)->indexCollection[id]))
            break;
    }

    return index;
}

QMap<QString, QByteArray> MailFolder::entryCacheBlock()
{
    QMap<QString, QByteArray> cacheMap;

    /*  QDictIterator<IndexClass> it(indexCollection);
      IndexClass *index;
      while((index=it.current()))
      {
        MessageDevice *dev=new MessageDevice(index);
        cacheMap.insert(index->getID(), dev->getEntryCacheBlock());
        delete dev;
        ++it;
      }*/

    return cacheMap;
}

void MailFolder::setPruneIndexCount(unsigned int _pruneIndexCount)
{
    pruneIndexCount=_pruneIndexCount;
}

void MailFolder::incrementPruneIndexCount()
{
    pruneIndexCount++;
}

unsigned int MailFolder::getPruneIndexCount() const
{
    return pruneIndexCount;
}

QString MailFolder::getIndexFileName() const
{
    return indexFileName;
}

QString MailFolder::getDescriptorFileName() const
{
    return descriptorFileName;
}

unsigned long MailFolder::getLastIndexID() const
{
    return lastIndexID;
}

void MailFolder::setLastIndexID(long unsigned indexId)
{
    lastIndexID=indexId;
}

bool MailFolder::compact(unsigned level)
{
    // sync first so that you are sure the database is ok
    sync();

    if ( level ) {
        qDebug("Folder compressing isn't yet available.");
    }

    QCString mboxInfo;
    mboxInfo = "From - " + QDateTime::currentDateTime().toString() + "\r\n";

    QFile descriptorFile( getDescriptorFileName() );
    if ( !descriptorFile.open(IO_ReadOnly) )
        return false;

    QFile newDescriptorFile( getDescriptorFileName()+"new" );
    if ( !newDescriptorFile.open(IO_WriteOnly) )
        return false;

    QFile messageFile( getMessagesFileName() );
    if ( !messageFile.open(IO_ReadOnly) )
        return false;

    QFile newMessageFile( getMessagesFileName()+"new" );
    if ( !newMessageFile.open(IO_WriteOnly) )
        return false;

    IndexClass * index = 0L;
    QByteArray buffer;

    for (QDictIterator<IndexClass> it(indexCollection); it.current(); ++it) {
        index = it.current();

        if ( !index->getDescriptorLength() ||
                !index->getUniblockLength() ) {
            qDebug("The index file seems to be broken :(");
            indexCollection.remove( index->getID() );
            continue;
        }

        // read the descriptor
        buffer.resize( index->getDescriptorLength() );
        descriptorFile.at( index->getDescriptorOffset() );
        descriptorFile.readBlock(buffer.data(), buffer.size());

        // write the descriptor
        index->setDescriptorOffset( newDescriptorFile.at() );
        newDescriptorFile.writeBlock( buffer );

        // read the message
        buffer.resize( index->getUniblockLength() );
        messageFile.at( index->getUniblockOffset() );
        messageFile.readBlock(buffer.data(), buffer.size());

        // write the message
        // The MBOX line isn't right but the line isn't used too.
        // So, I decided to just store only the current date.
        newMessageFile.writeBlock((const char *)mboxInfo, mboxInfo.length());
        index->setUniblockOffset(newMessageFile.at(), true);

        newMessageFile.writeBlock( buffer );
        newMessageFile.writeBlock("\r\n", 2);
    }

    descriptorFile.close();
    newDescriptorFile.close();
    messageFile.close();
    newMessageFile.close();

    buffer.truncate(0);

    // replace the old files
    descriptorFile.remove();
    messageFile.remove();

    QDir folder( getStorageDevice() );
    folder.rename( newDescriptorFile.name(), getDescriptorFileName(), true );
    folder.rename( newMessageFile.name(), getMessagesFileName(), true );

    saveIndex();

    return true;
}

