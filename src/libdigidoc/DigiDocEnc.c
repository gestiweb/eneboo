//==================================================
// FILE:	DigiDocEnc.c
// PROJECT:     Digi Doc Encryption
// DESCRIPTION: DigiDocEnc structure admin functions
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
//      15.09.2004      Veiko Sinivee
//                      Creation
//==================================================

// config data comes from there
#ifndef WIN32
  #if HAVE_CONFIG_H
    #include <config.h>
  #endif
#endif // no win32

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocEnc.h>
#include <libdigidoc/DigiDocEncGen.h>
#include <libdigidoc/DigiDocError.h>
#include <libdigidoc/DigiDocDebug.h> 
#include <libdigidoc/DigiDocPKCS11.h> 
#include <libdigidoc/DigiDocConvert.h>
#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/DigiDocMem.h>

#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <zlib.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#define snprintf   _snprintf
#endif


//======================< DEncEncryptedData >==============================


//--------------------------------------------------
// Validates the <EncryptionMethod> parameter. We
// currently support only AES-128-CBC.
// szEncMethod - value to be checked
// returns error code or ERR_OK
//--------------------------------------------------
int dencEncDataValidateEncMethod(const char* szEncMethod)
{
  if(szEncMethod && strcmp(szEncMethod, DENC_ENC_METHOD_AES128))
    SET_LAST_ERROR_RETURN(ERR_DENC_ENC_METHOD, ERR_DENC_ENC_METHOD)
  else
    return ERR_OK;
}

//--------------------------------------------------
// Validates the XML namespace parameter. We
// currently support only 
// szEncMethod - value to be checked
// returns error code or ERR_OK
//--------------------------------------------------
int dencValidateEncXmlNs(const char* szXmlNs)
{
  if(szXmlNs && strcmp(szXmlNs, DENC_XMLNS_XMLENC))
    SET_LAST_ERROR_RETURN(ERR_DENC_ENC_XMLNS, ERR_DENC_ENC_XMLNS)
  else
    return ERR_OK;
}

//--------------------------------------------------
// "Constructor" of DEncEncryptedData object
// pEncData - address of buffer for newly allocated object [REQUIRED]
// szXmlNs - XML namespace uri
// szEncMethod - encyrption method algorithm uri
// szId - elements Id attribute [OPTIONAL]
// szType - elements type atribute [OPTIONAL]
// szMimeType - elements mime-type attribute [OPTIONAL]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_new(DEncEncryptedData** pEncData, 
			  const char* szXmlNs, const char* szEncMethod,
			  const char* szId, const char* szType, 
			  const char* szMimeType)
{
  int err = ERR_OK;

  // check input parameters
  ddocDebug(3, "dencEncryptedData_new", "EncMethod: %s, Id: %s, type: %s, mime: %s",
	    szEncMethod, szId, szType, szMimeType);
  RETURN_IF_NULL_PARAM(pEncData);
  err = dencEncDataValidateEncMethod(szEncMethod);
  if(err) return err;
  err = dencValidateEncXmlNs(szXmlNs);
  if(err) return err;
  *pEncData = (DEncEncryptedData*)malloc(sizeof(DEncEncryptedData));
  // allocate new object
  RETURN_IF_BAD_ALLOC(*pEncData);
  memset(*pEncData, 0, sizeof(DEncEncryptedData));
  // set required fields
  if(szXmlNs) {
    err = ddocMemAssignString((char**)&((*pEncData)->szXmlNs), szXmlNs);
    if(err) return err;
  }
  if(szEncMethod) {
    err = ddocMemAssignString((char**)&((*pEncData)->szEncryptionMethod), szEncMethod);
    if(err) return err;
  }
  // set optional fields
  if(szId) {
    err = ddocMemAssignString((char**)&((*pEncData)->szId), szId);
    if(err) return err;
  }
  if(szType) {
    err = ddocMemAssignString((char**)&((*pEncData)->szType), szType);
    if(err) return err;
  }
  if(szMimeType) {
    err = ddocMemAssignString((char**)&((*pEncData)->szMimeType), szMimeType);
    if(err) return err;
  }
  // set meta info carrying lib & document format versions
  dencMetaInfo_SetLibVersion(*pEncData);
  dencMetaInfo_SetFormatVersion(*pEncData);
  return err;
}

//--------------------------------------------------
// "Destructor" of DEncEncryptedData object
// pEncData - address of object to be deleted [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_free(DEncEncryptedData* pEncData)
{
  int i, err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncData)
  // cleanup this object
  if(pEncData->szId)
    free(pEncData->szId);
  if(pEncData->szType)
    free(pEncData->szType);
  if(pEncData->szMimeType)
    free(pEncData->szMimeType);
  if(pEncData->szEncryptionMethod)
    free(pEncData->szEncryptionMethod);
  if(pEncData->szXmlNs)
    free(pEncData->szXmlNs);
  // cleanup child objects
  ddocMemBuf_free(&(pEncData->mbufEncryptedData)); 
  if(pEncData->encProperties.szId)
    free(pEncData->encProperties.szId);
  for(i = 0; i < pEncData->encProperties.nEncryptionProperties; i++) {
    if(pEncData->encProperties.arrEncryptionProperties[i]) {
      err = dencEncryptionProperty_free(pEncData->encProperties.arrEncryptionProperties[i]);
      if(err) return err;
    }
  }
  if(pEncData->encProperties.arrEncryptionProperties)
    free(pEncData->encProperties.arrEncryptionProperties);
  ddocMemBuf_free(&(pEncData->mbufTransportKey));
  for(i = 0; i < pEncData->nEncryptedKeys; i++) {
    if(pEncData->arrEncryptedKeys[i]) {
      err = dencEncryptedKey_free(pEncData->arrEncryptedKeys[i]);
      if(err) return err;
    }
  }
  free(pEncData->arrEncryptedKeys);
  free(pEncData);
  return err;
}

//======================< DEncEncryptedData - accessors >===================


//--------------------------------------------------
// Accessor for Id atribute of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* dencEncryptedData_GetId(DEncEncryptedData* pEncData)
{
  RETURN_OBJ_IF_NULL(pEncData, NULL)
  return pEncData->szId;
}

//--------------------------------------------------
// Accessor for Type atribute of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* dencEncryptedData_GetType(DEncEncryptedData* pEncData)
{
  RETURN_OBJ_IF_NULL(pEncData, NULL)
  return pEncData->szType;
}

//--------------------------------------------------
// Accessor for MimeType atribute of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* dencEncryptedData_GetMimeType(DEncEncryptedData* pEncData)
{
  RETURN_OBJ_IF_NULL(pEncData, NULL)
  return pEncData->szMimeType;
}

//--------------------------------------------------
// Accessor for xmlns atribute of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* dencEncryptedData_GetXmlNs(DEncEncryptedData* pEncData)
{
  RETURN_OBJ_IF_NULL(pEncData, NULL)
  return pEncData->szXmlNs;
}

//--------------------------------------------------
// Accessor for EncryptionMethod subelement of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* dencEncryptedData_GetEncryptionMethod(DEncEncryptedData* pEncData)
{
  RETURN_OBJ_IF_NULL(pEncData, NULL)
  return pEncData->szEncryptionMethod;
}

//--------------------------------------------------
// Accessor for Id atribute of EncryptionProperties subelement of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* dencEncryptedData_GetEncryptionPropertiesId(DEncEncryptedData* pEncData)
{
  RETURN_OBJ_IF_NULL(pEncData, NULL)
  return pEncData->encProperties.szId;
}

//--------------------------------------------------
// Accessor for count of EncryptionProperties subelement of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// returns count or -1 for error. Then use error API to check errors
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_GetEncryptionPropertiesCount(DEncEncryptedData* pEncData)
{
  SET_LAST_ERROR_RETURN_IF_NOT(pEncData, ERR_NULL_POINTER, -1)
  return pEncData->encProperties.nEncryptionProperties;
}

//--------------------------------------------------
// Accessor for EncryptionProperties subelement of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// nIdx - index of EncryptionProperty object [REQUIRED]
// returns EncryptionProperty pointer or NULL for error
//--------------------------------------------------
EXP_OPTION DEncEncryptionProperty* dencEncryptedData_GetEncryptionProperty(DEncEncryptedData* pEncData, int nIdx)
{
  RETURN_OBJ_IF_NULL(pEncData, NULL)
  SET_LAST_ERROR_RETURN_IF_NOT(nIdx >= 0 && nIdx < pEncData->encProperties.nEncryptionProperties, ERR_DENC_BAD_PROP_IDX, NULL);
  RETURN_OBJ_IF_NULL(pEncData->encProperties.arrEncryptionProperties[nIdx], 0);
  return pEncData->encProperties.arrEncryptionProperties[nIdx];
}

//--------------------------------------------------
// Finds EncryptionProperty by Name atribute
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// name - name of searched property
// returns EncryptionProperty pointer or NULL for error
//--------------------------------------------------
EXP_OPTION DEncEncryptionProperty* dencEncryptedData_FindEncryptionPropertyByName(DEncEncryptedData* pEncData, const char* name)
{
  DEncEncryptionProperty* pEncProp = 0;
  int i;

  RETURN_OBJ_IF_NULL(pEncData, NULL)
  RETURN_OBJ_IF_NULL(name, NULL)
  for(i = 0; i < pEncData->encProperties.nEncryptionProperties; i++) {
    if(pEncData->encProperties.arrEncryptionProperties[i] &&
       pEncData->encProperties.arrEncryptionProperties[i]->szName &&
       !strcmp(pEncData->encProperties.arrEncryptionProperties[i]->szName, name)) {
	 pEncProp = pEncData->encProperties.arrEncryptionProperties[i];
	 break;
    }
  }
  return pEncProp;
}

//--------------------------------------------------
// Retrieves the last EncryptionProperty subelement of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// returns EncryptionProperty pointer or NULL for error
//--------------------------------------------------
EXP_OPTION DEncEncryptionProperty* dencEncryptedData_GetLastEncryptionProperty(DEncEncryptedData* pEncData)
{
  int nIdx;
  RETURN_OBJ_IF_NULL(pEncData, NULL)
  nIdx = pEncData->encProperties.nEncryptionProperties -1;
  SET_LAST_ERROR_RETURN_IF_NOT(nIdx >= 0 && nIdx < pEncData->encProperties.nEncryptionProperties, ERR_DENC_BAD_PROP_IDX, NULL);
  RETURN_OBJ_IF_NULL(pEncData->encProperties.arrEncryptionProperties[nIdx], 0);
  return pEncData->encProperties.arrEncryptionProperties[nIdx];
}

//--------------------------------------------------
// Accessor for count of EncryptedKey subelement of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// returns count or -1 for error. Then use error API to check errors
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_GetEncryptedKeyCount(DEncEncryptedData* pEncData)
{
  SET_LAST_ERROR_RETURN_IF_NOT(pEncData, ERR_NULL_POINTER, -1)
  return pEncData->nEncryptedKeys;
}

//--------------------------------------------------
// Accessor for EncryptedKey subelement of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// nIdx - index of EncryptedKey object [REQUIRED]
// returns EncryptedKey pointer or NULL for error
//--------------------------------------------------
EXP_OPTION DEncEncryptedKey* dencEncryptedData_GetEncryptedKey(DEncEncryptedData* pEncData, int nIdx)
{
  RETURN_OBJ_IF_NULL(pEncData, NULL)
  SET_LAST_ERROR_RETURN_IF_NOT(nIdx >= 0 && nIdx < pEncData->nEncryptedKeys, ERR_DENC_BAD_KEY_IDX, NULL);
  RETURN_OBJ_IF_NULL(pEncData->arrEncryptedKeys[nIdx], 0);
  return pEncData->arrEncryptedKeys[nIdx];
}


//--------------------------------------------------
// Searches an EncryptedKey by recipients name
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// recipient - recipient name used to search the key [REQUIRED]
// returns EncryptedKey pointer or NULL for error
//--------------------------------------------------
EXP_OPTION DEncEncryptedKey* dencEncryptedData_FindEncryptedKeyByRecipient(DEncEncryptedData* pEncData, const char* recipient)
{
  int i;
  DEncEncryptedKey *pKey;

  RETURN_OBJ_IF_NULL(pEncData, NULL)
  RETURN_OBJ_IF_NULL(recipient, NULL)
  for(i = 0; i < pEncData->nEncryptedKeys; i++) {
    pKey = pEncData->arrEncryptedKeys[i];
    if(pKey && pKey->szRecipient && !strcmp(pKey->szRecipient, recipient))
      return pKey;
  }
  return NULL;
}

