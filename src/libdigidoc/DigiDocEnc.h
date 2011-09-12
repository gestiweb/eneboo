#ifndef __DIGIDOC_ENC_H__
#define __DIGIDOC_ENC_H__
//==================================================
// FILE:	DigiDocEnc.h
// PROJECT:     Digi Doc Encryption
// DESCRIPTION: DigiDocEnc structures
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

#include <openssl/x509.h>
#include <libdigidoc/DigiDocMem.h>
#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocDefs.h>

#ifdef  __cplusplus
extern "C" {
#endif


typedef struct DEncEncryptionProperty_st {
  char* szId;          // Id atribute value if present
  char* szTarget;      // Target atribute value if present
  char* szName;        // "name" atribute value if present - used in our implementation
  char* szContent;     // element content if used
  // TODO - other atributes ?
} DEncEncryptionProperty;

typedef struct DEncEncryptionProperties_st {
  char* szId;          // Id atribute value if present
  DEncEncryptionProperty** arrEncryptionProperties; // array of EncryptionProperty pointers
  int                      nEncryptionProperties;   // number of EncryptionProperty objects
} DEncEncryptionProperties;


typedef struct DEncEncrytedKey_st {
  char* szId;          // Id atribute value if present
  char* szRecipient;   // Recipient atribute value if present
  char* szEncryptionMethod; // EncryptionMethod element value
  char* szKeyName;     // KeyName element value if used
  char* szCarriedKeyName; // CarriedKeyName element value if used
  X509* pCert;         // receivers certificate - required in our implementation!
  DigiDocMemBuf mbufTransportKey; // encrypted transport key
} DEncEncryptedKey;

typedef struct DEncEncrytedData_st {
  char* szId;          // Id atribute value if present
  char* szType;        // Type atribute value if present
  char* szMimeType;    // MimeType atribute value if present
  char* szEncryptionMethod; // EncryptionMethod element value
  char* szXmlNs;       // XML namespace
  DEncEncryptedKey ** arrEncryptedKeys;
  int                  nEncryptedKeys;
  DigiDocMemBuf mbufEncryptedData;
  DEncEncryptionProperties encProperties;

  // private transient fields
  DigiDocMemBuf mbufTransportKey; // unencrypted transport key
  char initVector[16];
  // flags
  int nDataStatus;
  int nKeyStatus;
} DEncEncryptedData;

typedef struct DEncRecvInfo_st {
  char* szId;          // Id of recipient
  char* szRecipient;   // Recipient atribute value if present
  char* szKeyName;     // KeyName element value if used
  char* szCarriedKeyName; // CarriedKeyName element value if used
  X509* pCert;         // receivers certificate - required in our implementation!
} DEncRecvInfo;

typedef struct DEncRecvInfoList_st {
  int nItems;
  DEncRecvInfo** pItems;
} DEncRecvInfoList;



#define DENC_DATA_STATUS_UNINITIALIZED                      0
#define DENC_DATA_STATUS_UNENCRYPTED_AND_NOT_COMPRESSED     1
#define DENC_DATA_STATUS_UNENCRYPTED_AND_COMPRESSED         2
#define DENC_DATA_STATUS_ENCRYPTED_AND_NOT_COMPRESSED       3
#define DENC_DATA_STATUS_ENCRYPTED_AND_COMPRESSED           4

#define DENC_KEY_STATUS_UNINITIALIZED    0
#define DENC_KEY_STATUS_INITIALIZED      1
#define DENC_KEY_STATUS_DISCARDED        2

#define ENCPROP_FILENAME                 "Filename"
#define ENCPROP_ORIG_SIZE                "OriginalSize"
#define ENCPROP_ORIG_MIME                "OriginalMimeType"
#define ENCPROP_ORIG_CONTENT             "orig_file"
#define ENCPROP_LIB_VERSION              "LibraryVersion"
#define ENCPROP_DOC_FORMAT               "DocumentFormat"

#define DENC_FORMAT_ENCDOC_XML           "ENCDOC-XML"
#define DENC_VERSION_1_0                 "1.0"

#define DENC_COMPRESS_ALLWAYS            0
#define DENC_COMPRESS_NEVER              1
#define DENC_COMPRESS_BEST_EFFORT        2

#define DENC_ENCRYPTED_KEY_LEN           128
#define DENC_DECRYPTED_KEY_LEN           16

#define ENCRYPT		1
#define DECRYPT         0

#define DENC_ENCDATA_TYPE_DDOC   "http://www.sk.ee/DigiDoc/v1.3.0/digidoc.xsd"

#define DENC_ENCDATA_MIME_XML    "text/xml"
#define DENC_ENCDATA_MIME_ZLIB   "http://www.isi.edu/in-noes/iana/assignments/media-types/application/zip"

#define DENC_ENC_METHOD_AES128   "http://www.w3.org/2001/04/xmlenc#aes128-cbc"
#define DENC_ENC_METHOD_RSA1_5   "http://www.w3.org/2001/04/xmlenc#rsa-1_5"
#define DENC_ENC_METHOD_RSA1_5_BUGGY   "http://www.w3.org/2001/04/xmlenc#rsa-1-5"
#define DENC_XMLNS_XMLENC        "http://www.w3.org/2001/04/xmlenc#"
#define DENC_XMLNS_XMLENC_ELEMENT "http://www.w3.org/2001/04/xmlenc#Element"
#define DENC_XMLNS_XMLENC_CONTENT "http://www.w3.org/2001/04/xmlenc#Content"
#define DENC_XMLNS_XMLENC_ENCPROP "http://www.w3.org/2001/04/xmlenc#EncryptionProperties"
#define DENC_XMLNS_XMLDSIG        "http://www.w3.org/2000/09/xmldsig#"


//======================< DEncEncryptedData >==============================

  //--------------------------------------------------
  // "Constructor" of DEncEncryptedData object
  // pEncData - address of buffer for newly allocated object [REQUIRED]
  // szXmlNs - XML namespace uri [REQUIRED]
  // szEncMethod - encyrption method algorithm uri [REQUIRED]
  // szId - elements Id attribute [OPTIONAL]
  // szType - elements type atribute [OPTIONAL]
  // szMimeType - elements mime-type attribute [OPTIONAL]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_new(DEncEncryptedData** ppEncData, 
				       const char* szXmlNs, const char* szEncMethod,
				       const char* szId, const char* szType, 
				       const char* szMimeType);
  
  //--------------------------------------------------
  // "Destructor" of DEncEncryptedData object
  // pEncData - address of object to be deleted [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_free(DEncEncryptedData* pEncData);

