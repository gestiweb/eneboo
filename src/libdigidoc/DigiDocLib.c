//==================================================
// FILE:	DigiDocLib.c
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
// GNU Lesser General Public Licence is available at
// http://www.gnu.org/copyleft/lesser.html
//==========< HISTORY >=============================
//    20.09.2004       Changed createOCSPRequest() to use custom functions for adding
//                     OCSP nonce to OCSP request for digidoc formats 1.0, 1.1 and 1.2
//                     thereby guaranteing that in case of those formats only 20 bytes are sent.
//                     This is necessary to guarantee backward compatibility with OpenSSL 0.9.6
//    22.08.2004       Added DataFile content caching, function ddocGetDataFileCachedData()
//                     and modified functions readSignedDoc(), extractDataFile(), 
//                     ddocExtractDataFile(), ddocGetDataFile() and ddocCopyDataFile() to use it.
//                     Added function ddocSaxReadSignedDocFromMemory()
//                     New debugging interface ddocDebug()
//    12.08.2004       Moved all SAX xml parsing functions to DigiDocSAXParser.c
//                     and fixed some memory problems
//    03.08.2004                Added check to initializeNotaryInfoWithOCSP() to handle 22 byte ASN.1
//                              encoded nonce with OpenSSL 0.9.7d
//                              added a check on output filename for extractDataFile() function.
//                              added a check for verifyNotaryInfoCERT() function to check empty certificate array size.
//                              Added back the function verifyCertificateByOCSP()
//    14.06.2004		Changed generateDataFileXML() to replaces '&' with '&amp;'
//						int file names. Changed handleStartDataFile() acordingly.
//    20.05.2004		Added new error codes to handle OCSP response
//						status and fixed initializeNotaryInfoWithOCSP()
//    19.05.2004		Added a nullpointer test to checkFileExists()
//    12.05.2004		Fixed convertStringToTimeT() by changing
//						timestamp format pattern according to 1.3 format version
//    11.05.2004		Added error code ERR_OCSP_WRONG_URL to sendOCSPRequest
//						to indicate that the given server is not an OCSP server
//    23.03.2004        fixed setSignatureValueFromFile() that was affected
//                      by siganture caching. Fixed getConfirmation() to
//                      allowe sending not signed OCSP requests.
//    17.03.2004        fixed getSignerCode() to handle certs that
//                      have no comma in CN. Fixed NotaryInfo_new to
//                      reset szOrigContent of signature. Added function
//                      handleOCSPCertStatus() to check cert status 
//                      and modified initializeNotaryInfoWithOCSP() and
//                      checkNonceAndCertbyOCSP()
//    11.03.2004	added SignedDoc namespace in format 1.3
//					generateDataFileXML(), createSignedXMLDoc()
//					handleStartDataFile(), 
//					bugs in emptying buffers
//					unicode2ascii(), getSignerLastName()
//					Veiko Sinivee
//	  25.11.2003	added functions calculateFileLength(),
//				verifyCertificateByOCSP() and helper functions
//	  21.11.2003	Fixed SignatureValue decoding problem
//				Veiko Sinivee
//      17.11.2003      Aare Amenberg
//                      Changed error texts
//                      Changed sendOCSPRequest
//                      (added more error checkings) 
//      14.11.2003      Aare Amenberg
//                      Removed WriteOCSPResponse
//                      Changed addNotaryInfoXML by Veiko
//      11.11.2003      Aare Amenberg
//                      Added CorrectCharacters
//                      Added error 96 ERR_WRONG_URL_OR_PROXY
//      31.10.2003      Aare Amenberg
//                      Added error text 95
//                      Removed  if(single->singleExtensions) 
//                               SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_NO_SINGLE_EXT);
//                      Function initializeNotaryInfoWithOCSP
//      29.10.2003      Aare Amenberg
//                      WIN32_CSP functions moved 
//                      to module DigiDocCsp
// 1.0	09.04.2002	Veiko Sinivee	
//  Supports XML format (Type: SK-XML-1.0)
//==================================================

#include <libdigidoc/DigiDocDefs.h>

#define WIN32_PKCS 1

#ifdef WIN32
  #include <windows.h>
  #include <wincrypt.h>
#endif

#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocSAXParser.h>
#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocConvert.h>
#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/DigiDocGen.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
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
#include <openssl/rand.h>

long int tzone = -7200;	/* default for Estonia, but see initDigiDocLib() */
int daylight = 0;		/* default, but see initDigiDocLib() */

//==========< global constants >====================

char g_szLibName[] = "CDigiDoc";
char g_szLibVer[] = DIGIDOC_VERSION;
//char g_szLibVer[] = VERSION;
char* g_szSupportedVersions[] = { "1.0", "1.1", "1.2", "1.3" 
#ifdef WITH_TS
				  , "1.4"
#endif
 };

char* g_arrFormats[] = { SK_XML_1_NAME, 0 };

