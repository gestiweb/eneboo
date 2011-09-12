//==================================================
// FILE:	DigiDocPKCS11.c
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
//      10.02.2004      Integrated
//      26.01.2004      Aare
//                      Removed function signOCSPRequestWithPKCS11onWin
//      13.01.2004      Veiko Sinivee
//                      Creation
//==================================================


#include "DigiDocPKCS11.h"
#include "DigiDocConfig.h"
#include "DigiDocDebug.h"
#include "DigiDocCert.h"
#include "DigiDocGen.h"

#include <stdio.h>
#include <string.h>



static CK_FUNCTION_LIST_PTR ckFunc = 0;

//AA 04/01/26
extern X509_ALGOR* setSignAlgorithm(const EVP_MD * type);

// I suppose there won't be so many slots

#define INVALID_SLOTIID	   1000

//============================================================
// Attempts to load and initialize on PKCS#11 driver DLL.
//============================================================
EXP_OPTION LIBHANDLE initPKCS11Library(const char* libName) 
{	
  LIBHANDLE pLibrary = 0;
  CK_C_GetFunctionList pC_GetFunctionList;
  CK_RV rv;

  // load PKCS#11 driver
  ddocDebug(3, "initPKCS11Library", "Loading driver: %s\n", libName);
#ifdef WIN32
  pLibrary = LoadLibrary((LPCSTR)libName);
#else
  pLibrary = dlopen(libName, RTLD_NOW);
#endif
  if(pLibrary != NULL) {
    // printf("Resolve PKCS#11 function index!\n");
    // Get function pointer to C_GetFunctionList
#ifdef WIN32
    pC_GetFunctionList = (CK_C_GetFunctionList)GetProcAddress(/*(HINSTANCE__*)*/pLibrary, "C_GetFunctionList");
#else
    pC_GetFunctionList = (CK_C_GetFunctionList)dlsym(pLibrary, "C_GetFunctionList");
#endif
    if(pC_GetFunctionList != NULL) {
      ddocDebug(3, "initPKCS11Library", "Getting PKCS#11 func!\n");
      // Get function pointers to all PKCS #11 functions
      rv = (*pC_GetFunctionList)(&ckFunc);
      if(rv == CKR_OK) {
	ddocDebug(3, "initPKCS11Library", "Initializing PKCS#11 library:");
	// Initalize Cryptoki
	rv = (*ckFunc->C_Initialize)(0);
	ddocDebug(3, "initPKCS11Library", "Initlialized: %d", (int)rv);
	if(rv != CKR_OK) {
	  ddocDebug(2, "initPKCS11Library", "Error initializing library!\n");
	  pLibrary = NULL;  // error initializing the library 
	}
      } else {
	ddocDebug(2, "initPKCS11Library", "Error getting PKCS#11 func!");
	pLibrary = NULL;  // error getting function pointers
      }
    } else {
      ddocDebug(2, "initPKCS11Library", "Error resolving PKCS#11 function index!");
      pLibrary = NULL;  // error getting function list function
    }
  } else {
#ifdef WIN32
    ddocDebug(2, "initPKCS11Library", "Error loading driver : %s", libName);
#else
    ddocDebug(2, "initPKCS11Library", "Error loading driver : %s", dlerror());
#endif
  }
  return pLibrary;
}

//============================================================
// Cleanup PKCS#11 library session
//============================================================
EXP_OPTION void closePKCS11Library(LIBHANDLE pLibrary, CK_SESSION_HANDLE hSession)
{
  CK_RV rv;

  // close session
  if(hSession > 0) {
    ddocDebug(3, "closePKCS11Library", "Closing PKCS#11 session!");
    rv = (*ckFunc->C_CloseSession)(hSession);
  }
  // finalize library
  rv = (*ckFunc->C_Finalize)(0);
  // remove .so from memory
  ddocDebug(3, "closePKCS11Library", "Closing PKCS#11 library!\n");
  if(pLibrary)
#ifdef WIN32
    FreeLibrary(/*(HINSTANCE__*)*/pLibrary);
#else
  dlclose(pLibrary);
#endif
}

//============================================================
// Retrieves the slotid list
//============================================================
EXP_OPTION CK_RV GetSlotIds(CK_SLOT_ID_PTR pSlotids, CK_ULONG_PTR pLen)
{
  CK_RV rv;
	
  rv = (*ckFunc->C_GetSlotList)(TRUE, pSlotids, pLen);
  return rv;
}

//============================================================
// Retrieves one tokens info
//============================================================
EXP_OPTION CK_RV GetTokenInfo(CK_TOKEN_INFO_PTR pTokInfo, CK_SLOT_ID id)
{
  CK_RV rv;
	
  rv = (*ckFunc->C_GetTokenInfo)(id, pTokInfo);
  return rv;
}

