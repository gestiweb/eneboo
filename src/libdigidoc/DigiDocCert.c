//==================================================
// FILE:	DigiDocCert.c
// PROJECT:     Digi Doc
// DESCRIPTION: Digi Doc functions for certificate handling
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
//      09.09.2004      Veiko Sinivee
//                      Creation
//==================================================

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/DigiDocConvert.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocError.h>
#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocMem.h>
#include <libdigidoc/DigiDocOCSP.h>

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
#include <string.h>

//==========< forward declarations >====================

extern int createOCSPRequest(SignedDoc* pSigDoc, OCSP_REQUEST **req,
		      X509 *cert, X509 *pCA, byte* nonce, int nlen);
extern int signOCSPRequestPKCS12(OCSP_REQUEST *req, const char* filename, const char* passwd);

extern int verifyOCSPResponse(OCSP_RESPONSE* pResp, 
				    const X509** caCerts, const char *CApath,
				    const X509* notCert);
extern int hasUmlauts(const char* str);
extern int checkNonceAndCertbyOCSP(OCSP_RESPONSE* resp, X509* cert, byte* nonce1, int nonceLen);


//==========< utility functions >====================


//--------------------------------------------------
// Reads a certificate file
// certfile - name of the certificate file
//--------------------------------------------------
EXP_OPTION int ReadCertificate(X509 **x509, const char *szCertfile)
{
  BIO *in; 

  RETURN_IF_NULL_PARAM(szCertfile);

  if((in = BIO_new_file(szCertfile, "rb")) != NULL) {
    *x509 = PEM_read_bio_X509(in, NULL, NULL, 0);
    BIO_free(in);
    if(!*x509) SET_LAST_ERROR_RETURN_CODE(ERR_NULL_CERT_POINTER);
  } else 
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_READ);
  return ERR_OK;
}

//--------------------------------------------------
// Reads a certificate from pkcs12 conteiner
//--------------------------------------------------
EXP_OPTION int ReadCertificateByPKCS12(X509 **x509, const char *pkcs12file, const char *passwd, EVP_PKEY **pkey)
{
  BIO *bio;
  PKCS12 *p12;
	
  //memset(debugBuf,0,sizeof(debugBuf));
  RETURN_IF_NULL_PARAM(pkcs12file);
  bio=BIO_new(BIO_s_file());
  RETURN_IF_NULL(bio);
  BIO_read_filename(bio, pkcs12file);
  p12 = d2i_PKCS12_bio(bio, NULL);
  BIO_free(bio);
  RETURN_IF_NOT(p12, ERR_OCSP_PKCS12_CONTAINER);
  PKCS12_parse(p12, passwd, pkey, x509, NULL);
  PKCS12_free(p12);
	
  RETURN_IF_NOT(*x509, ERR_OCSP_PKCS12_CONTAINER);
  return ERR_OK;
}

//--------------------------------------------------
// Reads certificates serial number
// szSerial - buffer for serial number
// nMaxLen - maximum serial number buffer length
// certfile - name of the certificate file
//--------------------------------------------------
EXP_OPTION int GetCertSerialNumber(char *szSerial, int nMaxLen, const char *szCertfile)
{
  BIO *in; 
  X509 *x509;

  RETURN_IF_NULL_PARAM(szSerial);
  RETURN_IF_NULL_PARAM(szCertfile);
  if((in = BIO_new_file(szCertfile, "rb")) != NULL) {
    x509 = PEM_read_bio_X509(in, NULL, NULL, 0);
    BIO_free(in);
    RETURN_IF_NOT(x509, ERR_NULL_CERT_POINTER);
    ReadCertSerialNumber(szSerial, nMaxLen, x509);
    X509_free(x509);
  }
  else 
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_READ);
  return ERR_OK;
}

//--------------------------------------------------
// Reads certificates serial number
// x509 - certificate object
//--------------------------------------------------
EXP_OPTION int ReadCertSerialNumber(char* szSerial, int nMaxLen, X509 *x509)
{
  ASN1_INTEGER *bs = NULL;
  BIGNUM* bn;
  char* str;
  int err = ERR_OK;
  
  RETURN_IF_NOT(x509, ERR_NULL_CERT_POINTER);
  RETURN_IF_NOT(szSerial, ERR_NULL_POINTER);
  bs = X509_get_serialNumber(x509);
  RETURN_IF_NOT(bs, ERR_NULL_SER_NUM_POINTER);
  bn = ASN1_INTEGER_to_BN(bs, NULL);
  RETURN_IF_NOT(bn, ERR_NULL_SER_NUM_POINTER);
  str = BN_bn2dec(bn);
  RETURN_IF_NOT(str, ERR_NULL_SER_NUM_POINTER);
  memset(szSerial, 0, nMaxLen);
  strncpy(szSerial, str, nMaxLen -1);
  if(strlen(str) > (unsigned int)nMaxLen) {
    err = ERR_BUF_LEN;
    SET_LAST_ERROR(ERR_BUF_LEN);
  }
	//AM 28.05.08 bn should be freed too
  if(bn)
		BN_free(bn);
  OPENSSL_free(str);
  checkErrors();
  return err;
}



//--------------------------------------------------
// Reads a public key file
// certfile - name of the certificate file
//--------------------------------------------------
EXP_OPTION int ReadPublicKey(EVP_PKEY **PublicKey, const char *szCertfile)
{
  EVP_PKEY *pkey = NULL;
  X509 *x509 = NULL;
  int err = ERR_OK;

  RETURN_IF_NULL_PARAM(szCertfile);
  RETURN_IF_NULL_PARAM(PublicKey);  // SVEN - ver 1.92 - crashbug fixed
  if((err = ReadCertificate(&x509, szCertfile)) == ERR_OK) {
    pkey = X509_extract_key(x509);
    X509_free(x509);
    RETURN_IF_NOT(pkey, ERR_NULL_KEY_POINTER);
    *PublicKey = pkey;
    return ERR_OK;
  } else 
    SET_LAST_ERROR_RETURN_CODE(err); 
}


//--------------------------------------------------
// Reads a public key file
// certfile - name of the certificate file
//--------------------------------------------------
EXP_OPTION int GetPublicKey(EVP_PKEY **pubKey, const X509* x509)
{
  EVP_PKEY *pkey = NULL;
  
  RETURN_IF_NULL_PARAM(x509);	
  // SET_LAST_ERROR_RETURN_IF_NOT(x509, ERR_NULL_CERT_POINTER, pkey);
  // pkey = X509_extract_key((X509*)x509);
  pkey = X509_get_pubkey((X509*)x509);
  RETURN_IF_NOT(pkey, ERR_NULL_KEY_POINTER);
  *pubKey = pkey;
  return ERR_OK; 
}


//--------------------------------------------------
// Callback routine for passing key password
//--------------------------------------------------
int pemkey_callback(char *buf, int size, int rwflag, void *userdata)
{
  RETURN_OBJ_IF_NULL(buf, 0);
  RETURN_OBJ_IF_NULL(userdata, 0);
  memset(buf, 0, size);
  strncpy(buf, (char*)userdata, size);
  return strlen(buf);
}