FormatAndVer g_supportedFormatsAndVersions[] = {
	{ SK_XML_1_NAME, SK_XML_1_VER },
	{ DIGIDOC_XML_1_1_NAME, DIGIDOC_XML_1_1_VER },
	{ DIGIDOC_XML_1_1_NAME, DIGIDOC_XML_1_2_VER },
	{ DIGIDOC_XML_1_1_NAME, DIGIDOC_XML_1_3_VER },
#ifdef WITH_TS
	{ DIGIDOC_XML_1_1_NAME, DIGIDOC_XML_1_4_VER },
#endif
	{ NULL, NULL } // list end marker
};

// static buffer for name & version of the program using this library
#define GUI_VERSION_LEN   100
char g_szGUIVersion[GUI_VERSION_LEN];

//==========< global variables >====================



// forward deklaratsioon
int notary2ocspBasResp(const SignedDoc* pSigDoc, const NotaryInfo* pNotInfo, X509* notCert, OCSP_BASICRESP** pBasResp);
int calculateOcspBasicResponseDigest(OCSP_BASICRESP* pBsResp, byte* digBuf, int* digLen);

int createOCSPRequest(SignedDoc* pSigDoc, OCSP_REQUEST **req, 
		      X509 *cert, X509 *pCA, byte* nonce, int nlen);
EXP_OPTION int signOCSPRequestPKCS12(OCSP_REQUEST *req, const char* filename, const char* passwd);

int verifyOCSPResponse(OCSP_RESPONSE* pResp, 
				    const X509** caCerts, const char *CApath, 
				    const X509* notCert);
int checkNonceAndCertbyOCSP(OCSP_RESPONSE* resp, X509* cert, byte* nonce1, int nonceLen);

int verifyResp(OCSP_BASICRESP* bs, const char *CA2file, const char *CA1file,
			   const char *CApath, const char* notCertFile);

extern int writeCertToXMLFile(BIO* bout, X509* cert);

//==========< utility functions >====================


#ifdef WIN32
  CRITICAL_SECTION cs_ddocLocaltime;
#endif

//--------------------------------------------------
// Converts timestamp (time_t) value to a struct
// tm value. Caller must provide address of tm struct.
// This function is used because loacltime() is not
// thread-safe and win32 has no equvalent of localtime_r().
// pTime - time_t value address
// pTmStruct - struct tm address
// bLocal - 1=localtime_r, 0=gmtime_r
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocLocalTime(time_t* pTime, struct tm* pTmStruct, int bLocal)
{
#ifdef WIN32
  struct tm *pTm;
#endif
  RETURN_IF_NULL_PARAM(pTime);
  RETURN_IF_NULL_PARAM(pTmStruct);

#ifdef WIN32
  EnterCriticalSection(&cs_ddocLocaltime);
  if(bLocal)
    pTm = localtime(pTime);
  else
    pTm = gmtime(pTime);
  *pTmStruct = *pTm;
  LeaveCriticalSection(&cs_ddocLocaltime);
#else
  if(bLocal)
    localtime_r(pTime, pTmStruct);
  else
    gmtime_r(pTime, pTmStruct);
#endif
  return ERR_OK;
}


//==========< admin functions >====================

//--------------------------------------------------
// returns the library name
//--------------------------------------------------
EXP_OPTION const char* getLibName()
{
	return g_szLibName;
}

//--------------------------------------------------
// returns the library version
//--------------------------------------------------
EXP_OPTION const char* getLibVersion()
{
	return g_szLibVer;
}

//--------------------------------------------------
// returns an array of supported formats terminated by NULL
//--------------------------------------------------
EXP_OPTION const char** getSupportedFormats()
{
	return (const char**)g_arrFormats;
}

//--------------------------------------------------
// returns an array of supported formats  and versions terminated by NULL
//--------------------------------------------------
EXP_OPTION FormatAndVer* getSupportedFormatsAndVersions()
{
	return g_supportedFormatsAndVersions;
}

//--------------------------------------------------
// Cheks a combination of format and version for validity
// format - digidoc format name
// version - version name
// returns error code or ERR_OK
//--------------------------------------------------
int ddocCheckFormatAndVer(const char* format, const char* version)
{
  int i;

  for(i = 0; i < SUPPORTED_VERSION_COUNT; i++)
    if(!strcmp(g_supportedFormatsAndVersions[i].szFormat, format) &&
       !strcmp(g_supportedFormatsAndVersions[i].szVersion, version))
      return ERR_OK;
  return ERR_UNSUPPORTED_FORMAT;
}

//--------------------------------------------------
// initializes DigiDoc library
//--------------------------------------------------
EXP_OPTION void initDigiDocLib() 
{
  memset(g_szGUIVersion, 0, sizeof(g_szGUIVersion));
#ifdef __FreeBSD__
  struct tm local_tm;
  time_t t;
  time(&t);
  ddocLocalTime(&t, &local_tm, 1);	// Extract timezone info from struct tm
  _timezone = - local_tm.tm_gmtoff;
  _daylight = local_tm.tm_isdst;
#endif
  ERR_load_ERR_strings();
  ERR_load_crypto_strings();
  ERR_clear_error();
  OpenSSL_add_all_algorithms();
#ifdef WIN32 
  InitializeCriticalSection(&cs_ddocErrors);
  InitializeCriticalSection(&cs_ddocLocaltime);
#endif
}