//======================< DEncEncryptedData - accessors >===================

  //--------------------------------------------------
  // Accessor for Id atribute of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION const char* dencEncryptedData_GetId(DEncEncryptedData* pEncData);

  //--------------------------------------------------
  // Accessor for Type atribute of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION const char* dencEncryptedData_GetType(DEncEncryptedData* pEncData);

  //--------------------------------------------------
  // Accessor for MimeType atribute of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION const char* dencEncryptedData_GetMimeType(DEncEncryptedData* pEncData);

  //--------------------------------------------------
  // Accessor for xmlns atribute of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION const char* dencEncryptedData_GetXmlNs(DEncEncryptedData* pEncData);

  //--------------------------------------------------
  // Accessor for EncryptionMethod subelement of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION const char* dencEncryptedData_GetEncryptionMethod(DEncEncryptedData* pEncData);

  //--------------------------------------------------
  // Accessor for Id atribute of EncryptionProperties subelement of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION const char* dencEncryptedData_GetEncryptionPropertiesId(DEncEncryptedData* pEncData);

  //--------------------------------------------------
  // Accessor for count of EncryptionProperties subelement of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // returns count or -1 for error. Then use error API to check errors
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_GetEncryptionPropertiesCount(DEncEncryptedData* pEncData);

  //--------------------------------------------------
  // Accessor for EncryptionProperties subelement of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // nIdx - index of EncryptionProperty object [REQUIRED]
  // returns EncryptionProperty pointer or NULL for error
  //--------------------------------------------------
  EXP_OPTION DEncEncryptionProperty* dencEncryptedData_GetEncryptionProperty(DEncEncryptedData* pEncData, int nIdx);

  //--------------------------------------------------
  // Retrieves the last EncryptionProperty subelement of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // returns EncryptionProperty pointer or NULL for error
  //--------------------------------------------------
  EXP_OPTION DEncEncryptionProperty* dencEncryptedData_GetLastEncryptionProperty(DEncEncryptedData* pEncData);

  //--------------------------------------------------
  // Finds EncryptionProperty by Name atribute
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // name - name of searched property
  // returns EncryptionProperty pointer or NULL for error
  //--------------------------------------------------
  EXP_OPTION DEncEncryptionProperty* dencEncryptedData_FindEncryptionPropertyByName(DEncEncryptedData* pEncData, const char* name);

  //--------------------------------------------------
  // Accessor for count of EncryptedKey subelement of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // returns count or -1 for error. Then use error API to check errors
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_GetEncryptedKeyCount(DEncEncryptedData* pEncData);

  //--------------------------------------------------
  // Accessor for EncryptedKey subelement of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // nIdx - index of EncryptedKey object [REQUIRED]
  // returns EncryptedKey pointer or NULL for error
  //--------------------------------------------------
  EXP_OPTION DEncEncryptedKey* dencEncryptedData_GetEncryptedKey(DEncEncryptedData* pEncData, int nIdx);

  //--------------------------------------------------
  // Searches an EncryptedKey by recipients name
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // recipient - recipient name used to search the key [REQUIRED]
  // returns EncryptedKey pointer or NULL for error
  //--------------------------------------------------
  EXP_OPTION DEncEncryptedKey* dencEncryptedData_FindEncryptedKeyByRecipient(DEncEncryptedData* pEncData, const char* recipient);

  //--------------------------------------------------
  // Searches an EncryptedKey by certs CN field
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // cn - cert CN used to search the key [REQUIRED]
  // returns EncryptedKey pointer or NULL for error
  //--------------------------------------------------
  EXP_OPTION DEncEncryptedKey* dencEncryptedData_FindEncryptedKeyByCN(DEncEncryptedData* pEncData, const char* cn);

  //--------------------------------------------------
  // Accessor for EncryptedKey subelement of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // returns EncryptedKey pointer or NULL for error
  //--------------------------------------------------
  EXP_OPTION DEncEncryptedKey* dencEncryptedData_GetLastEncryptedKey(DEncEncryptedData* pEncData);

  //--------------------------------------------------
  // Accessor for encrypted data.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // ppBuf - address for encrypted data pointer [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_GetEncryptedData(DEncEncryptedData* pEncData, DigiDocMemBuf** ppBuf);

  //--------------------------------------------------
  // Accessor for encrypted data status flag.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // returns status or -1 for error. Then use error API to check errors
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_GetEncryptedDataStatus(DEncEncryptedData* pEncData);


