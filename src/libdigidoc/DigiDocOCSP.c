//==================================================
// FILE:	DigiDocOCSP.c
// PROJECT:     Digi Doc
// DESCRIPTION: DigiDoc OCSP handling routines
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
//      26.04.2006      Veiko Sinivee
//                      Creation
//==================================================

#include <libdigidoc/DigiDocOCSP.h>
#include <libdigidoc/DigiDocError.h>

#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocConvert.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/DigiDocVerify.h>

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

//================< OCSP functions> =================================



//============================================================
// Decodes binary (DER) OCSP_RESPONSE data and returns a OCSP_RESPONSE object
// ppResp - pointer to a buffer to receive newly allocated OCSP_RESPONSE pointer
// data - (DER) OCSP_RESPONSE data
// len - length of data in bytes
//============================================================
EXP_OPTION int ddocDecodeOCSPResponseData(OCSP_RESPONSE **ppResp, const byte* data, int len)
{
  BIO* b1 = 0;

  // check input params
  RETURN_IF_NULL_PARAM(data);
  RETURN_IF_NULL_PARAM(ppResp);
  // mark as not read yet
  *ppResp = 0;
  // create BIO
  b1 = BIO_new_mem_buf((void*)data, len);
  RETURN_IF_NOT(b1, ERR_NULL_POINTER);
  // decode OCSP
  *ppResp = d2i_OCSP_RESPONSE_bio(b1, NULL);
  BIO_free(b1);
  ddocDebug(3, "ddocDecodeOCSPResponseData", "Decoding %d bytes DER data - OCSP_RESPONSE %s", len, (*ppResp ? "OK" : "ERROR"));
  RETURN_IF_NOT(*ppResp, ERR_OCSP_UNKNOWN_TYPE);
  return ERR_OK;
}

//============================================================
// Decodes base64 (PEM) OCSP_RESPONSE data and returns a OCSP_RESPONSE object
// ppResp - pointer to a buffer to receive newly allocated OCSP_RESPONSE pointer
// data - (PEM) OCSP_RESPONSE data
// len - length of data in bytes
//============================================================
EXP_OPTION int ddocDecodeOCSPResponsePEMData(OCSP_RESPONSE **ppResp, const byte* data, int len)
{
  byte* p1 = 0;
  int l1 = 0, err = ERR_OK;

  // check input params
  RETURN_IF_NULL_PARAM(data);
  RETURN_IF_NULL_PARAM(ppResp);
  // mark as not read yet
  *ppResp = 0;
  // allocate memory for decoding
  l1 = len; // should be enough as it shrinks
  p1 = (byte*)malloc(l1);
  RETURN_IF_BAD_ALLOC(p1);
  memset(p1, 0, l1);
  // decode base64 data
  decode((const byte*)data, len, p1, &l1);
  // decode OCSP
  err = ddocDecodeOCSPResponseData(ppResp, p1, l1);
  // cleanup
  if(p1)
	  free(p1);
  ddocDebug(3, "ddocDecodeOCSPResponsePEMData", "Decoding %d bytes PEM data - OSCP_RESPONSE %s", len, (*ppResp ? "OK" : "ERROR"));
  return err;
}

//============================================================
// Reads in an OCSP Response file in DER format
// szFileName - OCSP response file name
//============================================================
int ReadOCSPResponse(OCSP_RESPONSE **newOCSP_RESPONSE, const char* szFileName)
{
  BIO *bin = NULL;
  OCSP_RESPONSE *resp = NULL;
  int err = ERR_OK;
	
  ddocDebug(4, "ReadOCSPResponse", "File: %s", szFileName);
  RETURN_IF_NULL_PARAM(newOCSP_RESPONSE);
  RETURN_IF_NULL_PARAM(szFileName);

  if((bin = BIO_new_file(szFileName, "rb")) != NULL) {
    ddocDebug(4, "ReadOCSPResponse", "File opened");
    resp = d2i_OCSP_RESPONSE_bio(bin, NULL);
    BIO_free(bin);
    if (resp == NULL) {
      err = ERR_OCSP_WRONG_VERSION;
    }
  } else {
    ddocDebug(4, "ReadOCSPResponse", "Cannot read file:%s", szFileName);
    err =ERR_FILE_READ;
  }
  if (err != ERR_OK) SET_LAST_ERROR(err);
  *newOCSP_RESPONSE = resp;
  return err;
}

//============================================================
// Writes an OCSP Response to a file in DER format
// szFileName - OCSP response file name
// resp - OCSP response object
//============================================================
int WriteOCSPResponse(const char* szFileName, const OCSP_RESPONSE* resp)
{
  BIO* bout = 0;

  RETURN_IF_NULL_PARAM(szFileName);
  RETURN_IF_NULL_PARAM(resp);
  if((bout = BIO_new_file(szFileName, "wb")) != NULL) {
#if OPENSSL_VERSION_NUMBER > 0x00908000
    ASN1_i2d_bio((int (*)(void*, unsigned char**))i2d_OCSP_RESPONSE, bout, (unsigned char*)resp);
#else
    ASN1_i2d_bio((int (*)())i2d_OCSP_RESPONSE, bout, (unsigned char*)resp);
#endif
    //i2d_OCSP_RESPONSE_bio((unsigned char*)bout, resp);
    BIO_free(bout);
  } else 
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_WRITE);
  return ERR_OK;	
}

//============================================================
// Reads in an OCSP Request file in DER format
// szFileName - OCSP Request file name
//============================================================
int ReadOCSPRequest(OCSP_REQUEST **newOCSP_REQUEST, const char* szFileName)
{
  BIO *bin = NULL;
  OCSP_REQUEST *req = NULL;
  int err = ERR_OK;
  
  RETURN_IF_NULL_PARAM(*newOCSP_REQUEST);
  RETURN_IF_NULL_PARAM(szFileName);
  
  if((bin = BIO_new_file(szFileName, "rb")) != NULL) {
    req = d2i_OCSP_REQUEST_bio(bin, NULL);
    BIO_free(bin);
    if (req == NULL) {
      err = ERR_OCSP_WRONG_VERSION;
    }
  } else
    err =ERR_FILE_READ;
  if (err != ERR_OK) SET_LAST_ERROR(err);
  *newOCSP_REQUEST = req;
  return err;
}

//============================================================
// Writes an OCSP Request to a file in DER format
// szFileName - OCSP Request file name
// resp - OCSP Request object
//============================================================
int WriteOCSPRequest(const char* szFileName, const OCSP_REQUEST* req)
{
  BIO* bout = 0;
	
  if((bout = BIO_new_file(szFileName, "wb")) != NULL) {
#if OPENSSL_VERSION_NUMBER > 0x00908000
    ASN1_i2d_bio((int (*)(void*, unsigned char**))i2d_OCSP_RESPONSE, bout, (unsigned char*)req);
#else
    ASN1_i2d_bio((int (*)())i2d_OCSP_RESPONSE, bout, (unsigned char*)req);
#endif
    //i2d_OCSP_REQUEST_bio(bout, req);
    BIO_free(bout);
  } else
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_WRITE);
  return ERR_OK;
}