//--------------------------------------------------
// Searches an EncryptedKey by certs CN field
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// cn - cert CN used to search the key [REQUIRED]
// returns EncryptedKey pointer or NULL for error
//--------------------------------------------------
EXP_OPTION DEncEncryptedKey* dencEncryptedData_FindEncryptedKeyByCN(DEncEncryptedData* pEncData, const char* cn)
{
  int i, err = ERR_OK;
  DEncEncryptedKey *pKey;
  DigiDocMemBuf mbuf;

  mbuf.pMem = 0;
  mbuf.nLen = 0;
  RETURN_OBJ_IF_NULL(pEncData, NULL)
  RETURN_OBJ_IF_NULL(cn, NULL)
  for(i = 0; i < pEncData->nEncryptedKeys; i++) {
    pKey = pEncData->arrEncryptedKeys[i];
    if(pKey && pKey->pCert) {
      err = ddocCertGetSubjectCN(pKey->pCert, &mbuf);
      if(!strcmp((const char*)mbuf.pMem, cn)) {
	ddocMemBuf_free(&mbuf);
	return pKey;
      }
    }
  }
  ddocMemBuf_free(&mbuf);
  return NULL;
}


//--------------------------------------------------
// Accessor for EncryptedKey subelement of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// returns EncryptedKey pointer or NULL for error
//--------------------------------------------------
EXP_OPTION DEncEncryptedKey* dencEncryptedData_GetLastEncryptedKey(DEncEncryptedData* pEncData)
{
  int nIdx;
  RETURN_OBJ_IF_NULL(pEncData, NULL)
  nIdx = pEncData->nEncryptedKeys-1;
  SET_LAST_ERROR_RETURN_IF_NOT(nIdx >= 0 && nIdx < pEncData->nEncryptedKeys, ERR_DENC_BAD_KEY_IDX, NULL);
  RETURN_OBJ_IF_NULL(pEncData->arrEncryptedKeys[nIdx], 0);
  return pEncData->arrEncryptedKeys[nIdx];
}

//--------------------------------------------------
// Accessor for encrypted data.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// ppBuf - address for encrypted data pointer [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_GetEncryptedData(DEncEncryptedData* pEncData, DigiDocMemBuf** ppBuf)
{
  RETURN_IF_NULL_PARAM(pEncData)
  RETURN_IF_NULL_PARAM(ppBuf)
  *ppBuf = &(pEncData->mbufEncryptedData);
  return ERR_OK;
}

//--------------------------------------------------
// Accessor for encrypted data status flag.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// returns status or -1 for error. Then use error API to check errors
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_GetEncryptedDataStatus(DEncEncryptedData* pEncData)
{
  SET_LAST_ERROR_RETURN_IF_NOT(pEncData, ERR_NULL_POINTER, -1)
  return pEncData->nDataStatus;
}


//======================< DEncEncryptedData - mutators >===================

//--------------------------------------------------
// Mutatoror for Id atribute of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_SetId(DEncEncryptedData* pEncData, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncData)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignString((char**)&(pEncData->szId), value);
  return err;
}

//--------------------------------------------------
// Mutatoror for Type atribute of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_SetType(DEncEncryptedData* pEncData, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncData)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignString((char**)&(pEncData->szType), value);
  return err;
}

//--------------------------------------------------
// Mutatoror for MimeType atribute of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_SetMimeType(DEncEncryptedData* pEncData, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncData)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignString((char**)&(pEncData->szMimeType), value);
  return err;
}

//--------------------------------------------------
// Mutatoror for xmlns atribute of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_SetXmlNs(DEncEncryptedData* pEncData, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncData)
  RETURN_IF_NULL_PARAM(value)
  err = dencValidateEncXmlNs(value);
  if(err) return err;
  err = ddocMemAssignString((char**)&(pEncData->szXmlNs), value);
  return err;
}


//--------------------------------------------------
// Mutatoror for EncryptionMethod subelement of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_SetEncryptionMethod(DEncEncryptedData* pEncData, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncData)
  RETURN_IF_NULL_PARAM(value)
  // replace the buggy URI with correct one to enable
  // decrypting files with buggy URI but write only 
  // correct URI in new files
  if(!strcmp(value, DENC_ENC_METHOD_RSA1_5_BUGGY))
    value = DENC_ENC_METHOD_RSA1_5;
  err = dencEncDataValidateEncMethod(value);
  if(err) return err;
  err = ddocMemAssignString((char**)&(pEncData->szEncryptionMethod), value);
  return err;
}

//--------------------------------------------------
// Adds unencrypted data to encrypted data element
// waiting to be encrypted in next steps
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// data - new unencrypted data [REQUIRED]
// len - length of data. Use -1 for null terminated strings [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_AppendData(DEncEncryptedData* pEncData, const char* data, int len)
{
  ddocDebug(3, "dencEncryptedData_AppendData", "data: %s, len: %d", (data ? "OK" : "NULL"), len);
  RETURN_IF_NULL_PARAM(pEncData)
  RETURN_IF_NULL_PARAM(data)
  SET_LAST_ERROR_RETURN_IF_NOT(pEncData->nDataStatus == DENC_DATA_STATUS_UNINITIALIZED ||
			       pEncData->nDataStatus == DENC_DATA_STATUS_UNENCRYPTED_AND_NOT_COMPRESSED, 
			       ERR_DENC_DATA_STATUS, ERR_DENC_DATA_STATUS)
  if(pEncData->nDataStatus == DENC_DATA_STATUS_UNINITIALIZED)
    pEncData->nDataStatus = DENC_DATA_STATUS_UNENCRYPTED_AND_NOT_COMPRESSED;
  return ddocMemAppendData(&(pEncData->mbufEncryptedData), data, len);
}

//--------------------------------------------------
// Mutatoror for Id atribute of EncryptionProperties subelement of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_SetEncryptionPropertiesId(DEncEncryptedData* pEncData, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncData)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignString((char**)&(pEncData->encProperties.szId), value);
  return err;
}

//--------------------------------------------------
// Deletes EncryptionProperties subelement of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// nIdx - index of EncryptionProperty object to be removed [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_DeleteEncryptionProperty(DEncEncryptedData* pEncData, int nIdx)
{
  int err = ERR_OK, i;

  RETURN_IF_NULL_PARAM(pEncData)
  SET_LAST_ERROR_RETURN_IF_NOT(nIdx >= 0 && nIdx < pEncData->encProperties.nEncryptionProperties, ERR_DENC_BAD_PROP_IDX, ERR_DENC_BAD_PROP_IDX);
  RETURN_IF_NULL_PARAM(pEncData->encProperties.arrEncryptionProperties[nIdx]);
  // delete the given object
  err = dencEncryptionProperty_free(pEncData->encProperties.arrEncryptionProperties[nIdx]);
  if(err) return err;
  pEncData->encProperties.arrEncryptionProperties[nIdx] = 0;
  // move other objects 1 step close to array start
  for(i = nIdx; i < pEncData->encProperties.nEncryptionProperties-1; i++) 
    pEncData->encProperties.arrEncryptionProperties[i] =
      pEncData->encProperties.arrEncryptionProperties[i+1];
  pEncData->encProperties.arrEncryptionProperties[pEncData->encProperties.nEncryptionProperties - 1] = 0;
  pEncData->encProperties.nEncryptionProperties--;
  return err;
}

//--------------------------------------------------
// Deletes EncryptedKey subelement of DEncEncryptedData object.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// nIdx - index of EncryptedKey object to be removed [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_DeleteEncryptedKey(DEncEncryptedData* pEncData, int nIdx)
{
  int err = ERR_OK, i;

  RETURN_IF_NULL_PARAM(pEncData)
  SET_LAST_ERROR_RETURN_IF_NOT(nIdx >= 0 && nIdx < pEncData->nEncryptedKeys, ERR_DENC_BAD_KEY_IDX, ERR_DENC_BAD_KEY_IDX);
  RETURN_IF_NULL_PARAM(pEncData->arrEncryptedKeys[nIdx]);
  // delete the given object
  err = dencEncryptedKey_free(pEncData->arrEncryptedKeys[nIdx]);
  if(err) return err;
  pEncData->arrEncryptedKeys[nIdx] = 0;
  // move other objects 1 step closer to array start
  for(i = nIdx; i < pEncData->nEncryptedKeys-1; i++) 
    pEncData->arrEncryptedKeys[i] =
      pEncData->arrEncryptedKeys[i+1];
  pEncData->nEncryptedKeys--;
  pEncData->arrEncryptedKeys[pEncData->nEncryptedKeys - 1] = 0;
  return err;
}


//--------------------------------------------------
// Mutatoror for Id atribute of DEncEncryptedData object.
// Sets the default value - "ED0"
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_SetId_default(DEncEncryptedData* pEncData)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncData)
  err = ddocMemAssignString((char**)&(pEncData->szId), "ED0");
  return err;
}

//--------------------------------------------------
// Mutatoror for Type atribute of DEncEncryptedData object.
// Sets the default value - "http://www.sk.ee/DigiDoc/v1.3.0/digidoc.xsd"
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_SetType_default(DEncEncryptedData* pEncData)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncData)
  err = ddocMemAssignString((char**)&(pEncData->szType), DENC_ENCDATA_TYPE_DDOC);
  return err;
}

//======================< DEncEncryptionProperty >===================

//--------------------------------------------------
// "Constructor" for EncryptionProperty
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// pEncProperty - address of buffer for new property object [REQUIRED]
// szId - Id atribute of EncryptionProperty [OPTIONAL]
// szTarget - Target atribute of EncryptionProperty [OPTIONAL]
// szName - name atribute of EncryptionProperty [OPTIONAL]
// szContent - content of EncryptionProperty [OPTIONAL]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptionProperty_new(DEncEncryptedData* pEncData, 
			       DEncEncryptionProperty** ppEncProperty,
			       const char* szId, const char* szTarget,
			       const char* szName, const char* szContent)
{
  int err = ERR_OK, nProps;
  DEncEncryptionProperty **pProps, *pProp;

  // check parameters
  RETURN_IF_NULL_PARAM(pEncData)
  RETURN_IF_NULL_PARAM(ppEncProperty)
  *ppEncProperty = 0; // mark as not yet allocated
  // allocate memory for pointer array
  nProps = pEncData->encProperties.nEncryptionProperties + 1;
  pProps = (DEncEncryptionProperty **)realloc(pEncData->encProperties.arrEncryptionProperties, 
					      sizeof(DEncEncryptionProperty *) * nProps);
  if(!pProps)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
  pEncData->encProperties.arrEncryptionProperties = pProps;
  pProps[pEncData->encProperties.nEncryptionProperties] = 0;
  // allocate memory for new property
  pProp = (DEncEncryptionProperty*)malloc(sizeof(DEncEncryptionProperty));
  if(!pProp)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
  memset(pProp, 0, sizeof(DEncEncryptionProperty));
  pProps[pEncData->encProperties.nEncryptionProperties] = pProp;
  *ppEncProperty = pProp;
  pEncData->encProperties.nEncryptionProperties = nProps;
  // set data
  if(szId) {
    err = ddocMemAssignString((char**)&(pProp->szId), szId);
    if(err) return err;
  }
  if(szTarget) {
    err = ddocMemAssignString((char**)&(pProp->szTarget), szTarget);
    if(err) return err;
  }
  if(szName) {
    err = ddocMemAssignString((char**)&(pProp->szName), szName);
    if(err) return err;
  }
  if(szContent) {
    err = ddocMemAssignString((char**)&(pProp->szContent), szContent);
    if(err) return err;
  }
  return err;
}

//--------------------------------------------------
// "Destructor" for EncryptionProperty
// pEncProperty - address of buffer for new property object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptionProperty_free(DEncEncryptionProperty* pEncProperty)
{
  RETURN_IF_NULL_PARAM(pEncProperty)
  if(pEncProperty->szId)
    free(pEncProperty->szId);
  if(pEncProperty->szTarget)
    free(pEncProperty->szTarget);
  if(pEncProperty->szName)
    free(pEncProperty->szName);
  if(pEncProperty->szContent)
    free(pEncProperty->szContent);
  free(pEncProperty);
  return ERR_OK;
}

