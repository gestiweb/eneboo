#ifndef __DIGIDOC_OBJ_H__
#define __DIGIDOC_OBJ_H__
//==================================================
// FILE:	DigiDocGen.h
// PROJECT:     Digi Doc
// DESCRIPTION: DigiDoc helper routines for accessing dogidoc data
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
//#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocMem.h>
#include <openssl/x509.h>
#include <openssl/ocsp.h>
#ifdef WITH_TS
#include <libdigidoc/ts/DigiDocTS.h>
#endif

//==========< DigiDoc object structure >========================

#ifdef  __cplusplus
extern "C" {
#endif


// contains the signed attributes of each document file entry
typedef struct DocInfo_st {
	char* szDocId;			// document id
	char* szDigestType;		// digest method used
	byte* szDigest;			// digest value
	int   nDigestLen;			// digest value length
	byte* szMimeDigest;		// digest value
	int nMimeDigestLen;		// digest value length
} DocInfo;

typedef struct SignatureProductionPlace_st {
  char* szCity;
  char* szStateOrProvince;
  char* szPostalCode;
  char* szCountryName;
} SignatureProductionPlace;

typedef struct SignerRole_st {
  int nClaimedRoles;
  char** pClaimedRoles;
  int nCertifiedRoles;
  char** pCertifiedRoles;
} SignerRole;

typedef struct DigestValue_st {
  char* szDigestMethod;
  DigiDocMemBuf mbufDigestValue;
} DigestValue;
  /*
typedef struct ReferenceInfo_st {
  char* szId;
  char* szUri;
  char* szType;
  DigestValue digestValue;
} ReferenceInfo;
  */
typedef struct SignatureValue_st {
  char* szId;
  char* szType;
  DigiDocMemBuf mbufSignatureValue;
} SignatureValue;

#define CERTID_TYPE_UNKNOWN             0
#define CERTID_TYPE_SIGNERS_CERTID      1
#define CERTID_TYPE_RESPONDERS_CERTID   2
#define CERTID_TYPE_TSA_CERTID          3


typedef struct CertID_st {
  int nType;    // internal CertID type:
  char* szId;
  char* szIssuerSerial;
  char* szIssuerName;
  DigestValue* pDigestValue;
} CertID;

typedef struct CertIDList_st {
  int nCertIDs;
  CertID** pCertIDs;
} CertIDList;

#define CERTID_VALUE_UNKNOWN             0
#define CERTID_VALUE_SIGNERS_CERT        1
#define CERTID_VALUE_RESPONDERS_CERT     2
#define CERTID_VALUE_TSA_CERT            3

typedef struct CertValue_st {
  int nType;
  char* szId;
  X509* pCert;
} CertValue;

typedef struct CertValueList_st {
  int nCertValues;
  CertValue** pCertValues;
} CertValueList;

// data file entry for each data file
typedef struct DataFile_st {
  char* szId;			// data file id
  char* szFileName;	// signed doc file name
  char* szMimeType;	// date file mime type
  char* szContentType;	// DETATCHED, EMBEDDED or EMBEDDED_BASE64
  long  nSize;		// file size (unencoded)
  char* szDigestType;	// digest type
  DigiDocMemBuf mbufDigest;  // real DataFile digest value
  DigiDocMemBuf mbufWrongDigest; // bad digest calculated in some versions
  DigiDocMemBuf mbufDetachedDigest; // detached file digest
  int nAttributes;		// number of other attributes
  char* szCharset;		// datafile initial codepage
  char** pAttNames;		// other attribute names
  char** pAttValues;		// other attribute values
  DigiDocMemBuf mbufContent;
} DataFile;

typedef struct NotaryInfo_sk {
  char* szId;			// Notary id
  char* szNotType;	// notary info type (OCSP-1.0)
  char* timeProduced;	// producedAt
  int nRespIdType;  // RESP_ID_NAME, RESP_ID_KEY
  DigiDocMemBuf mbufRespId;  // responder id value
  char* szDigestType;	// digest method used
  // notaries personal signature
  char* szSigType;	// signature type used
  DigiDocMemBuf mbufOcspDigest;  // OCSP response digest (as in XML
  DigiDocMemBuf mbufOcspResponse; // OCSP response (in memory held in DER)
} NotaryInfo;

// signature info for each user signature
typedef struct SignatureInfo_st {
  char* szId;			// signature id
  int nDocs;			// number of separate documents signed
  DocInfo** pDocs;	// info for each signed document	
  char* szTimeStamp;	// signature timestamp in format "YYYY-MM-DDTHH:MM:SSZ"
  DigestValue *pSigPropDigest;
  DigestValue *pSigPropRealDigest;
  DigestValue *pSigInfoRealDigest;
  SignatureValue *pSigValue;    // RSA+SHA1 signature value
  X509* pX509Cert;	// X509Cert certificate (used internally for data during loading)
  SignatureProductionPlace sigProdPlace;
  SignerRole signerRole;
  DigiDocMemBuf mbufOrigContent;
  NotaryInfo* pNotary;
#ifdef WITH_TS
  TimestampInfoList *pTimestamps;
#endif
  CertIDList *pCertIDs;
  CertValueList *pCertValues;
} SignatureInfo;



typedef struct SignedDoc_st {
	char* szFormat;		// data format name
	char* szFormatVer;	// data format version
	int nDataFiles;
	DataFile** pDataFiles;
	int nSignatures;
	SignatureInfo** pSignatures;
} SignedDoc;

//============================================================
// Sets a string element of a struct to a new value
// dest - element pointer
// value - new value
// valLen - value length (use -1 for null terminated strings)
//============================================================
EXP_OPTION int setString(char** dest, const char* value, int valLen);

// creates a new <SignedDoc> structure
EXP_OPTION int SignedDoc_new(SignedDoc **newSignedDoc, const char* format, const char* version);
// cleanup signed doc data
EXP_OPTION void SignedDoc_free(SignedDoc* pSigDoc);

//======================< DataFile functions >=============================

// returns the number of data files
EXP_OPTION int getCountOfDataFiles(const SignedDoc* pSigDoc);
// returns the n-th DataFile object
EXP_OPTION DataFile* getDataFile(const SignedDoc* pSigDoc, int nIdx);
// returns the last DataFile object
EXP_OPTION DataFile* ddocGetLastDataFile(const SignedDoc* pSigDoc);
// Retrieve and convert DataFile Filename atribute and convert
EXP_OPTION int ddocGetDataFileFilename(SignedDoc* pSigDoc, const char* szDocId, void** ppBuf, int* pLen);
// returns the DataFile object with the given id
EXP_OPTION DataFile* getDataFileWithId(const SignedDoc* pSigDoc, const char* id);
// add a <DataFile> block to <SignedDoc>
// use NULL for any parameter you don't have the value
// use NULL for id if you want to auto-calculate it (recommended!!!)
EXP_OPTION int DataFile_new(DataFile **newDataFile, SignedDoc* pSigDoc, const char* id,
					   const char* filename, const char* contentType, 
					   const char* mime, long size,
					   const byte* digest, int digLen,
					   const char* digType, const char* szCharset);
// cleanup DataFile memory
EXP_OPTION void DataFile_free(DataFile* pDataFile);

// Removes this DataFile from signed doc and frees it's memory
EXP_OPTION int DataFile_delete(SignedDoc* pSigDoc, const char* id);

//--------------------------------------------------
// Accessor for Digest atribute of DataFile object.
// pDataFile - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION DigiDocMemBuf* ddocDataFile_GetDigestValue(DataFile* pDataFile);

//--------------------------------------------------
// Mutatoror for Digest atribute of DataFile object.
// pDataFile - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// len - length of value in bytes [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocDataFile_SetDigestValue(DataFile* pDataFile, 
					   const char* value, long len);