//============================================================
// Checks OCSP certificate status and handles errors
// status - status code
// return error code
//============================================================
int handleOCSPCertStatus(int status)
{
  int err = ERR_OK;
  switch(status) {
  case V_OCSP_CERTSTATUS_GOOD: // cert is ok, do nothing
    break;
  case V_OCSP_CERTSTATUS_REVOKED: // cert has been revoked
    err = ERR_OCSP_CERT_REVOKED;
    break;
  case V_OCSP_CERTSTATUS_UNKNOWN: // cert status unknown
    err = ERR_OCSP_CERT_UNKNOWN;
    break;
  default: // should never happen?
    err = ERR_OCSP_RESP_STATUS;
  }
  return err;
}

//============================================================
// Calculates NotaryInfo digest if possible
// pSigDoc - digidoc main object pointer
// pNotary - NotaryInfo object to be initialized
// return error code
//============================================================
int calcNotaryDigest(SignedDoc* pSigDoc, NotaryInfo* pNotary)
{
  int err = ERR_OK, l1;
  char buf1[25];

  RETURN_IF_NULL_PARAM(pNotary);
  RETURN_IF_NULL_PARAM(pSigDoc);
  l1 = sizeof(buf1);
  err = calculateNotaryInfoDigest(pSigDoc, pNotary, (byte*)buf1, &l1);
  //err = calculateOcspBasicResponseDigest(br, buf1, &l1);
  if(!err) {
    err = ddocNotInfo_SetOcspDigest(pNotary, buf1, l1);
  }
  return err;
}

//============================================================
// Initializes NotaryInfo object with data from OCSP object
// pSigDoc - digidoc main object pointer
// pNotary - NotaryInfo object to be initialized
// resp - OCSP response object
// notCert - Notary cert object
// return error code
//============================================================
int initializeNotaryInfoWithOCSP(SignedDoc *pSigDoc, NotaryInfo *pNotary, 
				OCSP_RESPONSE *resp, X509 *notCert, int initDigest)
{
  int n, err = ERR_OK;
  char buf[500];
  OCSP_RESPBYTES *rb = NULL;
  OCSP_BASICRESP *br = NULL;
  OCSP_RESPDATA  *rd = NULL;
  OCSP_RESPID *rid = NULL;
  // OCSP_CERTSTATUS *cst = NULL;
  OCSP_SINGLERESP *single = NULL;
  OCSP_CERTID *cid = NULL;
  X509_EXTENSION *nonce;
	//AM 26.09.08
	DigiDocMemBuf mbuf1;
	mbuf1.pMem = 0;
	mbuf1.nLen = 0;
	
  RETURN_IF_NULL_PARAM(pNotary);
  RETURN_IF_NULL_PARAM(resp);
  // check the OCSP Response validity
  switch(OCSP_response_status(resp)) {
  case OCSP_RESPONSE_STATUS_SUCCESSFUL: // OK
    break;
  case OCSP_RESPONSE_STATUS_MALFORMEDREQUEST:
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_MALFORMED);
  case OCSP_RESPONSE_STATUS_INTERNALERROR:
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_INTERNALERR);
  case OCSP_RESPONSE_STATUS_TRYLATER:
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_TRYLATER);
  case OCSP_RESPONSE_STATUS_SIGREQUIRED:
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_SIGREQUIRED);
  case OCSP_RESPONSE_STATUS_UNAUTHORIZED:
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_UNAUTHORIZED);
  default:
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_UNSUCCESSFUL);
  }
  RETURN_IF_NULL_PARAM(resp->responseBytes);;
  rb = resp->responseBytes;
  if(OBJ_obj2nid(rb->responseType) != NID_id_pkix_OCSP_basic) 
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_UNKNOWN_TYPE);
  if((br = OCSP_response_get1_basic(resp)) == NULL) 
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_NO_BASIC_RESP);
  rd = br->tbsResponseData;
  if(ASN1_INTEGER_get(rd->version) != 0) 
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_WRONG_VERSION);
  n = sk_OCSP_SINGLERESP_num(rd->responses);
  if(n != 1) 
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_ONE_RESPONSE);
  single = sk_OCSP_SINGLERESP_value(rd->responses, 0);
  RETURN_IF_NULL(single);
  cid = single->certId;
  RETURN_IF_NULL(cid);
  ddocDebug(4, "initializeNotaryInfoWithOCSP", "CertStatus-type: %d", single->certStatus->type);
  //printf("TYPE: %d\n", single->certStatus->type);
  if(single->certStatus->type != 0) {
    ddocDebug(4, "initializeNotaryInfoWithOCSP", "errcode: %d", handleOCSPCertStatus(single->certStatus->type));
    SET_LAST_ERROR_RETURN_CODE(handleOCSPCertStatus(single->certStatus->type));
  }
  //Removed 31.10.2003
  //if(single->singleExtensions) 
  //	SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_NO_SINGLE_EXT);
  if(!rd->responseExtensions ||
     (sk_X509_EXTENSION_num(rd->responseExtensions) != 1) ||
     ((nonce = sk_X509_EXTENSION_value(rd->responseExtensions, 0)) == NULL)) 
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_NO_NONCE);
  i2t_ASN1_OBJECT(buf,sizeof(buf),nonce->object);
  if(strcmp(buf, OCSP_NONCE_NAME)) 
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_NO_NONCE);
  rid =  rd->responderId;
  if(rid->type == V_OCSP_RESPID_NAME) {
    pNotary->nRespIdType = RESPID_NAME_TYPE;
  } else if(rid->type == V_OCSP_RESPID_KEY) {
    pNotary->nRespIdType = RESPID_KEY_TYPE;
  } else {
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_WRONG_RESPID);
  }
  // producedAt
  err = asn1time2str(pSigDoc, rd->producedAt, buf, sizeof(buf));
  setString(&(pNotary->timeProduced), buf, -1);
  n = sizeof(buf);
  if(rid->type == V_OCSP_RESPID_NAME) {
    //X509_NAME_oneline(rid->value.byName,buf,n);
		//AM 26.09.08
		err = ddocCertGetDNFromName(rid->value.byName, &mbuf1);
		RETURN_IF_NOT(err == ERR_OK, err);
		err = ddocNotInfo_SetResponderId(pNotary, (char*)mbuf1.pMem, -1);
		ddocMemBuf_free(&mbuf1);
  }
  if(rid->type == V_OCSP_RESPID_KEY) {
    err = ddocNotInfo_SetResponderId(pNotary, (const char*)rid->value.byKey->data, rid->value.byKey->length);
  }
  // digest type
  i2t_ASN1_OBJECT(buf,sizeof(buf),cid->hashAlgorithm->algorithm);
  setString(&(pNotary->szDigestType), buf, -1);
  // signature algorithm
  i2t_ASN1_OBJECT(buf,sizeof(buf),br->signatureAlgorithm->algorithm);
  setString(&(pNotary->szSigType), buf, -1);
  // notary cert
  if(notCert && !err)
    err = addNotaryInfoCert(pSigDoc, pNotary, notCert);
  // save the response in memory
  err = ddocNotInfo_SetOCSPResponse_Value(pNotary, resp);
  // get the digest from original OCSP data
  if(initDigest && notCert) {
    err = calcNotaryDigest(pSigDoc, pNotary);
  }
  if(br != NULL)
    OCSP_BASICRESP_free(br);
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}

