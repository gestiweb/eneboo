#ifndef __DIGI_DOC_PKCS11_H__
#define __DIGI_DOC_PKCS11_H__
//==================================================
// FILE:	DigiDocPKCS11.h
// PROJECT:     Digi Doc
// DESCRIPTION: Digi Doc functions for signing using PKCS#11 API
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
//      13.01.2004      Veiko Sinivee
//                      Creation
//==================================================

#include <libdigidoc/DigiDocDefs.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef WIN32
	#include <windows.h>
	#define LIBHANDLE   HANDLE
	#include "pkcs11/cryptoki.h"
#else
	#include <dlfcn.h>   // Linux .so loading interface
	#define LIBHANDLE   void*
	#include "pkcs11/pkcs11.h"
#endif



#include <libdigidoc/DigiDocLib.h>
#include <openssl/ocsp.h>


EXP_OPTION LIBHANDLE initPKCS11Library(const char* libName);
EXP_OPTION void closePKCS11Library(LIBHANDLE pLibrary, CK_SESSION_HANDLE hSession);
EXP_OPTION int calculateSignatureWithEstID(SignedDoc* pSigDoc, SignatureInfo* pSigInfo, 
				int slot, const char* passwd);

EXP_OPTION CK_RV GetSlotIds(CK_SLOT_ID_PTR pSlotids, CK_ULONG_PTR pLen);
EXP_OPTION CK_RV GetTokenInfo(CK_TOKEN_INFO_PTR pTokInfo, CK_SLOT_ID id);
int loadAndTestDriver(const char* driver, LIBHANDLE* pLibrary, CK_SLOT_ID* slotids, 
		      int slots, CK_ULONG slot);
EXP_OPTION CK_RV getDriverInfo(CK_INFO_PTR pInfo);
EXP_OPTION CK_RV GetSlotInfo(CK_SLOT_INFO_PTR pSlotInfo, CK_SLOT_ID id);

//============================================================
// Decrypts RSA encrypted data with the private key
// slot - number of the slot for decryption key. On ID card allways 0
// pin - corresponding pin for the key. On ID card - PIN1
// encData - encrypted data
// encLen - length of encrypted data
// decData - buffer for decrypted data
// encLen - length of buffer. Will be modified by amount of decrypted data
// return error code or ERR_OK
//============================================================
EXP_OPTION int decryptWithEstID(int slot, const char* pin, 
				const char* encData, int encLen, 
				char* decData, int *decLen);

//============================================================
// Locates and reads users certificate from smartcard
// slot - number of the slot for decryption key. On ID card allways 0
// ppCert - address for newly allocated certificate pointer
// return error code or ERR_OK
//============================================================
EXP_OPTION int findUsersCertificate(int slot, X509** ppCert);

#ifdef  __cplusplus
}
#endif

#endif // __DIGI_DOC_PKCS11_H__

