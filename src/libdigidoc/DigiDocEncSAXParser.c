//==================================================
// FILE:	DigiDocEncSAXParser.c
// PROJECT:     Digi Doc Encryption 
// DESCRIPTION: DigiDocEnc XML SAX parsing
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
#include <libdigidoc/DigiDocEncSAXParser.h>
#include <libdigidoc/DigiDocError.h>
#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocStack.h>
#include <libdigidoc/DigiDocConvert.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocPKCS11.h> 
#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/DigiDocDfExtract.h>

#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <zlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>


#include <libxml/globals.h>
#include <libxml/xmlerror.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h> /* only for xmlNewInputFromFile() */

//===============< SAX handlers >==============================

/*
* Working area for XML parsing with SAX
*/
typedef struct DEncParse_st {
  DEncEncryptedData* pEncData;	// document file to be filled with data
  int errcode;
  ElementEntry dencStack;  // stack used for keeping the current parsing position
  DigiDocMemBuf mbufContent; // used for collecting element content
  int bCollectMode;          // flag to switch collection of content on and off
  char errmsg[100];
} DEncParse;


//--------------------------------------------------
// Cleans up the memory that might habe been allocated
// during parsing process
// pctx - SAX parser context
//--------------------------------------------------
void dencSaxCleanup(DEncParse* pctx)
{
  ddocMemBuf_free(&(pctx->mbufContent));
  memset(pctx, 0, sizeof(DEncParse));
}

//--------------------------------------------------
// Handles the <EncryptedData> start event
// pctx - SAX parser context
// atts - array of atribute names and values
// returns error code or ERR_OK.
//--------------------------------------------------

int dencSaxHandleStartEncryptedData(DEncParse* pctx, const xmlChar** atts)
{
  int i, err = ERR_OK;
  char *id = NULL, *type = NULL, *mime = NULL, *xmlns = NULL;

  // check the atributes
  for(i = 0; atts && atts[i] && atts[i+1]; i++) {
    if(!strcmp((char*)atts[i], "Id"))
      id = (char*)atts[i+1];
    if(!strcmp((char*)atts[i], "Type"))
      type = (char*)atts[i+1];
    if(!strcmp((char*)atts[i], "MimeType"))
      mime = (char*)atts[i+1];
    if(!strncmp((char*)atts[i], "xmlns", 5))
      xmlns = (char*)atts[i+1];
  }
  // create new EncryptedData object
  err = dencEncryptedData_new(&(pctx->pEncData), xmlns, NULL, id, type, mime);
  // delete automatically generated meta info to read the stuff from file
  err = dencMetaInfo_deleteVersionInfo(pctx->pEncData);
  return err;
}

//--------------------------------------------------
// Handles the <EncryptionMethod> start event
// pctx - SAX parser context
// atts - array of atribute names and values
// returns error code or ERR_OK.
//--------------------------------------------------
int dencSaxHandleStartEncryptionMethod(DEncParse* pctx, const xmlChar** atts)
{
  int i, err = ERR_OK;
  char *alg = NULL;

  // check the atributes
  for(i = 0; atts && atts[i] && atts[i+1]; i++) {
    if(!strcmp((char*)atts[i], "Algorithm"))
      alg = (char*)atts[i+1];
  }
  // check the EncryptionMethod position in xml doc
  if(ddocStackHasParentWithName(&(pctx->dencStack), (xmlChar*)"EncryptedKey", NULL)) {
    DEncEncryptedKey* pEncKey = dencEncryptedData_GetLastEncryptedKey(pctx->pEncData);
    if(pEncKey)
      err = dencEncryptedKey_SetEncryptionMethod(pEncKey, alg);
  } else
    if(ddocStackHasParentWithName(&(pctx->dencStack), (xmlChar*)"EncryptedData", NULL))
      err = dencEncryptedData_SetEncryptionMethod(pctx->pEncData, alg);
  return err;
}

//--------------------------------------------------
// Handles the <EncryptedKey> start event
// pctx - SAX parser context
// atts - array of atribute names and values
// returns error code or ERR_OK.
//--------------------------------------------------
int dencSaxHandleStartEncryptedKey(DEncParse* pctx, const xmlChar** atts)
{
  int i;
  char *id = NULL, *recipient = NULL;
  DEncEncryptedKey* pEncKey = 0;

  // check the atributes
  for(i = 0; atts && atts[i] && atts[i+1]; i++) {
    if(!strcmp((char*)atts[i], "Id"))
      id = (char*)atts[i+1];
    if(!strcmp((char*)atts[i], "Recipient"))
      recipient = (char*)atts[i+1];
  }
  // create new EncryptedData object
  return dencEncryptedKey_new(pctx->pEncData, &pEncKey, NULL, NULL,
			      id, recipient, NULL, NULL);
}

//--------------------------------------------------
// Handles the <X509Certificate> end event
// pctx - SAX parser context
// atts - array of atribute names and values
// returns error code or ERR_OK.
//--------------------------------------------------
int dencSaxHandleEndX509Certificate(DEncParse* pctx)
{
  int err = ERR_OK;
  X509 *pCert = 0;
  DEncEncryptedKey* pEncKey = 0;

  err = ddocDecodeX509PEMData(&pCert, (const char*)pctx->mbufContent.pMem, (int)pctx->mbufContent.nLen);
  if(err) return err;
  pEncKey = dencEncryptedData_GetLastEncryptedKey(pctx->pEncData);
  if(pEncKey)
    err = dencEncryptedKey_SetCertificate(pEncKey, pCert);
  ddocDebug(4, "dencSaxHandleEndX509Certificate", "EncKey: %s, cert: %s rc: %d",
	    (pEncKey ? "OK" : "NULL"), (pCert ? "OK" : "NULL"), err);
  // reset collect mode and cleanup
  pctx->bCollectMode = 0;
  ddocMemBuf_free(&(pctx->mbufContent));
  return err;
}

