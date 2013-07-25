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

#include <qdatastream.h>
#include <accounts.h>
#include <qsettings.h>
#include <qsettingsextra.h>
#include <names.h>
#include <qstring.h>
#include <stdio.h>

#include <settings.h>
#include <mimecodec.h>

Account::Account()
  : type( Account::UNDEFINE ),
    auth( Account::NONE ),
    haspassword(false),
    port(0),
    ssl(false),
    tls(false),
    useCertificates(false),
    s_type( Account::Sendmail ),
    s_auth( Account::NONE ),
    s_haspassword(false),
    s_port(0),
    s_ssl(false),
    s_tls(false),
    s_useCertificates(false),
    leave(true),
    sync(true),
    sorted(true),
    enabled(true),
    getdisabled(false),
    passentered(true),
    persistence(true),
    groupware(false),
    groupType( Account::Citadel ),
    max_action( Account::ask ),
    max_dl(0)
{
}

Account::Account(const Account & acc)
  : accname( acc.accname ),
    name( acc.name ),
    type( acc.type ),
    auth( acc.auth ),
    server( acc.server ),
    user( acc.user ),
    haspassword( acc.haspassword ),
    password( acc.password ),
    port( acc.port ),
    ssl( acc.ssl ),
    tls( acc.tls ),
    useCertificates(acc.useCertificates),
    s_type( acc.s_type ),
    s_auth( acc.s_auth ),
    smtp( acc.smtp ),
    s_user( acc.s_user ),
    s_haspassword( acc.s_haspassword ),
    s_password( acc.s_password ),
    s_port( acc.s_port ),
    s_ssl( acc.s_ssl ),
    s_tls( acc.s_tls ),
    s_useCertificates(acc.s_useCertificates),
    from( acc.from ),
    reply( acc.reply ),
    mbox( acc.mbox ),
    speed( acc.speed ),
    calendarFolder( acc.calendarFolder ),
    contactsFolder( acc.contactsFolder ),
    notesFolder( acc.notesFolder ),
    todoFolder( acc.todoFolder ),
    leave( acc.leave ),
    sync( acc.sync ),
    sorted( acc.sorted ),
    enabled( acc.enabled ),
    getdisabled( acc.getdisabled ),
    passentered( acc.passentered ),
    persistence( acc.persistence ),
    groupware( acc.groupware ),
    groupType( acc.groupType ),
    max_action( acc.max_action ),
    max_dl( acc.max_dl )
{
}

