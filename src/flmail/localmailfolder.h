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
                  localmailfolder.h  -  the local mail folder class
                             -------------------
    begin                : Mon Mar  5 16:16:00 EET 2001
    copyright            : (C) 2001 by theKompany (www.thekompany.com>
    author               : Eugen Constantinescu
    email                : eug@thekompany.com
 ***************************************************************************/

#ifndef __LocalMailFolder__
#define __LocalMailFolder__

#include <qstring.h>
#include <qcstring.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qdict.h>
#include <qlist.h>
#include <qmap.h>
#include <qdatetime.h>

#include <messageclass.h>
#include <mailfolder.h>

class LocalMailFolder : public MailFolder
{
public:

    LocalMailFolder(const QString &_storageDevice);
    virtual ~LocalMailFolder();

    // message file
    QString getMessagesFileName() const;

    // message generators
    QString createMessage(const QCString &text, const QCString &uid, const QDateTime &rcvtime,
                          const QString &account=QString::null,
                          const MessageClass *parsedMessage = 0,
                          bool bSync=false, const unsigned flags=0);

    // copy/move/delete/expunge
    virtual QString copyMessage(IndexClass *);
    virtual QString moveMessage(IndexClass *);
    virtual bool deleteMessage(IndexClass *);
    virtual bool expunge(bool force=false);

    // processing many messages
    virtual bool moveMessage(const QStringList & urls);
    virtual bool copyMessage(const QStringList & urls);
    virtual bool deleteMessage(const QStringList & urls);

    /** sync the messages from the list.*/
    void sync();

private:

    // methods
    void setupFiles();
    void loadIndex();
    bool shouldExpunge();

    // files
    QString messagesFileName;
};

#endif