//======================< DEncEncryptionProperty - accessors >===================

//--------------------------------------------------
// Accessor for Id atribute of EncryptionProperty object.
// pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* dencEncryptionProperty_GetId(DEncEncryptionProperty* pEncProp)
{
  RETURN_OBJ_IF_NULL(pEncProp, NULL)
  return pEncProp->szId;
}

//--------------------------------------------------
// Accessor for Target atribute of EncryptionProperty object.
// pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* dencEncryptionProperty_GetTarget(DEncEncryptionProperty* pEncProp)
{
  RETURN_OBJ_IF_NULL(pEncProp, NULL)
  return pEncProp->szTarget;
}

//--------------------------------------------------
// Accessor for Name atribute of EncryptionProperty object.
// pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* dencEncryptionProperty_GetName(DEncEncryptionProperty* pEncProp)
{
  RETURN_OBJ_IF_NULL(pEncProp, NULL)
  return pEncProp->szName;
}

//--------------------------------------------------
// Accessor for content of EncryptionProperty object.
// pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* dencEncryptionProperty_GetContent(DEncEncryptionProperty* pEncProp)
{
  RETURN_OBJ_IF_NULL(pEncProp, NULL)
  return pEncProp->szContent;
}

//======================< DEncEncryptionProperty - mutators >===================

//--------------------------------------------------
// Mutatoror for Id atribute of DEncEncryptionProperty object.
// pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptionProperty_SetId(DEncEncryptionProperty* pEncProp, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncProp)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignString((char**)&(pEncProp->szId), value);
  return err;
}

//--------------------------------------------------
// Mutatoror for Target atribute of DEncEncryptionProperty object.
// pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptionProperty_SetTarget(DEncEncryptionProperty* pEncProp, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncProp)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignString((char**)&(pEncProp->szTarget), value);
  return err;
}

//--------------------------------------------------
// Mutatoror for Name atribute of DEncEncryptionProperty object.
// pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptionProperty_SetName(DEncEncryptionProperty* pEncProp, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncProp)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignString((char**)&(pEncProp->szName), value);
  return err;
}

//--------------------------------------------------
// Mutatoror for content of DEncEncryptionProperty object.
// pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptionProperty_SetContent(DEncEncryptionProperty* pEncProp, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncProp)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignString((char**)&(pEncProp->szContent), value);
  return err;
}


//======================< DEncEncryptedKey >===================

//--------------------------------------------------
// Initializes transport key and init vector.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int dencEncryptedData_initTransportKey(DEncEncryptedData* pEncData)
{
  int err = ERR_OK, l1;
  char salt[8], indata[128], key[16];

  RETURN_IF_NULL_PARAM(pEncData)
  // init random generator
#ifdef WIN32
  RAND_screen();
  RAND_bytes((unsigned char*)salt, sizeof(salt));
  RAND_bytes((unsigned char*)indata, sizeof(indata));
  RAND_bytes((unsigned char*)pEncData->initVector, sizeof(pEncData->initVector));
#else
  if(l1 = RAND_load_file("/dev/urandom", 1024)) {
    ddocDebug(4, "dencEncryptedData_initTransportKey", "rand load: %d", l1);
    RAND_bytes((unsigned char*)salt, sizeof(salt));
    RAND_bytes((unsigned char*)indata, sizeof(indata));
    RAND_bytes((unsigned char*)pEncData->initVector, sizeof(pEncData->initVector));
  }
#endif
  // intialize IV
  memset(key, 0, sizeof(key));
  l1 = EVP_BytesToKey(EVP_aes_128_cbc(), EVP_md5(), (const unsigned char*)salt, (const unsigned char*)indata, sizeof(indata), 1, (unsigned char*)key, NULL);
  ddocDebug(3, "dencEncryptedData_initTransportKey", "BytesToKey: %d", l1);
  err = ddocMemAssignData(&(pEncData->mbufTransportKey), key, sizeof(key));
  if(err) return err;
  ddocDebug(3, "dencEncryptedData_initTransportKey", "RC: %d key: %d", 
	    l1, pEncData->mbufTransportKey.nLen);
  pEncData->nKeyStatus = DENC_KEY_STATUS_INITIALIZED;
  return err;
}

//--------------------------------------------------
// Validates the <EncryptionMethod> parameter. We
// currently support only RSA-1.5
// szEncMethod - value to be checked
// returns error code or ERR_OK
//--------------------------------------------------
int dencEncKeyValidateEncMethod(const char* szEncMethod)
{
  if(szEncMethod && strcmp(szEncMethod, DENC_ENC_METHOD_RSA1_5))
    SET_LAST_ERROR_RETURN(ERR_DENC_ENC_METHOD, ERR_DENC_ENC_METHOD)
  else
    return ERR_OK;
}


//--------------------------------------------------
// Encrypts data with RSA public key (receivers key?)
// pCert - receivers certificate
// data - input data
// dLen - input data length
// result - encrypted data
// resLen - output buffer length / used bytes
//--------------------------------------------------
int dencEncryptWithCert(X509* pCert, const char* data, int dLen, char* result, int* resLen)
{
  int err = ERR_OK;
  EVP_PKEY* pkey;

  // check parameters
  RETURN_IF_NULL_PARAM(pCert)
  RETURN_IF_NULL_PARAM(data)
  RETURN_IF_NULL_PARAM(result)
  RETURN_IF_NULL_PARAM(resLen)
  // get certificates public key	
  err = GetPublicKey(&pkey, pCert);
  if(err) return err;
  // encrypt data
  memset((char*)result, 0, *resLen);
  *resLen = RSA_public_encrypt(dLen, (const unsigned char*)data, (unsigned char*)result, pkey->pkey.rsa, RSA_PKCS1_PADDING);
  // cleanup
  EVP_PKEY_free(pkey); // should I ???

  return err;
}


//--------------------------------------------------
// "Constructor" for EncryptedKey
// Encrypts the transport key for a receiver
// and stores encrypted key in memory
// Call this function repeatedly for all receivers,
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// pEncKey - address of buffer for new encrypted key object [REQUIRED]
// pCert - recevers certificate [REQUIRED]
// szEncMethod - encryption method [REQUIRED]
// szId - Id atribute of EncryptedKey [OPTIONAL]
// szRecipient - Recipient atribute of EncryptedKey [OPTIONAL]
// szKeyName - KeyName subelement of EncryptedKey [OPTIONAL]
// szCarriedKeyName - CarriedKeyName subelement of EncryptedKey [OPTIONAL]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedKey_new(DEncEncryptedData* pEncData, 
				    DEncEncryptedKey** pEncKey, X509* pCert,
				    const char* szEncMethod, const char* szId,
				    const char* szRecipient, const char* szKeyName,
				    const char* szCarriedKeyName)
{
  int err = ERR_OK, nKeys, l1;
  DEncEncryptedKey **pKeys, *pKey;
  char tkey[130];

  ddocDebug(3, "dencEncryptedKey_new", "cert: %s, method: %s, id: %s, recipient: %s, keyname: %s carriedkeyname: %s", 
      (pCert ? "OK" : "NULL"), szEncMethod, szId, szRecipient, szKeyName, szCarriedKeyName);
  // check parameters
  RETURN_IF_NULL_PARAM(pEncData)
  RETURN_IF_NULL_PARAM(pEncKey)
  err = dencEncKeyValidateEncMethod(szEncMethod);
  if(err) return err;
  *pEncKey = 0;
  // increase the buffer for EncryptedKey pointers
  nKeys = pEncData->nEncryptedKeys + 1;
  pKeys = (DEncEncryptedKey **)realloc(pEncData->arrEncryptedKeys, sizeof(DEncEncryptedKey *) * nKeys);
  if(!pKeys)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
  pEncData->arrEncryptedKeys = pKeys;
  pEncData->arrEncryptedKeys[pEncData->nEncryptedKeys] = 0;
  pKey = (DEncEncryptedKey*)malloc(sizeof(DEncEncryptedKey));
  if(!pKey)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
  pEncData->arrEncryptedKeys[pEncData->nEncryptedKeys] = pKey;
  pEncData->nEncryptedKeys = nKeys;
  *pEncKey = pKey;
  memset(pKey, 0, sizeof(DEncEncryptedKey));
  // set required parameters
  if(szEncMethod) {
    err = ddocMemAssignString((char**)&(pKey->szEncryptionMethod), szEncMethod);
    if(err) return err;
  }
  if(pCert)
    pKey->pCert = pCert;
  // set optional parameters
  if(szId) {
    err = ddocMemAssignString((char**)&(pKey->szId), szId);
    if(err) return err;
  }
  if(szRecipient) {
    err = ddocMemAssignString((char**)&(pKey->szRecipient), szRecipient);
    if(err) return err;
  }
  if(szKeyName) {
    err = ddocMemAssignString((char**)&(pKey->szKeyName), szKeyName);
    if(err) return err;
  }
  if(szCarriedKeyName) {
    err = ddocMemAssignString((char**)&(pKey->szCarriedKeyName), szCarriedKeyName);
    if(err) return err;
  }
  if(pCert) {
    // encrypt the key
    if(pEncData->nKeyStatus == DENC_KEY_STATUS_UNINITIALIZED) {
      err = dencEncryptedData_initTransportKey(pEncData);
      if(err) return err;
    }
    l1 = sizeof(tkey);
    err = dencEncryptWithCert(pCert, (const char*)pEncData->mbufTransportKey.pMem, 
			      pEncData->mbufTransportKey.nLen, tkey, &l1);
    if(err) return err;
    err = ddocMemAssignData(&(pKey->mbufTransportKey), tkey, l1);
  }
  return err;
}

//--------------------------------------------------
// "Destructor" for EncryptedKey
// pEncKey - address of buffer for new encrypted key object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedKey_free(DEncEncryptedKey* pEncKey)
{
  RETURN_IF_NULL_PARAM(pEncKey)
  if(pEncKey->szId)
    free(pEncKey->szId);
  if(pEncKey->szRecipient)
    free(pEncKey->szRecipient);
  if(pEncKey->szEncryptionMethod)
    free(pEncKey->szEncryptionMethod);
  if(pEncKey->szKeyName)
    free(pEncKey->szKeyName);
  if(pEncKey->szCarriedKeyName)
    free(pEncKey->szCarriedKeyName);
  if(pEncKey->pCert)
    X509_free(pEncKey->pCert);
  ddocMemBuf_free(&(pEncKey->mbufTransportKey));
  free(pEncKey);
  return ERR_OK;
}

//======================< DEncEncryptedKey - acessors >===================


//--------------------------------------------------
// Accessor for Id atribute of DEncEncryptedKey object.
// pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* dencEncryptedKey_GetId(DEncEncryptedKey* pEncKey)
{
  RETURN_OBJ_IF_NULL(pEncKey, NULL)
  return pEncKey->szId;
}

//--------------------------------------------------
// Accessor for Recipient atribute of DEncEncryptedKey object.
// pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* dencEncryptedKey_GetRecipient(DEncEncryptedKey* pEncKey)
{
  RETURN_OBJ_IF_NULL(pEncKey, NULL)
  return pEncKey->szRecipient;
}

//--------------------------------------------------
// Accessor for EncryptionMethod subelement of DEncEncryptedKey object.
// pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* dencEncryptedKey_GetEncryptionMethod(DEncEncryptedKey* pEncKey)
{
  RETURN_OBJ_IF_NULL(pEncKey, NULL)
  return pEncKey->szEncryptionMethod;
}

//--------------------------------------------------
// Accessor for KeyName subelement of DEncEncryptedKey object.
// pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* dencEncryptedKey_GetKeyName(DEncEncryptedKey* pEncKey)
{
  RETURN_OBJ_IF_NULL(pEncKey, NULL)
  return pEncKey->szKeyName;
}

//--------------------------------------------------
// Accessor for CarriedKeyName subelement of DEncEncryptedKey object.
// pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION const char* dencEncryptedKey_GetCarriedKeyName(DEncEncryptedKey* pEncKey)
{
  RETURN_OBJ_IF_NULL(pEncKey, NULL)
  return pEncKey->szCarriedKeyName;
}