//--------------------------------------------------
// Handles the <KeyName> end event
// pctx - SAX parser context
// atts - array of atribute names and values
// returns error code or ERR_OK.
//--------------------------------------------------
int dencSaxHandleEndKeyName(DEncParse* pctx)
{
  int err = ERR_OK;

  DEncEncryptedKey* pEncKey = dencEncryptedData_GetLastEncryptedKey(pctx->pEncData);
  if(pEncKey)
    err = dencEncryptedKey_SetKeyName(pEncKey, (char*)pctx->mbufContent.pMem);
  ddocDebug(4, "dencSaxHandleEndKeyName", "EncKey: %s, KeyName: %s rc: %d",
    (pEncKey ? "OK" : "NULL"), (pctx->mbufContent.pMem ? pctx->mbufContent.pMem : "NULL"), err);
  // reset collect mode and cleanup
  pctx->bCollectMode = 0;
  ddocMemBuf_free(&(pctx->mbufContent));
  return err;
}

//--------------------------------------------------
// Handles the <CarriedKeyName> end event
// pctx - SAX parser context
// atts - array of atribute names and values
// returns error code or ERR_OK.
//--------------------------------------------------
int dencSaxHandleEndCarriedKeyName(DEncParse* pctx)
{
  int err = ERR_OK;

  DEncEncryptedKey* pEncKey = dencEncryptedData_GetLastEncryptedKey(pctx->pEncData);
  if(pEncKey)
    err = dencEncryptedKey_SetCarriedKeyName(pEncKey, (char*)pctx->mbufContent.pMem);
  ddocDebug(4, "dencSaxHandleEndKeyName", "EncKey: %s, CarriedKeyName: %s rc: %d",
    (pEncKey ? "OK" : "NULL"), (pctx->mbufContent.pMem ? pctx->mbufContent.pMem : "NULL"), err);
  // reset collect mode and cleanup
  pctx->bCollectMode = 0;
  ddocMemBuf_free(&(pctx->mbufContent));
  return err;
}

//--------------------------------------------------
// Handles the <CipherValue> end event
// pctx - SAX parser context
// atts - array of atribute names and values
// returns error code or ERR_OK.
//--------------------------------------------------
int dencSaxHandleEndCipherValue(DEncParse* pctx)
{
  int err = ERR_OK, l = 0, i;
  char *p = 0;
  EVP_ENCODE_CTX ectx;

  if(pctx->mbufContent.pMem && pctx->mbufContent.nLen) {
    l = pctx->mbufContent.nLen; // enough since it's shrinking
    p = (char*)malloc(l);
    RETURN_IF_BAD_ALLOC(p)
    //decode((const byte*)pctx->mbufContent.pMem, pctx->mbufContent.nLen, p, &l);
    EVP_DecodeInit(&ectx);
    EVP_DecodeUpdate(&ectx, (unsigned char*)p, &l, (unsigned char*)pctx->mbufContent.pMem, pctx->mbufContent.nLen);
    ddocDebug(3, "dencSaxHandleEndCipherValue", "Initial decoding: %d -> %d bytes", pctx->mbufContent.nLen, l);
    i = pctx->mbufContent.nLen - l;
    EVP_DecodeFinal(&ectx, (unsigned char*)p+l, &i);
    l += i;
    ddocDebug(3, "dencSaxHandleEndCipherValue", "Final decoding: %d bytes", i);
    ddocDebug(3, "dencSaxHandleEndCipherValue", "Decoding: %d bytes of base64 data, got: %d bytes", pctx->mbufContent.nLen, l);
    ddocMemBuf_free(&(pctx->mbufContent));
  }
  if(p) {
  // check the EncryptionMethod position in xml doc
    if(ddocStackHasParentWithName(&(pctx->dencStack), (xmlChar*)"EncryptedKey", NULL)) {
      DEncEncryptedKey* pEncKey = dencEncryptedData_GetLastEncryptedKey(pctx->pEncData);
      if(pEncKey) {
	pEncKey->mbufTransportKey.pMem = p;
	pEncKey->mbufTransportKey.nLen = l;
	ddocDebug(4, "dencSaxHandleEndCipherValue", "Set encrypted tarnsport key: %d bytes", l);
      } else
	free(p);
    } else {
      if(ddocStackHasParentWithName(&(pctx->dencStack), (xmlChar*)"EncryptedData", NULL)) {
	pctx->pEncData->mbufEncryptedData.pMem = p;
	pctx->pEncData->mbufEncryptedData.nLen = l;
	ddocDebug(4, "dencSaxHandleEndCipherValue", "Set encrypted data: %d bytes", l);
	if(pctx->pEncData->szMimeType && 
	   !strcmp(pctx->pEncData->szMimeType, DENC_ENCDATA_MIME_ZLIB))
	  pctx->pEncData->nDataStatus = DENC_DATA_STATUS_ENCRYPTED_AND_COMPRESSED;
	else
	  pctx->pEncData->nDataStatus = DENC_DATA_STATUS_ENCRYPTED_AND_NOT_COMPRESSED;
      } else
	free(p);
    }
  }
  // reset collect mode and cleanup
  pctx->bCollectMode = 0;
  return err;
}

//--------------------------------------------------
// Handles the <EncryptionProperties> start event
// pctx - SAX parser context
// atts - array of atribute names and values
// returns error code or ERR_OK.
//--------------------------------------------------
int dencSaxHandleStartEncryptionProperties(DEncParse* pctx, const xmlChar** atts)
{
  int i, err = ERR_OK;
  char *id = NULL;

  // check the atributes
  for(i = 0; atts && atts[i] && atts[i+1]; i++) {
    if(!strcmp((char*)atts[i], "Id"))
      id = (char*)atts[i+1];
  }
  if(id)
    err = dencEncryptedData_SetEncryptionPropertiesId(pctx->pEncData, id);
  return err;
}