//--------------------------------------------------
// Reads a private key file
// keyfile - name of the private key file
// passwd - key password (problems with encrypted passwwords!)
// format - file format (PEM or DER)
//--------------------------------------------------
EXP_OPTION int ReadPrivateKey(EVP_PKEY **privKey, const char *keyfile, const char* passwd, int format)
{
  BIO *in; 
  EVP_PKEY *pkey = NULL;

  RETURN_IF_NULL_PARAM(keyfile);
  RETURN_IF_NULL_PARAM(passwd);
  if((in = BIO_new_file(keyfile, "rb")) != NULL) {
    switch(format) {
    case FILE_FORMAT_ASN1:
      pkey = d2i_PrivateKey_bio(in,NULL);
      break;
    case FILE_FORMAT_PEM:
      PEM_read_bio_PrivateKey(in, &pkey, pemkey_callback, (void*)passwd);
      break;
    }
    BIO_free(in);
    RETURN_IF_NOT(pkey, ERR_NULL_KEY_POINTER);
    *privKey = pkey;
  }
  else
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_READ);
  return ERR_OK;
}

//--------------------------------------------------
// Writes a private key file
// keyfile - name of the private key file
// passwd - key password (problems with encrypted passwwords!)
// format - file format (PEM or DER)
//--------------------------------------------------
EXP_OPTION int WritePrivateKey(EVP_PKEY *privKey, const char *keyfile, const char* passwd, int format)
{
  BIO *out = NULL; 
  EVP_PKEY *pkey = privKey;

  RETURN_IF_NULL_PARAM(privKey);
  RETURN_IF_NULL_PARAM(keyfile);
  if((out = BIO_new_file(keyfile, "wb")) != NULL) {
    switch(format) {
    case FILE_FORMAT_ASN1:
	  i2d_PUBKEY_bio(out, pkey);
      break;
    case FILE_FORMAT_PEM:
		PEM_write_bio_PrivateKey(out, pkey, (passwd ? EVP_des_ede3_cbc() : NULL), 
			(char*)passwd, strlen(passwd), pemkey_callback, (void*)passwd) ;
      break;
    }
    BIO_free(out);
  }
  else
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_WRITE);
  return ERR_OK;
}

//--------------------------------------------------
// Writes a private key and cert to a PEM file 
// privKey - private key
// pCert - certificate
// keyfile - name of the private key file
// passwd - key password (problems with encrypted passwwords!)
//--------------------------------------------------
EXP_OPTION int ddocWriteKeyAndCertPem(EVP_PKEY *privKey, X509* pCert, 
								  const char *keyfile, const char* passwd)
{
  BIO *out = NULL; 
  
  RETURN_IF_NULL_PARAM(pCert);
  RETURN_IF_NULL_PARAM(privKey);
  RETURN_IF_NULL_PARAM(keyfile);
  if((out = BIO_new_file(keyfile, "wb")) != NULL) {
	PEM_write_bio_X509(out, pCert);
    PEM_write_bio_PrivateKey(out, privKey, (passwd ? EVP_des_ede3_cbc() : NULL), 
		(char*)passwd, (passwd ? strlen(passwd) : 0), pemkey_callback, (void*)passwd);
    BIO_free(out);
  }
  else
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_WRITE);
  return ERR_OK;
}



//--------------------------------------------------
// Reads an RSA private key file
// keyfile - name of the private key file
// passwd - key password (problems with encrypted passwwords!)
// format - file format (PEM or DER)
//--------------------------------------------------
EXP_OPTION int ReadRSAPrivateKey(RSA **privKey, const char *keyfile, const char* passwd, int format)
{

  BIO *in = 0;
  RSA *pkey = 0;

  RETURN_IF_NULL_PARAM(keyfile);
  RETURN_IF_NULL_PARAM(passwd);
  if((in = BIO_new_file(keyfile, "rb")) != 0) {
    switch(format) {
    case FILE_FORMAT_ASN1:
      pkey = d2i_RSAPrivateKey_bio(in,NULL);
      break;
    case FILE_FORMAT_PEM:
      PEM_read_bio_RSAPrivateKey(in, &pkey, pemkey_callback, (void*)passwd);
      break;
    }
    BIO_free(in);
    RETURN_IF_NOT(pkey, ERR_NULL_KEY_POINTER);
    *privKey = pkey;
  }
  else
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_READ);
  return ERR_OK;
}

//--------------------------------------------------
// Verifys a certificate by sending an OCSP_REQUEST object
// to the notary server and checking the response.
// Uses servers timestamps hash code as nonce value.
// pCert - certificate to test
// caCerts - responder CA certs chain
// notaryCert - notarys cert search
// proxyHost - proxy servers name
// proxyPort - proxy servers port
// notaryURL - notarys URL
// ppResp - address to return OCSP response. Use NULL if
// you don't want OCSP response to be returned
// return 0 for OK, or error code
//--------------------------------------------------
EXP_OPTION int verifyCertificateByOCSP(X509* pCert, const X509** caCerts, 
				       const X509* notaryCert, char* notaryURL, 
				       char* proxyHost, char* proxyPort, 
				       const char* pkcs12file, const char* pkcs12paswd,
				       OCSP_RESPONSE **ppResp)
{
	return verifyCertificateByOCSPWithIp(pCert, caCerts, notaryCert, 
		notaryURL, proxyHost, proxyPort, pkcs12file, pkcs12paswd, ppResp, 0);
}

//--------------------------------------------------
// Verifys a certificate by sending an OCSP_REQUEST object
// to the notary server and checking the response.
// Uses servers timestamps hash code as nonce value.
// pCert - certificate to test
// caCerts - responder CA certs chain
// notaryCert - notarys cert search
// proxyHost - proxy servers name
// proxyPort - proxy servers port
// notaryURL - notarys URL
// ppResp - address to return OCSP response. Use NULL if
// you don't want OCSP response to be returned
// return 0 for OK, or error code
//--------------------------------------------------
EXP_OPTION int verifyCertificateByOCSPWithIp(X509* pCert, const X509** caCerts, 
				       const X509* notaryCert, char* notaryURL, 
				       char* proxyHost, char* proxyPort, 
				       const char* pkcs12file, const char* pkcs12paswd,
				       OCSP_RESPONSE **ppResp, unsigned long ip) 

{
  OCSP_REQUEST *req = 0;
  OCSP_RESPONSE *resp = 0;
  int err = ERR_OK, l1, i, j, nType, nCAs1, err2;
  byte nonce1[DIGEST_LEN+2];
  time_t tNow;
  X509* pCA = NULL;
  X509* caCerts1[10];
  X509* pNotCert1;
  char szCN[200], szCA[30];
  
  RETURN_IF_NULL(pCert);
  //RETURN_IF_NULL(notaryCert);
  //RETURN_IF_NULL(caCerts);
  RETURN_IF_NULL(notaryURL);
  
  // mark as not found yet
  if(ppResp)
    *ppResp = 0;
  time(&tNow);
  l1 = sizeof(nonce1);
  calculateDigest((const byte*)&tNow, sizeof(tNow), DIGEST_SHA1, nonce1, &l1);
  // find lowest CA
  for(i = 0; (caCerts != NULL) && (caCerts[i] != NULL); i++)
    pCA = (X509*)caCerts[i];
  err = createOCSPRequest(NULL, &req, pCert, pCA, nonce1, l1);		
  if(err == ERR_OK && pkcs12file)
    err = signOCSPRequestPKCS12(req, pkcs12file, pkcs12paswd);
  if(err == ERR_OK) {
    //WriteOCSPRequest("test1.req", req);
    err = sendOCSPRequest(&resp, req, notaryURL, proxyHost, proxyPort, ip);
    //WriteOCSPResponse("test1.resp", resp);
    //printf("sendOCSPRequest returned %d\n", err);
    if(err == ERR_OK) {
      if(caCerts && notaryCert) {
        ddocDebug(1, "verifyCertificateByOCSPWithIp", "Verify OCSP resp with known CA certs");
        // check the response signature
        err = verifyOCSPResponse(resp, (const X509**)caCerts, NULL, notaryCert);
      } 
      if(err == ERR_OK)
	err = checkNonceAndCertbyOCSP(resp, pCert, nonce1, l1);
      //WriteOCSPResponse("test1.resp", resp);
    }
  }
  // free OCSP request, if allocated;
  if (req)
    OCSP_REQUEST_free(req);
  // return OCSP response if required
  if(ppResp)
    *ppResp = resp;
  else
    OCSP_RESPONSE_free(resp);
  return err;
}

