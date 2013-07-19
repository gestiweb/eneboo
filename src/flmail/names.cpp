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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qglobal.h>
#include <names.h>

#ifdef Q_OS_WIN32
//#warning PORT ME
QString Names::systemUserName()
{
	return "user";
}

QString Names::systemLoginName()
{
	return "User Name";
}

QString Names::systemHostName()
{
  return "host";
}
		
QString Names::systemDomainName()
{
  return "domain";
}

QString Names::systemHostDomainName()
{
	QString hostdomain;
	QString domain=Names::systemDomainName();
	if(domain.isEmpty())
		hostdomain=Names::systemHostName();
	else
		hostdomain=Names::systemHostName()+"."+Names::systemDomainName();
	
	return hostdomain;
}

QString Names::systemPOP3ServerName()
{
	return systemHostName();
}

QString Names::systemIMAP4ServerName()
{
	return systemHostName();
}

QString Names::systemSMTPServerName()
{
	return systemHostName();
}

QString Names::systemMailbox()
{
	return "mailbox";
}

QString Names::systemServerLoginName()
{
	return systemLoginName();
}

QString Names::systemFullFromAddress()
{
	// we prefer user@domain instead of user@host.domain
	
	QString from=systemUserName()+" <"+systemLoginName()+"@"+systemHostDomainName()+">";
	
	return from;
}

QString Names::systemFromAddress()
{
	QString from=systemLoginName()+"@"+systemHostDomainName();
	
	return from;
}

QString Names::systemReplyAddress()
{
	return systemFromAddress();
}

QString Names::systemBugReportAddress()
{
	QString bugs="aethera@thekompany.com"; 
	
	return bugs;
}

#else // win32

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>

QString Names::systemUserName()
{
	QString name;
	struct passwd *pw=getpwuid(getuid());
	if(pw)
		name=pw->pw_gecos;
	else
		name="user";
	
	return name;
}

QString Names::systemLoginName()
{
	QString user;
	struct passwd *pw=getpwuid(getuid());
	if(pw)
		user=pw->pw_name;
	else
		user="User Name";
	
	return user;
}

QString Names::systemHostName()
{

	char buf[50];
	struct utsname ubuf;

	QString host;

	if(uname(&ubuf)==-1)
	{
		if(gethostname(buf, 50)==0)
			host=buf;
	}
	else
	{
		host=ubuf.nodename;
	}

	if(host.isEmpty())
		host=getenv("HOSTNAME");
		
	return host;
}
		
QString Names::systemDomainName()
{
	char buf[256];
	struct utsname ubuf;
		
	QString domain;

#ifdef OS_FREEBSD
	if(getdomainname(buf, 255)==0)
		domain=buf;
#elif defined ( _QTMAC_ )
	if(getdomainname(buf, 255)==0)
		domain=buf;
#else
	if(uname(&ubuf)==-1)
	{
		if(getdomainname(buf, 255)==0)
			domain=buf;
	}
	else
	{
#ifdef __USE_GNU
		domain=ubuf.domainname;
#else
		domain=ubuf.__domainname;
#endif
	}
#endif
	
	return domain;
}

QString Names::systemHostDomainName()
{
	QString hostdomain;
	QString domain=Names::systemDomainName();
	if(domain.isEmpty())
		hostdomain=Names::systemHostName();
	else
		hostdomain=Names::systemHostName()+"."+Names::systemDomainName();
	
	return hostdomain;
}

QString Names::systemPOP3ServerName()
{
	// here we should query the KDE default POP3 server, when they'll publish the API
	
	return systemHostName();
}

QString Names::systemIMAP4ServerName()
{
	// ..KDE
	
	return systemHostName();
}

QString Names::systemSMTPServerName()
{
	// ..KDE
	
	return systemHostName();
}

QString Names::systemMailbox()
{
	QString mailbox="/var/spool/mail/"+systemLoginName();
	
	return mailbox;
}

QString Names::systemServerLoginName()
{
	// ..KDE
	
	return systemLoginName();
}

QString Names::systemFullFromAddress()
{
	// .. KDE
	
	// we prefer user@domain instead of user@host.domain
	
	QString from=systemUserName()+" <"+systemLoginName()+"@"+systemHostDomainName()+">";
	
	return from;
}

QString Names::systemFromAddress()
{
	// .. KDE
	
	QString from=systemLoginName()+"@"+systemHostDomainName();
	
	return from;
}

QString Names::systemReplyAddress()
{
	// ..KDE
	
	return systemFromAddress();
}

QString Names::systemBugReportAddress()
{
	// this is temporary
	
	QString bugs="aethera@thekompany.com"; 
	
	return bugs;
}

#endif // WIN32