//--------------------------------------------------
// Accessor for certificate of DEncEncryptedKey object.
// pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
// returns value of atribute or NULL.
//--------------------------------------------------
EXP_OPTION X509* dencEncryptedKey_GetCertificate(DEncEncryptedKey* pEncKey)
{
  RETURN_OBJ_IF_NULL(pEncKey, NULL)
  return pEncKey->pCert;
}

//======================< DEncEncryptedKey - mutators >===================

//--------------------------------------------------
// Mutatoror for Id atribute of DEncEncryptedKey object.
// pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedKey_SetId(DEncEncryptedKey* pEncKey, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncKey)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignString((char**)&(pEncKey->szId), value);
  return err;
}

//--------------------------------------------------
// Mutatoror for Recipient atribute of DEncEncryptedKey object.
// pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedKey_SetRecipient(DEncEncryptedKey* pEncKey, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncKey)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignString((char**)&(pEncKey->szRecipient), value);
  return err;
}

//--------------------------------------------------
// Mutatoror for EncryptionMethod subelement of DEncEncryptedKey object.
// pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedKey_SetEncryptionMethod(DEncEncryptedKey* pEncKey, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncKey)
  RETURN_IF_NULL_PARAM(value)
  err = dencEncKeyValidateEncMethod(value);
  if(err) return err;
  err = ddocMemAssignString((char**)&(pEncKey->szEncryptionMethod), value);
  return err;
}

//--------------------------------------------------
// Mutatoror for KeyName subelement of DEncEncryptedKey object.
// pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedKey_SetKeyName(DEncEncryptedKey* pEncKey, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncKey)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignString((char**)&(pEncKey->szKeyName), value);
  return err;
}

//--------------------------------------------------
// Mutatoror for CarriedKeyName subelement of DEncEncryptedKey object.
// pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedKey_SetCarriedKeyName(DEncEncryptedKey* pEncKey, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pEncKey)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAssignString((char**)&(pEncKey->szCarriedKeyName), value);
  return err;
}

//--------------------------------------------------
// Mutatoror for certificate of DEncEncryptedKey object.
// pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
// value - new value for atribute [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedKey_SetCertificate(DEncEncryptedKey* pEncKey, X509* value)
{
  RETURN_IF_NULL_PARAM(pEncKey)
  RETURN_IF_NULL_PARAM(value)
  pEncKey->pCert = value;
  return ERR_OK;
}

//==========< general crypto fucntions >============

//--------------------------------------------------
// Locates the correct EncryptedKey object by reading
// users certificate from smartcard and searching the
// right EncryptedKey object
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// ppEncKey - address of a buffer for EncryptedKey pointer [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_findEncryptedKeyByPKCS11(DEncEncryptedData* pEncData, DEncEncryptedKey** ppEncKey)
{
  int err = ERR_OK, i, n;
  X509 *pCert = 0;
  DEncEncryptedKey *pEncKey = 0;
  char buf1[100], buf2[100];

  RETURN_IF_NULL_PARAM(pEncData)
  RETURN_IF_NULL_PARAM(ppEncKey)
  *ppEncKey = 0; // mark as not found yet
  err = findUsersCertificate(0, &pCert);  
  if(err) return err;
  memset(buf1, 0, sizeof(buf1));
  err = ReadCertSerialNumber(buf1, sizeof(buf1), pCert);
  if(err) return err;
  n = dencEncryptedData_GetEncryptedKeyCount(pEncData);
  for(i = 0; i < n; i++) {
    pEncKey = dencEncryptedData_GetEncryptedKey(pEncData, i);
    memset(buf2, 0, sizeof(buf2));
    err = ReadCertSerialNumber(buf2, sizeof(buf2), pEncKey->pCert);
    if(!err && !strcmp(buf1, buf2)) {
      *ppEncKey = pEncKey;
      break;
    }
  }
  if(pCert)
    X509_free(pCert);
  if(!*ppEncKey)
    SET_LAST_ERROR_RETURN(ERR_DENC_NO_KEY_FOUND, ERR_DENC_NO_KEY_FOUND)
  return err;
}



//--------------------------------------------------
// AES encrypt/decrypt operations
// pInData - input data
// pOutData - output buffer
// pKey - AES key
// operation - ENCRYPT/DECRYPT
// iv - init vector for the cipher
//--------------------------------------------------
int encryptDecrypt(DigiDocMemBuf *pInData, DigiDocMemBuf *pOutData,
		   DigiDocMemBuf *pKey, int operation, const char* iv)
{
  EVP_CIPHER_CTX ectx;
  int err = ERR_OK, len, i, nInLen, nOutLen;
  char padBuf[16], *pInMem;
  int lOrigLen, lEncLen;

  ddocDebug(3, "encryptDecrypt", "Input: %d, output: %d, key: %d, operation: %s", 
	    (pInData ? pInData->nLen : 0), (pOutData ? pOutData->nLen : 0),
	    (pKey ? pKey->nLen : 0), (operation ? "ENCRYPT" : "DECRYPT"));
  RETURN_IF_NULL_PARAM(pInData)
  RETURN_IF_NULL_PARAM(pOutData)
  RETURN_IF_NULL_PARAM(pKey)
  nInLen = pInData->nLen;
  pInMem = (char*)pInData->pMem;
  // use the first 16 bytes as IV
  // and remove this data from data to be decrypted
  if(operation == DECRYPT) {
    memcpy((char*)iv, (const char*)pInData->pMem, 16);
    //for(i = 0; i < 16; i++)
    //  ddocDebug(3, "encryptDecrypt", "IV pos: %d = %d", i, iv[i]);
    //nInLen -= 16;
    pInMem += 16;
    ddocDebug(3, "encryptDecrypt", "DECRYPT using iv input left: %d", nInLen);
  }
  lOrigLen = lEncLen = 0;
  len = nInLen % 16;
  if(len && operation == ENCRYPT) {
    len = 16 - (nInLen % 16);
    ddocDebug(3, "encryptDecrypt", "Input len: %d adding padding: %d\n", nInLen, len);
    memset(padBuf, 0, sizeof(padBuf));
    for(i = 0; i < len; i++)
      padBuf[i] = 0;
    padBuf[len-1] = (unsigned char)len; // number of padded chars
  }
  if(pOutData->pMem)
    free(pOutData->pMem);
  pOutData->nLen = nInLen * 2 + len * 2;
  if(operation == ENCRYPT)
    pOutData->nLen += 16;
  nOutLen = pOutData->nLen;
  pOutData->pMem = (char*)malloc(pOutData->nLen);
  memset(pOutData->pMem, 0, pOutData->nLen);
  if(!pOutData->pMem)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
  ddocDebug(3, "encryptDecrypt", "Allocated: %d", pOutData->nLen);
  // copy init vector to begin of output data
  if(operation == ENCRYPT)
    memcpy(pOutData->pMem, iv, 16);
  EVP_CIPHER_CTX_init(&ectx);
  EVP_CipherInit_ex(&ectx, EVP_aes_128_cbc(), NULL, (const unsigned char*)pKey->pMem, (const unsigned char*)iv, operation);
  //checkErrors();
  lOrigLen += nInLen;
  i = nOutLen;
  // set initial position for encryted data
  if(operation == ENCRYPT)
    pOutData->nLen = 16;
  else
    pOutData->nLen = 0;
  EVP_CipherUpdate(&ectx, (unsigned char*)pOutData->pMem + pOutData->nLen, &i, (const unsigned char*)pInMem, nInLen);
  lEncLen += i;
  pOutData->nLen += i;
  ddocDebug(3, "encryptDecrypt", "Initial update: %d into: %d -> %d", nInLen, nOutLen, i);
  if(len && operation == ENCRYPT) {
    EVP_CipherUpdate(&ectx, (unsigned char*)pOutData->pMem + pOutData->nLen, &i, (const unsigned char*)padBuf, len);
    ddocDebug(3, "encryptDecrypt", "Padding update: %d -> %d", len, i);
    pOutData->nLen += i;
    lOrigLen += len;
    nOutLen -= i;
    lEncLen += i;
  }
  i = nOutLen;
  EVP_CipherFinal_ex(&ectx, (unsigned char*)pOutData->pMem + pOutData->nLen, &i);
  ddocDebug(3, "encryptDecrypt", "Final update: %d into: %d", i, nOutLen);
  pOutData->nLen += i;
  lEncLen += i;
  ddocDebug(3, "encryptDecrypt", "Total input: %d encrypted: %d", lOrigLen, lEncLen);
  EVP_CIPHER_CTX_cleanup(&ectx);
  if(operation == DECRYPT) {
    // if the last 16 bytes are all 0x0F then remove this block 
    // and continue evaluationg padding
    len = (int)(unsigned char)((char*)pOutData->pMem)[pOutData->nLen-1];
    if(len == 16) {
      ddocDebug(3, "encryptDecrypt", "check padding: %d", len);
      for(i = pOutData->nLen - 16; i < pOutData->nLen - 1; i++) {
	//ddocDebug(3, "encryptDecrypt", "Byte at: %d = %d", i, ((char*)pOutData->pMem)[i]);
	if(((char*)pOutData->pMem)[i] != 16) {
	  len = 0; // set not matched flag
	  //break;
	}
      }
      if(len) {
	ddocDebug(3, "encryptDecrypt", "Decrypted len: %d reduce by: %d", pOutData->nLen, len);
	pOutData->nLen -= len;
      }
      else
	ddocDebug(3, "encryptDecrypt", "Decrypted len remains: %d", pOutData->nLen);
    }
    // check PKCS7padding
    len = (int)(unsigned char)((char*)pOutData->pMem)[pOutData->nLen-1];
    if(len > 0 && len < 16) {
      ddocDebug(3, "encryptDecrypt", "check padding: %d", len);
      // check if all previous are 0-s
      for(i = pOutData->nLen - len; i < pOutData->nLen - 1; i++) {
	ddocDebug(3, "encryptDecrypt", "Byte at: %d = %d", i, ((char*)pOutData->pMem)[i]);
	if(((char*)pOutData->pMem)[i]) {
	  len = 0; // set not matched flag
	  //break;
	}
      }
      if(len) {
	ddocDebug(3, "encryptDecrypt", "Decrypted len: %d reduce by: %d", pOutData->nLen, len);
	pOutData->nLen -= len;
      }
      else
	ddocDebug(3, "encryptDecrypt", "Decrypted len remains: %d", pOutData->nLen);
    }
    else
      ddocDebug(3, "encryptDecrypt", "Impossible padding: %d", len);
  }
  return err;
}

//--------------------------------------------------
// Encrypts data with the generated key
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// nCompressOption - flag: DENC_COMPRESS_ALLWAYS, 
//    DENC_COMPRESS_NEVER or DENC_COMPRESS_BEST_EFFORT
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_encryptData(DEncEncryptedData* pEncData, int nCompressOption)
{
  int err = ERR_OK;
  DigiDocMemBuf outData;

  RETURN_IF_NULL_PARAM(pEncData)
  // check compression
  if(nCompressOption == DENC_COMPRESS_ALLWAYS ||
     nCompressOption == DENC_COMPRESS_BEST_EFFORT) {
    err = dencEncryptedData_compressData(pEncData, nCompressOption);
    if(err) return err;
  }
  // check data status
  if(pEncData->nDataStatus != DENC_DATA_STATUS_UNENCRYPTED_AND_NOT_COMPRESSED &&
     pEncData->nDataStatus != DENC_DATA_STATUS_UNENCRYPTED_AND_COMPRESSED)
    SET_LAST_ERROR_RETURN(ERR_DENC_DATA_STATUS, ERR_DENC_DATA_STATUS)
  // init transport key if necessary
  if(pEncData->nKeyStatus == DENC_KEY_STATUS_UNINITIALIZED) {
    err = dencEncryptedData_initTransportKey(pEncData);
    if(err) return err;
  }
  outData.pMem = 0;
  outData.nLen = 0;
  err = encryptDecrypt(&(pEncData->mbufEncryptedData), &outData, 
		       &(pEncData->mbufTransportKey), ENCRYPT,
		       pEncData->initVector);
  if(!err) {
    free(pEncData->mbufEncryptedData.pMem);
    pEncData->mbufEncryptedData.pMem = outData.pMem;
    pEncData->mbufEncryptedData.nLen = outData.nLen;
    // check if we have original length in <EncryptionProperties>
    if(pEncData->nDataStatus == DENC_DATA_STATUS_UNENCRYPTED_AND_NOT_COMPRESSED)
      pEncData->nDataStatus = DENC_DATA_STATUS_ENCRYPTED_AND_NOT_COMPRESSED;
    else
      pEncData->nDataStatus = DENC_DATA_STATUS_ENCRYPTED_AND_COMPRESSED;
  }
  return err;
}

