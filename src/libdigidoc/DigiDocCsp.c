//==================================================
// FILE:	DigiDocCsp.c
// PROJECT: Digi Doc
// DESCRIPTION: Digi Doc functions for creating
//	and reading signed documents. 
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
//      20.10.2004  Changed createOCSPRequest, added SignedDoc parameeter
//      10.02.2004  Changed decodeCertificateData
//                          ReadCertSerialNumber
//                          ReadCertificate
//                          NotaryInfo_new
//      01.02.2004  Aare Amenberg
//                  from comments "Functions from EstIDLib.c"
//                  functions from EstIdLib.c file
//      20.11.2003  Aare Amenberg
//                  removed tes2.resp file creating
//      29.10.2003  Created by AA
//
//==================================================
#ifdef WIN32

#define WIN32_PKCS 1

#include <windows.h>
#include <wincrypt.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/DigiDocMem.h>
#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocOCSP.h>
#include <libdigidoc/DigiDocObj.h>
#include <libdigidoc/DigiDocGen.h>
#include <libdigidoc/DigiDocVerify.h>
#include <libdigidoc/ts/DigiDocTS.h>
#include <libdigidoc/DigiDocConvert.h>
#include <libdigidoc/DigiDocDebug.h>


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>
#include <direct.h>  

#include <fcntl.h>
#include <time.h>


#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/ocsp.h>
#include <openssl/pkcs12.h>

#include <libxml/globals.h>
#include <libxml/xmlerror.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h> /* only for xmlNewInputFromFile() */
#include <libxml/tree.h>
#include <libxml/debugXML.h>
#include <libxml/xmlmemory.h>
#include <libxml/c14n.h>

#include "DigiCrypt.h"
#include "DigiDocCert.h"

#include "DigiDocCsp.h"

CSProvider * cCSProvider;
CSProvider * knownCSProviders[3];

#ifdef WITH_TS
#include <libdigidoc/DigiDocGlobals.h>
#endif

//AARE01102003
typedef struct StoreHandle_st {
	HCERTSTORE hHandle;
	BOOL       fRoot;
}	 StoreHandle;


BOOL       Digi_OpenStore(StoreHandle *hStore);
void       Digi_CloseStore(StoreHandle *hStore);
BOOL       Digi_IsValidResponderCert(PCCERT_CONTEXT  pCert);

X509 * Digi_FindX509CopyFromStore(StoreHandle *hStore, X509 *pX509cert); 
X509       *Digi_FindResponderCert(StoreHandle *hStore, X509 *poSignerCert);
char       *Digi_GetName(X509_NAME *pName);
BOOL       Digi_CheckEnhancedKeyUsage(PCCERT_CONTEXT pCert, char *psValue);
X509       *Digi_FindCertByResponse(StoreHandle *hStore, OCSP_RESPONSE *poResponse);
PCCERT_CONTEXT Digi_FindCertBySubject(StoreHandle *hStore,char *psCN, BOOL bCheckValid, const char* szSerialNr);
BOOL       Digi_CompareCN(char *psSub1, char *psSub2);
X509       **Digi_MakeCertList(X509 *poX509Main, StoreHandle *hStore);
X509       **Digi_MakeCertListLow(X509 *poX509Main, StoreHandle *hStore);
BOOL       Digi_CheckResponderCertByResponse(X509 *poX509Responder, OCSP_RESPONSE *poResponse);

void       Digi_FreeCertList(X509 **caCerts);
BOOL       Digi_IsCert1SubjectDNEqualCert2IssuerDN(X509 *pCert1, X509 *pCert2);
int        ReadTestAare(char *pkcs12file);
int        Test_ReadCertData(X509 *poX509);
int        Test_ReadCertDataC(PCCERT_CONTEXT pCert);

// Reads a certificate from pkcs12 container
EXP_OPTION int Digi_readCertificateByPKCS12OnlyCertHandle(const char *pkcs12file, const char * passwd, X509 **x509);
int Digi_getConfirmationWithCertSearch(SignedDoc* pSigDoc, SignatureInfo* pSigInfo, char* pkcs12File, char* password,
                            char* notaryURL, char* proxyHost, char* proxyPort);
//int Digi_setNotaryCertificate(NotaryInfo* pNotary, X509* notCert);
int Digi_verifyNotaryInfoWithCertSearch(const SignedDoc* pSigDoc, const NotaryInfo* pNotInfo);

// verifies this one signature
int Digi_verifySignatureInfo(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo, 
						const char* szDataFile, FilePath* pPaths, int nPaths);
// verifies the whole document (returns on first err)
int Digi_verifySigDoc(const SignedDoc* pSigDoc, const char* szDataFile, 
							FilePath* pPaths, int nPaths);
int Digi_verifySigDocWithCertSearch(const SignedDoc* pSigDoc, const char* szDataFile, FilePath* pPaths, int nPaths);
int Digi_verifyNotaryInfoCERT(const SignedDoc* pSigDoc, 
				    const NotaryInfo* pNotInfo,  
				    const X509** caCerts, 
				    const X509* notCert);
//int Digi_initializeNotaryInfoWithOCSP(SignedDoc* pSigDoc, NotaryInfo* pNotary, 
//				OCSP_RESPONSE* resp, X509* notCert, int initDigest);


// verifies signed doc 
int verifySigDoc_ByCertStore(const SignedDoc* pSigDoc, const char* szDataFile,FilePath* pPaths, int nPaths);
// Verifies this signature
int verifySignatureInfo_ByCertStore(const SignedDoc* pSigDoc, 
					const SignatureInfo* pSigInfo, const char* szDataFile,FilePath* pPaths, int nPaths);
// Verfies NotaryInfo signature
int verifyNotaryInfo_ByCertStore(const SignedDoc* pSigDoc, const NotaryInfo* pNotInfo);

// resolves certificate chain from MS CertStore upto root cert
int resolveCertChain_ByCertStore(CertItem* pListStart);

int GetAllCertificatesFromStore(const CertSearchStore *sS, CertItem **certList, int *numberOfCerts);
void prepareString(const char * strIN,char * strOUT);
void reverseArray(unsigned char *array, unsigned long arrayLen);

extern int setup_verifyCERT(X509_STORE **newX509_STORE,	const char *CApath,	const X509** certs);

extern X509_ALGOR* setSignAlgorithm(const EVP_MD * type);

extern EXP_OPTION int signOCSPRequestPKCS12(OCSP_REQUEST *req, const char* filename, const char* passwd);


extern LPBYTE getDefaultKeyName(CSProvider * cProvider);
//extern CERT_PUBLIC_KEY_INFO * getCardKeyInfo(const char * ppContainerName);
extern int GetCertificateFromStore(const CertSearchStore *sS, X509 **x509);
extern int GetSignedHashWithEstIdCSPkey(const char * dataToBeSigned,unsigned long dataLen,unsigned char *pbKeyBlob, unsigned long *pbKeyBlobLen,unsigned char *hash, unsigned long *hashLen, unsigned char * hashedSignature,unsigned long * sigLen);
extern int GetSignedHashWithKeyAndCSP(char *psKeyName, char *psCSPName, const char * dataToBeSigned,unsigned long dataLen,unsigned char *pbKeyBlob, unsigned long *pbKeyBlobLen,unsigned char *hash, unsigned long *hashLen, unsigned char * hashedSignature,unsigned long * sigLen);
extern X509 * findIssuerCertificatefromStore(X509 *x509);
X509* Digi_FindDirectCA(X509 *poX509Main, StoreHandle *hStore);

//==========< macros >====================

#define SET_LAST_ERROR(code)                (addError((code), __FILE__, __LINE__, ""))
#define SET_LAST_ERROR_IF_NOT(expr, code)   { if(!(expr)) addError((code), __FILE__, __LINE__, #expr); }
#define SET_LAST_ERROR_RETURN(code, retVal) { SET_LAST_ERROR(code); return (retVal); }
#define SET_LAST_ERROR_RETURN_IF_NOT(expr, code, retVal) { if(!(expr)) { addError((code), __FILE__, __LINE__, #expr); return (retVal); } }
#define SET_LAST_ERROR_RETURN_VOID_IF_NOT(expr, code) { if(!(expr)) { addError((code), __FILE__, __LINE__, #expr); return; } }
#define RETURN_IF_NOT(expr, code) SET_LAST_ERROR_RETURN_IF_NOT((expr), (code), (code));
#define RETURN_IF_NULL(p)                   RETURN_IF_NOT((p), ERR_NULL_POINTER);
#define RETURN_VOID_IF_NULL(p)              SET_LAST_ERROR_RETURN_VOID_IF_NOT((p), ERR_NULL_POINTER);
#define RETURN_OBJ_IF_NULL(p, obj)          SET_LAST_ERROR_RETURN_IF_NOT((p), ERR_NULL_POINTER, (obj));
#define SET_LAST_ERROR_RETURN_CODE(code)    { SET_LAST_ERROR(code); return (code); }

//========================================


BOOL Digi_OpenStore(StoreHandle *hStore)
{
  BOOL fRes = FALSE;
#ifdef WITH_TS
  TSAProfile_init();
#endif
  if (hStore != NULL) {
  memset(hStore,0,sizeof(StoreHandle));
  hStore->hHandle = CertOpenStore(CERT_STORE_PROV_SYSTEM,0,(HCRYPTPROV)NULL,
	  CERT_SYSTEM_STORE_CURRENT_USER | CERT_STORE_READONLY_FLAG,L"CA");
  if (hStore->hHandle != NULL)
    fRes = TRUE;
  }
  return(fRes);
}

void Digi_CloseStore(StoreHandle *hStore)
{
if (hStore != NULL && hStore->hHandle)
  CertCloseStore(hStore->hHandle,CERT_CLOSE_STORE_FORCE_FLAG);
hStore->hHandle = NULL;
hStore->fRoot = FALSE;
}


X509 * Digi_FindX509CopyFromStore(StoreHandle *hStore, X509 *pX509cert) {
	X509 *pResultCert = NULL;
	char* certBlob; 
	int certBlobLen; 
	PCCERT_CONTEXT pCertFindContext = NULL;
	PCCERT_CONTEXT pCertContext = NULL;
	char  *psValue = "1.3.6.1.5.5.7.3.9";

	if (hStore == NULL || pX509cert == NULL) return(pResultCert);

	certBlobLen = i2d_X509(pX509cert, NULL) + 1000;
	certBlob = malloc(certBlobLen);

	encodeCert(pX509cert,certBlob,&certBlobLen);
	if (!certBlob) return(pResultCert);

	pCertFindContext = CertCreateCertificateContext(X509_ASN_ENCODING|PKCS_7_ASN_ENCODING,certBlob,certBlobLen);
	if (!pCertFindContext) return(pResultCert);

	pCertContext = CertFindCertificateInStore(hStore->hHandle, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 
										0, CERT_FIND_EXISTING, pCertFindContext, NULL); 

	if (pCertContext) { //cert found 
		if (Digi_CheckEnhancedKeyUsage(pCertContext,psValue) != TRUE) return(pResultCert);
		ddocDecodeX509Data(&pResultCert,pCertContext->pbCertEncoded,pCertContext->cbCertEncoded);
	} else {
		char schultz[300]; 
		snprintf(schultz, sizeof(schultz), "CertFindCertificateInStore open failed with 0x%0lx ", GetLastError());			
		return(pResultCert);
	}

	if (certBlob) free(certBlob);
	
	return pResultCert; 
}



