#ifndef __DIGI_DOC_CERT_H__
#define __DIGI_DOC_CERT_H__
//==================================================
// FILE:	DigiDocCert.h
// PROJECT: Digi Doc
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
// Lesser General Public License for more details.ode
// GNU Lesser General Public Licence is available at
// http://www.gnu.org/copyleft/lesser.html
//==========< HISTORY >=============================
//==================================================


#ifdef  __cplusplus
extern "C" {
#endif

#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocLib.h>

// structure for reading certificate policies
typedef struct PolicyIdentifier_st {
	char* szOID;						// stringified OID
	char* szCPS;						// CPS URL
	char* szUserNotice;					// user notice
} PolicyIdentifier;


EXP_OPTION int ReadPrivateKey(EVP_PKEY **privKey, const char *keyfile, const char* passwd, int format);
EXP_OPTION int WritePrivateKey(EVP_PKEY *privKey, const char *keyfile, const char* passwd, int format);
EXP_OPTION int ReadPublicKey(EVP_PKEY **pkey, const char *certfile);
EXP_OPTION int GetPublicKey(EVP_PKEY **pubKey, const X509* x509);

//--------------------------------------------------
// Writes a private key and cert to a PEM file 
// privKey - private key
// pCert - certificate
// keyfile - name of the private key file
// passwd - key password (problems with encrypted passwwords!)
//--------------------------------------------------
EXP_OPTION int ddocWriteKeyAndCertPem(EVP_PKEY *privKey, X509* pCert, 
								  const char *keyfile, const char* passwd);


EXP_OPTION int  ReadCertificate(X509 **x509, const char *certfile);
EXP_OPTION int  ReadCertSerialNumber(char* szSerial, int nMaxLen, X509 *x509);
EXP_OPTION int ReadCertificateByPKCS12(X509 **x509, const char *pkcs12file, const char *passwd, EVP_PKEY **pkey);

// Decodes binary (DER) cert data and returns a cert object
EXP_OPTION int ddocDecodeX509Data(X509 **ppX509, const byte* certData, int certLen);

// Decodes base64 (PEM) cert data and returns a cert object
EXP_OPTION int ddocDecodeX509PEMData(X509 **ppX509, const char* certData, int certLen);

// get certificate PEM form
EXP_OPTION int getCertPEM(X509* cert, int bHeaders, char** buf);


// retrieves this certificates serial number
EXP_OPTION int GetCertSerialNumber(char* szSerial, int nMaxLen, const char *szCertfile);
// Returns the certificates validity first date
EXP_OPTION int getCertNotBefore(const SignedDoc* pSigDoc, X509* cert, char* timestamp, int len);

// Retrieves the certificates first validity time as tim_t in GMT zone
EXP_OPTION time_t getCertNotBeforeTimeT(X509* pCert);
// Retrieves the certificates last validity time as tim_t in GMT zone
EXP_OPTION time_t getCertNotAfterTimeT(X509* pCert);

// Returns the certificates validity last date
EXP_OPTION int getCertNotAfter(const SignedDoc* pSigDoc, X509* cert, char* timestamp, int len);
// Saves the certificate in a file
EXP_OPTION int saveCert(X509* cert, const char* szFileName, int nFormat);
// decodes PEM cert data
EXP_OPTION void* decodeCert(const char* pemData);
// encodes certificate
EXP_OPTION void encodeCert(const X509* x509, char * encodedCert, int* encodedCertLen);

// Reads certificates PolicyIdentifiers and returns 
// them in a newly allocated structure
EXP_OPTION int readCertPolicies(X509* pX509, PolicyIdentifier** pPolicies, int* nPols);

// Frees policy identifiers array
EXP_OPTION void PolicyIdentifiers_free(PolicyIdentifier* pPolicies, int nPols);

// Checks if this is a company CPS policy
EXP_OPTION int isCompanyCPSPolicy(PolicyIdentifier* pPolicy);

EXP_OPTION int isCertValid(X509* cert, time_t tDate);
EXP_OPTION int isCertSignedBy(X509* cert, const char* cafile);
int writeCertToXMLFile(BIO* bout, X509* cert);

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
				       OCSP_RESPONSE **ppResp);

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
				       OCSP_RESPONSE **ppResp, unsigned long ip);

  //--------------------------------------------------
  // Returns the certificates sha1 hash. 
  // pCert - certificate data
  // pMemBuf - memory buffer object for storing DN
  // returns error code or ERR_OK
  //--------------------------------------------------
  int ddocCertGetDigest(X509* pCert, DigiDocMemBuf* pMemBuf);

  //--------------------------------------------------
  // Returns the certificates public key sha1 hash. 
  // pCert - certificate data
  // pMemBuf - memory buffer object for storing DN
  // returns error code or ERR_OK
  //--------------------------------------------------
  int ddocCertGetPubkeyDigest(X509* pCert, DigiDocMemBuf* pMemBuf);

  //--------------------------------------------------
  // Returns the certificates issuer name.
  // pCert - certificate data
  // pMemBuf - memory buffer object for storing DN
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int ddocCertGetIssuerDN(X509* pCert, DigiDocMemBuf* pMemBuf);

  //--------------------------------------------------
  // Returns the certificates subject name.
  // pCert - certificate data
  // pMemBuf - memory buffer object for storing DN
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int ddocCertGetSubjectDN(X509* pCert, DigiDocMemBuf* pMemBuf);

	EXP_OPTION int ddocCertGetDNFromName(X509_NAME* pName, DigiDocMemBuf* pMemBuf);

  //--------------------------------------------------
  // Returns the certificates subject CN
  // pCert - certificate data
  // pMemBuf - memory buffer object for storing result
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int ddocCertGetSubjectCN(X509* pCert, DigiDocMemBuf* pMemBuf);

  //--------------------------------------------------
  // Returns the certificates issuer CN
  // pCert - certificate data
  // pMemBuf - memory buffer object for storing result
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int ddocCertGetIssuerCN(X509* pCert, DigiDocMemBuf* pMemBuf);

  //--------------------------------------------------
  // Returns the certificates subject first name
  // pCert - certificate data
  // pMemBuf - memory buffer object for storing result
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int ddocCertGetSubjectFirstName(X509* pCert, DigiDocMemBuf* pMemBuf);

  //--------------------------------------------------
  // Returns the certificates subject last name
  // pCert - certificate data
  // pMemBuf - memory buffer object for storing result
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int ddocCertGetSubjectLastName(X509* pCert, DigiDocMemBuf* pMemBuf);

  //--------------------------------------------------
  // Returns the certificates subject personal code
  // pCert - certificate data
  // pMemBuf - memory buffer object for storing result
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int ddocCertGetSubjectPerCode(X509* pCert, DigiDocMemBuf* pMemBuf);

  //--------------------------------------------------
  // Returns the certificates subject country code
  // pCert - certificate data
  // pMemBuf - memory buffer object for storing result
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int ddocCertGetSubjectCountryName(X509* pCert, DigiDocMemBuf* pMemBuf);

  //--------------------------------------------------
  // Returns the certificates subject organization
  // pCert - certificate data
  // pMemBuf - memory buffer object for storing result
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int ddocCertGetSubjectOrganization(X509* pCert, DigiDocMemBuf* pMemBuf);

  //--------------------------------------------------
  // Returns the certificates subject organization unit
  // pCert - certificate data
  // pMemBuf - memory buffer object for storing result
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int ddocCertGetSubjectOrganizationUnit(X509* pCert, DigiDocMemBuf* pMemBuf);


