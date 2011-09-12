//==================================================
// FILE:	DigiDocTS.c
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

#include <libdigidoc/ts/DigiDocTS.h>
#include <libdigidoc/DigiDocError.h>
#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocGen.h>
#include <libdigidoc/DigiDocConvert.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocCert.h>

//#include <openssl/bio.h>
//#include <openssl/err.h>
//#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/ts.h>

#include <string.h>

#define	NONCE_LENGTH		64
#define ERR_LIB_NONE            1

//==========< validation >========================


//--------------------------------------------------
// Validates the timestamp type.
// szType - value to be checked
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTsValidateTimestampType(const char* szType)
{
  if(!szType || 
     (strcmp(szType, TS_TYPE_ALL_DATA_OBJECTS) &&
      strcmp(szType, TS_TYPE_INDIVIDUAL_DATA_OBJECTS) &&
      strcmp(szType, TS_TYPE_SIGNATURE) &&
      strcmp(szType, TS_TYPE_SIG_AND_REFS) &&
      strcmp(szType, TS_TYPE_REFS_ONLY) &&
      strcmp(szType, TS_TYPE_ARCHIVE)))
    SET_LAST_ERROR_RETURN(ERR_TS_TIMESTAMPINFO_TYPE, ERR_TS_TIMESTAMPINFO_TYPE)
  else
    return ERR_OK;
}

//==========< IncludeInfo >========================

//--------------------------------------------------
// "Constructor" of IncludeInfo object
// pTimestampInfo - parent object [REQUIRED]
// ppIncludeInfo - address of buffer for newly allocated object [REQUIRED]
// szURI - hashed object uri [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocIncludeInfo_new(TimestampInfo* pTimestampInfo, 
				    IncludeInfo** ppIncludeInfo, const char* szURI)
{
  int err = ERR_OK, nHashes;
  IncludeInfo **ppHashes, *pHash;

  // check input parameters
  ddocDebug(3, "ddocIncludeInfo_new", "URI: %s", (szURI ? szURI : "NULL"));
  RETURN_IF_NULL_PARAM(pTimestampInfo);
  RETURN_IF_NULL_PARAM(ppIncludeInfo);
  RETURN_IF_NULL_PARAM(szURI);

  *ppIncludeInfo = 0; // mark as not yet allocated
  // allocate memory for pointer array
  nHashes = pTimestampInfo->nIncludeInfos + 1;
  ppHashes = (IncludeInfo **)realloc(pTimestampInfo->pIncludeInfos, 
				      sizeof(IncludeInfo *) * nHashes);
  if(!ppHashes)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
  pTimestampInfo->pIncludeInfos = ppHashes;
  ppHashes[pTimestampInfo->nIncludeInfos] = 0;
  // allocate memory for new property
  pHash = (IncludeInfo*)malloc(sizeof(IncludeInfo));
  if(!pHash)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
  memset(pHash, 0, sizeof(IncludeInfo));
  ppHashes[pTimestampInfo->nIncludeInfos] = pHash;
  *ppIncludeInfo = pHash;
  pTimestampInfo->nIncludeInfos = nHashes;
  // set required fields
  if(szURI) {
    err = ddocMemAssignString((char**)&((*ppIncludeInfo)->szURI), szURI);
    if(err) return err;
  }
  return err;
}

//--------------------------------------------------
// "Destructor" of IncludeInfo object
// pIncludeInfo - address of object to be deleted [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocIncludeInfo_free(IncludeInfo* pIncludeInfo)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pIncludeInfo)
  // cleanup this object
  if(pIncludeInfo->szURI)
    free(pIncludeInfo->szURI);
  free(pIncludeInfo);
  return err;
}

//--------------------------------------------------
// Accessor for URI atribute of IncludeInfo object.
// pIncludeInfo - address of object to be deleted [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* ddocIncludeInfo_GetURI(const IncludeInfo* pIncludeInfo)
{
  RETURN_OBJ_IF_NULL(pIncludeInfo, NULL)
  return pIncludeInfo->szURI;
}

//--------------------------------------------------
// Mutatoror for Id atribute of IncludeInfo object.
// pIncludeInfo - address of object to be deleted [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocIncludeInfo_SetURI(IncludeInfo* pIncludeInfo, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pIncludeInfo)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignString((char**)&(pIncludeInfo->szURI), value);
  return err;
}

//--------------------------------------------------
// Generates XML for <IncludeInfo> element
// pIncludeInfo - IncludeInfo object [REQUIRED]
// pBuf - memory buffer for storing xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocIncludeInfo_toXML(const IncludeInfo* pIncludeInfo, DigiDocMemBuf* pBuf)
{
  int err = ERR_OK;
  char* p;

  RETURN_IF_NULL_PARAM(pBuf)
  RETURN_IF_NULL_PARAM(pIncludeInfo)
  // start of element
  err = ddocGen_startElemBegin(pBuf, "Include");
  if(err) return err;
  // uri atribute
  p = (char*)ddocIncludeInfo_GetURI(pIncludeInfo);
  if(p)
    err = ddocGen_addAtribute(pBuf, "URI", p);
  if(err) return err;
  // referencedData
  if(pIncludeInfo->referencedData)
    err = ddocGen_addAtribute(pBuf, "referencedData", "TRUE");
  if(err) return err;
  // end of element start tag
  err = ddocGen_startElemEnd(pBuf);
  if(err) return err;
  err = ddocGen_endElem(pBuf, "Include");
  return err;
}

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
				     const char* szId, const char* szType)
{
  int err = ERR_OK, nInfos;
  TimestampInfo **ppInfos, *pInfo;

  // check input parameters
  ddocDebug(3, "ddocTimestampInfo_new", "ID: %s: Type: %s", 
	    (szId ? szId : "NULL"), (szType ? szType : "NULL"));
  RETURN_IF_NULL_PARAM(pTimestampInfoList);
  RETURN_IF_NULL_PARAM(ppTimestampInfo);
  err = ddocTsValidateTimestampType(szType);
  if(err) return err;
  *ppTimestampInfo = 0; // mark as not yet allocated
  // allocate memory for pointer array
  nInfos = pTimestampInfoList->nTimestampInfos + 1;
  ppInfos = (TimestampInfo **)realloc(pTimestampInfoList->pTimestampInfos, 
				      sizeof(TimestampInfo *) * nInfos);
  if(!ppInfos)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
  pTimestampInfoList->pTimestampInfos = ppInfos;
  ppInfos[pTimestampInfoList->nTimestampInfos] = 0;
  // allocate memory for new property
  pInfo = (TimestampInfo*)malloc(sizeof(TimestampInfo));
  if(!pInfo)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
  memset(pInfo, 0, sizeof(TimestampInfo));
  ppInfos[pTimestampInfoList->nTimestampInfos] = pInfo;
  *ppTimestampInfo = pInfo;
  pTimestampInfoList->nTimestampInfos = nInfos;
  // set required fields
  if(szType) {
    err = ddocMemAssignString((char**)&((*ppTimestampInfo)->szType), szType);
    if(err) return err;
  }
  // set optional fields
  if(szId) {
    err = ddocMemAssignString((char**)&((*ppTimestampInfo)->szId), szId);
    if(err) return err;
  } 
  return err;
}

//--------------------------------------------------
// "Destructor" of TimestampInfo object
// pTimestampInfo - address of object to be deleted [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_free(TimestampInfo* pTimestampInfo)
{
  int i, err = ERR_OK;
  RETURN_IF_NULL_PARAM(pTimestampInfo)
  // cleanup this object
  if(pTimestampInfo->szId)
    free(pTimestampInfo->szId);
  if(pTimestampInfo->szType)
    free(pTimestampInfo->szType);
  ddocMemBuf_free(&(pTimestampInfo->mbufTS));
  ddocMemBuf_free(&(pTimestampInfo->mbufRealDigest));
  // free hash-data-infos
  for(i = 0; i < pTimestampInfo->nIncludeInfos; i++) {
    if(pTimestampInfo->pIncludeInfos[i]) {
      err = ddocIncludeInfo_free(pTimestampInfo->pIncludeInfos[i]);
      if(err) return err;
    }
  }
  free(pTimestampInfo->pIncludeInfos);
  free(pTimestampInfo);
  return err;
}

