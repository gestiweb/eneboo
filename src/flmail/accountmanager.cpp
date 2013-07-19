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
#include "accountmanager.h"

#include <qsettings.h>
#include <qdir.h>
#include <qfile.h>
#include <qregexp.h>

#include <settings.h>
//#include <appstatus.h>

//#include <pop3handler.h>
//#include <imap4handler.h>
//#include <smtphandler.h>

AccountManager *AccountManager::thisInstance;

AccountManager *AccountManager::ref()
{
  if(!thisInstance)
    thisInstance=new AccountManager;
  
  return thisInstance;
}

AccountManager::AccountManager()
{
  loadAccounts();
}

AccountManager::~AccountManager()
{
  saveAccounts();
}

void AccountManager::loadAccounts()
{
  qDebug("AccountManager::loadAccounts()");
  
  // cleanup
  accDict.setAutoDelete(true);
  accDict.clear();
  
  // (re)initialize accounts
  defaultAccount=0;

  QString defacc;
  QStringList accList;

  defacc = Settings::instance()->readEntry("/Aethera/Receive Options/Default");
  accList = Settings::instance()->readListEntry("/Aethera/Receive Options/Accounts", ',');

  for(QStringList::Iterator it = accList.begin(); it != accList.end(); ++it)
  {
    Account * t = new Account(*it);

    if ( (*it) == defacc)
    {
      defaultAccount = t;
    }

    if( t )
    {
      accDict.insert((*it), t);
    }
  }
}

void AccountManager::saveAccounts()
{
  for(QDictIterator<Account> it(accDict); it.current(); ++it)
    it.current()->saveSettings();
}

QString AccountManager::getAccountHostName(QString acc)
{
  QString ret;
  Account *a=accDict[QString(acc)];
  if(a)
    ret=a->server;
  return ret;
}

QString AccountManager::getAccountLogin(QString acc)
{
  QString ret;

  Account *a=accDict[QString(acc)];
  if(a)
    ret=a->user;
  return ret;
}

QString AccountManager::getAccountPassword(QString acc)
{
  QString ret;
  Account *a=accDict[QString(acc)];
  if(a)
    ret=a->password;
  return ret;
}

int AccountManager::getAccountPort(QString acc)
{
  int ret=0;
  Account *a=accDict[QString(acc)];
  if(a)
    ret=a->port;
  return ret;
}

Account *AccountManager::getDefaultAccount()
{
  return defaultAccount;
}

Account *AccountManager::getAccount(QString accname)
{
  return accDict[accname];
}

QStringList AccountManager::getAccountNames(int iType)
{
  QStringList ret;
  QDictIterator<Account> accit(accDict);
  while(accit.current())
  {
    if( iType==Account::UNDEFINE || iType==accit.current()->type )
      ret.append(accit.currentKey());
    ++accit;
  }
  return ret;
}

bool AccountManager::addAccount(Account *acc)
{
  if(acc)
  {
    {
      QStringList accList =
        Settings::instance()->readListEntry("/Aethera/Receive Options/Accounts", ',');

      if( accDict[acc->accname] )
      {
        qDebug("Cannot create a new account because there is allready one with %s name.", acc->accname.latin1());
        return false;
      }
      
      accDict.insert(acc->accname, acc);
      
      // check to see if there is allready an account with this name
      QStringList::Iterator itFind = accList.find( acc->accname );
      if( itFind == accList.end() )
        accList.append(acc->accname);

      if( !defaultAccount )
        defaultAccount = acc;
      
      Settings::instance()->writeEntry("/Aethera/Receive Options/Accounts", accList, ',');
      if( acc->type == Account::IMAP4 &&
          acc->groupware )
      {
        QStringList kolabList = 
          Settings::instance()->readListEntry("/Aethera/Kolab/Accounts", ',');
        
        if( kolabList.end() == kolabList.find( acc->accname ) )
        {
          kolabList.append( acc->accname );
          Settings::instance()->writeEntry("/Aethera/Kolab/Accounts", kolabList, ',');
        }
        
        if( acc->groupType == Account::Citadel )
        {
          QStringList citadelList = 
            Settings::instance()->readListEntry("/Aethera/Citadel/Accounts", ',');
          
          if( citadelList.end() == citadelList.find( acc->accname ) )
          {
            citadelList.append( acc->accname );
            Settings::instance()->writeEntry("/Aethera/Citadel/Accounts", citadelList, ',');
          }
        }
      }
      
      if( acc->type == Account::IMAP4 )
      {
// ---------------------------------------------------------------------------
// This part has to be made by the mail plug in itself
// Anyway the info will not be showed if you don't have the mail plug in loaded
// TODO move this part into the mail plug in
        // create an extra VFS folder
        QStringList subfolders = Settings::instance()->readListEntry("/Aethera/VFS/Mail/Subfolders", ',');
        
        // check if this is already made
        QStringList::Iterator it = subfolders.find(acc->accname);
        if( it == subfolders.end() )
        {
          subfolders.append(acc->accname);
          Settings::instance()->writeEntry("/Aethera/VFS/Mail/Subfolders", subfolders, ',');
          
          QString imapVFSFolder="/Aethera/VFS/Mail/";
          imapVFSFolder+=acc->accname+"/";
    
          Settings::instance()->writeEntry(imapVFSFolder+"Binding", "thisInstance");
          Settings::instance()->writeEntry(imapVFSFolder+"Type", "imap");
          Settings::instance()->writeEntry(imapVFSFolder+"Flags", "c");
          Settings::instance()->writeEntry(imapVFSFolder+"MIME type", "");
          Settings::instance()->writeEntry(imapVFSFolder+"View type", "genmail");
          Settings::instance()->writeEntry(imapVFSFolder+"Account", acc->accname);
          // add the default INBOX folder
          QString serverPath=QString("imap_")+acc->accname;
          Settings::instance()->writeEntry(imapVFSFolder+"Subfolders", "Inbox");
    
          // create the default IMAP mailbox - INBOX
          Settings::instance()->writeEntry(imapVFSFolder+"Inbox/Binding", "thisInstance");
          Settings::instance()->writeEntry(imapVFSFolder+"Inbox/Server Path", serverPath);
          Settings::instance()->writeEntry(imapVFSFolder+"Inbox/Type", "imap");
          Settings::instance()->writeEntry(imapVFSFolder+"Inbox/Flags", "c");
          Settings::instance()->writeEntry(imapVFSFolder+"Inbox/View type", "mail");
          Settings::instance()->writeEntry(imapVFSFolder+"Inbox/MIME type", "text/rfc822");
          Settings::instance()->writeEntry(imapVFSFolder+"Inbox/Account", acc->accname);
          Settings::instance()->writeEntry(imapVFSFolder+"Inbox/Mailbox", "INBOX");
          
          // create an extra IMAP folder
          QString mdir = Settings::instance()->readEntry("/Aethera/Directory/AetheraDir");
          QDir top( mdir );
          top.mkdir(serverPath);
          // create the IMAP config file
          QFile f( mdir+"/"+serverPath+"/"+"config" );
          f.open( IO_WriteOnly );
          QDataStream s( &f );
          s << Settings::instance()->readEntry(imapVFSFolder+"Inbox/Account");
          s << Settings::instance()->readEntry(imapVFSFolder+"Inbox/Mailbox");
          f.close();
// ---------------------------------------------------------------------------
        }
      }
    }
    
    acc->saveSettings();
    return true;
  }
  
  return false;
}