X509 *Digi_FindResponderCert(StoreHandle *hStore, X509 *poSignerCert)
{
X509 *poResultCert = NULL;
FILETIME oResultTime;
X509 *poX509;
BOOL fSet;
PCCERT_CONTEXT pCert = NULL;
X509_NAME *poX509SignerName = NULL;
X509_NAME *poX509CurrentName = NULL;

poX509SignerName = X509_get_issuer_name(poSignerCert); 
while (pCert = CertEnumCertificatesInStore(hStore->hHandle, pCert))
  {
  fSet = FALSE;
  //get handle
  //AA100204
  ddocDecodeX509Data(&poX509, pCert->pbCertEncoded,pCert->cbCertEncoded);
  if (poX509 != NULL)
    {
    //get current cert issuer
    poX509CurrentName = X509_get_issuer_name(poX509); 
    //TEST
    //psTemp = Digi_GetName(poX509CurrentName);
    //Digi_CheckEnhancedKeyUsage(pCert,"1.3.6.1.5.5");
    //Test_ReadCertData(poX509);
    //END TEST
    // if issuer match
    if (poX509SignerName != NULL && poX509CurrentName != NULL && X509_NAME_cmp(poX509SignerName, poX509CurrentName) == 0)
      {
      // check date
      // NULL, if current time
      if (Digi_IsValidResponderCert(pCert) == TRUE)
        {
        if (poResultCert == NULL)
          fSet = TRUE;
        else
          {
          if (CompareFileTime(&pCert->pCertInfo->NotBefore, &oResultTime) > 0)
            fSet = TRUE;
          }
        if (fSet == TRUE)
          {
          //delete previous if exists
          if (poResultCert != NULL)
            X509_free(poResultCert);
          //set result value
          poResultCert = poX509;
          memmove(&oResultTime,&pCert->pCertInfo->NotBefore,sizeof(FILETIME)); 
          }        
        }
      }
    }
  if (poX509 != NULL && fSet == FALSE)
	  X509_free(poX509);
  }


return(poResultCert);
}

int Test_ReadCertDataC(PCCERT_CONTEXT pCert)
{
int iRes = 0;
X509	*poX509 = NULL;
if (pCert != NULL);
  //AA100204
  ddocDecodeX509Data(&poX509,pCert->pbCertEncoded,pCert->cbCertEncoded);
if (poX509 != NULL)
  iRes = Test_ReadCertData(poX509);
return(iRes);
}



int Test_ReadCertData(X509 *poX509)
{
	int iRes;
	DigiDocMemBuf mbuf1;
	DigiDocMemBuf mbuf2;

	mbuf1.pMem = 0;
	mbuf1.nLen = 0;
	mbuf2.pMem = 0;
	mbuf2.nLen = 0;
	// get issuer DN
	iRes = ddocCertGetIssuerDN(poX509, &mbuf1);
	iRes = ddocMemAppendData(&mbuf1, "\n", -1);
	// get subject DN
	iRes = ddocCertGetSubjectDN(poX509, &mbuf2);
	iRes = ddocMemAppendData(&mbuf1, (const char*)mbuf2.pMem, mbuf2.nLen);
	// display it
	MessageBox(NULL,(const char*)mbuf1.pMem,"TEST",0);
	ddocMemBuf_free(&mbuf1);
	ddocMemBuf_free(&mbuf2);
	return(0);
}

BOOL Digi_IsValidResponderCert(PCCERT_CONTEXT  pCert)
{
BOOL  fIsValid = FALSE;
char  *psValue = "1.3.6.1.5.5.7.3.9";
if (pCert != NULL)
  {
  fIsValid = Digi_CheckEnhancedKeyUsage(pCert,psValue);
  if (fIsValid == TRUE)
    {
    if (CertVerifyTimeValidity(NULL,pCert->pCertInfo) != 0)
      fIsValid = FALSE;
    }
  }
return(fIsValid);
}


BOOL Digi_CheckEnhancedKeyUsage(PCCERT_CONTEXT pCert, char *psValue)
{
BOOL fRes = FALSE;
DWORD dwFlags = CERT_FIND_EXT_ONLY_ENHKEY_USAGE_FLAG;
PCERT_ENHKEY_USAGE pUsage = NULL;
//DWORD cUsageIdentifier - number of elements in rgpszUsageIdentifier
//LPSTR *rgpszUsageIdentifier - array;
DWORD cbUsage = 0;        
BOOL fCallRes;
int  iI;
char *psValueCurrent;
fCallRes = CertGetEnhancedKeyUsage(pCert,dwFlags,NULL, &cbUsage);                   
if (fCallRes == TRUE)
  {
  pUsage = (PCERT_ENHKEY_USAGE)malloc(cbUsage);
  fCallRes = CertGetEnhancedKeyUsage(pCert,dwFlags,pUsage, &cbUsage);                   
  if (fCallRes == TRUE)
    {
    for (iI=0; iI < (int)pUsage->cUsageIdentifier;++iI)
      {
      psValueCurrent = pUsage->rgpszUsageIdentifier[iI];
      if (psValueCurrent != NULL && psValue != NULL)
        {
        if (strstr(psValueCurrent,psValue) != NULL)
		  {
          fRes = TRUE;
		  break;
		  }
        }
      }
    } 
  }
if (pUsage != NULL)
  free(pUsage);
return(fRes);
}


X509 *Digi_FindCertByResponse(StoreHandle *hStore, OCSP_RESPONSE *poResponse)
{
  X509 *poX509 = NULL;
  PCCERT_CONTEXT pCert = NULL;
  OCSP_RESPID    *rid = NULL;
  OCSP_BASICRESP *br = NULL;
  OCSP_RESPDATA  *rd = NULL;
  int iLen;
  char sCN[255];

  if (poResponse != NULL) {
	if ((br = OCSP_response_get1_basic(poResponse)) == NULL)
          return(poX509); 
	rd = br->tbsResponseData;
	rid =  rd->responderId;
	if (rid->type != V_OCSP_RESPID_NAME) {
          if(br) OCSP_BASICRESP_free(br);
          return(poX509); 
        }
	iLen = X509_NAME_get_text_by_NID(rid->value.byName,NID_commonName,sCN,sizeof(sCN));
	if (iLen > 0)  //VS: 18.03.2006 - use only currently valid cert for new notary
      pCert = Digi_FindCertBySubject(hStore, sCN, TRUE, 0);
    if(pCert != NULL)
      //AA100204
	  ddocDecodeX509Data(&poX509,pCert->pbCertEncoded,pCert->cbCertEncoded);
  }
  if(br) OCSP_BASICRESP_free(br);
  return(poX509);
}

#ifdef WITH_TS

/*X509 *Digi_FindTSACert(StoreHandle *hStore)
{
  X509 *poX509 = NULL;
  PCCERT_CONTEXT pCert = NULL;
  TSAProfile *pProfile = TSAProfile_init();
  if(pProfile) {
    pCert = Digi_FindCertBySubject(hStore, pProfile->g_szTsaCN, TRUE, 0);
    if(pCert != NULL)
      ddocDecodeX509Data(&poX509,pCert->pbCertEncoded,pCert->cbCertEncoded);
  }
  return(poX509);
}*/

X509 *Digi_FindTSACACert(StoreHandle *hStore, TimestampInfo *pTS)
{
  X509 *poX509 = NULL;
  PCCERT_CONTEXT pCert = NULL;
  TS_RESP* pResp = NULL;
  DigiDocMemBuf mbuf1;
  char cn[200];
  const DigiDocMemBuf *pMBuf = NULL;

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  pMBuf = ddocTimestampInfo_GetTS(pTS);
  if(pMBuf)
    ddocTsReadTsResp(&pResp, (DigiDocMemBuf*)pMBuf);
  if(pResp) {
	ddocTsResp_GetSignerCertIssuer(pResp, &mbuf1);
	findCN((char*)mbuf1.pMem, cn);
    pCert = Digi_FindCertBySubject(hStore, cn, TRUE, 0);
    if(pCert != NULL)
      ddocDecodeX509Data(&poX509,pCert->pbCertEncoded,pCert->cbCertEncoded);
  	  TS_RESP_free(pResp);
  }
  ddocMemBuf_free(&mbuf1);
  return(poX509);
}

#endif // WITH_TS


//Added by AA 09/10/2003
BOOL Digi_CheckResponderCertByResponse(X509 *poX509Responder, OCSP_RESPONSE *poResponse)
{
BOOL fRes = FALSE;
OCSP_RESPID    *rid = NULL;
OCSP_BASICRESP *br = NULL;
OCSP_RESPDATA  *rd = NULL;
int iLen;
char sCNResp[255];
char sCNCert[255];
if (poResponse != NULL)
  {
  if ((br = OCSP_response_get1_basic(poResponse)) == NULL)
    return(fRes); 
	rd = br->tbsResponseData;
	rid =  rd->responderId;
	if (rid->type != V_OCSP_RESPID_NAME)
    return(fRes); 
	iLen = X509_NAME_get_text_by_NID(rid->value.byName,NID_commonName,sCNResp,sizeof(sCNResp));
	if (iLen > 0)
	  {
      iLen = X509_NAME_get_text_by_NID(X509_get_subject_name(poX509Responder),NID_commonName, sCNCert,sizeof(sCNCert));
      if (iLen > 0)
	    {
        fRes = Digi_CompareCN(sCNResp,sCNCert);
		}
	  }
  }
return(fRes);
}

//VS: 18.03.2006 - use only currently valid cert for new notary
PCCERT_CONTEXT Digi_FindCertBySubject(StoreHandle *hStore, char *psCN, BOOL bCheckValid, const char* szSerialNr)
{
  PCCERT_CONTEXT pCert = NULL;
  BOOL fFind = FALSE;
  X509 *poX509;
  char sSubject[255], sSerial[255];
  int  iLen;
  time_t tNow;

  if (hStore == NULL || psCN == NULL)
    return(pCert);
  Digi_CloseStore(hStore);
  hStore->hHandle = CertOpenStore(CERT_STORE_PROV_SYSTEM,0,(HCRYPTPROV)NULL,
	  CERT_SYSTEM_STORE_CURRENT_USER | CERT_STORE_READONLY_FLAG,L"CA");
  if (hStore->hHandle == NULL)
    return(pCert);
  //get signer issuer
  while (fFind == FALSE && (pCert = CertEnumCertificatesInStore(hStore->hHandle,pCert)))
  {
    //AA100204
    ddocDecodeX509Data(&poX509, pCert->pbCertEncoded,pCert->cbCertEncoded);
    if (poX509 != NULL)
    {
      iLen = X509_NAME_get_text_by_NID(X509_get_subject_name(poX509),NID_commonName, sSubject,255);
      if(iLen > 0) {
        fFind = Digi_CompareCN(sSubject, psCN);
		if(fFind) {
			if(szSerialNr) {  //VS: 18.03.2006 - look for a cert with specific serial nr
				ReadCertSerialNumber(sSerial, sizeof(sSerial)-1, poX509);
				fFind = !strcmp(szSerialNr, sSerial);
			} else if(bCheckValid) {  //VS: 18.03.2006 - use only currently valid cert for new notary
				time(&tNow);
				fFind = !isCertValid(poX509, tNow);
			}
		}
	  }
      X509_free(poX509);
    }
  }
  if (fFind == FALSE)
    pCert = NULL;
  return(pCert);
}