//--------------------------------------------------
// returns the GUI version
//--------------------------------------------------
EXP_OPTION const char* getGUIVersion()
{
	return g_szGUIVersion;
}

//--------------------------------------------------
// sets the GUI version
//--------------------------------------------------
EXP_OPTION void setGUIVersion(const char* szVer)
{
  strncpy(g_szGUIVersion, szVer, sizeof(g_szGUIVersion)-1);
}

//--------------------------------------------------
// cleanup of DigiDoc library
//--------------------------------------------------
EXP_OPTION void finalizeDigiDocLib()
{
	ERR_free_strings();
	EVP_cleanup();
}

//==========< general crypto fucntions >============


//--------------------------------------------------
// Checks and prints errors
//--------------------------------------------------
EXP_OPTION long checkErrors() 
{
  char buf[200];
  long e=0;

  while((ERR_peek_error()) > ERR_LIB_NONE) {
    e = ERR_get_error();
    ERR_error_string_n(e, buf, sizeof(buf));
    // VS: keep silent !!!
    printf("ERROR: %ld - %s \n", e, buf);
  }
  ERR_clear_error();
  return e;
}




//--------------------------------------------------
// Calculates files SHA1 digest
// szFileName - file name
// nDigestType - digest type. Supports only SHA1 (0)
// pDigestBuf - buffer to store the digest
// nDigestLen - buffer size, must be at least 20
//				will be updated by actual digest length
// lFileLen - pointer to a buffer where to store the file length
// returns error code or ERR_OK for success
//--------------------------------------------------
EXP_OPTION int calculateFileDigest(const char* szFileName, int nDigestType,
				   byte* pDigestBuf, int* nDigestLen, long* lFileLen)
{
  int err = ERR_OK;
  SHA_CTX ctx;
  byte buf[FILE_BUFSIZE];
  int i;
  FILE *f = NULL;

  RETURN_IF_NULL_PARAM(szFileName);
  RETURN_IF_NULL_PARAM(pDigestBuf);
  RETURN_IF_NULL_PARAM(nDigestLen);
  RETURN_IF_NULL_PARAM(lFileLen);

  memset(pDigestBuf, 0, *nDigestLen);
  if(nDigestType == DIGEST_SHA1) {
    if(*nDigestLen >= SHA_DIGEST_LENGTH) {
      *nDigestLen = SHA_DIGEST_LENGTH;
      if((f = fopen(szFileName,"rb")) != NULL) {
	//byte *data,*temp_data;
	SHA1_Init(&ctx);
	*lFileLen = 0;
	for (;;) {
	  i = fread(buf, sizeof(char), FILE_BUFSIZE, f);
	  if (i <= 0) break;
	  *lFileLen += i;
	  SHA1_Update(&ctx, buf, (unsigned long)i);
	}
	SHA1_Final(pDigestBuf,&ctx);
	fclose(f);
      } // if - fopen
      else
	err = ERR_FILE_READ;
    } 
    else
      err = ERR_DIGEST_LEN;
  }
  else
    err = ERR_UNSUPPORTED_DIGEST;
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}


//--------------------------------------------------
// Calculates file size
// szFileName - file name
// lFileLen - pointer to a buffer where to store the file length
// returns error code or ERR_OK for success
//--------------------------------------------------
EXP_OPTION int calculateFileSize(const char* szFileName, long* lFileLen)
{
  FILE* hFile = 0;
  
  RETURN_IF_NULL_PARAM(szFileName);
  RETURN_IF_NULL_PARAM(lFileLen);
  if((hFile = fopen(szFileName,"rb")) != NULL) {
    fseek(hFile, 0, SEEK_END);
    *lFileLen = ftell(hFile);
    fclose(hFile);
  } // if - fopen
  else
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_READ);
  return ERR_OK;
}


//--------------------------------------------------
// Calculates files SHA1-RSA signature
// szFileName - file name
// nDigestType - digest type. Supports only SHA1 (0)
// pSigBuf - buffer to store the signature
// nSigLen - buffer size, must be at least 128
//			will be updated by actual signature length
// keyfile - name of the private key file
// passwd - private key password
// returns error code or ERR_OK for success
//--------------------------------------------------
EXP_OPTION int calculateFileSignature(const char* szFileName, int nDigestType,
							byte* pSigBuf, int* nSigLen,
							const char *keyfile, const char* passwd)
{
  int err = ERR_OK;
  EVP_MD_CTX  ctx;
  byte buf[FILE_BUFSIZE];
  int i;
  FILE *f = NULL;
  EVP_PKEY* pkey = NULL;

  RETURN_IF_NULL_PARAM(szFileName);
  RETURN_IF_NULL_PARAM(pSigBuf);
  RETURN_IF_NULL_PARAM(nSigLen);
  RETURN_IF_NULL_PARAM(keyfile);
  RETURN_IF_NULL_PARAM(passwd);

  memset(pSigBuf, 0, *nSigLen);
  if(nDigestType == DIGEST_SHA1) {
    if(*nSigLen >= SIGNATURE_LEN) {
      if((err = ReadPrivateKey(&pkey, keyfile, passwd, FILE_FORMAT_PEM)) == ERR_OK) {
	if((f = fopen(szFileName,"rb")) != NULL) {					
	  EVP_SignInit(&ctx, EVP_sha1());
	  for (;;) {
	    i = fread(buf, sizeof(char), FILE_BUFSIZE, f);
	    if (i <= 0) break;
	    EVP_SignUpdate (&ctx, buf, (unsigned long)i);
	  }
	  err = EVP_SignFinal(&ctx, pSigBuf, (unsigned int*)nSigLen, pkey);
	  if(err == ERR_LIB_NONE)
	    err = ERR_OK;
	  fclose(f);
	  EVP_PKEY_free(pkey);					
	} // if - fopen
	else
	  err = ERR_FILE_READ;
      }
      else
	err = ERR_PRIVKEY_READ;
    } 
    else
      err = ERR_SIGNATURE_LEN;
  }
  else
    err = ERR_UNSUPPORTED_DIGEST;
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}