//--------------------------------------------------
// Accessor for DetachedDigest atribute of DataFile object.
// pDataFile - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION DigiDocMemBuf* ddocDataFile_GetDetachedDigestValue(DataFile* pDataFile);

//--------------------------------------------------
// Mutatoror for DetachedDigest atribute of DataFile object.
// pDataFile - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// len - length of value in bytes [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocDataFile_SetDetachedDigestValue(DataFile* pDataFile, 
						   const char* value, long len);



// Returns number of DataFile attributes
EXP_OPTION int getCountOfDataFileAttributes(const DataFile* pDataFile);
// Adds an attribute to data file
EXP_OPTION int addDataFileAttribute(DataFile* pDataFile, const char* name, 
									const char* value);
// Gets an attribute of a data file
EXP_OPTION int getDataFileAttribute(DataFile* pDataFile, int idx, char** name, char** value);

// get datafile cahed data
int ddocGetDataFileCachedData(SignedDoc* pSigDoc, const char* szDocId, void** ppBuf, long* pLen);

// apppends DataFile content to cache
void ddocAppendDataFileData(DataFile* pDf, int maxLen, void* data, int len);

// calculates file size and digest and store in the
// given DataFile object
EXP_OPTION int calculateDataFileSizeAndDigest(SignedDoc* pSigDoc, const char* id,
								const char* filename, int digType);

//======================< DigestValue functions >=============================

//--------------------------------------------------
// "Constructor" of DigestValue object
// ppDigestValue - address of buffer for newly allocated object [REQUIRED]
// szDigestMethod - digest method [OPTIONAL]
// szDigVal/lDigLen - digest value and length [OPTIONAL]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocDigestValue_new(DigestValue** ppDigestValue, 
				   const char* szDigestMethod, 
				   void* szDigVal, long lDigLen);

//--------------------------------------------------
// "Destructor" of DigestValue object
// pDigestValue - address of object to be deleted [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocDigestValue_free(DigestValue* pDigestValue);

//--------------------------------------------------
// Accessor for DigestMethod atribute of DigestValue object.
// pDigestValue - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* ddocDigestValue_GetDigestMethod(DigestValue* pDigestValue);

//--------------------------------------------------
// Mutatoror for DigestMethod atribute of DigestValue object.
// pDigestValue - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocDigestValue_SetDigestMethod(DigestValue* pDigestValue, const char* value);

//--------------------------------------------------
// Accessor for DigestValue atribute of DigestValue object.
// pDigestValue - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION DigiDocMemBuf* ddocDigestValue_GetDigestValue(DigestValue* pDigestValue);

//--------------------------------------------------
// Mutatoror for DigestValue atribute of DigestValue object.
// pDigestValue - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// len - length of value in bytes [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocDigestValue_SetDigestValue(DigestValue* pDigestValue, 
					      const char* value, long len);

//--------------------------------------------------
// Compares two DigestValue structure on equality
// pDigest1 - address of first digest [REQUIRED]
// pDigest2 - address of second digest [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocCompareDigestValues(DigestValue* pDigest1, DigestValue* pDigest2);

//--------------------------------------------------
// Generates XML for <DigestValue> element
// pSigDoc - signed doc object [REQUIRED]
// pDigestValue - DigestValue object [REQUIRED]
// pBuf - memory buffer for storing xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocDigestValue_toXML(const SignedDoc* pSigDoc, const DigestValue* pDigestValue, DigiDocMemBuf* pBuf);

//======================< SignatureValue functions >=============================

//============================================================
// Returns the next free signature id
// pSigDoc - signed doc pointer
//============================================================
EXP_OPTION int getNextSignatureId(const SignedDoc* pSigDoc);

//--------------------------------------------------
// "Constructor" of SignatureValue object
// ppSignatureValue - address of buffer for newly allocated object [REQUIRED]
// szId - Id atribute value [OPTIONAL]
// szType - signature type [OPTIONAL]
// szDigVal/lDigLen - digest value and length [OPTIONAL]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocSignatureValue_new(SignatureValue** ppSignatureValue, 
				      const char* szId, const char* szType,
				      void* szSigVal, long lSigLen);

