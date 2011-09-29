#ifndef __DIGIDOC_TS_H__
#define __DIGIDOC_TS_H__
//==================================================
// FILE:	DigiDocTS.h
// PROJECT:     Digi Doc Encryption
// DESCRIPTION: DigiDoc timestamping routines
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
//      10.04.2006      Veiko Sinivee
//                      Creation
//==================================================

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocMem.h>
//#include <libdigidoc/DigiDocObj.h>

#ifdef  __cplusplus
extern "C" {
#endif

#include <openssl/ts.h>

//==========< glocal constants >=============

#define TS_TYPE_ALL_DATA_OBJECTS               "AllDataObjectsTimeStamp"
#define TS_TYPE_INDIVIDUAL_DATA_OBJECTS        "IndividualDataObjectsTimeStamp"
#define TS_TYPE_SIGNATURE                      "SignatureTimeStamp"
#define TS_TYPE_SIG_AND_REFS                   "SigAndRefsTimeStamp"
#define TS_TYPE_REFS_ONLY                      "RefsOnlyTimeStamp"
#define TS_TYPE_ARCHIVE                        "ArchiveTimeStamp"

#define ID_TS_TYPE_ALL_DATA_OBJECTS            1
#define ID_TS_TYPE_INDIVIDUAL_DATA_OBJECTS     2
#define ID_TS_TYPE_SIGNATURE                   3
#define ID_TS_TYPE_SIG_AND_REFS                4
#define ID_TS_TYPE_REFS_ONLY                   5
#define ID_TS_TYPE_ARCHIVE                     6

//==========< structures >===================

typedef struct IncludeInfo_st {
  char* szURI;          // URI atribute value
  int   referencedData; // referencedData atribute value
  // no Transforms used currently
} IncludeInfo;

typedef struct TimestampInfo_st {
  char* szId;          // Id atribute value if present
  char* szType;        // Type atribute value if present
  char* szCanonicalizationMethod; // CanonicalizationMethod element
  DigiDocMemBuf mbufTS; // timestamp data
  DigiDocMemBuf mbufRealDigest; // digest calculated over xml as read from file
  // list of IncludeInfo-s
  int nIncludeInfos;
  IncludeInfo** pIncludeInfos;
} TimestampInfo;

typedef struct TimestampInfoList_st {
  int nTimestampInfos;
  TimestampInfo** pTimestampInfos;
} TimestampInfoList;

//==========< IncludeInfo >========================

//--------------------------------------------------
// "Constructor" of IncludeInfo object
// pTimestampInfo - parent object [REQUIRED]
// ppIncludeInfo - address of buffer for newly allocated object [REQUIRED]
// szURI - hashed object uri [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocIncludeInfo_new(TimestampInfo* pTimestampInfo, 
				    IncludeInfo** ppIncludeInfo, const char* szURI);

//--------------------------------------------------
// "Destructor" of IncludeInfo object
// pIncludeInfo - address of object to be deleted [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocIncludeInfo_free(IncludeInfo* pIncludeInfo);

//--------------------------------------------------
// Accessor for URI atribute of IncludeInfo object.
// pIncludeInfo - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* ddocIncludeInfo_GetURI(const IncludeInfo* pIncludeInfo);

//--------------------------------------------------
// Mutatoror for Id atribute of IncludeInfo object.
// pIncludeInfo - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocIncludeInfo_SetURI(IncludeInfo* pIncludeInfo, const char* value);

//==========< TimestampInfo >========================

//--------------------------------------------------
// "Constructor" of TimestampInfo object
// pTimestampInfoList - parent object [REQUIRED]
// ppTimestampInfo - address of buffer for newly allocated object [REQUIRED]
// szId - timestamp id [OPTIONAL]
// szType - timestamp type [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_new(TimestampInfoList* pTimestampInfoList,
				     TimestampInfo** ppTimestampInfo, 
				     const char* szId, const char* szType);

//--------------------------------------------------
// "Destructor" of TimestampInfo object
// pTimestampInfo - address of object to be deleted [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_free(TimestampInfo* pTimestampInfo);