//--------------------------------------------------
// Accessor for Id atribute of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* ddocTimestampInfo_GetId(const TimestampInfo* pTimestampInfo)
{
  RETURN_OBJ_IF_NULL(pTimestampInfo, NULL)
  return pTimestampInfo->szId;
}

//--------------------------------------------------
// Mutatoror for Id atribute of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_SetId(TimestampInfo* pTimestampInfo, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pTimestampInfo)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignString((char**)&(pTimestampInfo->szId), value);
  return err;
}

//--------------------------------------------------
// Accessor for Type atribute of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* ddocTimestampInfo_GetType(const TimestampInfo* pTimestampInfo)
{
  RETURN_OBJ_IF_NULL(pTimestampInfo, NULL)
  return pTimestampInfo->szType;
}

//--------------------------------------------------
// Mutatoror for Type atribute of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_SetType(TimestampInfo* pTimestampInfo, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pTimestampInfo)
  RETURN_IF_NULL_PARAM(value)
  err = ddocTsValidateTimestampType(value);
  if(err) return err;
  err = ddocMemAssignString((char**)&(pTimestampInfo->szType), value);
  return err;
}

//--------------------------------------------------
// Accessor for Timestamp data of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const DigiDocMemBuf* ddocTimestampInfo_GetTS(const TimestampInfo* pTimestampInfo)
{
  RETURN_OBJ_IF_NULL(pTimestampInfo, NULL)
  return &(pTimestampInfo->mbufTS);
}

//--------------------------------------------------
// Mutatoror for Timestamp data of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// len - length of value in bytes [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_SetTS(TimestampInfo* pTimestampInfo, 
				       const char* value, long len)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pTimestampInfo)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignData(&(pTimestampInfo->mbufTS), value, len);
  return err;
}

//--------------------------------------------------
// Accessor for real digest value of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const DigiDocMemBuf* ddocTimestampInfo_GetRealDigest(const TimestampInfo* pTimestampInfo)
{
  RETURN_OBJ_IF_NULL(pTimestampInfo, NULL)
  return &(pTimestampInfo->mbufRealDigest);
}

//--------------------------------------------------
// Mutatoror for real digest data of TimestampInfo object.
// pTimestampInfo - address of object [REQUIRED]
// value - new value for atribute [REQUIRED]
// len - length of value in bytes [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_SetRealDigest(TimestampInfo* pTimestampInfo, 
				       const char* value, long len)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pTimestampInfo)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignData(&(pTimestampInfo->mbufRealDigest), value, len);
  return err;
}

//--------------------------------------------------
// Accessor for count of IncludeInfos subelement of TimestampInfo object.
// pTimestampInfo - pointer to TimestampInfo object [REQUIRED]
// returns count or -1 for error. Then use error API to check errors
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_GetIncludeInfosCount(const TimestampInfo* pTimestampInfo)
{
  SET_LAST_ERROR_RETURN_IF_NOT(pTimestampInfo, ERR_NULL_POINTER, -1)
  return pTimestampInfo->nIncludeInfos;
}

//--------------------------------------------------
// Accessor for IncludeInfos subelement of TimestampInfo object.
// pTimestampInfo - pointer to TimestampInfo object [REQUIRED]
// nIdx - index of IncludeInfo object [REQUIRED]
// returns IncludeInfo pointer or NULL for error
//--------------------------------------------------
EXP_OPTION IncludeInfo* ddocTimestampInfo_GetIncludeInfo(const TimestampInfo* pTimestampInfo, int nIdx)
{
  RETURN_OBJ_IF_NULL(pTimestampInfo, NULL)
  SET_LAST_ERROR_RETURN_IF_NOT(nIdx >= 0 && nIdx < pTimestampInfo->nIncludeInfos, ERR_TS_BAD_INCLUDEINFO_IDX, NULL);
  RETURN_OBJ_IF_NULL(pTimestampInfo->pIncludeInfos[nIdx], 0);
  return pTimestampInfo->pIncludeInfos[nIdx];
}

//--------------------------------------------------
// Accessor for last IncludeInfos subelement of TimestampInfo object.
// pTimestampInfo - pointer to TimestampInfo object [REQUIRED]
// returns IncludeInfo pointer or NULL for error
//--------------------------------------------------
EXP_OPTION IncludeInfo* ddocTimestampInfo_GetLastIncludeInfo(const TimestampInfo* pTimestampInfo)
{
  RETURN_OBJ_IF_NULL(pTimestampInfo, NULL)
  RETURN_OBJ_IF_NULL(pTimestampInfo->pIncludeInfos[pTimestampInfo->nIncludeInfos-1], 0);
  return pTimestampInfo->pIncludeInfos[pTimestampInfo->nIncludeInfos-1];
}


//--------------------------------------------------
// Deletes IncludeInfo subelement of TimestampInfo object.
// pTimestampInfo - pointer to TimestampInfo object [REQUIRED]
// nIdx - index of IncludeInfo object to be removed [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_DeleteIncludeInfo(TimestampInfo* pTimestampInfo, int nIdx)
{
  int err = ERR_OK, i;

  RETURN_IF_NULL_PARAM(pTimestampInfo)
  SET_LAST_ERROR_RETURN_IF_NOT(nIdx >= 0 && nIdx < pTimestampInfo->nIncludeInfos, ERR_TS_BAD_INCLUDEINFO_IDX, ERR_TS_BAD_INCLUDEINFO_IDX);
  RETURN_IF_NULL_PARAM(pTimestampInfo->pIncludeInfos[nIdx]);
  // delete the given object
  err = ddocIncludeInfo_free(pTimestampInfo->pIncludeInfos[nIdx]);
  if(err) return err;
  pTimestampInfo->pIncludeInfos[nIdx] = 0;
  // move other objects 1 step close to array start
  for(i = nIdx; i < pTimestampInfo->nIncludeInfos-1; i++) 
    pTimestampInfo->pIncludeInfos[i] = pTimestampInfo->pIncludeInfos[i+1];
  pTimestampInfo->pIncludeInfos[pTimestampInfo->nIncludeInfos - 1] = 0;
  pTimestampInfo->nIncludeInfos--;
  return err;
}

//--------------------------------------------------
// Adds an IncludeInfo subelement to TimestampInfo object.
// pTimestampInfo - pointer to TimestampInfo object [REQUIRED]
// szURI - URI atribute of IncludeInfo object to be added [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfo_AddIncludeInfo(TimestampInfo* pTimestampInfo, const char* szURI)
{
  IncludeInfo *pIncInfo = 0;

  RETURN_IF_NULL_PARAM(pTimestampInfo)
  return ddocIncludeInfo_new(pTimestampInfo, &pIncInfo, szURI);
}