//--------------------------------------------------
// Decrypts data with the generated key
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_decryptData(DEncEncryptedData* pEncData)
{
  int err = ERR_OK;
  DigiDocMemBuf outData;

  RETURN_IF_NULL_PARAM(pEncData)
  // check data status
  if(pEncData->nDataStatus != DENC_DATA_STATUS_ENCRYPTED_AND_NOT_COMPRESSED &&
     pEncData->nDataStatus != DENC_DATA_STATUS_ENCRYPTED_AND_COMPRESSED)
    SET_LAST_ERROR_RETURN(ERR_DENC_DATA_STATUS, ERR_DENC_DATA_STATUS)
  // check transport key
  if(pEncData->nKeyStatus == DENC_KEY_STATUS_UNINITIALIZED) 
    SET_LAST_ERROR_RETURN(ERR_DENC_KEY_STATUS, ERR_DENC_KEY_STATUS)
  outData.pMem = 0;
  outData.nLen = 0;
  err = encryptDecrypt(&(pEncData->mbufEncryptedData), &outData, 
		       &(pEncData->mbufTransportKey), DECRYPT,
		       pEncData->initVector);
  if(!err) {
    free(pEncData->mbufEncryptedData.pMem);
    pEncData->mbufEncryptedData.pMem = outData.pMem;
    pEncData->mbufEncryptedData.nLen = outData.nLen;
    if(pEncData->nDataStatus == DENC_DATA_STATUS_ENCRYPTED_AND_NOT_COMPRESSED)
      pEncData->nDataStatus = DENC_DATA_STATUS_UNENCRYPTED_AND_NOT_COMPRESSED;
    else
      pEncData->nDataStatus = DENC_DATA_STATUS_UNENCRYPTED_AND_COMPRESSED;
  }
  // decompress if necessary
  if(!err && pEncData->nDataStatus == DENC_DATA_STATUS_UNENCRYPTED_AND_COMPRESSED)
    err = dencEncryptedData_decompressData(pEncData);
  return err;
}


//--------------------------------------------------
// Decrypts data transport key with ID card and
// then decrypts the data with the transport key.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// pEncKey - transport key to decrypt [REQUIRED]
// pin - pin code for smart card [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_decrypt(DEncEncryptedData* pEncData, 
					 DEncEncryptedKey* pEncKey, 
					 const char* pin)
{
  int err = ERR_OK, nSlot, len;
  DEncEncryptionProperty* pEncProp;
  long l;
  
  RETURN_IF_NULL_PARAM(pEncData)
  RETURN_IF_NULL_PARAM(pEncKey)
  RETURN_IF_NULL_PARAM(pin)
  // check data status
  if(pEncData->nDataStatus != DENC_DATA_STATUS_ENCRYPTED_AND_NOT_COMPRESSED &&
     pEncData->nDataStatus != DENC_DATA_STATUS_ENCRYPTED_AND_COMPRESSED)
    SET_LAST_ERROR_RETURN(ERR_DENC_DATA_STATUS, ERR_DENC_DATA_STATUS)
  pEncData->nKeyStatus = DENC_KEY_STATUS_UNINITIALIZED;
  nSlot = ConfigItem_lookup_int("DIGIDOC_AUTH_KEY_SLOT", 0);
  // it will shrink during decrypt so this is enough
  pEncData->mbufTransportKey.nLen = pEncKey->mbufTransportKey.nLen; 
  pEncData->mbufTransportKey.pMem = (char*)malloc(pEncData->mbufTransportKey.nLen);
  if(!pEncData->mbufTransportKey.pMem)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
  ddocDebug(4, "dencEncryptedData_decrypt", "Decrypt enckey: %d into: %d",
	    pEncKey->mbufTransportKey.nLen, pEncData->mbufTransportKey.nLen);
  // MSTERN
  len = pEncData->mbufTransportKey.nLen;
  err = decryptWithEstID(nSlot, pin,
	 (const char*)pEncKey->mbufTransportKey.pMem, pEncKey->mbufTransportKey.nLen,
	 (char*)pEncData->mbufTransportKey.pMem, &len);
  pEncData->mbufTransportKey.nLen = len;
  if(err) return err;
  pEncData->nKeyStatus = DENC_KEY_STATUS_INITIALIZED;
  err = dencEncryptedData_decryptData(pEncData);
  pEncProp = dencEncryptedData_FindEncryptionPropertyByName(pEncData, ENCPROP_ORIG_SIZE);
  if(pEncProp && pEncProp->szContent) {
    l = atol(pEncProp->szContent);
    if(l > 0 && l < pEncData->mbufEncryptedData.nLen) {
      ddocDebug(4, "dencEncryptedData_decrypt", "Truncating decrypted data: %d to: %d",
		pEncData->mbufEncryptedData.nLen, l);
      pEncData->mbufEncryptedData.nLen = l;
    }
  }
  return err;
}


//--------------------------------------------------
// Decrypts data transport key with ID card and
// then decrypts the data with the transport key.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// tKey - decrypted transport key [REQUIRED]
// keyLen - length of trasnport key [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_decrypt_withKey(DEncEncryptedData* pEncData, 
						 const char* tKey, int keyLen)
{
  int err = ERR_OK;
  DEncEncryptionProperty* pEncProp;
  long l;

  RETURN_IF_NULL_PARAM(pEncData)
  RETURN_IF_NULL_PARAM(tKey)
  // check data status
  if(pEncData->nDataStatus != DENC_DATA_STATUS_ENCRYPTED_AND_NOT_COMPRESSED &&
     pEncData->nDataStatus != DENC_DATA_STATUS_ENCRYPTED_AND_COMPRESSED)
    SET_LAST_ERROR_RETURN(ERR_DENC_DATA_STATUS, ERR_DENC_DATA_STATUS)
  pEncData->mbufTransportKey.nLen = keyLen; 
  pEncData->mbufTransportKey.pMem = (char*)malloc(pEncData->mbufTransportKey.nLen);
  if(!pEncData->mbufTransportKey.pMem)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
  memcpy(pEncData->mbufTransportKey.pMem, tKey, keyLen);
  ddocDebug(4, "dencEncryptedData_decrypt_withKey", "Decrypt enckey: %d into: %d",
	    keyLen, pEncData->mbufTransportKey.nLen);
  pEncData->nKeyStatus = DENC_KEY_STATUS_INITIALIZED;
  err = dencEncryptedData_decryptData(pEncData);
  pEncProp = dencEncryptedData_FindEncryptionPropertyByName(pEncData, ENCPROP_ORIG_SIZE);
  if(pEncProp && pEncProp->szContent) {
    l = atol(pEncProp->szContent);
    if(l > 0 && l < pEncData->mbufEncryptedData.nLen) {
      ddocDebug(4, "dencEncryptedData_decrypt_withKey", "Truncating decrypted data: %d to: %d",
		pEncData->mbufEncryptedData.nLen, l);
      pEncData->mbufEncryptedData.nLen = l;
    }
  }
  return err;
}


//--------------------------------------------------
// Compresses data with ZLIB. Cannot compress encrypted data!!!
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// nCompressOption - flag: DENC_COMPRESS_ALLWAYS, 
//    DENC_COMPRESS_NEVER or DENC_COMPRESS_BEST_EFFORT
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_compressData(DEncEncryptedData* pEncData, int nCompressOption)
{
  int err = ERR_OK;
  DigiDocMemBuf outData;
  char buf[20];
  DEncEncryptionProperty* pEncProperty;

  RETURN_IF_NULL_PARAM(pEncData)
    // check compress option
  RETURN_IF_NOT(nCompressOption == DENC_COMPRESS_ALLWAYS ||
		nCompressOption == DENC_COMPRESS_BEST_EFFORT, ERR_COMPRESS)
  // check data status - cannot compress encrypted data!!!
  if(pEncData->nDataStatus != DENC_DATA_STATUS_UNENCRYPTED_AND_NOT_COMPRESSED)
    SET_LAST_ERROR_RETURN(ERR_DENC_DATA_STATUS, ERR_DENC_DATA_STATUS)
	//AM 28.05.08 2048 instead of 1024, because some bigger files got -5 (Z_BUF_ERROR)
  outData.nLen = pEncData->mbufEncryptedData.nLen + 2048; // it should get smaller
  outData.pMem = malloc(outData.nLen);
  if(!outData.pMem)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
  err = compress OF(( (Bytef *)outData.pMem, (uLongf *)&outData.nLen,
      (const Bytef *)pEncData->mbufEncryptedData.pMem, (uLong)pEncData->mbufEncryptedData.nLen));
  ddocDebug(3, "dencEncryptedData_compressData", "Orig len: %d, compressed: %d, RC: %d",
	    pEncData->mbufEncryptedData.nLen, outData.nLen, err);
  if(err != Z_OK) {
    free(outData.pMem);
    SET_LAST_ERROR_RETURN(ERR_COMPRESS, ERR_COMPRESS)
  }
  if(!err && 
     (nCompressOption == DENC_COMPRESS_ALLWAYS) ||
     (nCompressOption == DENC_COMPRESS_BEST_EFFORT && 
      outData.nLen < pEncData->mbufEncryptedData.nLen)) {
    snprintf(buf, sizeof(buf), "%d", pEncData->mbufEncryptedData.nLen);
    err = dencEncryptionProperty_new(pEncData, &pEncProperty,
				     NULL, NULL, ENCPROP_ORIG_SIZE, buf);
    free(pEncData->mbufEncryptedData.pMem);
    pEncData->mbufEncryptedData.pMem = outData.pMem;
    pEncData->mbufEncryptedData.nLen = outData.nLen;
    pEncData->nDataStatus = DENC_DATA_STATUS_UNENCRYPTED_AND_COMPRESSED;
    // store original mime type
    if(pEncData->szMimeType) {
      pEncProperty = 0;
      err = dencEncryptionProperty_new(pEncData, &pEncProperty,
				       NULL, NULL, ENCPROP_ORIG_MIME, pEncData->szMimeType);
      free(pEncData->szMimeType);
    }
    pEncData->szMimeType = (char*)strdup(DENC_ENCDATA_MIME_ZLIB);
  } else 
    free(outData.pMem);
  return err;
}

//--------------------------------------------------
// Decompresses data with ZLIB. 
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// nCompressOption - flag: DENC_COMPRESS_ALLWAYS, 
//    DENC_COMPRESS_NEVER or DENC_COMPRESS_BEST_EFFORT
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptedData_decompressData(DEncEncryptedData* pEncData)
{
  int err = ERR_OK;
  long origLen;
  DigiDocMemBuf outData;
  DEncEncryptionProperty* pEncProp;

  RETURN_IF_NULL_PARAM(pEncData)
  // check data status - cannot decompress encrypted data!!!
  if(pEncData->nDataStatus != DENC_DATA_STATUS_UNENCRYPTED_AND_COMPRESSED)
    SET_LAST_ERROR_RETURN(ERR_DENC_DATA_STATUS, ERR_DENC_DATA_STATUS)
  // find original data size
  pEncProp = dencEncryptedData_FindEncryptionPropertyByName(pEncData, ENCPROP_ORIG_SIZE);
  if(pEncProp && pEncProp->szContent) {
    origLen = atol(pEncProp->szContent);
    outData.nLen = (int)origLen;
  } else
    SET_LAST_ERROR_RETURN(ERR_DECOMPRESS, ERR_DECOMPRESS)
  outData.pMem = malloc(outData.nLen);
  if(!outData.pMem)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
  err = uncompress OF(( (Bytef *)outData.pMem, (uLongf *)&outData.nLen,
      (const Bytef *)pEncData->mbufEncryptedData.pMem, (uLong)pEncData->mbufEncryptedData.nLen));
  ddocDebug(3, "dencEncryptedData_decompressData", "Compressed len: %d, orig-len: %ld, uncompressed: %d, RC: %d",
	    pEncData->mbufEncryptedData.nLen, origLen, outData.nLen, err);
	if(err != Z_OK) {
    free(outData.pMem);
    SET_LAST_ERROR_RETURN(ERR_DECOMPRESS, ERR_DECOMPRESS)
  }
  if(!err) {
    free(pEncData->mbufEncryptedData.pMem);
    pEncData->mbufEncryptedData.pMem = outData.pMem;
    pEncData->mbufEncryptedData.nLen = outData.nLen;
    pEncData->nDataStatus = DENC_DATA_STATUS_UNENCRYPTED_AND_NOT_COMPRESSED;
    if(pEncData->szMimeType)
      free(pEncData->szMimeType);
    // restor original mime type ?
    pEncData->szMimeType = 0;
    // find original mime type
    pEncProp = dencEncryptedData_FindEncryptionPropertyByName(pEncData, ENCPROP_ORIG_MIME);
    if(pEncProp && pEncProp->szContent)
      pEncData->szMimeType = (char*)strdup(pEncProp->szContent);
  } else 
    free(outData.pMem);
  return err;
}