//----------------------------------------------------
// Returns a list of certificates based on the
// search criteria
// szCN - CN of certs to search for
// tValid - check validity on the given date (if not 0)
// szSerialNr - serach for certs with this serial nr
// pMBuf - public key hash to search
//----------------------------------------------------
X509 **Digi_FindCACerts(StoreHandle *hStore, const char* szCN, time_t tValid, 
						const char* szSerialNr, DigiDocMemBuf* pMBuf)
{
  int iMaxCerts = 512;
  int iRes = 0;
  X509 **caCerts = NULL;
  X509 *poX509;
  PCCERT_CONTEXT pCert = NULL;
  BOOL fAdd;
  char buf1[255];
  DigiDocMemBuf mbuf1;

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  //ddocDebug(3, "Digi_FindCACerts", "CN: %s tValid: %ld serial: %s, 
  // open CA store
  if(!hStore) return NULL;
  Digi_CloseStore(hStore);
  hStore->hHandle = CertOpenStore(CERT_STORE_PROV_SYSTEM,0,(HCRYPTPROV)NULL,
	  CERT_SYSTEM_STORE_CURRENT_USER | CERT_STORE_READONLY_FLAG,L"CA");
  if (hStore->hHandle == NULL) {
    return NULL;
  }
  
  // alloc list and initalize all positions to NULL
  caCerts = (X509**)malloc(sizeof(void*) * (iMaxCerts+1));
  if(caCerts == NULL) {
    return(caCerts);
  }
  memset(caCerts,0,sizeof(void*) * (iMaxCerts+1));
  

  if(hStore->hHandle != NULL) {  
    while(pCert= CertEnumCertificatesInStore(hStore->hHandle, pCert)) {
      fAdd = FALSE;
	  ddocDecodeX509Data(&poX509,pCert->pbCertEncoded,pCert->cbCertEncoded);
      if(poX509 != NULL) {
		  // check CN
		  if(szCN) {
			  buf1[0] = 0;
			  X509_NAME_get_text_by_NID(X509_get_subject_name(poX509), NID_commonName, buf1, sizeof(buf1));
			  fAdd = Digi_CompareCN((char*)szCN, buf1);
		  }
		  // check serial nr
		  if(szSerialNr) {
			buf1[0] = 0;
			ReadCertSerialNumber(buf1, sizeof(buf1)-1, poX509);
			fAdd = !strcmp(szSerialNr, (const char*)buf1);
		  }
		  // check pubkey hash
		  if(pMBuf) {
			ddocCertGetPubkeyDigest(poX509, &mbuf1);
			fAdd = !ddocMemCompareMemBufs(&mbuf1, pMBuf);
			ddocMemBuf_free(&mbuf1);
		  }
		  if(tValid) 
			fAdd = !isCertValid(poX509, tValid);
      }
	  // if any of the conditiones matched then add cert to list
      if (fAdd == TRUE) {
        caCerts[iRes] = poX509;  
        ++iRes;
      }
      else
   	    X509_free(poX509);
    }
  }
  // if no certs found then free the array
  caCerts[iRes] = NULL;
  if(iRes == 0) {
    free(caCerts);
    caCerts = NULL;
  }
  return(caCerts);
}


X509 **Digi_MakeCertList(X509 *poX509Main, StoreHandle *hStore)
{
X509 **caCerts = NULL;
memset(hStore,0,sizeof(StoreHandle));
hStore->hHandle = CertOpenStore(CERT_STORE_PROV_SYSTEM,0,(HCRYPTPROV)NULL,
								CERT_SYSTEM_STORE_CURRENT_USER | CERT_STORE_READONLY_FLAG,L"CA");
if (hStore->hHandle != NULL)
  {
  caCerts = Digi_MakeCertListLow(poX509Main, hStore);
  if (caCerts == NULL)
    { 
    CertCloseStore(hStore->hHandle,CERT_CLOSE_STORE_FORCE_FLAG);
    hStore->hHandle = CertOpenStore(CERT_STORE_PROV_SYSTEM,0,(HCRYPTPROV)NULL,
		CERT_SYSTEM_STORE_CURRENT_USER | CERT_STORE_READONLY_FLAG,L"ROOT");
    if (hStore->hHandle != NULL)
      {
      hStore->fRoot = TRUE;
      caCerts = Digi_MakeCertListLow(poX509Main, hStore);
      }
	}
  }
return(caCerts);
}


X509 **Digi_MakeCertListLow(X509 *poX509Main, StoreHandle *hStore)
{
int iMaxCerts = 512;
int iRes = 0;
X509 **caCerts = NULL;
X509 *poX509;
PCCERT_CONTEXT pCert = NULL;
BOOL fAdd;
if (poX509Main == NULL)
  return(caCerts); 
caCerts = (X509**)malloc(sizeof(void*) * (iMaxCerts+1));
if (caCerts == NULL)
  return(caCerts);
memset(caCerts,0,sizeof(void*) * (iMaxCerts+1));

if (hStore->hHandle != NULL)
  {
  while( pCert= CertEnumCertificatesInStore( hStore->hHandle,pCert))
    {
    fAdd = FALSE;
	ddocDecodeX509Data(&poX509,pCert->pbCertEncoded,pCert->cbCertEncoded);
    //TEST
    //Test_ReadCertData(poX509);
    //ENDTEST
    if (poX509 != NULL)
      fAdd = Digi_IsCert1SubjectDNEqualCert2IssuerDN(poX509,poX509Main);
    if (fAdd == TRUE)
      {
      caCerts[iRes] = poX509;  
      ++iRes;
      }
    else
   	  X509_free(poX509);
    }
  }
//Added by AA 2004/03/15
caCerts[iRes] = NULL;
if (iRes == 0)
  {
  free(caCerts);
  caCerts = NULL;
  }
return(caCerts);
}


X509* Digi_FindDirectCA(X509 *poX509Main, StoreHandle *hStore)
{
	X509 *poX509;
	PCCERT_CONTEXT pCert = NULL;

	if (poX509Main == NULL)
		return NULL; 

	if (hStore->hHandle != NULL)   {
		while(pCert = CertEnumCertificatesInStore( hStore->hHandle, pCert)) {
			ddocDecodeX509Data(&poX509,pCert->pbCertEncoded,pCert->cbCertEncoded);
			if(poX509 != NULL) {
				if(Digi_IsCert1SubjectDNEqualCert2IssuerDN(poX509, poX509Main)) {
					return poX509;
				}		
			}
			else
   				X509_free(poX509);
		}
	}
	return NULL;
}

void Digi_FreeCertList(X509 **caCerts)
{
int iMaxCerts = 512;
int i;
if (caCerts != NULL)
  {
	for(i = 0; i < iMaxCerts; i++)
    {
		if(caCerts[i] != NULL)
			X509_free(caCerts[i]);
    }
	free(caCerts);
  }
}


BOOL Digi_CompareCN(char *psSub1, char *psSub2)
{
  BOOL fRes = FALSE;
  if (psSub1 != NULL && psSub2 != NULL )
  {
  if (strlen(psSub1) > 0 || strlen(psSub2) > 0)
    {
    if (strcmp(psSub1,psSub2) == 0)
      fRes = TRUE;
    }
  }
  return(fRes);
}


int countCerts(const X509** certs)
{
	int i = 0;
	while(certs && certs[i])
		i++;
	return i;
}

BOOL Digi_IsCert1SubjectDNEqualCert2IssuerDN(X509 *pCert1, X509 *pCert2)
{
BOOL fEqual = FALSE;
unsigned long ulHash1;
unsigned long ulHash2;
ulHash1 = X509_subject_name_hash(pCert1);
ulHash2 = X509_issuer_name_hash(pCert2);
if (ulHash1 == ulHash2)
  fEqual = TRUE;
return(fEqual);
}


//29/09/2003
//31/05/2006 - (g_current_TSAProfile->g_bAddTimeStamp) enables timestamping
int Digi_getConfirmationWithCertSearch(SignedDoc* pSigDoc, SignatureInfo* pSigInfo,
                                  char* pkcs12File, char *password,
                                  char* notaryURL, char* proxyHost, char* proxyPort)
{
  int err = ERR_OK, err2 = ERR_OK, i;
  StoreHandle hStore;
  NotaryInfo* pNotInf = NULL;
  X509 *pNotCert = NULL, *pNotCertFound = NULL;
  X509 **caCerts = NULL;
  X509 **respCerts = NULL;
#ifdef WITH_TS
  X509 *pTsaCert = 0;
  TSAProfile *pProfile = 0;
#endif
  const DigiDocMemBuf *pMBuf = 0;
  char szCN[255];
  
  if (Digi_OpenStore(&hStore) == FALSE)
    SET_LAST_ERROR_RETURN_CODE(ERR_CERT_STORE_READ);
#ifdef WITH_TS
  pProfile = TSAProfile_init();
  RETURN_IF_NULL(pProfile);
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER) &&
	  pProfile->g_bAddSignatureTimeStamp) {
    err = ddocGetSignatureTimeStamp(pSigInfo, &pTsaCert, 
					pProfile->g_szTsaPolicy, pProfile->g_szTsaUrl,
					proxyHost, proxyPort);
	
	if(pTsaCert)
		X509_free(pTsaCert);
    RETURN_IF_NOT(err == ERR_OK, err);
  }