//--------------------------------------------------
// Handles the <EncryptionProperty> start event
// pctx - SAX parser context
// atts - array of atribute names and values
// returns error code or ERR_OK.
//--------------------------------------------------
int dencSaxHandleStartEncryptionProperty(DEncParse* pctx, const xmlChar** atts)
{
  int i, err = ERR_OK;
  char *id = NULL, *target = NULL, *name = NULL;
  DEncEncryptionProperty* pEncProperty = 0;

  // check the atributes
  for(i = 0; atts && atts[i] && atts[i+1]; i++) {
    if(!strcmp((char*)atts[i], "Id"))
      id = (char*)atts[i+1];
    if(!strcmp((char*)atts[i], "Target"))
      target = (char*)atts[i+1];
    if(!strcmp((char*)atts[i], "Name"))
      name = (char*)atts[i+1];
  }
  err = dencEncryptionProperty_new(pctx->pEncData, &pEncProperty,
				    id, target, name, NULL);
  if(err) return err;
  pctx->bCollectMode = 1;
  ddocMemBuf_free(&(pctx->mbufContent));
  return err;
}

//--------------------------------------------------
// Handles the <EncryptionProperty> end event
// pctx - SAX parser context
// atts - array of atribute names and values
// returns error code or ERR_OK.
//--------------------------------------------------
int dencSaxHandleEndEncryptionProperty(DEncParse* pctx)
{
  int err = ERR_OK;
  DEncEncryptionProperty* pEncProperty = 0;

  if(pctx->mbufContent.pMem && pctx->mbufContent.nLen) {
    pEncProperty = dencEncryptedData_GetLastEncryptionProperty(pctx->pEncData);
    if(pEncProperty)
      err = dencEncryptionProperty_SetContent(pEncProperty, (char*)pctx->mbufContent.pMem);
    ddocMemBuf_free(&(pctx->mbufContent));
  }
  pctx->bCollectMode = 0;
  return err;
}



//===============< SAX handlers >==============================

//--------------------------------------------------
// dencStartElementHandler:
// @ctxt:  An XML parser context
// @name:  The element name
// called when an opening tag has been processed.
//--------------------------------------------------
static void dencStartElementHandler(void *ctx, const xmlChar *name, const xmlChar **atts)
{
  DEncParse* pctx = (DEncParse*)ctx;
  ElementEntry* pCurrElem = 0;

  ddocDebug(5, "dencStartElementHandler", "<%s>, err: %d", (const char*)name, pctx->errcode);
  if(pctx->errcode) return; // if error skip all additional parsing
  pctx->errcode = ddocStackPushElementSAX(&(pctx->dencStack), name, atts, &pCurrElem);
  if(pctx->errcode) return;
  // check the element name
  if(pCurrElem) {
    if(!strcmp((const char*)pCurrElem->tag, "EncryptedData"))
      pctx->errcode = dencSaxHandleStartEncryptedData(pctx, atts);
    if(!strcmp((const char*)pCurrElem->tag, "EncryptionMethod"))
      pctx->errcode = dencSaxHandleStartEncryptionMethod(pctx, atts);
    if(!strcmp((const char*)pCurrElem->tag, "EncryptedKey"))
      pctx->errcode = dencSaxHandleStartEncryptedKey(pctx, atts);
    // start collecting certificate data
    if(!strcmp((const char*)pCurrElem->tag, "X509Certificate") ||
       !strcmp((const char*)pCurrElem->tag, "KeyName") ||
       !strcmp((const char*)pCurrElem->tag, "CipherValue") ||
       !strcmp((const char*)pCurrElem->tag, "CarriedKeyName") ) {
      pctx->bCollectMode = 1;
      ddocMemBuf_free(&(pctx->mbufContent));
    }
    if(!strcmp((const char*)pCurrElem->tag, "EncryptionProperties"))
      pctx->errcode = dencSaxHandleStartEncryptionProperties(pctx, atts);
    if(!strcmp((const char*)pCurrElem->tag, "EncryptionProperty"))
      pctx->errcode = dencSaxHandleStartEncryptionProperty(pctx, atts);
  }
}

//--------------------------------------------------
// dencEndElementHandler:
// @ctxt:  An XML parser context
// @name:  The element name
// called when the end of an element has been detected.
//--------------------------------------------------
static void dencEndElementHandler(void *ctx, const xmlChar *name)
{
  DEncParse* pctx = (DEncParse*)ctx;
  ElementEntry* pCurrElem = 0;

  ddocDebug(5, "dencEndElementHandler", "</%s>, err: %d", (const char*)name, pctx->errcode);
  if(pctx->errcode) return; // if error skip all additional parsing
  // find last element
  pCurrElem = ddocStackFindEnd(&(pctx->dencStack));
  // check the element name
  if(pCurrElem) {
    if(!strcmp((const char*)pCurrElem->tag, "X509Certificate"))
      pctx->errcode = dencSaxHandleEndX509Certificate(pctx);
    if(!strcmp((const char*)pCurrElem->tag, "KeyName"))
      pctx->errcode = dencSaxHandleEndKeyName(pctx);
    if(!strcmp((const char*)pCurrElem->tag, "CarriedKeyName"))
      pctx->errcode = dencSaxHandleEndCarriedKeyName(pctx);
    if(!strcmp((const char*)pCurrElem->tag, "CipherValue"))
      pctx->errcode = dencSaxHandleEndCipherValue(pctx);
    if(!strcmp((const char*)pCurrElem->tag, "EncryptionProperty"))
      pctx->errcode = dencSaxHandleEndEncryptionProperty(pctx);
  }
  // pop stack
  pctx->errcode = ddocStackPopElement(&(pctx->dencStack), 0, NULL);
}