//--------------------------------------------------
// Calculates input datas SHA1-RSA signature
// data - input data
// dlen - input data length
// nDigestType - digest type. Supports only SHA1 (0)
// pSigBuf - buffer to store the signature
// nSigLen - buffer size, must be at least 128
//			will be updated by actual signature length
// keyfile - name of the private key file
// passwd - private key password
// returns error code or ERR_OK for success
//--------------------------------------------------
EXP_OPTION int signData(const byte* data, int dlen, byte* pSigBuf, int* nSigLen,
			 int nDigestType, const char *keyfile, const char* passwd)
{
  int err = ERR_OK;
  EVP_MD_CTX  ctx;
  EVP_PKEY* pkey;

  RETURN_IF_NULL_PARAM(data);
  RETURN_IF_NULL_PARAM(pSigBuf);
  RETURN_IF_NULL_PARAM(nSigLen);
  RETURN_IF_NULL_PARAM(keyfile);
  RETURN_IF_NULL_PARAM(passwd);
  
  memset(pSigBuf, 0, *nSigLen);
  if(nDigestType == DIGEST_SHA1) {
    if(*nSigLen >= SIGNATURE_LEN) {
      if((err = ReadPrivateKey(&pkey, keyfile, passwd, FILE_FORMAT_PEM)) == ERR_OK) {
	EVP_SignInit(&ctx, EVP_sha1());
	EVP_SignUpdate (&ctx, data, (unsigned long)dlen);
	err = EVP_SignFinal(&ctx, pSigBuf, (unsigned int*)nSigLen, pkey);
	if(err == ERR_LIB_NONE)
	  err = ERR_OK;
	EVP_PKEY_free(pkey);				
      }
      else
	err = ERR_PRIVKEY_READ;
    } 
    else
      err = ERR_SIGNATURE_LEN;
  }
  else
    err = ERR_UNSUPPORTED_DIGEST;
  
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}

//--------------------------------------------------
// Calculates files SHA1 digest
// szFileName - file name
// nDigestType - digest type. Supports only SHA1 (0)
// pDigestBuf - buffer to store the digest
// nDigestLen - buffer size, must be at least 20
//				will be updated by actual digest length
// returns error code or ERR_OK for success
//--------------------------------------------------
EXP_OPTION int calculateDigest(const byte* data, int nDataLen, int nDigestType,
					byte* pDigestBuf, int* nDigestLen)
{
  int err = ERR_OK;

  RETURN_IF_NULL_PARAM(data);
  RETURN_IF_NULL_PARAM(pDigestBuf);
  RETURN_IF_NULL_PARAM(nDigestLen);
  memset(pDigestBuf, 0, *nDigestLen);
  if(nDigestType == DIGEST_SHA1) {
    if(*nDigestLen >= SHA_DIGEST_LENGTH) {
      *nDigestLen = SHA_DIGEST_LENGTH;
      SHA1(data, nDataLen, pDigestBuf);
    } 
    else
      err = ERR_DIGEST_LEN;
  }
  else
    err = ERR_UNSUPPORTED_DIGEST;
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}




//==========< signed doc fucntions >================



//============================================================
// Get the filename part of full file name
//============================================================
EXP_OPTION const char* getSimpleFileName(const char* szFileName)
{
	const char *p;

	RETURN_OBJ_IF_NULL(szFileName, NULL);
	
	for(p = szFileName + strlen(szFileName) -1; 
		(p > szFileName) && (*p != '\\') && (*p != '/');
		p--);
	if((*p == '\\') || (*p == '/'))
		p++;
	return p;
}