//============================================================
// Decodes binary (DER) cert data and returns a cert object
// certData - binary (DER) cert data
// certLen - cert data length
//============================================================
EXP_OPTION int ddocDecodeX509Data(X509 **ppX509, const byte* certData, int certLen)
{
  BIO* b1 = NULL;

  // check input params
  RETURN_IF_NULL_PARAM(certData);
  RETURN_IF_NULL_PARAM(ppX509);
  // mark as not read yet
  *ppX509 = 0;
  // create memory BIO on it
  b1 = BIO_new_mem_buf((void*)certData, certLen);
  RETURN_IF_NOT(b1, ERR_CERT_INVALID);
  *ppX509 = d2i_X509_bio(b1, NULL);
  ddocDebug(4, "ddocDecodeX509Data", "Decoding %d bytes DER data - cert %s", certLen, (*ppX509 ? "OK" : "ERROR"));
  // cleanup
  BIO_free(b1);
  RETURN_IF_NOT(*ppX509, ERR_CERT_INVALID);
  return ERR_OK;
}

//============================================================
// Decodes base64 (PEM) cert data and returns a cert object
// certData - base64 (PEM) cert data
// certLen - cert data length
//============================================================
EXP_OPTION int ddocDecodeX509PEMData(X509 **ppX509, const char* certData, int certLen)
{
  byte* p1 = 0;
  int l1 = 0, err = ERR_OK;

  // check input params
  RETURN_IF_NULL_PARAM(certData);
  RETURN_IF_NULL_PARAM(ppX509);
  // mark as not read yet
  *ppX509 = 0;
  // allocate memory for decoding
  l1 = certLen; // should be enough as it shrinks
  p1 = (byte*)malloc(l1);
  RETURN_IF_BAD_ALLOC(p1);
  memset(p1, 0, l1);
  // decode base64 data
  decode((const byte*)certData, certLen, p1, &l1);
  // decode cert
  err = ddocDecodeX509Data(ppX509, p1, l1);
  ddocDebug(4, "ddocDecodeX509PEMData", 
	    "Decoding %d bytes PEM data - cert %s", certLen, (*ppX509 ? "OK" : "ERROR"));
  // cleanup
  if(p1)
    free(p1);
  return err;
}

//============================================================
// Checks if the cert is valid (between begin and end date)
// cert - certificate object
// tDate - date to check the cert
//============================================================
EXP_OPTION int isCertValid(X509* cert, time_t tDate)
{
  int err = ERR_OK;
  ASN1_TIME *tm = 0;
  time_t tStart, tEnd;

  RETURN_IF_NULL_PARAM(cert);
  tm = X509_get_notBefore(cert);
  RETURN_IF_NULL(tm);
  asn1time2time_t_local(tm, &tStart);
  tm = X509_get_notAfter(cert);
  RETURN_IF_NULL(tm);
  asn1time2time_t_local(tm, &tEnd);
  if(tDate < tStart || tDate > tEnd) {
    err = ERR_CERT_INVALID;
    SET_LAST_ERROR(err);
  } 
  return err;
}

//============================================================
// Retrieves the certificates first validity time as tim_t in GMT zone
// pCert - certificate object
// returns certificates first validity time as tim_t in GMT zone
//============================================================
EXP_OPTION time_t getCertNotBeforeTimeT(X509* pCert)
{
  time_t t1 = 0;
  ASN1_TIME *tm = 0;

  if(pCert) {
    tm = X509_get_notBefore(pCert);
    if(tm) 
      asn1time2time_t_local(tm, &t1);
  } 
  return t1;
}

//============================================================
// Retrieves the certificates last validity time as tim_t in GMT zone
// pCert - certificate object
// returns certificates last validity time as tim_t in GMT zone
//============================================================
EXP_OPTION time_t getCertNotAfterTimeT(X509* pCert)
{
  time_t t1 = 0;
  ASN1_TIME *tm = 0;

  if(pCert) {
    tm = X509_get_notAfter(pCert);
    if(tm) 
      asn1time2time_t_local(tm, &t1);
  } 
  return t1;
}

//============================================================
// Checks if the cert has been signed by this CA-cert
// cert - certificate object
// cafile - CA cert file
//============================================================
EXP_OPTION int isCertSignedBy(X509* cert, const char* cafile)
{
  int err = ERR_OK;
  EVP_PKEY* pubkey = NULL; // SVEN - ver 1.92 - crashbug fixed
	
  (void)ReadPublicKey(&pubkey, cafile);
  RETURN_IF_NOT(pubkey, ERR_PUBKEY_READ);
  
  err = X509_verify(cert, pubkey);
  if(err == ERR_LIB_NONE)
    err = ERR_OK;
  else {
    err = ERR_CERT_ISSUER;
    SET_LAST_ERROR(err);
  }
  // checkErrors();
  //332:error:0D0890A1:lib(13):func(137):reason(161):.\crypto\asn1\a_verify.c:141:
  EVP_PKEY_free(pubkey);
  return err;
}

//============================================================
// Writes cert to file without the usual PEM headers
// bout - output file
// cert - certificate object
// returns error code
//============================================================
EXP_OPTION int writeCertToXMLFile(BIO* bout, X509* cert)
{
  int l1, l2;
  char *p1, *p2;

  RETURN_IF_NULL_PARAM(bout);
  RETURN_IF_NULL_PARAM(cert);
  l1 = i2d_X509(cert, NULL);
  p1 = (char*)malloc(l1+10);
  RETURN_IF_BAD_ALLOC(p1);
  p2 = p1;
  i2d_X509(cert, (unsigned char**)&p2);
  l2 = l1 * 2;
  p2 = (char*)malloc(l2);
  if (p2 == NULL) {
    free(p1);
    RETURN_IF_BAD_ALLOC(p2);
  }
  encode((const byte*)p1, l1, (byte*)p2, &l2);
  BIO_puts(bout, p2);
  free(p2);
  free(p1);
  return ERR_OK;
}

//============================================================
// Converts the certificate to PEM form with or without headers
// cert - certificate object
// bHeaders - 1= with headers, 0=no headers
// buf - output buffer newly allocated
// returns error code
//============================================================
EXP_OPTION int getCertPEM(X509* cert, int bHeaders, char** buf)
{
  int l1, l2;
  char *p1, *p2;

  RETURN_IF_NULL_PARAM(buf);
  RETURN_IF_NULL_PARAM(cert);
  l1 = i2d_X509(cert, NULL);
  p1 = (char*)malloc(l1+10);
  RETURN_IF_BAD_ALLOC(p1);
  p2 = p1;
  i2d_X509(cert, (unsigned char**)&p2);
  l2 = l1 * 2 + 200;
  *buf = (char*)malloc(l2);
  if(*buf == NULL) {
    free(p1);
    RETURN_IF_BAD_ALLOC(*buf);
  }
  memset(*buf, 0, l2);
  if(bHeaders)
    strncpy(*buf, "-----BEGIN CERTIFICATE-----\n", l2);
  encode((const byte*)p1, l1, (byte*)strchr(*buf, 0), &l2);
  l2 = l1 * 2 + 200 - strlen(*buf);
  if(bHeaders)
    strncat(*buf, "\n-----END CERTIFICATE-----", l2);
  free(p1);
  return ERR_OK;
}