//--------------------------------------------------
// "Destructor" of SignatureValue object
// pSignatureValue - address of object to be deleted [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocSignatureValue_free(SignatureValue* pSignatureValue);

//--------------------------------------------------
// Accessor for Id atribute of SignatureValue object.
// pSignatureValue - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* ddocSignatureValue_GetId(const SignatureValue* pSignatureValue);

//--------------------------------------------------
// Mutatoror for Id atribute of SignatureValue object.
// pSignatureValue - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocSignatureValue_SetId(SignatureValue* pSignatureValue, const char* value);

//--------------------------------------------------
// Accessor for Type atribute of SignatureValue object.
// pSignatureValue - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* ddocSignatureValue_GetType(const SignatureValue* pSignatureValue);

//--------------------------------------------------
// Mutatoror for Type atribute of SignatureValue object.
// pSignatureValue - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocSignatureValue_SetType(SignatureValue* pSignatureValue, const char* value);

//--------------------------------------------------
// Accessor for SignatureValue atribute of SignatureValue object.
// pSignatureValue - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION DigiDocMemBuf* ddocSignatureValue_GetSignatureValue(const SignatureValue* pSignatureValue);

//--------------------------------------------------
// Mutatoror for SignatureValue atribute of SignatureValue object.
// pSignatureValue - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// len - length of value in bytes [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocSignatureValue_SetSignatureValue(SignatureValue* pSignatureValue, 
						    const char* value, long len);

//--------------------------------------------------
// Generates XML for <IncludeInfo> element
// pSignatureValue - SignatureValue object [REQUIRED]
// pBuf - memory buffer for storing xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocSignatureValue_toXML(const SignatureValue* pSignatureValue, DigiDocMemBuf* pBuf);

//======================< CertID >====================================

//--------------------------------------------------
// "Constructor" of CertID object
// ppCertID - address of buffer for newly allocated object [REQUIRED]
// szId - Id atribute value [OPTIONAL]
// nType - certid internal type (signers or responders cert) [REQUIRED]
// szIssuerSerial - issuer serial number [OPTIONAL]
// szIssuerName - issuer DN [OPTIONAL]
// szDigVal/lDigLen - digest value and length [OPTIONAL]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertID_new(CertID** ppCertID, 
			      int nType, const char* szId,
			      const char* szIssuerSerial, const char* szIssuerName,
			      void* szDigVal, long lDigLen);

//--------------------------------------------------
// "Destructor" of CertID object
// pCertID - address of object to be deleted [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertID_free(CertID* pCertID);

//--------------------------------------------------
// Accessor for Id atribute of CertID object.
// pCertID - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* ddocCertID_GetId(const CertID* pCertID);

//--------------------------------------------------
// Mutatoror for Id atribute of CertID object.
// pCertID - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertID_SetId(CertID* pCertID, const char* value);

//--------------------------------------------------
// Accessor for IssuerSerial atribute of CertID object.
// pCertID - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* ddocCertID_GetIssuerSerial(const CertID* pCertID);

//--------------------------------------------------
// Mutatoror for IssuerSerial atribute of CertID object.
// pCertID - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertID_SetIssuerSerial(CertID* pCertID, const char* value);

//--------------------------------------------------
// Accessor for IssuerName atribute of CertID object.
// pCertID - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* ddocCertID_GetIssuerName(const CertID* pCertID);

//--------------------------------------------------
// Mutatoror for IssuerName atribute of CertID object.
// pCertID - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertID_SetIssuerName(CertID* pCertID, const char* value);

//--------------------------------------------------
// Accessor for DigestValue atribute of CertID object.
// pCertID - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION DigiDocMemBuf* ddocCertID_GetDigestValue(const CertID* pCertID);


//--------------------------------------------------
// Mutatoror for DigestValue atribute of CertID object.
// pCertID - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// len - length of value in bytes [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertID_SetDigestValue(CertID* pCertID, 
					 const char* value, long len);

//--------------------------------------------------
// Generates XML for <Cert> element
// pCertID - CertID object [REQUIRED]
// pBuf - memory buffer for storing xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocCertID_toXML(const SignedDoc* pSigDoc, const CertID* pCertID, DigiDocMemBuf* pBuf);

//--------------------------------------------------
// Generates XML for <CompleteCertificateRefs> element
// pSigDoc - SignedDoc object [REQUIRED]
// pBuf - memory buffer for storing xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocCompleteCertificateRefs_toXML(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo, DigiDocMemBuf* pBuf);

//--------------------------------------------------
// Generates XML for <CompleteRevocationRefs> element
// pSigDoc - SignedDoc object [REQUIRED]
// pBuf - memory buffer for storing xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocCompleteRevocationRefs_toXML(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo, DigiDocMemBuf* pBuf);


//==========< CertIDList >====================

//--------------------------------------------------
// "Constructor" of CertIDList object
// ppCertIDList - address of buffer for newly allocated object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertIDList_new(CertIDList** ppCertIDList);

//--------------------------------------------------
// "Destructor" of CertIDList object
// pCertIDList - address of object to be deleted [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertIDList_free(CertIDList* pCertIDList);

//--------------------------------------------------
// Accessor for count of CertIDs subelement of CertIDList object.
// pCertIDList - pointer to CertIDList object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocCertIDList_addCertID(CertIDList* pCertIDList, CertID* pCertID);

//--------------------------------------------------
// Accessor for count of CertIDs subelement of CertIDList object.
// pCertIDList - pointer to CertIDList object [REQUIRED]
// returns count or -1 for error. Then use error API to check errors
//--------------------------------------------------
EXP_OPTION int ddocCertIDList_GetCertIDsCount(CertIDList* pCertIDList);

//--------------------------------------------------
// Accessor for CertIDs subelement of CertIDList object.
// pCertIDList - pointer to CertIDList object [REQUIRED]
// nIdx - index of CertID object [REQUIRED]
// returns CertID pointer or NULL for error
//--------------------------------------------------
EXP_OPTION CertID* ddocCertIDList_GetCertID(CertIDList* pCertIDList, int nIdx);