//============================================================
// Retrieves one slots info
//============================================================
EXP_OPTION CK_RV GetSlotInfo(CK_SLOT_INFO_PTR pSlotInfo, CK_SLOT_ID id)
{
  CK_RV rv;
	
  rv = (*ckFunc->C_GetSlotInfo)(id, pSlotInfo);
  return rv;
}

//============================================================
// Loads the PKCS#11 driver and
// tests if the driver loaded correctly
// Returns 0 for ok otherwise error code
//============================================================
int loadAndTestDriver(const char* driver, LIBHANDLE* pLibrary, CK_SLOT_ID* slotids, int slots, CK_ULONG slot)
{
  CK_TOKEN_INFO tokinfo;   
  CK_SLOT_INFO slotinfo;
  CK_ULONG idlen, i, ok;
  CK_RV rv;
  int err = ERR_OK;
	
  // initialize
  *pLibrary = NULL;
  memset(slotids, 0, sizeof(CK_SLOT_ID) * slots);
  // try to load the driver
  *pLibrary = initPKCS11Library(driver);
  if(!(*pLibrary))
    SET_LAST_ERROR_RETURN_CODE(ERR_PKCS_LIB_LOAD);    
  idlen = slots;
  rv = GetSlotIds(slotids, &idlen);
  ddocDebug(3, "loadAndTestDriver", "RV: %d slots: %ld", (int)rv, (long)idlen);
  if (rv != CKR_OK) {
    err = ERR_PKCS_SLOT_LIST;
    SET_LAST_ERROR(err);
  }
  if ((slot < 0) || (slot >= idlen)) {
    err = ERR_PKCS_WRONG_SLOT;
    SET_LAST_ERROR(err);
  }
  // it's useful to test DLL load status this way:
  ok = 0;
  for(i = 0; i < idlen; i++) {
    rv = GetSlotInfo(&slotinfo, slotids[i]);
    if(slotinfo.flags & CKF_TOKEN_PRESENT) {
      ddocDebug(3, "loadAndTestDriver", "Read Token: %ld", (long)i);
      rv = GetTokenInfo(&tokinfo, slotids[i]); // if !CKR_OK test
      tokinfo.label[31] = 0;
      ddocDebug(3, "loadAndTestDriver", "RV: %d Token: %s", (int)rv, tokinfo.label);
      if(rv != CKR_OK)
	slotids[i] = INVALID_SLOTIID; // set bad slotids to 0
      else
	ok++; // count the good slots
    } else {
      slotids[i] = INVALID_SLOTIID; // no tokne in this slot
    }
  }
  // fill other slotid's with invalid slotid
  for(i = idlen; i < (CK_ULONG)slots; i++)
    slotids[i] = INVALID_SLOTIID;
  if(ok < slot) {
    err = ERR_PKCS_CARD_READ; // if not enough good slots
    SET_LAST_ERROR(err);
  }
  // in case of error try to unload the module and notify caller
  if (err) {
    if (*pLibrary)
      closePKCS11Library(*pLibrary, 0);
    *pLibrary = NULL;
  }
  return err;
}

//============================================================
// Opens smart card session.
// slotId - id of the sert slot
// pin - card pin
// return session id or -1 for failure
//============================================================
CK_SESSION_HANDLE OpenSession(CK_SLOT_ID slotId, const char *pin)
{ 
  CK_SESSION_HANDLE hSession = 0;
  /*
    Ainult SERIAL_SESSION toetatud, Digiallkirja andvat privaatvo~tit kasutav sessioon
    on initsialiseeritud CKS_RO_USER_FUNCTIONS staatusesse, seda on tehtud Netscape lolli-
    tamiseks, et see igal vo~imalikul ja vo~imatul juhul ei ku"siks pin'i vo~tme jaoks
    mida ta niiehknii kasutada ei oska. Selle jaoks tuleb vajaduse korral ikka C_Login 
    va"lja kutsuda.
  */
  CK_RV rv = (*ckFunc->C_OpenSession)(slotId, CKF_SERIAL_SESSION,0,0,&hSession);
  if(rv == CKR_OK && pin) { // don't login if pin is null. Session can be used also to read certs.
    /* Kommentaar:  	Ainult CKU_USER toetatud. */
    rv = (*ckFunc->C_Login)(hSession, CKU_USER, (unsigned char*)pin, strlen(pin));
    if(rv != CKR_OK)
      hSession = CK_INVALID_HANDLE; // mark session bad!
  }
  // Return the session handle and exit
  return hSession;
}