//--------------------------------------------------
// dencCharactersHandler:
// @ctxt:  An XML parser context
// @ch:  a xmlChar string
// @len: the number of xmlChar
// receiving some chars from the parser.
//--------------------------------------------------
static void dencCharactersHandler(void *ctx, const xmlChar *ch, int len)
{
  DEncParse* pctx = (DEncParse*)ctx;
	
  ddocDebug(5, "dencCharactersHandler", "err: %d", pctx->errcode);
  if(pctx->errcode) return; // if error skip all additional parsing

  if(pctx->bCollectMode) {
    pctx->errcode = ddocMemAppendData(&(pctx->mbufContent), (char*)ch, len);
  }

  ddocDebug(5, "dencCharactersHandler: %s", "End");
}


//--------------------------------------------------
// cdataBlockHandler:
// @ctx: the user data (XML parser context)
// @value:  The pcdata content
// @len:  the block length
// called when a pcdata block has been parsed
//--------------------------------------------------
static void dencCdataBlockHandler(void * ctx, const xmlChar *value, int len)
{
  ddocDebug(5, "dencCdataBlockHandler", "SAX.pcdata(%.20s, %d)", (char*)value, len);
}


//--------------------------------------------------
// dencWarningHandler:
// @ctxt:  An XML parser context
// @msg:  the message to display/transmit
// @...:  extra parameters for the message display
// Display and format a warning messages, gives file, line, position and
// extra parameters
//--------------------------------------------------
static void dencWarningHandler(void * ctx, const char *msg, ...)
{
    va_list args;

    va_start(args, msg);
    ddocDebug(2, "dencWarningHandler", msg, args);
    fprintf(stdout, "SAX.warning: ");
    vfprintf(stdout, msg, args);
    va_end(args);
}

//--------------------------------------------------
// dencErrorHandler:
// @ctxt:  An XML parser context
// @msg:  the message to display/transmit
// @...:  extra parameters for the message display
// Display and format a error messages, gives file, line, position and
// extra parameters.
//--------------------------------------------------
static void dencErrorHandler(void *ctx, const char *msg, ...)
{
  va_list args;
  DEncParse* pctx = (DEncParse*)ctx;
	
  va_start(args, msg);	
  pctx->errcode = ERR_DIGIDOC_PARSE;
  ddocDebugVaArgs(1, "dencErrorHandler", msg, args);
  addError(pctx->errcode, __FILE__, __LINE__, "XML parsing error");
  va_end(args);
}

//--------------------------------------------------
// dencFatalErrorHandler:
// @ctxt:  An XML parser context
// @msg:  the message to display/transmit
// @...:  extra parameters for the message display
// Display and format a fatalError messages, gives file, line, position and
// extra parameters.
//--------------------------------------------------
static void dencFatalErrorHandler(void *ctx, const char *msg, ...)
{
  va_list args;
  DEncParse* pctx = (DEncParse*)ctx;

  va_start(args, msg);
  pctx->errcode = ERR_DIGIDOC_PARSE;
  ddocDebugVaArgs(1, "dencFatalErrorHandler", msg, args);
  addError(pctx->errcode, __FILE__, __LINE__, "XML parsing error");
  va_end(args);
}

xmlSAXHandler dencSAXHandlerStruct = {
    NULL, //internalSubsetHandler,
    NULL, //isStandaloneHandler,
    NULL, //hasInternalSubsetHandler,
    NULL, //hasExternalSubsetHandler,
    NULL, //resolveEntityHandler,
    NULL, //getEntityHandler,
    NULL, //entityDeclHandler,
    NULL, //notationDeclHandler,
    NULL, //attributeDeclHandler,
    NULL, //elementDeclHandler,
    NULL, //unparsedEntityDeclHandler,
    NULL, //setDocumentLocatorHandler,
    NULL, //startDocumentHandler,
    NULL, //endDocumentHandler,
    dencStartElementHandler,
    dencEndElementHandler,
    NULL, //referenceHandler,
    dencCharactersHandler,
    NULL, //ignorableWhitespaceHandler,
    NULL, //processingInstructionHandler,
    NULL, //commentHandler,
    dencWarningHandler,
    dencErrorHandler,
    dencFatalErrorHandler,
    NULL, //getParameterEntityHandler,
    dencCdataBlockHandler,
    NULL, //externalSubsetHandler,
    1
};


xmlSAXHandlerPtr dencSAXHandler = &dencSAXHandlerStruct;



//--------------------------------------------------
// Reads in encrypted XML document.
// ppEncData - address for new encrypted data object [REQUIRED]
// szFileName - input file name
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencSaxReadEncryptedData(DEncEncryptedData** ppEncData, const char* szFileName)
{
  FILE *f;
  int ret;
  char chars[1025], convFileName[250];
  xmlParserCtxtPtr ctxt;
  DEncParse pctx;

  RETURN_IF_NULL_PARAM(szFileName)
  RETURN_IF_NULL_PARAM(ppEncData)
  clearErrors();
  *ppEncData = 0; // mark as not read yet
  ddocDebug(3, "dencSaxReadEncryptedData", "file: %s", szFileName);
  memset(&pctx, 0, sizeof(pctx));
  ddocConvertFileName(convFileName, sizeof(convFileName), szFileName );
  if((f = fopen(convFileName, "r")) != NULL) {
    ret = fread(chars, 1, 1024, f);
    if (ret > 0) {
      ctxt = xmlCreatePushParserCtxt(dencSAXHandler, &pctx,
				     chars, ret, szFileName);
      while ((ret = fread(chars, 1, 1024, f)) > 0) {
	xmlParseChunk(ctxt, chars, ret, 0);
      }
      xmlParseChunk(ctxt, chars, 0, 1);
      xmlFreeParserCtxt(ctxt);
    }
    fclose(f);
  } else {
    ddocDebug(1, "dencSaxReadEncryptedData", "Error reading file: %s", szFileName);
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_READ);
  }
  // cleanup stack
  ret = pctx.errcode = ddocStackPopElement(&(pctx.dencStack), 1, NULL);
  ddocDebug(3, "dencSaxReadEncryptedData", "End parsing file: %s - RC: %d", szFileName, ret);
  if(ret == 0)
    *ppEncData = pctx.pEncData;
  // cleanup
  dencSaxCleanup(&pctx);
  return ret;
}