//======================< DEncEncryptedData - mutators >===================

  //--------------------------------------------------
  // Mutatoror for Id atribute of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_SetId(DEncEncryptedData* pEncData, const char* value);

  //--------------------------------------------------
  // Mutatoror for Type atribute of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_SetType(DEncEncryptedData* pEncData, const char* value);

  //--------------------------------------------------
  // Mutatoror for MimeType atribute of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_SetMimeType(DEncEncryptedData* pEncData, const char* value);

  //--------------------------------------------------
  // Mutatoror for xmlns atribute of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_SetXmlNs(DEncEncryptedData* pEncData, const char* value);

  //--------------------------------------------------
  // Mutatoror for EncryptionMethod subelement of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_SetEncryptionMethod(DEncEncryptedData* pEncData, const char* value);

  //--------------------------------------------------
  // Adds unencrypted data to encrypted data element
  // waiting to be encrypted in next steps
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // data - new unencrypted data [REQUIRED]
  // len - length of data. Use -1 for null terminated strings [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_AppendData(DEncEncryptedData* pEncData, const char* data, int len);

  //--------------------------------------------------
  // Mutatoror for Id atribute of EncryptionProperties subelement of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_SetEncryptionPropertiesId(DEncEncryptedData* pEncData, const char* value);

  //--------------------------------------------------
  // Deletes EncryptionProperties subelement of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // nIdx - index of EncryptionProperty object to be removed [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_DeleteEncryptionProperty(DEncEncryptedData* pEncData, int nIdx);

  //--------------------------------------------------
  // Deletes EncryptedKey subelement of DEncEncryptedData object.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // nIdx - index of EncryptedKey object to be removed [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_DeleteEncryptedKey(DEncEncryptedData* pEncData, int nIdx);


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
				 DEncEncryptionProperty** pEncProperty,
				 const char* szId, const char* szTarget,
				 const char* szName, const char* szContent);

  //--------------------------------------------------
  // "Destructor" for EncryptionProperty
  // pEncProperty - address of buffer for new property object [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptionProperty_free(DEncEncryptionProperty* pEncProperty);