//============================================================
// Sign a string using a private key referred by the supplied handle.
// hSession - card session handle
// hPrivateKey - private key handle
// Signature - buffer for signature
// ulSignatureLen - signature buffer length
// sigData - data to be  signed
//============================================================
CK_RV SignData(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hPrivateKey, 
			  CK_BYTE* Signature, CK_ULONG* ulSignatureLen, 
			  CK_BYTE* sigData, CK_ULONG dataLen)
{
  // Set up mechanism for PKCS #1 signing
  CK_MECHANISM Mechanism = { CKM_RSA_PKCS, 0, 0 };
  // Initiate the signature operation
  CK_RV rv = (*ckFunc->C_SignInit)(hSession,&Mechanism,hPrivateKey);
  if(rv == CKR_OK) {
    // Kommentaar: Realiseeritud meetodi CKM_RSA_PKCS jaoks.
    rv = (*ckFunc->C_Sign)(hSession, sigData, dataLen, 
			   Signature, ulSignatureLen);
    if(rv != CKR_OK)
      ddocDebug(1, "SignData", "Error signing - sess: %uld pkey: %uld slen: %uld dlen: %uld. RV = %uld\n", 
		hSession, hPrivateKey, ulSignatureLen, dataLen, rv);
  } // if C_SignInit
  else
    ddocDebug(1, "SignData", "Error initing sign session. RV = %ld", rv);
  ddocDebug(3, "SignData", "RV = %ld", rv);
  return rv;
}


//============================================================
// Locates a certificate on the token on basis of its CKA_LABEL attribute.
// hSession - card session handle
// label - key label
// return objects handle or -1 for failure
//============================================================
CK_OBJECT_HANDLE LocateCertificate(CK_SESSION_HANDLE hSession, 
				   CK_BYTE_PTR certData, CK_ULONG_PTR certLen, 
				   char idData[20][20], CK_ULONG idLen[20],
				   int* pSelKey)
{
  CK_OBJECT_HANDLE Objects[10];
  CK_ULONG ulObjectCount = sizeof(Objects)/sizeof(CK_OBJECT_HANDLE), i, j;
  CK_BYTE buf1[20];
  CK_OBJECT_HANDLE hCert = CK_INVALID_HANDLE;
  CK_RV rv;

  // Set up a template to search for Certificate token objects
  // in the given session and thus slot
  CK_OBJECT_CLASS ObjClass = CKO_CERTIFICATE;	
  CK_ATTRIBUTE Template1[] = {
    { CKA_CLASS,     &ObjClass,  sizeof(ObjClass) },
    { CKA_ID,	     (void*)0,  0 }
  };
  CK_ATTRIBUTE Template3[] = {
    { CKA_CLASS,     &ObjClass,  sizeof(ObjClass) },
  };
  CK_ATTRIBUTE Template2[] = {
    { CKA_VALUE,      (void*)0,	*certLen  },
    { CKA_ID,	     (void*)0,  0 }
  };
  CK_ULONG ulCount = 0;
  
  if(idLen)
    ulCount = sizeof(Template1)/sizeof(CK_ATTRIBUTE);
  else
    ulCount = sizeof(Template3)/sizeof(CK_ATTRIBUTE);
  *certLen = 0;
  /*
  ** Initiate the object search
  C_FindObjectsInit initializes a search for token and session objects that match a template.
  hSession is the sessions handle; pTemplate points to a search template that specifies the
  attribute values to match; ulCount is the number of attributes in the search template. The
  matching criterion is an exact byte-for-byte match with all attributes in the template. 
  To find all objects, set ulCount to 0.
  */
  
  for(j = 0; j < 20 && !(*certLen); j++) {
    if(idLen && idLen[j]) {
      memset(buf1, 0, sizeof(buf1));
      memcpy(buf1, idData[j], idLen[j]);
      Template1[1].pValue = buf1;
      Template1[1].ulValueLen = idLen[j];
    }
    rv = (*ckFunc->C_FindObjectsInit)(hSession, (idLen ? Template1 : Template3), 1); //ulCount);
    if(rv==CKR_OK) {            
      rv = (*ckFunc->C_FindObjects)(hSession,Objects,ulObjectCount, &ulObjectCount);
      ddocDebug(3, "LocateCertificate", "search key-id: %s, found: %ld rv: %ld", buf1, ulObjectCount, rv);
      if(rv==CKR_OK) {
	// pick the first cert that is valid
	// list and ignore any other
	for(i = 0; i < ulObjectCount; i++) {
	  hCert = Objects[i];
	  memset(certData, 0, *certLen);
	  ulCount = sizeof(Template2) / sizeof(CK_ATTRIBUTE);
	  // get cert length
	  *certLen = 0;
	  Template2[1].pValue = buf1;
	  Template2[1].ulValueLen = sizeof(buf1);
	  rv = (*ckFunc->C_GetAttributeValue)(hSession, hCert, Template2, ulCount);
	  ddocDebug(3, "LocateCertificate", "cert-id: %s", buf1);
	  if(rv == CKR_OK && (!idLen ||
	     (idLen && !memcmp(idData[j], buf1, idLen[1])))) {
	      *certLen = Template2[0].ulValueLen;
	      // now get cert data
	      Template2[0].pValue = certData;
	      rv = (*ckFunc->C_GetAttributeValue)(hSession, hCert, Template2, ulCount);
	      ddocDebug(3, "LocateCertificate", "cert-len: %ld", *certLen);
	      if(*certLen > 0 && pSelKey) {
		*pSelKey = j;
		break; // found it
	      }
	    
	  } // if rv == CKR_OK
	} // for i < ulObjectCount
      } // if rv
    } // if rv
    rv = (*ckFunc->C_FindObjectsFinal)(hSession);
  } // for j
  if(hCert == CK_INVALID_HANDLE)
    *certLen = 0;
  return hCert;
}