//--------------------------------------------------
// Returns the certificates validity first date
// cert - certificate data
// timestamp - timestamp buffer
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int getCertNotBefore(const SignedDoc* pSigDoc, X509* cert, char* timestamp, int len)
{
  int err = ERR_OK;
  ASN1_TIME *tm = 0;
  RETURN_IF_NULL_PARAM(cert);
  RETURN_IF_NULL_PARAM(timestamp);
  
  tm = X509_get_notBefore(cert);
  RETURN_IF_NULL(tm);
  err = asn1time2strYear(pSigDoc, tm, timestamp, 1900, len);
  
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}


//--------------------------------------------------
// Returns the certificates validity last date
// cert - certificate data
// timestamp - timestamp buffer
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int getCertNotAfter(const SignedDoc* pSigDoc, X509* cert, char* timestamp, int len)
{
  int err = ERR_OK;
  ASN1_TIME *tm = 0;

  RETURN_IF_NULL_PARAM(cert);
  RETURN_IF_NULL_PARAM(timestamp);
  
  tm = X509_get_notAfter(cert);
  RETURN_IF_NULL(tm);
  err = asn1time2strYear(pSigDoc, tm, timestamp, 1900, len);

  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}


//--------------------------------------------------
// Saves the certificate in a file
// cert - certificate data
// szFileName - destination filename
// nFormat - cert format
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int saveCert(X509* cert, const char* szFileName, int nFormat)
{
  //	int err = ERR_OK;
  BIO* b;
  
  RETURN_IF_NULL_PARAM(cert);
  RETURN_IF_NULL_PARAM(szFileName);
  
  if((b = BIO_new_file(szFileName, "w")) != NULL) {
    if(nFormat == FILE_FORMAT_PEM)
      PEM_write_bio_X509(b, cert);
    if(nFormat == FILE_FORMAT_ASN1)
      i2d_X509_bio(b, cert);
    BIO_free(b);
  } else 
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_WRITE);
  return ERR_OK;
}

// new functions for Win client

EXP_OPTION void* decodeCert(const char* pemData)
{
  BIO* b1;
  X509* x509;
  int l1;

  RETURN_OBJ_IF_NULL(pemData, NULL);
  l1 = strlen(pemData);
  b1 = BIO_new_mem_buf((void*)pemData, l1);
  RETURN_OBJ_IF_NULL(b1, 0);
  x509 = PEM_read_bio_X509(b1, NULL, NULL, 0);
  if(!x509)
    SET_LAST_ERROR(ERR_CERT_READ);
  //		checkErrors();
  BIO_free(b1);
  return x509;
}




//===========================================================
// Encodes certificate from X509 to binary
//===========================================================
EXP_OPTION void encodeCert(const X509* x509, char * encodedCert, int* encodedCertLen)
{
  if(x509==NULL){
    *encodedCertLen = 0;
    return;
  }
  if(encodedCert==NULL){
    *encodedCertLen = 0;
    return;
  }
  *encodedCertLen = i2d_X509((X509*)x509, NULL);
  i2d_X509((X509*)x509, (unsigned char**)&encodedCert);
}

//============================================================
// Checks if the cert has been signed by this CA-cert
// cert - certificate object
// cafile - CA cert file
//============================================================
EXP_OPTION int isCertSignedByCERT(const X509* cert, const X509* caCert)
{
  int err = ERR_OK;
  EVP_PKEY* pubkey;
  DigiDocMemBuf mbuf1, mbuf2, mbuf3;

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  mbuf2.pMem = 0;
  mbuf2.nLen = 0;
  mbuf3.pMem = 0;
  mbuf3.nLen = 0;
  RETURN_IF_NULL_PARAM(cert);
  RETURN_IF_NULL_PARAM(caCert);
  err = ddocCertGetSubjectCN((X509*)caCert, &mbuf1);
  err = ddocCertGetSubjectCN((X509*)cert, &mbuf2);
  err = ddocCertGetIssuerCN((X509*)cert, &mbuf3);
  ddocDebug(4, "isCertSignedByCERT", 
	    "Issuer: %s, Subject: %s, Subjects-issuer: %s", 
	    (const char*)mbuf1.pMem, (const char*)mbuf2.pMem,
	    (const char*)mbuf3.pMem);
  ddocMemBuf_free(&mbuf1);
  ddocMemBuf_free(&mbuf2);
  ddocMemBuf_free(&mbuf3);
  err = GetPublicKey(&pubkey, caCert);
  if(err == ERR_OK) {
    err = X509_verify((X509*)cert, pubkey);
    ddocDebug(4, "isCertSignedByCERT", 
	      "verify: %d", err);
    if(err == ERR_LIB_NONE)
      err = ERR_OK;
    else
      err = ERR_CERT_ISSUER;
    checkErrors();
    EVP_PKEY_free(pubkey);
  }
  else 
    err = ERR_PUBKEY_READ;
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}


/*int utc2latin1(const char* data, int len, char* dest)
{
  int i, j;
  
  for(i = j = 0; i < len; i++) {
    if(data[i]) {
      dest[j] = data[i];
      j++;
    }
  }
  dest[j] = 0;
  return j;
  }*/

//--------------------------------------------------------
// Finds and copies a substring from source string that is
// prefixed by szLabel and terminated by szTerminator
// szSrc - source string
// szLabel - prefix of searched string
// szTerminator - terminator string
// szDest - found string. Caller must free this mem.
//--------------------------------------------------------
EXP_OPTION int ddocCertCopySubstring(const char* szSrc, const char* szLabel, const char* szTerminator, char** szDest)
{
	int l;
	char *p1, *p2;

	RETURN_IF_NULL_PARAM(szSrc);
	RETURN_IF_NULL_PARAM(szLabel);
	RETURN_IF_NULL_PARAM(szTerminator);
	RETURN_IF_NULL_PARAM(szDest);
	*szDest = 0; // mark as empty
	p1 = strstr(szSrc, szLabel);
	if(p1) {
		p1 += strlen(szLabel);
		p2 = strstr(p1, szTerminator);
		if(!p2)
			p2 = strchr(p1, 0);
		if(p2 && p2 > p1) {
			l = (int)p2 - (int)p1 + 1;
			*szDest = (char*)malloc(l);
			if(*szDest) {
				memset(*szDest, 0, l);
				strncpy(*szDest, p1, (int)p2 - (int)p1);
			}
		}
	}
	return ERR_OK;
}