//--------------------------------------------------
// Helper function to read OCSP_RESPONSE from binary input data
// ppResp - address of newly allocated OCSP_RESPONSE object
// pMBufInData - input data
// returns error code or ERR_OK
//--------------------------------------------------
int ddocOcspReadOcspResp(OCSP_RESPONSE** ppResp, DigiDocMemBuf* pMBufInData)
{
  int err = ERR_OK;
  unsigned char* p1;

  RETURN_IF_NULL_PARAM(ppResp);
  RETURN_IF_NULL_PARAM(pMBufInData);
  RETURN_IF_NULL_PARAM(pMBufInData->pMem);
  ddocDebug(4, "ddocOcspReadOcspResp", "converting: %d bytes to OCSP_RESPONSE", pMBufInData->nLen);
  p1 = (unsigned char*)pMBufInData->pMem;
  d2i_OCSP_RESPONSE(ppResp, (const unsigned char**)&p1, pMBufInData->nLen);
  ddocDebug(4, "ddocOcspReadOcspResp", "OCSP_RESPONSE: %s", (*ppResp ? "OK" : "ERR"));
  return err;
}

//--------------------------------------------------
// Helper function to write OCSP_RESPONSE to binary output data
// pResp - address of OCSP_RESPONSE object
// pMBufOutData - output data
// returns error code or ERR_OK
//--------------------------------------------------
int ddocOcspWriteOcspResp(OCSP_RESPONSE* pResp, DigiDocMemBuf* pMBufOutData)
{
  int err = ERR_OK, l1;
  unsigned char* p1;

  RETURN_IF_NULL_PARAM(pResp);
  RETURN_IF_NULL_PARAM(pMBufOutData);
  pMBufOutData->pMem = NULL;
  pMBufOutData->nLen = 0;
  // find out how big a buffer we need
  l1 = i2d_OCSP_RESPONSE(pResp, NULL);
  ddocDebug(4, "ddocOcspReadOcspResp", "converting: %d bytes from OCSP_RESPONSE", l1);
  // alloc mem
  err = ddocMemSetLength(pMBufOutData, l1 + 50);  
  p1 = (unsigned char*)pMBufOutData->pMem;
  l1 = i2d_OCSP_RESPONSE(pResp, &p1);
  pMBufOutData->nLen = l1;
  ddocDebug(4, "ddocOcspReadOcspResp", "Converted data: %d", l1);
  return err;
}

//============================================================
// Converts OCSP_RESPONSE to PEM form with or without the headers
// pResp - OCSP_RESPONSE
// bHeaders - 1= with headers, 0=no headers
// buf - output buffer newly allocated
// returns error code
//============================================================
EXP_OPTION int getOcspPEM(OCSP_RESPONSE* pResp, int bHeaders, char** buf)
{
  int l1, l2;
  char *p1, *p2;

  RETURN_IF_NULL_PARAM(buf);
  RETURN_IF_NULL_PARAM(pResp);
  l1 = i2d_OCSP_RESPONSE(pResp, NULL);
  p1 = (char*)malloc(l1+10);
  RETURN_IF_BAD_ALLOC(p1);
  p2 = p1;
  i2d_OCSP_RESPONSE(pResp, (unsigned char**)&p2);
  l2 = l1 * 2 + 200;
  *buf = (char*)malloc(l2);
  if(*buf == NULL) {
    free(p1);
    RETURN_IF_BAD_ALLOC(*buf);
  }
  memset(*buf, 0, l2);
  if(bHeaders)
    strncpy(*buf, "-----BEGIN OCSP RESPONSE-----\n", l2);
  encode((const byte*)p1, l1, (byte*)strchr(*buf, 0), &l2);
  if(bHeaders)
    strncat(*buf, "\n-----END OCSP RESPONSE-----", l2 - strlen(*buf));
  free(p1);
  return ERR_OK;
}

//--------------------------------------------------
// Helper function to return OCSP_RESPONSE in base64 form
// Memory buffer will be resized as necessary.
// Caller must release output buffer.
// pNotary - Notary object
// bHeaders - 1= with headers, 0=no headers
// pMBufOutData - output data
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocGetOcspBase64(NotaryInfo *pNotary, int bHeaders, DigiDocMemBuf* pMBufOutData)
{
  const DigiDocMemBuf *pMBuf = 0;
  DigiDocMemBuf mbuf1;

  RETURN_IF_NULL_PARAM(pNotary);
  RETURN_IF_NULL_PARAM(pMBufOutData);
  pMBufOutData->pMem = 0;
  pMBufOutData->nLen = 0;
  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  pMBuf = ddocNotInfo_GetOCSPResponse(pNotary);
  RETURN_IF_NULL(pMBuf);
  if(bHeaders) {
    ddocMemAppendData(pMBufOutData, "-----BEGIN OCSP RESPONSE-----\n", -1);
    ddocEncodeBase64(pMBuf, &mbuf1);
    ddocMemAppendData(pMBufOutData, (const char*)mbuf1.pMem, mbuf1.nLen);
    ddocMemAppendData(pMBufOutData, "\n-----END OCSP RESPONSE-----", -1);
    ddocMemBuf_free(&mbuf1);
  }
  else
    ddocEncodeBase64(pMBuf, pMBufOutData);
  return ERR_OK;
}


//--------------------------------------------------
// teeb 00:0a:df stiilis hexprinditud stringist tagasi tavalise
//--------------------------------------------------
// Tanel - ver 1.66
unsigned char *decodeHex(unsigned char *str)
{
  unsigned int i, j, k, len;
  unsigned char *ret;
  static unsigned char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', 
				 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  
  len = (int)(strlen((const char*)str) / 3) + 2;
  if((ret=(unsigned char*)malloc(len)) == NULL)
    return NULL;
  memset(ret, 0, len);
  for(i=0, j=0; i<strlen((const char*)str); i+=3) {
    for(k=0; k<16; k++)
      if(str[i] == hex[k])
	ret[j] = (unsigned char)(k<<4);
    for(k=0; k<16; k++)
      if(str[i+1] == hex[k])
	ret[j++] += (unsigned char)k;
  }
  return(ret);
}