//============================================================
// Locates a private key on the token on basis of its CKA_LABEL attribute.
// hSession - card session handle
// idData - address of an array of label buffers
// idlen - array of label lengths
// return objects handle or -1 for failure
//============================================================
CK_RV LocatePrivateKey(CK_SESSION_HANDLE hSession, char idData[20][20], CK_ULONG idLen[20], CK_OBJECT_HANDLE_PTR hKeys)
{
    CK_OBJECT_HANDLE Objects[10];
    CK_RV rv;
    CK_ULONG ulObjectCount = sizeof(Objects)/sizeof(CK_OBJECT_HANDLE), i;
    // CK_BBOOL IsTrue = TRUE;
    CK_OBJECT_HANDLE hPrivateKey = CK_INVALID_HANDLE;
    // Set up a template to search for all Private Key tokens 
    // Given the session context, that is associated with
    // one slot we will find only one object
    CK_OBJECT_CLASS ObjClass = CKO_PRIVATE_KEY;
    char buf1[20];
    CK_ATTRIBUTE Template1[] = {
      { CKA_CLASS,            &ObjClass,  sizeof(ObjClass)    }
    };	
    CK_ATTRIBUTE Template2[] = {
      { CKA_ID,      (void*)0,	idLen[0] }
    };
    CK_ULONG ulCount = sizeof(Template1) / sizeof(CK_ATTRIBUTE);
	
    /*
    ** Initiate the object search
    C_FindObjectsInit initializes a search for token and session objects that match a template.
    hSession is the sessions handle; pTemplate points to a search template that specifies the
    attribute values to match; ulCount is the number of attributes in the search template. The
    matching criterion is an exact byte-for-byte match with all attributes in the template. 
    To find all objects, set ulCount to 0.
    */
    ddocDebug(3, "LocatePrivateKey", "LocatePrivateKey");
    rv = (*ckFunc->C_FindObjectsInit)(hSession,Template1,ulCount);
    if(rv==CKR_OK) {
      // Get list of object handles
      rv = (*ckFunc->C_FindObjects)(hSession,Objects,ulObjectCount, &ulObjectCount);
      ddocDebug(3, "LocatePrivateKey", "Find: %d count: %ld", rv, ulObjectCount);
      if(rv==CKR_OK) {
	// get labels of all possible private keys
	for(i = 0; i < ulObjectCount; i++) {
	  hKeys[i] = Objects[i];
	  ulCount = sizeof(Template2) / sizeof(CK_ATTRIBUTE);
	  // get key id length
	  rv = (*ckFunc->C_GetAttributeValue)(hSession, hKeys[i], Template2, ulCount);
	  if(rv == CKR_OK) {
	    idLen[i] = Template2[0].ulValueLen;
	    // now get key id data
	    Template2[0].pValue = buf1;
	    memset(buf1, 0, sizeof(buf1));
	    rv = (*ckFunc->C_GetAttributeValue)(hSession, hKeys[i], Template2, ulCount);
	    ddocDebug(3, "LocatePrivateKey", "key: %d id %s len: %ld", i, buf1, idLen[i]);
	    memcpy(idData[i], buf1, idLen[i]);
	  }
	} // for i < ulObjectsCount
      }
    }
    // Remember to call C_FindObjectsFinal to terminate the search
    rv = (*ckFunc->C_FindObjectsFinal)(hSession);
    return rv;  
}