//--------------------------------------------------
// Encrypts a file and writes it to output file
// The caller must have initialized the transport keys
// but not the data.
// pEncData - pointer to DEncEncryptedData object [REQUIRED]
// szInputFileName - input data name [REQUIRED]
// szOutputFileName - output file name [REQUIRED]
// szMimeType - input data mime type [OPTIONAL]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencEncryptFile(DEncEncryptedData* pEncData, 
			       const char* szInputFileName, const char* szOutputFileName,
			       const char* szMimeType)
{
  int err = ERR_OK, l1, l2, l3, i, nBlock;
  long lOrigLen, lEncSize, lWritten;
  EVP_CIPHER_CTX ectx;
  EVP_ENCODE_CTX bctx;
  char convInFileName[250], convOutFileName[250];
  char buf1[4096], buf2[5120], buf3[6144], buf4[70], *p2;
  DigiDocMemBuf mbuf;
  FILE *hInFile, *hOutFile;
  DEncEncryptionProperty* pEncProperty;

  RETURN_IF_NULL_PARAM(pEncData)
  RETURN_IF_NULL_PARAM(szInputFileName)
  RETURN_IF_NULL_PARAM(szOutputFileName)
  ddocDebug(3, "dencEncryptFile", "In-file: %s, out-file: %s, key: %d", 
	    szInputFileName, szOutputFileName, pEncData->mbufTransportKey.nLen);
  // check data status - must be uninitialized since we will read it from file
  if(pEncData->nDataStatus != DENC_DATA_STATUS_UNINITIALIZED)
    SET_LAST_ERROR_RETURN(ERR_DENC_DATA_STATUS, ERR_DENC_DATA_STATUS)
  // check key status - must have been initialized already!
  if(pEncData->nKeyStatus == DENC_KEY_STATUS_UNINITIALIZED)
    SET_LAST_ERROR_RETURN(ERR_DENC_KEY_STATUS, ERR_DENC_KEY_STATUS)
  // convert filenames
  ddocConvertFileName( convInFileName, sizeof(convInFileName), szInputFileName );
  ddocConvertFileName( convOutFileName, sizeof(convOutFileName), szOutputFileName );
  // TODO: compress the data

  // read and encrypt data
  memset(buf4, 0, sizeof(buf4));
  nBlock = 0;
  if((hInFile = fopen(convInFileName, "rb")) != NULL) {
    if((hOutFile = fopen(convOutFileName, "wb")) != NULL) {
      // write header
      mbuf.pMem = 0;
      mbuf.nLen = 0;
      err = dencGenEncryptedData_header_toXML(pEncData, &mbuf);
      fwrite(mbuf.pMem, 1, mbuf.nLen, hOutFile);
      ddocMemBuf_free(&mbuf);
      // init encryption
      EVP_CIPHER_CTX_init(&ectx);
      // init encoding
      EVP_EncodeInit(&bctx);
      EVP_CipherInit_ex(&ectx, EVP_aes_128_cbc(), NULL, 
			(const unsigned char*)pEncData->mbufTransportKey.pMem,
			(const unsigned char*)pEncData->initVector, ENCRYPT);
      lOrigLen = 0;
      lEncSize = 0;
      lWritten = 0;
      // read file & encrypt & write to output
      do {
	memset(buf1, 0, sizeof(buf1));
	l1 = fread(buf1, 1, sizeof(buf1), hInFile);
	if(l1 > 0) {
	  lOrigLen += l1;
	  // padding for final block
	  if(l1 != sizeof(buf1)) {
	    l2 = 16 - (l1 % 16);
	    ddocDebug(3, "dencEncryptFile", "Original %d padding: %d, new block: %d", l1, l2, l1+l2);
	    for(i = 0; i < l2; i++)
	      buf1[l1 + i] = 0;
	    buf1[l1 + l2 - 1] = (unsigned char)l2;
	    l1 += l2;
	  }
	  memset(buf2, 0, sizeof(buf2));
	  l2 = sizeof(buf2);
	  p2 = buf2;
	  // first block of encrypted data will contain
	  // not encrypted IV vector in the first 16 bytes
	  if(nBlock == 0) {
	    memcpy(buf2, pEncData->initVector, 16);
	    p2 += 16;
	    l2 -= 16;
	  }
	  EVP_CipherUpdate(&ectx, (unsigned char*)p2, &l2, (const unsigned char*)buf1, l1);
	  ddocDebug(3, "dencEncryptFile", "Input: %d, block: %d, buf: %d encrypted: %d", l1, nBlock, sizeof(buf2), l2);
	  lEncSize += l2;
	  // if it's the final block
	  if(l1 != sizeof(buf1)) {
	    l3 = sizeof(buf2) - l2;
	    p2 = buf2 + l2;
	    if(nBlock == 0) {
	      p2 += 16;
	      l3 -= 16;
	    }
	    EVP_CipherFinal_ex(&ectx, (unsigned char*)p2, &l3);
	    ddocDebug(3, "dencEncryptFile", "Buf: %d Final encrypted: %d", sizeof(buf2) - l2, l3);
	    l2 += l3;
	    lEncSize += l3;
	  }
	  // base64 encode
	  l3 = sizeof(buf3);
	  memset(buf3, 0, l3);
	  // encode also the IV vector at the beginning of first block
	  if(nBlock == 0)
	    l2 += 16;
	  EVP_EncodeUpdate(&bctx, (unsigned char*)buf3, &l3, (byte*)buf2, l2);
	  lWritten += l3;
	  fwrite(buf3, 1, l3, hOutFile);
	  ddocDebug(3, "dencEncryptFile", "In: %d, encrypted: %d, base64: %d", l1, l2, l3);
	}
	nBlock++;
      } while(!err && l1 > 0);
      EVP_CIPHER_CTX_cleanup(&ectx);
      // write the last portion of line data
      l3 = sizeof(buf3);
      memset(buf3, 0, l3);
      EVP_EncodeFinal(&bctx, (unsigned char*)buf3, &l3);
      lWritten += l3;
      fwrite(buf3, 1, l3, hOutFile);
      ddocDebug(4, "dencEncryptFile", "Total input: %d, blocks: %d, encrypted: %d written: %d", lOrigLen, nBlock, lEncSize, lWritten);
      // setup encryption properties
      if(szMimeType) {
	pEncProperty = 0;
	err = dencEncryptionProperty_new(pEncData, &pEncProperty,
					 NULL, NULL, ENCPROP_ORIG_MIME, szMimeType);
      }
      snprintf(buf1, sizeof(buf1), "%ld", lOrigLen);
      pEncProperty = 0;
      err = dencEncryptionProperty_new(pEncData, &pEncProperty,
				       NULL, NULL, ENCPROP_ORIG_SIZE, buf1);
      pEncProperty = 0;
      err = dencEncryptionProperty_new(pEncData, &pEncProperty,
				       NULL, NULL, ENCPROP_FILENAME, szInputFileName);
      // write trailer
      mbuf.pMem = 0;
      mbuf.nLen = 0;
      err = dencGenEncryptedData_trailer_toXML(pEncData, &mbuf);
      fwrite(mbuf.pMem, 1, mbuf.nLen, hOutFile);
      ddocMemBuf_free(&mbuf);
      fclose(hOutFile);
    } else {
      SET_LAST_ERROR_RETURN_CODE(ERR_FILE_WRITE);
      ddocDebug(1, "dencEncryptFile", "Error writing encrypted document: %s", convOutFileName);
    }
    fclose(hInFile);
  } else {
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_READ);
    ddocDebug(1, "dencEncryptFile", "Error reading input from file: %s", convInFileName);
  }

  return err;
}


//====================< RecipientInfo functions >==========================

//--------------------------------------------------
// "Constructor" of DEncRecvInfo object
// ppRecvInfo - address of buffer for newly allocated object [REQUIRED]
// szId - recipients id [REQUIRED]
// szRecipient - recipient atribute [OPTIONAL]
// szKeyName - KeyName element [OPTIONAL]
// szCarriedKeyName - CarriedKeyName element [OPTIONAL]
// pCert - certificate [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencRecvInfo_new(DEncRecvInfo** ppRecvInfo, 
				const char* szId, const char* szRecipient, 
				const char* szKeyName, const char* szCarriedKeyName,
				const X509* pCert)
{
  int err = ERR_OK;

  // check input parameters
  ddocDebug(3, "dencRecvInfo_new", "RecvInfo id: %s, recipient: %s, keyname: %s, ckeyname: %s cert: %s",
	    szId, szRecipient, szKeyName, szCarriedKeyName, (pCert ? "OK" : "NULL"));
  RETURN_IF_NULL_PARAM(ppRecvInfo);
  RETURN_IF_NULL_PARAM(szId);
  RETURN_IF_NULL_PARAM(pCert);

  *ppRecvInfo = (DEncRecvInfo*)malloc(sizeof(DEncRecvInfo));
  // allocate new object
  RETURN_IF_BAD_ALLOC(*ppRecvInfo);
  memset(*ppRecvInfo, 0, sizeof(DEncRecvInfo));
  // set required fields
  if(szId) {
    err = ddocMemAssignString((char**)&((*ppRecvInfo)->szId), szId);
    if(err) return err;
  }
  (*ppRecvInfo)->pCert = (X509*)pCert;
  // set optional fields
  if(szRecipient) {
    err = ddocMemAssignString((char**)&((*ppRecvInfo)->szRecipient), szRecipient);
    if(err) return err;
  }
  if(szKeyName) {
    err = ddocMemAssignString((char**)&((*ppRecvInfo)->szKeyName), szKeyName);
    if(err) return err;
  }
  if(szCarriedKeyName) {
    err = ddocMemAssignString((char**)&((*ppRecvInfo)->szCarriedKeyName), szCarriedKeyName);
    if(err) return err;
  }
  return err;
}

//--------------------------------------------------
// "Destructor" of DEncRecvInfo object
// pRecvInfo - address of buffer for newly allocated object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencRecvInfo_free(DEncRecvInfo* pRecvInfo)
{
  int err = ERR_OK;

  RETURN_IF_NULL_PARAM(pRecvInfo);
  if(pRecvInfo->szId)
    free(pRecvInfo->szId);
  if(pRecvInfo->szRecipient)
    free(pRecvInfo->szRecipient);
  if(pRecvInfo->szKeyName)
    free(pRecvInfo->szKeyName);
  if(pRecvInfo->szCarriedKeyName)
    free(pRecvInfo->szCarriedKeyName);
  if(pRecvInfo->pCert)
    X509_free(pRecvInfo->pCert);
  return err;
}

//--------------------------------------------------
// Replaces newlines with blanks
// data - data with newlines
// returns data without newlines
//--------------------------------------------------
char* removeNewlines(char* data)
{
  int i, n, j;
  n = strlen(data);
  for(i = j = 0; i < n; i++) {
    if(data[i] != '\n' && data[i] != '\r') {
      data[j] = data[i];
      j++;
    }
  }
  if(j < i)
    data[j] = 0;
  return data;
}