#endif
  // we need to find some CA-s of signer before asking for
  // confirmation in order to be able to construct certid
  // we need this because of users direct CA
  respCerts = Digi_MakeCertListLow(ddocSigInfo_GetSignersCert(pSigInfo), &hStore);
  // VS: ver 1.5.33 - make this decision lower if confirmation can still be retrieved
  //if(!respCerts) SET_LAST_ERROR_RETURN_CODE(ERR_SIGNERS_CERT_NOT_TRUSTED);
  err = getConfirmation(pSigDoc, pSigInfo, respCerts, NULL,
			pkcs12File, password,
			notaryURL, proxyHost, proxyPort);
  Digi_FreeCertList(respCerts);

  if(err) return err;
  pNotInf = pSigInfo->pNotary;
  pMBuf = ddocNotInfo_GetResponderId(pNotInf);
  RETURN_IF_NOT(pMBuf != NULL, ERR_NOTARY_SIG_MATCH);
  if(pNotInf->nRespIdType == RESPID_NAME_TYPE) {
    szCN[0] = 0;
    if(pMBuf && pMBuf->pMem)
	  findCN((char*)pMBuf->pMem, szCN);
    pMBuf = NULL; // don't look for pubkey hash
  } else if(pNotInf->nRespIdType == RESPID_KEY_TYPE) {
    // look for pubkey hash in Digi_FindCACerts
  }


  RETURN_IF_NOT(pNotInf != NULL, ERR_NOTARY_SIG_MATCH);
  // find a list of potential responder certificates
  // search only by responder id = CN. Should I check also validity on current time ?
  respCerts = Digi_FindCACerts(&hStore, szCN, 0, 0, (DigiDocMemBuf*)pMBuf);
  clearErrors();
  
  if(!err && respCerts) {
	i = 0;
	do {
	  pNotCert = respCerts[i];
	  // get the next potential notary cert
	  if(pNotCert) { 
		// find CA certs for it
        Digi_CloseStore(&hStore);
        caCerts = Digi_MakeCertList(pNotCert, &hStore);

        if (caCerts != NULL) {
		  err2 = finalizeAndVerifyNotary(pSigDoc, pSigInfo, pNotInf, (const X509**)caCerts, (const X509*)pNotCert);
		  // if this didn't verify then free the cert and mark the slot as freed
		  if(err2) {
			// remove certid and certvalue created for verification
			removeNotaryInfoCert(pSigInfo);
			// mark slot as freed (was done above)
			respCerts[i] = 0;
		  }
		  // found one cert, save it for later
		  if(!err2) {
			  // if one cert was already found
			  // then pick the freshes one - one were not-after-date is latest
			  if(pNotCertFound) {
				if(getCertNotAfterTimeT(pNotCert) > getCertNotAfterTimeT(pNotCertFound)) {
					// release older cert
					X509_free(pNotCertFound);
					pNotCertFound = pNotCert;
				}
			  }
			  pNotCertFound = pNotCert;
		  }
		  // else don't free, give ownership to new Notary!
		  // mark the slot as freed 
		  respCerts[i] = 0;
		}
        else {
          err = ERR_CERT_STORE_READ;
		  //fprintf(hFile, "No CA-s found, err: %d\n", err);
 		}
		// free CA certs
		Digi_FreeCertList(caCerts);
      } 
	  i++;
	} while(pNotCert);
	// free the remaining responder certs
	Digi_FreeCertList(respCerts);
  }

  // else clear verifying errors
  clearErrors();
  // final check with the selected responders certificate
  if(pNotCertFound) {
	  Digi_CloseStore(&hStore);
      caCerts = Digi_MakeCertList(pNotCertFound, &hStore);
	  err = finalizeAndVerifyNotary(pSigDoc, pSigInfo, pNotInf, (const X509**)caCerts, (const X509*)pNotCertFound);
	  Digi_FreeCertList(caCerts);
  }
  else
	  err = ERR_OCSP_CERT_NOTFOUND;
  //fprintf(hFile, "OCSP finalize RC: %d\n", err);
  // reset original signature content
  if(pSigInfo->mbufOrigContent.pMem)
    ddocMemBuf_free(&(pSigInfo->mbufOrigContent));
#ifdef WITH_TS
  if (!err && !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER) &&
	  pProfile->g_bAddSigAndRefsTimeStamp) {
	//pTsaCert = Digi_FindTSACert(&hStore);
    err = ddocGetSigAndRefsTimeStamp(pSigDoc, pSigInfo, &pTsaCert, 
					pProfile->g_szTsaPolicy, pProfile->g_szTsaUrl,
					proxyHost, proxyPort);
	if(pTsaCert)
		X509_free(pTsaCert);
    RETURN_IF_NOT(err == ERR_OK, err);
  }
#endif
  Digi_CloseStore(&hStore);
  if(err != ERR_OK) 
	SET_LAST_ERROR(err);
  return(err);
}

//--------------------------------------------------
// Verfies NotaryInfo signature
// pSigDoc - signed doc object
// pNotInfo - NotaryInfo object
// caFiles - array of CA file names terminated with NULL
// CApath - path to (directory) all certs
// notCertFile - Notary (e.g. OCSP responder) cert file 
//--------------------------------------------------
int Digi_verifyNotaryInfoWithCertSearch(const SignedDoc* pSigDoc, const NotaryInfo* pNotInfo) 
{
	X509** caCerts = NULL;
	X509* notCert = NULL;
    X509* cert = NULL;
	StoreHandle hStore;
    SignatureInfo *pSigInfo = NULL;
	int err = ERR_OK;
#ifdef WITH_TS
	X509* pTsaCaCert = NULL, *pTsaCert = NULL;
	X509 * tsaCaCerts[3];
	int nCAs;
	time_t t1 = 0, t2 = 0, t0 = 0;
    TimestampInfo *pTS;
	TSAProfile *pProfile = 0;
	char buf1[100], buf2[100], buf3[100];
    DigiDocMemBuf  mbuf1;

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
#endif

    if(pNotInfo)
		pSigInfo = ddocGetSignatureForNotary(pSigDoc, pNotInfo);
    if(pSigInfo != NULL)
       cert = ddocSigInfo_GetOCSPRespondersCert(pSigInfo);
    caCerts = Digi_MakeCertList(cert,&hStore);
    if(caCerts == NULL)
      SET_LAST_ERROR_RETURN_CODE(ERR_CERT_STORE_READ);
#ifdef WITH_TS
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)) {
	  pProfile = TSAProfile_init();
	  RETURN_IF_NULL(pProfile);
	  err = ddocNotInfo_GetProducedAt_timet(pNotInfo, &t0);
	  // verify SignatureTimeStamp
     pTS = ddocSigInfo_GetTypestampOfType(pSigInfo, TS_TYPE_SIGNATURE);
     if(pTS) {
	   pTsaCaCert = Digi_FindTSACACert(&hStore, pTS);
	   if(!pTsaCaCert) {
			SET_LAST_ERROR(ERR_TSA_NOT_TRUSTED);
			return ERR_TSA_NOT_TRUSTED;
	   }
       nCAs = 1;
       tsaCaCerts[0] = pTsaCaCert;
	   tsaCaCerts[1] = tsaCaCerts[2] = 0;     
       err = ddocVerifyTimestampInfo(pTS, tsaCaCerts, 0, &t1, &pTsaCert);
      if(err)
	     SET_LAST_ERROR(err);
	  /*if(pTsaCert)
		X509_free(pTsaCert);*/
	  if(pTsaCaCert)
		X509_free(pTsaCaCert);
    }
    // verify SigAndRefsTimeStamp
    pTS = ddocSigInfo_GetTypestampOfType(pSigInfo, TS_TYPE_SIG_AND_REFS);
    if(!err && pTS) {
	  pTsaCaCert = Digi_FindTSACACert(&hStore, pTS);
	   if(!pTsaCaCert) {
			SET_LAST_ERROR(ERR_TSA_NOT_TRUSTED);
			return ERR_TSA_NOT_TRUSTED;
	   }
       nCAs = 1;
       tsaCaCerts[0] = pTsaCaCert;
	   tsaCaCerts[1] = tsaCaCerts[2] = 0;
      err = ddocVerifyTimestampInfo(pTS, tsaCaCerts, 0, &t2, &pTsaCert);
      if(err)
	    SET_LAST_ERROR(err);
	  /*if(pTsaCert)
		X509_free(pTsaCert);*/
	  if(pTsaCaCert)
		X509_free(pTsaCaCert);
    }	
	// check times
	// check timestamp dates
    if(!err && t1 > t2) { // SignatureTimeStamp must be before SigAndRefsTimeStamp
      ddocDebug(1, "Digi_verifyNotaryInfoWithCertSearch", "SignatureTimeStamp %ld must be before SigAndRefsTimeStamp %ld", (long)t1, (long)t2);
      err = ERR_TS_VERIFY;
	  SET_LAST_ERROR(err);
    }    
    // OCSP timestamp must be between SignatureTimeStamp and SigAndRefsTimeStamp
    if(!err && (t0 < (t1 - pProfile->g_nTsaMaxTsInterval) 
		|| t1 > (t2 + pProfile->g_nTsaMaxTsInterval))) { 
	  time_t2str(t0, buf1, sizeof(buf1));
      time_t2str(t1 - pProfile->g_nTsaMaxTsInterval, buf2, sizeof(buf2));
	  time_t2str(t2 + pProfile->g_nTsaMaxTsInterval, buf3, sizeof(buf3));
	  ddocDebug(1, "Digi_verifyNotaryInfoWithCertSearch", "OCSP: %s SignatureTimeStamp %s must be before SigAndRefsTimeStamp %s", 
		  buf1, buf2, buf3);
      err = ERR_TS_VERIFY;
	  SET_LAST_ERROR(err);
    }
  }
#endif
  //siin leitakse responderi cert vastavalt kirjeldusele
  //notCert = Digi_FindResponderCert(&hStore,cert);
  //08.03.2005 leitakse responderi koopia win certstoorest 
  if(!err) {
	notCert = Digi_FindX509CopyFromStore(&hStore,cert); 
	if (notCert) 
		err = Digi_verifyNotaryInfoCERT(pSigDoc, pNotInfo, (const X509**)caCerts, notCert);
	else
		err = ERR_OCSP_CERT_NOTFOUND;  
  }
  Digi_CloseStore(&hStore);
  Digi_FreeCertList(caCerts);
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}

//============================================================
// Verifies the whole document, but returns on first error
// Use the functions defined earlier to verify all contents
// step by step.
// pSigDoc - signed doc data
// 
//============================================================
int Digi_verifySigDocWithCertSearch(const SignedDoc* pSigDoc, const char* szDataFile, FilePath* pPaths, int nPaths)

{
	SignatureInfo* pSigInfo;
	NotaryInfo* pNotInfo;
	int i, d, err = ERR_OK;
	
	RETURN_IF_NULL(pSigDoc);
	//assert(pSigDoc);
	d = getCountOfSignatures(pSigDoc);
	for(i = 0; i < d; i++) {
		pSigInfo = getSignature(pSigDoc, i);
		err = Digi_verifySignatureInfo(pSigDoc, pSigInfo, szDataFile, pPaths, nPaths);
		RETURN_IF_NOT(err == ERR_OK, err);
		pNotInfo = pSigInfo->pNotary;
		err = Digi_verifyNotaryInfoWithCertSearch(pSigDoc, pNotInfo);
		RETURN_IF_NOT(err == ERR_OK, err);
	}
	return err;
}


//--------------------------------------------------
// Reads a certificate from pkcs12 conteiner
//--------------------------------------------------
// AA 18/09/2003
EXP_OPTION int Digi_readCertificateByPKCS12OnlyCertHandle(const char *pkcs12file, const char * passwd, X509 **x509)
{
  int err = ERR_OK;
  EVP_PKEY *pkey;
  err = ReadCertificateByPKCS12(x509,pkcs12file,passwd,&pkey);
	//AM 22.05.08 pKey should be freed
	EVP_PKEY_free(pkey);
  RETURN_IF_NOT(err == ERR_OK, err); 
  return ERR_OK; 
}