int ddocLocateSlotWithSignatureCert(LIBHANDLE pLibrary, CK_SLOT_ID* slotids, 
				    CK_SLOT_ID* pSlotId, char* pLabel, CK_ULONG nLabelLen)
{
  int err = ERR_OK, i, j;
  CK_RV rv;
  CK_SESSION_HANDLE hSession = 0;
  CK_OBJECT_HANDLE objects[10];
  CK_ULONG ulObjectCount = sizeof(objects)/sizeof(CK_OBJECT_HANDLE), ulCount = 0, l1 = 0;
  CK_OBJECT_CLASS ObjClass = CKO_CERTIFICATE;	
  CK_ATTRIBUTE Template1[] = {
    { CKA_CLASS,     &ObjClass,  sizeof(ObjClass) }
  };
  CK_ATTRIBUTE Template2[] = {
    { CKA_VALUE,     (void*)0,	l1  }
    //,   { CKA_ID,	     (void*)pLabel,  nLabelLen }
  };
  char buf1[3000];
  X509* pCert;

  RETURN_IF_NULL_PARAM(pLibrary);
  RETURN_IF_NULL_PARAM(pSlotId);
  RETURN_IF_NULL_PARAM(slotids);
  RETURN_IF_NULL_PARAM(pLabel);
  // mark as not found
  *pSlotId = INVALID_SLOTIID;
  ddocDebug(3, "ddocLocateSlotWithSignatureCert", "Driver handle: %d err = %d", pLibrary, err);
  // now check every slot
  for(i = 0; i < 20; i++) {
    if(slotids[i] != INVALID_SLOTIID) {
      ddocDebug(3, "ddocLocateSlotWithSignatureCert", "Checking slot nr: %d id: %d", i, slotids[i]);
      // open session to slot but no login since we just need the cert
      rv = (*ckFunc->C_OpenSession)(slotids[i], CKF_SERIAL_SESSION,0,0,&hSession);
      ddocDebug(3, "ddocLocateSlotWithSignatureCert", "Login rv: %ld session: %ld", rv, hSession);
      if(rv == CKR_OK) {
	ulCount = sizeof(Template1)/sizeof(CK_ATTRIBUTE);
	rv = (*ckFunc->C_FindObjectsInit)(hSession, Template1, ulCount);
	if(rv == CKR_OK) {
	  rv = (*ckFunc->C_FindObjects)(hSession, objects, ulObjectCount, &ulObjectCount);
	  ddocDebug(3, "ddocLocateSlotWithSignatureCert", "slot id: %ld, objects: %ld", 
		    slotids[i], ulObjectCount);
	  if(rv == CKR_OK && ulObjectCount > 0) {
	    ulCount = sizeof(Template2) / sizeof(CK_ATTRIBUTE);
	    for(j = 0; j < (int)ulObjectCount; j++) {
	      l1 = sizeof(buf1);
	      memset(buf1, 0, l1);
	      //Template2[0].pValue = 0; // check length first
	      rv = (*ckFunc->C_GetAttributeValue)(hSession, objects[j], Template2, ulCount);
	      if(rv == CKR_OK && Template2[0].ulValueLen < sizeof(buf1)) {
		l1 = Template2[0].ulValueLen;
		// now get cert data
		Template2[0].pValue = buf1;
		rv = (*ckFunc->C_GetAttributeValue)(hSession, objects[j], Template2, ulCount);
		ddocDebug(3, "ddocLocateSlotWithSignatureCert", "slot id: %ld, object: %ld cert-len: %ld rv: %ld", 
			  slotids[i], j, l1, rv);
		pCert = 0;
		err = ddocDecodeX509Data(&pCert, (const byte*)buf1, l1);
		if(pCert) 
		ddocDebug(3, "ddocLocateSlotWithSignatureCert", 
			  "flags: %ld kusage: %ld xkusage: %ld ku-dig-sign: %d ku-non-repud: %d ku-ex: %d non-rep-ex: %d",
			  pCert->ex_flags, pCert->ex_kusage, pCert->ex_xkusage,
			  pCert->ex_kusage & X509v3_KU_DIGITAL_SIGNATURE,
			  pCert->ex_kusage & X509v3_KU_NON_REPUDIATION,
			  pCert->ex_xkusage & X509v3_KU_DIGITAL_SIGNATURE,
			  pCert->ex_xkusage & X509v3_KU_NON_REPUDIATION);
		if(pCert)
		  X509_free(pCert);
		
	      }
	    } // for j < ulObjectCount
	  } // if found any certs
	} // if find-init ok
	rv = (*ckFunc->C_FindObjectsFinal)(hSession);
      } // if login ok
      rv = (*ckFunc->C_CloseSession)(hSession);      
    } // if slotid
  } // for i 

  return err;
}

extern void dumpInFile(const char* fileName, const char* data);