//--------------------------------------------------
// Accessor for last CertIDs subelement of CertIDList object.
// pCertIDList - pointer to CertIDList object [REQUIRED]
// returns CertID pointer or NULL for error
//--------------------------------------------------
EXP_OPTION CertID* ddocCertIDList_GetLastCertID(CertIDList* pCertIDList);

//--------------------------------------------------
// Deletes CertID subelement of CertIDList object.
// pCertIDList - pointer to CertIDList object [REQUIRED]
// nIdx - index of CertID object to be removed [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertIDList_DeleteCertID(CertIDList* pCertIDList, int nIdx);

//--------------------------------------------------
// Finds a CertID object with required type
// pCertIDList - pointer to CertIDList object [REQUIRED]
// nType - type of CertID object [REQUIRED]
// returns CertID pointer or NULL for error
//--------------------------------------------------
EXP_OPTION CertID* ddocCertIDList_GetCertIDOfType(CertIDList* pCertIDList, int nType);

//--------------------------------------------------
// Finds a CertID object with required serial nr
// pCertIDList - pointer to CertIDList object [REQUIRED]
// szSerial - issuer serial
// returns CertID pointer or NULL for error
//--------------------------------------------------
EXP_OPTION CertID* ddocCertIDList_GetCertIDOfSerial(CertIDList* pCertIDList, const char* szSerial);

//--------------------------------------------------
// Finds a CertID object with required type or creates a new one
// pCertIDList - pointer to CertIDList object [REQUIRED]
// nType - type of CertID object [REQUIRED]
// returns CertID pointer or NULL for error
//--------------------------------------------------
EXP_OPTION CertID* ddocCertIDList_GetOrCreateCertIDOfType(CertIDList* pCertIDList, int nType);

//======================< CertValue >====================================

//--------------------------------------------------
// "Constructor" of CertValue object
// ppCertValue - address of buffer for newly allocated object [REQUIRED]
// szId - Id atribute value [OPTIONAL]
// nType - certid internal type (signers or responders cert) [REQUIRED]
// pCert - certificate itself [OPTIONAL]. Must fill in later. Do not X509_free() param!
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertValue_new(CertValue** ppCertValue, 
				 int nType, const char* szId,
				 X509* pCert);

//--------------------------------------------------
// "Destructor" of CertValue object
// pCertValue - address of object to be deleted [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertValue_free(CertValue* pCertValue);

//--------------------------------------------------
// Accessor for Id atribute of CertValue object.
// pCertValue - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* ddocCertValue_GetId(CertValue* pCertValue);

//--------------------------------------------------
// Mutatoror for Id atribute of CertValue object.
// pCertValue - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertValue_SetId(CertValue* pCertValue, const char* value);

//--------------------------------------------------
// Accessor for Cert atribute of CertValue object.
// pCertValue - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION X509* ddocCertValue_GetCert(CertValue* pCertValue);

//--------------------------------------------------
// Mutatoror for Cert atribute of CertValue object.
// pCertValue - address of object [REQUIRED]
// pCert - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertValue_SetCert(CertValue* pCertValue, X509* pCert);

//--------------------------------------------------
// Generates XML for <EncapsulatedX509Certificate> element
// pCertID - CertID object [REQUIRED]
// pBuf - memory buffer for storing xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocCertValue_toXML(const CertValue* pCertValue, DigiDocMemBuf* pBuf);

//==========< CertValueList >====================

//--------------------------------------------------
// "Constructor" of CertValueList object
// ppCertValueList - address of buffer for newly allocated object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertValueList_new(CertValueList** ppCertValueList);

//--------------------------------------------------
// "Destructor" of CertValueList object
// pCertValueList - address of object to be deleted [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertValueList_free(CertValueList* pCertValueList);

//--------------------------------------------------
// Adds a CertValue element to CertValueList object.
// pCertValueList - pointer to CertValueList object [REQUIRED]
// pCertValue - new object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertValueList_addCertValue(CertValueList* pCertValueList, CertValue* pCertValue);

//--------------------------------------------------
// Accessor for count of CertValues subelement of CertValueList object.
// pCertValueList - pointer to CertValueList object [REQUIRED]
// returns count or -1 for error. Then use error API to check errors
//--------------------------------------------------
EXP_OPTION int ddocCertValueList_GetCertValuesCount(CertValueList* pCertValueList);

//--------------------------------------------------
// Accessor for CertValues subelement of CertValueList object.
// pCertValueList - pointer to CertValueList object [REQUIRED]
// nIdx - index of CertValue object [REQUIRED]
// returns CertValue pointer or NULL for error
//--------------------------------------------------
EXP_OPTION CertValue* ddocCertValueList_GetCertValue(CertValueList* pCertValueList, int nIdx);

//--------------------------------------------------
// Deletes CertValue subelement of CertValueList object.
// pCertValueList - pointer to CertValueList object [REQUIRED]
// nIdx - index of CertValue object to be removed [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocCertValueList_DeleteCertValue(CertValueList* pCertValueList, int nIdx);

//--------------------------------------------------
// Finds a CertValue object with required type
// pCertValueList - pointer to CertValueList object [REQUIRED]
// nType - type of CertValue object [REQUIRED]
// returns CertValue pointer or NULL for error
//--------------------------------------------------
EXP_OPTION CertValue* ddocCertValueList_GetCertValueOfType(CertValueList* pCertValueList, int nType);

//--------------------------------------------------
// Finds a CertValue object with required type or creates a new one
// pCertValueList - pointer to CertValueList object [REQUIRED]
// nType - type of CertValue object [REQUIRED]
// returns CertValue pointer or NULL for error
//--------------------------------------------------
EXP_OPTION CertValue* ddocCertValueList_GetOrCreateCertValueOfType(CertValueList* pCertValueList, int nType);

//======================< SignatureInfo functions >=============================