//============================================================
// Verifies this signature
// pSigDoc - signed doc data
// pSigInfo - signature info object
// signerCA - direct signer CA certs filename
// szDateFile - name of the digidoc file
// bUseCA - use CA certs or not 1/0
// pPaths - file name / path pairs for detatched files
// nPaths - number of file name / path pairs
// from original file and use it for hash function.
// This is usefull if the file has been generated by
// another library and possibly formats these elements
// differently.
//============================================================
int Digi_verifySignatureInfo(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo, 
       						const char* szDataFile, FilePath* pPaths, int nPaths)
{
	PCCERT_CONTEXT pCert = NULL;
	char buf2[100], *p1 = 0;
    X509* pCaCert = 0;
    int err = ERR_OK;
    StoreHandle hStore;

    if(!Digi_OpenStore(&hStore))
		return ERR_CERT_STORE_READ;
	p1 = ddocSigInfo_GetSignersCert_IssuerName(pSigInfo);
	//strncpy(buf1, ddocSigInfo_GetSignersCert_IssuerName(pSigInfo), sizeof(buf1));
	if(p1) {
	  findCN((char*)p1, buf2);
      pCert = Digi_FindCertBySubject(&hStore, buf2, TRUE, 0);
	}
    if(pCert != NULL) 
      ddocDecodeX509Data(&pCaCert,pCert->pbCertEncoded,pCert->cbCertEncoded);
	err = verifySignatureInfoCERT(pSigDoc, pSigInfo, pCaCert, 
		szDataFile, (pCaCert != NULL), pPaths, nPaths);
    if(pCaCert)
		X509_free(pCaCert);
	Digi_CloseStore(&hStore);
	return err;
}


//============================================================
// Verifies the whole document, but returns on first error
// Use the functions defined earlier to verify all contents
// step by step.
// pSigDoc - signed doc data
// 
//============================================================

int Digi_verifySigDoc(const SignedDoc* pSigDoc, const char* szDataFile, FilePath* pPaths, int nPaths)

{
	SignatureInfo* pSigInfo;
	NotaryInfo* pNotInfo;
	int i, d, err = ERR_OK;
	
	RETURN_IF_NULL_PARAM(pSigDoc);
	//assert(pSigDoc);
	d = getCountOfSignatures(pSigDoc);
	for(i = 0; i < d; i++) {
		pSigInfo = getSignature(pSigDoc, i);
		err = Digi_verifySignatureInfo(pSigDoc, pSigInfo, szDataFile, pPaths, nPaths);
		RETURN_IF_NOT(err == ERR_OK, err);
	}
	d = getCountOfNotaryInfos(pSigDoc);
	for(i = 0;i < d; i++) {
		pNotInfo = getNotaryInfo(pSigDoc, i);
		err = Digi_verifyNotaryInfoWithCertSearch(pSigDoc, pNotInfo);
		RETURN_IF_NOT(err == ERR_OK, err);
	}
	return ERR_OK;
}


//--------------------------------------------------
// Verfies NotaryInfo signature
// pSigDoc - signed doc object
// pNotInfo - NotaryInfo object
// caCerts - CA certificate pointer array terminated with NULL
// CApath - path to (directory) all certs
// notCertFile - Notary (e.g. OCSP responder) cert file 
//--------------------------------------------------
int Digi_verifyNotaryInfoCERT(const SignedDoc* pSigDoc, 
				    const NotaryInfo* pNotInfo,
				    const X509** caCerts, 
				    const X509* notCert)
{
	SignatureInfo* pSigInfo = NULL;
	pSigInfo = ddocGetSignatureForNotary(pSigDoc, pNotInfo);
    RETURN_IF_NOT(pSigInfo != NULL, ERR_NOTARY_NO_SIGNATURE);
	return verifyNotaryInfoCERT(pSigDoc, pSigInfo, pNotInfo, caCerts, 0, notCert);
}


//====================================================================
// Finds issuer certificate of given certificate
// returns NULL if not found or any error occures
// The stores "My","CA" and "Root" will be scanned.
//====================================================================
EXP_OPTION X509 * findIssuerCertificatefromMsStore(X509 *x509){
	return findIssuerCertificatefromStore(x509);

}

//Functions from EstIDLib.c


//============================================================
// Calculates and stores a signature for this SignatureInfo object
// Uses EstEID card as CSP to sign the info
// pSigInfo - signature info object
//============================================================
EXP_OPTION int calculateSigInfoSignatureWithCSPEstID(SignedDoc* pSigDoc, SignatureInfo* pSigInfo, int iByKeyContainer)
{
  int err = ERR_OK, l1;
  PCCERT_CONTEXT  pCert;
  char buf1[50];
  unsigned long sigLen;
  int digLen, len=0;
  char sigDig[100],signature[2084], *p1;
  char *psKeyName = NULL;
  char *psCSPName = NULL;
  //long tmpSerial;
  //HCRYPTPROV hProvider;
  DWORD dwRes;
  X509 *pX509;
  DigiDocMemBuf *pMBuf1;

  ddocDebug(3, "calculateSigInfoSignatureWithCSPEstID", "Sign id: %s", pSigInfo->szId);
  pCert = DigiCrypt_FindContext((BOOL) iByKeyContainer, &dwRes);
  if(pCert == NULL) {
  if (dwRes == dDigiCrypt_Error_NotFoundCSP)
	 err = ERR_CSP_NO_CARD_DATA;
  if(dwRes == dDigiCrypt_Error_UserCancel)
	 err = ERR_CSP_USER_CANCEL;
  if (dwRes == dDigiCrypt_Error_NoDefaultKey)
     err = ERR_CSP_NODEFKEY_CONTAINER;
  if (dwRes == dDIgiCrypt_Error_NotFoundCert) 
	 err = ERR_CSP_CERT_FOUND;

  SET_LAST_ERROR_RETURN_CODE(err);
  }
  psKeyName = DigiCrypt_FindContext_GetKeyName();
  psCSPName = DigiCrypt_FindContext_GetCSPName();
  //AA100204
  ddocDecodeX509Data(&pX509,pCert->pbCertEncoded,pCert->cbCertEncoded);
    // 1) we add certificate to digidoc 
    // set signers cert
  err = setSignatureCert(pSigInfo, pX509);
  RETURN_IF_NOT(err == ERR_OK, err);

	//AA-Viimase minuti jama
	createTimestamp(pSigDoc, sigDig, sizeof(sigDig));
	setString(&(pSigInfo->szTimeStamp), sigDig, -1);


	// Signed properties digest
	// now calculate signed properties digest
    err = calculateSignedPropertiesDigest(pSigDoc, pSigInfo);
    // TODO: replace later
    pMBuf1 = ddocDigestValue_GetDigestValue(pSigInfo->pSigPropDigest);
    ddocSigInfo_SetSigPropRealDigest(pSigInfo, (const char*)pMBuf1->pMem, pMBuf1->nLen);
    // signature type & val
    ddocSignatureValue_new(&(pSigInfo->pSigValue), 0, SIGN_RSA_NAME, 0, 0);
    // calc signed-info digest
    l1 = sizeof(buf1);
    err = calculateSignedInfoDigest(pSigDoc, pSigInfo, (byte*)buf1, &l1);
    err = ddocSigInfo_SetSigInfoRealDigest(pSigInfo, buf1, l1);

	sigLen = sizeof(signature);
	memset(signature, 0, sizeof(signature));
    p1 = createXMLSignedInfo(pSigDoc, pSigInfo);
    len = strlen(p1);

    // sign the <SignedInfo> hash with CSP    
	digLen = sizeof(sigDig);
    err = GetSignedHashWithKeyAndCSP(psKeyName,psCSPName, p1, len, NULL, NULL, sigDig, &digLen,&signature[0],&sigLen);
	if(p1)
		free(p1);
	RETURN_IF_NOT(err == ERR_OK, err);
	// set signature value
	ddocSigInfo_SetSignatureValue(pSigInfo, signature, (int)sigLen);
	ddocDebug(3, "calculateSigInfoSignatureWithCSPEstID", "End signing Sign id: %s, rc: %d", pSigInfo->szId, err);
    return ERR_OK;
}



X509* findCertificate(const CertSearch * cS){
	//debugPrint("findCertificate");
	X509* poCert = NULL;
	if(cS==NULL){
		return NULL;
	}else if(cS->searchType==CERT_SEARCH_BY_X509){
		ReadCertificate(&poCert,cS->x509FileName);
		return(poCert);
	}else if(cS->searchType==CERT_SEARCH_BY_PKCS12){
		X509 * x509=NULL;
		EVP_PKEY * pKey=NULL;
		int err=0;
		err=ReadCertificateByPKCS12(&x509,cS->pkcs12FileName,cS->pswd,&pKey);
		EVP_PKEY_free(pKey);
		if(err==ERR_OK){
			return x509;
		}else{
			SET_LAST_ERROR(err);
			return NULL;
		}
	}else if(cS->searchType==CERT_SEARCH_BY_STORE){
		#ifdef WIN32_CSP
		X509 * x509=NULL;
		int err=0;
		RETURN_OBJ_IF_NULL(cS->certSearchStore, NULL);
		err=GetCertificateFromStore(cS->certSearchStore,&x509);
		//debugPrint("return from findCertificate (MS Store)");
		if(err==ERR_OK){
			return x509;
		}else{
			SET_LAST_ERROR(err);
			return NULL;
		}
		#else
		return NULL;
		#endif
	}else{
		return NULL;
	}
}

EXP_OPTION int findAllCertificates(const CertSearchStore *sS, X509 ***certsArray, int *numberOfCerts){
	#ifdef WIN32_CSP
	int rc=0,i;
	X509 **array;
	CertItem *certItem, *certItem2;
	rc=GetAllCertificatesFromStore(sS,&certItem,numberOfCerts);
	if(rc==ERR_OK){
		array=malloc(sizeof(X509*)*(*numberOfCerts));
		RETURN_IF_BAD_ALLOC(array);
		for(i=0;i< *numberOfCerts ;i++){
			array[i]=certItem->pCert;
			certItem2=certItem;
			certItem=certItem->nextItem;
			free(certItem2);
		}
		*certsArray=array;
	}
	if (rc != ERR_OK) SET_LAST_ERROR(rc);
	return rc;
	#else
	SET_LAST_ERROR_RETURN_CODE(ERR_UNSUPPORTED_CERT_SEARCH);
	#endif // WIN32_CSP
}

/***********************************************************************/
//=========================================================================
// Returns pointer to current CSP. If current CSP was not initialized (i.e
// NULL ) and input parameter is TRUE then active CSP is asked from  
// function getActiveProvider() and if CSP is "EST ID CSP" then 
// signature flag in CSP structure  will turned to true
//=========================================================================
EXP_OPTION CSProvider * getCurrentCSProvider(BOOL tryToFindIfMissing){
	if(tryToFindIfMissing==FALSE){
		return cCSProvider;
	}
	// so tryToFindIfMissing was true, we need to find out if not found yet
	if(cCSProvider==NULL ){
		//cCSProvider=getActiveProvider();
		if(cCSProvider!=NULL){
			if(strcmp(cCSProvider->CSPName,EST_EID_CSP)==0)
				cCSProvider->at_sig=TRUE;
			else
				cCSProvider->at_sig=FALSE;
		}
	}
	return cCSProvider;
	
}
EXP_OPTION void setCurrentCSProvider(CSProvider * newProvider){
	cCSProvider=newProvider;
}