//================< deprecated functions> =================================
// these functions are deprecated. Use the replacements in DigiDocCert.h
// these functions will be removed in future releases!
#ifdef WITH_DEPRECATED_FUNCTIONS

// decodes cert data - deprecated!
// USE ddocDecodeX509PEMData() instead!
EXP_OPTION int decodeCertificateData(X509 **newX509, const byte* certData, int certLen);

// Returns the certificates issuer name
// USE: ddocCertGetIssuerDN()
EXP_OPTION int getCertIssuerName(X509* cert, char* buf, int* buflen);

// Returns the certificates subject name
// USE: ddocCertGetSubjectDN()
EXP_OPTION int getCertSubjectName(X509* cert, char* buf, int* buflen);


// reads cert issuers CN
// USE: ddocCertGetIssuerCN()
EXP_OPTION int getCertIssuerCN(X509* cert, char* buf, int* buflen, int bUTF8);

// Returns the certificates subjects DN
// USE: ddocCertGetSubjectDN()
EXP_OPTION int getCertSubjectDN(X509* cert, char* buf, int* buflen, int bUTF8);

// reads cert subjects CN
// USE: ddocCertGetSubjectCN()
EXP_OPTION int getCertSubjectCN(X509* cert, char* buf, int* buflen, int bUTF8);

// get certificate owners id-code
// USE: ddocCertGetSubjectPerCode()
EXP_OPTION int getCertOwnerCode(const X509* pCert, char* buf, int len);

#endif  // WITH_DEPRECATED_FUNCTIONS

#ifdef  __cplusplus
}
#endif


#endif  // __DIGI_DOC_CERT_H__