//--------------------------------------------------
// Accessor for Id atribute of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* ddocTimestampInfo_GetId(const TimestampInfo* pTimestampInfo);

//--------------------------------------------------
// Mutatoror for Id atribute of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_SetId(TimestampInfo* pTimestampInfo, const char* value);

//--------------------------------------------------
// Accessor for Type atribute of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* ddocTimestampInfo_GetType(const TimestampInfo* pTimestampInfo);

//--------------------------------------------------
// Mutatoror for Type atribute of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_SetType(TimestampInfo* pTimestampInfo, const char* value);

//--------------------------------------------------
// Accessor for Timestamp data of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const DigiDocMemBuf* ddocTimestampInfo_GetTS(const TimestampInfo* pTimestampInfo);

//--------------------------------------------------
// Mutatoror for Timestamp data of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// len - length of value in bytes [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_SetTS(TimestampInfo* pTimestampInfo, 
				       const char* value, long len);

//--------------------------------------------------
// Accessor for real digest value of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const DigiDocMemBuf* ddocTimestampInfo_GetRealDigest(const TimestampInfo* pTimestampInfo);

//--------------------------------------------------
// Mutatoror for real digest data of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// len - length of value in bytes [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_SetRealDigest(TimestampInfo* pTimestampInfo, 
				       const char* value, long len);

//--------------------------------------------------
// Accessor for count of IncludeInfos subelement of TimestampInfo object.
// pTimestampInfo - pointer to TimestampInfo object [REQUIRED]
// returns count or -1 for error. Then use error API to check errors
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_GetIncludeInfosCount(const TimestampInfo* pTimestampInfo);

//--------------------------------------------------
// Accessor for IncludeInfos subelement of TimestampInfo object.
// pTimestampInfo - pointer to TimestampInfo object [REQUIRED]
// nIdx - index of IncludeInfo object [REQUIRED]
// returns IncludeInfo pointer or NULL for error
//--------------------------------------------------
EXP_OPTION IncludeInfo* ddocTimestampInfo_GetIncludeInfo(const TimestampInfo* pTimestampInfo, int nIdx);

//--------------------------------------------------
// Accessor for last IncludeInfos subelement of TimestampInfo object.
// pTimestampInfo - pointer to TimestampInfo object [REQUIRED]
// returns IncludeInfo pointer or NULL for error
//--------------------------------------------------
EXP_OPTION IncludeInfo* ddocTimestampInfo_GetLastIncludeInfo(const TimestampInfo* pTimestampInfo);

//--------------------------------------------------
// Deletes IncludeInfo subelement of TimestampInfo object.
// pTimestampInfo - pointer to TimestampInfo object [REQUIRED]
// nIdx - index of IncludeInfo object to be removed [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_DeleteIncludeInfo(TimestampInfo* pTimestampInfo, int nIdx);

//--------------------------------------------------
// Adds an IncludeInfo subelement to TimestampInfo object.
// pTimestampInfo - pointer to TimestampInfo object [REQUIRED]
// szURI - URI atribute of IncludeInfo object to be added [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_AddIncludeInfo(TimestampInfo* pTimestampInfo, const char* szURI);

//--------------------------------------------------
// Generates XML for <TimestampInfo> element
// pTimestampInfo - TimestampInfo object [REQUIRED]
// pBuf - memory buffer for storing xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTimestampInfo_toXML(const TimestampInfo* pTimestampInfo, DigiDocMemBuf* pBuf);

//==========< TimestampInfoList >====================

//--------------------------------------------------
// "Constructor" of TimestampInfoList object
// ppTimestampInfoList - address of buffer for newly allocated object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfoList_new(TimestampInfoList** ppTimestampInfoList);

//--------------------------------------------------
// "Destructor" of TimestampInfoList object
// pTimestampInfoList - address of object to be deleted [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfoList_free(TimestampInfoList* pTimestampInfoList);