//============================================================
// Get the absolute filename with path
//============================================================
EXP_OPTION int getFullFileName(const char* szFileName, char* szDest, int len)
{
  int err = ERR_OK;
  memset(szDest, 0, len);
  // if this is a filename with path then we are ready
  if(strchr(szFileName, '/') || strchr(szFileName, '\\')) {
    strncpy(szDest, szFileName, len);
  } else { // local filename, must prepend directory
    _getcwd(szDest, len);
#ifdef WIN32
  if(strlen(szDest) < len - 2)
    strncat(szDest, "\\", sizeof(szDest) - strlen(szDest));
#else
  if(strlen(szDest) < len - 2)
    strncat(szDest, "/", sizeof(szDest) - strlen(szDest));
#endif
  if(strlen(szDest) + strlen(szFileName) < len - 2)
    strncat(szDest, szFileName, sizeof(szDest) - strlen(szDest));
  }
  return err;
}


//============================================================
// Get the path part of full file name
//============================================================
EXP_OPTION int getFileNamePath(const char* szFileName, char* szPath, int len)
{
  int nFound = 0, i, err = ERR_OK;

  RETURN_IF_NULL_PARAM(szFileName);
  RETURN_IF_NULL_PARAM(szPath);
  strncpy(szPath, szFileName, len);
  for(i = strlen(szPath); i >= 0; i--) {
    if(szPath[i] == '\\') {
      szPath[i+1] = 0;
      nFound = 1;
      break;
    }
  }
  if(!nFound)
    szPath[0] = 0;
  return err;
}


//============================================================
// Gets a new temporary filename
// buf - filename buffer
//============================================================
EXP_OPTION int getTempFileName(char* szFileName, int len)
{
	char tbuf[200];
        int f = 0;
#ifdef WIN32
	char* pFileName = 0;
#endif
	RETURN_IF_NULL_PARAM(szFileName);
	memset(szFileName, 0, len);
#ifdef WIN32
	GetTempPath(sizeof(tbuf), tbuf);
	pFileName = _tempnam(tbuf, "ddoc");
	strncpy(szFileName, pFileName, len);
	free(pFileName);
#else
	strncpy(tbuf, "/tmp/ddocXXXXXX", sizeof(tbuf));
	f = mkstemp(tbuf);
	if (f > 0) // Maybe we should use the file instead of closing it and reopening later? 
          close(f);
	strncpy(szFileName, tbuf, len);
#endif
	return ERR_OK;
}


//============================================================
// Sets the signatures certificate and calculates
// certificate digest & serial number
// pSigInfo - signature info object
// cert - certficate 
//============================================================
EXP_OPTION int setSignatureCert(SignatureInfo* pSigInfo, X509* cert)
{
  int err = ERR_OK;

  RETURN_IF_NULL_PARAM(pSigInfo);
  RETURN_IF_NULL_PARAM(cert);
  err = ddocSigInfo_addCert(pSigInfo, cert, CERTID_TYPE_SIGNERS_CERTID);
  // release old content if it exists
  ddocMemBuf_free(&(pSigInfo->mbufOrigContent));
  return ERR_OK;
}

//============================================================
// Sets the signatures certificate and calculates
// certificate digest & serial number
// pSigInfo - signature info object
// certFile - certficate file in PEM
//============================================================
EXP_OPTION int setSignatureCertFile(SignatureInfo* pSigInfo, const char* certFile)
{
  X509 *cert = NULL;
  int err = ERR_OK;

  RETURN_IF_NULL_PARAM(pSigInfo);
  RETURN_IF_NULL_PARAM(certFile);
  err = ReadCertificate(&cert, certFile);
  RETURN_IF_NOT(err == ERR_OK, err);
  return setSignatureCert(pSigInfo, cert);;
}


//--------------------------------------------------
// Checks if this file exists
// szFileName - file name
// returns 1 if exists
// AA 2004/05/20 debuggeri all viga, kui null pikkus
//--------------------------------------------------
EXP_OPTION int checkFileExists(const char* szFileName)
{
  FILE* hFile;
  int exists = 0, err;
  char buf1[300];

  err = ddocConvertFileName(buf1, sizeof(buf1), szFileName);
  if(err) return 0; // return doesn't exist if error
  if (strlen(buf1) > 0) {
    if((hFile = fopen(buf1, "r")) != NULL) {
      exists = 1;
      fclose(hFile);
    }
  }
  return exists;
}


//================< certficate info functions> =================================

//--------------------------------------------------
// Returns the user signatures certificate data
// pSignInfo - signature object
// returns cert data. This is actually X509*. Obsolete function
//--------------------------------------------------
EXP_OPTION X509* getSignCertData(const SignatureInfo* pSignInfo)
{
  RETURN_OBJ_IF_NULL(pSignInfo, 0);
  return ddocSigInfo_GetSignersCert(pSignInfo);
}