//--------------------------------------------------
// otsib X.509v3 laienduste seest Authority Key Identifieri välja
//--------------------------------------------------
// Tanel - ver 1.66
unsigned char *get_authority_key(STACK_OF(X509_EXTENSION) *exts)
{
  int i, found=0;
  X509_EXTENSION *ex=0;
  ASN1_OBJECT *obj;
  X509V3_EXT_METHOD *met;
  void *st = NULL;
  unsigned char *p;
  STACK_OF(CONF_VALUE) *vals=NULL;
  CONF_VALUE *val;
  unsigned char *ret = 0;
  
  for(i=0; i<sk_X509_EXTENSION_num(exts); i++) {
    ex = sk_X509_EXTENSION_value(exts, i);
    obj = X509_EXTENSION_get_object(ex);
    if(OBJ_obj2nid(obj) == NID_authority_key_identifier) {
      found++;
      break;
    }
  }

  if(!found) {
    ddocDebug(4, "get_authority_key", "Extension not found");
    return(NULL);
  }
  
  met = X509V3_EXT_get(ex);
  p = ex->value->data;
#if OPENSSL_VERSION_NUMBER > 0x00908000
  // crashes here!
  st = ASN1_item_d2i(NULL, (const unsigned char**)&p, ex->value->length, ASN1_ITEM_ptr(met->it));
#else
  st = ASN1_item_d2i(NULL, &p, ex->value->length, ASN1_ITEM_ptr(met->it));
#endif
  vals = met->i2v(met, st, NULL);

  /* P.R */
  ASN1_item_free((ASN1_VALUE *)st, ASN1_ITEM_ptr(met->it));
  /* P.R */
  
  for(i=0; i<sk_CONF_VALUE_num(vals); i++) {
    val = sk_CONF_VALUE_value(vals, i);
    ddocDebug(4, "get_authority_key", "Extension %s - %s", val->name, val->value);
    if(val->name && (strcmp(val->name, "keyid") == 0))
	  ret = decodeHex((unsigned char*)val->value);
  }
  /* P.R */ 
  sk_CONF_VALUE_pop_free(vals, X509V3_conf_free);
  /* P.R */
 
  return ret;

}



//--------------------------------------------------
// creates OCSP_CERTID without using the issuer cert
// cert - certificate for which we need certid
// returns OCSP_CERTID pointer
//--------------------------------------------------
// Tanel - ver 1.66
OCSP_CERTID* createOCSPCertid(X509 *cert, X509* pCACert)
{
  OCSP_CERTID *pId = NULL;
  X509_NAME *iname;
  unsigned char *ikey = NULL;
  ASN1_INTEGER *sno;
  const EVP_MD *dgst;
  X509_ALGOR *alg;
  unsigned char md[EVP_MAX_MD_SIZE];
  unsigned int len;

  if(cert != NULL) {
    // standard variant would be
    //pId = OCSP_cert_to_id(EVP_sha1(), cert, issuer);

    // issuer name hashi arvutamine 
    iname = X509_get_issuer_name(cert);
    dgst = EVP_sha1();
	len = sizeof(md);
    if(X509_NAME_digest(iname, dgst, md, &len)) {
      // issuer key hashi lugemine
      ikey = get_authority_key(cert->cert_info->extensions);
      if(ikey != NULL) {
	// serial numbri lugemine
	sno = X509_get_serialNumber(cert);
	// OCSP certid koostamine
	if((pId = OCSP_CERTID_new()) != NULL) {
	  // replace default algorithm ???
	  alg = pId->hashAlgorithm;
	  if(alg->algorithm != NULL)
	    ASN1_OBJECT_free(alg->algorithm);
	  alg->algorithm = OBJ_nid2obj(EVP_MD_type(dgst));
	  if((alg->parameter = ASN1_TYPE_new()) != NULL) {
	    alg->parameter->type = V_ASN1_NULL;
	    ASN1_INTEGER_free(pId->serialNumber);
	    pId->serialNumber = ASN1_INTEGER_dup(sno);
	    if(!ASN1_OCTET_STRING_set(pId->issuerNameHash, md, len) ||
	       !ASN1_OCTET_STRING_set(pId->issuerKeyHash, ikey, strlen((const char*)ikey)) ||
	       !pId->serialNumber) {
	      fprintf(stderr, "Unable to fill in CID\n");
	      OCSP_CERTID_free(pId);
	      pId = NULL;
	    }
	  } // else - failed to create algorithm					
	}
	// cleanup ikey
	free(ikey);

      } // else failed to find Authority Key Identifier
      else {
	//TODO: on UNIX crashes here - pCACert ?
	pId = OCSP_cert_to_id(NULL, cert, pCACert);

      }
    } // else - SHA1 failed	
  }
  return pId;
}

//--------------------------------------------------
// Helper function that handles OCSP nonce "the old way"
// e.g. by sending pure data instead of ASN.1 OCTET_STRING
// like in openssl 0.9.7d. This required to correctly 
// handle OCSP verification. This code was contributed
// by Sven Heiberg <sven@taru.cyber.ee>
//--------------------------------------------------
static int MY_ocsp_add1_nonce(STACK_OF(X509_EXTENSION) **exts, unsigned char *val, int len)
{  
  unsigned char *tmpval;  
  ASN1_OCTET_STRING os;  
  int ret = 0;  

  if (len <= 0) 
    len = OCSP_DEFAULT_NONCE_LENGTH;  
  if (val) 
    tmpval = val;  
  else {  
    tmpval = (unsigned char*)OPENSSL_malloc(len);
    if (!tmpval)
      goto err;    
    RAND_pseudo_bytes(tmpval, len);  
  }  
  os.data = tmpval;  
  os.length = len;  
  if(!X509V3_add1_i2d(exts, NID_id_pkix_OCSP_Nonce, &os, 0, X509V3_ADD_REPLACE))    
    goto err;  
  ret = 1; 
err:  
  if(!val) 
    OPENSSL_free(tmpval);  
  return ret;
}

int MY_OCSP_request_add1_nonce(OCSP_REQUEST *req, unsigned char *val, int len)
{  
  return MY_ocsp_add1_nonce(&req->tbsRequest->requestExtensions, val, len);
}

int MY_OCSP_basic_add1_nonce(OCSP_BASICRESP *resp, unsigned char *val, int len)
{  
  return MY_ocsp_add1_nonce(&resp->tbsResponseData->responseExtensions, val, len);
}