//--------------------------------------------------
// Generates XML for <TimestampInfo> element
// pTimestampInfo - TimestampInfo object [REQUIRED]
// pBuf - memory buffer for storing xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTimestampInfo_toXML(const TimestampInfo* pTimestampInfo, DigiDocMemBuf* pBuf)
{
  int err = ERR_OK, i;
  char* p, *name;
  IncludeInfo* pHash;

  RETURN_IF_NULL_PARAM(pBuf)
  RETURN_IF_NULL_PARAM(pTimestampInfo)
  // start of element
  name = (char*)ddocTimestampInfo_GetType(pTimestampInfo);
  RETURN_IF_NULL_PARAM(name)
  err = ddocGen_startElemBegin(pBuf, name);
  if(err) return err;
  // uri atribute
  p = (char*)ddocTimestampInfo_GetId(pTimestampInfo);
  if(p)
    err = ddocGen_addAtribute(pBuf, "Id", p);
  if(err) return err;
  err = ddocGen_startElemEnd(pBuf);
  if(err) return err;
  // IncludeInfo-s
  for(i = 0; !err && (i < ddocTimestampInfo_GetIncludeInfosCount(pTimestampInfo)); i++) {
    pHash = ddocTimestampInfo_GetIncludeInfo(pTimestampInfo, i);
    err = ddocIncludeInfo_toXML(pHash, pBuf);
  }
  if(err) return err;
  // timestamp data
  i = 2 * pTimestampInfo->mbufTS.nLen;
  p = (char*)malloc(i);
  if(!p)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
  memset(p, 0, i);
  encode((const byte*)pTimestampInfo->mbufTS.pMem, 
	 pTimestampInfo->mbufTS.nLen, (byte*)p, &i);
  if(p && i) {
    err = ddocGen_startElem(pBuf, "EncapsulatedTimeStamp");
    if(err) return err;
    err = ddocMemAppendData(pBuf, p, -1);
    free(p);
    p = 0;
    if(err) return err;
    err = ddocGen_endElem(pBuf, "EncapsulatedTimeStamp");
    if(err) return err;
  }
  err = ddocGen_endElem(pBuf, name);
  return err;
}

//==========< TimestampInfoList >====================

//--------------------------------------------------
// "Constructor" of TimestampInfoList object
// ppTimestampInfoList - address of buffer for newly allocated object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfoList_new(TimestampInfoList** ppTimestampInfoList)
{
  int err = ERR_OK;

  // check input parameters
  ddocDebug(3, "ddocTimestampInfoList_new", "Create new timestamp info list");
  RETURN_IF_NULL_PARAM(ppTimestampInfoList);
  *ppTimestampInfoList = (TimestampInfoList*)malloc(sizeof(TimestampInfoList));
  // allocate new object
  RETURN_IF_BAD_ALLOC(*ppTimestampInfoList);
  memset(*ppTimestampInfoList, 0, sizeof(TimestampInfoList));
  return err;
}

//--------------------------------------------------
// "Destructor" of TimestampInfoList object
// pTimestampInfoList - address of object to be deleted [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfoList_free(TimestampInfoList* pTimestampInfoList)
{
  int i, err = ERR_OK;
  RETURN_IF_NULL_PARAM(pTimestampInfoList)
  // free timestamp-infos
  for(i = 0; i < pTimestampInfoList->nTimestampInfos; i++) {
    if(pTimestampInfoList->pTimestampInfos[i]) {
      err = ddocTimestampInfo_free(pTimestampInfoList->pTimestampInfos[i]);
      if(err) return err;
    }
  }
  free(pTimestampInfoList->pTimestampInfos);
  free(pTimestampInfoList);
  return err;
}

//--------------------------------------------------
// Accessor for count of TimestampInfos subelement of TimestampInfoList object.
// pTimestampInfoList - pointer to TimestampInfoList object [REQUIRED]
// returns count or -1 for error. Then use error API to check errors
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfoList_GetTimestampInfosCount(const TimestampInfoList* pTimestampInfoList)
{
  SET_LAST_ERROR_RETURN_IF_NOT(pTimestampInfoList, ERR_NULL_POINTER, -1)
  return pTimestampInfoList->nTimestampInfos;
}

//--------------------------------------------------
// Accessor for TimestampInfos subelement of TimestampInfoList object.
// pTimestampInfoList - pointer to TimestampInfoList object [REQUIRED]
// nIdx - index of TimestampInfo object [REQUIRED]
// returns TimestampInfo pointer or NULL for error
//--------------------------------------------------
EXP_OPTION TimestampInfo* ddocTimestampInfoList_GetTimestampInfo(const TimestampInfoList* pTimestampInfoList, int nIdx)
{
  RETURN_OBJ_IF_NULL(pTimestampInfoList, NULL)
  SET_LAST_ERROR_RETURN_IF_NOT(nIdx >= 0 && nIdx < pTimestampInfoList->nTimestampInfos, ERR_TS_BAD_TIMESTAMPINFO_IDX, NULL);
  RETURN_OBJ_IF_NULL(pTimestampInfoList->pTimestampInfos[nIdx], 0);
  return pTimestampInfoList->pTimestampInfos[nIdx];
}

//--------------------------------------------------
// Returns the TimestampInfo with required type
// pTimestampInfoList - pointer to TimestampInfoList object [REQUIRED]
// szType - type of TimestampInfo object [REQUIRED]
// returns TimestampInfo pointer or NULL for error
//--------------------------------------------------
EXP_OPTION TimestampInfo* ddocTimestampInfoList_GetTimestampInfoOfType(const TimestampInfoList* pTimestampInfoList, const char* szType)
{
  int i;
  RETURN_OBJ_IF_NULL(pTimestampInfoList, NULL)
  for(i = 0; i < pTimestampInfoList->nTimestampInfos; i++) {
    if(pTimestampInfoList->pTimestampInfos[i]->szType && szType &&
       !strcmp(pTimestampInfoList->pTimestampInfos[i]->szType, szType)) {
      return pTimestampInfoList->pTimestampInfos[i];
    }
  }
  return NULL;
}

//--------------------------------------------------
// Returns the TimestampInfo with required type.
// Creates one if it doesn't exist yet.
// pTimestampInfoList - pointer to TimestampInfoList object [REQUIRED]
// szType - type of TimestampInfo object [REQUIRED]
// returns TimestampInfo pointer or NULL for error
//--------------------------------------------------
EXP_OPTION TimestampInfo* ddocTimestampInfoList_GetOrCreateTimestampInfoOfType(TimestampInfoList* pTimestampInfoList, const char* szId, const char* szType)
{
  TimestampInfo* pTS = NULL;

  RETURN_OBJ_IF_NULL(pTimestampInfoList, NULL);
  pTS = ddocTimestampInfoList_GetTimestampInfoOfType(pTimestampInfoList, szType);
  if(!pTS)
    ddocTimestampInfo_new(pTimestampInfoList, &pTS, szId, szType);
  return pTS;
}

//--------------------------------------------------
// Deletes TimestampInfo subelement of TimestampInfoList object.
// pTimestampInfoList - pointer to TimestampInfoList object [REQUIRED]
// nIdx - index of TimestampInfo object to be removed [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTimestampInfoList_DeleteTimestampInfo(TimestampInfoList* pTimestampInfoList, int nIdx)
{
  int err = ERR_OK, i;

  RETURN_IF_NULL_PARAM(pTimestampInfoList)
  SET_LAST_ERROR_RETURN_IF_NOT(nIdx >= 0 && nIdx < pTimestampInfoList->nTimestampInfos, ERR_TS_BAD_TIMESTAMPINFO_IDX, ERR_TS_BAD_TIMESTAMPINFO_IDX);
  RETURN_IF_NULL_PARAM(pTimestampInfoList->pTimestampInfos[nIdx]);
  // delete the given object
  err = ddocTimestampInfo_free(pTimestampInfoList->pTimestampInfos[nIdx]);
  if(err) return err;
  pTimestampInfoList->pTimestampInfos[nIdx] = 0;
  // move other objects 1 step close to array start
  for(i = nIdx; i < pTimestampInfoList->nTimestampInfos-1; i++) 
    pTimestampInfoList->pTimestampInfos[i] = pTimestampInfoList->pTimestampInfos[i+1];
  pTimestampInfoList->pTimestampInfos[pTimestampInfoList->nTimestampInfos - 1] = 0;
  pTimestampInfoList->nTimestampInfos--;
  return err;
}






//==========< utility functions >========================

//--------------------------------------------------
// Helper function to read TS_REQ from binary input data
// ppReq - address of newly allocated TS_REQ object
// pMBufInData - input data
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTsReadTsReq(TS_REQ** ppReq, DigiDocMemBuf* pMBufInData)
{
  int err = ERR_OK;
  unsigned char* p1;

  RETURN_IF_NULL_PARAM(ppReq);
  RETURN_IF_NULL_PARAM(pMBufInData);
  RETURN_IF_NULL_PARAM(pMBufInData->pMem);
  ddocDebug(4, "ddocTsReadTsReq", "converting: %d bytes to TS_REQ", pMBufInData->nLen);
  p1 = (unsigned char*)pMBufInData->pMem;
  d2i_TS_REQ(ppReq, (const unsigned char**)&p1, pMBufInData->nLen);
  ddocDebug(4, "ddocTsReadTsReq", "TS_REQ: %s", (*ppReq ? "OK" : "ERR"));
  return err;
}