//--------------------------------------------------
// sends an OCSP_REQUES object to remore server and
// retrieves the OCSP_RESPONSE object
// resp - buffer to store the new responses pointer
// req - request objects pointer
// url - OCSP responder URL
//--------------------------------------------------
int ddocPullUrl(const char* url, DigiDocMemBuf* pSendData, DigiDocMemBuf* pRecvData, 
		const char* proxyHost, const char* proxyPort)
{	
  BIO* cbio = 0, *sbio = 0;
  SSL_CTX *ctx = NULL;
  char *host = NULL, *port = NULL, *path = "/", buf[200];
  int err = ERR_OK, use_ssl = -1, rc;
  long e;

  //RETURN_IF_NULL_PARAM(pSendData); // may be null if nothing to send?
  RETURN_IF_NULL_PARAM(pRecvData);
  RETURN_IF_NULL_PARAM(url);

  ddocDebug(3, "ddocPullUrl", "URL: %s, in: %d bytes", url, pSendData->nLen);
  //there is an HTTP proxy - connect to that instead of the target host
  if (proxyHost != 0 && *proxyHost != '\0') {
    host = (char*)proxyHost;
    if(proxyPort != 0 && *proxyPort != '\0')
      port = (char*)proxyPort;
    path = (char*)url;
  } else {
    if(OCSP_parse_url((char*)url, &host, &port, &path, &use_ssl) == 0) {
      ddocDebug(1, "ddocPullUrl", "Failed to parse the URL");
      return ERR_WRONG_URL_OR_PROXY; 
    }
  }
	
  if((cbio = BIO_new_connect(host)) != 0) {
    if(port != NULL) 
      BIO_set_conn_port(cbio, port);
    if(use_ssl == 1) {
      ctx = SSL_CTX_new(SSLv23_client_method());
      SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
      sbio = BIO_new_ssl(ctx, 1);
      cbio = BIO_push(sbio, cbio);
    }
    if ((rc = BIO_do_connect(cbio)) > 0) {
      ddocDebug(4, "ddocPullUrl", "Connected: %d", rc);
      if(pSendData && pSendData->nLen && pSendData->pMem) {
	rc = BIO_write(cbio, pSendData->pMem, pSendData->nLen);
	ddocDebug(4, "ddocPullUrl", "Sent: %d bytes, got: %d", pSendData->nLen, rc);
      }
      do {
	memset(buf, 0, sizeof(buf));
	rc = BIO_read(cbio, buf, sizeof(buf)-1);
	ddocDebug(4, "ddocPullUrl", "Received: %d bytes\n", rc);
	if(rc > 0)
	  err = ddocMemAppendData(pRecvData, buf, rc);
      } while(rc > 0);
      ddocDebug(4, "ddocPullUrl", "Total received: %d bytes\n", pRecvData->nLen);
    } else {
      //if no connection
	  e = checkErrors();
	  if(ERR_GET_REASON(e) == BIO_R_BAD_HOSTNAME_LOOKUP ||
		 ERR_GET_REASON(e) == OCSP_R_SERVER_WRITE_ERROR)
		  err = ERR_CONNECTION_FAILURE;
	  else
		  err = (host != NULL) ? ERR_WRONG_URL_OR_PROXY : ERR_CONNECTION_FAILURE;
    }
    BIO_free_all(cbio);
    if (use_ssl != -1) {
      OPENSSL_free(host);
      OPENSSL_free(port);
      OPENSSL_free(path);
      SSL_CTX_free(ctx);
    }
  }
  else
    err = ERR_CONNECTION_FAILURE;
  return(err);
}




// ASN1 structure prefix - RSA-SHA1 signature with 20 bytes follows
char g_sigPrefix[] = { 48, 33, 48, 9, 6, 5, 43, 14, 3, 2, 26, 5, 0, 4, 20 };

//--------------------------------------------------
// Prepares a new signature for signing and calculates
// the final hash value to sign.
// pSigDoc - signed document object
// ppSigInfo - pointer for address of newly allocated signature
// manifest - manifest or role
// city - signers address , city
// state - signers address , state or province
// zip - signers address , postal code
// country - signers address , country name
// id - id for new signature. Optional, use NULL for default
// return returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocPrepareSignature(SignedDoc* pSigDoc, SignatureInfo** ppSigInfo,
			 const char* manifest, const char* city, 
			 const char* state, const char* zip, 
			 const char* country, X509* pCert, const char* id)
{
  int err = ERR_OK, l1;
  DigiDocMemBuf mbuf1, *pMBuf1;
  char buf1[50];

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  ddocDebug(3, "ddocPrepareSignature", "Preparing signature manifest: %s country: %s, state: %s, city: %s, zip: %s, cert: %s, id: %s", 
	    (manifest ? manifest : "NULL"), (country ? country : "NULL"),
	    (state ? state : "NULL"), (city ? city : "NULL"), (zip ? zip : "NULL"),
	    (pCert ? "OK" : "ERROR"), (id ? id : "NULL"));
  // check mandator fields
  RETURN_IF_NULL_PARAM(pSigDoc);
  RETURN_IF_NULL_PARAM(ppSigInfo);
  RETURN_IF_NULL_PARAM(pCert);
  clearErrors();
  // add new signature
  err = SignatureInfo_new(ppSigInfo, pSigDoc, id);
  RETURN_IF_NOT(err == ERR_OK, err);
  // automatically calculate doc-info elements for this signature
  addAllDocInfos(pSigDoc, *ppSigInfo);
  // add signature production place
  if (city || state || zip || country)
    err = setSignatureProductionPlace(*ppSigInfo, city, state, zip, country);
  // add user roles/manifests
  if (manifest)
    err = addSignerRole(*ppSigInfo, 0, manifest, -1, 0);
  RETURN_IF_NOT(err == ERR_OK, err);
  // add signers certificate
  err = setSignatureCert(*ppSigInfo, pCert);
  RETURN_IF_NOT(err == ERR_OK, err); 
  // timestamp
  createTimestamp(pSigDoc, (char*)buf1, sizeof(buf1));
  setString((char**)&((*ppSigInfo)->szTimeStamp), (const char*)buf1, -1);
  // now calculate signed properties digest
  err = calculateSignedPropertiesDigest(pSigDoc, *ppSigInfo);
  // TODO: replace later
  pMBuf1 = ddocDigestValue_GetDigestValue((*ppSigInfo)->pSigPropDigest);
  ddocSigInfo_SetSigPropRealDigest(*ppSigInfo, 
				   (const char*)pMBuf1->pMem, pMBuf1->nLen);
  // signature type & val
  ddocSignatureValue_new(&((*ppSigInfo)->pSigValue), 0, SIGN_RSA_NAME, 0, 0);
  // calc signed-info digest
  l1 = sizeof(buf1);
  err = calculateSignedInfoDigest(pSigDoc, *ppSigInfo, (byte*)buf1, &l1);
  err = ddocSigInfo_SetSigInfoRealDigest(*ppSigInfo, buf1, l1);
  // debug output - final hash to sign
  pMBuf1 = ddocDigestValue_GetDigestValue((*ppSigInfo)->pSigInfoRealDigest);
  ddocEncodeBase64(pMBuf1, &mbuf1);
  ddocDebug(3, "ddocPrepareSignature", "signing hash %s len: %d b64len: %d", 
	    (char*)mbuf1.pMem, mbuf1.nLen, l1);
  ddocMemBuf_free(&mbuf1);
  return err;
}

