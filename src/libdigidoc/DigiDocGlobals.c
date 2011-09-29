//==================================================
// FILE:	DigiDocGlobals.c
// PROJECT:     Digi Doc Encryption
// DESCRIPTION: DigiDoc TSA profiles
// AUTHOR:  Veiko Sinivee, S|E|B IT Partner Estonia
//==================================================
// Copyright (C) AS Sertifitseerimiskeskus
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// GNU Lesser General Public Licence is available at
// http://www.gnu.org/copyleft/lesser.html
//=================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocGlobals.h>
#include <libdigidoc/DigiDocConfig.h>


// default TSAProfile (to be extended)
LPTSAProfile g_current_TSAProfile = 0;

//--------------------------------------------------
// Initializes TSA profile block
// returns pointer to global TSA profile
//--------------------------------------------------
EXP_OPTION TSAProfile* TSAProfile_init()
{
  TSAProfile* p = g_current_TSAProfile;
	
  if(!p) {
    p = (TSAProfile*)malloc(sizeof(TSAProfile));
    if(p) {
      memset(p, 0, sizeof(TSAProfile));
      g_current_TSAProfile = p;
      p->g_bAddSignatureTimeStamp = 1;
      p->g_bAddSigAndRefsTimeStamp = 1;
      p->g_nTsaMaxTsInterval = 1;
	  p->m_nDebugLevel = 0;
      
	  strncpy(p->g_szTsaUrl, "http://ns.szikszi.hu:8080/tsa", sizeof(p->g_szTsaUrl));
	  memset(p->m_szDebugFilePath, 0, sizeof(p->m_szDebugFilePath)); // VS: changed initialization
    }
	// init config store
	initConfigStore(NULL);
  }
  return p;
}