//===============< Large file decryption SAX handlers >==============================

/*
* Working area for XML parsing with SAX
*/
typedef struct DEncDecryptParse_st {
  int errcode;
  ElementEntry dencStack;    // stack used for keeping the current parsing position
  FILE* hOutFile;
  DigiDocMemBuf mbufTransportKey;
  DigiDocMemBuf mbufTemp;
  X509* pCert;
  char* szPin;
  int nSlot;
  long lB64Len, lBinLen, lDecLen;
  EVP_ENCODE_CTX ectx;
  EVP_CIPHER_CTX dctx;
  int nB64SkipMode;
  char errmsg[100];
  char szCertSerial[100];
  int nCipherInited;
} DEncDecryptParse;


//--------------------------------------------------
// Cleans up data that might have been allocated
// during the parsing process
// pctx:  An XML parser context
//--------------------------------------------------
void dencDecryptSaxCleanup(DEncDecryptParse* pctx)
{
  ddocMemBuf_free(&(pctx->mbufTransportKey));
  ddocMemBuf_free(&(pctx->mbufTemp));
  if(pctx->pCert)
    X509_free(pctx->pCert);
  if(pctx->hOutFile)
    fclose(pctx->hOutFile);
  memset(pctx, 0, sizeof(DEncDecryptParse));
}

//--------------------------------------------------
// dencDecryptStartElementHandler:
// @ctxt:  An XML parser context
// @name:  The element name
// called when an opening tag has been processed.
//--------------------------------------------------
static void dencDecryptStartElementHandler(void *ctx, const xmlChar *name, const xmlChar **atts)
{
  DEncDecryptParse* pctx = (DEncDecryptParse*)ctx;
  ElementEntry* pCurrElem = 0;

  ddocDebug(4, "dencDecryptStartElementHandler", "<%s>, err: %d", (const char*)name, pctx->errcode);
  if(pctx->errcode) return; // if error skip all additional parsing
  pctx->errcode = ddocStackPushElementSAX(&(pctx->dencStack), name, atts, &pCurrElem);
  // initialize decoding and decryption
  if(pCurrElem && !strcmp((const char*)pCurrElem->tag, "CipherValue") &&
     !ddocStackHasParentWithName(&(pctx->dencStack), (xmlChar*)"EncryptedKey", NULL)) {    
    if(pctx->nB64SkipMode == 0) {
      ddocDebug(4, "dencDecryptStartElementHandler", "Decode init");
      EVP_DecodeInit(&(pctx->ectx));
      EVP_CIPHER_CTX_init(&(pctx->dctx));      
      pctx->lB64Len = pctx->lBinLen = pctx->lDecLen = 0;
    }
    pctx->nB64SkipMode++; // increment skip mode
    ddocDebug(4, "dencDecryptStartElementHandler", "Decode start, skip: %d", pctx->nB64SkipMode);
  }

}