//--------------------------------------------------
// Creates an OCSP_REQUEST object
// pSigDoc - address of signed document. If not NULL then
// used to check if older openssl 0.9.6 style request must
// be constructed.
// req - buffer for storing the pointer of new object
// cert - client certificate to verify
// nonce - nonce value (e.g. client signature value RSA-128 bytes)
// nlen - nonce value length
// pkey - public key used to signe th request (not used yet)
//--------------------------------------------------
// VS - ver 1.66
int createOCSPRequest(SignedDoc* pSigDoc, OCSP_REQUEST **req, 
		      X509 *cert, X509* pCACert, byte* nonce, int nlen)
{
  int err = ERR_OK;
  OCSP_CERTID *id = 0;
	
  RETURN_IF_NULL_PARAM(req);
  RETURN_IF_NULL_PARAM(cert);
  RETURN_IF_NULL_PARAM(nonce);
  //RETURN_IF_NULL_PARAM(pCACert);
  if((*req = OCSP_REQUEST_new()) != 0) {
    // VS - ver 1.66
    if((id = createOCSPCertid(cert, pCACert)) != 0) {
      if(OCSP_request_add0_id(*req, id)) {
	if(pSigDoc && 
	   (!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER) ||
	    !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_1_VER) ||
	    !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER) 
#ifdef WITH_TS
	    ||  !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)
#endif
	    )) {
	     if((err = MY_OCSP_request_add1_nonce(*req, nonce, nlen)) != 0)
	       err = ERR_OK;
	   } else {
	     if((err = OCSP_request_add1_nonce(*req, nonce, nlen)) != 0)
	       err = ERR_OK;
	   }
      }
    }
  }
  return err;
}

/* Quick and dirty HTTP OCSP request handler.
 * Could make this a bit cleverer by adding
 * support for non blocking BIOs and a few
 * other refinements.
 * Qick and dirty adaption of openssl -s 
 * OCSP_sendreq_bio() to add UserAgent HTTP header
 */

OCSP_RESPONSE *OCSP_sendreq_bio_withParams(BIO *b, char *path, 
				      OCSP_REQUEST *req, unsigned long ip_addr )
{
  BIO *mem = NULL;
  char tmpbuf[1024], adrhdr[100];
  OCSP_RESPONSE *resp = NULL;
  char *p, *q, *r;
  int len, retcode;
  static char req_txt[] =
"POST %s HTTP/1.0\r\n\
Content-Type: application/ocsp-request\r\n\
User-Agent: LIB %s/%s APP %s\r\n%s\
Content-Length: %d\r\n\r\n";

  adrhdr[0] = 0;
  if(ip_addr > 0)
    snprintf(adrhdr, sizeof(adrhdr), "From: %d.%d.%d.%d\r\n", 
    (ip_addr>>24)&0xFF, (ip_addr>>16)&0xFF, (ip_addr>>8)&0xFF, ip_addr&0xFF); 
  len = i2d_OCSP_REQUEST(req, NULL);
  if(BIO_printf(b, req_txt, path, getLibName(), getLibVersion(), 
		getGUIVersion(), (ip_addr > 0 ? adrhdr : ""), len) < 0) {
    OCSPerr(OCSP_F_OCSP_SENDREQ_BIO,OCSP_R_SERVER_WRITE_ERROR);
    goto err;
  }
#if OPENSSL_VERSION_NUMBER > 0x00908000
  retcode = ASN1_i2d_bio((int (*)(void*, unsigned char**))i2d_OCSP_REQUEST, b, (unsigned char*)req);
#else
  retcode = ASN1_i2d_bio((int (*)())i2d_OCSP_REQUEST, b, (unsigned char*)req);
#endif
  if(retcode <= 0) {
    OCSPerr(OCSP_F_OCSP_SENDREQ_BIO,OCSP_R_SERVER_WRITE_ERROR);
    goto err;
  }
  mem = BIO_new(BIO_s_mem());
  if(!mem) goto err;
  /* Copy response to a memory BIO: socket bios can't do gets! */
  do {
    len = BIO_read(b, tmpbuf, sizeof tmpbuf);
    if(len < 0) {
      OCSPerr(OCSP_F_OCSP_SENDREQ_BIO,OCSP_R_SERVER_READ_ERROR);
      goto err;
    }
    BIO_write(mem, tmpbuf, len);
  } while(len > 0);
  if(BIO_gets(mem, tmpbuf, 512) <= 0) {
    OCSPerr(OCSP_F_OCSP_SENDREQ_BIO,OCSP_R_SERVER_RESPONSE_PARSE_ERROR);
    goto err;
  }
  /* Parse the HTTP response. This will look like this:
   * "HTTP/1.0 200 OK". We need to obtain the numeric code and
   * (optional) informational message.
   */
  
  /* Skip to first white space (passed protocol info) */
  for(p = tmpbuf; *p && !isspace((unsigned char)*p); p++) continue;
  if(!*p) {
    OCSPerr(OCSP_F_OCSP_SENDREQ_BIO,OCSP_R_SERVER_RESPONSE_PARSE_ERROR);
    goto err;
  }
  /* Skip past white space to start of response code */
  while(*p && isspace((unsigned char)*p)) p++;
  if(!*p) {
    OCSPerr(OCSP_F_OCSP_SENDREQ_BIO,OCSP_R_SERVER_RESPONSE_PARSE_ERROR);
    goto err;
  }
  /* Find end of response code: first whitespace after start of code */
  for(q = p; *q && !isspace((unsigned char)*q); q++) continue;
  if(!*q) {
    OCSPerr(OCSP_F_OCSP_SENDREQ_BIO,OCSP_R_SERVER_RESPONSE_PARSE_ERROR);
    goto err;
  }
  /* Set end of response code and start of message */ 
  *q++ = 0;
  /* Attempt to parse numeric code */
  retcode = strtoul(p, &r, 10);
  if(*r) goto err;
  /* Skip over any leading white space in message */
  while(*q && isspace((unsigned char)*q))  q++;
  if(*q) {
    /* Finally zap any trailing white space in message (include CRLF) */
    /* We know q has a non white space character so this is OK */
    for(r = q + strlen(q) - 1; isspace((unsigned char)*r); r--) *r = 0;
  }
  if(retcode != 200) {
    OCSPerr(OCSP_F_OCSP_SENDREQ_BIO,OCSP_R_SERVER_RESPONSE_ERROR);
    if(!*q) { 
      ERR_add_error_data(2, "Code=", p);
    }
    else {
      ERR_add_error_data(4, "Code=", p, ",Reason=", q);
    }
    goto err;
  }
  /* Find blank line marking beginning of content */	
  while(BIO_gets(mem, tmpbuf, 512) > 0)
  {
    for(p = tmpbuf; *p && isspace((unsigned char)*p); p++) continue;
    if(!*p) break;
  }
  if(*p) {
    OCSPerr(OCSP_F_OCSP_SENDREQ_BIO,OCSP_R_NO_CONTENT);
    goto err;
  }
  resp = d2i_OCSP_RESPONSE_bio(mem, NULL);
  if(!resp) {
    OCSPerr(OCSP_F_OCSP_SENDREQ_BIO,ERR_R_NESTED_ASN1_ERROR);
    goto err;
  }
 err:
  BIO_free(mem);
  return resp;
}