//--------------------------------------------------
// Stores DEncRecvInfo object to configuration store
// pRecvInfo - address of RecvInfo object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencRecvInfo_store(DEncRecvInfo* pRecvInfo)
{
  int err = ERR_OK;
  char key[100], *p;

  RETURN_IF_NULL_PARAM(pRecvInfo);
  RETURN_IF_NULL_PARAM(pRecvInfo->szId);
  RETURN_IF_NULL_PARAM(pRecvInfo->pCert);
  if(!err) {
    snprintf(key, sizeof(key), "RECV_%s_RECIPIENT", pRecvInfo->szId);
    if(pRecvInfo->szRecipient)
      err = createOrReplacePrivateConfigItem(NULL, key, removeNewlines(pRecvInfo->szRecipient));
    else
      err = ConfigItem_delete(key);
  }
  if(!err) {
    snprintf(key, sizeof(key), "RECV_%s_KEY_NAME", pRecvInfo->szId);
    if(pRecvInfo->szKeyName)
      err = createOrReplacePrivateConfigItem(NULL, key, removeNewlines(pRecvInfo->szKeyName));
    else
      err = ConfigItem_delete(key);
  }
  if(!err) {
    snprintf(key, sizeof(key), "RECV_%s_CARRIED_KEY_NAME", pRecvInfo->szId);
    if(pRecvInfo->szCarriedKeyName)
      err = createOrReplacePrivateConfigItem(NULL, key, removeNewlines(pRecvInfo->szCarriedKeyName));
    else
      err = ConfigItem_delete(key);
  }
  if(!err && pRecvInfo->pCert) {
    snprintf(key, sizeof(key), "RECV_%s_CERT", pRecvInfo->szId);
    err = getCertPEM(pRecvInfo->pCert, 0, &p);
    if(!err && p) {
      err = createOrReplacePrivateConfigItem(NULL, key, removeNewlines(p));
      free(p);
    }
  }
  return err;
}

//--------------------------------------------------
// Deletes DEncRecvInfo object from configuration store
// pRecvInfo - address of RecvInfo [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencRecvInfo_delete(DEncRecvInfo* pRecvInfo)
{
  int err = ERR_OK;
  char key[100];

  RETURN_IF_NULL_PARAM(pRecvInfo);
  RETURN_IF_NULL_PARAM(pRecvInfo->szId);
  snprintf(key, sizeof(key), "RECV_%s_RECIPIENT", pRecvInfo->szId);
  err = ConfigItem_delete(key);
  snprintf(key, sizeof(key), "RECV_%s_KEY_NAME", pRecvInfo->szId);
  err = ConfigItem_delete(key);
  snprintf(key, sizeof(key), "RECV_%s_CARRIED_KEY_NAME", pRecvInfo->szId);
  err = ConfigItem_delete(key);
  snprintf(key, sizeof(key), "RECV_%s_CERT", pRecvInfo->szId);
  err = ConfigItem_delete(key);
  return err;
}


//--------------------------------------------------
// Finds the DEncRecvInfo object with the given id
// pConfStore - store to search in [OPTIONAL]. Use NULL for default
// ppRecvInfo - address of buffer for newly allocated object [REQUIRED]
// szId - id of the object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencRecvInfo_findById(ConfigurationStore *pConfStore, 
				     DEncRecvInfo** ppRecvInfo, const char* szId)
{
  int err = ERR_OK;
  char key[100], *p;
  X509* pCert = NULL;

  RETURN_IF_NULL_PARAM(ppRecvInfo);
  *ppRecvInfo = 0; // mark as not found
  // check for existence first
  snprintf(key, sizeof(key), "RECV_%s_CERT", szId);
  if(pConfStore)
    p = (char*)ConfigItem_lookup_fromStore(pConfStore, key);
  else
    p = (char*)ConfigItem_lookup(key);
  if(!p) return err;
  err = ddocDecodeX509PEMData(&pCert, (const char*)p, strlen((const char*)p));
  if(!pCert) return err;
  err = dencRecvInfo_new(ppRecvInfo, szId, NULL, NULL, NULL, pCert);
  // now set the optional items if possible
  snprintf(key, sizeof(key), "RECV_%s_RECIPIENT", szId);
  if(pConfStore)
    p = (char*)ConfigItem_lookup_fromStore(pConfStore, key);
  else
    p = (char*)ConfigItem_lookup(key);
  if(p)
    (*ppRecvInfo)->szRecipient = (char*)strdup(p);
  snprintf(key, sizeof(key), "RECV_%s_KEY_NAME", szId);
  if(pConfStore)
    p = (char*)ConfigItem_lookup_fromStore(pConfStore, key);
  else
    p = (char*)ConfigItem_lookup(key);
  if(p)
    (*ppRecvInfo)->szKeyName = (char*)strdup(p);
  snprintf(key, sizeof(key), "RECV_%s_CARRIED_KEY_NAME", szId);
  if(pConfStore)
    p = (char*)ConfigItem_lookup_fromStore(pConfStore, key);
  else
    p = (char*)ConfigItem_lookup(key);
  if(p)
    (*ppRecvInfo)->szCarriedKeyName = (char*)strdup(p);
  return err;
}

//--------------------------------------------------
// Returns all DEncRecvInfo objects
// pRecvInfoList - address of the list receiving the items [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencRecvInfo_findAll(DEncRecvInfoList* pRecvInfoList)
{
  int err = ERR_OK, i, j, k;
  char id[50];
  ConfigurationStore confStore;
  DEncRecvInfo* pRecvInfo;

  RETURN_IF_NULL_PARAM(pRecvInfoList);
  pRecvInfoList->nItems = 0;
  pRecvInfoList->pItems = NULL;
  confStore.nItems = 0;
  confStore.pItems = NULL;
  // search the id's
  err = ConfigItem_findByPrefix(&confStore, "RECV");
  ddocDebug(4, "dencRecvInfo_findAll", "RECV items: %d", confStore.nItems);
  for(i = 0; confStore.pItems && i < confStore.nItems; i++) {
    j = (confStore.pItems[i] && confStore.pItems[i]->szKey) ?
      strlen(confStore.pItems[i]->szKey) : 0;
    if(j && !strcmp(confStore.pItems[i]->szKey + j - 4, "CERT")) {
      j -= 6;
      for(k = j; k > 0 && confStore.pItems[i]->szKey[k] != '_'; k--);
      memset(id, 0, sizeof(id));
      strncpy(id, confStore.pItems[i]->szKey + k + 1, j - k);
      ddocDebug(4, "dencRecvInfo_findAll", "Read obj with id: %s", id);
      err = dencRecvInfo_findById(&confStore, &pRecvInfo, id);
      if(!err && pRecvInfo)
	err = dencRecvInfoList_add(pRecvInfoList, pRecvInfo);
    }
  }

  // cleanup
  cleanupConfigStore(&confStore);
  return err;
}

//====================< RecipientInfoList functions >==========================

//--------------------------------------------------
// Adds a DEncRecvInfo object to the list
// pRecvInfoList - address of the list receiving the item [REQUIRED]
// pRecvInfo - new object to be added
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencRecvInfoList_add(DEncRecvInfoList* pRecvInfoList, DEncRecvInfo *pRecvInfo)
{
  int err = ERR_OK;
  DEncRecvInfo** pItems;

  RETURN_IF_NULL_PARAM(pRecvInfoList);
  RETURN_IF_NULL_PARAM(pRecvInfo);
  pItems = (DEncRecvInfo**)realloc(pRecvInfoList->pItems, 
				   (pRecvInfoList->nItems + 1) * sizeof(DEncRecvInfo *));
  RETURN_IF_BAD_ALLOC(pItems);
  pRecvInfoList->pItems = pItems;
  pRecvInfoList->pItems[pRecvInfoList->nItems] = pRecvInfo;
  pRecvInfoList->nItems++;
  return err;
}

//--------------------------------------------------
// Frees the contents of a DEncRecvInfoList object
// pRecvInfoList - address of the list [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencRecvInfoList_free(DEncRecvInfoList* pRecvInfoList)
{
  int err = ERR_OK, i;

  RETURN_IF_NULL_PARAM(pRecvInfoList);
  for(i = 0; i < pRecvInfoList->nItems; i++) {
    if(pRecvInfoList->pItems[i])
      dencRecvInfo_free(pRecvInfoList->pItems[i]);
  }
  free(pRecvInfoList->pItems);
  pRecvInfoList->pItems = 0;
  pRecvInfoList->nItems = 0;
  return err;
}

//--------------------------------------------------
// Removes the given DEncRecvInfo object from the list
// pRecvInfoList - address of the list [REQUIRED]
// szId - id of the obect to be removed [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencRecvInfoList_delete(DEncRecvInfoList* pRecvInfoList, const char* szId)
{
  int err = ERR_OK, i, j;

  ddocDebug(4, "dencRecvInfoList_delete", "Deleting id: %s orig-len: %d", szId, pRecvInfoList->nItems);
  RETURN_IF_NULL_PARAM(pRecvInfoList);
  RETURN_IF_NULL_PARAM(szId);
  for(i = j = 0; i < pRecvInfoList->nItems; i++) {
    if(pRecvInfoList->pItems[j] && 
       pRecvInfoList->pItems[j]->szId &&
       !strcmp(pRecvInfoList->pItems[j]->szId, szId)) {
      free(pRecvInfoList->pItems[j]);
      ddocDebug(4, "dencRecvInfoList_delete", "Dleting item on pos: %d", j);
    } else {
      pRecvInfoList->pItems[j] = pRecvInfoList->pItems[i];
      j++;
    }
  }
  pRecvInfoList->nItems = j;
  ddocDebug(4, "dencRecvInfoList_delete", "remaining-len: %d", pRecvInfoList->nItems);
  return err;
}

//====================< original content functions >===================

//--------------------------------------------------
// Returns the count of "orig_file" properties
// pEncData - EncryptedData object [REQUIRED]
// returns count or -1 for error.
//--------------------------------------------------
EXP_OPTION int dencOrigContent_count(DEncEncryptedData* pEncData)
{
  int nCount = 0, i, n;
  DEncEncryptionProperty* pEncProp = 0;
  char* pName;

  SET_LAST_ERROR_RETURN_IF_NOT(pEncData, ERR_NULL_POINTER, -1)
  n = dencEncryptedData_GetEncryptionPropertiesCount(pEncData);
  for(i = 0; i < n; i++) {
    pEncProp = (DEncEncryptionProperty*)dencEncryptedData_GetEncryptionProperty(pEncData, i);
    if(pEncProp) {
      pName = (char*)dencEncryptionProperty_GetName(pEncProp);
      if(pName && !strcmp(pName, ENCPROP_ORIG_CONTENT))
	nCount++;
    }
  }
  return nCount;
}

//--------------------------------------------------
// Creates a new property of type "orig_file"
// pEncData - EncryptedData object [REQUIRED]
// szOrigContentId - Id atribute for new Property object [OPTIONAL]
// szName - original file name [REQUIRED]
// szSize - size as string or irginal file [REQUIRED]
// szMime - mime type or original file [REQUIRED]
// szDfId - Id atribute of original file [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencOrigContent_add(DEncEncryptedData* pEncData, const char* szOrigContentId,
   const char* szName, const char* szSize, const char* szMime, const char* szDfId)
{
  int err = ERR_OK, l1 = 0;
  char* p1 = 0;
  DEncEncryptionProperty* pEncProp = 0;

  RETURN_IF_NULL_PARAM(pEncData);
  RETURN_IF_NULL_PARAM(szName);
  RETURN_IF_NULL_PARAM(szSize);
  RETURN_IF_NULL_PARAM(szMime);
  RETURN_IF_NULL_PARAM(szDfId);
  l1 = strlen(szName) + strlen(szSize) + strlen(szMime) + strlen(szDfId) + 10;
  p1 = (char*)malloc(l1);
  RETURN_IF_BAD_ALLOC(p1);
  memset(p1, 0, l1);
  snprintf(p1, l1, "%s|%s|%s|%s", szName, szSize, szMime, szDfId);
  err = dencEncryptionProperty_new(pEncData, &pEncProp, szOrigContentId,
				   NULL, ENCPROP_ORIG_CONTENT, p1);
  if(p1)
    free(p1);
  return err;
}