Account::Account(const QString & acc_name)
  : accname( acc_name ),
    type( Account::UNDEFINE ),
    auth( Account::NONE ),
    haspassword(false),
    port(0),
    ssl(false),
    tls(false),
    useCertificates(false),
    s_type( Account::Sendmail ),
    s_auth( Account::NONE ),
    s_haspassword(false),
    s_port(0),
    s_ssl(false),
    s_tls(false),
    s_useCertificates(false),
    leave(true),
    sync(true),
    sorted(true),
    enabled(true),
    getdisabled(false),
    passentered(true),
    persistence(true),
    groupware(false),
    groupType( Account::Citadel ),
    max_action( Account::ask ),
    max_dl(0)
{
  if(acc_name.isEmpty())
  {
    qDebug("ERROR: cannot setup an account with an empty name.");
    return;
  }
//  qDebug("Trying to set up account %s...", (const char *)acc_name);
  
  // Check if the name has already the settings path in it
  QString group = QString("/Aethera/Accounts/") + acc_name + "/";
  
  // receive account type
  QString t = Settings::instance()->readEntry(group + "Type");
  if(t == "POP3")
    type = Account::POP3;
  else if(t == "IMAP4")
    type = Account::IMAP4;
  else
    type = Account::MAILBOX;

  // send account type
  t = Settings::instance()->readEntry(group + "SendType");
  if( "Qmail" == t )
    s_type = Account::Qmail;
  else if( "SMTP" == t )
    s_type = Account::SMTP;
  else
    s_type = Account::Sendmail;

  // receive account auth mode
  t = Settings::instance()->readEntry(group + "AuthType");
  if( "LOGIN" == t )
    auth = Account::LOGIN;
  else if( "PLAIN" == t )
    auth = Account::PLAIN;
  else if( "DIGEST_MD5" == t )
    auth = Account::DIGEST_MD5;
  else if( "CRAM_MD5" == t )
    auth = Account::CRAM_MD5;
  else
    auth = Account::NONE;
  
  // send account auth mode
  t = Settings::instance()->readEntry(group + "SendAuthType");
  if( "LOGIN" == t )
    s_auth = Account::LOGIN;
  else if( "PLAIN" == t )
    s_auth = Account::PLAIN;
  else if( "DIGEST_MD5" == t )
    s_auth = Account::DIGEST_MD5;
  else if( "CRAM_MD5" == t )
    s_auth = Account::CRAM_MD5;
  else
    s_auth = Account::NONE;
  
  // SMTP server and port
  smtp = Settings::instance()->readEntry(group + "Smtp");
  s_port = Settings::instance()->readNumEntry(group + "SPort");

  // SMTP defaults
  if(s_port==0)
    s_port=25;
  if(smtp.isEmpty())
    smtp=Names::systemSMTPServerName();

  // send login
  s_user = Settings::instance()->readEntry(group + "SendUser");

  // login default
  if( s_user.isEmpty() )
    s_user = Names::systemServerLoginName();

  // has password?
  s_haspassword=
    Settings::instance()->readBoolEntry(group + "SendRememberPassword", false);

  // password
  if(s_haspassword)
    s_password = decodePassword( QCString(Settings::instance()->readEntry(group + "SendPassword")) );

  // Send SSL
  s_ssl=
    Settings::instance()->readBoolEntry(group + "SendSSL", false);
    
  // Send TLS
  s_tls=
    Settings::instance()->readBoolEntry(group + "SendTLS", false);
    
  // Send uses SSL Certificates
  s_useCertificates =
    Settings::instance()->readBoolEntry(group + "SendCertificates", false);
    
  // "From" address
  from=Settings::instance()->readEntry(group + "From");

  // "From" default
  if(from.isEmpty())
    from=Names::systemFromAddress();

  // "Reply-To" address
  reply=Settings::instance()->readEntry(group + "Reply To");

  // full user name
  name=Settings::instance()->readEntry(group + "Name");

  // name default
  if(name.isEmpty())
    name=Names::systemUserName();

  // speed
  speed=Settings::instance()->readEntry(group + "Speed").lower();

  // speed default
  if(speed.isEmpty() || !QString("normal, slow, very slow").contains(speed))
    speed="normal";

  // max download
  max_dl=Settings::instance()->readNumEntry(group + "Max download");

  // enabled
  enabled=
    Settings::instance()->readBoolEntry(group + "Enabled", true);

  // max download action
//  QString act=Settings::instance()->readEntry(group + "Max action");
//  max_action=act=="ignore"?ignore:(act=="remove"?remove:ask);
  max_action = Account::ask;

  if(type==MAILBOX)
  {
    // mailbox
    mbox=Settings::instance()->readEntry(group + "Mailbox");

    // mailbox default
    if(mbox.isEmpty())
      mbox=Names::systemMailbox();
  }
  else
  {
    // server
    server=Settings::instance()->readEntry(group + "Server");

    // port
    port=Settings::instance()->readNumEntry(group + "Port");

    // server defaults
    if(server.isEmpty())
      if(type==POP3)
        server=Names::systemPOP3ServerName();
      else
        server=Names::systemIMAP4ServerName();

    // port defaults
    if(!port)
      if(type==POP3)
        port=110;
      else
        port=143;

    // server login
    user=Settings::instance()->readEntry(group + "User");

    // login default
    if(user.isEmpty())
      user=Names::systemServerLoginName();

    // has password?
    haspassword=
      Settings::instance()->readBoolEntry(group + "Remember password", false);

    // password
    if(haspassword)
      password = decodePassword( QCString(Settings::instance()->readEntry(group + "Password")) );

    // SSL
    ssl=
      Settings::instance()->readBoolEntry(group + "SSL", false);
    
    // TLS
    tls=
      Settings::instance()->readBoolEntry(group + "TLS", false);
    
    // SSL Certificates
    useCertificates =
      Settings::instance()->readBoolEntry(group + "Certificates", false);
    
    if(type==POP3) // POP3 specific options
    {
      // persistence
      persistence=Settings::instance()->readBoolEntry(group + "Persistence", true);

      // leave on server
      leave=Settings::instance()->readBoolEntry(group + "Leave on server", true);

      // sync on delete
      sync=Settings::instance()->readBoolEntry(group + "Sync", true);

      // retrieve sorted by size
      sorted=Settings::instance()->readBoolEntry(group + "Sorted", false);
    }
    else // IMAP specific options
    {
      // mailbox
      mbox=Settings::instance()->readEntry(group + "Mailbox");

      // mailbox default
      if(mbox.isEmpty())
        mbox="Inbox";
    }
  }

  groupware = Settings::instance()->readBoolEntry(group + "Kolab", false);
  if(groupware)
  {
    QStringList kolabList = 
      Settings::instance()->readListEntry("/Aethera/Kolab/Accounts", ',');
    
    if( kolabList.end() == kolabList.find( accname ) )
    {
      QStringList citadelList = 
        Settings::instance()->readListEntry("/Aethera/Citadel/Accounts", ',');
      
      if( citadelList.end() != citadelList.find( accname ) )
      {
        groupType = Account::Citadel;
      }
    }
    else
    {
      groupType = Account::Kolab1;
    }
        
    calendarFolder = Settings::instance()->readEntry(group + "Kolab/Calendar");
    contactsFolder = Settings::instance()->readEntry(group + "Kolab/Contacts");
    notesFolder    = Settings::instance()->readEntry(group + "Kolab/Notes");
    todoFolder     = Settings::instance()->readEntry(group + "Kolab/ToDo");
  }
  
  getdisabled=false;
}