//=====================================================
EXP_OPTION CertSearchStore* CertSearchStore_new()
{
	CertSearchStore* certSearch;
	certSearch=(CertSearchStore*)malloc(sizeof(CertSearchStore));
	memset(certSearch, 0, sizeof(CertSearchStore));
	return certSearch;
}

EXP_OPTION void CertSearchStore_free(CertSearchStore* certSearchStore){
	int i=0;
	RETURN_VOID_IF_NULL(certSearchStore);
	if (certSearchStore->subDNCriterias) {
		for(; i < certSearchStore->numberOfSubDNCriterias ; i++ ){
			if(certSearchStore->subDNCriterias[i] != NULL){
				free(certSearchStore->subDNCriterias[i]);
			}
		}
		free(certSearchStore->subDNCriterias);
	}
	certSearchStore->numberOfSubDNCriterias=0;
	if (certSearchStore->issDNCriterias) {
		for(; i < certSearchStore->numberOfIssDNCriterias ; i++ ){
			if(certSearchStore->issDNCriterias[i] != NULL){
				free(certSearchStore->issDNCriterias[i]);
			}
		}
		free(certSearchStore->issDNCriterias);
	}
	certSearchStore->numberOfIssDNCriterias=0;
	if(certSearchStore->storeName != NULL){
		free(certSearchStore->storeName);
	}
	if(certSearchStore->publicKeyInfo){
		free(certSearchStore->publicKeyInfo);
	}
	free(certSearchStore);
	certSearchStore=NULL;
}


EXP_OPTION CertSearch* CertSearch_new(){
	CertSearch* certSearch;
	certSearch=(CertSearch*)malloc(sizeof(CertSearch));
	memset(certSearch, 0, sizeof(CertSearch));
	return certSearch;
}

EXP_OPTION void CertSearch_free(CertSearch* certSearch){
	if(certSearch->certSearchStore != NULL){
		CertSearchStore_free(certSearch->certSearchStore);
	}
	if(certSearch->pkcs12FileName != NULL){
		free(certSearch->pkcs12FileName);
	}
	if(certSearch->pswd != NULL){
		free(certSearch->pswd);
	}
	if(certSearch->x509FileName != NULL){
		free(certSearch->x509FileName);
	}
	if(certSearch->keyFileName != NULL){
		free(certSearch->keyFileName);
	}
	free(certSearch);
	certSearch=NULL;

}

EXP_OPTION void CertSearch_setX509FileName(CertSearch* certSearch, const char* str)
{
	if(certSearch && str) {
		setString((char**)&certSearch->x509FileName, str, -1);
	}
}

EXP_OPTION void CertSearch_setKeyFileName(CertSearch* certSearch, const char* str)
{
	if(certSearch && str) {
		setString((char**)&certSearch->keyFileName, str, -1);
	}
}

EXP_OPTION void CertSearch_setPkcs12FileName(CertSearch* certSearch, const char* str)
{
	if(certSearch && str) {
		setString((char**)&certSearch->pkcs12FileName, str, -1);
	}
}

EXP_OPTION void CertSearch_setPasswd(CertSearch* certSearch, const char* str)
{
	if(certSearch && str) {
		setString((char**)&certSearch->pswd, str, -1);
	}
}



// Frees cert handles and items in this list. 
// WARNING! when caller does not own the first cert in chain,
// then pass pListStart->nextItem instead of pListStart itself.
EXP_OPTION void CertList_free(CertItem* pListStart) {
	if (pListStart) {
		CertItem* pItem = pListStart;
		CertItem* pTmp;
		while (pItem) {
			pTmp = pItem->nextItem;
			X509_free((X509*)pItem->pCert);
			free(pItem);
			pItem = pTmp;
		}
	}
}

//=====================================================================
// reads certificate from system store
// IN  CertSearchStore *sS - search criterias
// OUT x509 certificate
//=====================================================================
int GetAllCertificatesFromStore(const CertSearchStore *sS, CertItem ** certList, int *numberOfCerts)
{
	int retCode=ERR_OK;
/*
	int retCode1=ERR_UNSUPPORTED_CERT_SEARCH; // Unknown search type
	int retCode2=ERR_CSP_OPEN_STORE; // Can not open system store
	int retCode3=ERR_CSP_CERT_FOUND; // Certificate not found from store, probably cetificate not registered
	int retCode4=ERR_INCORRECT_CERT_SEARCH; // search type Sub DN but mismatch of parameters
	int retCode5=ERR_PKCS_CERT_DECODE; 
*/

	int i=0,certCounter=0;
	int initSize=10;
	X509 *x509;
	CertItem *certFirst,*certCurrent,*certIt;
	BOOL certFound=FALSE;
	BOOL useSerial=FALSE;
	BOOL useSubDN=FALSE;
	BOOL useIssDN=FALSE;
	HCERTSTORE hCertStore = NULL;
	PCCERT_CONTEXT pCert=NULL;
	char defaultName[]="My";
	char* storeName;
	char buf[4000];
	char buf1[4000];
  long tmpSerial;
	int err;
	DigiDocMemBuf mbuf1;
	mbuf1.pMem = 0;
	mbuf1.nLen = 0;

	RETURN_IF_NOT(numberOfCerts !=NULL,  ERR_NULL_SER_NUM_POINTER);

	if((sS->searchType&(CERT_STORE_SEARCH_BY_SERIAL|CERT_STORE_SEARCH_BY_SUBJECT_DN|CERT_STORE_SEARCH_BY_ISSUER_DN)) == 0)
		SET_LAST_ERROR_RETURN_CODE(ERR_UNSUPPORTED_CERT_SEARCH);
	
	if((sS->searchType&CERT_STORE_SEARCH_BY_SERIAL) != 0){
		useSerial=TRUE;
	}
	if((sS->searchType&CERT_STORE_SEARCH_BY_SUBJECT_DN) != 0){
		useSubDN=TRUE;
		if(sS->numberOfSubDNCriterias<1)
			SET_LAST_ERROR_RETURN_CODE(ERR_INCORRECT_CERT_SEARCH);
		if (sS->subDNCriterias == NULL)
			SET_LAST_ERROR_RETURN_CODE(ERR_INCORRECT_CERT_SEARCH);
		for(i=0;i<sS->numberOfSubDNCriterias;i++){
			if(sS->subDNCriterias[i]==NULL)
				SET_LAST_ERROR_RETURN_CODE(ERR_INCORRECT_CERT_SEARCH);
		}
	}
	if((sS->searchType&CERT_STORE_SEARCH_BY_ISSUER_DN) != 0){
		useIssDN=TRUE;
		if(sS->numberOfIssDNCriterias<1)
			SET_LAST_ERROR_RETURN_CODE(ERR_INCORRECT_CERT_SEARCH);
	
		if (sS->issDNCriterias == NULL)
			SET_LAST_ERROR_RETURN_CODE(ERR_INCORRECT_CERT_SEARCH);
		for(i=0;i<sS->numberOfIssDNCriterias;i++){
			if(sS->issDNCriterias[i]==NULL)
				SET_LAST_ERROR_RETURN_CODE(ERR_INCORRECT_CERT_SEARCH);
		}
	}
	if(sS->storeName!=NULL){
		storeName=sS->storeName;
	}else{
		storeName=defaultName;
	}
	certFirst = malloc(sizeof(CertItem));
	RETURN_IF_BAD_ALLOC(certFirst);
	// memset(certFirst,0,sizeof(certFirst)); <-- Andrus: fills with zeros only first 32 bits
	memset(certFirst,0,sizeof(CertItem));
	*certList=certFirst;
	certCurrent=certFirst;
	/////******************** start task ***********************************
	while(TRUE){
		hCertStore=CertOpenSystemStore(0,storeName);
		if(!hCertStore){
			retCode=ERR_CSP_OPEN_STORE;
			SET_LAST_ERROR(retCode);
			break;
		}
		
		while(TRUE){
			pCert = CertEnumCertificatesInStore( hCertStore,pCert);
			if(!pCert)
				break;
			//AA100204
			ddocDecodeX509Data(&x509,pCert->pbCertEncoded,pCert->cbCertEncoded);
			RETURN_IF_NOT(x509 != NULL, ERR_PKCS_CERT_DECODE);

			// if serial is a criteria check it
			if(useSerial){
				//AA-viimase minuti jama
                //ReadCertSerialNumber(&tmpSerial,x509);
				ReadCertSerialNumber(buf,sizeof(buf),x509);
				tmpSerial = atol(buf);
				if(tmpSerial == sS->certSerial){
					certFound=TRUE;
				}else{
					certFound=FALSE;
					X509_free(x509);
					continue;
				}
			}//if(useSerial){

			// if issuer name is a criteria check it
			if(useIssDN){
				int len=0;
				X509_NAME * x509name;
				x509name = X509_get_issuer_name(x509);
				len=sizeof(buf);
				memset(buf,0,len);
				memset(buf1,0,sizeof(buf1));
				//AM 26.09.08
				//X509_NAME_oneline(x509name,buf,len);
				err = ddocCertGetIssuerDN(x509, &mbuf1);
				RETURN_IF_NOT(err == ERR_OK, err);
				len=strlen((char*)mbuf1.pMem);
				prepareString((char*)mbuf1.pMem,buf1);
				for(i=0;i<sS->numberOfIssDNCriterias;i++){
					memset(buf,0,sizeof(buf));
					prepareString(sS->issDNCriterias[i],buf);
					if(strstr(buf1,buf)){
						certFound=TRUE;
					}else{
						certFound=FALSE;
					  //X509_free(x509);  <-- Andrus: free operation performed in the end of useIssDN block
						break;
					}
				}
        if(!certFound){
					X509_free(x509);
					continue;
				}
			}//if(useIssDN){
			
			// if subject name is a criteria check it
			if(useSubDN){
				int len=0;
				X509_NAME * x509name;
				x509name = X509_get_subject_name(x509);
				len=sizeof(buf);
				memset(buf,0,len);
				memset(buf1,0,sizeof(buf1));
				//AM 26.09.08
				//X509_NAME_oneline(x509name,buf,len);
				err = ddocCertGetSubjectDN(x509, &mbuf1);
				RETURN_IF_NOT(err == ERR_OK, err);
				len=strlen((char*)mbuf1.pMem);
				prepareString((char*)mbuf1.pMem,buf1);
				for(i=0;i<sS->numberOfSubDNCriterias;i++){
					memset(buf,0,sizeof(buf));
					prepareString(sS->subDNCriterias[i],buf);
					if(strstr(buf1,buf)){
						certFound=TRUE;
					}else{
						certFound=FALSE;
						break;
					}
				}
				// did we find ?
				if(!certFound){
					X509_free(x509);
					continue;
				}
			}//if(useSubDN){
			ddocMemBuf_free(&mbuf1);
			// did we find ?
			if(certFound){
				certCounter++;
				if(certCounter==1){
					certFirst->pCert=x509;
					continue;
				}
				certIt = malloc(sizeof(CertItem));
				RETURN_IF_BAD_ALLOC(certIt);
				// memset(certIt,0,sizeof(certIt)); <-- Andrus: fills with zeros only first 32 bits
				memset(certIt,0,sizeof(CertItem));
				certIt->pCert=x509;
				certCurrent->nextItem=certIt;
				certCurrent=certIt;
			}else{
				X509_free(x509);
				continue;
			}
		}//while(pCertt=CertEnumCertificatesInStore(hCertStore,pCert))
		*numberOfCerts=certCounter;
		if( certCounter<1 ){
			retCode = ERR_CSP_CERT_FOUND;
			SET_LAST_ERROR(retCode);
			break;
		}
		break;
	}//while(TRUE){
	/////******************** close and free objects ***********************************
	if(pCert)			{CertFreeCertificateContext(pCert);		}
	if(hCertStore)		{CertCloseStore(hCertStore,CERT_CLOSE_STORE_FORCE_FLAG);hCertStore=0;}
	return retCode;
	/////*******************************************************************************
}
//==========================================================
// removes all spaces and tabs and makes all characters uppercase
//==========================================================
void prepareString(const char * strIN,char * strOUT){
	int len=0;
	int i=0;
	int tempLen=0;
	len=strlen(strIN);
	
	if(len==0){
		return;
	}
	for (;i<len ; i++) {
		if((strIN[i] == ' ') || (strIN[i] == '\t')){
			continue;
		}else if( strIN[i]>0x60 && strIN[i]<0x7B ){
			strOUT[tempLen++] = (strIN[i]-0x20);
		}else {
			strOUT[tempLen++] = strIN[i];
		}
	}
}