//======================< DEncEncryptionProperty - accessors >===================

  //--------------------------------------------------
  // Accessor for Id atribute of EncryptionProperty object.
  // pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION const char* dencEncryptionProperty_GetId(DEncEncryptionProperty* pEncProp);

  //--------------------------------------------------
  // Accessor for Target atribute of EncryptionProperty object.
  // pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION const char* dencEncryptionProperty_GetTarget(DEncEncryptionProperty* pEncProp);

  //--------------------------------------------------
  // Accessor for Name atribute of EncryptionProperty object.
  // pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION const char* dencEncryptionProperty_GetName(DEncEncryptionProperty* pEncProp);

  //--------------------------------------------------
  // Accessor for content of EncryptionProperty object.
  // pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION const char* dencEncryptionProperty_GetContent(DEncEncryptionProperty* pEncProp);

//======================< DEncEncryptionProperty - mutators >===================

  //--------------------------------------------------
  // Mutatoror for Id atribute of DEncEncryptionProperty object.
  // pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptionProperty_SetId(DEncEncryptionProperty* pEncProp, const char* value);

  //--------------------------------------------------
  // Mutatoror for Target atribute of DEncEncryptionProperty object.
  // pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptionProperty_SetTarget(DEncEncryptionProperty* pEncProp, const char* value);

  //--------------------------------------------------
  // Mutatoror for Name atribute of DEncEncryptionProperty object.
  // pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptionProperty_SetName(DEncEncryptionProperty* pEncProp, const char* value);

  //--------------------------------------------------
  // Mutatoror for content of DEncEncryptionProperty object.
  // pEncProp - pointer to DEncEncryptionProperty object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptionProperty_SetContent(DEncEncryptionProperty* pEncProp, const char* value);


//======================< DEncEncryptedKey >===================

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
				      const char* szCarriedKeyName);

  //--------------------------------------------------
  // "Destructor" for EncryptedKey
  // pEncKey - address of buffer for new encrypted key object [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedKey_free(DEncEncryptedKey* pEncKey);

//======================< DEncEncryptedKey - acessors >===================

  //--------------------------------------------------
  // Accessor for Id atribute of DEncEncryptedKey object.
  // pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION const char* dencEncryptedKey_GetId(DEncEncryptedKey* pEncKey);

  //--------------------------------------------------
  // Accessor for Recipient atribute of DEncEncryptedKey object.
  // pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION const char* dencEncryptedKey_GetRecipient(DEncEncryptedKey* pEncKey);

  //--------------------------------------------------
  // Accessor for EncryptionMethod subelement of DEncEncryptedKey object.
  // pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION const char* dencEncryptedKey_GetEncryptionMethod(DEncEncryptedKey* pEncKey);

  //--------------------------------------------------
  // Accessor for KeyName subelement of DEncEncryptedKey object.
  // pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION const char* dencEncryptedKey_GetKeyName(DEncEncryptedKey* pEncKey);

  //--------------------------------------------------
  // Accessor for CarriedKeyName subelement of DEncEncryptedKey object.
  // pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION const char* dencEncryptedKey_GetCarriedKeyName(DEncEncryptedKey* pEncKey);

  //--------------------------------------------------
  // Accessor for certificate of DEncEncryptedKey object.
  // pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
  // returns value of atribute or NULL.
  //--------------------------------------------------
  EXP_OPTION X509* dencEncryptedKey_GetCertificate(DEncEncryptedKey* pEncKey);