//--------------------------------------------------
// sends an OCSP_REQUES object to remore server and
// retrieves the OCSP_RESPONSE object
// resp - buffer to store the new responses pointer
// req - request objects pointer
// url - OCSP responder URL
// ip_addr - senders ip address if known or 0
//--------------------------------------------------
int sendOCSPRequest(OCSP_RESPONSE** resp, OCSP_REQUEST *req, 
		    char* url, char* proxyHost, char* proxyPort,
		    unsigned long ip_addr)
{	
  BIO* cbio = 0, *sbio = 0;
  SSL_CTX *ctx = NULL;
  char *host = NULL, *port = NULL, *path = "/";
  int err = ERR_OK, use_ssl = -1;
  int connResult = 0;
  long e;

  RETURN_IF_NULL_PARAM(resp);
  RETURN_IF_NULL_PARAM(req);
  RETURN_IF_NULL_PARAM(url);

  //there is an HTTP proxy - connect to that instead of the target host
  if (proxyHost != 0 && *proxyHost != '\0') {
    host = proxyHost;
    if(proxyPort != 0 && *proxyPort != '\0')
      port = proxyPort;
    path = url;
  } else {
    if(OCSP_parse_url(url, &host, &port, &path, &use_ssl) == 0) {
      //printf("BIO_parse_url failed\n");
      return ERR_WRONG_URL_OR_PROXY; 
    }
  }
  if((cbio = BIO_new_connect(host)) != 0) {
    if(port != NULL)
      BIO_set_conn_port(cbio, port);
    if (use_ssl == 1) {
      ctx = SSL_CTX_new(SSLv23_client_method());
      SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
      sbio = BIO_new_ssl(ctx, 1);
      cbio = BIO_push(sbio, cbio);
    }
    if ((connResult = BIO_do_connect(cbio)) > 0) {
	  e = checkErrors();
      //printf("BIO_do_connect returned %d\n", connResult);
      *resp = OCSP_sendreq_bio_withParams(cbio, path, req, ip_addr);
      //printf("OCSP_sendreq_bio answered %lX\n", *resp);
	  e = checkErrors();
	  if(ERR_GET_REASON(e) == BIO_R_BAD_HOSTNAME_LOOKUP ||
		 ERR_GET_REASON(e) == OCSP_R_SERVER_WRITE_ERROR)
		  err = ERR_CONNECTION_FAILURE;
	  //if(ERR_GET_REASON(e) == BIO_R_BAD_HOSTNAME_LOOKUP)
	//	  err = ERR_CONNECTION_FAILURE;
	  else
		err = (*resp == 0) ? ERR_OCSP_WRONG_URL : ERR_OK;
      //if (*resp == 0) 
      //  printErrors();
    } else {
      //printf("BIO_do_connect failed, rc = %d, shouldRetry = %d\n", connResult, BIO_should_retry(cbio));
      //printErrors();
      //if no connection
      if (host != NULL)
	err = ERR_WRONG_URL_OR_PROXY;
      else
	err = ERR_CONNECTION_FAILURE;
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

//--------------------------------------------------
// Creates and writes an OCSP_REQUEST object
// to disk
// pSigDoc - signedDoc address
// signerCertFile - cert file to verify
// issuertCertFile - this certs direct CA cert
// nonce - nonce (signature value)
// nlen - nonce length
// szOutputFile - output filename
//--------------------------------------------------
 EXP_OPTION int writeOCSPRequest(SignedDoc* pSigDoc, 
				 const char* signerCertFile, const char* issuertCertFile,
				 byte* nonce, int nlen, const char* szOutputFile) 

{
  OCSP_REQUEST *req = 0;
  X509 *cert = 0, *issuer = 0;
  int err = ERR_OK, l1;
  //EVP_PKEY* pkey; 
  byte buf1[DIGEST_LEN+2];

  RETURN_IF_NULL_PARAM(signerCertFile);
  RETURN_IF_NULL_PARAM(issuertCertFile);
  RETURN_IF_NULL_PARAM(nonce);
  RETURN_IF_NULL_PARAM(szOutputFile);

  if((err = ReadCertificate(&cert, signerCertFile)) == ERR_OK) {
    //pkey = ReadPublicKey(signerCertFile);
    if((err = ReadCertificate(&issuer, issuertCertFile)) == ERR_OK) {
      l1 = sizeof(buf1);
      calculateDigest(nonce, nlen, DIGEST_SHA1, buf1, &l1);
      err = createOCSPRequest(pSigDoc, &req, cert, issuer, buf1, l1);
      //WriteOCSPRequest(szOutputFile, req);
      X509_free(issuer);
			//AM 22.04.08
			if(req)
				OCSP_REQUEST_free(req);
    }
    X509_free(cert);
  }
  return err;
}

//--------------------------------------------------
// Signs an OCSP_REQUEST using PKCS#12 conteiner
// req - OCSP_REQUEST
// filename - PKCS#12 conteiner file
// passwd - key decryption passwd
//--------------------------------------------------
EXP_OPTION int signOCSPRequestPKCS12(OCSP_REQUEST *req, const char* filename, const char* passwd)
{
  EVP_PKEY *pkey;
  int err = ERR_OK;
  time_t tNow;
  
  STACK_OF(X509)* certs = NULL;
  X509* x509=0;
  
  RETURN_IF_NULL_PARAM(filename);
  err = ReadCertificateByPKCS12(&x509, filename, passwd, &pkey);
  RETURN_IF_NOT(err == ERR_OK, err);

  // VS: ver 1.66
  time(&tNow);
  err = isCertValid(x509, tNow);
  if (err != ERR_OK)
    X509_free(x509);
  RETURN_IF_NOT(err == ERR_OK, ERR_PKCS12_EXPIRED);
  certs = sk_X509_new_null();
  RETURN_IF_NULL(certs);

  //sk_X509_push(certs, x509);
  if (! OCSP_request_sign(req,x509,pkey,EVP_sha1(),certs,0)) {
    EVP_PKEY_free(pkey);
    err = ERR_OCSP_SIGN;
    SET_LAST_ERROR(err);
  }
  X509_free(x509);
  EVP_PKEY_free(pkey);
  return err;
}

//--------------------------------------------------
// Signs an OCSP_REQUEST using X509 cert and separate keyfile
// req - OCSP_REQUEST
// certFile - signers certificate file
// keyfile - signer's key file
// passwd - key decryption passwd
//--------------------------------------------------
EXP_OPTION int signOCSPRequest(OCSP_REQUEST *req,const char* certFile,const char* keyfile,const char* passwd){
	
  EVP_PKEY *pkey;
  int err = ERR_OK;
  STACK_OF(X509)* certs = NULL;
  X509* x509 = NULL;
  
  certs = sk_X509_new_null();
  RETURN_IF_NULL_PARAM(certs);
  
  if((err = ReadCertificate(&x509, certFile)) != ERR_OK) {
    SET_LAST_ERROR_RETURN_CODE(ERR_PKCS_CERT_LOC);
  }
  sk_X509_push(certs, x509);
  if((err = ReadPrivateKey(&pkey, keyfile, passwd, FILE_FORMAT_PEM)) == ERR_OK) {
    //ASN1_item_sign(ASN1_ITEM_rptr(OCSP_REQINFO),req->optionalSignature->signatureAlgorithm,NULL,req->optionalSignature->signature,req->tbsRequest,pkey,setSignAlgorithm(EVP_sha1()));
    //OCSP_request_sign_internal(req, x509,pkey, NULL);
    if(! OCSP_request_sign(req,x509,pkey,EVP_sha1(),certs,0)){
      EVP_PKEY_free(pkey);
      SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_SIGN);
    }
    //printf("OCSP_request_sign()=%d \n",r);
    EVP_PKEY_free(pkey);
  }else{
    SET_LAST_ERROR_RETURN_CODE(ERR_PRIVKEY_READ);
  }
  return err;
}

//--------------------------------------------------
// Creates and sends an OCSP_REQUEST object
// to the notary server, receives the response
// and uses it to create a confirmation object.
// pSigDoc - signed doc info
// pSigInfo - signature info
// caCerts - responder CA certs chain
// notaryCert - notarys cert search
// pkcs12FileName -  
// pkcs12Password - 
// notaryURL - notarys URL
// proxyHost - proxy host if needed
// proxyPort - proxy port if needed
//--------------------------------------------------
EXP_OPTION int getConfirmation(SignedDoc* pSigDoc, SignatureInfo* pSigInfo, 
			       const X509** caCerts, const X509* pNotCert,
			       char* pkcs12FileName, char* pkcs12Password,
			       char* notaryURL, char* proxyHost, char* proxyPort) 

{
  // default way to invoke it is without callers ip.
  return getConfirmationWithIp(pSigDoc, pSigInfo, caCerts, pNotCert,
			       pkcs12FileName, pkcs12Password,
			       notaryURL, proxyHost, proxyPort, 0);
}

//--------------------------------------------------
// Creates and sends an OCSP_REQUEST object
// to the notary server, receives the response
// and uses it to create a confirmation object.
// pSigDoc - signed doc info
// pSigInfo - signature info
// caCerts - responder CA certs chain
// notaryCert - notarys cert search
// pkcs12FileName -  
// pkcs12Password - 
// notaryURL - notarys URL
// proxyHost - proxy host if needed
// proxyPort - proxy port if needed
// ip - callers ip address if known
//--------------------------------------------------
EXP_OPTION int getConfirmationWithIp(SignedDoc* pSigDoc, SignatureInfo* pSigInfo, 
				     const X509** caCerts, const X509* pNotCert,
				     char* pkcs12FileName, char* pkcs12Password,
				     char* notaryURL, char* proxyHost, char* proxyPort,
				     unsigned long ip) 

{
  OCSP_REQUEST *req = 0;
  OCSP_RESPONSE *resp = 0;
  X509 *cert = 0, *pCA = 0;
  int err = ERR_OK, l1, i;
  byte buf1[DIGEST_LEN+2];
  NotaryInfo* pNotInf = NULL;
  DigiDocMemBuf* pMBuf;

  RETURN_IF_NULL_PARAM(pSigDoc);
  RETURN_IF_NULL_PARAM(pSigInfo);
  cert = ddocSigInfo_GetSignersCert(pSigInfo);
  RETURN_IF_NULL(cert);	
  RETURN_IF_NULL_PARAM(notaryURL);
  
  clearErrors();
	
  l1 = sizeof(buf1);
  pMBuf = ddocSigInfo_GetSignatureValue_Value(pSigInfo);
  RETURN_IF_NOT(pMBuf, err);
  // in format 1.4 we use just a plain random value for hash
#ifdef WITH_TS
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)) {
#ifdef WIN32
  RAND_screen();
  RAND_bytes((unsigned char*)buf1, DIGEST_LEN);
#else
  if(l1 = RAND_load_file("/dev/urandom", 1024)) {
    RAND_bytes((unsigned char*)buf1, DIGEST_LEN);
    l1 = DIGEST_LEN;
  }
#endif
  } else
#endif
    err = calculateDigest(pMBuf->pMem, pMBuf->nLen, DIGEST_SHA1, buf1, &l1);
  RETURN_IF_NOT(err == ERR_OK, err);
  
  // find lowest CA cert
  for(i = 0; (caCerts != NULL) && (caCerts[i] != NULL); i++)
    pCA = (X509*)caCerts[i];
  err = createOCSPRequest(pSigDoc, &req, cert, pCA, buf1, l1);
  RETURN_IF_NOT(err == ERR_OK, err);

  // if both are NULL then this means don't sign OCSP requests
  if(pkcs12FileName /*&& pkcs12Password*/) {
    ddocDebug(3, "getConfirmationWithIp", "Sign OCSP request with: %s", pkcs12FileName);
    err=signOCSPRequestPKCS12(req, pkcs12FileName, pkcs12Password);
  }
  RETURN_IF_NOT(err == ERR_OK, err);
  
  ddocDebug(3, "getConfirmationWithIp", "Send OCSP to: %s over: %s:%s", notaryURL,
	    (proxyHost ? proxyHost : ""), (proxyPort ? proxyPort : ""));
  err = sendOCSPRequest(&resp, req, notaryURL, proxyHost, proxyPort, ip);
  RETURN_IF_NOT(err == ERR_OK, err);

  err = NotaryInfo_new(&pNotInf, pSigDoc, pSigInfo);
  RETURN_IF_NOT(err == ERR_OK, err);

  err = initializeNotaryInfoWithOCSP(pSigDoc, pNotInf, resp, NULL, 1);
  RETURN_IF_NOT(err == ERR_OK, err);

  if(caCerts && pNotCert) {
    err = finalizeAndVerifyNotary(pSigDoc, pSigInfo, pNotInf, caCerts, pNotCert);
    RETURN_IF_NOT(err == ERR_OK, err);
  }
  if(resp)
    OCSP_RESPONSE_free(resp);
	//AM 22.04.08
	if(req)
		OCSP_REQUEST_free(req);
  return ERR_OK;
}