//--------------------------------------------------
// dencDecryptEndElementHandler:
// @ctxt:  An XML parser context
// @name:  The element name
// called when the end of an element has been detected.
//--------------------------------------------------
static void dencDecryptEndElementHandler(void *ctx, const xmlChar *name)
{
  DEncDecryptParse* pctx = (DEncDecryptParse*)ctx;
  char buf1[4096], buf2[4096];
  int l1, l2, l3;

  ddocDebug(4, "dencDecryptEndElementHandler", "</%s>, err: %d", (const char*)name, pctx->errcode);
  if(pctx->errcode) return; // if error skip all additional parsing
  // decode the certificate data
  if(strstr((char*)name, "X509Certificate")) {
    pctx->errcode = ddocDecodeX509PEMData(&(pctx->pCert), (const char*)pctx->mbufTemp.pMem, (int)pctx->mbufTemp.nLen);
    ddocDebug(4, "dencDecryptEndElementHandler", "Decoding pem: %d cert: %s, rc: %d", 
	      pctx->mbufTemp.nLen, (pctx->pCert ? "OK" : "NULL"), pctx->errcode);
    ddocMemBuf_free(&(pctx->mbufTemp));
  }
  // check if it was the right key and decrypt transport key 
  if(strstr((char*)name, "EncryptedKey")) {
    memset(buf1, 0, sizeof(buf1));
    pctx->errcode = ReadCertSerialNumber(buf1, sizeof(buf1), pctx->pCert);
    ddocDebug(4, "dencDecryptEndElementHandler", "Looking for cert: %s, found: %s, rc: %d", 
	      pctx->szCertSerial, buf1, pctx->errcode);
    if(!strcmp(pctx->szCertSerial, buf1)) {
      l1 = sizeof(buf1);
      memset(buf1, 0, l1);
      decode((const byte*)pctx->mbufTemp.pMem, pctx->mbufTemp.nLen, (byte*)buf1, &l1);
      ddocDebug(4, "dencDecryptEndElementHandler", "Decoded key-len: %d got: %d", 
		pctx->mbufTemp.nLen, l1);
      // cleanup temp buffer
      ddocMemBuf_free(&(pctx->mbufTemp));
      if(l1 != DENC_ENCRYPTED_KEY_LEN) {
	SET_LAST_ERROR(ERR_DENC_DECRYPT);
	pctx->errcode = ERR_DENC_DECRYPT;
	return;
      }
      // decrypt the transport key
      pctx->mbufTransportKey.nLen = l1; 
      pctx->mbufTransportKey.pMem = (char*)malloc(l1);
      if(!pctx->mbufTransportKey.pMem) {
	SET_LAST_ERROR(ERR_BAD_ALLOC);
	pctx->errcode = ERR_BAD_ALLOC;
	return;
      }
      memset(pctx->mbufTransportKey.pMem, 0, l1);
      l3 = pctx->mbufTransportKey.nLen;
      pctx->errcode = decryptWithEstID(pctx->nSlot, pctx->szPin, (char *)buf1, l1,
				       (char*)pctx->mbufTransportKey.pMem, &l3);
      pctx->mbufTransportKey.nLen = l3;
      ddocDebug(4, "dencDecryptEndElementHandler", "Decrypted key-len: %d rc: %d", 
		pctx->mbufTransportKey.nLen, pctx->errcode);
      if(pctx->mbufTransportKey.nLen != DENC_DECRYPTED_KEY_LEN) {
	SET_LAST_ERROR(ERR_DENC_DECRYPT);
	pctx->errcode = ERR_DENC_DECRYPT;
	return;
      }
    }
  }
  // last block of encrypted data
  if(strstr((char*)name, "CipherValue") &&
     !ddocStackHasParentWithName(&(pctx->dencStack), (xmlChar*)"EncryptedKey", NULL)) {
    ddocDebug(4, "dencDecryptEndElementHandler", "Decode end, skip: %d", pctx->nB64SkipMode);
    if(pctx->nB64SkipMode > 0)
      pctx->nB64SkipMode--;
    if(pctx->nB64SkipMode == 0) {
      l1 = sizeof(buf1);
      memset(buf1, 0, l1);
      ddocDebug(4, "dencDecryptEndElementHandler", "Decoding: final into: %d", l1);
      EVP_DecodeFinal(&(pctx->ectx), (unsigned char*)buf1, &l1);
      pctx->lBinLen += l1;
      ddocDebug(4, "dencDecryptEndElementHandler", "Decoded: final got: %d, total %d -> %d", l1, pctx->lB64Len, pctx->lBinLen);
      // decrypt decoded data
      l2 = sizeof(buf2);
      memset(buf2, 0, l2);
      ddocDebug(3, "dencDecryptEndElementHandler", "Decrypting: final into: %d", l2);
      EVP_CipherFinal_ex(&(pctx->dctx), (unsigned char*)buf2, &l2);
      ddocDebug(4, "dencDecryptEndElementHandler", "Decrypted: final got: %d", l2);
      // write to file
      if(pctx->hOutFile) {
	if(l2)
	  pctx->lDecLen += fwrite(buf2, 1, l2, pctx->hOutFile);
	fclose(pctx->hOutFile);
	pctx->hOutFile = 0;
      }
      ddocDebug(3, "dencDecryptEndElementHandler", "Total base64: %d decoded: %d decrypted: %d RC: %d", 
	      pctx->lB64Len, pctx->lBinLen, pctx->lDecLen, pctx->errcode);
      pctx->nB64SkipMode = 0;
    }
  }
  // pop stack
  pctx->errcode = ddocStackPopElement(&(pctx->dencStack), 0, NULL);
}