//======================< DEncEncryptedKey - mutators >===================

  //--------------------------------------------------
  // Mutatoror for Id atribute of DEncEncryptedKey object.
  // pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedKey_SetId(DEncEncryptedKey* pEncKey, const char* value);

  //--------------------------------------------------
  // Mutatoror for Recipient atribute of DEncEncryptedKey object.
  // pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedKey_SetRecipient(DEncEncryptedKey* pEncKey, const char* value);

  //--------------------------------------------------
  // Mutatoror for EncryptionMethod subelement of DEncEncryptedKey object.
  // pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedKey_SetEncryptionMethod(DEncEncryptedKey* pEncKey, const char* value);

  //--------------------------------------------------
  // Mutatoror for KeyName subelement of DEncEncryptedKey object.
  // pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedKey_SetKeyName(DEncEncryptedKey* pEncKey, const char* value);

  //--------------------------------------------------
  // Mutatoror for CarriedKeyName subelement of DEncEncryptedKey object.
  // pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedKey_SetCarriedKeyName(DEncEncryptedKey* pEncKey, const char* value);

  //--------------------------------------------------
  // Mutatoror for certificate of DEncEncryptedKey object.
  // pEncKey - pointer to DEncEncryptedKey object [REQUIRED]
  // value - new value for atribute [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedKey_SetCertificate(DEncEncryptedKey* pEncKey, X509* value);

//==========< general crypto fucntions >============

  //--------------------------------------------------
  // Locates the correct EncryptedKey object by reading
  // users certificate from smartcard and searching the
  // right EncryptedKey object
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // ppEncKey - address of a buffer for EncryptedKey pointer [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_findEncryptedKeyByPKCS11(DEncEncryptedData* pEncData, DEncEncryptedKey** ppEncKey);

  //--------------------------------------------------
  // Encrypts data with the generated key
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // nCompressOption - flag: DENC_COMPRESS_ALLWAYS, 
  //    DENC_COMPRESS_NEVER or DENC_COMPRESS_BEST_EFFORT
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_encryptData(DEncEncryptedData* pEncData, int nCompressOption);

  //--------------------------------------------------
  // Decrypts data transport key with ID card and
  // then decrypts the data with the transport key.
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // tKey - decrypted transport key [REQUIRED]
  // keyLen - length of trasnport key [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_decrypt_withKey(DEncEncryptedData* pEncData, 
						   const char* tKey, int keyLen);

  //--------------------------------------------------
  // Decrypts data with the generated key
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_decryptData(DEncEncryptedData* pEncData);

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
					   const char* pin);

  //--------------------------------------------------
  // Compresses data with ZLIB. Cannot compress encrypted data!!!
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // nCompressOption - flag: DENC_COMPRESS_ALLWAYS, 
  //    DENC_COMPRESS_NEVER or DENC_COMPRESS_BEST_EFFORT
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_compressData(DEncEncryptedData* pEncData, int nCompressOption);

  //--------------------------------------------------
  // Decompresses data with ZLIB. 
  // pEncData - pointer to DEncEncryptedData object [REQUIRED]
  // nCompressOption - flag: DENC_COMPRESS_ALLWAYS, 
  //    DENC_COMPRESS_NEVER or DENC_COMPRESS_BEST_EFFORT
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencEncryptedData_decompressData(DEncEncryptedData* pEncData);


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
				  const X509* pCert);

  //--------------------------------------------------
  // "Destructor" of DEncRecvInfo object
  // pRecvInfo - address of buffer for newly allocated object [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencRecvInfo_free(DEncRecvInfo* pRecvInfo);

  //--------------------------------------------------
  // Stores DEncRecvInfo object to configuration store
  // pRecvInfo - address of buffer for newly allocated object [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencRecvInfo_store(DEncRecvInfo* pRecvInfo);

  //--------------------------------------------------
  // Stores DEncRecvInfo object to configuration store
  // pConfStore - store to search in [OPTIONAL]. Use NULL for default
  // pRecvInfo - address of buffer for newly allocated object [REQUIRED]
  // szId - id of the object [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencRecvInfo_findById(ConfigurationStore *pConfStore, 
				       DEncRecvInfo** ppRecvInfo, const char* szId);

  //--------------------------------------------------
  // Deletes DEncRecvInfo object from configuration store
  // pRecvInfo - address of RecvInfo [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencRecvInfo_delete(DEncRecvInfo* pRecvInfo);

  //--------------------------------------------------
  // Returns all DEncRecvInfo objects
  // pRecvInfoList - address of the list receiving the items [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencRecvInfo_findAll(DEncRecvInfoList* pRecvInfoList);


