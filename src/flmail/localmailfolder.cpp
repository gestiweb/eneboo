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
                localmailfolder.cpp  -  the local mail folder class
                             -------------------
    begin                : Mon Mar  5 16:16:00 EET 2001
    copyright            : (C) 2001 by theKompany (www.thekompany.com>
    author               : Eugen Constantinescu
    email                : eug@thekompany.com
 ***************************************************************************/

#include "localmailfolder.h"

#include <mailfolder.h>
//#include <imap4handler.h>
#include <indexclass.h>
//#include <servernotifier.h>
//#include <accounts.h>
//#include <accountmanager.h>
#include <messagedevice.h>
#include <messagedescriptor.h>
#include <messagefactory.h>
#include <mimecodec.h>
#include <dateclass.h>
//#include <iosynchandler.h>
//#include <orb.h>

//#include <settings.h>

#include <qdir.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qtextstream.h>

#include "AQConfig.h"

LocalMailFolder::LocalMailFolder(const QString &_storageDevice)
        : MailFolder(_storageDevice)
{
    setupFiles();
    loadIndex();
};

LocalMailFolder::~LocalMailFolder()
{
    // make a force synching before removing the folder
    sync();
}

void LocalMailFolder::setupFiles()
{
    messagesFileName=getStorageDevice()+"/messages";
};

void LocalMailFolder::loadIndex()
{
    // debug
    //qDebug("mailfolder: [%s] is reading index...", (const char *)name());

    IndexClass *tindex = 0L;

    QFile f(getIndexFileName());
    QDataStream stream(&f);

    // check index version number
    if (f.open(IO_ReadOnly)) {
        QString idxExpectedVersion(AQ_VERSION);
        QString idxVersion;
        stream>>idxVersion;

        if (idxExpectedVersion!=idxVersion) {
            qDebug("Folder %s attempted to read an older version or invalid index\
                   cache, discarding index...", (const char *)name());
        } else {
            while (!stream.atEnd()) {
                tindex=new IndexClass(this);

                // read index
                stream>>(*tindex);

                // check to see if the index was already included
                IndexClass *pruneIndex;
                if ((pruneIndex=indexCollection[tindex->getID()])) {
                    // ...it was, increment the prune list
                    incrementPruneIndexCount();

                    // update folder stats
                    if (pruneIndex->getUnreadMark())
                        decrementUnread();
                } else {
                    // update the last index id
                    unsigned long tlast=tindex->getID().mid(3).toULong();
                    if (tlast>getLastIndexID())
                        setLastIndexID(tlast+1);
                }
                // add index to collection
                indexCollection.replace(tindex->getID(), tindex);

                // debug
                // qDebug("localmailfolder: [loadIndex] loaded %s...", (const char *)tindex->getID());

                // update folder stats
                if (tindex->getUnreadMark())
                    incrementUnread();
            }
        }

        f.close();
    }
}

QString LocalMailFolder::createMessage(const QCString &text, const QCString &uid,
                                       const QDateTime &rcvtime, const QString &account,
                                       const MessageClass *parsedMessage,
                                       bool bSync, const unsigned flags)
{
    // rfc-parse message
    const MessageClass *rfcMessage = 0L;

    if ( parsedMessage ) {
        rfcMessage = parsedMessage;
    } else {
        rfcMessage = new MessageClass(text);
    }

    // create index, set id and add to collection
    IndexClass *index = new IndexClass(this);
    if (!index) {
        return QString::null;
    }

    syncIndexID(index);
    QString newUUID = index->getID();
    indexCollection.insert(newUUID, index);

    // create message device
    MessageDevice *dev = new MessageDevice(index);

    // populate descriptor
    MessageDescriptor &descriptor=dev->getDescriptor();
    descriptor.load(*rfcMessage);

    // add other data (account, uid, received time, index id etc)
    descriptor.account = account;
    descriptor.indexID = newUUID.local8Bit();
    descriptor.receivedDate=(QCString)DateClass(rcvtime);
    descriptor.uid=uid;

    if ( uid.isEmpty() ) {
        // we need a new UID and message Id.
        descriptor.messageID = newUUID;
        descriptor.uid = newUUID;
    }

    index->setMessageID(descriptor.messageID);
    if ( flags&MailFolder::Seen ) {
        index->setUnreadMark(false);
        descriptor.status="Read";
    } else {
        index->setUnreadMark(true);
        descriptor.status="New";
    }

    // update folder stats
    if (index->getUnreadMark())
        incrementUnread();

    // save message
    QFile dataFile(getMessagesFileName());
    dataFile.open(IO_WriteOnly|IO_Append);

    // write MBOX information
    QCString mboxInfo;
    mboxInfo="From - " + descriptor.receivedDate + "\r\n";
    dataFile.writeBlock((const char *)mboxInfo, mboxInfo.length());

    // write uniblock data (index is uniblock by default)
    index->setUniblockOffset(dataFile.at());
    index->setUniblockLength(text.length());
    index->setMultipartOnly(false);

    dataFile.writeBlock((const char *)text, index->getUniblockLength());
    dataFile.writeBlock("\r\n", 2);

    dataFile.close();

    // create index part list and update offsets
    QPtrList<MimePart> temp(rfcMessage->partList);

    for (unsigned int i=0;i<rfcMessage->partList.count();i++)
        index->addPart(new MimePart(*temp.at(i)), true);

    // notify client or wait for a sync signal
    if ( bSync ) {
        syncMessages.append( dev );

        // force sync for a better memory management when
        // downloading big email boxes
        if ( 300 <= syncMessages.count() ) {
            qDebug("Force syncing");
            sync();
        }
    } else {
        //     // add index and descriptor to hierarchy
        //     if(Settings::instance()->readBoolEntry("/Aethera/Threading/Enable", false))
        //     {
        //       reparentIndex(index);
        //       crossReferenceIndexSet(index);
        //     }

        // save descriptor (that will save the index as well)
        dev->saveDescriptor();
        // cleanup
        delete dev;

        //ServerNotifier::thisInstance()->objectCreated(name() + "/" + newUUID);
    }

    // if it's a local parser
    if ( !parsedMessage ) {
        delete rfcMessage;
        rfcMessage = 0;
    }

    return newUUID;
}

void LocalMailFolder::sync()
{
    if ( !syncMessages.count() ) {
        //    qDebug("MailFolder::sync() : Nothing to sync!");
    } else {
        qDebug("Sync %u mails.", syncMessages.count());

        // save the descriptors and build the messages url list
        QStringList serverURLs;
        QFile dataFile( getDescriptorFileName() );
        if (dataFile.open(IO_WriteOnly|IO_Append)) {
            QDataStream stream(&dataFile);
            for (QPtrListIterator<MessageDevice> it(syncMessages); it.current(); ++it) {
                MessageDescriptor &descriptor = it.current()->getDescriptor();
                IndexClass* index = it.current()->index();

                serverURLs.append(name()+"/"+ descriptor.indexID);

                index->setDescriptorOffset(dataFile.at());
                stream << descriptor;
                index->setDescriptorLength(dataFile.at() - index->getDescriptorOffset());

            }
            dataFile.close();

            // save the whole index once
            saveIndex();

            // this should delete the descriptors too
            syncMessages.clear();

            // notify the UI for the changes
            //ServerNotifier::thisInstance()->objectCreated(serverURLs);
        } else
            qDebug("MailFolder::sync() : the descriptors cannot be saved!");
    }
}

QString LocalMailFolder::copyMessage(IndexClass *idx)
{
    sync();

    if (idx) {
        MailFolder * mailFolder = idx->getParentFolder();
        if (!mailFolder)
            return QString::null;

        if ( MailFolder::IMAP4 == mailFolder->getFolderProtocol() &&
                !idx->isFullMessage() ) {
            QStringList param;

            //IOSyncHandler::ref()->insertToDo(idx->url(), name(), WaitingOperations::CopyObject, param);
            //IMAP4Handler::ref()->updateMessage(*idx);
        } else {
            // load descriptor
            MessageDevice *dev=new MessageDevice(idx);
            dev->loadDescriptor();
            MessageDescriptor &descriptor=dev->getDescriptor();

            QCString message=dev->rfc822Message();
            DateClass rcvTime((QCString)descriptor.receivedDate.latin1());

            QString newUid = descriptor.uid;

            // Create the new message
            QString messageId = createMessage( message, newUid.latin1(), rcvTime.toQDateTime(),
                                               descriptor.account.latin1(), 0, false,
                                               idx->getUnreadMark() ? MailFolder::NO_FLAGS : MailFolder::Seen);
            // Delete the allocated device
            delete dev;
            return messageId;
        }
    }

    return QString::null;
}

QString LocalMailFolder::moveMessage(IndexClass *idx)
{
    sync();

    if (idx) {
        MailFolder * mailFolder = idx->getParentFolder();
        if (!mailFolder)
            return QString::null;

        if ( MailFolder::IMAP4 == mailFolder->getFolderProtocol() &&
                !idx->isFullMessage() ) {
            QStringList param;

            //IOSyncHandler::ref()->insertToDo(idx->url(), name(), WaitingOperations::MoveObject, param);
            //IMAP4Handler::ref()->updateMessage(*idx);
        } else {
            // load descriptor
            MessageDevice *dev = new MessageDevice(idx);
            dev->loadDescriptor();
            MessageDescriptor &descriptor = dev->getDescriptor();

            QCString message = dev->rfc822Message();
            DateClass rcvTime((QCString)descriptor.receivedDate.latin1());
            // Use the same UID, I don't know if it's good (or not).
            QString newUid = descriptor.uid;
            // Create the new message
            QString messageId = createMessage(
                                    message, newUid.latin1(), rcvTime.toQDateTime(),
                                    descriptor.account.latin1(), 0, false,
                                    idx->getUnreadMark() ? MailFolder::NO_FLAGS : MailFolder::Seen);
            // Delete the allocated device
            delete dev;

            mailFolder->deleteMessage(idx);

            return messageId;
        }
    }

    // return null because the new mail will be created with createMessage()
    return QString::null;
}

bool LocalMailFolder::deleteMessage(IndexClass *idx)
{
    sync();

    // qDebug("localmailfolder: message deleted");

    if (!idx || idx->getParentFolder()!=this)
        return false;

    QString path=name()+"/"+idx->getID();

    if (idx->getUnreadMark())
        decrementUnread();

    indexCollection.remove(idx->getID());

    // this will save the index too
    compact();

    // notify the client
    //  ServerNotifier::thisInstance()->objectDeleted(path);
    //  ServerNotifier::thisInstance()->objectChanged(name());

    //  return getAutoExpunge()?expunge():true;
    return true;
}

bool LocalMailFolder::shouldExpunge()
{
    // get waste configuration
    unsigned wasteSize=0;
    //Settings::instance()->readNumEntry("/Aethera/Folder Management/Waste");

    if (wasteSize<51200)
        wasteSize=102400; // default 100k, at least 50k allowed per folder

    // debug
    qDebug("localmailfolder: computing waste...");

    // compute waste
    unsigned int folderSize=QFileInfo(getMessagesFileName()).size(), usedSize=0;

    QDictIterator<IndexClass> it(indexCollection);
    IndexClass *index;
    while ((index=it.current())) {
        // debug
        // qDebug("localmailfolder: adding waste for index %s...", (const char *)index->getID());

        if (index->isMultipartOnly()) {
            // debug
            // qDebug("\tindex is multipart, adding part lengths");
            for (int i=0;i<index->getPartCount();i++) {
                // debug
                // qDebug("\t\tpart %d: %d", i, index->getPartAt(i)->length);

                usedSize+=index->getPartAt(i)->length;
            }
        } else {
            // debug
            // qDebug("\tindex is uniblock, adding length %d", index->getUniblockLength());

            usedSize+=index->getUniblockLength();
        }

        // debug
        // qDebug("\tadding index descriptor length %d", index->getDescriptorLength());

        usedSize+=index->getDescriptorLength();
        ++it;
    }

    // debug
    // qDebug("localmailfolder: computed waste; results: \tfolder size [%d]\tused [%d]\twaste [%d]\ttolerated waste [%d]", folderSize, usedSize, folderSize-usedSize, wasteSize);

    if ((folderSize-usedSize)>wasteSize)
        return true;

    return false;
}

bool LocalMailFolder::expunge(bool force)
{
    if ( force || shouldExpunge() ) {
        // TODO: Remove the indexes older then a date
        // or the oldest mails till the folder size is good.

        compact();

        return true;
    }

    return false;
}

QString LocalMailFolder::getMessagesFileName() const
{
    return messagesFileName;
}

bool LocalMailFolder::moveMessage(const QStringList & /*urls*/)
{
    /*sync();

    IndexClass * idx = ObjectRequestBroker::thisInstance()->indexReference(urls[0]);
    MailFolder * mailFolder = idx->getParentFolder();
    if( !mailFolder )
      return false;

    QStringList deleteList;

    for(QStringList::ConstIterator it=urls.begin(); it!=urls.end(); ++it)
    {
      idx = mailFolder->indexIDict()[ ObjectRequestBroker::thisInstance()->object(*it) ];
      if(idx)
      {
        if( MailFolder::IMAP4 == mailFolder->getFolderProtocol() &&
            !idx->isFullMessage() )
        {
          QStringList param;
        
          IOSyncHandler::ref()->insertToDo(idx->url(), name(), WaitingOperations::MoveObject, param);
          IMAP4Handler::ref()->updateMessage(*idx);
        }
        else
        {
          // load descriptor
          MessageDevice *dev = new MessageDevice(idx);
          dev->loadDescriptor();
          MessageDescriptor &descriptor = dev->getDescriptor();
        
          QCString message = dev->rfc822Message();
          DateClass rcvTime((QCString)descriptor.receivedDate.latin1());
          // Use the same UID, I don't know if it's good (or not).
          QString newUid = descriptor.uid;
          // Create the new message
          QString messageId = createMessage(
                                message, newUid.latin1(), rcvTime.toQDateTime(),
                                descriptor.account.latin1(), 0, true,
                                idx->getUnreadMark() ? MailFolder::NO_FLAGS : MailFolder::Seen);
          // Delete the allocated device
          delete dev;
        
          if( !messageId.isEmpty() )
            deleteList.append(*it);
        }
      }
    }

    sync();

    if( deleteList.count() )
      mailFolder->deleteMessage(deleteList);
    */
    return true;
}

bool LocalMailFolder::copyMessage(const QStringList & /*urls*/)
{
    sync();

    /* IndexClass * idx = ObjectRequestBroker::thisInstance()->indexReference(urls[0]);
     MailFolder * mailFolder = idx->getParentFolder();
     if( !mailFolder )
       return false;
     
     for(QStringList::ConstIterator it=urls.begin(); it!=urls.end(); ++it)
     {
       idx = mailFolder->indexIDict()[ ObjectRequestBroker::thisInstance()->object(*it) ];
       if(idx)
       {
         if( MailFolder::IMAP4 == mailFolder->getFolderProtocol() &&
             !idx->isFullMessage() )
         {
           QStringList param;
        
           IOSyncHandler::ref()->insertToDo(idx->url(), name(), WaitingOperations::CopyObject, param);
           IMAP4Handler::ref()->updateMessage(*idx);
         }
         else
         {
           // load descriptor
           MessageDevice *dev=new MessageDevice(idx);
           dev->loadDescriptor();
           MessageDescriptor &descriptor=dev->getDescriptor();
        
           QCString message=dev->rfc822Message();
           DateClass rcvTime((QCString)descriptor.receivedDate.latin1());
        
           QString newUid = descriptor.uid;
        
           // Create the new message
           createMessage( message, newUid.latin1(), rcvTime.toQDateTime(),
                          descriptor.account.latin1(), 0, true,
                          idx->getUnreadMark() ? MailFolder::NO_FLAGS : MailFolder::Seen);
        
           // Delete the allocated device
           delete dev;
         }
       }
     }
     
     sync();*/

    return true;
}

bool LocalMailFolder::deleteMessage(const QStringList & /*urls*/)
{
    /* sync();
     
     IndexClass * idx = 0L;
     QStringList deleteList;
     QString path;
     
     for(QStringList::ConstIterator it=urls.begin(); it!=urls.end(); ++it)
     {
       idx = indexCollection[ ObjectRequestBroker::thisInstance()->object(*it) ];
       if( idx )
       {
         path = name()+"/"+idx->getID();
         deleteList.append( path );
        
         if( idx->getUnreadMark() )
           decrementUnread();
        
         indexCollection.remove(idx->getID());
       }
     }
       
     // this will save the index too
     compact();
     
     // notify the client
     if( deleteList.count() )
       ServerNotifier::thisInstance()->objectDeleted( deleteList );
     */
    return true;
}