//--------------------------------------------------
// Returns the hash to be signed with or without ASN1
// prefix and with or without base64 encoding
// pSigInfo - signature address
// pBuf - buffer for hash value with or without prefix
// pBufLen - pointer to buffer length
// enc - return 0=unencoded, 1=base64, 2=hex
// bWithAsn1Prefix - return with or without ASN1 prefix 1/0
// return returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION  int ddocGetSignedHash(SignatureInfo* pSigInfo, 
		   char* pBuf, int* pBufLen, int enc, int bWithAsn1Prefix)
{
  int err = ERR_OK, l1 = 0;
  char buf1[50];
  DigiDocMemBuf *pMBuf;

  RETURN_IF_NULL_PARAM(pSigInfo);
  RETURN_IF_NULL_PARAM(pBuf);
  //RETURN_IF_NOT(err == ERR_OK, err);
  // TODO: check buflen
  pMBuf = ddocDigestValue_GetDigestValue(pSigInfo->pSigInfoRealDigest);
  RETURN_IF_NULL_PARAM(pMBuf);
  if(enc == 2) { // hex mode
    if(bWithAsn1Prefix) {
      if(sizeof(buf1) > sizeof(g_sigPrefix) + pMBuf->nLen) {
        memcpy(buf1, g_sigPrefix, sizeof(g_sigPrefix));
        memcpy(buf1 + sizeof(g_sigPrefix), pMBuf->pMem, pMBuf->nLen);
        l1 = pMBuf->nLen + sizeof(g_sigPrefix);
      }
      else
        err = ERR_BUF_LEN;
    }
    else {
      if(sizeof(buf1) > pMBuf->nLen) {
        memcpy(buf1, pMBuf->pMem, pMBuf->nLen);
        l1 = pMBuf->nLen;
      }
      else
        err = ERR_BUF_LEN;
    }
    bin2hex((const byte*)buf1, l1, (char*)pBuf, pBufLen);
  } else if(enc == 1) { // base64 mode
    if(bWithAsn1Prefix) {
      if(sizeof(buf1) > sizeof(g_sigPrefix) + pMBuf->nLen) {
        memcpy(buf1, g_sigPrefix, sizeof(g_sigPrefix));
        memcpy(buf1 + sizeof(g_sigPrefix), pMBuf->pMem, pMBuf->nLen);
        l1 = pMBuf->nLen + sizeof(g_sigPrefix);
      }
      else
        err = ERR_BUF_LEN;
    }
    else {
      if(sizeof(buf1) > pMBuf->nLen) {
        memcpy(buf1, pMBuf->pMem, pMBuf->nLen);
        l1 = pMBuf->nLen;
      }
      else
        err = ERR_BUF_LEN;
    }
    encode((const byte*)buf1, l1, (byte*)pBuf, pBufLen);
  } else {
    if(bWithAsn1Prefix) {
      if(*pBufLen > sizeof(g_sigPrefix) + pMBuf->nLen) {
        memcpy(pBuf, g_sigPrefix, sizeof(g_sigPrefix));
        memcpy(pBuf + sizeof(g_sigPrefix), pMBuf->pMem, pMBuf->nLen);
        *pBufLen = pMBuf->nLen  + sizeof(g_sigPrefix);
      }
      else
        err = ERR_BUF_LEN;
    }
    else {
      if(*pBufLen > pMBuf->nLen) {
        *pBufLen = pMBuf->nLen;
        memcpy(pBuf, pMBuf->pMem, pMBuf->nLen);
      }
      else
        err = ERR_BUF_LEN;
    }
  }
  return err;
}


