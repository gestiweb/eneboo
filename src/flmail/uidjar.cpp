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
                uidjar.cpp  -  the UID jar interface class 
                             -------------------
    begin                : Wed Mar 14 19:02:00 EET 2001
    copyright            : (C) 2001 by theKompany (www.thekompany.com>
    author               : Eugen Constantinescu
    email                : eug@thekompany.com
 ***************************************************************************/

#include <qglobal.h>
#include <uidjar.h>
#include <stdio.h>

const int UIDJar::UID_LIMIT = 20000;

UIDJar::UIDJar()
{
}
        
UIDJar::~UIDJar()
{
}
       
void UIDJar::loadLists()
{	
  // load the received UID
  ifstream rcvFile(rcvFileName.c_str());
  
//  qDebug("Load the UID from file:%s", rcvFileName.c_str());
  
  string uidString;
  UIDINFO uidData;
    
  if( rcvFile )
  {
    while( rcvFile )
    {
      // init
      uidString="";
      uidData.info="";
      uidData.rcvTime=0;
      
      rcvFile >> uidString;
      rcvFile >> uidData.info;
			rcvFile >> uidData.rcvTime;
			// insert the new UID in the map
			if( uidString.size() )
        rcvList.insert(UIDValuePair(uidString, uidData));
    }
    rcvFile.close();
  }

//  qDebug("********* UIDJAR ********* = %u", rcvList.size());
    
  // load the deleted UID
  ifstream delFile(delFileName.c_str());
  if( delFile )
  {
    while( delFile )
    {
      // init
      uidString="";
      uidData.info="";
      uidData.rcvTime=0;
      
      delFile >> uidString;
      delFile >> uidData.info;
			delFile >> uidData.rcvTime;
			// A new UID is ready to be deleted :)
			if( uidString.size() )
        trashList.insert(UIDValuePair(uidString, uidData));
    }
    delFile.close();
  }
}

void UIDJar::saveLists()
{
//  qDebug("********* UIDJAR SAVE ********* = %u", rcvList.size());
    
  ofstream rcvFile(rcvFileName.c_str());
  if( rcvFile )
  {
		for(UIDMap::iterator it=rcvList.begin(); it!=rcvList.end(); ++it)
		{
      rcvFile << (*it).first;
			rcvFile << " ";
      rcvFile << (*it).second.info;
			rcvFile << " ";
			rcvFile << (*it).second.rcvTime;
			rcvFile << "\n";
		}
    rcvFile.close();
  }
  		
  ofstream delFile(delFileName.c_str());
  if( delFile )
  {
		for(UIDMap::iterator it=trashList.begin(); it!=trashList.end(); ++it)
		{
      delFile << (*it).first;
			delFile << " ";
      delFile << (*it).second.info;
			delFile << " ";
			delFile << (*it).second.rcvTime;
			delFile << "\n";
		}
    delFile.close();
  }
}