EXP_OPTION int readCertPoliciesFromOU(X509* pX509, PolicyIdentifier** pPolicies, int* nPols)
{
	DigiDocMemBuf mbuf1;
	int err = ERR_OK;
	char *pUri, *pDesc, *pOid;
	PolicyIdentifier *pTmp;

    RETURN_IF_NULL_PARAM(pX509);
    RETURN_IF_NULL_PARAM(pPolicies);
    RETURN_IF_NULL_PARAM(nPols);
	mbuf1.nLen = 0;
	mbuf1.pMem = 0;
	pUri = pDesc = pOid = 0;
	err = ddocCertGetSubjectOrganizationUnit(pX509, &mbuf1);
	if(!err && mbuf1.pMem) {
	  err = ddocCertCopySubstring((const char*)mbuf1.pMem, "SP Desc:", "\n", &pDesc);
	  if(!err) {
	    err = ddocCertCopySubstring((const char*)mbuf1.pMem, "SP URI:", "\n", &pUri);
		if(!err) {
		  err = ddocCertCopySubstring((const char*)mbuf1.pMem, "SP OID:", "\n", &pOid);
		  if(!err && (pDesc || pUri || pOid)) {
			pTmp = (PolicyIdentifier*)realloc(*pPolicies, sizeof(PolicyIdentifier) * ((*nPols) + 1));
			if(pTmp) {
				*pPolicies = pTmp;
				pTmp = (*pPolicies) + (*nPols);
				*nPols  = (*nPols) + 1;
				pTmp->szOID = (pOid ? strdup((const char*)pOid) : 0);
				pTmp->szUserNotice = (pDesc ? strdup((const char*)pDesc) : 0);
				pTmp->szCPS = (pUri ? strdup((const char*)pUri) : 0);
			}
		  }
		  free(pOid);
		}
		free(pUri);
	  }
	  free(pDesc);
	}
	ddocMemBuf_free(&mbuf1);
	return err;
}

//--------------------------------------------------
// Reads certificates PolicyIdentifiers and returns 
// them in a newly allocated structure. Caller must 
// free this, regardless of function return status.
// pX509 - certificate
// pPolicies - returned PolicyIdentifiers array
// nPols - number of returned policies
//--------------------------------------------------
EXP_OPTION int readCertPolicies(X509* pX509, PolicyIdentifier** pPolicies, int* nPols)
{
  char buf[512], *p;
  int err = 0, i, k, j, pos, l;
  PolicyIdentifier *pPol, *pTmp;
  
  RETURN_IF_NULL_PARAM(pX509);
  RETURN_IF_NULL_PARAM(pPolicies);
  RETURN_IF_NULL_PARAM(nPols);
  *pPolicies = NULL;
  *nPols = 0;
  // try read from certificates OU first
  if(!err)
    err = readCertPoliciesFromOU(pX509, pPolicies, nPols);
  // read from usual place
  pos = X509_get_ext_by_NID(pX509, NID_certificate_policies, -1);
  if(pos >= 0) {
    POLICYINFO *pol;
    STACK_OF(POLICYINFO)* pPols;
    STACK_OF(POLICYQUALINFO)* pQuals;
    POLICYQUALINFO* pQual;
    X509_EXTENSION* pExt = X509_get_ext(pX509, pos);
    // X509V3_EXT_METHOD *method = X509V3_EXT_get(pExt);
    pPols = (STACK*)X509V3_EXT_d2i(pExt);
    for(i = 0; i < sk_POLICYINFO_num(pPols); i++) {
      pol = sk_POLICYINFO_value(pPols, i);
      if(*pPolicies && *nPols) {
	pTmp = (PolicyIdentifier*)realloc(*pPolicies, sizeof(PolicyIdentifier) * ((*nPols) + 1));
	if (pTmp != NULL) {
	  *pPolicies = pTmp;
	  *nPols = (*nPols) + 1;
	  pPol = (*pPolicies) + ((*nPols) - 1);
	} else
	  SET_LAST_ERROR_RETURN_CODE(ERR_BAD_ALLOC);
      } else {
	pTmp = (PolicyIdentifier*)malloc(sizeof(PolicyIdentifier));
	if (pTmp != NULL) {
	  *pPolicies = pTmp;
	  pPol = *pPolicies;
	  *nPols = 1;
	} else
	  SET_LAST_ERROR_RETURN_CODE(ERR_BAD_ALLOC);
      }
      pPol->szCPS = pPol->szOID = pPol->szUserNotice = NULL;
      // check
      i2t_ASN1_OBJECT(buf, sizeof(buf), pol->policyid);
      pPol->szOID = strdup(buf);
      pQuals = pol->qualifiers;
      for(k = 0; k < sk_POLICYQUALINFO_num(pQuals); k++) {
	pQual = sk_POLICYQUALINFO_value(pQuals, k);
	switch(OBJ_obj2nid(pQual->pqualid)) {
	  case NID_id_qt_cps:
	    pPol->szCPS = strdup((const char*)pQual->d.cpsuri->data);
	    // if strdup fails, szCPS will be NULL
	    break;
	  case NID_id_qt_unotice:
	    ddocDebug(4, "readCertPolicies", "Exptext: %d - \'%s\'",
		      pQual->d.usernotice->exptext->length, pQual->d.usernotice->exptext->data);
	    if(pQual->d.usernotice->exptext) {
	      if(pQual->d.usernotice->exptext->type == V_ASN1_UTF8STRING) {
		pPol->szUserNotice = strdup((const char*)pQual->d.usernotice->exptext->data);
	      } else {
		p = 0;
		l = ASN1_STRING_to_UTF8((unsigned char**)&p, pQual->d.usernotice->exptext);
		pPol->szUserNotice = strdup(p);
		OPENSSL_free(p);
	      }
	      /*if(pQual->d.usernotice->exptext->data[0]) {
		pPol->szUserNotice = strdup((const char*)pQual->d.usernotice->exptext->data);
	      } else {
		utc2latin1((const char*)pQual->d.usernotice->exptext->data,
			   pQual->d.usernotice->exptext->length, buf);
		pPol->szUserNotice = strdup(buf);
		// if strdup fails, szUserNotice will be NULL
		}*/
	    }
	    if(pQual->d.usernotice->noticeref) {
	      NOTICEREF *ref = pQual->d.usernotice->noticeref;
	      p = 0; 
	      l = 0;
	      // calculate the memory required
	      l += strlen((const char*)ref->organization->data) + 1;
	      for(j = 0; j < sk_ASN1_INTEGER_num(ref->noticenos); j++) {
		ASN1_INTEGER *num;
		char *tmp;
		num = sk_ASN1_INTEGER_value(ref->noticenos, j);
		tmp = i2s_ASN1_INTEGER(NULL, num);
		l += strlen(tmp) + 2;
		OPENSSL_free(tmp);
	      }
	      // now alloc mem and copy data
	      l += 10; // alloc with a little extra
	      p = (char*)malloc(l); 
	      if(p) {
		memset(p, 0, l);
		strncpy(p, (const char*)ref->organization->data, l);
		strncat(p, " ", l - strlen(p));
		for(j = 0; j < sk_ASN1_INTEGER_num(ref->noticenos); j++) {
		  ASN1_INTEGER *num;
		  char *tmp;
		  num = sk_ASN1_INTEGER_value(ref->noticenos, j);
		  if (j) strncat(p, ", ", l - strlen(p));
		  tmp = i2s_ASN1_INTEGER(NULL, num);
		  strncat(buf, tmp, sizeof(buf) - strlen(buf));
		  OPENSSL_free(tmp);
		}
		pPol->szUserNotice = p;
	      }
	    }
	    break;
	default:
	  break;
	}
      }
    }
  }
  
  return err;
}



//--------------------------------------------------
// Frees policy identifiers array
// pPolicies - PolicyIdentifiers array
// nPols - number of policies
//--------------------------------------------------
EXP_OPTION void PolicyIdentifiers_free(PolicyIdentifier* pPolicies, int nPols)
{
  int i;
  for(i = 0; i < nPols; i++) {
    free(pPolicies[i].szOID);
    free(pPolicies[i].szCPS);
    free(pPolicies[i].szUserNotice);
    //free(pPolicies[i]);
  }
  free(pPolicies);
}