//--------------------------------------------------
// dencDecryptCharactersHandler:
// @ctxt:  An XML parser context
// @ch:  a xmlChar string
// @len: the number of xmlChar
// receiving some chars from the parser.
//--------------------------------------------------
static void dencDecryptCharactersHandler(void *ctx, const xmlChar *ch, int len)
{
  DEncDecryptParse* pctx = (DEncDecryptParse*)ctx;
  ElementEntry* pCurrElem = 0;
  char *buf1, *buf2, *p1;
  int l1, l2, i, l;

  ddocDebug(4, "dencDecryptCharactersHandler", "Parsing: %d chars err: %d, skip: %d", len, pctx->errcode, pctx->nB64SkipMode);
  if(pctx->errcode) return; // if error skip all additional parsing
  // find last element
  pCurrElem = ddocStackFindEnd(&(pctx->dencStack));
  // if this is data belonging to <CipherValue> tag and
  // the latter is not a child of <EncryptedKey>, thus
  // it must be the main content, then decrypt it
  if(pCurrElem && !strcmp((const char*)pCurrElem->tag, "X509Certificate")) {
    // collect the certificate data
    pctx->errcode = ddocMemAppendData(&(pctx->mbufTemp), (char*)ch, len);
  }
  if(pCurrElem || pctx->nB64SkipMode > 0) {
    // handle encrypted data
    if((pCurrElem && !strcmp((const char*)pCurrElem->tag, "CipherValue")) || pctx->nB64SkipMode > 0) {
      // collect encrypted transport key data
      if(pCurrElem && 
	 ddocStackHasParentWithName(&(pctx->dencStack), (xmlChar*)"EncryptedKey", NULL)) {
	pctx->errcode = ddocMemAppendData(&(pctx->mbufTemp), (char*)ch, len);
      }
      // if this is the real encrypted content not a key then decrypt it
      else {
	// check if the transport key is ready for decryption
	if(pctx->mbufTransportKey.nLen != DENC_DECRYPTED_KEY_LEN) {
	  ddocDebug(1, "dencDecryptCharactersHandler", "Transport key len: %d", pctx->mbufTransportKey.nLen);
	  SET_LAST_ERROR(ERR_DENC_DECRYPT);
	  pctx->errcode = ERR_DENC_DECRYPT;
	  return;
	}
	// decode base64 encrypted data
	pctx->lB64Len += len;
	l1 = len;
	buf1 = (char*)malloc(l1);
	if(!buf1) {
	  SET_LAST_ERROR(ERR_BAD_ALLOC);
	  return;
	}
	memset(buf1, 0, l1);
	ddocDebug(4, "dencDecryptCharactersHandler", "Decoding: %d into: %d, skip: %d", len, l1, pctx->nB64SkipMode);
	EVP_DecodeUpdate(&(pctx->ectx), (unsigned char*)buf1, &l1, (unsigned char*)ch, len);
	// if this was the first block of decoded base64 data 
	// then use the first 16 bytes as the IV value
	p1 = buf1;
	if(pctx->lBinLen == 0) {
	  ddocDebug(4, "dencDecryptCharactersHandler", "Using 16 bytes for IV. Initing cipher");
	  p1 += 16; // don't decrypt the IV data
	  l1 -= 16;
	  EVP_CipherInit_ex(&(pctx->dctx), EVP_aes_128_cbc(), NULL, 
			(const unsigned char*)pctx->mbufTransportKey.pMem, (const unsigned char*)buf1, DECRYPT);
	}
	pctx->lBinLen += l1;
	ddocDebug(4, "dencDecryptCharactersHandler", "Decoded: %d got: %d, skip: %d", len, l1, pctx->nB64SkipMode);
	// decrypt decoded data
	l2 = l1 * 2;
	buf2 = (char*)malloc(l2);
	if(!buf2) {
	  SET_LAST_ERROR(ERR_BAD_ALLOC);
	  return;
	}
	memset(buf2, 0, l2);
	if(pctx->nB64SkipMode == 4)
	  l1 += 16; // ???
	ddocDebug(4, "dencDecryptCharactersHandler", "Decrypting: %d into: %d", l1, l2);
	EVP_CipherUpdate(&(pctx->dctx), (unsigned char*)buf2, &l, (const unsigned char*)p1, l1);
	ddocDebug(4, "dencDecryptCharactersHandler", "Decrypted: %d got: %d, skip: %d", l1, l, pctx->nB64SkipMode);
	free(buf1); 

	// no padding until the final chunk
	if(pctx->nB64SkipMode == 0 || pctx->nB64SkipMode == 4) {
	  // on the last block check for a block with all 0x0F
	  l1 = (int)(unsigned char)buf2[l-1];
	  if(l1 == 16) {
	    ddocDebug(4, "dencDecryptCharactersHandler", "Check 0x0F padding: %d", l1);
	    for(i = l - l1; i < l - 1; i++) {
	      if(buf2[i] != 16) {
		l1 = 0; // set not matched flag
		break;
	      }
	    }
	    if(l1) {
	      ddocDebug(4, "dencDecryptCharactersHandler", "Decrypted len: %d reduce by: %d", l2, l1);
	      l -= l1;
	    }
	  }
	  // remove padding
	  l1 = (int)(unsigned char)buf2[l-1];
	  if(l1 > 0 && l1 < 16) {
	    ddocDebug(4, "dencDecryptCharactersHandler", "Check padding: %d", l1);
	    for(i = l - l1; i < l - 1; i++) {
	      if(buf2[i]) {
		l1 = 0; // set not matched flag
		break;
	      }
	    }
	    if(l1) {
	      ddocDebug(4, "dencDecryptCharactersHandler", "Decrypted len: %d reduce by: %d", l2, l1);
	      l -= l1;
	    }
	  }
	  else
	    ddocDebug(4, "dencDecryptCharactersHandler", "Impossible padding: %d", l1);
	  if(pctx->nB64SkipMode == 4)
	    pctx->nB64SkipMode = 1; // reset flag - look for padding
	}
	// write to file
	if(pctx->hOutFile) 
	  pctx->lDecLen += fwrite(buf2, 1, l, pctx->hOutFile);
	free(buf2);
      }

    }

  }

  ddocDebug(5, "dencDecryptCharactersHandler", "End");
}


xmlSAXHandler dencDecryptSAXHandlerStruct = {
    NULL, //internalSubsetHandler,
    NULL, //isStandaloneHandler,
    NULL, //hasInternalSubsetHandler,
    NULL, //hasExternalSubsetHandler,
    NULL, //resolveEntityHandler,
    NULL, //getEntityHandler,
    NULL, //entityDeclHandler,
    NULL, //notationDeclHandler,
    NULL, //attributeDeclHandler,
    NULL, //elementDeclHandler,
    NULL, //unparsedEntityDeclHandler,
    NULL, //setDocumentLocatorHandler,
    NULL, //startDocumentHandler,
    NULL, //endDocumentHandler,
    dencDecryptStartElementHandler,
    dencDecryptEndElementHandler,
    NULL, //referenceHandler,
    dencDecryptCharactersHandler,
    NULL, //ignorableWhitespaceHandler,
    NULL, //processingInstructionHandler,
    NULL, //commentHandler,
    dencWarningHandler,
    dencErrorHandler,
    dencFatalErrorHandler,
    NULL, //getParameterEntityHandler,
    dencCdataBlockHandler,
    NULL, //externalSubsetHandler,
    1
};


xmlSAXHandlerPtr dencDecryptSAXHandler = &dencDecryptSAXHandlerStruct;

// string used to force parser to flush it's buffers
static char g_szCipherValueFlush1[] = "</denc:CipherValue>";
static char g_szCipherValueFlush2[] = "<denc:CipherValue>";

