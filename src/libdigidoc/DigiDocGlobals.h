#ifndef __DIGIDOC_PROFILE_H__
#define __DIGIDOC_PROFILE_H__
//==================================================
// FILE:	DigiDocGlobals.h
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
//==========< HISTORY >=============================
//      01.06.2006      Arvo Sulakatko
//                      Creation
//      08.06.2006      Veiko Sinivee
//                      Changed implementation and assignment of profile values
//==================================================

#include <libdigidoc/DigiDocDefs.h>

#ifdef __cplusplus
extern "C"
{
#endif	

// TSAProfile
typedef struct tag_TSAProfile *LPTSAProfile;

// TSAProfile
typedef struct tag_TSAProfile
{
	char g_szTsaPolicy[255];
	char g_szTsaUrl[255];
	int g_nTsaMaxTsInterval;	
	int g_bAddSignatureTimeStamp;
	int g_bAddSigAndRefsTimeStamp;

	char m_szDebugFilePath[0xFF];
	int m_nDebugLevel;

} TSAProfile, *LPTSAProfile;


//--------------------------------------------------
// Initializes TSA profile block
// returns pointer to global TSA profile
//--------------------------------------------------
EXP_OPTION TSAProfile* TSAProfile_init();


// default TSAProfile (to be extended)
extern LPTSAProfile g_current_TSAProfile;



#ifdef __cplusplus
}
#endif	 

#endif	// __DIGIDOC_PROFILE_H__

