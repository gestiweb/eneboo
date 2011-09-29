#ifndef __DIGIDOC_OCSP_H__
#define __DIGIDOC_OCSP_H__
//==================================================
// FILE:	DigiDocOCSP.h
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

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocObj.h>
#include <libdigidoc/DigiDocMem.h>

#include <openssl/ocsp.h>

//==========< XML generation routines >========================

#ifdef  __cplusplus
extern "C" {
#endif

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
		    unsigned long ip_addr);

// Decodes binary (DER) OCSP_RESPONSE data and returns a OCSP_RESPONSE object
EXP_OPTION int ddocDecodeOCSPResponseData(OCSP_RESPONSE **ppResp, const byte* data, int len);

// Decodes base64 (PEM) OCSP_RESPONSE data and returns a OCSP_RESPONSE object
EXP_OPTION int ddocDecodeOCSPResponsePEMData(OCSP_RESPONSE **ppResp, const byte* data, int len);

EXP_OPTION int writeOCSPRequest(SignedDoc* pSigDoc, const char* signerCertFile, 
				 const char* issuertCertFile,
				 byte* nonce, int nlen, const char* szOutputFile);

// Creates and sends an OCSP_REQUEST object
// to the notary server, receives the response
// and uses it to create a confirmation object.
EXP_OPTION int getConfirmation(SignedDoc* pSigDoc, SignatureInfo* pSigInfo,
                               	const X509** caCerts, const X509* pNotCert,
			       char* pkcs12FileName, char* pkcs12Password,
                               	char* notaryURL, char* proxyHost, char* proxyPort);

// another way to get OCSP confirmation by sending also callers ip address
EXP_OPTION int getConfirmationWithIp(SignedDoc* pSigDoc, SignatureInfo* pSigInfo, 
				     const X509** caCerts, const X509* pNotCert,
				     char* pkcs12FileName, char* pkcs12Password,
				     char* notaryURL, char* proxyHost, char* proxyPort,
				     unsigned long ip);

// use this if you passed NULL-s to getConfirmation() param notaryCert
EXP_OPTION int finalizeAndVerifyNotary(SignedDoc* pSigDoc, SignatureInfo* pSigInfo,
				       NotaryInfo* pNotInf,
				       const X509** caCerts, const X509* pNotCert);

int ReadOCSPResponse(OCSP_RESPONSE **newOCSP_RESPONSE, const char* szFileName);
int initializeNotaryInfoWithOCSP(SignedDoc *pSigDoc, NotaryInfo *pNotary, 
				OCSP_RESPONSE *resp, X509 *notCert, int initDigest);

//--------------------------------------------------
// Verfies OCSP_RESPONSE signature
// pResp - signed OCSP response
// caCerts - CA certificate pointer array terminated with NULL
// CApath - path to (directory) all certs
// notCertFile - Notary (e.g. OCSP responder) cert file 
//--------------------------------------------------
int verifyOCSPResponse(OCSP_RESPONSE* pResp, 
				    const X509** caCerts, const char *CApath, 
				    const X509* notCert);

//--------------------------------------------------
// Helper function to read OCSP_RESPONSE from binary input data
// ppResp - address of newly allocated OCSP_RESPONSE object
// pMBufInData - input data
// returns error code or ERR_OK
//--------------------------------------------------
int ddocOcspReadOcspResp(OCSP_RESPONSE** ppResp, DigiDocMemBuf* pMBufInData);

//--------------------------------------------------
// Helper function to write OCSP_RESPONSE to binary output data
// pResp - address of OCSP_RESPONSE object
// pMBufOutData - output data
// returns error code or ERR_OK
//--------------------------------------------------
int ddocOcspWriteOcspResp(OCSP_RESPONSE* pResp, DigiDocMemBuf* pMBufOutData);

// get OCSP in PEM form
EXP_OPTION int getOcspPEM(OCSP_RESPONSE* pResp, int bHeaders, char** buf);

//--------------------------------------------------
// Helper function to return OCSP_RESPONSE in base64 form.
// Memory buffer will be resized as necessary.
// Caller must release output buffer.
// pNotary - Notary object
// bHeaders - 1= with headers, 0=no headers
// pMBufOutData - output data
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocGetOcspBase64(NotaryInfo *pNotary, int bHeaders, DigiDocMemBuf* pMBufOutData);

#ifdef  __cplusplus
}
#endif

#endif // __DIGIDOC_OCSP_H__