// returns the number of signatures
EXP_OPTION int getCountOfSignatures(const SignedDoc* pSigDoc);
// Returns the desired SignatureInfo object
EXP_OPTION SignatureInfo* getSignature(const SignedDoc* pSigDoc, int nIdx);

//============================================================
// Returns signatures signed properties digest
// pSigInfo - signature info object
// return digest value as DigiDocMemBuf pointer or NULL
//============================================================
EXP_OPTION DigiDocMemBuf* ddocSigInfo_GetSigPropDigest(SignatureInfo* pSigInfo);

//============================================================
// Sets signatures signed properties digest
// pSigInfo - signature info object
// value - new binary digest value
// len - length of the value
//============================================================
EXP_OPTION int ddocSigInfo_SetSigPropDigest(SignatureInfo* pSigInfo, const char* value, long len);

//============================================================
// Returns signatures signed properties digest as read from file
// pSigInfo - signature info object
// return digest value as DigiDocMemBuf pointer or NULL
//============================================================
EXP_OPTION DigiDocMemBuf* ddocSigInfo_GetSigPropRealDigest(SignatureInfo* pSigInfo);

//============================================================
// Sets signatures signed properties real digest as read from file
// pSigInfo - signature info object
// value - new binary digest value
// len - length of the value
//============================================================
EXP_OPTION int ddocSigInfo_SetSigPropRealDigest(SignatureInfo* pSigInfo, const char* value, long len);

//============================================================
// Returns signatures signed info digest as read from file
// pSigInfo - signature info object
// return digest value as DigiDocMemBuf pointer or NULL
//============================================================
EXP_OPTION DigiDocMemBuf* ddocSigInfo_GetSigInfoRealDigest(SignatureInfo* pSigInfo);

//============================================================
// Sets signatures signed info real digest as read from file
// pSigInfo - signature info object
// value - new binary digest value
// len - length of the value
//============================================================
EXP_OPTION int ddocSigInfo_SetSigInfoRealDigest(SignatureInfo* pSigInfo, const char* value, long len);

//============================================================
// Returns signatures signature-value
// pSigInfo - signature info object
// return signature-value as SignatureValue pointer or NULL
//============================================================
EXP_OPTION SignatureValue* ddocSigInfo_GetSignatureValue(SignatureInfo* pSigInfo);

//============================================================
// Returns signatures signature-value
// pSigInfo - signature info object
// return signature-value as DigiDocMemBuf pointer or NULL
//============================================================
EXP_OPTION DigiDocMemBuf* ddocSigInfo_GetSignatureValue_Value(SignatureInfo* pSigInfo);

//============================================================
// Sets signatures signature-value
// pSigInfo - signature info object
// value - new binary signature value
// len - length of the value
//============================================================
EXP_OPTION int ddocSigInfo_SetSignatureValue(SignatureInfo* pSigInfo, const char* value, long len);

//============================================================
// Returns signaers certs - issuer-serial
// pSigInfo - signature info object
// return required atribute value
//============================================================
EXP_OPTION const char* ddocSigInfo_GetSignersCert_IssuerSerial(const SignatureInfo* pSigInfo);

//============================================================
// Sets signers certs issuer serial
// pSigInfo - signature info object
// value - new value
//============================================================
EXP_OPTION int ddocSigInfo_SetSignersCert_IssuerSerial(SignatureInfo* pSigInfo, const char* value);

//============================================================
// Returns signaers certs - issuer-name
// pSigInfo - signature info object
// return required atribute value
//============================================================
EXP_OPTION const char* ddocSigInfo_GetSignersCert_IssuerName(const SignatureInfo* pSigInfo);

//============================================================
// Sets signers certs issuer name
// pSigInfo - signature info object
// value - new value
//============================================================
EXP_OPTION int ddocSigInfo_SetSignersCert_IssuerName(SignatureInfo* pSigInfo, const char* value);

//============================================================
// Returns signers certs digest as DigiDocMemBuf object
// pSigInfo - signature info object
// return signers certs digest as DigiDocMemBuf pointer or NULL
//============================================================
EXP_OPTION DigiDocMemBuf* ddocSigInfo_GetSignersCert_DigestValue(const SignatureInfo* pSigInfo);

//============================================================
// Sets signers certs digest
// pSigInfo - signature info object
// value - new binary signature value
// len - length of the value
//============================================================
EXP_OPTION int ddocSigInfo_SetSignersCert_DigestValue(SignatureInfo* pSigInfo, const char* value, long len);

//--------------------------------------------------
// Finds a CertID object with required type
// pSigInfo - signature info object [REQUIRED]
// nType - type of CertID object [REQUIRED]
// returns CertID pointer or NULL for error
//--------------------------------------------------
EXP_OPTION CertID* ddocSigInfo_GetCertIDOfType(const SignatureInfo* pSigInfo, int nType);

//--------------------------------------------------
// Finds a CertID object with required type or creates a new one
// pSigInfo - signature info object [REQUIRED]
// nType - type of CertID object [REQUIRED]
// returns CertID pointer or NULL for error
//--------------------------------------------------
EXP_OPTION CertID* ddocSigInfo_GetOrCreateCertIDOfType(SignatureInfo* pSigInfo, int nType);


//--------------------------------------------------
// Finds a CertValue object with required type
// pSigInfo - signature info object [REQUIRED]
// nType - type of CertValue object [REQUIRED]
// returns CertValue pointer or NULL for error
//--------------------------------------------------
EXP_OPTION CertValue* ddocSigInfo_GetCertValueOfType(const SignatureInfo* pSigInfo, int nType);

//--------------------------------------------------
// Finds last CertValue
// pSigInfo - signature info object [REQUIRED]
// returns CertValue pointer or NULL for error
//--------------------------------------------------
EXP_OPTION CertValue* ddocSigInfo_GetLastCertValue(const SignatureInfo* pSigInfo);

//--------------------------------------------------
// Finds a CertValue object with required type or creates a new one
// pSigInfo - signature info object [REQUIRED]
// nType - type of CertValue object [REQUIRED]
// returns CertValue pointer or NULL for error
//--------------------------------------------------
EXP_OPTION CertValue* ddocSigInfo_GetOrCreateCertValueOfType(SignatureInfo* pSigInfo, int nType);