//--------------------------------------------------
// Decrypts an encrypted XML document and stores the
// cleartext data in another document.
// szInputFileName - input file name [REQUIRED]
// szOutputFileName - output file name [REQUIRED]
// szPin - PIN1 of the id-card to decrypt the transport key [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int dencSaxReadDecryptFile(const char* szInputFileName, 
				      const char* szOutputFileName, 
				      const char* szPin)
{
  FILE *f;
  int ret;
  char chars[1025], convInFileName[250], convOutFileName[250], *p;
  xmlParserCtxtPtr ctxt;
  DEncDecryptParse pctx;
  X509* pCert = 0;

  RETURN_IF_NULL_PARAM(szInputFileName)
  RETURN_IF_NULL_PARAM(szOutputFileName)
  RETURN_IF_NULL_PARAM(szPin)
  clearErrors();
  ddocDebug(3, "dencSaxReadDecryptFile", "input-file: %s, output-file: %s", 
	    szInputFileName, szOutputFileName);
  memset(&pctx, 0, sizeof(pctx));

  ddocConvertFileName(convInFileName, sizeof(convInFileName), szInputFileName);
  ddocConvertFileName(convOutFileName, sizeof(convOutFileName), szOutputFileName);

  //store decryption params
  pctx.nSlot = ConfigItem_lookup_int("DIGIDOC_AUTH_KEY_SLOT", 0);
  pctx.szPin = (char*)szPin;
  pctx.hOutFile = fopen(convOutFileName, "wb");
  pctx.nB64SkipMode = 0;
  if(!pctx.hOutFile) {
    ddocDebug(1, "dencSaxReadDecryptFile", "Error writing to file: %s", szOutputFileName);
    SET_LAST_ERROR_RETURN(ERR_FILE_WRITE, ERR_FILE_WRITE)
  }
  pctx.errcode = findUsersCertificate(pctx.nSlot, &pCert);
  if(pCert) {
    pctx.errcode = ReadCertSerialNumber(pctx.szCertSerial, sizeof(pctx.szCertSerial), pCert);
    ddocDebug(3, "dencSaxReadDecryptFile", "Decryptiong using certificate: %s", pctx.szCertSerial);
  }
  // parse and decrypt data in file
  if((f = fopen(convInFileName, "r")) != NULL && !pctx.errcode) {
    ret = fread(chars, 1, 1024, f);
    if (ret > 0) {
      ctxt = xmlCreatePushParserCtxt(dencDecryptSAXHandler, &pctx,
				     chars, ret, convInFileName);
      do {
	memset(chars, 0, sizeof(chars));
	ret = fread(chars, 1, 1024, f);
	if(ret == 0)
	  ret = strlen(chars);
	ddocDebug(4, "dencSaxReadDecryptFile", "In: %d Parsed: %d, skip: %d", ret, ctxt->nbChars, pctx.nB64SkipMode);
	// this horrible chemistry is done to prevent
	// libxml2 to start colecting huge memory structures
	// Since we cannot disable it we'll just bypass parser
	// with a large amount of base64 data
	if(pctx.nB64SkipMode > 0) {
	  p = strchr(chars, '<');
	  // if <CipherValue> was found and no "<" (beginn of new element)
	  // is found then send the "flush command" and enter bypass mode
	  if(pctx.nB64SkipMode == 1 && !p) {
	    pctx.nB64SkipMode += 2;
	    ddocDebug(4, "dencSaxReadDecryptFile", "Starting bypass mode, skip: %d", pctx.nB64SkipMode);
	    // force the parser to release element content 
	    // before entering into bypass mode
	    xmlParseChunk(ctxt, g_szCipherValueFlush1, strlen(g_szCipherValueFlush1), 0);
	    ddocDebug(4, "dencSaxReadDecryptFile", "Entering bypass mode, skip: %d", pctx.nB64SkipMode);
	  }
	  if(pctx.nB64SkipMode >= 2 && !p) {
	    ddocDebug(4, "dencSaxReadDecryptFile", "Parsing in bypass mode, skip: %d", pctx.nB64SkipMode);
	    dencDecryptCharactersHandler(&pctx, (const xmlChar *)chars, ret);
	  }
	  // if we reached the first block that contains a start of xml element
	  // then stop bypass mode
	  if(strchr(chars, '<')) {
	    pctx.nB64SkipMode = 2;
	    xmlParseChunk(ctxt, g_szCipherValueFlush2, strlen(g_szCipherValueFlush2), 0);
	    ddocDebug(4, "dencSaxReadDecryptFile", "Ending bypass mode len: %d, skip: %d", ret, pctx.nB64SkipMode);
	  }
	} // if bypass mode
	// if in normal mode or finished bypass mode
	if(pctx.nB64SkipMode == 0 || pctx.nB64SkipMode == 3) {
	  if(pctx.nB64SkipMode == 3)
	    pctx.nB64SkipMode = 4; // used as flag: last block - look for padding
	  ddocDebug(4, "dencSaxReadDecryptFile", "parsing normal chunk");
	  xmlParseChunk(ctxt, chars, ret, 0);	  
	}
	//memset(chars, 0, sizeof(chars));
      } while(ret == 1024);  // do-while
      // last block of data
      ddocDebug(4, "dencSaxReadDecryptFile", "parsing final chunk: %d", strlen(chars));
      xmlParseChunk(ctxt, NULL, 0, 1);
      xmlFreeParserCtxt(ctxt);
    } // if(ret > 0)
    fclose(f);
  } else {
    ddocDebug(1, "dencSaxReadDecryptFile", "Error reading file: %s", szInputFileName);
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_READ);
  }
  // cleanup stack
  ret = pctx.errcode = ddocStackPopElement(&(pctx.dencStack), 1, NULL);
  // cleanup
  dencDecryptSaxCleanup(&pctx);
  ddocDebug(1, "dencSaxReadDecryptFile", 
	    "End parsing file: %s - RC: %d", szInputFileName, ret);
  return ret;
}