//--------------------------------------------------
// Checks if this is a company CPS policy
// pPolicy - PolicyIdentifier to be checked
//--------------------------------------------------
EXP_OPTION int isCompanyCPSPolicy(PolicyIdentifier* pPolicy)
{
  //return (strstr(pPolicy->szCPS, "1.3.6.4.1.10015.7") != NULL);
  if(pPolicy && pPolicy->szCPS)
    return (strstr(pPolicy->szCPS, "1.3.6.1.4.1.10015.5") != NULL);
  else
    return 0;
}


//--------------------------------------------------
// Returns the certificates sha1 hash. 
// pCert - certificate data
// pMemBuf - memory buffer object for storing DN
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertGetDigest(X509* pCert, DigiDocMemBuf* pMemBuf)
{
  int err = ERR_OK;
  unsigned int l1;

  RETURN_IF_NULL_PARAM(pCert);
  RETURN_IF_NULL_PARAM(pMemBuf);  
  pMemBuf->pMem = 0;
  pMemBuf->nLen = 0;
  l1 = 30;
  err = ddocMemSetLength(pMemBuf, l1);
  X509_digest(pCert, EVP_sha1(), (unsigned char*)pMemBuf->pMem, &l1);
  pMemBuf->nLen = l1;
  
  return err;
}

//--------------------------------------------------
// Returns the certificates public key sha1 hash. 
// pCert - certificate data
// pMemBuf - memory buffer object for storing DN
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertGetPubkeyDigest(X509* pCert, DigiDocMemBuf* pMemBuf)
{
  int err = ERR_OK;
  unsigned int l1;

  RETURN_IF_NULL_PARAM(pCert);
  RETURN_IF_NULL_PARAM(pMemBuf);  
  pMemBuf->pMem = 0;
  pMemBuf->nLen = 0;
  l1 = 30;
  err = ddocMemSetLength(pMemBuf, l1);
  X509_pubkey_digest(pCert, EVP_sha1(), (unsigned char*)pMemBuf->pMem, &l1);
  pMemBuf->nLen = l1;
  
  return err;
}


//--------------------------------------------------
// Returns the certificates DN. Internal function.
// Do not call directly, subject to change
// pCert - certificate data
// pMemBuf - memory buffer object for storing DN
// bIssuer - 1=issuer, 0=subject
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertGetDN(X509* pCert, DigiDocMemBuf* pMemBuf, int bIssuer)
{
  int err = ERR_OK;
  X509_NAME *pName = 0;
  X509_NAME_ENTRY *pNe = 0;
  int i, n, l, t, b = 0;
  const char *s;
  unsigned char* p;
	
  RETURN_IF_NULL_PARAM(pCert);
  RETURN_IF_NULL_PARAM(pMemBuf);
  
  // initialize
  if(pMemBuf->pMem)
    err = ddocMemBuf_free(pMemBuf);
  pMemBuf->pMem = 0;
  pMemBuf->nLen = 0;

  // iterate over name entries
  if(bIssuer)
    pName = X509_get_issuer_name(pCert);
  else
    pName = X509_get_subject_name(pCert);
  for(i = 0; (err == ERR_OK) && (i < sk_X509_NAME_ENTRY_num(pName->entries)); i++) {
    pNe = sk_X509_NAME_ENTRY_value(pName->entries, i);
    n = OBJ_obj2nid(pNe->object);
    s = OBJ_nid2sn(n);
    t = pNe->value->type;
    // mostly we find here:
    // V_ASN1_PRINTABLESTRING, V_ASN1_TELETEXSTRING or V_ASN1_BMPSTRING
    // that we convert to UTF, but V_ASN1_UTF8STRING allready is in UTF8
    // handle only the enry types we know
    if(n != NID_undef && s != NULL) {
      // convert to UTF8 only
      p = 0;
      if(t == V_ASN1_UTF8STRING) {
	p = pNe->value->data;
	l = pNe->value->length;
      } else
	l = ASN1_STRING_to_UTF8(&p, pNe->value);
      ddocDebug(5, "ddocCertGetDN", 
		"NameEntry nid: %d type: %d len: %d item: %s value: \'%s\'", 
		n, t, l, s, (p ? (const char*)p : "NULL"));
      // append separator if necessary
      if(b) 
	err = ddocMemAppendData(pMemBuf, "/", -1);
      else 
	b = 1;
      // print the entry
      err = ddocMemAppendData(pMemBuf, s, -1);
      err = ddocMemAppendData(pMemBuf, "=", -1);
      err = ddocMemAppendData(pMemBuf, (const char*)p, l);
      // cleanup
      if(p && t != V_ASN1_UTF8STRING)
	OPENSSL_free(p);
    }
    ddocDebug(5, "ddocCertGetDN", "Subject: \'%s\' len: %d", 
	      (const char*)pMemBuf->pMem, pMemBuf->nLen);
  } // for

  return err;
}

//--------------------------------------------------
// Returns the certificates issuer name.
// pCert - certificate data
// pMemBuf - memory buffer object for storing DN
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertGetIssuerDN(X509* pCert, DigiDocMemBuf* pMemBuf)
{
  return ddocCertGetDN(pCert, pMemBuf, 1);
}


//--------------------------------------------------
// Returns the certificates subject name.
// pCert - certificate data
// pMemBuf - memory buffer object for storing DN
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertGetSubjectDN(X509* pCert, DigiDocMemBuf* pMemBuf)
{
  return ddocCertGetDN(pCert, pMemBuf, 0);
}

//--------------------------------------------------
// Returns the certificates DN.
// Do not call directly, subject to change
// pCert - certificate data
// pMemBuf - memory buffer object for storing DN
// bIssuer - 1=issuer, 0=subject
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertGetDNFromName(X509_NAME* pName, DigiDocMemBuf* pMemBuf)
{
  int err = ERR_OK;
  X509_NAME_ENTRY *pNe = 0;
  int i, n, l, t, b = 0;
  const char *s;
  unsigned char* p;
	
  RETURN_IF_NULL_PARAM(pMemBuf);
  
  // initialize
  if(pMemBuf->pMem)
    err = ddocMemBuf_free(pMemBuf);
  pMemBuf->pMem = 0;
  pMemBuf->nLen = 0;

  for(i = 0; (err == ERR_OK) && (i < sk_X509_NAME_ENTRY_num(pName->entries)); i++) {
    pNe = sk_X509_NAME_ENTRY_value(pName->entries, i);
    n = OBJ_obj2nid(pNe->object);
    s = OBJ_nid2sn(n);
    t = pNe->value->type;
    // mostly we find here:
    // V_ASN1_PRINTABLESTRING, V_ASN1_TELETEXSTRING or V_ASN1_BMPSTRING
    // that we convert to UTF, but V_ASN1_UTF8STRING allready is in UTF8
    // handle only the enry types we know
    if(n != NID_undef && s != NULL) {
      // convert to UTF8 only
      p = 0;
      if(t == V_ASN1_UTF8STRING) {
	p = pNe->value->data;
	l = pNe->value->length;
      } else
	l = ASN1_STRING_to_UTF8(&p, pNe->value);
      ddocDebug(5, "ddocCertGetDN", 
		"NameEntry nid: %d type: %d len: %d item: %s value: \'%s\'", 
		n, t, l, s, (p ? (const char*)p : "NULL"));
      // append separator if necessary
      if(b) 
	err = ddocMemAppendData(pMemBuf, "/", -1);
      else 
	b = 1;
      // print the entry
      err = ddocMemAppendData(pMemBuf, s, -1);
      err = ddocMemAppendData(pMemBuf, "=", -1);
      err = ddocMemAppendData(pMemBuf, (const char*)p, l);
      // cleanup
      if(p && t != V_ASN1_UTF8STRING)
	OPENSSL_free(p);
    }
    ddocDebug(5, "ddocCertGetDN", "Subject: \'%s\' len: %d", 
	      (const char*)pMemBuf->pMem, pMemBuf->nLen);
  } // for

  return err;
}