//============================================================
// Calculates and stores a signature for this SignatureInfo object
// Uses EstEID card to sign the info
// pSigInfo - signature info object
// nSigType - signature type code
// keyfile - RSA key file
// passwd - key password
// certfile - certificate file
//============================================================
EXP_OPTION int calculateSignatureWithEstID(SignedDoc* pSigDoc, SignatureInfo* pSigInfo, 
                    int slot, const char* passwd)
{
  int err = ERR_OK, nKey;
  LIBHANDLE pLibrary = 0;
  CK_ULONG certLen, sigLen, padDigLen;
  CK_RV rv;
  CK_SLOT_ID slotids[20], slId = 0;
  CK_SESSION_HANDLE hSession = 0;
  CK_OBJECT_HANDLE hPrivateKey, hKeys[20], hCert;
  char keyId[20][20];
  CK_ULONG keyIdLen[20];
  CK_BYTE certData[2048];
  CK_BYTE sigDig[100], padDig[130];
  CK_BYTE signature[200];
  CK_BYTE padding[] = { 48, 33, 48, 9, 6, 5, 43, 14, 3, 2, 26, 5, 0, 4, 20 };
  char* buf1;
  int l1, l2;
  X509* x509;
  DigiDocMemBuf mbuf1;

  RETURN_IF_NULL_PARAM(pSigInfo);
  RETURN_IF_NULL_PARAM(pSigDoc);
  // try active driver driver first
  snprintf((char*)signature, sizeof(signature), "DIGIDOC_DRIVER_%d_FILE", 
	  ConfigItem_lookup_int("DIGIDOC_DEFAULT_DRIVER", 1));
  for(l1 = 0; l1 < 20; l1++) 
    slotids[l1] = INVALID_SLOTIID; // initialize
  err = loadAndTestDriver(ConfigItem_lookup((const char*)signature),
			  &pLibrary, (CK_SLOT_ID*)slotids, 20, (CK_ULONG)slot);
  ddocDebug(3, "calculateSignatureWithEstID", "Driver handle: %d err = %d slot: %d", 
	    pLibrary, err, slot);
  RETURN_IF_NOT(err == ERR_OK, err);
  // inittialize
  slId = INVALID_SLOTIID; // not found yet
  //err = ddocLocateSlotWithSignatureCert(pLibrary, slotids, 
  //					&slId, (char*)signature, sizeof(signature));
  
  // find suitable slotid
  
  for(l1 = 0; l1 < 20; l1++) {
    if(slotids[l1] != INVALID_SLOTIID)
      ddocDebug(3, "calculateSignatureWithEstID", 
		"Slot idx: %d = %d", l1, slotids[l1]);
    if(slotids[l1] != INVALID_SLOTIID && l1 == slot) {
      slId = slotids[l1];
      ddocDebug(3, "calculateSignatureWithEstID", 
		"Select idx: %d slot: %d", l1, slId);
    }
  }
  
  // open session
  if(slId != INVALID_SLOTIID) {
    hSession = OpenSession(slId, passwd);
    ddocDebug(3, "calculateSignatureWithEstID", 
	      "Open sess for slot: %d sess = %uld\n", slId, hSession);
    if (hSession == CK_INVALID_HANDLE) { err = ERR_PKCS_LOGIN; SET_LAST_ERROR(err); return err; }
    ddocDebug(3, "calculateSignatureWithEstID", "OpenSession ok, hSession = %d\n", (int)hSession);

    // get private key
    for(l1 = 0; l1 < 20; l1++) {
      memset(keyId[l1], 0, 20);
      keyIdLen[l1] = 0;
    }
    err = LocatePrivateKey(hSession, keyId, keyIdLen, hKeys);
    //ddocDebug(3, "calculateSignatureWithEstID", "Priv key: %s", keyId);
    //if (hPrivateKey == CK_INVALID_HANDLE) { err = ERR_PKCS_PK; SET_LAST_ERROR(err); return err; }

    // get cert     
    memset(certData, 0, sizeof(certData));  
    certLen = sizeof(certData);
    hCert = LocateCertificate(hSession, certData, &certLen, keyId, keyIdLen, &nKey);
    hPrivateKey = hKeys[nKey];
    ddocDebug(3, "calculateSignatureWithEstID", "selected priv-key: %ld pos %d id: %s", hPrivateKey, nKey, keyId[nKey]);
    ddocDebug(3, "calculateSignatureWithEstID", "Cert-len: %ld", certLen);
    //printf("Cert: %s", certData);
    if (hCert == (CK_OBJECT_HANDLE)-1) { err = ERR_PKCS_CERT_LOC; SET_LAST_ERROR(err); return err; }

    // set cert data
    err = ddocDecodeX509Data(&x509, certData, certLen);
    if (!x509) { err = ERR_PKCS_CERT_DECODE;  }
    setSignatureCert(pSigInfo, x509);

    // FIXME
    createTimestamp(pSigDoc, (char*)sigDig, sizeof(sigDig));
    setString((char**)&(pSigInfo->szTimeStamp), (const char*)sigDig, -1);

    // Signed properties digest
    buf1 = createXMLSignedProperties(pSigDoc, pSigInfo, 0);
    //dumpInFile("sigprop-sign1.txt", buf1);
    if (!buf1) { 
      err = ERR_NULL_POINTER; 
      SET_LAST_ERROR(err);
      return err;
    }
    mbuf1.pMem = canonicalizeXML((char*)buf1, strlen(buf1));
    mbuf1.nLen = strlen((const char*)mbuf1.pMem);
    ddocDebugWriteFile(4, "sigprop-signed.txt", &mbuf1);
    l2 = sizeof(sigDig);
    err = calculateDigest((const byte*)mbuf1.pMem, mbuf1.nLen, DIGEST_SHA1, sigDig, &l2);
    free(buf1);
    ddocMemBuf_free(&mbuf1);
    if (err != ERR_OK) {
      SET_LAST_ERROR(err);			
      return err;
    }
    ddocSigInfo_SetSigPropDigest(pSigInfo, (const char*)sigDig, l2);
    ddocSigInfo_SetSigPropRealDigest(pSigInfo, (const char*)sigDig, l2);
    // create signed info
    buf1 = createXMLSignedInfo(pSigDoc, pSigInfo);     
    if (!buf1) {
      err = ERR_NULL_POINTER;
      SET_LAST_ERROR(err);
      return err ;
    }
    // get digest
    l2 = sizeof(sigDig);
    err = calculateDigest((const byte*)buf1, strlen(buf1), 
			  DIGEST_SHA1, sigDig, &l2);
    free(buf1);
    if (err != ERR_OK) {
      err = ERR_NULL_POINTER;
      SET_LAST_ERROR(err);
      return err;
    } 
    ddocSigInfo_SetSigInfoRealDigest(pSigInfo, (const char*)sigDig, l2);
    // sign data
    sigLen = sizeof(signature);
    memset(signature, 0, sizeof(signature));
    // pad PKCS#1 ver 1
    padDigLen = 35;
    memset(padDig, 0, sizeof(padDig));
    memcpy(padDig, padding, 15);
    memcpy(padDig + 15, sigDig, l2);
    //rv = RSA_padding_add_PKCS1_type_1(padDig, padDigLen, sigDig, l2);
    //rv = RSA_padding_check_PKCS1_type_1(sigDig, l2, padDig, padDigLen, padDigLen+1);
    // checkErrors();
    // sign data
    rv = SignData(hSession, hPrivateKey, 
		  signature, &sigLen, padDig, padDigLen);    
    if (rv != CKR_OK) { 
      err = ERR_PKCS_SIGN_DATA;
      SET_LAST_ERROR(err);
      return err;
    }		

    // set signature value
    ddocSigInfo_SetSignatureValue(pSigInfo, (const char*)signature, (int)sigLen);
  } // if slotid found
    
  if(hSession)
    closePKCS11Library(pLibrary, hSession);
  return err;
}

