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
              pop3uidjar.h  -  the UID jar for pop3 accounts
                             -------------------
    begin                : Wed Mar 14 19:02:00 EET 2001
    copyright            : (C) 2001 by theKompany (www.thekompany.com>
    author               : Eugen Constantinescu
    email                : eug@thekompany.com
 ***************************************************************************/
#ifndef POP3UIDJAR_H
#define POP3UIDJAR_H

#include <uidjar.h>

class POP3_UIDJar : public UIDJar
{
  public:
    POP3_UIDJar();
    ~POP3_UIDJar();
    /** Return the POP3_UIDJar reference.*/
		static POP3_UIDJar *ref();
		/** Insert an UID in the trash list.*/
		void markForDeletion(const char* uid);
		/** Insert a new UID in receiving list.*/
		void rcvInsert(string uid, unsigned int rcvTime=0);
	private:
	  void setupFiles();
		static POP3_UIDJar *thisInstance;
};


#endif