//--------------------------------------------------
// Returns the info from "orig_file" properties
// pEncData - EncryptedData object [REQUIRED]
// szOrigContentId - Id atribute for new Property object [OPTIONAL]
// szName - buffer for original file name [REQUIRED]
// szSize - buffer for size as string or irginal file [REQUIRED]
// szMime - buffer for mime type or original file [REQUIRED]
// szDfId - buffer for Id atribute of original file [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencOrigContent_findByIndex(DEncEncryptedData* pEncData, int origContIdx,
     char* szName, char* szSize, char* szMime, char* szDfId)
{
  int nCount = -1, i, n, j, k, l, m;
  DEncEncryptionProperty* pEncProp = 0;
  char *p1;

  RETURN_IF_NULL_PARAM(pEncData);
  RETURN_IF_NULL_PARAM(szName);
  RETURN_IF_NULL_PARAM(szSize);
  RETURN_IF_NULL_PARAM(szMime);
  RETURN_IF_NULL_PARAM(szDfId);
  szName[0] = szSize[0] = szMime[0] = szDfId[0] = 0;
  n = dencEncryptedData_GetEncryptionPropertiesCount(pEncData);
  for(i = 0; i < n; i++) {
    pEncProp = (DEncEncryptionProperty*)dencEncryptedData_GetEncryptionProperty(pEncData, i);
    if(pEncProp) {
      p1 = (char*)dencEncryptionProperty_GetName(pEncProp);
      if(p1 && !strcmp(p1, ENCPROP_ORIG_CONTENT))
	nCount++;
      ddocDebug(4, "dencOrigContent_findByIndex", "Prop: %d, name: %s count: %d", i, p1, nCount);
      if(nCount == origContIdx && p1 && !strcmp(p1, ENCPROP_ORIG_CONTENT)) {
	p1 = (char*)dencEncryptionProperty_GetContent(pEncProp);
	ddocDebug(4, "dencOrigContent_findByIndex", "Prop: %d, count: %d, content: %s", i, nCount, p1); 
	if(p1) {
	  k = strlen(p1);
	  for(j = l = m = 0; j < k; j++) {
	    if(p1[j] == '|') {
	      switch(m) {
	      case 0: szName[l] = 0; break;
	      case 1: szSize[l] = 0; break;
	      case 2: szMime[l] = 0; break;
	      case 3: szDfId[l] = 0; break;
	      }
	      l = 0;
	      m++;
	    } else {
	      switch(m) {
	      case 0: szName[l] = p1[j]; l++; break;
	      case 1: szSize[l] = p1[j]; l++; break;
	      case 2: szMime[l] = p1[j]; l++; break;
	      case 3: szDfId[l] = p1[j]; l++; break;
	      }
	    }
	  }
	  ddocDebug(4, "dencOrigContent_findByIndex", "Prop: %d, name: %s size: %s, mime: %s, id: %s", i, szName, szSize, szMime, szDfId);
	}
      }
    }
  }
  return ERR_OK;
}

//--------------------------------------------------
// Checks if there is a digidoc document in this
// encrypted document.
// pEncData - EncryptedData object [REQUIRED]
// returns 1 if digidoc document is inside
//--------------------------------------------------
EXP_OPTION int dencOrigContent_isDigiDocInside(DEncEncryptedData* pEncData)
{
  int i, n;
  DEncEncryptionProperty* pEncProp = 0;
  char *p1, *p2;

  RETURN_IF_NULL_PARAM(pEncData);
  if(pEncData->szMimeType && !strcmp(pEncData->szMimeType, DENC_ENCDATA_TYPE_DDOC)) {
    ddocDebug(3, "dencOrigContent_isDigiDocInside", "mime: %s", pEncData->szMimeType);
    return 1;
  }
  n = dencEncryptedData_GetEncryptionPropertiesCount(pEncData);
  ddocDebug(3, "dencOrigContent_isDigiDocInside", "Props: %d", n);
  for(i = 0; i < n; i++) {
    pEncProp = (DEncEncryptionProperty*)dencEncryptedData_GetEncryptionProperty(pEncData, i);
    if(pEncProp) {
      p1 = (char*)dencEncryptionProperty_GetName(pEncProp);
      ddocDebug(3, "dencOrigContent_isDigiDocInside", "Prop: %d name: %s", i, p1);
      if(p1 && !strcmp(p1, ENCPROP_ORIG_MIME)) {
	p2 = (char*)dencEncryptionProperty_GetContent(pEncProp);
	ddocDebug(3, "dencOrigContent_isDigiDocInside", "Prop: %d mime: %s", i, p2);
	if(p2 && !strcmp(p2, DENC_ENCDATA_TYPE_DDOC)) {
	  ddocDebug(3, "dencOrigContent_isDigiDocInside", "Name: %s mime: %s", p1, p2);
	  return 1;
	}
      }
    }
  }
  return 0;
}

//--------------------------------------------------
// Registers digidoc document as encrypted datatype
// and stores it's data file info.
// pEncData - EncryptedData object [REQUIRED]
// pSigDoc - SignedDoc object [REQUIRED]
// returns 1 if digidoc document is inside
//--------------------------------------------------
EXP_OPTION int dencOrigContent_registerDigiDoc(DEncEncryptedData* pEncData, SignedDoc* pSigDoc)
{
  int err = ERR_OK, i, n;
  DEncEncryptionProperty* pEncProp = 0;
  DataFile* pDf;
  char buf[500];

  RETURN_IF_NULL_PARAM(pEncData);
  RETURN_IF_NULL_PARAM(pSigDoc);
  // set the mime type
  err = dencEncryptedData_SetMimeType(pEncData, DENC_ENCDATA_TYPE_DDOC);
  n = getCountOfDataFiles(pSigDoc);
  for(i = 0; i < n; i++) {
    pDf = getDataFile(pSigDoc, i);
    if(pDf) {
      pEncProp = 0;
      snprintf(buf, sizeof(buf), "%s|%ld|%s|%s", pDf->szFileName, pDf->nSize, pDf->szMimeType, pDf->szId);
      err = dencEncryptionProperty_new(pEncData, &pEncProp, 
		NULL, NULL, ENCPROP_ORIG_CONTENT, buf);
    }
  }
  return err;
}

//====================< other meta-info functions >===================

//--------------------------------------------------
// Sets the library name and version property
// pEncData - EncryptedData object [REQUIRED]
// returns count or -1 for error.
//--------------------------------------------------
EXP_OPTION int dencMetaInfo_SetLibVersion(DEncEncryptedData* pEncData)
{
  int err = ERR_OK;
  DEncEncryptionProperty* pEncProp = 0;
  char buf[500];

  RETURN_IF_NULL_PARAM(pEncData);
  // set the library name and version
  snprintf(buf, sizeof(buf), "%s|%s", getLibName(), getLibVersion());
  pEncProp = dencEncryptedData_FindEncryptionPropertyByName(pEncData, ENCPROP_LIB_VERSION);
  if(pEncProp) { // property exists, set the content
    err = dencEncryptionProperty_SetContent(pEncProp, (const char*)buf);
  } else { // create a new property
    err = dencEncryptionProperty_new(pEncData, &pEncProp, NULL, NULL, 
				     ENCPROP_LIB_VERSION, buf);
  }
  return err;
}

//--------------------------------------------------
// Sets the format name and version property
// pEncData - EncryptedData object [REQUIRED]
// returns count or -1 for error.
//--------------------------------------------------
EXP_OPTION int dencMetaInfo_SetFormatVersion(DEncEncryptedData* pEncData)
{
  int err = ERR_OK;
  DEncEncryptionProperty* pEncProp = 0;
  char buf[500];

  RETURN_IF_NULL_PARAM(pEncData);
  // set the library name and version
  snprintf(buf, sizeof(buf), "%s|%s", DENC_FORMAT_ENCDOC_XML, DENC_VERSION_1_0);
  pEncProp = dencEncryptedData_FindEncryptionPropertyByName(pEncData, ENCPROP_DOC_FORMAT);
  if(pEncProp) { // property exists, set the content
    err = dencEncryptionProperty_SetContent(pEncProp, (const char*)buf);
  } else { // create a new property
    err = dencEncryptionProperty_new(pEncData, &pEncProp, NULL, NULL, 
				     ENCPROP_DOC_FORMAT, buf);
  }
  return err;
}



//--------------------------------------------------
// Returns the library name and version meta-info of this document
// pEncData - EncryptedData object [REQUIRED]
// returns count or -1 for error.
//--------------------------------------------------
EXP_OPTION int dencMetaInfo_GetLibVersion(DEncEncryptedData* pEncData, char* szLibrary, char* szVersion)
{
  int err = ERR_OK, i, j, n;
  DEncEncryptionProperty* pEncProp = 0;
  char *p1;

  RETURN_IF_NULL_PARAM(pEncData);
  RETURN_IF_NULL_PARAM(szLibrary);
  RETURN_IF_NULL_PARAM(szVersion);
  // mark as not found
  szLibrary[0] = szVersion[0] = 0;
  pEncProp = dencEncryptedData_FindEncryptionPropertyByName(pEncData, ENCPROP_LIB_VERSION);
  if(pEncProp) { // property exists, set the content
    p1 = (char*)dencEncryptionProperty_GetContent(pEncProp);
    if(p1) {
      for(i = j = n = 0; p1[i]; i++) {
	if(p1[i] == '|') {
	  szLibrary[j] = 0;
	  j = 0;
	  n++;
	  continue;
	} else {
	  if(!n) 
	    szLibrary[j++] = p1[i];
	  else
	    szVersion[j++] = p1[i];
	}
      }
      szVersion[j] = 0;
    }
  } return err;
}


//--------------------------------------------------
// Returns the format name and version meta-info of this document
// pEncData - EncryptedData object [REQUIRED]
// returns count or -1 for error.
//--------------------------------------------------
EXP_OPTION int dencMetaInfo_GetFormatVersion(DEncEncryptedData* pEncData, char* szFormat, char* szVersion)
{
  int err = ERR_OK, i, j, n;
  DEncEncryptionProperty* pEncProp = 0;
  char *p1;

  RETURN_IF_NULL_PARAM(pEncData);
  RETURN_IF_NULL_PARAM(szFormat);
  RETURN_IF_NULL_PARAM(szVersion);
  // mark as not found
  szFormat[0] = szVersion[0] = 0;
  pEncProp = dencEncryptedData_FindEncryptionPropertyByName(pEncData, ENCPROP_DOC_FORMAT);
  if(pEncProp) { // property exists, set the content
    p1 = (char*)dencEncryptionProperty_GetContent(pEncProp);
    if(p1) {
      for(i = j = n = 0; p1[i]; i++) {
	if(p1[i] == '|') {
	  szFormat[j] = 0;
	  j = 0;
	  n++;
	  continue;
	} else {
	  if(!n) 
	    szFormat[j++] = p1[i];
	  else
	    szVersion[j++] = p1[i];
	}
      }
      szVersion[j] = 0;
    }
  } return err;
}

//--------------------------------------------------
// Deletes the meta-info properties
// pEncData - EncryptedData object [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencMetaInfo_deleteVersionInfo(DEncEncryptedData* pEncData)
{
  int err = ERR_OK, i, j;

  RETURN_IF_NULL_PARAM(pEncData);
  // delete doc-format property
  j = -1;
  for(i = 0; i < pEncData->encProperties.nEncryptionProperties; i++) {
    if(pEncData->encProperties.arrEncryptionProperties[i] &&
       pEncData->encProperties.arrEncryptionProperties[i]->szName &&
       !strcmp(pEncData->encProperties.arrEncryptionProperties[i]->szName, ENCPROP_DOC_FORMAT)) {
      j = i;
      break;
    }
  }
  if(j >= 0)
    err = dencEncryptedData_DeleteEncryptionProperty(pEncData, j);
  // delete lib-version property
  j = -1;
  for(i = 0; i < pEncData->encProperties.nEncryptionProperties; i++) {
    if(pEncData->encProperties.arrEncryptionProperties[i] &&
       pEncData->encProperties.arrEncryptionProperties[i]->szName &&
       !strcmp(pEncData->encProperties.arrEncryptionProperties[i]->szName, ENCPROP_LIB_VERSION)) {
      j = i;
      break;
    }
  }
  if(j >= 0)
    err = dencEncryptedData_DeleteEncryptionProperty(pEncData, j);
  return err;
}