//--------------------------------------------------
// Adds responder certificate to notary, searches it's
// CA chain and then verifies notary
// pSigDoc - signed doc info
// pSigInfo - signature info
// caCertSearches - responder CA certs chain
// notaryCert - notarys cert search
// returns error code
//--------------------------------------------------
int EXP_OPTION finalizeAndVerifyNotary(SignedDoc* pSigDoc, SignatureInfo* pSigInfo, 
				       NotaryInfo* pNotInf,
				       const X509** caCerts, const X509* pNotCert)
{
  int err = ERR_OK;

  RETURN_IF_NULL_PARAM(pNotCert);
  RETURN_IF_NULL_PARAM(caCerts);
  ddocDebug(3, "finalizeAndVerifyNotary", "Notary: %s cert: %s", pNotInf->szId, (pNotCert ? "OK" : "NULL"));
  err = addNotaryInfoCert(pSigDoc, pNotInf, (X509*)pNotCert);
  RETURN_IF_NOT(err == ERR_OK, err);
  err = calcNotaryDigest(pSigDoc, pNotInf);
  RETURN_IF_NOT(err == ERR_OK, err);
  err = verifyNotaryInfoCERT(pSigDoc, pSigInfo, pNotInf, caCerts, NULL, pNotCert);
  RETURN_IF_NOT(err == ERR_OK, err);
  ddocDebug(3, "finalizeAndVerifyNotary", "rc: %d cert: %s cert2: %s", err, (pNotCert ? "OK" : "NULL"), (ddocSigInfo_GetOCSPRespondersCert(pSigInfo) ? "OK" : "NULL"));
  return ERR_OK;
}