//--------------------------------------------------
// Helper function to write TS_REQ to binary output data
// pReq - address of TS_REQ object
// pMBufOutData - output data
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTsWriteTsReq(TS_REQ* pReq, DigiDocMemBuf* pMBufOutData)
{
  int err = ERR_OK, l1;
  unsigned char* p1;

  RETURN_IF_NULL_PARAM(pReq);
  RETURN_IF_NULL_PARAM(pMBufOutData);
  pMBufOutData->pMem = NULL;
  pMBufOutData->nLen = 0;
  // find out how big a buffer we need
  l1 = i2d_TS_REQ(pReq, NULL);
  ddocDebug(4, "ddocTsReadTsReq", "converting: %d bytes from TS_REQ", l1);
  // alloc mem
  err = ddocMemSetLength(pMBufOutData, l1 + 50);  
  p1 = (unsigned char*)pMBufOutData->pMem;
  l1 = i2d_TS_REQ(pReq, &p1);
  pMBufOutData->nLen = l1;
  ddocDebug(4, "ddocTsReadTsReq", "Converted data: %d", l1);
  return err;
}

//--------------------------------------------------
// Helper function to read TS_RESP from binary input data
// ppResp - address of newly allocated TS_RESP object
// pMBufInData - input data
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTsReadTsResp(TS_RESP** ppResp, DigiDocMemBuf* pMBufInData)
{
  int err = ERR_OK;
  unsigned char* p1;

  RETURN_IF_NULL_PARAM(ppResp);
  RETURN_IF_NULL_PARAM(pMBufInData);
  RETURN_IF_NULL_PARAM(pMBufInData->pMem);
  ddocDebug(4, "ddocTsReadTsResp", "converting: %d bytes to TS_RESP", pMBufInData->nLen);
  p1 = (unsigned char*)pMBufInData->pMem;
  d2i_TS_RESP(ppResp, (const unsigned char**)&p1, pMBufInData->nLen);
  ddocDebug(4, "ddocTsReadTsResp", "TS_RESP: %s", (*ppResp ? "OK" : "ERR"));
  return err;
}

//--------------------------------------------------
// Helper function to write TS_RESP to binary output data
// pResp - address of TS_RESP object
// pMBufOutData - output data
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTsWriteTsResp(TS_RESP* pResp, DigiDocMemBuf* pMBufOutData)
{
  int err = ERR_OK, l1;
  unsigned char* p1;

  RETURN_IF_NULL_PARAM(pResp);
  RETURN_IF_NULL_PARAM(pMBufOutData);
  pMBufOutData->pMem = NULL;
  pMBufOutData->nLen = 0;
  // find out how big a buffer we need
  l1 = i2d_TS_RESP(pResp, NULL);
  ddocDebug(4, "ddocTsReadTsResp", "converting: %d bytes from TS_RESP", l1);
  // alloc mem
  err = ddocMemSetLength(pMBufOutData, l1 + 50);  
  p1 = (unsigned char*)pMBufOutData->pMem;
  l1 = i2d_TS_RESP(pResp, &p1);
  pMBufOutData->nLen = l1;
  ddocDebug(4, "ddocTsReadTsResp", "Converted data: %d", l1);
  return err;
}


//--------------------------------------------------
// Helper function to create a nonce value
// ppNonce - address for new ASN1_INTEGER pointer
// bits - length of nonce in bits
// returns error code or ERR_OK
//--------------------------------------------------
int ddocCreateNonce(ASN1_INTEGER** ppNonce, int bits)
{
  int err = ERR_CREATE_NONCE;
  unsigned char buf[20], buf2[50];
  int len = (bits - 1) / 8 + 1;
  int i, l2;

  RETURN_IF_NULL_PARAM(ppNonce);
  *ppNonce = 0; // mark as not created yet
  // Generating random byte sequence.
  if(len <= (int)sizeof(buf)) {
    if(RAND_bytes(buf, len)) {
      // Find the first non-zero byte and creating ASN1_INTEGER object.
      for(i = 0; i < len && !buf[i]; ++i);
      if((*ppNonce = ASN1_INTEGER_new())) {
	OPENSSL_free((*ppNonce)->data);
	// Allocate at least one byte.
	(*ppNonce)->length = len - i;
	l2 = sizeof(buf2);
	bin2hex((const byte*)buf, len, buf2, &l2);
	ddocDebug(3, "ddocCreateNonce", "nonce: %s", buf2);
	if(((*ppNonce)->data = OPENSSL_malloc((*ppNonce)->length + 1))) {
	  memcpy((*ppNonce)->data, buf + i, (*ppNonce)->length);
	  err = ERR_OK;
	}
      }
    }
  }
  return err;
}


//--------------------------------------------------
// Helper function to convert text/name to OID
// ppObj - address for new ASN1_OBJECT pointer
// oid - stringified oid value
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTxt2Obj(ASN1_OBJECT **ppObj, const char *oid)
{
  RETURN_IF_NULL_PARAM(ppObj);
  *ppObj = 0; // mark as not allocated
  if(!(*ppObj = OBJ_txt2obj(oid, 0)))
    SET_LAST_ERROR_RETURN_CODE(ERR_TXT2OID);
  return ERR_OK;
}