bool AccountManager::deleteAccount(Account *acc)
{
  if(acc && stopAccount(acc))
  {
    if( acc == defaultAccount )
      defaultAccount = 0;
  
    if( acc->type == Account::IMAP4 &&
        acc->groupware )
    {
      QStringList kolabList = 
        Settings::instance()->readListEntry("/Aethera/Kolab/Accounts", ',');
        
      kolabList.remove( acc->accname );
      Settings::instance()->writeEntry("/Aethera/Kolab/Accounts", kolabList, ',');
      
      if( acc->groupType == Account::Citadel )
      {
        QStringList citadelList = 
          Settings::instance()->readListEntry("/Aethera/Citadel/Accounts", ',');
          
        citadelList.remove( acc->accname );
        Settings::instance()->writeEntry("/Aethera/Citadel/Accounts", citadelList, ',');
      }
    }
    
    QString accountName = acc->accname;
    
    QStringList accList =
      Settings::instance()->readListEntry("/Aethera/Receive Options/Accounts", ',');
    accList.remove(accountName);
    Settings::instance()->writeEntry("/Aethera/Receive Options/Accounts", accList, ',');
    acc->removeSettings();
    
    accDict.remove(accountName);
    
    return true;
  }
  
  return false;
}

bool AccountManager::deleteAccount(QString name)
{
  if(!name.isEmpty())
  {
    Account* account = accDict[name];
    if( account &&
        stopAccount(account) &&
        deleteAccount(account) )
    {
      return true;
    }
  }
  
  return false;
}

void AccountManager::setDefaultAccount(Account *acc)
{
  if(acc && accDict[acc->accname])
  {
    defaultAccount = acc;
    Settings::instance()->writeEntry("/Aethera/Receive Options/Default", acc->accname);
    Settings::forceWrite();
  }
}

void AccountManager::setDefaultAccount(QString name)
{
  Account * acc = 0;
  if(!name.isEmpty() && (acc=accDict[name]))
  {
    defaultAccount = acc;
    Settings::instance()->writeEntry("/Aethera/Receive Options/Default", acc->accname);
    Settings::forceWrite();
  }
}

bool AccountManager::stopAccount(const QString & name)
{
  bool bRet = false;
  Account * acc = 0;
  if(!name.isEmpty() && (acc=accDict[name]))
  {
    bRet = stopAccount(acc);
  }
  
  return bRet;
}

bool AccountManager::stopAccount(Account * /*acc*/)
{
  return true;
#if 0
  bool bRet = false;
  if( acc &&
      POP3Handler::ref()->stopDelivery(acc) &&
      IMAP4Handler::ref()->stopDelivery(acc) &&
      SMTPHandler::ref()->stopSending(acc) )
  {
    QString text = acc->accname + QObject::tr(" is stopped.");
    AppStatus::instance()->message(AppStatus::Server, AppStatus::Text, AppStatus::Warning, text);
    bRet = true;
  }
  
  return bRet;
#endif
}

bool AccountManager::stopAllAccounts()
{
  bool bRet = true;
  if( accDict.count() )
  {
    for(QDictIterator<Account> it(accDict); it.current() && bRet; ++it)
    {
      bRet = stopAccount(it.current());
    }
    
    if( bRet )
    {
      //AppStatus::instance()->message(AppStatus::Server, AppStatus::Text, AppStatus::Warning, QObject::tr("The email protocols are stopped"));
    }
    else
    {
      // TODO show something here too
    }
  }
  
  return bRet;
}