//--------------------------------------------------
// Finds the signers certificate
// pSigInfo - signature info object [REQUIRED]
// returns certificate or NULL
//--------------------------------------------------
EXP_OPTION X509* ddocSigInfo_GetSignersCert(const SignatureInfo* pSigInfo);

//--------------------------------------------------
// Sets the signers certificate
// pSigInfo - signature info object [REQUIRED]
// pCert - certificate [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocSigInfo_SetSignersCert(SignatureInfo* pSigInfo, X509* pCert);

//--------------------------------------------------
// Finds the OCSP responders certificate
// pSigInfo - signature info object [REQUIRED]
// returns certificate or NULL
//--------------------------------------------------
EXP_OPTION X509* ddocSigInfo_GetOCSPRespondersCert(const SignatureInfo* pSigInfo);

//--------------------------------------------------
// Sets the OCSP Responders certificate
// pSigInfo - signature info object [REQUIRED]
// pCert - certificate [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocSigInfo_SetOCSPRespondersCert(SignatureInfo* pSigInfo, X509* pCert);

//============================================================
// Adds a certificate and it's certid to this signature
// pSigInfo - signature info object [REQUIRED]
// pCert - vertificate [REQUIRED]
// nCertIdType - type of cert [REQUIRED]
// return error code or ERR_OK
//============================================================
EXP_OPTION int ddocSigInfo_addCert(SignatureInfo* pSigInfo, X509* pCert, int nCertIdType);


#ifdef WITH_TS

//--------------------------------------------------
// Finds the TSA certificate
// pSigInfo - signature info object [REQUIRED]
// returns certificate or NULL
//--------------------------------------------------
EXP_OPTION X509* ddocSigInfo_GetTSACert(const SignatureInfo* pSigInfo);

//--------------------------------------------------
// Sets the TSA certificate
// pSigInfo - signature info object [REQUIRED]
// pTsInfo - timestamp object [REQUIRED]
// pCert - certificate [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocSigInfo_SetTSACert(SignatureInfo* pSigInfo, X509* pCert);

//--------------------------------------------------
// Finds a TimestampInfo object with required type
// pSigInfo - signature info object [REQUIRED]
// nType - type of TimestampInfo object [REQUIRED]
// returns TimestampInfo pointer or NULL for error
//--------------------------------------------------
EXP_OPTION TimestampInfo* ddocSigInfo_GetTypestampOfType(const SignatureInfo* pSigInfo, const char* szType);

//--------------------------------------------------
// Finds a TimestampInfo object with required type
// pSigInfo - signature info object [REQUIRED]
// nType - type of TimestampInfo object [REQUIRED]
// returns TimestampInfo pointer or NULL for error
//--------------------------------------------------
EXP_OPTION TimestampInfo* ddocSigInfo_GetOrCreateTypestampOfType(SignatureInfo* pSigInfo, const char* szType);

//--------------------------------------------------
// Finds last TimestampInfo object
// pSigInfo - signature info object [REQUIRED]
// returns TimestampInfo pointer or NULL for error
//--------------------------------------------------
EXP_OPTION TimestampInfo* ddocSigInfo_GetLastTypestamp(const SignatureInfo* pSigInfo);

//--------------------------------------------------
// Calculates digest of timestampable xml block
// pSigDoc - signed doc pointer [REQUIRED]
// pSigInfo - signature pointer [REQUIRED]
// nTsType - timestamp type [REQUIRED]
// pMBufDigest - buffer for digest [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocCalculateTimestampSourceDigest(const SignedDoc* pSigDoc,
				       const SignatureInfo* pSigInfo, 
				       int nTsType, DigiDocMemBuf* pMBufDigest);

//--------------------------------------------------
// Adds a <SignatureTimeStamp> to signature
// retrieves the TS_RESP object
// pSigInfo - signature pointer [REQUIRED]
// ppCert - TSA certificate, returnes TSA cert [OPTIONAL]
// policy - policy OID [OPTIONAL]
// url - TSA url [REQUIRED]
// proxyHost - http proxy host [OPTIONAL]
// proxyPort - http proxy port [OPTIONAL]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocGetSignatureTimeStamp(SignatureInfo* pSigInfo, X509** ppCert,
			      const char* policy, const char* url, 
			      const char* proxyHost, const char* proxyPort);


//--------------------------------------------------
// Generates <SignatureTimeStamp> element XML
// pSigInfo - signature pointer [REQUIRED]
// pMBuf - buffer for XML [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocSigInfo_SignatureTimeStamp_toXML(const SignatureInfo* pSigInfo, DigiDocMemBuf* pMBuf);

//--------------------------------------------------
// Adds a <SigAndRefsTimeStamp> to signature
// retrieves the TS_RESP object
// pSigInfo - signature pointer [REQUIRED]
// ppCert - TSA certificate [OPTIONAL]
// policy - policy OID [OPTIONAL]
// url - TSA url [REQUIRED]
// proxyHost - http proxy host [OPTIONAL]
// proxyPort - http proxy port [OPTIONAL]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocGetSigAndRefsTimeStamp(const SignedDoc* pSigDoc, 
			       const SignatureInfo* pSigInfo, X509** ppCert,
			       const char* policy, const char* url,
			       const char* proxyHost, const char* proxyPort);

//--------------------------------------------------
// Generates <SignatureTimeStamp> element XML
// pSigInfo - signature pointer [REQUIRED]
// pMBuf - buffer for XML [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocSigInfo_SigAndRefsTimeStamp_toXML(const SignatureInfo* pSigInfo, DigiDocMemBuf* pMBuf);

#endif