//=====================================================================
// reads certificate from system store
// IN  CertSearchStore *sS - search criterias
// OUT x509 certificate
//=====================================================================
int GetCertificateFromStore(const CertSearchStore *sS, X509 **cert){
	int retCode=ERR_OK;
	int retCode1=ERR_UNSUPPORTED_CERT_SEARCH; // Unknown search type
	int retCode2=ERR_CSP_OPEN_STORE; // Can not open system store
	int retCode3=ERR_CSP_CERT_FOUND; // Certificate not found from store, probably cetificate not registered
	int retCode4=ERR_INCORRECT_CERT_SEARCH; // search type Sub DN but mismatch of parameters
	int retCode5=ERR_PKCS_CERT_DECODE; 

	int i=0;
	X509 *x509;
	BOOL certFound=FALSE;
	BOOL useKeyInfo=FALSE;
	BOOL useSerial=FALSE;
	BOOL useSubDN=FALSE;
	BOOL useIssDN=FALSE;
	HCERTSTORE hCertStore = NULL;
	CERT_PUBLIC_KEY_INFO* pKeyInfo;
	PCCERT_CONTEXT pCert=NULL;
	char defaultName[]="My";
	char* storeName;
	char buf[3000];
	char buf1[3000];
    long tmpSerial;
	int err=0;
	DigiDocMemBuf mbuf1;
	mbuf1.pMem = 0;
	mbuf1.nLen = 0;

	//---------------------------------
	// TODO: it's just a test - remove it
	//---------------------------------
	/*
	char* pSelArr[] = {"Good reader", "Better reader", "The best reader", NULL};
	int iRes = runDigiDocDialogUnit(pSelArr, "Please select smartcard reader:");
	if (iRes != -1) {
		// handle picked element
		char resultAsText[128];
		//sprintf(resultAsText, "Selected item with index=%d", iRes);
		MessageBox(NULL, resultAsText, "DigiDoc", MB_OK|MB_ICONEXCLAMATION);
	} else {
		// if possible, handle Cancel operation
	}
	*/
	//---------------------------------
	// End of TODO
	//---------------------------------
	if((sS->searchType&(CERT_STORE_SEARCH_BY_SERIAL|CERT_STORE_SEARCH_BY_SUBJECT_DN|CERT_STORE_SEARCH_BY_ISSUER_DN|CERT_STORE_SEARCH_BY_KEY_INFO)) == 0)
		SET_LAST_ERROR_RETURN_CODE(ERR_UNSUPPORTED_CERT_SEARCH);
	if((sS->searchType&CERT_STORE_SEARCH_BY_KEY_INFO) != 0){
		useKeyInfo=TRUE;
		pKeyInfo=sS->publicKeyInfo;
		if(pKeyInfo==NULL){
			SET_LAST_ERROR_RETURN_CODE(ERR_INCORRECT_CERT_SEARCH);
		}
	}
	if((sS->searchType&CERT_STORE_SEARCH_BY_SERIAL) != 0){
		useSerial=TRUE;
	}
	if((sS->searchType&CERT_STORE_SEARCH_BY_SUBJECT_DN) != 0){
		useSubDN=TRUE;
		if(sS->numberOfSubDNCriterias<1)
			SET_LAST_ERROR_RETURN_CODE(ERR_INCORRECT_CERT_SEARCH);
		if (sS->subDNCriterias == NULL)
			SET_LAST_ERROR_RETURN_CODE(ERR_INCORRECT_CERT_SEARCH);
		for(i=0;i<sS->numberOfSubDNCriterias;i++){
			if(sS->subDNCriterias[i]==NULL)
				SET_LAST_ERROR_RETURN_CODE(ERR_INCORRECT_CERT_SEARCH);
		}
	}
	if((sS->searchType&CERT_STORE_SEARCH_BY_ISSUER_DN) != 0){
		useIssDN=TRUE;
		if(sS->numberOfIssDNCriterias<1)
			SET_LAST_ERROR_RETURN_CODE(ERR_INCORRECT_CERT_SEARCH);
		if (sS->issDNCriterias == NULL)
			SET_LAST_ERROR_RETURN_CODE(ERR_INCORRECT_CERT_SEARCH);
		for(i=0;i<sS->numberOfIssDNCriterias;i++){
			if(sS->issDNCriterias[i]==NULL)
				SET_LAST_ERROR_RETURN_CODE(ERR_INCORRECT_CERT_SEARCH);
		}
	}
	if(sS->storeName!=NULL){
		storeName=sS->storeName;
	}else{
		storeName=&defaultName[0];
	}
	/////******************** start task ***********************************
	while(TRUE){
		hCertStore=CertOpenSystemStore(0,storeName);
		if(!hCertStore){
			retCode = ERR_CSP_OPEN_STORE;
			SET_LAST_ERROR(retCode);
			break;
		}
		if(useKeyInfo){
			pCert=CertFindCertificateInStore(hCertStore,X509_ASN_ENCODING|PKCS_7_ASN_ENCODING,0,CERT_FIND_PUBLIC_KEY,pKeyInfo,NULL );
            if(pCert){
				//AA100204
				ddocDecodeX509Data(&x509,pCert->pbCertEncoded,pCert->cbCertEncoded);
				certFound=TRUE;
                //memcpy(cert,x509,sizeof(x509));
                //X509_free(x509);
                *cert=x509;
				break;
			}
		}//if(useSerial){
		while( pCert= CertEnumCertificatesInStore( hCertStore,pCert)){
			//AA100204
			ddocDecodeX509Data(&x509,pCert->pbCertEncoded,pCert->cbCertEncoded);
			RETURN_IF_NOT(x509 != NULL, ERR_PKCS_CERT_DECODE);
			if(useSerial){
                //AA-Viimase minuti jama
				//ReadCertSerialNumber(&tmpSerial,x509);
                ReadCertSerialNumber(buf,sizeof(buf),x509);
				tmpSerial = atol(buf);
				if(tmpSerial == sS->certSerial){
					certFound=TRUE;
				}else{
					certFound=FALSE;
					X509_free(x509);
					continue;
				}
			}//if(useSerial){
			if(useIssDN){
				int len=0;
				X509_NAME * x509name;
				x509name = X509_get_issuer_name(x509);
				len=sizeof(buf);
				memset(buf,0,len);
				memset(buf1,0,sizeof(buf1));
				//AM 26.09.08
				//X509_NAME_oneline(x509name,buf,len);
				err = ddocCertGetIssuerDN(x509, &mbuf1);
				RETURN_IF_NOT(err == ERR_OK, err);
				len=strlen((char*)mbuf1.pMem);
				prepareString((char*)mbuf1.pMem,buf1);
				
				for(i=0;i<sS->numberOfIssDNCriterias;i++){
					memset(buf,0,sizeof(buf));
					prepareString(sS->issDNCriterias[i],buf);
					if(strstr(buf1,buf)){
						certFound=TRUE;
					}else{
						certFound=FALSE;
						X509_free(x509);
						break;
					}
				}
				if(!certFound){
					X509_free(x509);
					continue;
				}
			}//if(useIssDN){
			
			if(useSubDN){
				int len=0;
				X509_NAME * x509name;
				x509name = X509_get_subject_name(x509);
				len=sizeof(buf);
				memset(buf,0,len);
				memset(buf1,0,sizeof(buf1));
				//AM 26.09.08
				//X509_NAME_oneline(x509name,buf,len);
				err = ddocCertGetSubjectDN(x509, &mbuf1);
				RETURN_IF_NOT(err == ERR_OK, err);
				len=strlen((char*)mbuf1.pMem);
				prepareString((char*)mbuf1.pMem,buf1);
				
				for(i=0;i<sS->numberOfSubDNCriterias;i++){
					memset(buf,0,sizeof(buf));
					prepareString(sS->subDNCriterias[i],buf);
					if(strstr(buf1,buf)){
						certFound=TRUE;
					}else{
						certFound=FALSE;
						break;
					}
				}
				// did we find ?
				if(!certFound){
					X509_free(x509);
					continue;
				}
			}//if(useSubDN){
			// did we find ?
			if(certFound){
				break;
			}else{
				X509_free(x509);
				continue;
			}
		}//while(pCertt=CertEnumCertificatesInStore(hCertStore,pCert))
		if( certFound ){
			//memcpy(cert,x509,sizeof(x509));
			//X509_free(x509);
			*cert=x509;
		}else{
			retCode=ERR_CSP_CERT_FOUND;
			SET_LAST_ERROR(retCode);
			break;
		}
		break;
	}//while(TRUE){

	/////******************** close and free objects ***********************************
	if(pCert)			{CertFreeCertificateContext(pCert);		}
	if(hCertStore)		{CertCloseStore(hCertStore,CERT_CLOSE_STORE_FORCE_FLAG);hCertStore=0;}
	return retCode;
	/////*******************************************************************************
}
//=======================================================
//Asks and returns key name or NULL if can not be found. 
//Key name must be freed. Name will be like "AUT_VIISAKAS,VILLU,19901012020"
//IN cProvider - crypto provider name, "EstEID Card CSP" in
// EstID's case.
//=======================================================
LPBYTE getDefaultKeyName(CSProvider * cCSP){
	BOOL fRes;
	HCRYPTPROV hProv = 0;	
	LPBYTE pbContName = NULL;//string, key name to be stored
	DWORD dwContName;
	if(cCSP==NULL){
		cCSP=getCurrentCSProvider(TRUE);
	}
	if(cCSP==NULL){
		return NULL;
	}
	if(cCSP->rsa_full){
		fRes = CryptAcquireContext(&hProv,NULL,cCSP->CSPName, PROV_RSA_FULL, 0);
	}else{
        fRes = CryptAcquireContext(&hProv,NULL,cCSP->CSPName, PROV_RSA_SIG, 0);
	}
	if (RCRYPT_FAILED(fRes)){ return NULL; }
	// asks keypair name, in auth case it is  AUT_<name, given name, code>
	// first size and the the keyname itself
	fRes = CryptGetProvParam(hProv,PP_CONTAINER,NULL,&dwContName,0);
	if (RCRYPT_FAILED(fRes)){ return NULL; }
	//pbContName =(LPBYTE) LocalAlloc(0,dwContName);
	pbContName =(LPBYTE) malloc(dwContName);
	if(pbContName != NULL ){
		fRes = CryptGetProvParam(hProv,PP_CONTAINER,pbContName,&dwContName,0);
	}
	//in pbContName-s there is now ASCII string
	fRes = CryptReleaseContext(hProv,0);
	if (RCRYPT_FAILED(fRes)){ return NULL; }
	hProv =0;
	return pbContName;

}