Account::~Account()
{
}

void Account::saveSettings()
{
  QString group = QString("/Aethera/Accounts/") + accname + "/";
  
//  qDebug("Save Account Path=%s", group.latin1());
  
  // receive account type
  switch( type )
  {
    case Account::POP3 :
      Settings::instance()->writeEntry(group+"Type", "POP3");
      break;
    case Account::IMAP4 :
      Settings::instance()->writeEntry(group+"Type", "IMAP4");
      break;
    default:
      Settings::instance()->writeEntry(group+"Type", "MAILBOX");
      break;
  }
  
  // send account type
  switch( s_type )
  {
    case Account::Qmail :
      Settings::instance()->writeEntry(group+"SendType", "Qmail");
      break;
    case Account::SMTP :
      Settings::instance()->writeEntry(group+"SendType", "SMTP");
      break;
    case Account::Sendmail :
    default:
      Settings::instance()->writeEntry(group+"SendType", "Sendmail");
      break;
  }
  
  // receive account auth type
  switch( auth )
  {
    case LOGIN:
      Settings::instance()->writeEntry(group+"AuthType", "LOGIN");
      break;
    case PLAIN:
      Settings::instance()->writeEntry(group+"AuthType", "PLAIN");
      break;
    case DIGEST_MD5:
      Settings::instance()->writeEntry(group+"AuthType", "DIGEST_MD5");
      break;
    case CRAM_MD5:
      Settings::instance()->writeEntry(group+"AuthType", "CRAM_MD5");
      break;
    case NONE:
    default:
      Settings::instance()->writeEntry(group+"AuthType", "NONE");
      break;
  }
  
  // send account auth type
  switch( s_auth )
  {
    case LOGIN:
      Settings::instance()->writeEntry(group+"SendAuthType", "LOGIN");
      break;
    case PLAIN:
      Settings::instance()->writeEntry(group+"SendAuthType", "PLAIN");
      break;
    case DIGEST_MD5:
      Settings::instance()->writeEntry(group+"SendAuthType", "DIGEST_MD5");
      break;
    case CRAM_MD5:
      Settings::instance()->writeEntry(group+"SendAuthType", "CRAM_MD5");
      break;
    case NONE:
    default:
      Settings::instance()->writeEntry(group+"SendAuthType", "NONE");
      break;
  }
  
  // SMTP server and port
  Settings::instance()->writeEntry(group + "Smtp", smtp);
  Settings::instance()->writeEntry(group + "SPort", s_port);
  
  // send login
  Settings::instance()->writeEntry(group + "SendUser", s_user);

  // has password?
  Settings::instance()->writeEntry(group + "SendRememberPassword", s_haspassword);
  // password
  if( s_haspassword )
    Settings::instance()->writeEntry(group + "SendPassword", QString(encodePassword(s_password)) );

  // Send SSL
  Settings::instance()->writeEntry(group + "SendSSL", s_ssl);
    
  // Send TLS
  Settings::instance()->writeEntry(group + "SendTLS", s_tls);
    
  // Send uses SSL Certificates
  Settings::instance()->writeEntry(group + "SendCertificates", s_useCertificates);
    
  // "From" address
  Settings::instance()->writeEntry(group + "From", from);

  // "Reply-To" address
  Settings::instance()->writeEntry(group + "Reply To", reply);

  // full user name
  Settings::instance()->writeEntry(group + "Name", name);

  // speed
  Settings::instance()->writeEntry(group + "Speed", speed);

  // max download
  Settings::instance()->writeEntry(group + "Max download", max_dl);

  // enabled
  Settings::instance()->writeEntry(group + "Enabled", enabled);
    
  // max download action
//  Settings::instance()->writeEntry(group + "Max action", max_action);

  if(type==MAILBOX)
    Settings::instance()->writeEntry(group + "Mailbox", mbox);
  else
  {
    Settings::instance()->writeEntry(group + "Server", server);
    Settings::instance()->writeEntry(group + "Port", port);

    // server login
    Settings::instance()->writeEntry(group + "User", user);

    // has password?
    Settings::instance()->writeEntry(group + "Remember password", haspassword);
    // password
    if( haspassword )
      Settings::instance()->writeEntry(group + "Password", QString(encodePassword(password)) );

    // SSL
    Settings::instance()->writeEntry(group + "SSL", ssl);
    
    // TLS
    Settings::instance()->writeEntry(group + "TLS", tls);
    
    // SSL Certificates
    Settings::instance()->writeEntry(group + "Certificates", useCertificates);
    
    if(type==POP3) // POP3 specific options
    {
      // persistence
      Settings::instance()->writeEntry(group + "Persistence", persistence);

      // leave on server
      Settings::instance()->writeEntry(group + "Leave on server", leave);
      
      // sync on delete
      Settings::instance()->writeEntry(group + "Sync", sync);

      // retrieve sorted by size
      Settings::instance()->writeEntry(group + "Sorted", sorted);
    }
    else // IMAP specific options
    {
      Settings::instance()->writeEntry(group + "Mailbox", mbox);
      Settings::instance()->writeEntry(group + "Kolab", groupware);
      
      if(groupware)
      {
        Settings::instance()->writeEntry(group + "Kolab/Calendar", calendarFolder);
        Settings::instance()->writeEntry(group + "Kolab/Contacts", contactsFolder);
        Settings::instance()->writeEntry(group + "Kolab/Notes", notesFolder);
        Settings::instance()->writeEntry(group + "Kolab/ToDo", todoFolder);
      }
    }
  }
  
  Settings::forceWrite();
}

