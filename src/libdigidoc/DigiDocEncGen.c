//==================================================
// FILE:	DigiDocEncGen.c
// PROJECT:     Digi Doc Encryption 
// DESCRIPTION: DigiDocEnc XML generation
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
//      11.10.2004      Veiko Sinivee
//                      Creation
//==================================================

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocEncGen.h>
#include <libdigidoc/DigiDocError.h>
#include <libdigidoc/DigiDocMem.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocConvert.h>
#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/DigiDocGen.h>
#include <string.h>


//-----------< XML generation functions >----------------------------

//--------------------------------------------------
// Generates XML for <EncryptionProperty> element
// pEncProperty - encryption property object [REQUIRED]
// pBuf - memory buffer for storing xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int dencGenEncryptionProperty_toXML(DEncEncryptionProperty* pEncProperty, DigiDocMemBuf* pBuf)
{
  int err = ERR_OK;
  char* p;

  RETURN_IF_NULL_PARAM(pBuf)
  RETURN_IF_NULL_PARAM(pEncProperty)
  // start of element
  err = ddocGen_startElemBegin(pBuf, "denc:EncryptionProperty");
  if(err) return err;
  // Id atribute
  p = (char*)dencEncryptionProperty_GetId(pEncProperty);
  if(p)
    err = ddocGen_addAtribute(pBuf, "Id", p);
  if(err) return err;
  // Target atribute
  p = (char*)dencEncryptionProperty_GetTarget(pEncProperty);
  if(p)
    err = ddocGen_addAtribute(pBuf, "Target", p);
  if(err) return err;
  // Name atribute
  p = (char*)dencEncryptionProperty_GetName(pEncProperty);
  if(p)
    err = ddocGen_addAtribute(pBuf, "Name", p);
  if(err) return err;
  // end of element start tag
  err = ddocGen_startElemEnd(pBuf);
  if(err) return err;
  // content
  p = (char*)dencEncryptionProperty_GetContent(pEncProperty);
  if(p)
    err = ddocMemAppendData(pBuf, p, -1);
  if(err) return err;
  err = ddocGen_endElem(pBuf, "denc:EncryptionProperty");
  return err;
}

//--------------------------------------------------
// Generates XML for <EncryptionProperties> element
// pEncData - encrypted data object [REQUIRED]
// pBuf - memory buffer for storing xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int dencGenEncryptionProperties_toXML(DEncEncryptedData* pEncData, DigiDocMemBuf* pBuf)
{
  int err = ERR_OK;
  char* p;
  int n, i;

  RETURN_IF_NULL_PARAM(pBuf)
  RETURN_IF_NULL_PARAM(pEncData)
  // count the prperties - do nothing if none exist
  n = dencEncryptedData_GetEncryptionPropertiesCount(pEncData);
  if(!n) return err;

  // start of element
  err = ddocGen_startElemBegin(pBuf, "denc:EncryptionProperties");
  if(err) return err;
  // Id atribute
  p = (char*)dencEncryptedData_GetEncryptionPropertiesId(pEncData);
  if(p)
    err = ddocGen_addAtribute(pBuf, "Id", p);
  if(err) return err;
  // end of element start tag
  err = ddocGen_startElemEnd(pBuf);
  if(err) return err;
  // content
  for(i = 0; i < n; i++) {
    DEncEncryptionProperty* pEncProp = dencEncryptedData_GetEncryptionProperty(pEncData, i);
    if(pEncProp)
      err = dencGenEncryptionProperty_toXML(pEncProp, pBuf);
    if(err) return err;
  }
  // end of element
  err = ddocGen_endElem(pBuf, "denc:EncryptionProperties");
  return err;
}