//--------------------------------------------------
// Accessor for count of TimestampInfos subelement of TimestampInfoList object.
// pTimestampInfoList - pointer to TimestampInfoList object [REQUIRED]
// returns count or -1 for error. Then use error API to check errors
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfoList_GetTimestampInfosCount(const TimestampInfoList* pTimestampInfoList);

//--------------------------------------------------
// Accessor for TimestampInfos subelement of TimestampInfoList object.
// pTimestampInfoList - pointer to TimestampInfoList object [REQUIRED]
// nIdx - index of TimestampInfo object [REQUIRED]
// returns TimestampInfo pointer or NULL for error
//--------------------------------------------------
EXP_OPTION TimestampInfo* ddocTimestampInfoList_GetTimestampInfo(const TimestampInfoList* pTimestampInfoList, int nIdx);

//--------------------------------------------------
// Deletes TimestampInfo subelement of TimestampInfoList object.
// pTimestampInfoList - pointer to TimestampInfoList object [REQUIRED]
// nIdx - index of TimestampInfo object to be removed [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfoList_DeleteTimestampInfo(TimestampInfoList* pTimestampInfoList, int nIdx);

//--------------------------------------------------
// Returns the TimestampInfo with required type
// pTimestampInfoList - pointer to TimestampInfoList object [REQUIRED]
// szType - type of TimestampInfo object [REQUIRED]
// returns TimestampInfo pointer or NULL for error
//--------------------------------------------------
EXP_OPTION TimestampInfo* ddocTimestampInfoList_GetTimestampInfoOfType(const TimestampInfoList* pTimestampInfoList, const char* szType);

//--------------------------------------------------
// Returns the TimestampInfo with required type.
// Creates one if it doesn't exist yet.
// pTimestampInfoList - pointer to TimestampInfoList object [REQUIRED]
// szType - type of TimestampInfo object [REQUIRED]
// returns TimestampInfo pointer or NULL for error
//--------------------------------------------------
  EXP_OPTION TimestampInfo* ddocTimestampInfoList_GetOrCreateTimestampInfoOfType(TimestampInfoList* pTimestampInfoList, const char* szId, const char* szType);



//==========< general functions >============

//--------------------------------------------------
// Helper function to create a new TS_REQ from XML data objects
// that will be digested. Stores the result in PEM form
// in a DigiDocMemBuf object.
// pMBufReq - memory buffer to store new request in PEM format
// pMBufDigest - digest data to timestamp (xml)
// policy - oilicy oid in stringform. Use NULL if not necessary
// bNonce - 1=send nonce, 0=no nonce
// bCert - request req_cert property 1/0
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTsCreateTsReqBin(DigiDocMemBuf* pMBufReq, DigiDocMemBuf* pMBufDigest, 
			 const char* policy, int bNonce, int bCert);

//--------------------------------------------------
// sends a Timestamp request to TSA server 
// retrieves the TS_RESP object
// pMBufDigest - digest of data to be timestamped [REQUIRED]
// ppResp - address for TS_RESP [REQUIRED]
// policy - policy OID [OPTIONAL]
// bNonce - flag use nonce: 1/0 [REQUIRED]
// bCert - flag send cert: 1/0 [REQUIRED]
// url - TSA url [REQUIRED]
// proxyHost - http proxy host [OPTIONAL]
// proxyPort - http proxy port [OPTIONAL]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocSendTSRequest(DigiDocMemBuf* pMBufDigest, TS_RESP** ppResp,
		      const char* policy, int bNonce, int bCert, const char* url, 
		      const char* proxyHost, const char* proxyPort);


//==========< utility functions >========================

//--------------------------------------------------
// Helper function to read TS_REQ from binary input data
// ppReq - address of newly allocated TS_REQ object
// pMBufInData - input data
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTsReadTsReq(TS_REQ** ppReq, DigiDocMemBuf* pMBufInData);

//--------------------------------------------------
// Helper function to write TS_REQ to binary output data
// pReq - address of TS_REQ object
// pMBufOutData - output data
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTsWriteTsReq(TS_REQ* pReq, DigiDocMemBuf* pMBufOutData);