//--------------------------------------------------
// Helper function to create a new TS_REQ from XML data objects
// that will be digested. 
// ppReq - address of newly allocated TS_REQ object
// pMBufDigest - digest data to timestamp (xml)
// policy - oilicy oid in stringform. Use NULL if not necessary
// bNonce - 1=send nonce, 0=no nonce
// bCert - request req_cert property 1/0
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTsCreateTsReq(TS_REQ** ppReq, DigiDocMemBuf* pMBufDigest, 
		      const char* policy, int bNonce, int bCert)
{
  int err = ERR_TS_CREATE_TS_REQ, l2;
  EVP_MD *md = NULL;
  TS_MSG_IMPRINT *msg_imprint = NULL;
  X509_ALGOR *algo = NULL;
  unsigned char *data = NULL;
  ASN1_OBJECT *policy_obj = NULL;
  ASN1_INTEGER *nonce_asn1 = NULL;
  char buf2[100];

  RETURN_IF_NULL_PARAM(ppReq);
  RETURN_IF_NULL_PARAM(pMBufDigest);
  RETURN_IF_NULL_PARAM(pMBufDigest->pMem);
  ddocDebug(4, "ddocTsCreateTsReq", "timestamp dig: %d bytes, policy: %s, nonce: %d cert: %d", 
	    pMBufDigest->nLen, (policy ? policy : "NONE"), bNonce, bCert);

  *ppReq = 0; // mark as not allocated
  // Setting default message digest.
  if ((md = (EVP_MD*)EVP_get_digestbyname("sha1")) != NULL) {
    // Creating request object.
    if((*ppReq = TS_REQ_new()) != NULL) {
      // Setting version.
      if(TS_REQ_set_version(*ppReq, 1)) {
	// Creating and adding MSG_IMPRINT object.
	if((msg_imprint = TS_MSG_IMPRINT_new())) {
	  // Adding algorithm.
	  if((algo = X509_ALGOR_new())) {
	    if((algo->algorithm = OBJ_nid2obj(EVP_MD_type(md)))) {
	      if((algo->parameter = ASN1_TYPE_new())) {
		algo->parameter->type = V_ASN1_NULL;
		if(TS_MSG_IMPRINT_set_algo(msg_imprint, algo)) {
		  // Adding message digest.
		  err = ERR_OK; 
		  l2 = sizeof(buf2);
		  bin2hex((const byte*)pMBufDigest->pMem, pMBufDigest->nLen, buf2, &l2);
		  ddocDebug(3, "ddocTsCreateTsReq", "msg-imprint: %s", buf2);
		  if(TS_MSG_IMPRINT_set_msg(msg_imprint, (unsigned char*)pMBufDigest->pMem, pMBufDigest->nLen)) {
		    if(TS_REQ_set_msg_imprint(*ppReq, msg_imprint)) {
		      // Setting policy if requested.
		      if(policy && strlen(policy) &&
			 !(err = ddocTxt2Obj(&policy_obj, policy)) &&
			 TS_REQ_set_policy_id(*ppReq, policy_obj))
			err = ERR_OK;
		      // Setting nonce if requested.
		      if(bNonce && 
			 !(err = ddocCreateNonce(&nonce_asn1, NONCE_LENGTH)) &&
			 TS_REQ_set_nonce(*ppReq, nonce_asn1))
			err = ERR_OK;
		      // Setting certificate request flag if requested.
		      if(!TS_REQ_set_cert_req(*ppReq, bCert))
			err = ERR_TS_CREATE_TS_REQ;
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
  // if error then free result object
  if(err) {
    TS_REQ_free(*ppReq);
    *ppReq = NULL;
    SET_LAST_ERROR(err);
  }
  TS_MSG_IMPRINT_free(msg_imprint);
  X509_ALGOR_free(algo);
  OPENSSL_free(data);
  ASN1_OBJECT_free(policy_obj);
  ASN1_INTEGER_free(nonce_asn1);
  ddocDebug(4, "ddocTsCreateTsReq", "RC: %d", err);
  return err;
}

//--------------------------------------------------
// Helper function to create a new TS_REQ from XML data objects
// that will be digested. Stores the result in DER form
// in a DigiDocMemBuf object.
// pMBufReq - memory buffer to store new request in PEM format
// pMBufInData - data to timestamp (xml)
// policy - oilicy oid in stringform. Use NULL if not necessary
// bNonce - 1=send nonce, 0=no nonce
// bCert - request req_cert property 1/0
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTsCreateTsReqBin(DigiDocMemBuf* pMBufReq, DigiDocMemBuf* pMBufInData, 
		      const char* policy, int bNonce, int bCert)
{
  int err = ERR_OK;
  TS_REQ *pReq = 0;

  RETURN_IF_NULL_PARAM(pMBufReq);
  RETURN_IF_NULL_PARAM(pMBufInData);
  RETURN_IF_NULL_PARAM(pMBufInData->pMem);
  ddocDebug(4, "ddocTsCreateTsReqBin", "timestamping: %d bytes, policy: %s, nonce: %d cert: %d", 
	    pMBufInData->nLen, (policy ? policy : "NONE"), bNonce, bCert);
  err = ddocTsCreateTsReq(&pReq, pMBufInData, policy, bNonce, bCert);
  if(!err && pReq)
    err = ddocTsWriteTsReq(pReq, pMBufReq);
  if(pReq)
    TS_REQ_free(pReq);
  ddocDebug(4, "ddocTsCreateTsReqBin", "RC: %d", err);
  return err;
}

//--------------------------------------------------
// Returns HTTP return code
// pBuf- buffer with HTTP response
// returns error code or HTTP response code
//--------------------------------------------------
int ddocGetHttpResponseCode(DigiDocMemBuf* pBuf)
{
  int rc = ERR_OK;
  char *p = (char*)pBuf->pMem;
  RETURN_IF_NULL_PARAM(pBuf);
  if(p && !strncmp(p, "HTTP", 4)) {
    while(*p && *p != ' ') p++;
    while(*p && !isdigit(*p)) p++;
    rc = atoi(p);
  } else
    return ERR_HTTP_ERR;
  return rc;
}

//--------------------------------------------------
// Returns HTTP response body
// pInBuf- buffer with HTTP response
// pOutBuf - buffer for response body
// returns error code or ERR_OK
//--------------------------------------------------
int ddocGetHttpPayload(DigiDocMemBuf* pInBuf, DigiDocMemBuf* pOutBuf)
{
  int err = ERR_OK;
  char *p;
  RETURN_IF_NULL_PARAM(pInBuf);
  RETURN_IF_NULL_PARAM(pOutBuf);
  if((p = strstr((char*)pInBuf->pMem, "\r\n\r\n")) != NULL) {
    p += 4;
    err = ddocMemAssignData(pOutBuf, p, pInBuf->nLen - (int)((int)p - (int)pInBuf->pMem));
  } else
    return ERR_HTTP_ERR;
  return err;
}

//--------------------------------------------------
// Divides URL into parts with just protocoll+host+port
// and path/application
// url - input URL
// szHost - buffer for host+port+protocoll
// lHost - length of host buffer
// szPath - buffer for path/application
// lPath -length of path buffer
//--------------------------------------------------
int ddocFindHostAndPathFromUrl(const char* url, char* szHost, int lHost,
			       char* szPath, int lPath)
{
  int err = ERR_OK/*, i*/;
  char* p1;

  RETURN_IF_NULL_PARAM(url);
  RETURN_IF_NULL_PARAM(szHost);
  RETURN_IF_NULL_PARAM(szPath);
  szHost[0] = szPath[0] = 0;
  p1 = (char*)url;
  if(!strncmp(p1, "http://", 7))
    p1 += 7;
  if(!strncmp(p1, "https://", 8))
    p1 += 8;
  p1 = strchr(p1, '/');
  if(p1 && (int)(p1 - url) < lHost) {
    strncpy(szHost, url, (int)(p1 - url));
    szHost[(int)(p1 - url)] = 0;
    if(strlen(p1) < (unsigned int)lPath)
      strcpy(szPath, p1);
  } else {
    strncpy(szHost, url, lHost);
  }
  return err;
}

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
		      const char* proxyHost, const char* proxyPort)
{	
  char buf1[100], buf2[100];
  int err = ERR_OK, l1;
  int connResult = 0;
  DigiDocMemBuf mbuf1, mbuf2, mbuf3;
  X509 *pCert = 0;

  RETURN_IF_NULL_PARAM(pMBufDigest);
  RETURN_IF_NULL_PARAM(url);
  RETURN_IF_NULL_PARAM(ppResp);
  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  mbuf2.pMem = 0;
  mbuf2.nLen = 0;
  mbuf3.pMem = 0;
  mbuf3.nLen = 0;
  // create TS request
  l1 = sizeof(buf1);
  bin2hex((const byte*)pMBufDigest->pMem, pMBufDigest->nLen, buf1, &l1);
  ddocDebug(3, "ddocSendTSRequest", "timestamp: \'%s\' url: %s proxy: %s/%s nonce: %d ask-cert: %d", 
	    buf1, (url ? url : "NULL"), 
	    (proxyHost ? proxyHost : ""), 
	    (proxyPort ? proxyPort : ""), bNonce, bCert);
  // find host and path
  err = ddocFindHostAndPathFromUrl(url, buf1, sizeof(buf1), buf2, sizeof(buf2));
  if(err || !strlen(buf1) /*|| !strlen(buf2)*/) {
    ddocDebug(3, "ddocSendTSRequest", "url: %s host: %s path: %s err: %d", url, buf1, buf2, err);
    SET_LAST_ERROR(ERR_TS_REQUEST);
    return ERR_TS_REQUEST;
  }
  ddocDebug(3, "ddocSendTSRequest", "url: %s host: %s path: %s", url, buf1, buf2);
  // create HTTP request
  err = ddocTsCreateTsReqBin(&mbuf1, pMBufDigest, policy, bNonce, bCert);
  // compose HTTP message
  ddocMemAppendData(&mbuf2, "POST ", -1);
  if(proxyHost && strlen(proxyHost))
    ddocMemAppendData(&mbuf2, buf1, -1);
  ddocMemAppendData(&mbuf2, buf2, -1);
  ddocMemAppendData(&mbuf2, " HTTP/1.0\r\n", -1);
  ddocMemAppendData(&mbuf2, "User-Agent: cdigidoc\r\n", -1);
  ddocMemAppendData(&mbuf2, "Content-Type: application/timestamp-query\r\n", -1);
  ddocMemAppendData(&mbuf2, "Accept: application/timestamp-reply\r\n", -1);
  ddocMemAppendData(&mbuf2, "Connection: Close\r\n", -1);
  ddocMemAppendData(&mbuf2, "Cache-Control: no-cache\r\n", -1);
  ddocMemAppendData(&mbuf2, "Content-Length: ", -1);
  sprintf(buf1, "%ld", mbuf1.nLen);
  ddocMemAppendData(&mbuf2, buf1, -1);
  ddocMemAppendData(&mbuf2, "\r\n\r\n", -1);
  ddocMemAppendData(&mbuf2, mbuf1.pMem, mbuf1.nLen);
  ddocMemBuf_free(&mbuf1);
  // send & receive
  ddocDebug(3, "ddocSendTSRequest", "Send to: %s req: %d bytes", url, mbuf2.nLen);
  ddocDebugWriteFile(5, "tsreq.txt", &mbuf2);
  err = ddocPullUrl(url, &mbuf2, &mbuf3, proxyHost, proxyPort);
  SET_LAST_ERROR_IF_NOT(err == ERR_OK, ERR_TS_REQUEST);
  ddocDebug(3, "ddocSendTSRequest", "RC: %d RECEIVED: %d bytes HTTP: %d", 
	    err, mbuf3.nLen, ddocGetHttpResponseCode(&mbuf3));
  ddocDebugWriteFile(5, "tsresp.txt", &mbuf3);
  ddocMemBuf_free(&mbuf2);
  // parse reply
  if(!err && ((l1 = ddocGetHttpResponseCode(&mbuf3)) == 200)) {
    err = ddocGetHttpPayload(&mbuf3, &mbuf2);
    ddocDebugWriteFile(4, "tsresp.bin", &mbuf2);
    if(!err)
      err = ddocTsReadTsResp(ppResp, &mbuf2);
    // DEBUG
    /*
    if(*ppResp) {
      // debug
      ddocMemBuf_free(&mbuf1);
      ddocTSResp_GetPolicyId(*ppResp, &mbuf1);
      ddocDebug(3, "ddocSendTSRequest", "policy id: %s", (mbuf1.pMem ? (char*)mbuf1.pMem : ""));
      ddocMemBuf_free(&mbuf1);
      ddocTSResp_GetStatus(*ppResp, &l1);
      ddocDebug(3, "ddocSendTSRequest", "status: %d", l1);
      ddocTSResp_GetSerial(*ppResp, &mbuf1);
      ddocDebug(3, "ddocSendTSRequest", "serial: %s", (mbuf1.pMem ? (char*)mbuf1.pMem : ""));
      ddocMemBuf_free(&mbuf1);
      ddocTSResp_GetNonce(*ppResp, &mbuf1);
      ddocDebug(3, "ddocSendTSRequest", "nonce: %s", (mbuf1.pMem ? (char*)mbuf1.pMem : ""));
      ddocMemBuf_free(&mbuf1);
      ddocTSResp_GetTime(*ppResp, &t1);
      time_t2str(t1, buf1);
      ddocDebug(3, "ddocSendTSRequest", "time: %s", buf1);
      ddocTSResp_GetTsaName(*ppResp, &mbuf1);
      ddocDebug(3, "ddocSendTSRequest", "tsa: %s", (mbuf1.pMem ? (char*)mbuf1.pMem : ""));
      ddocMemBuf_free(&mbuf1);
      ddocMemBuf_free(&mbuf2);
      
      ddocTSResp_GetMsgImprint(*ppResp, &mbuf1);
      l1 = mbuf1.nLen * 2 + 20;
      ddocMemSetLength(&mbuf2, l1);
      bin2hex((const byte*)mbuf1.pMem, mbuf1.nLen, (char*)mbuf2.pMem, &l1);
      mbuf2.nLen = l1;
      ddocDebug(3, "ddocSendTSRequest", "msg-imprint: %s", (mbuf2.pMem ? (char*)mbuf2.pMem : ""));
      ddocMemBuf_free(&mbuf1);
      ddocMemBuf_free(&mbuf2);
      
      ddocTSResp_GetMsgImprint_Algoritm(*ppResp, &mbuf1);
      ddocDebug(3, "ddocSendTSRequest", "algoritm: %s", (mbuf1.pMem ? (char*)mbuf1.pMem : ""));
      ddocMemBuf_free(&mbuf1);
      ddocTsResp_GetSignerCertSerial(*ppResp, &mbuf1);
      ddocDebug(3, "ddocSendTSRequest", "signer cert-serial: %s", (mbuf1.pMem ? (char*)mbuf1.pMem : ""));
      ddocMemBuf_free(&mbuf1);
      ddocTsResp_GetSignerCertIssuer(*ppResp, &mbuf1);
      ddocDebug(3, "ddocSendTSRequest", "signer cert-issuer: %s", (mbuf1.pMem ? (char*)mbuf1.pMem : ""));
      ddocMemBuf_free(&mbuf1);
      // test
      ddocTsResp_GetSignerCert(*ppResp, &pCert);
      if(pCert) {
	ddocMemSetLength(&mbuf1, 100);
	ReadCertSerialNumber((char*)mbuf1.pMem, 100, pCert);
	ddocDebug(3, "ddocSendTSRequest", "signer cert-serial2: %s", (mbuf1.pMem ? (char*)mbuf1.pMem : ""));
	ddocMemBuf_free(&mbuf1);
	X509_free(pCert);
      }

    }
    */
  }
  ddocMemBuf_free(&mbuf2);
  ddocMemBuf_free(&mbuf3);
  if(l1 != 200) {
    SET_LAST_ERROR(ERR_TS_REQUEST);
    err = ERR_TS_REQUEST;
  }
  return(err);
}


//--------------------------------------------------
// Verifies a Timestamp response to TSA.
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBufDigest - digest of timestamped data [REQUIRED]
// pCACerts - CA certs array [REQUIRED]
// szCAPath - path to dir with CA certs [OPTIONAL]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocVerifyTSResponse(TS_RESP* pResp, const DigiDocMemBuf* pMBufDigest, 
			 X509** pCACerts, char* szCAPath)
{	
  TS_VERIFY_CTX *ctx = NULL;
  int err = ERR_OK, l1;
  DigiDocMemBuf mbuf1;
  X509 *pTsaCert = NULL, *pTsaCACert = NULL;
  EVP_PKEY* pubkey = NULL;
  //char *p1 = 0, *p2 = 0;
  RETURN_IF_NULL_PARAM(pResp);
  RETURN_IF_NULL_PARAM(pMBufDigest);
  RETURN_IF_NULL_PARAM(pCACerts);
  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  // find CA cert as the last in this chain
  for(l1 = 0; pCACerts[l1]; l1++)
    pTsaCACert = pCACerts[l1];
  RETURN_IF_NULL(pTsaCACert); // must find direct CA cert
  // create TS request
  l1 = 60;
  ddocMemSetLength(&mbuf1, l1);
  bin2hex((const byte*)pMBufDigest->pMem, pMBufDigest->nLen, (char*)mbuf1.pMem, &l1);
  mbuf1.nLen = l1;
  ddocDebug(3, "ddocVerifyTSResponse", "timestamp digest: \'%s\' len: %d", 
	    (char*)mbuf1.pMem, pMBufDigest->nLen);
  ddocMemBuf_free(&mbuf1);
  // setup context
  ctx = TS_VERIFY_CTX_new();
  RETURN_IF_NULL(ctx);
  // debug import
  //p1 = p2;
  //i2d_TS_MSG_IMPRINT(pResp->tst_info->msg_imprint, &p2);

  ctx->flags |= TS_VFY_IMPRINT;
  ctx->imprint = pMBufDigest->pMem; // who should release this ???
  ctx->imprint_len = pMBufDigest->nLen;
  ctx->flags |= TS_VFY_SIGNATURE;
  // Initialising the X509_STORE object.
  err = setup_verifyCERT(&(ctx->store), szCAPath, (const X509**)pCACerts);
  if(!err)
    err = TS_RESP_verify_response(ctx, pResp);
  ddocDebug(3, "ddocVerifyTSResponse", "RC: %d", err);
  if(err == 1) {
    err = ERR_OK;
  } else {
    err = ERR_TS_VERIFY;
    checkErrors();
  }
  if(!err) {
    err = ddocTSResp_GetStatus(pResp, &l1);
    if(l1 != TS_STATUS_GRANTED &&
       l1 != TS_STATUS_GRANTED_WITH_MODS) {
      ddocDebug(3, "ddocVerifyTSResponse", "invalid timestamp status: %d", l1);
      err = ERR_TS_VERIFY;
	}
  }
  if(ctx) {
    ctx->imprint = 0;
    ctx->imprint_len =  0;
    TS_VERIFY_CTX_free(ctx);
  }
  // check signers cert
  if(!err) {
    err = ddocTsResp_GetSignerCert(pResp, &pTsaCert);
    if(pTsaCert) {
      ddocCertGetSubjectCN(pTsaCert, &mbuf1);
      ddocDebug(3, "ddocVerifyTSResponse", "TSA Cert: %s", (const char*)mbuf1.pMem);
      ddocMemBuf_free(&mbuf1);
      ddocCertGetSubjectCN(pTsaCACert, &mbuf1);
      ddocDebug(3, "ddocVerifyTSResponse", "TSA CA Cert: %s", (const char*)mbuf1.pMem);
      ddocMemBuf_free(&mbuf1);
      err = GetPublicKey(&pubkey, pTsaCACert);
      if(!err && pubkey) {
      	err = X509_verify(pTsaCert, pubkey);
	ddocDebug(3, "ddocVerifyTSResponse", "verify RC: %d", err);
	if(err == ERR_LIB_NONE)
	  err = ERR_OK;
	else
	  err = ERR_CERT_ISSUER;
      }
	  X509_free(pTsaCert);
    }
  }
  return(err);
}

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
				time_t *pTime, X509** ppTsaCert)
{
  TS_RESP *pResp = NULL;
  int err = ERR_OK;
  DigiDocMemBuf* pMBuf = NULL;

  RETURN_IF_NULL_PARAM(pTS);
  ddocDebug(3, "ddocVerifyTSResponse", "timestamp %s type: %s", pTS->szId, pTS->szType);
  pMBuf = (DigiDocMemBuf*)ddocTimestampInfo_GetTS(pTS);
  RETURN_IF_NULL(pMBuf);
  err = ddocTsReadTsResp(&pResp, pMBuf);
  err = ddocVerifyTSResponse(pResp, ddocTimestampInfo_GetRealDigest(pTS), pCACerts, szCAPath);
  ddocDebug(3, "ddocVerifyTSResponse", "timestamp %s RC: %d", pTS->szId, err);
  if(pResp && pTime && !err)
    err = ddocTSResp_GetTime(pResp, pTime);
  if(pResp && ppTsaCert && !err)
    err = ddocTsResp_GetSignerCert(pResp, ppTsaCert);
  if(pResp)
    TS_RESP_free(pResp);
  return err;
}


//--------------------------------------------------
// Returns TS_RESP policy id.
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBufDigest - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTSResp_GetPolicyId(TS_RESP* pResp, DigiDocMemBuf* pMBuf)
{	
  TS_VERIFY_CTX *ctx = NULL;
  int err = ERR_OK, l1;

  RETURN_IF_NULL_PARAM(pMBuf);
  RETURN_IF_NULL_PARAM(pResp);
  pMBuf->pMem = 0;
  pMBuf->nLen = 0;
  // find length
  ddocMemSetLength(pMBuf, 100);
  l1 = i2t_ASN1_OBJECT(pMBuf->pMem, 100, pResp->tst_info->policy_id);
  ddocDebug(3, "ddocTSResp_GetPolicyId", "policy: %d - %s", l1, (pMBuf->pMem ? (char*)pMBuf->pMem : ""));
  return err;
}


//--------------------------------------------------
// Returns TS_RESP status code as string
// pResp - TS_RESP timestamp response [REQUIRED]
// pStatus - address of variable for status [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTSResp_GetStatus(TS_RESP* pResp, int* pStatus)
{
  BIGNUM* bn;
  char* str = 0;
  int err = ERR_OK;

  RETURN_IF_NULL_PARAM(pStatus);
  RETURN_IF_NULL_PARAM(pResp);
  *pStatus = -1;
  bn = ASN1_INTEGER_to_BN(pResp->status_info->status, NULL);
  if(bn) {
    str = BN_bn2dec(bn);
    if(str) 
      *pStatus = atoi(str);
  }
  // create TS request
  if(str)
    OPENSSL_free(str);
  return err;
}

//--------------------------------------------------
// Returns TS_RESP serial number as a string
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBufDigest - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTSResp_GetSerial(TS_RESP* pResp, DigiDocMemBuf* pMBuf)
{
  BIGNUM* bn;
  char* str = 0;
  int err = ERR_OK;

  RETURN_IF_NULL_PARAM(pMBuf);
  RETURN_IF_NULL_PARAM(pResp);
  pMBuf->pMem = 0;
  pMBuf->nLen = 0;
  bn = ASN1_INTEGER_to_BN(pResp->tst_info->serial, NULL);
  if(bn) {
    str = BN_bn2dec(bn);
    if(str) 
      ddocMemAssignData(pMBuf, str, -1);
  }
  // create TS request
  if(str)
    OPENSSL_free(str);
  return err;
}

//--------------------------------------------------
// Returns TS_RESP nonce as string
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBufDigest - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTSResp_GetNonce(TS_RESP* pResp, DigiDocMemBuf* pMBuf)
{
  BIGNUM* bn;
  char* str = 0;
  int err = ERR_OK;

  RETURN_IF_NULL_PARAM(pMBuf);
  RETURN_IF_NULL_PARAM(pResp);
  pMBuf->pMem = 0;
  pMBuf->nLen = 0;
  bn = ASN1_INTEGER_to_BN(pResp->tst_info->nonce, NULL);
  if(bn) {
    str = BN_bn2dec(bn);
    if(str) 
      ddocMemAssignData(pMBuf, str, -1);
  }
  // create TS request
  if(str)
    OPENSSL_free(str);
  return err;
}

//--------------------------------------------------
// Returns TS_RESP time as string
// pResp - TS_RESP timestamp response [REQUIRED]
// pTime - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTSResp_GetTime(TS_RESP* pResp, time_t* pTime)
{
  int err = ERR_OK;
  struct tm tm1;
  int dmz = 0;

  RETURN_IF_NULL_PARAM(pTime);
  RETURN_IF_NULL_PARAM(pResp);
  *pTime = 0;
  err = decodeGeneralizedTime(pResp->tst_info->time, 
			      &tm1.tm_year, &tm1.tm_mon, 
			      &tm1.tm_mday, &tm1.tm_hour, &tm1.tm_min, &tm1.tm_sec);
  //tm1.tm_year -= 1900;
  tm1.tm_mon -= 1;
  tm1.tm_isdst = _daylight;
  *pTime = mktime(&tm1);
  if(_daylight != 0) {
    if(_timezone<0){
      dmz = (_timezone / 3600) - _daylight;
    }else{
      dmz = (_timezone / 3600) + _daylight;
    }
  }else{
    dmz=_timezone / 3600;
  }
  (*pTime) = (*pTime) - (dmz * 3600);

  return err;
}


//--------------------------------------------------
// Returns TS_RESP tsa name as string
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBufDigest - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTSResp_GetTsaName(TS_RESP* pResp, DigiDocMemBuf* pMBuf)
{
  int err = ERR_OK, i, n, t, l, b = 0;
  X509_NAME_ENTRY *pNe = 0;
  char *p, *s;

  RETURN_IF_NULL_PARAM(pMBuf);
  RETURN_IF_NULL_PARAM(pResp);
  pMBuf->pMem = 0;
  pMBuf->nLen = 0;
  if(pResp->tst_info->tsa) {
    ddocDebug(3, "ddocTSResp_GetTsaName", "type: %d", pResp->tst_info->tsa->type);
    switch(pResp->tst_info->tsa->type) {
    case GEN_DIRNAME:
      for(i = 0; i < sk_X509_NAME_ENTRY_num(pResp->tst_info->tsa->d.directoryName->entries); i++) {
	pNe = sk_X509_NAME_ENTRY_value(pResp->tst_info->tsa->d.directoryName->entries, i);
	n = OBJ_obj2nid(pNe->object);
	s = (char*)OBJ_nid2sn(n);
	t = pNe->value->type;
	if(n != NID_undef && s != NULL) {
	  // convert to UTF8 only
	  p = 0;
	  if(t == V_ASN1_UTF8STRING) {
	    p = pNe->value->data;
	    l = pNe->value->length;
	  } else
	    l = ASN1_STRING_to_UTF8((unsigned char **)&p, pNe->value);
	  // append separator if necessary
	  if(b) 
	    err = ddocMemAppendData(pMBuf, "/", -1);
	  else 
	    b = 1;
	  // print the entry
	  err = ddocMemAppendData(pMBuf, s, -1);
	  err = ddocMemAppendData(pMBuf, "=", -1);
	  err = ddocMemAppendData(pMBuf, (const char*)p, l);
	  // cleanup
	  if(p && t != V_ASN1_UTF8STRING)
	    OPENSSL_free(p);
	}
      }      
      break;
    }
  }
  return err;
}


//--------------------------------------------------
// Returns TS_RESP message imprint as binary data
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBuf - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTSResp_GetMsgImprint(TS_RESP* pResp, DigiDocMemBuf* pMBuf)
{
  int err = ERR_OK, l1;
  unsigned char *p1;

  RETURN_IF_NULL_PARAM(pMBuf);
  RETURN_IF_NULL_PARAM(pResp);
  pMBuf->pMem = 0;
  pMBuf->nLen = 0;
  if(pResp->tst_info && pResp->tst_info->msg_imprint) {
    l1 = i2d_ASN1_OCTET_STRING(pResp->tst_info->msg_imprint->hashed_msg, NULL);
    ddocDebug(4, "ddocTsReadTsReq", "converting: %d bytes from MSG_IMPRINT", l1);
    // alloc mem
    err = ddocMemSetLength(pMBuf, l1 + 50);  
    p1 = (unsigned char*)pMBuf->pMem;
    l1 = i2d_ASN1_OCTET_STRING(pResp->tst_info->msg_imprint->hashed_msg, &p1);
    pMBuf->nLen = l1;
  }
  return err;
}


//--------------------------------------------------
// Returns TS_RESP message imprint algoritm as string
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBuf - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTSResp_GetMsgImprint_Algoritm(TS_RESP* pResp, DigiDocMemBuf* pMBuf)
{
  int err = ERR_OK, i;

  RETURN_IF_NULL_PARAM(pMBuf);
  RETURN_IF_NULL_PARAM(pResp);
  pMBuf->pMem = 0;
  pMBuf->nLen = 0;
  if(pResp->tst_info && pResp->tst_info->msg_imprint) {
    i = OBJ_obj2nid(pResp->tst_info->msg_imprint->hash_algo->algorithm);
    err = ddocMemSetLength(pMBuf, 50); 
    strncpy((char*)pMBuf->pMem, ((i == NID_undef) ? "UNKNOWN" : OBJ_nid2ln(i)), pMBuf->nLen);
    pMBuf->nLen = strlen((char*)pMBuf->pMem);
  }
  return err;
}

//--------------------------------------------------
// Returns TS_RESP signers (e.g. TSA) cert serial.
// Assumes that there's only 1 signer
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBuf - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocTsResp_GetSignerCertSerial(TS_RESP* pResp, DigiDocMemBuf* pMBuf)
{
  int err = ERR_OK;
  PKCS7 *pPkcs7;
  PKCS7_SIGNER_INFO *si;
  STACK_OF(PKCS7_SIGNER_INFO) *sinfos = NULL;
  char *p1;
  BIGNUM* bn;

  RETURN_IF_NULL_PARAM(pResp);
  RETURN_IF_NULL_PARAM(pMBuf);
  pPkcs7 = TS_RESP_get_token(pResp);
  RETURN_IF_NULL(pPkcs7);
  if(PKCS7_type_is_signed(pPkcs7)) {
    sinfos = PKCS7_get_signer_info(pPkcs7);
    if(sinfos && sk_PKCS7_SIGNER_INFO_num(sinfos) == 1) {
      si = sk_PKCS7_SIGNER_INFO_value(sinfos, 0);      
      bn = ASN1_INTEGER_to_BN(si->issuer_and_serial->serial, NULL);
      p1 = BN_bn2dec(bn);
      ddocMemAssignData(pMBuf, p1, -1);
      ddocDebug(3, "ddocTsResp_GetSignerCertSerial", "Signer cert serial: %s", p1);
      OPENSSL_free(p1);
    }
  }
  return err;
}

//--------------------------------------------------
// Returns TS_RESP signers (e.g. TSA) cert issuer DN.
// Assumes that there's only 1 signer
// pResp - TS_RESP timestamp response [REQUIRED]
// pMBuf - buffer for returned data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTsResp_GetSignerCertIssuer(TS_RESP* pResp, DigiDocMemBuf* pMBuf)
{
  int err = ERR_OK;
  PKCS7 *pPkcs7;
  PKCS7_SIGNER_INFO *si;
  STACK_OF(PKCS7_SIGNER_INFO) *sinfos = NULL;


  RETURN_IF_NULL_PARAM(pResp);
  RETURN_IF_NULL_PARAM(pMBuf);
  pPkcs7 = TS_RESP_get_token(pResp);
  RETURN_IF_NULL(pPkcs7);
  if(PKCS7_type_is_signed(pPkcs7)) {
    sinfos = PKCS7_get_signer_info(pPkcs7);
    if(sinfos && sk_PKCS7_SIGNER_INFO_num(sinfos) == 1) {
      si = sk_PKCS7_SIGNER_INFO_value(sinfos, 0);      
      err = ddocMemSetLength(pMBuf, 300); 
      X509_NAME_oneline(si->issuer_and_serial->issuer, (char*)pMBuf->pMem, pMBuf->nLen);
      ddocDebug(3, "ddocTsResp_GetSignerCertIssuer", "Signer cert issuer: %s", (char*)pMBuf->pMem);
    }
  }
  return err;
}

//--------------------------------------------------
// Returns TS_RESP signers (e.g. TSA) cert.
// Assumes that there's only 1 signer
// pResp - TS_RESP timestamp response [REQUIRED]
// ppCert - address of signers cert pointer [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocTsResp_GetSignerCert(TS_RESP* pResp, X509** ppCert)
{
  int err = ERR_OK;
  PKCS7 *pPkcs7;
  STACK_OF(X509) *signers = NULL;

  RETURN_IF_NULL_PARAM(pResp);
  RETURN_IF_NULL_PARAM(ppCert);
  // mark as not found
  *ppCert = 0;
  pPkcs7 = TS_RESP_get_token(pResp);
  RETURN_IF_NULL(pPkcs7);
  ddocDebug(3, "ddocTsResp_GetSignerCert", "PKCS7 signed: %d signed-and-envloped: %d", 
	    PKCS7_type_is_signed(pPkcs7), PKCS7_type_is_signedAndEnveloped(pPkcs7));
  if(PKCS7_type_is_signed(pPkcs7)) {
    signers = PKCS7_get0_signers(pPkcs7, NULL, 0);
    if(signers && sk_X509_num(signers) == 1) {
      *ppCert = sk_X509_value(signers, 0);
      if(*ppCert)
	CRYPTO_add(&(*ppCert)->references, 1, CRYPTO_LOCK_X509);
    }
  }
  return err;
}