//====================< RecipientInfoList functions >==========================

  //--------------------------------------------------
  // Adds a DEncRecvInfo object to the list
  // pRecvInfoList - address of the list receiving the item [REQUIRED]
  // pRecvInfo - new object to be added
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencRecvInfoList_add(DEncRecvInfoList* pRecvInfoList, DEncRecvInfo *pRecvInfo);

  //--------------------------------------------------
  // Frees the contents of a DEncRecvInfoList object
  // pRecvInfoList - address of the list [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencRecvInfoList_free(DEncRecvInfoList* pRecvInfoList);

  //--------------------------------------------------
  // Removes the given DEncRecvInfo object from the list
  // pRecvInfoList - address of the list [REQUIRED]
  // szId - id of the obect to be removed [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencRecvInfoList_delete(DEncRecvInfoList* pRecvInfoList, const char* szId);

//====================< big file functions >==========================

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
				 const char* szMimeType);

//====================< original content functions >===================

  //--------------------------------------------------
  // Returns the count of "orig_file" properties
  // pEncData - EncryptedData object [REQUIRED]
  // returns count or -1 for error.
  //--------------------------------------------------
  EXP_OPTION int dencOrigContent_count(DEncEncryptedData* pEncData);

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
	   const char* szName, const char* szSize, const char* szMime, const char* szDfId);

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
		   char* szName, char* szSize, char* szMime, char* szDfId);

  //--------------------------------------------------
  // Checks if there is a digidoc document in this
  // encrypted document.
  // pEncData - EncryptedData object [REQUIRED]
  // returns 1 if digidoc document is inside
  //--------------------------------------------------
  EXP_OPTION int dencOrigContent_isDigiDocInside(DEncEncryptedData* pEncData);

  //--------------------------------------------------
  // Registers digidoc document as encrypted datatype
  // and stores it's data file info.
  // pEncData - EncryptedData object [REQUIRED]
  // pSigDoc - SignedDoc object [REQUIRED]
  // returns 1 if digidoc document is inside
  //--------------------------------------------------
  EXP_OPTION int dencOrigContent_registerDigiDoc(DEncEncryptedData* pEncData, SignedDoc* pSigDoc);

//====================< other meta-info functions >===================

  //--------------------------------------------------
  // Sets the library name and version property
  // pEncData - EncryptedData object [REQUIRED]
  // returns count or -1 for error.
  //--------------------------------------------------
  EXP_OPTION int dencMetaInfo_SetLibVersion(DEncEncryptedData* pEncData);

  //--------------------------------------------------
  // Sets the format name and version property
  // pEncData - EncryptedData object [REQUIRED]
  // returns count or -1 for error.
  //--------------------------------------------------
  EXP_OPTION int dencMetaInfo_SetFormatVersion(DEncEncryptedData* pEncData);

  //--------------------------------------------------
  // Returns the library name and version meta-info of this document
  // pEncData - EncryptedData object [REQUIRED]
  // returns count or -1 for error.
  //--------------------------------------------------
  EXP_OPTION int dencMetaInfo_GetLibVersion(DEncEncryptedData* pEncData, char* szLibrary, char* szVersion);

  //--------------------------------------------------
  // Returns the format name and version meta-info of this document
  // pEncData - EncryptedData object [REQUIRED]
  // returns count or -1 for error.
  //--------------------------------------------------
  EXP_OPTION int dencMetaInfo_GetFormatVersion(DEncEncryptedData* pEncData, char* szFormat, char* szVersion);

  //--------------------------------------------------
  // Deletes the meta-info properties
  // pEncData - EncryptedData object [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencMetaInfo_deleteVersionInfo(DEncEncryptedData* pEncData);


#ifdef  __cplusplus
}
#endif

#endif // __DIGIDOC_ENC_H__


