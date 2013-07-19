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
                          mailinfo.h  -  description
                             -------------------
    begin                : Thu Jun 13 2002
    copyright            : (C) 2002 by theKompany.com
    email                : eug@thekompany.com
 ***************************************************************************/

#ifndef MAILINFO_H
#define MAILINFO_H

#include <qdict.h>
#include <qintdict.h>
#include <qlist.h>
#include <qptrlist.h>

/**A usefull class for mail protocols.
  *@author Eugen C.
  */

class MailInfo
{
  public:
    MailInfo(const QString mailUID=QString::null, 
             unsigned long  mailRetr=0, 
             unsigned long  mailSize=0);
    virtual ~MailInfo();
    
  QString       uid;
  unsigned long  retr;
  unsigned long  size;
};

typedef QDict<MailInfo> UIDMailList;
typedef QDictIterator<MailInfo> UIDIterator;
typedef QIntDict<MailInfo> ListMailList;
typedef QIntDictIterator<MailInfo> ListIterator;
typedef QPtrList<MailInfo> RetrMailList;
typedef QPtrListIterator<MailInfo> RetrIterator;

#endif