//--------------------------------------------------
// Generates XML for <EncryptedKey> element
// pEncKey - encrypted key object [REQUIRED]
// pBuf - memory buffer for storing xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int dencGenEncryptedKey_toXML(DEncEncryptedKey* pEncKey, DigiDocMemBuf* pBuf)
{
  int err = ERR_OK, l1;
  char* p;
  X509* pCert;

  RETURN_IF_NULL_PARAM(pBuf)
  RETURN_IF_NULL_PARAM(pEncKey)
  // start of element
  err = ddocGen_startElemBegin(pBuf, "denc:EncryptedKey");
  if(err) return err;
  // Id atribute
  p = (char*)dencEncryptedKey_GetId(pEncKey);
  if(p)
    err = ddocGen_addAtribute(pBuf, "Id", p);
  if(err) return err;
  // Recipient atribute
  p = (char*)dencEncryptedKey_GetRecipient(pEncKey);
  if(p)
    err = ddocGen_addAtribute(pBuf, "Recipient", p);
  if(err) return err;
  // end of element start tag
  err = ddocGen_startElemEnd(pBuf);
  if(err) return err;
  // <EncryptionMethod>
  p = (char*)dencEncryptedKey_GetEncryptionMethod(pEncKey);
  if(p) {
    err = ddocGen_startElemBegin(pBuf, "denc:EncryptionMethod");
    if(err) return err;
    err = ddocGen_addAtribute(pBuf, "Algorithm", p);
    if(err) return err;
    err = ddocGen_startElemEnd(pBuf);
    if(err) return err;
    err = ddocGen_endElem(pBuf, "denc:EncryptionMethod");
    if(err) return err;
  }
  // <KeyInfo>
  err = ddocGen_startElem(pBuf, "ds:KeyInfo");
  if(err) return err;
  // <KeyName>
  p = (char*)dencEncryptedKey_GetKeyName(pEncKey);
  if(p) {
    err = ddocGen_startElem(pBuf, "ds:KeyName");
    if(err) return err;
    err = ddocMemAppendData(pBuf, p, -1);
    if(err) return err;
    err = ddocGen_endElem(pBuf, "ds:KeyName");
    if(err) return err;
  }
  // <X509Data>/<X509Certificate>
  pCert = dencEncryptedKey_GetCertificate(pEncKey);
  if(pCert) {
    err = ddocGen_startElem(pBuf, "ds:X509Data");
    if(err) return err;
    err = ddocGen_startElem(pBuf, "ds:X509Certificate");
    if(err) return err;
    p = 0;
    err = getCertPEM(pCert, 0, &p);
    if(err) { 
      if(p) free(p); 
      return err; 
    }
    if(p) {
      err = ddocMemAppendData(pBuf, p, -1);
      free(p);
      p = 0;
      if(err) return err;
    }
    err = ddocGen_endElem(pBuf, "ds:X509Certificate");
    if(err) return err;
    err = ddocGen_endElem(pBuf, "ds:X509Data");
    if(err) return err;
  }
  // end of element <KeyInfo>
  err = ddocGen_endElem(pBuf, "ds:KeyInfo");
  if(err) return err;
  // transport key
  if(pEncKey->mbufTransportKey.pMem && pEncKey->mbufTransportKey.nLen) {
    l1 = 2 * pEncKey->mbufTransportKey.nLen;
    p = (char*)malloc(l1);
    if(!p)
      SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
    memset(p, 0, l1);
    encode((const byte*)pEncKey->mbufTransportKey.pMem, pEncKey->mbufTransportKey.nLen, (byte*)p, &l1);
    if(p && l1) {
      err = ddocGen_startElem(pBuf, "denc:CipherData");
      if(err) return err;
      err = ddocGen_startElem(pBuf, "denc:CipherValue");
      if(err) return err;
      err = ddocMemAppendData(pBuf, p, -1);
      free(p);
      p = 0;
      if(err) return err;
      err = ddocGen_endElem(pBuf, "denc:CipherValue");
      if(err) return err;
      err = ddocGen_endElem(pBuf, "denc:CipherData");
      if(err) return err;
    }
  }
  // <CarriedKeyName>
  p = (char*)dencEncryptedKey_GetCarriedKeyName(pEncKey);
  if(p) {
    err = ddocGen_startElem(pBuf, "denc:CarriedKeyName");
    if(err) return err;
    err = ddocMemAppendData(pBuf, p, -1);
    if(err) return err;
    err = ddocGen_endElem(pBuf, "denc:CarriedKeyName");
    if(err) return err;
  }
  // end of element <EncryptedKey>
  err = ddocGen_endElem(pBuf, "denc:EncryptedKey");
  return err;
}