//--------------------------------------------------
// Helper function to read TS_RESP from binary input data
// ppResp - address of newly allocated TS_RESP object
// pMBufInData - input data
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTsReadTsResp(TS_RESP** ppResp, DigiDocMemBuf* pMBufInData);

//--------------------------------------------------
// Helper function to write TS_RESP to binary output data
// pResp - address of TS_RESP object
// pMBufOutData - output data
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTsWriteTsResp(TS_RESP* pResp, DigiDocMemBuf* pMBufOutData);

//--------------------------------------------------
// Returns TS_RESP policy id.
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBufDigest - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTSResp_GetPolicyId(TS_RESP* pResp, DigiDocMemBuf* pMBuf);

//--------------------------------------------------
// Returns TS_RESP status code as string
// pResp - TS_RESP timestamp response [REQUIRED]
// pStatus - address of variable for status [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTSResp_GetStatus(TS_RESP* pResp, int* pStatus);

//--------------------------------------------------
// Returns TS_RESP serial number as a string
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBufDigest - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTSResp_GetSerial(TS_RESP* pResp, DigiDocMemBuf* pMBuf);

//--------------------------------------------------
// Returns TS_RESP nonce as string
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBufDigest - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTSResp_GetNonce(TS_RESP* pResp, DigiDocMemBuf* pMBuf);

//--------------------------------------------------
// Returns TS_RESP time as string
// pResp - TS_RESP timestamp response [REQUIRED]
// pTime - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTSResp_GetTime(TS_RESP* pResp, time_t* pTime);

//--------------------------------------------------
// Returns TS_RESP tsa name as string
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBufDigest - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTSResp_GetTsaName(TS_RESP* pResp, DigiDocMemBuf* pMBuf);

//--------------------------------------------------
// Returns TS_RESP message imprint as binary data
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBuf - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTSResp_GetMsgImprint(TS_RESP* pResp, DigiDocMemBuf* pMBuf);

//--------------------------------------------------
// Returns TS_RESP message imprint algoritm as string
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBuf - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTSResp_GetMsgImprint_Algoritm(TS_RESP* pResp, DigiDocMemBuf* pMBuf);

//--------------------------------------------------
// Verifies a Timestamp response to TSA.
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBufDigest - digest of timestamped data [REQUIRED]
// pCACerts - CA certs array [REQUIRED]
// szCAPath - path to dir with CA certs [OPTIONAL]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocVerifyTSResponse(TS_RESP* pResp, const DigiDocMemBuf* pMBufDigest, 
			 X509** pCACerts, char* szCAPath);

//--------------------------------------------------
// Returns TS_RESP signers (e.g. TSA) cert serial.
// Assumes that there's only 1 signer
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBuf - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTsResp_GetSignerCertSerial(TS_RESP* pResp, DigiDocMemBuf* pMBuf);

//--------------------------------------------------
// Returns TS_RESP signers (e.g. TSA) cert issuer DN.
// Assumes that there's only 1 signer
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBuf - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTsResp_GetSignerCertIssuer(TS_RESP* pResp, DigiDocMemBuf* pMBuf);

//--------------------------------------------------
// Returns TS_RESP signers (e.g. TSA) cert.
// Assumes that there's only 1 signer
// pResp - TS_RESP timestamp response [REQUIRED]
// ppCert - address of signers cert pointer [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTsResp_GetSignerCert(TS_RESP* pResp, X509** ppCert);

//--------------------------------------------------
// Verifies a TimestampInfo
// pTS - TimestampInfo object [REQUIRED]
// pCACerts - CA certs array [REQUIRED]
// szCAPath - path to dir with CA certs [OPTIONAL]
// pTime - address of time_t to store timestamping time [OPTIONAL]
// ppTsaCert - address of TSA cert to return it[OPTIONAL]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocVerifyTimestampInfo(TimestampInfo *pTS, 
			    X509** pCACerts, char* szCAPath, 
				time_t *pTime, X509** ppTsaCert);

int ddocTsResp_GetSignerCert(TS_RESP* pResp, X509** ppCert);

#ifdef  __cplusplus
}
#endif


#endif // __DIGIDOC_TS_H__