// Returns the last SignatureInfo object
EXP_OPTION SignatureInfo* ddocGetLastSignature(const SignedDoc* pSigDoc);
// Returns the SignatureInfo object with the given id
EXP_OPTION SignatureInfo* getSignatureWithId(const SignedDoc* pSigDoc, const char* id);
// Returns the SignatureInfo for the given NotaryInfo
EXP_OPTION SignatureInfo* ddocGetSignatureForNotary(const SignedDoc* pSigDoc, const NotaryInfo* pNotInfo);

// Adds a new SignedInfo element to a SignedDoc element and initializes it
EXP_OPTION int SignatureInfo_new(SignatureInfo **newSignatureInfo, SignedDoc* pSigDoc, const char* id);

// Sets the signature production place info (use NULL for unknown attributes)
EXP_OPTION int setSignatureProductionPlace(SignatureInfo* pSigInfo, 
								const char* city, const char* state,
								const char* zip, const char* country);
// Adds a signer role 
EXP_OPTION int addSignerRole(SignatureInfo* pSigInfo, int nCertified, 
				   const char* role, int rLen, int encode);
// Returns the number of signer roles
EXP_OPTION int getCountOfSignerRoles(SignatureInfo* pSigInfo, int nCertified);
// Returns the desired signer role
EXP_OPTION const char* getSignerRole(SignatureInfo* pSigInfo, int nCertified, int nIdx);

// Removes this SignatureInfo from signed doc and frees it's memory
EXP_OPTION int SignatureInfo_delete(SignedDoc* pSigDoc, const char* id);

// cleanup SignatureInfo memory
EXP_OPTION void SignatureInfo_free(SignatureInfo* pSigInfo);

//======================< DocInfo functions >=============================

// Adds a new DocInfo element to a SignatureInfo element and initializes it
EXP_OPTION int addDocInfo(DocInfo **newDocInfo, SignatureInfo* pSigInfo, const char* docId,
			   const char* digType, const byte* digest,
			   int digLen, const byte* mimeDig, int mimeDigLen);
// cleanup DocInfo memory
EXP_OPTION void DocInfo_free(DocInfo* pDocInfo);
// Returns number of DocInfos
EXP_OPTION int getCountOfDocInfos(const SignatureInfo* pSigInfo);
// Returns the desired DocInfo
EXP_OPTION DocInfo* getDocInfo(const SignatureInfo* pSigInfo, int idx);
// Returns the last DocInfo
EXP_OPTION DocInfo* ddocGetLastDocInfo(const SignatureInfo* pSigInfo);


// Returns the DocInfo object with the given id
EXP_OPTION DocInfo* getDocInfoWithId(const SignatureInfo* pSigInfo, const char* id);
// Sets the DocInfo objects document digest and digest type
EXP_OPTION void setDocInfoDigest(DocInfo* pDocInfo, const byte* digest, 
					  int digLen, const char* digType);
// Sets the DocInfo objects mime digest and mime type
EXP_OPTION void setDocInfoMimeDigest(DocInfo* pDocInfo, const byte* mimeDig, int mimeDigLen);

// Adds all DocInfo elements in this file to a SignatureInfo element
EXP_OPTION int addAllDocInfos(SignedDoc* pSigDoc, SignatureInfo* pSigInfo);

//======================< NotaryInfo functions >=============================

// returns the number of notarys
EXP_OPTION int getCountOfNotaryInfos(const SignedDoc* pSigDoc);
// Returns the desired NotaryInfo object
EXP_OPTION NotaryInfo* getNotaryInfo(const SignedDoc* pSigDoc, int nIdx);
// Returns the last NotaryInfo object
EXP_OPTION NotaryInfo* ddocGetLastNotaryInfo(const SignedDoc* pSigDoc);
// Returns the NotaryInfo object with the given id
EXP_OPTION NotaryInfo* getNotaryWithId(const SignedDoc* pSigDoc, const char* id);
// Returns the NotaryInfo object that corresponds to the given signature
EXP_OPTION NotaryInfo* getNotaryWithSigId(const SignedDoc* pSigDoc, const char* sigId);
// Returns the NotaryInfo object that corresponds to the given signature
// ore creates a new one
EXP_OPTION NotaryInfo* getOrCreateNotaryWithSigId(SignedDoc* pSigDoc, const char* sigId);

// Adds a new NotaryInfo element to a SignedDoc element and initializes it partly
EXP_OPTION int NotaryInfo_new(NotaryInfo** newNotaryInfo, SignedDoc* pSigDoc, SignatureInfo* pSigInfo);
// the same as above, but reads response and cert from file
EXP_OPTION int NotaryInfo_new_file(NotaryInfo**  newNotaryInfo, SignedDoc* pSigDoc, const SignatureInfo* pSigInfo, 
						   const char* ocspRespFile, const char* notaryCertFile);
// cleanup NotaryInfo memory
EXP_OPTION void NotaryInfo_free(NotaryInfo* pNotary);

//============================================================
// Returns OCSP responders id as in XML document
// pNotary - Notary info
// return DigiDocMemBuf buffer pointer or NULL for error
//============================================================
EXP_OPTION const DigiDocMemBuf* ddocNotInfo_GetResponderId(const NotaryInfo* pNotary);

//============================================================
// Returns OCSP responders id value as string
// pNotary - Notary info
// return responder id value or NULL
//============================================================
EXP_OPTION const char* ddocNotInfo_GetResponderId_Value(const NotaryInfo* pNotary);

//============================================================
// Sets OCSP responders id as in XML document
// pNotary - Notary info
// data - new responder id value
// len - length of value
// return DigiDocMemBuf buffer pointer or NULL for error
//============================================================
int ddocNotInfo_SetResponderId(NotaryInfo* pNotary, const char* data, long len);

//============================================================
// Returns OCSP response as memory buffer
// pNotary - Notary info
// return DigiDocMemBuf buffer pointer or NULL for error
//============================================================
const DigiDocMemBuf* ddocNotInfo_GetOCSPResponse(const NotaryInfo* pNotary);

//============================================================
// Retrieves OCSP responses responder id type and value
// pResp - OCSP response
// pType - buffer for type
// pMbufRespId - responder id
// returns error code or ERR_OK
//============================================================
int ddocGetOcspRespIdTypeAndValue(OCSP_RESPONSE* pResp, 
								  int *pType, DigiDocMemBuf* pMbufRespId);