//--------------------------------------------------
// Returns the certificates subject name and returns 
// the desired item from it.
// pCert - certificate data
// pMemBuf - memory buffer object for storing result
// nNid - cert name part NID
// bIssuer - 1=from issuer DN, 0=from subject DN
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertGetDNPart(X509* pCert, DigiDocMemBuf* pMemBuf, int nNid, int bIssuer)
{
  int err = ERR_OK;
  X509_NAME *pName = 0;
  X509_NAME_ENTRY *pNe = 0;
  int i, n, l, t;
  const char *s;
  unsigned char* p;
	
  RETURN_IF_NULL_PARAM(pCert);
  RETURN_IF_NULL_PARAM(pMemBuf);
  
  // initialize
  if(pMemBuf->pMem)
    err = ddocMemBuf_free(pMemBuf);
  pMemBuf->pMem = 0;
  pMemBuf->nLen = 0;

  // iterate over name entries
  if(bIssuer)
    pName = X509_get_issuer_name(pCert);
  else
    pName = X509_get_subject_name(pCert);
  for(i = 0; (err == ERR_OK) && (i < sk_X509_NAME_ENTRY_num(pName->entries)); i++) {
    pNe = sk_X509_NAME_ENTRY_value(pName->entries, i);
    n = OBJ_obj2nid(pNe->object);
    s = OBJ_nid2sn(n);
    t = pNe->value->type;
    // mostly we find here:
    // V_ASN1_PRINTABLESTRING, V_ASN1_TELETEXSTRING or V_ASN1_BMPSTRING
    // that we convert to UTF, but V_ASN1_UTF8STRING allready is in UTF8
    // handle only the enry types we know
    if(n == nNid && s != NULL) {
      // convert to UTF8 only
      p = 0;
      if(t == V_ASN1_UTF8STRING) {
	p = pNe->value->data;
	l = pNe->value->length;
      } else
	l = ASN1_STRING_to_UTF8(&p, pNe->value);
      ddocDebug(5, "ddocCertGetDNPart", 
		"NameEntry type: %d len: %d item: %s value: \'%s\'", 
		t, l, s, (p ? (const char*)p : "NULL"));
	  if(pMemBuf->pMem && strlen(pMemBuf->pMem))
		ddocMemAppendData(pMemBuf, "\n", -1);
      err = ddocMemAppendData(pMemBuf, (const char*)p, l);
      // cleanup
      if(p && t != V_ASN1_UTF8STRING)
	OPENSSL_free(p);
		// continue search
    }
  } // for

  return err;
}

//--------------------------------------------------
// Returns the certificates subject CN
// pCert - certificate data
// pMemBuf - memory buffer object for storing result
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertGetSubjectCN(X509* pCert, DigiDocMemBuf* pMemBuf)
{
  return ddocCertGetDNPart(pCert, pMemBuf, NID_commonName, 0);
}


//--------------------------------------------------
// Returns the certificates issuer CN
// pCert - certificate data
// pMemBuf - memory buffer object for storing result
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertGetIssuerCN(X509* pCert, DigiDocMemBuf* pMemBuf)
{
  return ddocCertGetDNPart(pCert, pMemBuf, NID_commonName, 1);
}

#define NID_firstName    99
#define NID_lastName     100
#define NID_perCode      105
#define NID_countryName  14
#define NID_serialNumber 105
#define NID_organization 17
#define NID_organizationUnit 18
#define NID_commonName   13
#define NID_emailAddress 48

//--------------------------------------------------
// Returns the certificates subject first name
// pCert - certificate data
// pMemBuf - memory buffer object for storing result
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertGetSubjectFirstName(X509* pCert, DigiDocMemBuf* pMemBuf)
{
  return ddocCertGetDNPart(pCert, pMemBuf, NID_firstName, 0);
}


//--------------------------------------------------
// Returns the certificates subject last name
// pCert - certificate data
// pMemBuf - memory buffer object for storing result
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertGetSubjectLastName(X509* pCert, DigiDocMemBuf* pMemBuf)
{
  return ddocCertGetDNPart(pCert, pMemBuf, NID_lastName, 0);
}


//--------------------------------------------------
// Returns the certificates subject personal code
// pCert - certificate data
// pMemBuf - memory buffer object for storing result
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertGetSubjectPerCode(X509* pCert, DigiDocMemBuf* pMemBuf)
{
  return ddocCertGetDNPart(pCert, pMemBuf, NID_perCode, 0);
}

//--------------------------------------------------
// Returns the certificates subject country code
// pCert - certificate data
// pMemBuf - memory buffer object for storing result
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertGetSubjectCountryName(X509* pCert, DigiDocMemBuf* pMemBuf)
{
  return ddocCertGetDNPart(pCert, pMemBuf, NID_countryName, 0);
}

//--------------------------------------------------
// Returns the certificates subject organization
// pCert - certificate data
// pMemBuf - memory buffer object for storing result
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertGetSubjectOrganization(X509* pCert, DigiDocMemBuf* pMemBuf)
{
  return ddocCertGetDNPart(pCert, pMemBuf, NID_organization, 0);
}

//--------------------------------------------------
// Returns the certificates subject organization unit
// pCert - certificate data
// pMemBuf - memory buffer object for storing result
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertGetSubjectOrganizationUnit(X509* pCert, DigiDocMemBuf* pMemBuf)
{
  return ddocCertGetDNPart(pCert, pMemBuf, NID_organizationUnit, 0);
}

//================< deprecated functions> =================================

#ifdef WITH_DEPRECATED_FUNCTIONS

//============================================================
// Decodes base64 (PEM) cert data and returns a cert object
// certData - base64 (PEM) cert data
// certLen - cert data length
//============================================================
// FIXME : Ugly as hell...
#pragma message ("Function decodeCertificateData may be removed from future releases. Please use function ddocDecodeX509PEMData() instead.")
EXP_OPTION int decodeCertificateData(X509 **newX509, const byte* certData, int certLen)
{
        int l1, l2;
        char *buf1;
        BIO* b1 = NULL;
        X509* x509 = NULL;

        RETURN_IF_NULL_PARAM(certData);

        l2 = certLen * 2;
        buf1 = (char*)malloc(l2);
        RETURN_IF_BAD_ALLOC(buf1);
        memset(buf1, 0, l2);
        strncpy(buf1, "-----BEGIN CERTIFICATE-----\r\n", l2);
        l1 = l2 - strlen(buf1);
        encode(certData, certLen, (byte*)strchr(buf1,0), &l1);
	l1 = l2 - strlen(buf1);
        strncat(buf1, "\n-----END CERTIFICATE-----", l1);
        l1 = strlen(buf1);
        b1 = BIO_new_mem_buf(buf1, l1);
        if (!b1) {
                free(buf1);
                SET_LAST_ERROR_RETURN_CODE(ERR_NULL_POINTER);
        }
        x509 = PEM_read_bio_X509(b1, NULL, NULL, 0);
        BIO_free(b1);
        free(buf1);
        RETURN_IF_NOT(x509, ERR_CERT_INVALID);

        *newX509 = x509;
        return ERR_OK;
}