//============================================================
// Loads the PKCS#11 driver and
// tests if the driver loaded correctly
// Returns 0 for ok otherwise error code
//============================================================
EXP_OPTION CK_RV getDriverInfo(CK_INFO_PTR pInfo)
{
  return (*ckFunc->C_GetInfo)(pInfo);
}

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
				char* decData, int *decLen)
{
  int err = ERR_OK, l1, l2;
  LIBHANDLE pLibrary = 0;
  CK_ULONG keyIdLen[20];
  CK_RV rv;
  CK_SLOT_ID slotids[20], slId;
  CK_SESSION_HANDLE hSession = 0;
  CK_OBJECT_HANDLE hPrivateKey, hKeys[20];
  char keyId[20][20];
  char driver[100];
  CK_MECHANISM Mechanism = { CKM_RSA_PKCS, 0, 0 };
  CK_ULONG outlen;

  ddocDebug(3, "decryptWithEstID", "slot: %d enc-data: %d bytes buffer size: %d", slot, encLen, *decLen);
  snprintf(driver, sizeof(driver), "DIGIDOC_DRIVER_%d_FILE", ConfigItem_lookup_int("DIGIDOC_DEFAULT_DRIVER", 1));
  ddocDebug(3, "decryptWithEstID", "Driver nr: %d - %s", 
	    ConfigItem_lookup_int("DIGIDOC_DEFAULT_DRIVER", 1),
	    ConfigItem_lookup(driver));
  err = loadAndTestDriver(ConfigItem_lookup(driver), 
			  &pLibrary, (CK_SLOT_ID*)slotids, 20, (CK_ULONG)slot);
  if(err) return err;

  // find the right slotid
  for(l1 = l2 = 0; l1 < 20; l1++) {
    if(slotids[l1] != INVALID_SLOTIID) {
      if(l2 == slot)
	slId = slotids[l1];
      l2++;
    }
  }
  // open session
  hSession = OpenSession(slId, pin);
  if (hSession == CK_INVALID_HANDLE) { SET_LAST_ERROR(ERR_PKCS_LOGIN); return ERR_PKCS_LOGIN; }
  ddocDebug(3, "decryptWithEstID", "OpenSession ok, hSession = %d", (int)hSession);
  
  // get private key
  for(l1 = 0; l1 < 20; l1++) {
    memset(keyId[l1], 0, 20);
    keyIdLen[l1] = 0;
  }
  err = LocatePrivateKey(hSession, keyId, keyIdLen, hKeys);
  hPrivateKey = hKeys[0]; //???
  //ddocDebug(3, "decryptWithEstID", "Priv key: %s", keyId);
  //if (hPrivateKey == CK_INVALID_HANDLE) { SET_LAST_ERROR(ERR_PKCS_PK); return ERR_PKCS_PK; }
  // init decrypt
  rv = (*ckFunc->C_DecryptInit)(hSession, &Mechanism, hPrivateKey);
  ddocDebug(3, "decryptWithEstID", "DecryptInit: %d", (int)rv);
  if(rv != CKR_OK) SET_LAST_ERROR_RETURN(ERR_DENC_DECRYPT, ERR_DENC_DECRYPT)
  // decrypt data
  outlen = *decLen;
  rv = (*ckFunc->C_Decrypt)(hSession, (CK_BYTE_PTR)encData, (CK_ULONG)encLen, (CK_BYTE_PTR)decData, (CK_ULONG_PTR)&outlen);
  *decLen = outlen;
  ddocDebug(3, "decryptWithEstID", "RV: %d, dec-len: %d", (int)rv, *decLen);
  if(hSession)
    closePKCS11Library(pLibrary, hSession);
  if(rv != CKR_OK)
    SET_LAST_ERROR_RETURN(ERR_DENC_DECRYPT, ERR_DENC_DECRYPT)
  return err;
}