void Account::removeSettings()
{
  Settings::instance()->removeGroup( QString("/Aethera/Accounts/") + accname );
  
/*
  QString group = QString("/Aethera/Accounts/") + accname + "/";
  
  qDebug("Remove Account Path = %s", group.latin1());
  
  // receive account type
  Settings::instance()->removeEntry(group+"Type");
  
  // send account type
  Settings::instance()->removeEntry(group+"SendType");
  
  // receive account auth type
  Settings::instance()->removeEntry(group+"AuthType");
  
  // send account auth type
  Settings::instance()->removeEntry(group+"SendAuthType");
  
  // SMTP server and port
  Settings::instance()->removeEntry(group + "Smtp");
  Settings::instance()->removeEntry(group + "SPort");
  
  // send login
  Settings::instance()->removeEntry(group + "SendUser");

  // has password?
  Settings::instance()->removeEntry(group + "SendRememberPassword");
  // password
  Settings::instance()->removeEntry(group + "SendPassword");

  // Send SSL
  Settings::instance()->removeEntry(group + "SendSSL");
    
  // Send TLS
  Settings::instance()->removeEntry(group + "SendTLS");
    
  // Send uses SSL Certificates
  Settings::instance()->removeEntry(group + "SendCertificates");
    
  // "From" address
  Settings::instance()->removeEntry(group + "From");

  // "Reply-To" address
  Settings::instance()->removeEntry(group + "Reply To");

  // full user name
  Settings::instance()->removeEntry(group + "Name");

  // speed
  Settings::instance()->removeEntry(group + "Speed");

  // max download
  Settings::instance()->removeEntry(group + "Max download");

  // enabled
  Settings::instance()->removeEntry(group + "Enabled");
    
  Settings::instance()->removeEntry(group + "Mailbox");
  
  Settings::instance()->removeEntry(group + "Server");
  Settings::instance()->removeEntry(group + "Port");

  // server login
  Settings::instance()->removeEntry(group + "User");

  // has password?
  Settings::instance()->removeEntry(group + "Remember password");
  // password
  Settings::instance()->removeEntry(group + "Password");

  // SSL
  Settings::instance()->removeEntry(group + "SSL");
    
  // TLS
  Settings::instance()->removeEntry(group + "TLS");
    
  // SSL Certificates
  Settings::instance()->removeEntry(group + "Certificates");
    
  if(type==POP3) // POP3 specific options
  {
    // persistence
    Settings::instance()->removeEntry(group + "Persistence");

    // leave on server
    Settings::instance()->removeEntry(group + "Leave on server");
      
    // sync on delete
    Settings::instance()->removeEntry(group + "Sync");

    // retrieve sorted by size
    Settings::instance()->removeEntry(group + "Sorted");
  }
  else // IMAP specific options
  {
    Settings::instance()->removeEntry(group + "Mailbox");
    Settings::instance()->removeEntry(group + "Kolab");
      
    if(groupware)
    {
      Settings::instance()->removeEntry(group + "Kolab/Calendar");
      Settings::instance()->removeEntry(group + "Kolab/Contacts");
      Settings::instance()->removeEntry(group + "Kolab/Notes");
      Settings::instance()->removeEntry(group + "Kolab/ToDo");
    }
  }*/
  
  Settings::forceWrite();
}