//--------------------------------------------------
// Generates XML for <EncryptedData> element
// pEncData - encrypted data object [REQUIRED]
// pBuf - memory buffer for storing xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int dencGenEncryptedData_toXML(DEncEncryptedData* pEncData, DigiDocMemBuf* pBuf)
{
  int err = ERR_OK, l1;
  char* p;
  int i, n;

  RETURN_IF_NULL_PARAM(pBuf)
  RETURN_IF_NULL_PARAM(pEncData)
  // xml header
  err = ddocMemAppendData(pBuf, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>", -1);
  if(err) return err;
  // start of element
  err = ddocGen_startElemBegin(pBuf, "denc:EncryptedData");
  if(err) return err;
  // xmlns:denc
  //p = dencEncryptedData_GetXmlNs(pEncData);
  //if(p)
  err = ddocGen_addAtribute(pBuf, "xmlns:denc", DENC_XMLNS_XMLENC);
  if(err) return err;
  // Id atribute
  p = (char*)dencEncryptedData_GetId(pEncData);
  if(p)
    err = ddocGen_addAtribute(pBuf, "Id", p);
  if(err) return err;
  // Type atribute
  p = (char*)dencEncryptedData_GetType(pEncData);
  if(p)
    err = ddocGen_addAtribute(pBuf, "Type", p);
  if(err) return err;
  // MimeType atribute
  p = (char*)dencEncryptedData_GetMimeType(pEncData);
  if(p)
    err = ddocGen_addAtribute(pBuf, "MimeType", p);
  if(err) return err;
  // Encoding ???
  // end of element start tag
  err = ddocGen_startElemEnd(pBuf);
  if(err) return err;
  // <EncryptionMethod>
  p = (char*)dencEncryptedData_GetEncryptionMethod(pEncData);
  if(p) {
    err = ddocGen_startElemBegin(pBuf, "denc:EncryptionMethod");
    if(err) return err;
    err = ddocGen_addAtribute(pBuf, "Algorithm", p);
    if(err) return err;
    // end of element start tag
    err = ddocGen_startElemEnd(pBuf);
    if(err) return err;
    err = ddocGen_endElem(pBuf, "denc:EncryptionMethod");
    if(err) return err;
  }
  n = dencEncryptedData_GetEncryptedKeyCount(pEncData);
  if(n > 0) {
    // <KeyInfo>
    err = ddocGen_startElemBegin(pBuf, "ds:KeyInfo");
    if(err) return err;
    // xmlns ???
    err = ddocGen_addAtribute(pBuf, "xmlns:ds", DENC_XMLNS_XMLDSIG);
    if(err) return err;
    err = ddocGen_startElemEnd(pBuf);
    if(err) return err;
    // <EncryptedKey>
    for(i = 0; i < n; i++) {
      DEncEncryptedKey* pEncKey = dencEncryptedData_GetEncryptedKey(pEncData, i);
      if(pEncKey) {
	err = dencGenEncryptedKey_toXML(pEncKey, pBuf);
	if(err) return err;
      }
    }
    // end of element <KeyInfo>
    err = ddocGen_endElem(pBuf, "ds:KeyInfo");
    if(err) return err;
  }
  // encrypted data
  if(pEncData->mbufEncryptedData.pMem && pEncData->mbufEncryptedData.nLen) {
    l1 = 2 * pEncData->mbufEncryptedData.nLen;
    p = (char*)malloc(l1);
    if(!p)
      SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
    memset(p, 0, l1);
    encode((const byte*)pEncData->mbufEncryptedData.pMem, 
	   pEncData->mbufEncryptedData.nLen, (byte*)p, &l1);
    ddocDebug(4, "dencGenEncryptedData_toXML", "Input data: %d base64: %d", pEncData->mbufEncryptedData.nLen, l1);
    if(p && l1) {
      err = ddocGen_startElem(pBuf, "denc:CipherData");
      if(err) return err;
      err = ddocGen_startElem(pBuf, "denc:CipherValue");
      if(err) return err;
      err = ddocMemAppendData(pBuf, p, -1);
      free(p);
      p = 0;
      if(err) return err;
      err = ddocGen_endElem(pBuf, "denc:CipherValue");
      if(err) return err;
      err = ddocGen_endElem(pBuf, "denc:CipherData");
      if(err) return err;
    }
  }
  // <EncryptionProperties>
  if(dencEncryptedData_GetEncryptionPropertiesCount(pEncData) > 0)
    err = dencGenEncryptionProperties_toXML(pEncData, pBuf);
  if(err) return err;
  // end of element <EncryptedData>
  err = ddocGen_endElem(pBuf, "denc:EncryptedData");
  return err;
}

//--------------------------------------------------
// Writes encrypted data to a file
// pEncData - encrypted data object [REQUIRED]
// szFileName - name of the file to write the data [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencGenEncryptedData_writeToFile(DEncEncryptedData* pEncData, const char* szFileName)
{
  int err = ERR_OK;
  DigiDocMemBuf mbufEncData;
  FILE* hFile;
  char convFileName[250];

  ddocDebug(3, "dencGenEncryptedData_writeToFile", "filename: %s", szFileName);
  RETURN_IF_NULL_PARAM(pEncData)
  RETURN_IF_NULL_PARAM(szFileName)
  // start of element
  mbufEncData.pMem = 0;
  mbufEncData.nLen = 0;
  // handle file names, input is UTF-8
  ddocConvertFileName( convFileName, sizeof(convFileName), szFileName );
  err = dencGenEncryptedData_toXML(pEncData, &mbufEncData);
  if(!err && (hFile = fopen(convFileName, "wb")) != NULL) {
    fwrite(mbufEncData.pMem, mbufEncData.nLen, 1, hFile);
    fclose(hFile);
  } else {
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_WRITE);
    ddocDebug(1, "dencGenEncryptedData_writeToFile", "Error writing encrypted document");
  }
  // cleanup
  ddocMemBuf_free(&mbufEncData);
  return err;
}