//================< deprecated functions> =================================

#ifdef WITH_DEPRECATED_FUNCTIONS

// get signers id-code
EXP_OPTION int getSignerCode(const SignatureInfo* pSigInfo, char* buf)
{
  int err, l1;
  X509* cert;
  char buf1[500], *p;
  
  RETURN_IF_NULL_PARAM(pSigInfo);
  RETURN_IF_NULL_PARAM(buf);
  cert = getSignCertData(pSigInfo);
  if (!cert) SET_LAST_ERROR_RETURN_CODE(ERR_CERT_INVALID);
  l1 = sizeof(buf1);
  err = getCertSubjectName(cert, buf1, &l1);
  if (err != ERR_OK) SET_LAST_ERROR_RETURN_CODE(err);
  err = ERR_CERT_READ; 
  p = strstr(buf1, "CN=");
  if (p) {
    p = strchr(p, ',');
    if(p) {
      p = strchr(p+1, ',');
      if(p) {
	strncpy(buf, p+1, 11);
	buf[11] = 0;
	err = ERR_OK;
      } 
    } else { // no comma -> no id-code !
      buf[0] = 0; 
      // is this really an error ?
      err = ERR_WRONG_CERT;
    }  
  }
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}

// get signers first name
EXP_OPTION int getSignerFirstName(const SignatureInfo* pSigInfo, char* buf)
{
  int err = ERR_OK, l1;
  X509* cert;
  char buf1[500], *p, *p2, *p1;

  RETURN_IF_NULL_PARAM(pSigInfo);
  RETURN_IF_NULL_PARAM(buf);
  cert = getSignCertData(pSigInfo);
  if (!cert) SET_LAST_ERROR_RETURN_CODE(ERR_CERT_INVALID);
  l1 = sizeof(buf1);
  err = getCertSubjectName(cert, buf1, &l1);
  if (err != ERR_OK) SET_LAST_ERROR_RETURN_CODE(err);
  p = strstr(buf1, "CN=");
  if (p) {
    p1 = strchr(p, ',');
    if(!p1)
      p1 = strchr(p, '/');
    if (p1) {
      p1 += 1;
      p2 = strchr(p1, ',');
      if(!p2)
	p2 = strchr(p1, '/');
      if(p2) {
	strncpy(buf, p1, p2-p1);
	buf[p2-p1] = 0;
	err = ERR_OK;
      }	
    }
  }
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}

// get signers last name
EXP_OPTION int getSignerLastName(const SignatureInfo* pSigInfo, char* buf)
{
  int err = ERR_OK, l1;
  X509* cert;
  char buf1[500], *p, *p2;
  
  RETURN_IF_NULL_PARAM(pSigInfo);
  RETURN_IF_NULL_PARAM(buf);
  cert = getSignCertData(pSigInfo);
  if (!cert) SET_LAST_ERROR_RETURN_CODE(ERR_CERT_INVALID);
  l1 = sizeof(buf1);
  err = getCertSubjectName(cert, buf1, &l1);
  if (err != ERR_OK) SET_LAST_ERROR_RETURN_CODE(err);
  p = strstr(buf1, "CN=");
  if(p) {
    p += 3;
    p2 = strchr(p, ',');
    if(!p2)
      p2 = strchr(p, '/');
    if(p2) {
      strncpy(buf, p, p2-p);
      buf[p2-p] = 0;
      err = ERR_OK;
    } else {
      strncpy(buf, p, strlen(p));
    }
  }
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}

EXP_OPTION int getSignerCN(const SignatureInfo* pSigInfo, char* buf, int bUTF8)
{
  int err = ERR_OK, l1;
  X509* cert;
  char buf1[500], *p, *p2, buf2[500];
  
  RETURN_IF_NULL_PARAM(pSigInfo);
  RETURN_IF_NULL_PARAM(buf);
  cert = getSignCertData(pSigInfo);
  if (!cert) SET_LAST_ERROR_RETURN_CODE(ERR_CERT_INVALID);
  l1 = sizeof(buf1);
  err = getCertSubjectName(cert, buf1, &l1);
  if (err != ERR_OK) SET_LAST_ERROR_RETURN_CODE(err);
  if(!bUTF8) {
    l1 = sizeof(buf2);
    utf82ascii(buf1, buf2, &l1);
    strncpy(buf1, buf2, sizeof(buf1));
  }
  err = ERR_CERT_READ;
  p = strstr(buf1, "CN=");
  if (p) {
    p += 3;
    p2 = strchr(p, '/');
    if(!p2)
      p2 = p + strlen(p);
    if (p2) {
      strncpy(buf, p, p2-p);
      buf[p2-p] = 0;
      err = ERR_OK;	
    } 
  }
  
  if (err != ERR_OK) SET_LAST_ERROR(err);	
  return err;
}

#endif  // WITH_DEPRECATED_FUNCTIONS