QDataStream & operator << (QDataStream & str, const Account & a)
{
  str
    << a.accname
    << a.name
    << a.server
    << a.smtp
    << a.user
    << a.s_user
    << a.password
    << a.s_password
    << a.from
    << a.reply
    << a.mbox
    << a.speed
    << a.calendarFolder
    << a.contactsFolder
    << a.notesFolder
    << a.todoFolder
    << Q_UINT8(a.leave)
    << Q_UINT8(a.sync)
    << Q_UINT8(a.sorted)
    << Q_UINT8(a.enabled)
    << Q_UINT8(a.haspassword)
    << Q_UINT8(a.s_haspassword)
    << Q_UINT8(a.ssl)
    << Q_UINT8(a.s_ssl)
    << Q_UINT8(a.tls)
    << Q_UINT8(a.s_tls)
    << Q_UINT8(a.useCertificates)
    << Q_UINT8(a.s_useCertificates)
    << Q_UINT8(a.getdisabled)
    << Q_UINT8(a.passentered)
    << Q_UINT8(a.persistence)
    << Q_UINT8(a.groupware)
    << Q_UINT8(a.groupType)
    << Q_UINT32(a.port)
    << Q_UINT32(a.s_port)
    << Q_UINT32(a.max_dl)
    << Q_UINT32(a.type)
    << Q_UINT32(a.s_type)
    << Q_UINT32(a.auth)
    << Q_UINT32(a.s_auth)
    << Q_UINT32(a.max_action)
    ;

  return str;
}

QDataStream & operator >> (QDataStream & str, Account & a)
{
  str
    >> a.accname
    >> a.name
    >> a.server
    >> a.smtp
    >> a.user
    >> a.s_user
    >> a.password
    >> a.s_password
    >> a.from
    >> a.reply
    >> a.mbox
    >> a.speed
    >> a.calendarFolder
    >> a.contactsFolder
    >> a.notesFolder
    >> a.todoFolder
    ;

  // Workaround for QDataStream::bool being missing.

  Q_UINT8 tmp;

  str >> tmp;
  a.leave = tmp;

  str >> tmp;
  a.sync = tmp;

  str >> tmp;
  a.sorted = tmp;

  str >> tmp;
  a.enabled = tmp;

  str >> tmp;
  a.haspassword = tmp;

  str >> tmp;
  a.s_haspassword = tmp;

  str >> tmp;
  a.ssl = tmp;

  str >> tmp;
  a.s_ssl = tmp;

  str >> tmp;
  a.tls = tmp;

  str >> tmp;
  a.s_tls = tmp;

  str >> tmp;
  a.useCertificates = tmp;

  str >> tmp;
  a.s_useCertificates = tmp;

  str >> tmp;
  a.getdisabled = tmp;

  str >> tmp;
  a.passentered = tmp;

  str >> tmp;
  a.persistence = tmp;
  
  str >> tmp;
  a.groupware = tmp;
  
  str >> tmp;
  a.groupType = (Account::GroupwareType)tmp;
  
  str
    >> a.port
    >> a.s_port
    >> a.max_dl;

  // Workaround for QDataStream::enum being missing. (of course it is)

  Q_UINT32 tmp2;

  str >> tmp2;
  a.type = Account::ServerType(tmp2);

  str >> tmp2;
  a.s_type = Account::SendServerType(tmp2);

  str >> tmp2;
  a.auth = Account::AuthType(tmp2);

  str >> tmp2;
  a.s_auth = Account::AuthType(tmp2);

//  qDebug("Streamed back account type of %d", a.type);

  str >> tmp2;
  a.max_action = Account::DownloadAction(tmp2);

  return str;
}


QCString Account::encodePassword( QString password )
{
  return QCString( MIMECodec::encodeBase64(password.utf8()) );
}

QString Account::decodePassword( QCString password )
{
  return QString::fromUtf8( MIMECodec::decodeBase64(password) );
}

