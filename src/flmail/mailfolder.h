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
                          mailfolder.h  -  the mail folder interface
                             -------------------
    begin                : Mon Mar  5 16:16:00 EET 2001
    copyright            : (C) 2001 by theKompany (www.thekompany.com>
    author               : Eugen Constantinescu
    email                : eug@thekompany.com
 ***************************************************************************/

#ifndef MAILFOLDER_H
#define MAILFOLDER_H

#include <qstring.h>
#include <qcstring.h>
#include <qdir.h>
#include <messageclass.h>
#include <qstringlist.h>
#include <qdict.h>
#include <qptrlist.h>
#include <qmap.h>
#include <qdatetime.h>

class MessageDevice;
class IndexClass;

class MailFolder
{
    /** The index class used for building the folder index.*/
    friend class IndexClass;
public:
    /** Mail folder place.*/
    enum MESSAGES_PLACE {LOCAL=0, REMOTE};
    /** Mail folder protocol types.*/
    enum PROTOCOL {MBOX=0, POP3, IMAP4};
    /** Local mail folder types.*/
    enum TYPES {Normal=100, Deleted, Drafts, Inbox, Outbox, Sent, Templates};
    /** Access rights.*/
    enum ACCESS_RIGHTS {DENIED=0, WRITE_ONLY=1, READ_ONLY=2, ALL=4};

    /** The protocol flags.*/
    enum IMAP4_FLAGS {NO_FLAGS=0, Seen=1, Answered=2, Flagged=4, Draft=8, Recent=16, Delete=32, Other=64};

    MailFolder(const QString &_storageDevice);
    virtual ~MailFolder();

    // Index management
    /** Incremental index save.*/
    void saveIndex(IndexClass *);
    /** Save index file.*/
    void saveIndex();
    /** A fast and easy access way for getting the index.*/
    QDict<IndexClass>& indexIDict();
    /** Get the next index id.*/
    static QString nextIndexID();
    /** Get the last index id.*/
    unsigned long getLastIndexID() const;

    // Attribute public methods
    /** Storage device (mail folder).*/
    QString getStorageDevice() const;
    /** The mapped name of the folder.*/
    QString name() const;
    /** Get the file name with the messages.*/
    virtual QString getMessagesFileName() const=0;
    /** Get the index file name.*/
    QString getIndexFileName() const;
    /** Get the descriptor file name.*/
    QString getDescriptorFileName() const;
    /** Get the folder type.*/
    int getFolderType() const;
    /** Get the folder protocol.*/
    int getFolderProtocol() const;
    /** Get the message place.*/
    int getMessagesPlace() const;
    /** Get the access rights.*/
    int getAccessRights() const;
    /** Set the access rights.*/
    void setAccessRights(int access);
    /** Set auto expunge flag.*/
    static void setAutoExpunge(bool expunge=true);
    /** Get auto expunge flag.*/
    bool getAutoExpunge() const;

    /** Number of messages.*/
    int getMessageCount() const;
    /** Number of unread messages.*/
    int getUnread() const;
    /** Get the prune index count.*/
    unsigned int getPruneIndexCount() const;

    // Add and delete messages from the folder.
    /** Add a message to the folder.*/
    virtual QString createMessage(const QCString &text, const QCString &uid,
                                  const QDateTime &rcvtime, const QString &account,
                                  const MessageClass *parsedMessage = 0, // parsed message, added to avoid parsing second time
                                  bool bSync=false, const unsigned flags=0)=0;

    /** Add a message to the folder.*/
    virtual QString createMessage();

    /** Delete a message from the folder.*/
    virtual bool deleteMessage(IndexClass *idx)=0;
    /** Copy a message in this folder.*/
    virtual QString copyMessage(IndexClass *idx)=0;
    /** Move a message in this folder.*/
    virtual QString moveMessage(IndexClass *idx)=0;

    /** Expunge the folder.*/
    virtual bool expunge(bool force)=0;

    virtual bool moveMessage(const QStringList & urls)=0;
    virtual bool copyMessage(const QStringList & urls)=0;
    virtual bool deleteMessage(const QStringList & urls)=0;

    /** Compacting the folder, level 0 is uncompressed.*/
    virtual bool compact(unsigned level=0);

    /** Folder cache.*/
    QMap<QString, QByteArray> entryCacheBlock();

    /** Increment the unread messages count.*/
    void incrementUnread();
    /** Decrement the unread messages count.*/
    void decrementUnread();
    /** Set the unread messages count.*/
    void setUnread(int _unread=0);
    /** Get a list of folder messages id.*/
    QStringList getEntryList(const QString url=QString::null) const;
    /**
      * Sync the messages from the list.
      */
    virtual void sync()=0;

protected:
    // Internal protected methods
    /** Set all the folder files name.*/
    void setupFiles();
    /** Check if it needs expunge.*/
    virtual bool shouldExpunge()=0;

    // Index methods
    /** Load the index file or build it.*/
    virtual void loadIndex()=0;
    /** Set up related-to index id cross-references.*/
    void crossReferenceIndex(IndexClass *);
    /** Set up parent and related-to index id cross references for a branch.*/
    void crossReferenceIndexSet(IndexClass *);
    /** Check for parent-son relationship and parent index, if appropriate.*/
    void reparentIndex(IndexClass *);
    /** Check for a related parent and parent index, if appropriate.*/
    bool reparentByRelated(IndexClass *);
    /** Check for a common subject and parent index, if appropriate.*/
    bool reparentBySubject(IndexClass *);
    /** Globally search for a given index.*/
    static IndexClass *findIndex(const QString &);
    /** Set the prune index count.*/
    void setPruneIndexCount(unsigned int _pruneIndexCount=0);
    /** Increment the prune index count.*/
    void incrementPruneIndexCount();
    /** Set the last index id.*/
    void setLastIndexID(long unsigned _lastIndexId);
    /** Sync index-message ids.*/
    void syncIndexID(IndexClass *);

    /** Check for a common subject.*/
    bool subjectThreadMatch(QString parentSubject, QString sonSubject);
    /** Translate a message id into an index id, checking all the folders.*/
    static QString translateGlobalMessageID(QString);
    /** Translate a message id into an index id, checking only one folder.*/
    QString translateLocalMessageID(QString);

    // Protected attributes
    /** Index dictionary for a quick data access.*/
    QDict<IndexClass> indexCollection;
    /** Prune index count.*/
    unsigned int pruneIndexCount;
    /** The message list for being created.*/
    QPtrList<MessageDevice> syncMessages;

private:
    // ---------- Private Attributes -------------
    // global data
    /** Global folder list.*/
    static QPtrList<MailFolder> folderList;
    /** Global auto expunge mode.*/
    static bool autoExpunge;
    /** Global index tag.*/
    static long unsigned lastIndexID;

    // files attributes
    /** Folder name.*/
    QString storageDevice;
    /** Index file name.*/
    QString indexFileName;
    /** Descriptor file name.*/
    QString descriptorFileName;
    /** Folder type.*/
    int folderType;
    /** Folder protocol.*/
    int folderProtocol;
    /** Messages place.*/
    int messagesPlace;
    /** Access rights.*/
    int accessRights;

    /** Unread message count.*/
    unsigned int unread;
};

#endif


