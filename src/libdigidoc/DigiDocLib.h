#ifndef __DIGI_DOC_LIB_H__
#define __DIGI_DOC_LIB_H__
//==================================================
// FILE:	DigiDocLib.h
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
// Lesser General Public License for more details.ode
// GNU Lesser General Public Licence is available at
// http://www.gnu.org/copyleft/lesser.html
//==========< HISTORY >=============================
//      10.02.2004      Integrated
//      02.01.2004      Veiko Sinivee
//                      Added support for format 1.3
//      29.10.2003      Aare Amenberg
//                      Added some function headers 
//                      (needed for compiling COM)
// 1.0	09.04.2002	Veiko Sinivee	
//  Supports XML format (Type: SK-XML-1.0)
//==================================================

#ifdef  __cplusplus
extern "C" {
#endif

#include <openssl/x509.h>
#include <openssl/ocsp.h>
#include <libdigidoc/DigiDocMem.h>
#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocError.h>
#include <libdigidoc/DigiDocObj.h>
#include <libdigidoc/DigiDocVerify.h>


//==========< data structures >====================

// supported format and version combination
typedef struct FormatAndVer_st {
	char* szFormat;
	char* szVersion;	
} FormatAndVer;


//==========< admin functions >====================

// returns the library name
EXP_OPTION const char* getLibName();
// returns the library version
EXP_OPTION const char* getLibVersion();
// returns an array of supported formats terminated by NULL
EXP_OPTION const char** getSupportedFormats();
// returns an array of supported formats  and versions terminated by NULL
EXP_OPTION FormatAndVer* getSupportedFormatsAndVersions();
// returns the GUI version
EXP_OPTION const char* getGUIVersion();
// sets the GUI version
EXP_OPTION void setGUIVersion(const char* szVer);

// initializes error library
EXP_OPTION void initDigiDocLib();
// cleanup of error library
EXP_OPTION void finalizeDigiDocLib();

// trim leading and trailing whitespace
EXP_OPTION char* trim(char* src);

// Sets a string element of a struct to a new value
int setString(char** dest, const char* value, int valLen);

EXP_OPTION int checkFileExists(const char* szFileName);

int ddocPullUrl(const char* url, DigiDocMemBuf* pSendData, DigiDocMemBuf* pRecvData, 
		const char* proxyHost, const char* proxyPort);

// Cheks a combination of format and version for validity
int ddocCheckFormatAndVer(const char* format, const char* version);


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
EXP_OPTION int ddocLocalTime(time_t* pTime, struct tm* pTmStruct, int bLocal);

EXP_OPTION int getTempFileName(char* szFileName, int len);

//==========< general crypto fucntions >============

// calculates input files digest.
// supports only SHA1
EXP_OPTION int calculateFileDigest(const char* szFileName, int nDigestType,
						byte* pDigestBuf, int* nDigestLen, long* lFileLen);

// calculate file size
EXP_OPTION int calculateFileSize(const char* szFileName, long* lFileLen);

// calculates files RSA+SHA1 signature
EXP_OPTION int calculateFileSignature(const char* szFileName, int nDigestType,
							byte* pSigBuf, int* nSigLen,
							const char *keyfile, const char* passwd);

// sign some arbitrary daya
EXP_OPTION int signData(const byte* data, int dlen, byte* pSigBuf, int* nSigLen,
			 int nDigestType, const char *keyfile, const char* passwd);

// calculate digest over some arbitrary data
EXP_OPTION int calculateDigest(const byte* data, int nDataLen, int nDigestType,
					byte* pDigestBuf, int* nDigestLen);



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
			 const char* country, X509* pCert, const char* id);

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
EXP_OPTION int ddocGetSignedHash(SignatureInfo* pSigInfo, char* pBuf, int* pBufLen, int enc, int bWithAsn1Prefix);


//==========< XML generating functions >=================
// thse functions generate certain XML form info, that
// is used as input data for either signing or digesting

// creates XML <SignedInfo> block (actually signed info)
EXP_OPTION char* createXMLSignedInfo(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo);
// creates XML mime type <SinatureProperty> block for signing
EXP_OPTION int createMimeType(char* buf, const char* mime, 
				   const char* sigId, const char* docId);

//==========< memory mgmt functions >=================

// Get the filename part of full file name
EXP_OPTION const char* getSimpleFileName(const char* szFileName);
// Get the absolute filename with path
EXP_OPTION int getFullFileName(const char* szFileName, char* szDest, int len);
// Get the path part of full file name
  EXP_OPTION int getFileNamePath(const char* szFileName, char* szPath, int len);


//------------------------ ??

// Sets the signatures certificate and calculates
EXP_OPTION int setSignatureCertFile(SignatureInfo* pSigInfo, const char* certFile);
// Sets the signatures certificate and calculates it's digest
EXP_OPTION int setSignatureCert(SignatureInfo* pSigInfo, X509* cert);

// Sets the signature value 
EXP_OPTION int setSignatureValue(SignatureInfo* pSigInfo, byte* szSignature, int sigLen);

// Sets the signature value from a file that contains
// the base64 encoded signature value
EXP_OPTION int setSignatureValueFromFile(SignatureInfo* pSigInfo, char* szSigFile);


//================< certficate info functions> =================================

// Decodes a signature value
void decodeSignatureValue(const char* b64val, int b64len, char* value, int* vlen);

// Returns the user signatures certificate data
EXP_OPTION X509* getSignCertData(const SignatureInfo* pSignInfo);
// Returns the notary signatures certificate data
//EXP_OPTION X509* getNotCertData(const NotaryInfo* pNotInfo);



//================< deprecated functions> =================================
// these functions are deprecated. Use the replacements in DigiDocCert.h
// these functions will be removed in future releases!
#ifdef WITH_DEPRECATED_FUNCTIONS

// get signers id-code
// USE: ddocCertGetSubjectPerCode(getSignCertData(pSignInfo), pMemBuf);
EXP_OPTION int getSignerCode(const SignatureInfo* pSigInfo, char* buf);

// get signers first name
// USE: ddocCertGetSubjectFirstName(getSignCertData(pSignInfo), pMemBuf);
EXP_OPTION int getSignerFirstName(const SignatureInfo* pSigInfo, char* buf);

// get signers last name
// USE: ddocCertGetSubjectLastName(getSignCertData(pSignInfo), pMemBuf);
EXP_OPTION int getSignerLastName(const SignatureInfo* pSigInfo, char* buf);

// get the whole signers CN in desired charset
// USE: ddocCertGetSubjectCN(getSignCertData(pSignInfo), pMemBuf);
EXP_OPTION int getSignerCN(const SignatureInfo* pSigInfo, char* buf, int bUTF8);

#endif // WITH_DEPRECATED_FUNCTIONS

//==================================================================================
void CorrectCharacters(char *psText);

#ifdef  __cplusplus
}
#endif


#endif // __DIGI_DOC_LIB_H__