//========================================================================
// Returns issuer certificate context
//========================================================================
const CERT_CONTEXT * findCertFromStore(const CERT_CONTEXT *cert, const char *storeName)
{
	HCERTSTORE hCertStore = NULL;
	const CERT_CONTEXT *issuer = NULL;
	DWORD flag=CERT_STORE_SIGNATURE_FLAG;
	hCertStore=CertOpenSystemStore(0,storeName);
	if(!hCertStore){
		return NULL;
	}
	issuer = CertGetIssuerCertificateFromStore(hCertStore,cert,NULL,&flag);
	CertCloseStore(hCertStore,0);
	return issuer;
}

//=========================================================================
X509 * findIssuerCertificatefromStore(X509 *x509)
{
	const CERT_CONTEXT *cert = NULL;
	const CERT_CONTEXT *issuer = NULL;
	char * storeNames[] = {"CA","Root","My"};
	
	unsigned char certBlob[5000];
	int i, certBlobLen = sizeof(certBlob);
	X509 *x509issuer = NULL;
	
	RETURN_OBJ_IF_NULL(x509, NULL);	
	//encode(
	memset(certBlob,0,sizeof(certBlob));
	encodeCert(x509,(char *)certBlob,&certBlobLen);
	cert = CertCreateCertificateContext(X509_ASN_ENCODING|PKCS_7_ASN_ENCODING,certBlob,certBlobLen);
	if(cert == NULL){
		return NULL;
	}
	for( i=0;i<3;i++){
		issuer=findCertFromStore(cert,storeNames[i]);
		if(issuer){
			break;
		}
	}
	if(issuer){
		//AA100204
		ddocDecodeX509Data(&x509issuer,issuer->pbCertEncoded,issuer->cbCertEncoded);
		CertFreeCertificateContext(issuer);
	}
	CertFreeCertificateContext(cert);

	return x509issuer;

}


//=====================================================================
// hashes and signes data with EstId card, returns also public_key_blob
// which can be used in order to verify signature
// IN dataToBeSigned - source data buffer
// IN dataLen - how many bytes will be read from source buffer
// OUT pbKeyBlob - public key buffer( corresponding private key was used to sign.
// OUT pbKeyBlobLen - public key length in buffer
// OUT hash - output data buffer for hash
// OUT hashLen - data length in output buffer
// OUT hashedSignature - output data buffer for hashed and signed data
// OUT sigLen - data length in output buffer
//=====================================================================
int GetSignedHashWithKeyAndCSP(  char *psKeyName, char *psCSPName,
							     const char * dataToBeSigned,unsigned long dataLen,
								 unsigned char *pbKeyBlob, unsigned long *pbKeyBlobLen,
								 unsigned char *hash, unsigned long *hashLen,
								 unsigned char * hashedSignature,unsigned long * sigLen){
	int retCode=ERR_OK, l1;
	HCRYPTPROV hProv = 0;	
	HCRYPTHASH sha1 = 0;	
	HCRYPTKEY hKey = 0;
	PCCERT_CONTEXT pCert=NULL;
	BOOL fRes; 
	DWORD dwRes;
	char *p1 = 0;
	
	ddocDebug(3, "GetSignedHashWithKeyAndCSP", "key: %s csp: %s", psKeyName, psCSPName);
	// debug
	ddocDebug(3, "GetSignedHashWithKeyAndCSP", "data to sign: \'%s\' len: %d tlen: %d", dataToBeSigned, dataLen, strlen(dataToBeSigned));
	//////******************** start task *************************************
	while(TRUE){ 
		//
		//sprintf(sTemp,"CSP=%s \nCON=%s\n%d",psCSPName,psKeyName,DigiCrypt_FindContext_GetCSPType(psCSPName));
		//MessageBox(NULL,sTemp,"TEST",MB_OK);
        //
		fRes=CryptAcquireContext(&hProv,psKeyName,psCSPName,DigiCrypt_FindContext_GetCSPType(psCSPName),0);//CRYPT_VERIFYCONTEXT);
		
		if(fRes==FALSE){	retCode=ERR_CSP_NO_CARD_DATA;break;		}
		fRes=CryptCreateHash(hProv,CALG_SHA1,0,0,&sha1);
		if(fRes==FALSE){
			ddocDebug(1, "GetSignedHashWithKeyAndCSP", "CryptCreateHash RC (bool): %d", (int)fRes);
			retCode=ERR_CSP_NO_HASH_START;
			SET_LAST_ERROR(retCode);		
			break;
		}
		// start hash
		fRes=CryptHashData(sha1,dataToBeSigned,dataLen,0);
	    
		if(fRes==FALSE){
			retCode=ERR_CSP_NO_HASH;	
			SET_LAST_ERROR(retCode);
			break;
		}
		if(hash!=NULL){
			ddocDebug(3, "GetSignedHashWithKeyAndCSP", "caling CryptGetHashParam hash: %s len: %ld", hash, *hashLen);				
			fRes=CryptGetHashParam(sha1,HP_HASHVAL, hash,hashLen ,0);
			if(fRes==FALSE){
				ddocDebug(1, "GetSignedHashWithKeyAndCSP", "CryptGetHashParam RC (bool): %d", (int)fRes);
				retCode=ERR_CSP_NO_HASH_RESULT;
				SET_LAST_ERROR(retCode);
				break;
			}
			// debug
			l1 = *hashLen * 2 + 10;
			p1 = (char*)malloc(l1);
			if(p1) {
				memset(p1, 0, l1);
				encode((const byte*)hash, *hashLen, (byte*)p1, &l1);
				ddocDebug(3, "GetSignedHashWithKeyAndCSP", "hash: %s len: %d", p1, l1);
				free(p1);
				p1 = 0;
			}
		}
	    
		if(hashedSignature != NULL)  {
			  //use by standard way is not allways possible -- AT_SIGNATURE -- main thing is  that cert is ok nor the keyspec
			  //instead of that we should use CryptAcquireCertificatePrivateKey function. But then we must change basic context of this function.
			  //workaround -- we try with both keyspecs

		  fRes=CryptSignHash(sha1,AT_SIGNATURE ,NULL,0, hashedSignature,sigLen);
		  if(fRes==FALSE)
		    {
		    dwRes = GetLastError();
			//try again with keyspec AT_KEYEXCHANGE
				if(dwRes==0x8009000d)  // wrong keyspec
				{
					fRes=CryptSignHash(sha1,AT_KEYEXCHANGE ,NULL,0, NULL,sigLen);  //*sigLen should be 128... this can be avoided whi
					fRes=CryptSignHash(sha1,AT_KEYEXCHANGE ,NULL,0, hashedSignature,sigLen);
					if(fRes==FALSE)
					{
						dwRes = GetLastError();
						ddocDebug(1, "GetSignedHashWithKeyAndCSP", "error in CryptSignHash: %ld", dwRes);
		  				retCode=ERR_CSP_SIGN;
						SET_LAST_ERROR(retCode);
						break;
					}
				} else if(dwRes == 0x8010006e) {
					ddocDebug(2, "GetSignedHashWithKeyAndCSP", "user cancelled signing");
					retCode=ERR_CSP_USER_CANCEL;
					SET_LAST_ERROR(ERR_CSP_USER_CANCEL);
					break;
				}
				else
				{
					ddocDebug(1, "GetSignedHashWithKeyAndCSP", "error in signing: %ld", dwRes);
					retCode=ERR_CSP_SIGN;	
					SET_LAST_ERROR(retCode);
					break;
				}
			}
		} else
			ddocDebug(1, "GetSignedHashWithKeyAndCSP", "hashed signature is NULL");
        
		// we must switsh end and begining
		// because windows uses little-endian but verification
		// assumes big-endian
		reverseArray(hashedSignature, *sigLen);

		if(pbKeyBlob!=NULL){
			fRes=CryptGetUserKey(hProv,AT_KEYEXCHANGE,&hKey);
			if(fRes==FALSE){
				ddocDebug(1, "GetSignedHashWithKeyAndCSP", "error in CryptGetUserKey");
				retCode=ERR_CSP_OPEN_KEY;
				SET_LAST_ERROR(retCode);
				break;
			}
			fRes=CryptExportKey(hKey,0,PUBLICKEYBLOB,0,pbKeyBlob,pbKeyBlobLen);
			if(fRes==FALSE){
				ddocDebug(1, "GetSignedHashWithKeyAndCSP", "error in CryptExportKey");
				retCode=ERR_CSP_READ_KEY;
				SET_LAST_ERROR(retCode);
				break;
			}
		}
        
		break;
	}//while(TRUE)
	///********************** free objects ************************************
	if(hKey){
		CryptDestroyKey(hKey);hKey=0;
	}
	if(sha1){
		CryptDestroyHash(sha1);sha1= 0;
	}
	if(hProv){
		CryptReleaseContext(hProv,0);
	}
    ddocDebug(3, "GetSignedHashWithKeyAndCSP", "end of signing RC: %d", retCode);
	return retCode;
	/////*******************************************************************************
}

//===============================================================
// This function is used to change order in a massive. Last byte
// becomes first and so on. 
// IN/OUT array - massive to be changed
// IN arrayLen  -  array's length
//===============================================================
void reverseArray(unsigned char *array, unsigned long arrayLen)
{
	int ri, rj;
	unsigned char t;
    
	for (ri = 0, rj = arrayLen - 1; ri < rj; ++ri, --rj) {
		t = array[ri];
		array[ri] = array[rj];
		array[rj] = t;
	}
}



#endif