//============================================================
// Locates and reads users certificate from smartcard
// slot - number of the slot for decryption key. On ID card allways 0
// ppCert - address for newly allocated certificate pointer
// return error code or ERR_OK
//============================================================
EXP_OPTION int findUsersCertificate(int slot, X509** ppCert)
{
  int err = ERR_OK, l1, l2;
  LIBHANDLE pLibrary = 0;
  CK_RV rv = 0;
  CK_SLOT_ID slotids[20], slId;
  CK_OBJECT_HANDLE hCert;
  CK_SESSION_HANDLE hSession = 0;
  CK_ULONG certLen;
  CK_BYTE certData[2048];
  char driver[100];


  *ppCert = 0;
  snprintf(driver, sizeof(driver), "DIGIDOC_DRIVER_%d_FILE", ConfigItem_lookup_int("DIGIDOC_DEFAULT_DRIVER", 1));
  ddocDebug(3, "findUsersCertificate", "Slot: %d Driver nr: %d - %s", slot,
	    ConfigItem_lookup_int("DIGIDOC_DEFAULT_DRIVER", 1),
	    ConfigItem_lookup(driver));
  err = loadAndTestDriver(ConfigItem_lookup(driver), 
			  &pLibrary, (CK_SLOT_ID*)slotids, 20, (CK_ULONG)slot);
  if(err) return err;

  // find the right slotid
  for(l1 = l2 = 0; l1 < 20; l1++) {
    if(slotids[l1] != INVALID_SLOTIID) {
      if(l2 == slot)
	slId = slotids[l1];
      l2++;
    }
  }
  // open session
  hSession = OpenSession(slId, NULL);
  if (hSession == CK_INVALID_HANDLE) { SET_LAST_ERROR(ERR_PKCS_LOGIN); return ERR_PKCS_LOGIN; }
  ddocDebug(3, "findUsersCertificate", "OpenSession ok, hSession = %d", (int)hSession);
  
  // get cert
  memset(certData, 0, sizeof(certData));  
  certLen = sizeof(certData);
  hCert = LocateCertificate(hSession, certData, &certLen, 0, 0, 0);
  ddocDebug(3, "findUsersCertificate", "hCert = %d, len: %d", (int)hCert, certLen);
  if (hCert == (CK_OBJECT_HANDLE)-1) { err = ERR_PKCS_CERT_LOC; SET_LAST_ERROR(err); }

  // set cert data
  if(certLen)
    err = ddocDecodeX509Data(ppCert, certData, certLen);

  
  ddocDebug(3, "findUsersCertificate", "RV: %d, cert: %s", (int)rv, (*ppCert ? "OK" : "NULL"));
  if(hSession)
    closePKCS11Library(pLibrary, hSession);

  return err;
}