//============================================================
// Sets OCSP respondese value as in XML document. Must pass in
// binary DER data!
// pNotary - Notary info
// data - new responder id value
// len - length of value
// return DigiDocMemBuf buffer pointer or NULL for error
//============================================================
int ddocNotInfo_SetOCSPResponse(NotaryInfo* pNotary, const char* data, long len);

//============================================================
// Returns OCSP response value
// pNotary - Notary info
// return OCSP_RESPONSE pointer or NULL for error. Caller must
//    use OCSP_RESPONSE_free() to release it.
//============================================================
OCSP_RESPONSE* ddocNotInfo_GetOCSPResponse_Value(const NotaryInfo* pNotary);

//============================================================
// Sets OCSP respondese value. Must pass in real OCSP_RESPONSE
// pNotary - Notary info
// data - new responder id value
// len - length of value
// return DigiDocMemBuf buffer pointer or NULL for error
//============================================================
int ddocNotInfo_SetOCSPResponse_Value(NotaryInfo* pNotary, OCSP_RESPONSE* pResp);

//============================================================
// Returns OCSP responders id type as string
// pNotary - Notary info
// return responder id type or NULL. DO NOT free() it!
//============================================================
EXP_OPTION const char* ddocNotInfo_GetResponderId_Type(const NotaryInfo* pNotary);

//============================================================
// Returns OCSP responses thisUpdate atribute as string
// pNotary - Notary info
// pMBuf - buffer for thisUpdate value
// return error code OR ERR_OK.
//============================================================
EXP_OPTION int ddocNotInfo_GetThisUpdate(const NotaryInfo* pNotary, DigiDocMemBuf* pMBuf);

//============================================================
// Returns OCSP responses producedAt atribute as time_t
// pNotary - Notary info
// pTime - address of time_t variable
// return error code OR ERR_OK.
//============================================================
int ddocNotInfo_GetProducedAt_timet(const NotaryInfo* pNotary, time_t* pTime);

//============================================================
// Returns OCSP responses thisUpdate atribute as time_t
// pNotary - Notary info
// pTime - address of time_t variable
// return error code OR ERR_OK.
//============================================================
int ddocNotInfo_GetThisUpdate_timet(const NotaryInfo* pNotary, time_t* pTime);

//============================================================
// Returns OCSP responses nextUpdate atribute as string
// pNotary - Notary info
// pMBuf - buffer for thisUpdate value
// return error code OR ERR_OK.
//============================================================
EXP_OPTION int ddocNotInfo_GetNextUpdate(const NotaryInfo* pNotary, DigiDocMemBuf* pMBuf);

//============================================================
// Returns OCSP responses IssuerNameHash atribute
// pNotary - Notary info
// pMBuf - buffer for IssuerNameHash value
// return error code OR ERR_OK.
//============================================================
int ddocNotInfo_GetIssuerNameHash(const NotaryInfo* pNotary, DigiDocMemBuf* pMBuf);

//============================================================
// Returns OCSP responses IssuerKeyHash atribute
// pNotary - Notary info
// pMBuf - buffer for IssuerKeyHash value
// return error code OR ERR_OK.
//============================================================
int ddocNotInfo_GetIssuerKeyHash(const NotaryInfo* pNotary, DigiDocMemBuf* pMBuf);

//============================================================
// Returns OCSP responses real digest from response data
// pNotary - Notary info
// pMBuf - buffer for digest value
// return error code OR ERR_OK.
//============================================================
int ddocNotInfo_GetOcspRealDigest(const NotaryInfo* pNotary, DigiDocMemBuf* pMBuf);

//============================================================
// Returns OCSP response digest as in XML document
// pNotary - Notary info
// return DigiDocMemBuf buffer pointer or NULL for error
//============================================================
EXP_OPTION const DigiDocMemBuf* ddocNotInfo_GetOcspDigest(const NotaryInfo* pNotary);

//============================================================
// Sets OCSP response digest id as in XML document
// pNotary - Notary info
// data - new digest value
// len - length of value
// return DigiDocMemBuf buffer pointer or NULL for error
//============================================================
int ddocNotInfo_SetOcspDigest(NotaryInfo* pNotary, const char* data, long len);

//============================================================
// Returns OCSP responses signature value
// pNotary - Notary info
// pMBuf - buffer for signature value
// return error code OR ERR_OK.
//============================================================
int ddocNotInfo_GetOcspSignatureValue(const NotaryInfo* pNotary, DigiDocMemBuf* pMBuf);


// Removes this NotaryInfo from signed doc and frees it's memory
EXP_OPTION int NotaryInfo_delete(SignatureInfo* pSigInfo);

// Calculates and stores a signature for this SignatureInfo object
EXP_OPTION int calculateSigInfoSignature(const SignedDoc* pSigDoc, SignatureInfo* pSigInfo, int nSigType, 
			const char* keyfile, const char* passwd, const char* certfile);

//============================================================
// Adds a certificate to Notary and initializes Notary
// pNotary - Notary info
// cert - responders certificate
// return error code
//============================================================
int addNotaryInfoCert(SignedDoc *pSigDoc, NotaryInfo *pNotary, X509 *cert);

//============================================================
// Removes Notary cert value and id after unsucessful verification attempt
// pSigInfo - signature info [REQUIRED]
// return error code
//============================================================
int removeNotaryInfoCert(SignatureInfo* pSigInfo);

// Calculates <SignedProperties> digest
EXP_OPTION int calculateSignedPropertiesDigest(SignedDoc* pSigDoc, SignatureInfo* pSigInfo);
// Calculates <SignedInfo> digest
EXP_OPTION int calculateSignedInfoDigest(SignedDoc* pSigDoc, SignatureInfo* pSigInfo, byte* digBuf, int* digLen);


#ifdef  __cplusplus
}
#endif


#endif    // __DIGIDOC_OBJ_H__