//--------------------------------------------------
// Generates the header of XML for <EncryptedData> element
// This contains everything upto the start of base64 encoded cipher data
// pEncData - encrypted data object [REQUIRED]
// pBuf - memory buffer for storing xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int dencGenEncryptedData_header_toXML(DEncEncryptedData* pEncData, DigiDocMemBuf* pBuf)
{
  int err = ERR_OK;
  char* p;
  int i, n;

  RETURN_IF_NULL_PARAM(pBuf)
  RETURN_IF_NULL_PARAM(pEncData)
  // xml header
  err = ddocMemAppendData(pBuf, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>", -1);
  if(err) return err;
  // start of element
  err = ddocGen_startElemBegin(pBuf, "denc:EncryptedData");
  if(err) return err;
  // xmlns:denc
  //p = dencEncryptedData_GetXmlNs(pEncData);
  //if(p)
  err = ddocGen_addAtribute(pBuf, "xmlns:denc", DENC_XMLNS_XMLENC);
  if(err) return err;
  // Id atribute
  p = (char*)dencEncryptedData_GetId(pEncData);
  if(p)
    err = ddocGen_addAtribute(pBuf, "Id", p);
  if(err) return err;
  // Type atribute
  p = (char*)dencEncryptedData_GetType(pEncData);
  if(p)
    err = ddocGen_addAtribute(pBuf, "Type", p);
  if(err) return err;
  // MimeType atribute
  p = (char*)dencEncryptedData_GetMimeType(pEncData);
  if(p)
    err = ddocGen_addAtribute(pBuf, "MimeType", p);
  if(err) return err;
  // Encoding ???
  // end of element start tag
  err = ddocGen_startElemEnd(pBuf);
  if(err) return err;
  // <EncryptionMethod>
  p = (char*)dencEncryptedData_GetEncryptionMethod(pEncData);
  if(p) {
    err = ddocGen_startElemBegin(pBuf, "denc:EncryptionMethod");
    if(err) return err;
    err = ddocGen_addAtribute(pBuf, "Algorithm", p);
    if(err) return err;
    // end of element start tag
    err = ddocGen_startElemEnd(pBuf);
    if(err) return err;
    err = ddocGen_endElem(pBuf, "denc:EncryptionMethod");
    if(err) return err;
  }
  n = dencEncryptedData_GetEncryptedKeyCount(pEncData);
  if(n > 0) {
    // <KeyInfo>
    err = ddocGen_startElemBegin(pBuf, "ds:KeyInfo");
    if(err) return err;
    // xmlns ???
    err = ddocGen_addAtribute(pBuf, "xmlns:ds", DENC_XMLNS_XMLDSIG);
    if(err) return err;
    err = ddocGen_startElemEnd(pBuf);
    if(err) return err;
    // <EncryptedKey>
    for(i = 0; i < n; i++) {
      DEncEncryptedKey* pEncKey = dencEncryptedData_GetEncryptedKey(pEncData, i);
      if(pEncKey) {
	err = dencGenEncryptedKey_toXML(pEncKey, pBuf);
	if(err) return err;
      }
    }
    // end of element <KeyInfo>
    err = ddocGen_endElem(pBuf, "ds:KeyInfo");
    if(err) return err;
  }
  // encrypted data
  err = ddocGen_startElem(pBuf, "denc:CipherData");
  if(err) return err;
  err = ddocGen_startElem(pBuf, "denc:CipherValue");
  // here would come the base64 encoded cipher data
  return err;
}


//--------------------------------------------------
// Generates the trailer of XML for <EncryptedData> element
// These are all the XML constructs following the 
// base64 encoded cipher data.
// pEncData - encrypted data object [REQUIRED]
// pBuf - memory buffer for storing xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int dencGenEncryptedData_trailer_toXML(DEncEncryptedData* pEncData, DigiDocMemBuf* pBuf)
{
  int err = ERR_OK;

  RETURN_IF_NULL_PARAM(pBuf)
  RETURN_IF_NULL_PARAM(pEncData)
  //  here ends the base64 encoded cipher data
  err = ddocGen_endElem(pBuf, "denc:CipherValue");
  if(err) return err;
  err = ddocGen_endElem(pBuf, "denc:CipherData");
  if(err) return err;
  // <EncryptionProperties>
  if(dencEncryptedData_GetEncryptionPropertiesCount(pEncData) > 0)
    err = dencGenEncryptionProperties_toXML(pEncData, pBuf);
  if(err) return err;
  // end of element <EncryptedData>
  err = ddocGen_endElem(pBuf, "denc:EncryptedData");
  return err;
}

