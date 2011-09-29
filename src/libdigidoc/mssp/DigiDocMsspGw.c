//==================================================
// FILE:	DigiDocMsspGw.c
// PROJECT:     Digi Doc
// DESCRIPTION: Digi Doc functions for MSSP_GW client
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
//      25.09.2005      Veiko Sinivee
//                      Creation
//==================================================

#include "DigiDocMsspGw.h"
#include "../DigiDocDefs.h"
#include "../DigiDocError.h"
#include "../DigiDocConfig.h"
#include "../DigiDocDebug.h"
#include "../DigiDocMem.h"
#include "../DigiDocCert.h"
#include "../DigiDocConvert.h"
#include "../DigiDocGen.h"
#include "../mssp/stdsoap2.h"
#include "MSSP_GW.nsmap"
#include "soapH.h"
#include "soapStub.h"


//--------------------------------------------------
// Initializes MSSP connection
// pMssp - pointer to MSSP context structure
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int ddocMsspConnect(MSSP** ppMssp)
{
  int err = ERR_OK;

  RETURN_IF_NULL_PARAM(ppMssp);
  // alloc new object only if there is none
  if(!*ppMssp) {
    // allocate new MSSP context object
    *ppMssp = (MSSP*)malloc(sizeof(MSSP));
    RETURN_IF_BAD_ALLOC(*ppMssp);
    memset(*ppMssp, 0, sizeof(MSSP));
  }
  ddocDebug(3, "ddocMsspConnect", "Connecting to: %s", (char*)ConfigItem_lookup("DIGIDOC_MSSP_ENDPOINT"));
  err = ddocMsspReconnect(*ppMssp);
  return err;
}