//--------------------------------------------------
// Verfies OCSP_RESPONSE signature
// pResp - signed OCSP response
// caCerts - CA certificate pointer array terminated with NULL
// CApath - path to (directory) all certs
// notCertFile - Notary (e.g. OCSP responder) cert file 
//--------------------------------------------------
int verifyOCSPResponse(OCSP_RESPONSE* pResp, 
				    const X509** caCerts, const char *CApath, 
				    const X509* notCert)
{
  X509_STORE *store;
  OCSP_BASICRESP* bs = NULL;
  STACK_OF(X509)* ver_certs = NULL;
  int err = ERR_OK;
  
  RETURN_IF_NULL_PARAM(pResp);
  RETURN_IF_NOT(ASN1_ENUMERATED_get(pResp->responseStatus) == 0, ERR_OCSP_UNSUCCESSFUL);
  RETURN_IF_NOT(OBJ_obj2nid(pResp->responseBytes->responseType) == NID_id_pkix_OCSP_basic, ERR_OCSP_UNKNOWN_TYPE);
  RETURN_IF_NOT(caCerts != NULL, ERR_OCSP_RESP_NOT_TRUSTED);
  RETURN_IF_NOT(notCert != NULL, ERR_OCSP_CERT_NOTFOUND);
  RETURN_IF_NOT((bs = OCSP_response_get1_basic(pResp)) != NULL, ERR_OCSP_NO_BASIC_RESP);
  // now create an OCSP object and check its validity
  if((setup_verifyCERT(&store, CApath, caCerts)) == ERR_OK) {
    // new basic response
    // create OCSP basic response
    ver_certs = sk_X509_new_null();
    if(ver_certs) {
      sk_X509_push(ver_certs, notCert);
      err = OCSP_basic_verify(bs, ver_certs, store, OCSP_TRUSTOTHER);
      if(err == ERR_LIB_NONE) {
	err = ERR_OK;
      } else {
	//checkErrors();
	SET_LAST_ERROR(ERR_OCSP_WRONG_RESPID);
	err = ERR_OCSP_WRONG_RESPID;
      }
      // cleanup
      sk_X509_free(ver_certs);
    }
    X509_STORE_free(store);
  }
  if(bs)
    OCSP_BASICRESP_free(bs);
  return err;
}

int checkNonceAndCertbyOCSP(OCSP_RESPONSE* resp, X509* cert, byte* nonce1, int nonceLen)
{
  int err = ERR_OK, n;
  char buf[100];
  // OCSP_RESPBYTES *rb = resp->responseBytes;
  OCSP_BASICRESP *br = NULL;
  OCSP_RESPDATA  *rd = NULL;
  // OCSP_RESPID *rid = NULL;
  // OCSP_CERTSTATUS *cst = NULL;
  OCSP_SINGLERESP *single = NULL;
  OCSP_CERTID *cid = NULL;
  X509_EXTENSION *nonce;
  long certNr1;
  X509_NAME *iname;
  unsigned char *ikey;
	
  RETURN_IF_NULL_PARAM(resp);
  RETURN_IF_NULL_PARAM(cert);
  if((br = OCSP_response_get1_basic(resp)) == NULL) 
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_NO_BASIC_RESP);
  rd = br->tbsResponseData;
  n = sk_OCSP_SINGLERESP_num(rd->responses);
  RETURN_IF_NOT(n == 1, ERR_OCSP_ONE_RESPONSE);
  single = sk_OCSP_SINGLERESP_value(rd->responses, 0);
  RETURN_IF_NOT(single, ERR_OCSP_ONE_RESPONSE);
  cid = single->certId;
  RETURN_IF_NULL(cid);
  err = handleOCSPCertStatus(single->certStatus->type);
  if(err)
    SET_LAST_ERROR_RETURN_CODE(err);
  if(single->singleExtensions) 
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_NO_SINGLE_EXT);
  if(!rd->responseExtensions ||
     (sk_X509_EXTENSION_num(rd->responseExtensions) != 1) ||
     ((nonce = sk_X509_EXTENSION_value(rd->responseExtensions, 0)) == NULL)) 
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_NO_NONCE);
  i2t_ASN1_OBJECT(buf, sizeof(buf), nonce->object);
  if(strcmp(buf, OCSP_NONCE_NAME)) 
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_NO_NONCE);
  // check serial number
  certNr1 = ASN1_INTEGER_get(cid->serialNumber);
  if(certNr1 != ASN1_INTEGER_get(X509_get_serialNumber(cert)))
    SET_LAST_ERROR_RETURN_CODE(ERR_WRONG_CERT);
  // check issuer name hash
  iname = X509_get_issuer_name(cert);
  n = sizeof(buf);
  X509_NAME_digest(iname, EVP_sha1(), (byte*)buf, (unsigned int*)&n);
  err = compareByteArrays((byte*)buf, (unsigned int)n, cid->issuerNameHash->data, cid->issuerNameHash->length);
  RETURN_IF_NOT(err == ERR_OK, err);
  // check issuer key hash
  if((ikey = get_authority_key(cert->cert_info->extensions)) != NULL) {
    err = compareByteArrays(ikey, strlen((const char*)ikey), 
			    cid->issuerKeyHash->data, cid->issuerKeyHash->length);
    // cleanup ikey
    free(ikey);
  } 
  // verify nonce value
  if(nonce->value->length == DIGEST_LEN)
    err = compareByteArrays(nonce->value->data, nonce->value->length, nonce1, nonceLen);
  else
    err = compareByteArrays(nonce->value->data + 2, nonce->value->length - 2, nonce1, nonceLen);
  ddocDebug(3, "checkNonceAndCertbyOCSP", "nonce1-len: %d nonce2-len: %d err: %d", nonce->value->length, nonceLen, err);
  if (err != ERR_OK) SET_LAST_ERROR(err);
  if(br)
    OCSP_BASICRESP_free(br);
  return err;
}