//--------------------------------------------------
// Returns the certificates issuer name
// cert - certificate data
// buf - usser name buffer
// buflen - pointer to buffer length (will be modified)
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int getCertIssuerName(X509* cert, char* buf, int* buflen)
{
  int l1;
  char buf1[X509_NAME_BUF_LEN];

  RETURN_IF_NULL_PARAM(cert);
  RETURN_IF_NULL_PARAM(buf);
  RETURN_IF_NULL_PARAM(buflen);

  if(*buflen >= X509_NAME_LEN) {
    l1 = sizeof(buf1);
    memset(buf1,0,l1);
    memset(buf, 0, *buflen);
    X509_NAME_oneline(X509_get_issuer_name(cert), buf1, l1);
    unicode2ascii(buf1, buf);
    ddocDebug(5, "getCertIssuerName", "Issuer: \'%s\' -> \'%s\'", buf1, buf);
    *buflen = strlen(buf);
  } else
    SET_LAST_ERROR_RETURN_CODE(ERR_BUF_LEN);	
  return ERR_OK;
}

//--------------------------------------------------
// Returns the certificates subject name. This finction
// is also used in the COM library
// cert - certificate data
// buf - user name buffer
// buflen - pointer to buffer length (will be modified)
// returns error code or ERR_OK
//--------------------------------------------------
#pragma message ("Function getCertSubjectName may be removed from future releases. Please use function ddocCertGetSubjectDN() instead.")
EXP_OPTION int getCertSubjectName(X509* cert, char* buf, int* buflen)
{
  int err = ERR_OK;
  DigiDocMemBuf mbuf;

  mbuf.pMem = 0;
  mbuf.nLen = 0;
  err = ddocCertGetSubjectDN(cert, &mbuf);
  if(err == ERR_OK) {
    if(*buflen >= X509_NAME_LEN) {
#ifdef WIN32
      utf82oem((const char*)mbuf.pMem, buf, buflen);
#else
      utf82ascii((const char*)mbuf.pMem, buf, buflen);
#endif
      *buflen = strlen(buf);
    } else
      SET_LAST_ERROR(ERR_BUF_LEN);
    ddocMemBuf_free(&mbuf);
  }

  return err;
}

//--------------------------------------------------
// Returns the certificates subjects DN
// cert - certificate data
// buf - usser name buffer
// buflen - pointer to buffer length (will be modified)
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int getCertSubjectDN(X509* cert, char* buf, int* buflen, int bUTF8)
{
  int  l1;
  char buf1[X509_NAME_BUF_LEN];

  RETURN_IF_NULL_PARAM(cert);
  RETURN_IF_NULL_PARAM(buf);
  RETURN_IF_NULL_PARAM(buflen);
  if(*buflen >= X509_NAME_LEN) {
    l1 = sizeof(buf1);
    memset(buf1, 0, l1);
    memset(buf, 0, *buflen);
    X509_NAME_oneline(X509_get_subject_name(cert), buf1, l1);
    if (bUTF8) {
      unicode2ascii(buf1, buf);
      *buflen = strlen(buf);
    } else {
      ascii2utf8(buf1, buf, buflen);
    }
  }
  else
    SET_LAST_ERROR_RETURN_CODE(ERR_BUF_LEN);	
  return ERR_OK;
}


//--------------------------------------------------
// Returns the certificates subjects name's CN part
// cert - certificate data
// buf - usser name buffer
// buflen - pointer to buffer length (will be modified)
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int getCertSubjectCN(X509* cert, char* buf, int* buflen, int bUTF8)
{
  int  l1, err = ERR_OK;
  char buf1[X509_NAME_BUF_LEN], *p , *p2, buf2[X509_NAME_BUF_LEN];

  RETURN_IF_NULL_PARAM(cert);
  RETURN_IF_NULL_PARAM(buf);
  RETURN_IF_NULL_PARAM(buflen);

  if(*buflen >= X509_NAME_LEN) {
    l1 = sizeof(buf1);
    memset(buf1,0,l1);
    memset(buf, 0, *buflen);
    err = getCertSubjectName(cert, buf1, &l1);
    if(bUTF8 && hasUmlauts(buf1)) {
      //printf("Converting umlauts toi UTF8\n");
      l1 = sizeof(buf2);
      ascii2utf8(buf1, buf2, &l1);
      strncpy(buf1, buf2, sizeof(buf1));
      ddocDebug(4, "getCertSubjectCN", "Subject: \'%s\'", buf1);
    }    
    /*if (!bUTF8) {
      l1 = sizeof(buf2);
      utf82ascii(buf1, buf2, &l1);
      //strcpy(buf1, buf2);
      }*/
    //printf("SUBJECT: %s\n", buf1);
    p = strstr(buf1, "CN=");
    if(p) {
      p += 3;
      p2 = strchr(p, '/');
      if(!p2)
	p2 = p + strlen(p);
      if(p2) {
	strncpy(buf, p, p2-p);
	buf[p2-p] = 0;
      }
    }
    *buflen = strlen(buf);
  }
  else
    SET_LAST_ERROR_RETURN_CODE(ERR_BUF_LEN);	
  return ERR_OK;
}

//--------------------------------------------------
// Returns the certificates issuer name's CN part
// cert - certificate data
// buf - usser name buffer
// buflen - pointer to buffer length (will be modified)
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int getCertIssuerCN(X509* cert, char* buf, int* buflen, int bUTF8)
{
  int  l1;
  char buf1[X509_NAME_BUF_LEN], *p , *p2, buf2[X509_NAME_BUF_LEN];

  RETURN_IF_NULL_PARAM(cert);
  RETURN_IF_NULL_PARAM(buf);
  RETURN_IF_NULL_PARAM(buflen);
  
  if(*buflen >= X509_NAME_LEN) {
    l1 = sizeof(buf1);
    memset(buf1,0,l1);
    memset(buf, 0, *buflen);
    X509_NAME_oneline(X509_get_issuer_name(cert), buf1, l1);
    if (bUTF8) {
      unicode2ascii(buf1, buf2);
    } else {
      l1 = sizeof(buf2);
      ascii2utf8(buf1, buf2, &l1);
    }
    //printf("SUBJECT: %s\n", buf2);
    p = strstr(buf2, "CN=");
    if(p) {
      p += 3;
      p2 = strchr(p, '/');
      if(!p2)
	p2 = p + strlen(p);
      if(p2) {
	strncpy(buf, p, p2-p);
	buf[p2-p] = 0;
      }
    }
    *buflen = strlen(buf);
  }
  else
    SET_LAST_ERROR_RETURN_CODE(ERR_BUF_LEN);	
  return ERR_OK;
}

// get cert owners id-code
EXP_OPTION int getCertOwnerCode(const X509* pCert, char* buf, int len)
{
  int err, l1;
  char buf1[500], *p;
  
  RETURN_IF_NULL_PARAM(pCert);
  RETURN_IF_NULL_PARAM(buf);
  l1 = sizeof(buf1);
  err = getCertSubjectName((void*)pCert, buf1, &l1);
  if (err != ERR_OK) SET_LAST_ERROR_RETURN_CODE(err);
  err = ERR_CERT_READ; 
  p = strstr(buf1, "CN=");
  if (p) {
    p = strchr(p, ',');
    if(p) {
      p = strchr(p+1, ',');
      if(p) {
	strncpy(buf, p+1, 1en);
	buf[1en] = 0;
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


#endif // WITH_DEPRECATED_FUNCTIONS