//--------------------------------------------------
// Reconnects to MSSP service if necessary
// pMssp - pointer to MSSP context structure
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int ddocMsspReconnect(MSSP* pMssp)
{
  int err = SOAP_OK;
  char buf1[250];

  RETURN_IF_NULL_PARAM(pMssp);
  if(!pMssp->endpoint) {
    ddocDebug(3, "ddocMsspReconnect", "Reconnecting to: %s", (char*)ConfigItem_lookup("DIGIDOC_MSSP_ENDPOINT"));
    pMssp->endpoint = (char*)ConfigItem_lookup("DIGIDOC_MSSP_ENDPOINT");
    pMssp->soap.proxy_host = (char*)ConfigItem_lookup("DIGIDOC_PROXY_HOST");
    if(ConfigItem_lookup("DIGIDOC_PROXY_PORT"))
      pMssp->soap.proxy_port = atoi(ConfigItem_lookup("DIGIDOC_PROXY_PORT"));
    soap_init(&(pMssp->soap));
    pMssp->soap.namespaces = namespaces;
    // if using HTTPS connection
#ifdef WIN32
    if(pMssp->endpoint && !_strnicmp(pMssp->endpoint, "https", 5)) {
#else
    if(pMssp->endpoint && !strncasecmp(pMssp->endpoint, "https", 5)) {
#endif
      // compose MSSP GW server cert path
      strcpy(buf1, (char*)ConfigItem_lookup("CA_CERT_PATH"));
#ifdef WIN32
      strcat(buf1, "\\");
#else
      strcat(buf1, "/");
#endif
      if(ConfigItem_lookup("DIGIDOC_MSSP_GW_CERT"))
        strcat(buf1, (char*)ConfigItem_lookup("DIGIDOC_MSSP_GW_CERT")); 
      ddocDebug(3, "ddocMsspReconnect", "Server cert: %s", (ConfigItem_lookup("DIGIDOC_MSSP_GW_CERT") ? buf1 : ""));
      if(soap_ssl_client_context(&(pMssp->soap),
	(ConfigItem_lookup("DIGIDOC_MSSP_GW_CERT") ? SOAP_SSL_DEFAULT : SOAP_SSL_NO_AUTHENTICATION),
				 NULL, // no client auth
				 NULL, // no client key passwd
				 (ConfigItem_lookup("DIGIDOC_MSSP_GW_CERT") ? buf1 : NULL), // server ca
				 NULL, // optional capath
				 NULL // no randfile
				 )) {
	soap_print_fault(&(pMssp->soap), stderr);
	return ERR_GENERIC_SOAP_ERR;
      }
    }
  }
  ddocDebug(3, "ddocMsspReconnect", "RC: %d", err);
  return err;
}



//--------------------------------------------------
// Cleanup MSSP connection
// pMssp - pointer to MSSP context structure
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int ddocMsspDisconnect(MSSP* pMssp)
{
  int err = ERR_OK;
  //RETURN_IF_NULL_PARAM(pMssp);
  ddocDebug(3, "ddocMsspDisconnect", "Disconnecting");
  if(pMssp && pMssp->endpoint) {
    ddocDebug(3, "ddocMsspDisconnect", "Disconnecting from: %s", pMssp->endpoint);
    soap_destroy(&(pMssp->soap));
    soap_end(&(pMssp->soap));
    soap_done(&(pMssp->soap));
    pMssp->endpoint = 0; // mark disconnected
  }
  if(pMssp)
    free(pMssp);
  return err;
}

//--------------------------------------------------
// Cleanup MSSP connection but don't disconnect
// pMssp - pointer to MSSP context structure
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int ddocMsspCleanup(MSSP* pMssp)
{
  int err = ERR_OK;
  //RETURN_IF_NULL_PARAM(pMssp);
  ddocDebug(3, "ddocMsspCleanup", "cleanup");
  if(pMssp && pMssp->endpoint) {
  ddocDebug(3, "ddocMsspCleanup", "cleanup: %s", (pMssp->endpoint ? pMssp->endpoint : ""));
  soap_destroy(&(pMssp->soap));
  soap_end(&(pMssp->soap));
  soap_done(&(pMssp->soap));
  pMssp->endpoint = 0; // mark disconnected
  }
  return err;
}

//--------------------------------------------------
// Rerieves SOAP fault string
// pMssp - pointer to MSSP context structure
// returns SOAP fault string or NULL
//--------------------------------------------------
EXP_OPTION char* ddocGetSoapFaultString(MSSP* pMssp) 
{
  char *s = NULL, **p = NULL;
  if(pMssp && pMssp->soap.error) {
    ddocDebug(4, "ddocGetSoapFaultString", "Get fault for err: %d", pMssp->soap);
    p = (char**)soap_faultstring(&(pMssp->soap));
    ddocDebug(4, "ddocGetSoapFaultString", "p: %s, s: %s", 
	      (p ? "OK" : "NULL"), ((p && *p) ? *p : "NULL"));
    if(p && *p)
      s = (char*)*p;
  }
  return s;
}

//--------------------------------------------------
// Rerieves SOAP fault code
// pMssp - pointer to MSSP context structure
// returns SOAP fault code or NULL
//--------------------------------------------------
EXP_OPTION char* ddocGetSoapFaultCode(MSSP* pMssp) 
{
  char *s = NULL, **d = NULL;
  if(pMssp && pMssp->soap.error) {
    d = (char**)soap_faultcode(&(pMssp->soap));
    if(d && *d)
      s = *d;
  }
  return s;
}

//--------------------------------------------------
// Rerieves SOAP fault detail
// pMssp - pointer to MSSP context structure
// returns SOAP fault detail or NULL
//--------------------------------------------------
EXP_OPTION char* ddocGetSoapFaultDetail(MSSP* pMssp) 
{
  char *s = NULL, **d = NULL;
  if(pMssp && pMssp->soap.error) {
    d = (char**)soap_faultdetail(&(pMssp->soap));
    if(d && *d)
      s = *d;
  }
  return s;
}


//--------------------------------------------------
// Sends an MSSP request to sign this data
// pMssp - pointer to MSSP context structure
// szPhoneNo - phone number on which to sign
// pHash - pointer to binary hash to sign
// nHashLen - length of hash data
// szDesc - description what user is signing (file name)
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocMsspSignatureReq(MSSP* pMssp, const char* szPhoneNo,
			 const char* pHash, int nHashLen, const char* szDesc)
{
  int err = SOAP_OK;
  unsigned long lTimeout;
  int nMsgMode, nB64HashLen, nB64ManifestLen;
  struct mss__AP_InfoType apInfo;
  struct mss__MSSP_InfoType msspInfo;
  struct mss__MobileUserType mu;
  struct mss__DataType DataToBeSigned, DataToBeDisplayed;
  struct mss__MSS_USCORESignatureResp resp;
  char szTimestamp[30], szTxnId[20], *pBase64Hash = 0, *pBase64Manifest = 0;

  RETURN_IF_NULL_PARAM(pMssp);
  RETURN_IF_NULL_PARAM(szPhoneNo);
  RETURN_IF_NULL_PARAM(pHash);
  ddocDebug(3, "ddocMsspSignatureReq", "Signing: %s on phone: %s", 
	    szDesc, szPhoneNo);
  // init parameters
  time(&pMssp->lApTxnId);
  sprintf(szTxnId, "%ld", pMssp->lApTxnId);
  createTimestamp(0, szTimestamp, sizeof(szTimestamp));
  apInfo.AP_USCOREID = (char*)ConfigItem_lookup("DIGIDOC_MSSP_AP_USERID");
  apInfo.AP_USCORETransID = szTxnId;
  apInfo.AP_USCOREPWD = (char*)ConfigItem_lookup("DIGIDOC_MSSP_AP_PASSWD");
  apInfo.Instant = szTimestamp;
  apInfo.AP_USCOREURL = (char*)ConfigItem_lookup("DIGIDOC_MSSP_URL");
  msspInfo.MSSP_USCOREID.mss__DNSName = 0;
  msspInfo.MSSP_USCOREID.mss__IPAddress = 0;
  msspInfo.MSSP_USCOREID.mss__IdentifierString = 0;
  msspInfo.MSSP_USCOREID.mss__URI = (char*)ConfigItem_lookup("DIGIDOC_MSSP_URL");
  msspInfo.Instant = szTimestamp;
  mu.mss__IdentityIssuer.mss__DNSName = 0;
  mu.mss__IdentityIssuer.mss__IPAddress = 0;
  mu.mss__IdentityIssuer.mss__URI = 0;
  mu.mss__IdentityIssuer.mss__IdentifierString = 0;
  mu.mss__UserIdentifier = 0;	
  mu.mss__HomeMSSP.mss__URI = 0;
  mu.mss__HomeMSSP.mss__IPAddress = 0;
  mu.mss__HomeMSSP.mss__DNSName = 0;
  mu.mss__HomeMSSP.mss__IdentifierString = 0;
  mu.mss__MSISDN = (char*)szPhoneNo;
  nB64HashLen = nHashLen * 2 + 10;
  pBase64Hash = (char*)malloc(nB64HashLen);
  if(pBase64Hash)
    encode((const byte*)pHash, nHashLen, (byte*)pBase64Hash, &nB64HashLen); 
  DataToBeSigned.__item = pBase64Hash;
  DataToBeSigned.mss__MimeType = "Base64";
  DataToBeSigned.mss__Encoding = "UTF8";
  nB64ManifestLen = strlen(szDesc) * 2 + 10;
  pBase64Manifest = (char*)malloc(nB64ManifestLen);
  if(pBase64Manifest) {
    memset(pBase64Manifest, 0, nB64ManifestLen);
    encode((const byte*)szDesc, strlen(szDesc), (byte*)pBase64Manifest, &nB64ManifestLen); 
  }
  DataToBeDisplayed.__item = pBase64Manifest;
  DataToBeDisplayed.mss__MimeType = "Base64";
  DataToBeDisplayed.mss__Encoding = "UTF8";
  lTimeout = ConfigItem_lookup_int("DIGIDOC_MSSP_TIMEOUT", 0);
  nMsgMode = ConfigItem_lookup_int("DIGIDOC_MSSP_MSG_MODE", 0);

  // reconnect if necessary
  err = ddocMsspReconnect(pMssp);
  // send MSSP request
  ddocDebug(3, "ddocMsspSignatureReq", "Sending request!");
  memset(&resp, 0, sizeof(resp));
  err = soap_call_mss__MSS_USCORESignatureReq(&(pMssp->soap), 
	    (const char*)pMssp->endpoint, "",
	    ConfigItem_lookup_int("DIGIDOC_MSSP_MAJOR", 0),
	    ConfigItem_lookup_int("DIGIDOC_MSSP_MINOR", 0),
	    0, // no validity date, we use timeout instead
	    (lTimeout ? &lTimeout : 0),
	    (enum mss__MessagingModeType)nMsgMode,
	    apInfo,msspInfo, mu,
	    DataToBeSigned, &DataToBeDisplayed,
	    0, 0, 0, 0, &resp);
   ddocDebug(3, "ddocMsspSignatureReq", "Sent RC: %d err: %d", err, pMssp->soap.error);
  // report error or success
  if(err) {
    ddocDebug(3, "ddocMsspSignatureReq", "RC: %d err: %d",
	      err, pMssp->soap.error);
  } else {
    ddocDebug(3, "ddocMsspSignatureReq", "RC: %d err: %d Status: %d - %s message: %s detail: %s trans-id: %ld",
	    err, pMssp->soap.error, resp.Status.StatusCode.Value, 
	    (resp.Status.StatusCode.__item ? resp.Status.StatusCode.__item : ""),
	    (resp.Status.StatusMessage ? resp.Status.StatusMessage : ""),
	    (resp.Status.StatusDetail ? resp.Status.StatusDetail : ""),
	    (unsigned long)resp.MSSP_USCORETransID);
  }
  if(err) {
    addError(ERR_GENERIC_SOAP_ERR, __FILE__, __LINE__, ddocGetSoapFaultCode(pMssp));
    addError(ERR_GENERIC_SOAP_ERR, __FILE__, __LINE__, ddocGetSoapFaultString(pMssp));
    addError(ERR_GENERIC_SOAP_ERR, __FILE__, __LINE__, ddocGetSoapFaultDetail(pMssp));
    soap_print_fault(&(pMssp->soap), stdout);
  }
  pMssp->lMsspTxnId = (unsigned long)resp.MSSP_USCORETransID;
  pMssp->nStatusCode = resp.Status.StatusCode.Value;
  pMssp->endpoint = NULL; // force reconnect
  // cleanup
  free(pBase64Hash);
  free(pBase64Manifest);
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}


//--------------------------------------------------
// Sends an MSSP request to find the status of signature
// operation and possibly it's value
// pMssp - pointer to MSSP context structure
// pMbufSignature - memory buffer to store signature value
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocMsspStatusReq(MSSP* pMssp, DigiDocMemBuf* pMbufSignature)
{
  int err = ERR_OK;
  struct mss__AP_InfoType apInfo;
  struct mss__MSSP_InfoType msspInfo;
  struct mss__MSS_USCOREStatusResp resp;
  char szTimestamp[30], szTxnId[20], *pBase64Hash = 0;

  RETURN_IF_NULL_PARAM(pMssp);
  RETURN_IF_NULL_PARAM(pMbufSignature);
  ddocDebug(3, "ddocMsspStatusReq", "Request status of txn: %ld", pMssp->lMsspTxnId);
  // init parameters
  time(&pMssp->lApTxnId);
  sprintf(szTxnId, "%ld", pMssp->lApTxnId);
  createTimestamp(0, szTimestamp, sizeof(szTimestamp));
  apInfo.AP_USCOREID = (char*)ConfigItem_lookup("DIGIDOC_MSSP_AP_USERID");
  apInfo.AP_USCORETransID = szTxnId;
  apInfo.AP_USCOREPWD = (char*)ConfigItem_lookup("DIGIDOC_MSSP_AP_PASSWD");
  apInfo.Instant = szTimestamp;
  apInfo.AP_USCOREURL = (char*)ConfigItem_lookup("DIGIDOC_MSSP_URL");
  msspInfo.MSSP_USCOREID.mss__DNSName = 0;
  msspInfo.MSSP_USCOREID.mss__IPAddress = 0;
  msspInfo.MSSP_USCOREID.mss__IdentifierString = 0;
  msspInfo.MSSP_USCOREID.mss__URI = (char*)ConfigItem_lookup("DIGIDOC_MSSP_URL");
  msspInfo.Instant = szTimestamp;

  // reconnect if necessary
  err = ddocMsspReconnect(pMssp);
  // send MSSP request
  ddocDebug(3, "ddocMsspStatusReq", "Sending request!");
  memset(&resp, 0, sizeof(resp));
  err = soap_call_mss__MSS_USCOREStatusReq(&(pMssp->soap), 
	    (const char*)pMssp->endpoint, "",
	    ConfigItem_lookup_int("DIGIDOC_MSSP_MAJOR", 0),
	    ConfigItem_lookup_int("DIGIDOC_MSSP_MINOR", 0),
	    apInfo, msspInfo, pMssp->lMsspTxnId, &resp);
  // report error or success
  if(!err) {
    pMssp->nStatusCode = resp.Status.StatusCode.Value;
    ddocDebug(3, "ddocMsspStatusReq", "RC: %d Status: %d - %s message: %s detail: %s",
	    err, resp.Status.StatusCode.Value, 
	    (resp.Status.StatusCode.__item ? resp.Status.StatusCode.__item : ""),
	    (resp.Status.StatusMessage ? resp.Status.StatusMessage : ""),
	    (resp.Status.StatusDetail ? resp.Status.StatusDetail : ""));
  } else {
    ddocDebug(3, "ddocMsspStatusReq", "RC: %d - no status", err);
    pMssp->nStatusCode = INTERNAL_ERROR;
  }
  if(!err)
    pMssp->nStatusCode = resp.Status.StatusCode.Value;
  if(err) {
    addError(ERR_GENERIC_SOAP_ERR, __FILE__, __LINE__, ddocGetSoapFaultCode(pMssp));
    addError(ERR_GENERIC_SOAP_ERR, __FILE__, __LINE__, ddocGetSoapFaultString(pMssp));
    addError(ERR_GENERIC_SOAP_ERR, __FILE__, __LINE__, ddocGetSoapFaultDetail(pMssp));
    soap_print_fault(&(pMssp->soap), stdout);
  }

  // get signature value if it exists
  if(err == SOAP_OK && resp.MSS_USCORESignature && 
     resp.MSS_USCORESignature->Base64Signature &&
     resp.MSS_USCORESignature->Base64Signature->__ptr &&
     resp.MSS_USCORESignature->Base64Signature->__size) {
    ddocDebug(3, "ddocMsspStatusReq", "Received signature value of %d bytes",
	      resp.MSS_USCORESignature->Base64Signature->__size);
    ddocMemAssignData(pMbufSignature, 
		      (const char*)resp.MSS_USCORESignature->Base64Signature->__ptr,
		      resp.MSS_USCORESignature->Base64Signature->__size);
  }
  pMssp->endpoint = NULL; // force reconnect
  if(err != SOAP_OK)
    err = ERR_GENERIC_SOAP_ERR;
  return err;
}

char * g_szCertPrefix = "-----BEGIN CERTIFICATE-----";
char * g_szCertSuffix = "-----END CERTIFICATE-----";

//--------------------------------------------------
// Reads the signers certificate based on the signers 
// phone number.
// szPhoneNo - phone number
// szCertUrl - full url where to get cert. Use NULL for default url
// ppCert - address to store the certificate
// bAuthCert - 1=auth cert, 0=sign cert
// returns error coder or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocMsspReadCertificate(const char* szPhoneNo, const char* szCertUrl, 
				       X509 **ppCert, int bAuthCert)
{
  int err = ERR_OK;
  DigiDocMemBuf mbuf1, mbuf2;
  char  *p1, *p2, *p3;

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  mbuf2.pMem = 0;
  mbuf2.nLen = 0;
  *ppCert = 0;
  RETURN_IF_NULL_PARAM(szPhoneNo);
  ddocDebug(3, "ddocMsspReadCertificate", "Read certificate for No: %s auth: %d url: %s", 
	    szPhoneNo, bAuthCert, (szCertUrl ? szCertUrl : "NULL"));
  p3 = (char*)ConfigItem_lookup("DIGIDOC_MSSP_CERT_URL_HOST");
  p1 = (char*)ConfigItem_lookup("DIGIDOC_PROXY_HOST");
  p2 = (char*)ConfigItem_lookup("DIGIDOC_PROXY_PORT");
  ddocMemAssignData(&mbuf1, "GET ", -1);
  // if URL was given then use it
  if(szCertUrl && strlen(szCertUrl)) {
    ddocMemAppendData(&mbuf1, szCertUrl, -1);
  } else { // compose default cert url based on phone no
    if(p1 || p2) // if we use proxy then send the full URL of cert
      ddocMemAppendData(&mbuf1, ConfigItem_lookup("DIGIDOC_MSSP_CERT_URL_HOST"), -1);
    ddocMemAppendData(&mbuf1, ConfigItem_lookup((bAuthCert == 1) ?
		"DIGIDOC_MSSP_AUTH_CERT_DIR" : "DIGIDOC_MSSP_SIGN_CERT_DIR"), -1);
    ddocMemAppendData(&mbuf1, szPhoneNo, -1);
    ddocMemAppendData(&mbuf1, ".pem", -1);
  }
  ddocMemAppendData(&mbuf1, " HTTP/1.0\r\n", -1);
  ddocMemAppendData(&mbuf1, "User-Agent: cdigidoc\r\n", -1);
  ddocMemAppendData(&mbuf1, "Host: www.sk.ee\r\n", -1);
  ddocMemAppendData(&mbuf1, "Accept: */*\r\n", -1);
  ddocMemAppendData(&mbuf1, "Connection: Close\r\n", -1);
  if(p1 || p2) // if we use proxy then send also Proxy-Connection
    ddocMemAppendData(&mbuf1, "Proxy-Connection: Close\r\n", -1);
  ddocMemAppendData(&mbuf1, "\r\n", -1);
  ddocDebug(4, "ddocMsspReadCertificate", "Send to host: %s request:\n------\n%s\n------\n", 
	    p3, (char*)mbuf1.pMem);
  err = ddocPullUrl(p3, &mbuf1, &mbuf2, p1, p2);
  ddocDebug(4, "ddocMsspReadCertificate", "RC: %d RECEIVED: %d bytes\n-----------------------------------\n%s\n-----------------------------------", err, mbuf2.nLen, (const char*)mbuf2.pMem);
  if(!err && mbuf2.pMem && mbuf2.nLen) {
    p1 = strstr((char*)mbuf2.pMem, g_szCertPrefix);
    if(p1) {
      p1 += strlen(g_szCertPrefix);
      while(*p1 && !isalnum(*p1)) p1++;
      p2 = strstr(p1, g_szCertSuffix);
      if(p2) {
	*p2 = 0;
	err = ddocDecodeX509PEMData(ppCert, p1, strlen(p1));    
	ddocDebug(3, "ddocMsspReadCertificate", "Decoding cert rc: %d - %s ", 
		  err, (*ppCert ? "OK" : "ERROR"));
      }
    }
  }
  // cleanup
  ddocMemBuf_free(&mbuf1);
  ddocMemBuf_free(&mbuf2);
  RETURN_IF_NOT(*ppCert, ERR_CERT_READ);
  return err;
}

