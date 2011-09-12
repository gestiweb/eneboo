//==================================================
// FILE:	DigiDocParser.c
// PROJECT:     Digi Doc
// DESCRIPTION: Digi Doc functions for xml parsing
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
//      10.07.2004      Veiko Sinivee
//                      Creation
//==================================================

// config data comes from there
#ifndef WIN32
  #if HAVE_CONFIG_H
    #include <config.h>
  #endif
#endif // no win32

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocConvert.h>
#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/DigiDocParser.h>
#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocStack.h>
#include <libdigidoc/DigiDocMem.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocOCSP.h>

#include <libxml/xmlreader.h>
#include <string.h>
#define MAX_FILENAME 250

//=====================< DataFile extraction function >=============================

#define EXTRACT_OP_WITH_TAGS   1  // copy <DataFile> with enclosing XML tags
#define EXTRACT_OP_NO_TAGS     2  // copy <DataFile> content, no changes, no decoding but no tags
#define EXTRACT_OP_BODY_ONLY   3  // copy only the <DataFile> content and base64 decode if necessary
#define EXTRACT_OP_BODY_MEM    4  // return the <DataFile> content in memory and base64 decode if necessary
#define MAX_CONTENT_TYPE       50  // maximum length for ContentType atribute 

extern char* canonicalizeXML(char* source, int len);





//--------------------------------------------------
// Processes node contents. This function is used 
// internally and not meant to be called directly
// by the users of this library!
// reader - XML reader cursor to current node
// phFile - address of FILE pointer. Used also as flag: searching/storing
// szDocId - Id atribute value of <DataFile> to extract
// szDataFile - name of file to stor it in
// operation - operation code: 
//    EXTRACT_OP_WITH_TAGS = copy <DataFile> with enclosing XML tags
//    EXTRACT_OP_BODY_ONLY = copy only the <DataFile> content and base64 decode if necessary
// pszContentType - address of DataFile content type atribute value
// pBuf - address of memory buffer for EXTRACT_OP_BODY_MEM
//--------------------------------------------------
int ddocExtractProcessNode(xmlTextReaderPtr reader, FILE** phFile, 
		       const char* szDocId, const char* szDataFile,
			   int operation, char* pszContentType, DigiDocMemBuf* pBuf)
{
  int err = ERR_OK, n;
  xmlChar *name = 0, *value = 0, *an, *av, *p;

  RETURN_IF_NULL_PARAM(reader);
  RETURN_IF_NULL_PARAM(phFile);
  RETURN_IF_NULL_PARAM(szDocId);
  RETURN_IF_NULL_PARAM(pszContentType);
  name = xmlTextReaderName(reader);
  ddocDebug(4, "ddocExtractProcessNode", "Node %d, name: %s depth: %d", 
	    xmlTextReaderNodeType(reader), (name ? (char*)name : "-"), xmlTextReaderDepth(reader));
  if(name) {
    xmlFree(name);
    name = 0;
  }
  switch(xmlTextReaderNodeType(reader)) {
  case XML_READER_TYPE_ELEMENT:
    name = xmlTextReaderName(reader);
    if(name) {
      if(!xmlStrcmp(name, (xmlChar*)"DataFile") && !*phFile) { // possible start
	value = xmlTextReaderGetAttribute(reader, (const xmlChar*)"Id");
	if(value) {
	  ddocDebug(4, "ddocExtractProcessNode", "DataFile Id=%s, search: %s", (const char*)value, szDocId);
	  if(!xmlStrcmp(value, (xmlChar*)szDocId)) { // found it
	    ddocDebug(4, "ddocExtractProcessNode", "open file: %s", szDataFile);
	    if(operation == EXTRACT_OP_BODY_MEM) {
	      *phFile = (FILE*)1; // we return in memory. Set this dummy value to indicate start of searched data
	    } else {
	      RETURN_IF_NULL_PARAM(szDataFile);
	      *phFile = fopen(szDataFile, "wb"); // open file for it
	    }
	    if(!*phFile) {
	      ddocDebug(1, "ddocExtractProcessNode", "Failed to open file: %s", szDataFile);
	      SET_LAST_ERROR_RETURN_CODE(ERR_FILE_WRITE);
	    } else
	      if(operation != EXTRACT_OP_BODY_MEM)
		ddocDebug(4, "ddocExtractProcessNode", "Successfully opened file: %s", szDataFile);
	  }
	  xmlFree(value);
	  value = xmlTextReaderGetAttribute(reader, (const xmlChar*)"ContentType");
	  if(value) {
	    ddocDebug(4, "ddocExtractProcessNode", "ContentType=%s", (const char*)value);
	    strncpy(pszContentType, (const char*)value, MAX_CONTENT_TYPE-1);
	    pszContentType[MAX_CONTENT_TYPE-1] = 0;
	    xmlFree(value);
	  }
	}
	if(*phFile && 
	   ((operation == EXTRACT_OP_WITH_TAGS && !xmlStrcmp(name, (xmlChar*)"DataFile")) ||
	    xmlStrcmp(name, (xmlChar*)"DataFile"))) {
	  if(operation == EXTRACT_OP_BODY_MEM) {
	    RETURN_IF_NULL_PARAM(pBuf);
	    err = ddocMemAppendData(pBuf, "<", -1);
	    err = ddocMemAppendData(pBuf, (const char*)name, -1);
	  } else {
	    fprintf(*phFile, "<%s", name);
	  }
	  while(xmlTextReaderMoveToNextAttribute(reader)) {
	    an = av = 0;
	    an = xmlTextReaderName(reader);
	    av = xmlTextReaderValue(reader);
	    if(operation == EXTRACT_OP_BODY_MEM) {
	      RETURN_IF_NULL_PARAM(pBuf);
	      err = ddocMemAppendData(pBuf, " ", -1);
	      err = ddocMemAppendData(pBuf, (const char*)an, -1);
	      err = ddocMemAppendData(pBuf, "=\"", -1);
	      err = ddocMemAppendData(pBuf, (const char*)av, -1);
	      err = ddocMemAppendData(pBuf, "\"", -1);
	    } else
	      fprintf(*phFile, " %s=\"%s\"", (const char*)an, (const char*)av);
	    if(an)
	      xmlFree(an);
	    if(av)
	      xmlFree(av);
	  }
	  if(operation == EXTRACT_OP_BODY_MEM) {
	    RETURN_IF_NULL_PARAM(pBuf);
	    err = ddocMemAppendData(pBuf, ">", -1);
	  } else {
	    fputs(">", *phFile);
	  }
	}
      }
      if(name)
	xmlFree(name);
    }
    break;
  case XML_READER_TYPE_END_ELEMENT:
    name = xmlTextReaderName(reader);
    // handle end element
    if(*phFile && 
       ((operation == EXTRACT_OP_WITH_TAGS && !xmlStrcmp(name, (xmlChar*)"DataFile")) ||
	xmlStrcmp(name, (xmlChar*)"DataFile")))
      if(operation == EXTRACT_OP_BODY_MEM) {
	RETURN_IF_NULL_PARAM(pBuf);
	err = ddocMemAppendData(pBuf, "</", -1);
	err = ddocMemAppendData(pBuf, (const char*)name, -1);
	err = ddocMemAppendData(pBuf, ">", -1);
      } else
	fprintf(*phFile, "</%s>", name);
    // check if it's the correct end element
    if(!xmlStrcmp(name, (xmlChar*)"DataFile")) { // possible end
      if(*phFile && *phFile != (FILE*)1) { // file opened then it's the correct end element
	err = -1;
	ddocDebug(4, "ddocExtractProcessNode", "close file: %s", szDataFile);
	fclose(*phFile);	
      }
      if(*phFile)
	*phFile = 0;
    }
    break;
  case XML_READER_TYPE_ATTRIBUTE:
    break;
  case XML_READER_TYPE_TEXT:
    // handle text node
    if(*phFile) {
      value = xmlTextReaderValue(reader);
      if(value) {
	ddocDebug(4, "ddocExtractProcessNode", "TEXT: \'%s\'", (const char*)value);
	if(operation != EXTRACT_OP_WITH_TAGS && 
	   operation != EXTRACT_OP_NO_TAGS &&
	   !strcmp(pszContentType, CONTENT_EMBEDDED_BASE64)) {
	  n = strlen((const char*)value)+1; // decoding will make it 25% smaller, so this must be enough
	  p = (xmlChar*)malloc(n);
	  if(p) {
	    decode((const byte*)value, strlen((const char*)value), p, &n);
	    ddocDebug(4, "ddocExtractProcessNode", "Decoded: %d bytes", n);
	    if(operation == EXTRACT_OP_BODY_MEM) {
	      RETURN_IF_NULL_PARAM(pBuf);
	      err = ddocMemAppendData(pBuf, (const char*)p, n);
	    }
	    else {
	      n = fwrite(p, 1, n, *phFile);
	      ddocDebug(4, "ddocExtractProcessNode", "Wrote: %d bytes", n);
	    }
	    free(p);
	  }
	} else {
	  // in memory we don't really want the escapes or do we ?
	  if(operation == EXTRACT_OP_BODY_MEM) {
	      RETURN_IF_NULL_PARAM(pBuf);
	      err = ddocMemAppendData(pBuf, (const char*)value, -1);
	  } else {
	    // convert xml special symbols to escapes on output
	    for(n = 0; value[n]; n++) {
	      switch(value[n]) {
	      case '<': fputs("&lt;", *phFile); break;
	      case '>': fputs("&gt;", *phFile); break;
	      case '&': fputs("&amp;", *phFile); break;
	      case '\'': fputs("&apos;", *phFile); break;
	      case '\"': fputs("&quot;", *phFile); break;
	      default: fputc(value[n], *phFile); break;
	      }
	    }
	  }
	}
	xmlFree(value);
      }
    }
    break;
  case XML_READER_TYPE_CDATA:
  case XML_READER_TYPE_ENTITY_REFERENCE:
  case XML_READER_TYPE_ENTITY:
  case XML_READER_TYPE_PROCESSING_INSTRUCTION:
  case XML_READER_TYPE_COMMENT:
  case XML_READER_TYPE_DOCUMENT:
  case XML_READER_TYPE_DOCUMENT_TYPE:
  case XML_READER_TYPE_DOCUMENT_FRAGMENT:
  case XML_READER_TYPE_NOTATION:
    break;
  }
  ddocDebug(4, "ddocExtractProcessNode", "Err %d", err);
  return err;
}


//--------------------------------------------------
// Reads in signed XML document and extracts the desired data file
// pSigDoc - signed document object if cached
// szFileName - name of digidoc file
// szDataFileName - name of new data file 
// szDocId - id if DataFile
// szCharset - output charset
//--------------------------------------------------
EXP_OPTION int ddocXRdrExtractDataFile(SignedDoc* pSigDoc, const char* szFileName, 
				       const char* szDataFileName, const char* szDocId, const char* szCharset)
{
  char szConvDigiDocName[MAX_FILENAME];
  char szConvDataFileName[MAX_FILENAME];
  char szContentType[MAX_CONTENT_TYPE];
  xmlTextReaderPtr reader;
  FILE* hFile = 0;
  int ret;
  long len;
  void* pBuf;

  // check input params
  ddocDebug(3, "ddocXRdrExtractDataFile", "SigDoc: %s, digidoc: %s, datafile: %s, output file: %s, charset: %s",
	    (pSigDoc ? "OK" : "NULL"), szFileName, szDocId, szDataFileName, szCharset);
  RETURN_IF_NULL_PARAM(szFileName);
  RETURN_IF_NULL_PARAM(szDataFileName);
  RETURN_IF_NULL_PARAM(szDocId);
  RETURN_IF_NULL_PARAM(szCharset);
  szContentType[0] = 0;
  clearErrors();
  // converts file names if necessary
  ddocConvertFileName(szConvDataFileName, sizeof(szConvDataFileName), szDataFileName);
  ddocConvertFileName(szConvDigiDocName, sizeof(szConvDigiDocName), szFileName);

  // try reading from memory if already cached?
  ret = ddocGetDataFileCachedData(pSigDoc, szDocId, &pBuf, &len);
  if(pBuf) { // gotcha
    ddocDebug(4, "ddocXRdrExtractDataFile", "Using cached data: %d bytes", len);
    if((hFile = fopen(szConvDataFileName, "wb")) != NULL) {
      fwrite(pBuf, 1, len, hFile);
      fclose(hFile);
    } else
      SET_LAST_ERROR_RETURN_CODE(ERR_FILE_WRITE);
    free(pBuf);
    return ERR_OK;
  }
  // read digidoc
  reader = xmlNewTextReaderFilename(szConvDigiDocName);
  if(reader != NULL) {
    ret = xmlTextReaderRead(reader);
    // loop over all nodes...
    while(ret == 1) {
      ret = ddocExtractProcessNode(reader, &hFile, szDocId, szConvDataFileName, 
				   EXTRACT_OP_BODY_ONLY, szContentType, 0);
      if(ret == -1) { // found all what I need but no errs. Stop parsing
	ret = 0;
	break;
      }
      ret = xmlTextReaderRead(reader);
    }
    // cleanup
    xmlFreeTextReader(reader);
    if (ret != 0)
      ddocDebug(1, "ddocXRdrExtractDataFile", "Error parsing file: %s parsing err: %d", szConvDigiDocName, ret);
    return ret;
  } else {
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_READ);
    ddocDebug(1, "ddocXRdrExtractDataFile", "Error reading file: %s", szConvDigiDocName);
  }
  return ERR_OK;
}

//--------------------------------------------------
// Reads in signed XML document and returns the
// desired DataFile-s content in a memory buffer.
// caller is responsible for freeing the memory.
// pSigDoc - digidoc object in case already read in
// szFileName - name of digidoc file
// szDocId - id if DataFile
// pBuf - address of buffer pointer
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocXRdrGetDataFile(SignedDoc* pSigDoc, const char* szFileName, 
			       const char* szDocId, DigiDocMemBuf* pBuf)
{
  char szConvDigiDocName[MAX_FILENAME];
  char szContentType[MAX_CONTENT_TYPE];
  xmlTextReaderPtr reader;
  FILE* hFile = 0;
  int ret;

  // check input params
  ddocDebug(3, "ddocXRdrGetDataFile", "SigDoc: %s, digidoc: %s, docid: %s",
	    (pSigDoc ? "OK" : "NULL"), szFileName, szDocId);
  RETURN_IF_NULL_PARAM(pBuf);
  RETURN_IF_NULL_PARAM(szDocId);
  memset(pBuf, 0, sizeof(DigiDocMemBuf));
  clearErrors();
  ret = ddocGetDataFileCachedData(pSigDoc, szDocId, &(pBuf->pMem), &pBuf->nLen);
  if(pBuf->pMem){ // gotcha
    ddocDebug(4, "ddocXRdrGetDataFile", "Using cached data: %d bytes", pBuf->nLen);
    return ERR_OK;
  }
  RETURN_IF_NULL_PARAM(szFileName);
  szContentType[0] = 0;
  // converts file names if necessary
  ddocConvertFileName(szConvDigiDocName, sizeof(szConvDigiDocName), szFileName);

  // read digidoc
  reader = xmlNewTextReaderFilename(szConvDigiDocName);
  if(reader != NULL) {
    ret = xmlTextReaderRead(reader);
    // loop over all nodes...
    while(ret == 1) {
      ret = ddocExtractProcessNode(reader, &hFile, szDocId, 0, 
				   EXTRACT_OP_BODY_MEM, szContentType, pBuf);
      if(ret == -1) { // found all what I need but no errs. Stop parsing
	ret = 0;
	break;
      }
      ret = xmlTextReaderRead(reader);
    }
    // cleanup
    xmlFreeTextReader(reader);
    if (ret != 0)
      ddocDebug(1, "ddocXRdrGetDataFile", "%s : failed to parse, err: %d", szConvDigiDocName, ret);
    return ret;
  } else {
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_READ);
    ddocDebug(1, "ddocXRdrGetDataFile", "Error reading file: %s", szConvDigiDocName);
  }
  return ERR_OK;
}


//--------------------------------------------------
// Reads in signed XML document and extracts the desired data file
// This function removes <DataFile> xml tags but doesn't
// otherwise change or decode the data. It is used
// internally to copy data content from an old digidoc file
// to a temp file while saving a the file
// szFileName - name of digidoc file
// szDataFileName - name of new data file 
// szDocId - id if DataFile
// szCharset - output charset
//--------------------------------------------------
int ddocXRdrCopyDataFile(SignedDoc* pSigDoc, const char* szFileName, 
			 const char* szDataFileName, const char* szDocId, const char* szCharset)
{
  char szConvDigiDocName[MAX_FILENAME];
  char szConvDataFileName[MAX_FILENAME];
  char szContentType[MAX_CONTENT_TYPE];
  xmlTextReaderPtr reader;
  FILE* hFile = 0;
  int ret;
  long len;
  void* pBuf;

  // check input params
  ddocDebug(3, "ddocXRdrCopyDataFile", "SigDoc: %s, digidoc: %s, datafile: %s, output file: %s, charset: %s",
	    (pSigDoc ? "OK" : "NULL"), szFileName, szDocId, szDataFileName, szCharset);
  RETURN_IF_NULL_PARAM(szFileName);
  RETURN_IF_NULL_PARAM(szDataFileName);
  RETURN_IF_NULL_PARAM(szDocId);
  RETURN_IF_NULL_PARAM(szCharset);
  szContentType[0] = 0;
  clearErrors();
  // converts file names if necessary
  ddocConvertFileName(szConvDataFileName, sizeof(szConvDataFileName), szDataFileName);
  ddocConvertFileName(szConvDigiDocName, sizeof(szConvDigiDocName), szFileName);

  // try reading from memory if already cached?
  ret = ddocGetDataFileCachedData(pSigDoc, szDocId, &pBuf, &len);
  if(pBuf) { // gotcha
    ddocDebug(4, "ddocXRdrCopyDataFile", "Using cached data: %d bytes", len);
    if((hFile = fopen(szConvDataFileName, "wb")) != NULL) {
      fwrite(pBuf, 1, len, hFile);
      fclose(hFile);
    } else
      SET_LAST_ERROR_RETURN_CODE(ERR_FILE_WRITE);
    free(pBuf);
    return ERR_OK;
  }
  // read digidoc
  reader = xmlNewTextReaderFilename(szConvDigiDocName);
  if(reader != NULL) {
    ret = xmlTextReaderRead(reader);
    // loop over all nodes...
    while(ret == 1) {
      ret = ddocExtractProcessNode(reader, &hFile, szDocId, szConvDataFileName, 
				   EXTRACT_OP_NO_TAGS, szContentType, 0);
      if(ret == -1) { // found all what I need but no errs. Stop parsing
	ret = 0;
	break;
      }
      ret = xmlTextReaderRead(reader);
    }
    // cleanup
    xmlFreeTextReader(reader);
    if (ret != 0)
      ddocDebug(1, "ddocXRdrCopyDataFile", "Error parsing file: %s, err: %d", szConvDigiDocName, ret);
    return ret;
  } else {
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_READ);
    ddocDebug(1, "ddocXRdrCopyDataFile", "Error reading file: %s", szConvDigiDocName);
  }
  return ERR_OK;
}


//=====================< digidoc parsing function >=============================

//--------------------------------------------------
// Records elements start tag to given buffer
// Used internally and not meant to be called directly
// by the users of this library!
// reader - XML reader cursor to current node
// pCurrElem - current element stack entry
// pBuf - address of buffer for collection certain portions of digidoc
//--------------------------------------------------
void ddocXRdrRecordElementStart(xmlTextReaderPtr reader, ElementEntry* pCurrElem, DigiDocMemBuf* pBuf)
{
  int n;
  xmlChar *an, *av;

  // if we are collecting some xml content 
  if(pBuf->pMem || pBuf->nLen) {
    ddocMemAppendData(pBuf, "<", -1);
    ddocMemAppendData(pBuf, (const char*)pCurrElem->tag, -1);
    for(n = 0; xmlTextReaderMoveToAttributeNo(reader, n); n++) {
      an = xmlTextReaderLocalName(reader);
      av = xmlTextReaderValue(reader);
      ddocMemAppendData(pBuf, " ", -1);
      ddocMemAppendData(pBuf, (const char*)an, -1);
      ddocMemAppendData(pBuf, "=\"", -1);
      ddocMemAppendData(pBuf, (const char*)av, -1);
      ddocMemAppendData(pBuf, "\"", -1);
      xmlFree(an);
      xmlFree(av);
    } 
    xmlTextReaderMoveToElement(reader);
    ddocMemAppendData(pBuf, ">", -1);
  }
}

//--------------------------------------------------
// Records elements end tag to given buffer
// Used internally and not meant to be called directly
// by the users of this library!
// pCurrElem - current element stack entry
// pBuf - address of buffer for collection certain portions of digidoc
//--------------------------------------------------
void ddocXRdrRecordElementEnd(ElementEntry* pCurrElem, DigiDocMemBuf* pBuf)
{
  // if we are collecting some xml content
  if(pBuf->pMem || pBuf->nLen) {
    ddocMemAppendData(pBuf, "</", -1);
    ddocMemAppendData(pBuf, (const char*)pCurrElem->tag, -1);
    ddocMemAppendData(pBuf, ">", -1);
  }
}

//--------------------------------------------------
// Records elements content to given buffer
// Used internally and not meant to be called directly
// by the users of this library!
// value - element content
// pBuf - address of buffer for collection certain portions of digidoc
//--------------------------------------------------
void ddocXRdrRecordElementContent(xmlChar* value,  DigiDocMemBuf* pBuf)
{
  // if we are collecting some xml content
  if(pBuf->pMem || pBuf->nLen)
    ddocMemAppendData(pBuf, (const char*)value, -1);
}

//--------------------------------------------------
// Starts recording xml content
// Used internally and not meant to be called directly
// by the users of this library!
// pBuf - address of buffer for collection certain portions of digidoc
// szTag - name of new tag thatwe start collecting for
//--------------------------------------------------
void ddocXRdrStartCollecting(DigiDocMemBuf* pBuf, const char* szTag)
{
  if(pBuf->pMem) {
    ddocDebug(2, "ddocXRdrStartCollecting", "Warning: old buffer not released when starting collecting: %s", szTag);
    free(pBuf->pMem);
  }
  ddocDebug(4, "ddocXRdrStartCollecting", "Starting collecting: %s", szTag);
  pBuf->pMem = malloc(16); // dummy buf to start recording
  pBuf->nLen = 0;
}

//--------------------------------------------------
// Stops recording xml content, calculates digest and stores it
// Used internally and not meant to be called directly
// by the users of this library!
// pBuf - address of buffer for collection certain portions of digidoc
// szTag - name of new tag thatwe start collecting for
// pDigestBuf - address of pointer receiving the digest value
// pDigestLen - address of length variable for digest length
//--------------------------------------------------
int ddocXRdrStopCollectingAndCalcDigest(DigiDocMemBuf* pBuf, const char* szTag,
					char** pDigestBuf, long* pDigestLen )
{
  char dig[DIGEST_LEN+2];
  char *p;
  SHA_CTX sctx;

  ddocDebug(7, "ddocXRdrStopCollectingAndCalcDigest", 
	    "Tag: %s Canonicalise:---------\n%s\n---------", szTag, pBuf->pMem);
  p = canonicalizeXML((char*)pBuf->pMem, strlen((const char*)pBuf->pMem));
  if(p) {
    SHA1_Init(&sctx);
    SHA1_Update(&sctx, p, strlen(p));
    SHA1_Final((unsigned char*)dig, &sctx);
    setString(pDigestBuf, dig, DIGEST_LEN);
    (*pDigestLen) = DIGEST_LEN;
    free(p);
    p = 0;
  }
  // stop recording xml content
  ddocDebug(4, "ddocParseProcessNode", "End recording: %s", szTag);
  if(pBuf->pMem)
    free(pBuf->pMem);
  pBuf->pMem = 0;
  pBuf->nLen = 0;
  return ERR_OK;
}

//--------------------------------------------------
// Stops recording xml content, calculates digest and stores it
// Used internally and not meant to be called directly
// by the users of this library!
// pBuf - address of buffer for collection certain portions of digidoc
// szTag - name of new tag thatwe start collecting for
// pDigestValue - address of DigestValue structure
// pDigestBuf - address of pointer receiving the digest value
// pDigestLen - address of length variable for digest length
//--------------------------------------------------
int ddocXRdrStopCollectingAndAssignDigest(DigiDocMemBuf* pBuf, const char* szTag,
					  DigiDocMemBuf* pDigestBuf)
{
  int err = ERR_OK;
  char dig[DIGEST_LEN+2];
  char *p;
  SHA_CTX sctx;

  ddocDebug(7, "ddocXRdrStopCollectingAndCalcDigest", 
	    "Tag: %s Canonicalise:---------\n%s\n---------", szTag, pBuf->pMem);
  p = canonicalizeXML((char*)pBuf->pMem, strlen((const char*)pBuf->pMem));
  if(p) {
    SHA1_Init(&sctx);
    SHA1_Update(&sctx, p, strlen(p));
    SHA1_Final((unsigned char*)dig, &sctx);
    err = ddocMemAssignData(pDigestBuf, dig, DIGEST_LEN);
    free(p);
    p = 0;
  }
  // stop recording xml content
  ddocDebug(4, "ddocParseProcessNode", "End recording: %s", szTag);
  if(pBuf->pMem)
    free(pBuf->pMem);
  pBuf->pMem = 0;
  pBuf->nLen = 0;
  return err;
}


//--------------------------------------------------
// Processes node contents. This function is used 
// internally and not meant to be called directly
// by the users of this library!
// reader - XML reader cursor to current node
// pSigDoc - address of signed doc pointer
// pStack - element stack pointer used to keep track of parsing position
// pBuf1 - address of first buffer for collection certain portions of digidoc
// pBuf2 - address of second buffer for collection certain portions of digidoc
// lMaxDFLen - maximum size of a cached DataFile
//--------------------------------------------------
int ddocParseProcessNode(xmlTextReaderPtr reader, SignedDoc **ppSigDoc, 
			 ElementEntry* pStack, DigiDocMemBuf* pBuf1, 
			 DigiDocMemBuf* pBuf2, long lMaxDFLen)
{
  int err = ERR_OK, n, l;
  xmlChar *name = 0, *value = 0;
  char* p;
  ElementEntry* pCurrElem = 0;
  DataFile *pDF = 0;
  CertID* pCertID;
  X509* pCert;
  const DigiDocMemBuf *pMBuf;

  RETURN_IF_NULL_PARAM(reader);
  RETURN_IF_NULL_PARAM(ppSigDoc);
  RETURN_IF_NULL_PARAM(pStack);
  RETURN_IF_NULL_PARAM(pBuf1);
  name = xmlTextReaderName(reader);
  ddocDebug(7, "ddocParseProcessNode", "Node %d, name: %s depth: %d", 
	    xmlTextReaderNodeType(reader), (name ? (char*)name : "-"), xmlTextReaderDepth(reader));
  if(name) {
    xmlFree(name);
    name = 0;
  }
  switch(xmlTextReaderNodeType(reader)) {
  case XML_READER_TYPE_ELEMENT:
    err = ddocStackPushElement(pStack, reader,&pCurrElem);
    if(pCurrElem) {
      // not in recording mode
      //if(!(*pBuf1)) { 
      ddocDebug(5, "ddocParseProcessNode", "Start element: %s", (const char*)pCurrElem->tag); 
      // start of <SignedDoc>
      if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"SignedDoc")) { 
	xmlChar* format = xmlTextReaderGetAttribute(reader, (const xmlChar*)"format");
	xmlChar* version = xmlTextReaderGetAttribute(reader, (const xmlChar*)"version");
	if(ddocCheckFormatAndVer((const char*)format, (const char*)version))
	  SET_LAST_ERROR_RETURN(ERR_UNSUPPORTED_FORMAT, ERR_UNSUPPORTED_FORMAT);
	err = SignedDoc_new(ppSigDoc, (const char*)format, (const char*)version);
	ddocDebug(4, "ddocParseProcessNode", "Create SignedDoc format: %s ver: %s", 
		    (const char*)format, (const char*)version);
	xmlFree(format);
	xmlFree(version);
      }

      // if not inside other <DataFile>
      if(!ddocStackHasParentWithName(pStack, (xmlChar*)"DataFile", pCurrElem)) {
	// start of <DataFile> 
	if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"DataFile")) {
	  xmlChar *an, *av;
	  DataFile *pDF = 0;
	  err = DataFile_new(&pDF, *ppSigDoc, NULL, NULL, NULL, NULL, 0,
			     NULL, 0, NULL, NULL);
	  for(n = 0; xmlTextReaderMoveToAttributeNo(reader, n); n++) {
	    an = xmlTextReaderLocalName(reader);
	    av = xmlTextReaderValue(reader);
	    ddocDebug(5, "ddocParseProcessNode", "DF attr: %s value: %s", 
		      (const char*)an, (const char*)av);
	    if(!xmlStrcmp(an, (const xmlChar*)"Id"))
	      setString(&(pDF->szId), (const char*)av, -1);
	    else if(!xmlStrcmp(an, (const xmlChar*)"ContentType"))
	      setString(&(pDF->szContentType), (const char*)av, -1);
	    else if(!xmlStrcmp(an, (const xmlChar*)"MimeType"))
	      setString(&(pDF->szMimeType), (const char*)av, -1);
	    else if(!xmlStrcmp(an, (const xmlChar*)"Filename"))
	      setString(&(pDF->szFileName), (const char*)av, -1);
	    else if(!xmlStrcmp(an, (const xmlChar*)"DigestType"))
	      setString(&(pDF->szDigestType), (const char*)av, -1);
	    else if(!xmlStrcmp(an, (const xmlChar*)"DigestValue")) {
	      l = xmlStrlen(av) + 1;
	      p = (char*)malloc(l);
	      RETURN_IF_BAD_ALLOC(p);
	      decode((const byte*)av, strlen((const char*)av), (byte*)p, &l);
	      ddocDataFile_SetDigestValue(pDF, p, l); //??? set binary or base64 ???
	    }
	    else if(!xmlStrcmp(an, (const xmlChar*)"Size"))
	      pDF->nSize = atol((const char*)av);
	    else { // all other are optional atributes
	      err = addDataFileAttribute(pDF, (const char*)an, (const char*)av);
	    }
	    xmlFree(an);
	    xmlFree(av);
	  } 
	  xmlTextReaderMoveToElement(reader);
	  ddocXRdrStartCollecting(pBuf1, "DataFile");
	}
	// start <Signature>
	if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"Signature")) {
	  SignatureInfo *pSig = 0;
	  value = xmlTextReaderGetAttribute(reader, (const xmlChar*)"Id");
	  ddocDebug(4, "ddocParseProcessNode", "SIG:%s", (const char*)value);
	  err = SignatureInfo_new(&pSig, *ppSigDoc, (const char*)value);
	  xmlFree(value);
	  ddocXRdrStartCollecting(pBuf2, "Signature");
	}
	// start <Reference>
	if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"Reference")) {
	  DocInfo *pDi = 0;
	  xmlChar *uri, *type;
	  SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	  RETURN_IF_NULL(pSig);
	  uri = xmlTextReaderGetAttribute(reader, (const xmlChar*)"URI");
	  type = xmlTextReaderGetAttribute(reader, (const xmlChar*)"Type");
	  ddocDebug(4, "ddocParseProcessNode", "Reference URI: %s Type: %s", 
		    (const char*)uri, (const char*)type);
	  // DataFile digest
	  if(uri[0] == '#' && uri[1] == 'D')
	    err = addDocInfo(&pDi, pSig, (const char*)(uri+1),
			     NULL, NULL, 0, NULL, 0); // TODO: ver 1.x mime digest handling
	  xmlFree(uri);
	  xmlFree(type);
	}

	// start <DigestMethod>
	if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"DigestMethod")) {
	  ElementEntry* pParent = 0;
	  value = xmlTextReaderGetAttribute(reader, (const xmlChar*)"Algorithm");
	  // if inside <SignedInfo>
	  if((pParent = ddocStackGetParentWithName(pStack, (xmlChar*)"Reference", pCurrElem)) != NULL) {
	    SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	    RETURN_IF_NULL(pSig);
	    // DataFile reference
	    if(pParent->uri && pParent->uri[1] == 'D') { 
	      DocInfo *pDi = ddocGetLastDocInfo(pSig);
	      if(pDi && !xmlStrcmp(value, (xmlChar*)DIGEST_METHOD_SHA1))
		setString((char**)&(pDi->szDigestType), DIGEST_SHA1_NAME, -1);
	      // what if something else ?
	    }
	    // SignedProperties reference

	  } // inside <Reference>

	  xmlFree(value);
	}

	// start of <SignedInfo> 
	if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"SignedInfo"))
	  ddocXRdrStartCollecting(pBuf1, "SignedInfo");

	// start of <SignedProperties> 
	if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"SignedProperties"))
	  ddocXRdrStartCollecting(pBuf1, "SignedProperties");

	// start of <UnsignedProperties> 
	if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"UnsignedProperties")) {
	  NotaryInfo* pNot = 0;
	  SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	  RETURN_IF_NULL(pSig);
	  ddocDebug(4, "ddocParseProcessNode", "Start of UnsignedProperties");
	  err = NotaryInfo_new(&pNot, *ppSigDoc, pSig);
	}


      } // if not inside other <DataFile>

      // if we are collecting some xml content (<DataFile>, <SignedInfo>, etc.)
      ddocXRdrRecordElementStart(reader, pCurrElem, pBuf1);
      // if we are collecting <Signature> xml content 
      ddocXRdrRecordElementStart(reader, pCurrElem, pBuf2);
    }
    break;
  case XML_READER_TYPE_END_ELEMENT:
    // get current element
    pCurrElem = ddocStackFindEnd(pStack);
    if(pCurrElem) {
      ddocDebug(5, "ddocParseProcessNode", "End element: %s", pCurrElem->tag);
      // if we are collecting some xml content (<DataFile>, <SignedInfo>, etc.)
      ddocXRdrRecordElementEnd(pCurrElem, pBuf1);
      // if we are collecting <Signature> xml content 
      ddocXRdrRecordElementEnd(pCurrElem, pBuf2);

      // </DataFile>
      if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"DataFile") &&
	 !ddocStackHasParentWithName(pStack, (xmlChar*)"DataFile", pCurrElem)) {
	ddocDebug(4, "ddocParseProcessNode", "End recording DataFile");
	pDF = ddocGetLastDataFile(*ppSigDoc);
	if(pDF) {
	  err = ddocXRdrStopCollectingAndAssignDigest(pBuf1, "DataFile", &(pDF->mbufDigest));
	  setString((char**)&(pDF->szDigestType), DIGEST_SHA1_NAME, -1);
	  // TODO: Version 1.0 buggy digest calculation...
	}
      } // </DataFile>

      // </SignedInfo>
      if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"SignedInfo") &&
	 !ddocStackHasParentWithName(pStack, (xmlChar*)"DataFile", pCurrElem)) {	
	SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	RETURN_IF_NULL(pSig);
	if(!pSig->pSigInfoRealDigest)
	  ddocDigestValue_new(&(pSig->pSigInfoRealDigest), 0, 0, 0);
	err = ddocXRdrStopCollectingAndAssignDigest(pBuf1, "SignedInfo", 
		    &(pSig->pSigInfoRealDigest->mbufDigestValue));
      } // </SignedInfo>

      // </SignedProperties>
      if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"SignedProperties") &&
	 !ddocStackHasParentWithName(pStack, (xmlChar*)"DataFile", pCurrElem)) {
	SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	RETURN_IF_NULL(pSig);
	if(!pSig->pSigPropRealDigest)
	  ddocDigestValue_new(&(pSig->pSigPropRealDigest), 0, 0, 0);
	ddocXRdrStopCollectingAndAssignDigest(pBuf1, "SignedProperties", 
			      &(pSig->pSigPropRealDigest->mbufDigestValue));
      } // </SignedProperties>

      // </Signature>
      if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"Signature") &&
	 !ddocStackHasParentWithName(pStack, (xmlChar*)"DataFile", pCurrElem)) {
	SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	RETURN_IF_NULL(pSig);
	ddocDebug(4, "ddocParseProcessNode", "End recording Signature");
	// DO NOT FREE THE MEM! Just take ownership
	ddocMemBuf_free(&(pSig->mbufOrigContent));
	pSig->mbufOrigContent.pMem = (byte*)pBuf2->pMem;
	pSig->mbufOrigContent.nLen = pBuf2->nLen;
	pBuf2->pMem = 0;
	pBuf2->nLen = 0;
      }  // </Signature>
      
    } // if(pCurrElem)
    err = ddocStackPopElement(pStack, 0, NULL);
    break;
  case XML_READER_TYPE_ATTRIBUTE:
    break;
  case XML_READER_TYPE_TEXT:
    value = xmlTextReaderValue(reader);
    if(value) {
      ddocDebug(8, "ddocParseProcessNode", "TEXT:\'%s\'", (const char*)value, -1);
      // if we are collecting some xml content (<DataFile>, <SignedInfo>, etc.)
      ddocXRdrRecordElementContent(value, pBuf1);
      // if we are collecting <Signature> xml content
      ddocXRdrRecordElementContent(value, pBuf2);

      // get current tag
      pCurrElem = ddocStackFindEnd(pStack);

      // content of <DataFile>
      if(pCurrElem && !xmlStrcmp(pCurrElem->tag, (xmlChar*)"DataFile") &&
	 !ddocStackHasParentWithName(pStack, (xmlChar*)"DataFile", pCurrElem)) {
	DataFile *pDF = ddocGetLastDataFile(*ppSigDoc);
	// collect <DataFile> contents
	ddocAppendDataFileData(pDF, lMaxDFLen, (void*)value, strlen((const char*)value));
      }

      // content of <DigestValue>
      if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"DigestValue")) {
	ElementEntry* pParent = 0;
	ddocDebug(4, "ddocParseProcessNode", "DigestValue: %s", (const char*)value);
	l = strlen((const char*)value);
	p = (char*)malloc(l+1);
	RETURN_IF_BAD_ALLOC(p);
	memset(p, 0, l+1);
	decode((const byte*)value, l, (byte*)p, &l);

	// if inside <Reference>
	if((pParent = ddocStackGetParentWithName(pStack, 
		   (xmlChar*)"Reference", pCurrElem)) != NULL) {
	  SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	  RETURN_IF_NULL(pSig);
	  // DataFile digest value
	  if(pParent->uri && pParent->uri[1] == 'D') {
	    DocInfo *pDi = ddocGetLastDocInfo(pSig);
	    if(pDi) {
	      ddocDebug(4, "ddocParseProcessNode", "SIG: %s DocInfo: %s digest", pSig->szId, pDi->szDocId);
	      setString((char**)&(pDi->szDigest), p, l);
	      pDi->nDigestLen = l;
	    }
	  }
	  // SignedProperties digest value
	  if(pParent->uri && pParent->uri[1] == 'S') {
	    ddocDebug(4, "ddocParseProcessNode", "SIG: %s SignedProperties digest", pSig->szId);
	    ddocSigInfo_SetSigPropDigest(pSig, p, l);
	  }
	  
	} // inside <Reference>

	// if inside <SigningCertificate>
	if((pParent = ddocStackGetParentWithName(pStack, (xmlChar*)"SigningCertificate", pCurrElem)) != NULL) {
	  SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	  RETURN_IF_NULL(pSig);
	  pCertID = ddocSigInfo_GetOrCreateCertIDOfType(pSig, CERTID_TYPE_SIGNERS_CERTID);
	  RETURN_IF_NULL(pCertID);
	  // Certificate digest value
	  ddocDebug(4, "ddocParseProcessNode", "SIG: %s signers certificate digest", pSig->szId);
	  ddocCertID_SetDigestValue(pCertID, p, l);
	} // inside <SigningCertificate>

	// if inside <CompleteCertificateRefs>
	if((pParent = ddocStackGetParentWithName(pStack, (xmlChar*)"CompleteCertificateRefs", pCurrElem)) != NULL) {
	  SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	  RETURN_IF_NULL(pSig);
	  pCertID = ddocSigInfo_GetOrCreateCertIDOfType(pSig, CERTID_TYPE_RESPONDERS_CERTID);
	  RETURN_IF_NULL(pCertID);
	  // Certificate digest value
	  ddocDebug(4, "ddocParseProcessNode", "Sig: %s responders certificate digest", pSig->szId);
	  ddocCertID_SetDigestValue(pCertID, p, l);
	} // inside <CompleteCertificateRefs>

	// if inside <CompleteRevocationRefs>
	if((pParent = ddocStackGetParentWithName(pStack, (xmlChar*)"CompleteRevocationRefs", pCurrElem)) != NULL) {
	  NotaryInfo* pNot = ddocGetLastNotaryInfo(*ppSigDoc);
	  RETURN_IF_NULL(pNot);
	  // Certificate digest value
	  ddocDebug(4, "ddocParseProcessNode", "NOT: %s OCSP digest", pNot->szId);
	  ddocNotInfo_SetOcspDigest(pNot, p, l);
	} // inside <CompleteRevocationRefs>


	free(p);
      } // DigestVlue

      // content of <SignatureValue>
      if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"SignatureValue")) {
	SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	ddocDebug(4, "ddocParseProcessNode", "SignatureValue: %s", (const char*)value);
	RETURN_IF_NULL(pSig);
	l = strlen((const char*)value);
	p = (char*)malloc(l+1);
	RETURN_IF_BAD_ALLOC(p);
	memset(p, 0, l+1);
	decode((const byte*)value, l, (byte*)p, &l);
	ddocSigInfo_SetSignatureValue(pSig, p, l);
	free(p);
      } // SignatureValue

      // content of <IssuerSerial> or <X509SerialNumber>
      if((!xmlStrcmp(pCurrElem->tag, (xmlChar*)"IssuerSerial") &&
	  strcmp((*ppSigDoc)->szFormatVer, DIGIDOC_XML_1_3_VER)) ||
	 !xmlStrcmp(pCurrElem->tag, (xmlChar*)"X509SerialNumber")) {
	if(ddocStackHasParentWithName(pStack, (xmlChar*)"SigningCertificate", pCurrElem)) {
	  SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	  ddocDebug(4, "ddocParseProcessNode", "X509IssuerSerial: %s", (const char*)value);
	  pCertID = ddocSigInfo_GetOrCreateCertIDOfType(pSig, CERTID_TYPE_SIGNERS_CERTID);
	  RETURN_IF_NULL(pCertID);
	  ddocCertID_SetIssuerSerial(pCertID, (const char*)value);
	}
	if(ddocStackHasParentWithName(pStack, (xmlChar*)"CompleteCertificateRefs", pCurrElem)) {
	  SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	  RETURN_IF_NULL(pSig);
	  ddocDebug(4, "ddocParseProcessNode", "X509IssuerSerial: %s", (const char*)value);
	  pCertID = ddocSigInfo_GetOrCreateCertIDOfType(pSig, CERTID_TYPE_RESPONDERS_CERTID);
	  RETURN_IF_NULL(pCertID);
	  ddocCertID_SetIssuerSerial(pCertID, (const char*)value);
	}

      } // IssuerSerial

      // content of <X509IssuerName>
      if(!strcmp((*ppSigDoc)->szFormatVer, DIGIDOC_XML_1_3_VER) &&
	 !xmlStrcmp(pCurrElem->tag, (xmlChar*)"X509IssuerName")) {
	if(ddocStackHasParentWithName(pStack, (xmlChar*)"SigningCertificate", pCurrElem)) {
	  SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	  ddocDebug(4, "ddocParseProcessNode", "X509IssuerName: %s", (const char*)value);
	  pCertID = ddocSigInfo_GetOrCreateCertIDOfType(pSig, CERTID_TYPE_SIGNERS_CERTID);
	  RETURN_IF_NULL(pCertID);
	  ddocCertID_SetIssuerName(pCertID, (const char*)value);
	}
	if(ddocStackHasParentWithName(pStack, (xmlChar*)"CompleteCertificateRefs", pCurrElem)) {
	  SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	  RETURN_IF_NULL(pSig);
	  ddocDebug(4, "ddocParseProcessNode", "X509IssuerName: %s", (const char*)value);
	  pCertID = ddocSigInfo_GetOrCreateCertIDOfType(pSig, CERTID_TYPE_RESPONDERS_CERTID);
	  RETURN_IF_NULL(pCertID);
	  ddocCertID_SetIssuerName(pCertID, (const char*)value);
	}

      } // IssuerSerial


      // content of <X509Certificate>
      if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"X509Certificate")) {
	SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
    l = strlen((const char*)value);
	ddocDebug(4, "ddocParseProcessNode", "X509Certificate: %s len: %d", (const char*)value, l);
	RETURN_IF_NULL(pSig);

	ddocDecodeX509PEMData((X509**)&pCert, (const char*)value, l);
	ddocSigInfo_SetSignersCert(pSig, pCert);
      } // X509Certificate

      // content of <SigningTime>
      if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"SigningTime")) {
	SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	ddocDebug(4, "ddocParseProcessNode", "SigningTime: %s", (const char*)value);
	RETURN_IF_NULL(pSig);
	setString((char**)&(pSig->szTimeStamp), (const char*)value, -1);
      } // SigningTime

      // content of <ClaimedRole>
      if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"ClaimedRole")) {
	SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	ddocDebug(4, "ddocParseProcessNode", "ClaimedRole: %s", (const char*)value);
	RETURN_IF_NULL(pSig);
	addSignerRole(pSig, 0, (const char*)value, strlen((const char*)value), 0);
      } // ClaimedRole

      // content of <ResponderID>
      if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"ResponderID")) {
	NotaryInfo* pNot = ddocGetLastNotaryInfo(*ppSigDoc);
	RETURN_IF_NULL(pNot);
	ddocDebug(4, "ddocParseProcessNode", "ResponderID: %s", (const char*)value);
	ddocNotInfo_SetResponderId(pNot, (const char*)value, -1);
      } // ResponderID

      // content of <ProducedAt>
      if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"ProducedAt")) {
	NotaryInfo* pNot = ddocGetLastNotaryInfo(*ppSigDoc);
	RETURN_IF_NULL(pNot);
	ddocDebug(4, "ddocParseProcessNode", "ProducedAt: %s", (const char*)value);
	setString((char**)&(pNot->timeProduced), (const char*)value, -1);
      } // ProducedAt

      // content of <EncapsulatedX509Certificate>
      if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"EncapsulatedX509Certificate")) {
	SignatureInfo *pSig = ddocGetLastSignature(*ppSigDoc);
	RETURN_IF_NULL(pSig);
	ddocDebug(4, "ddocParseProcessNode", "EncapsulatedX509Certificate: %s", (const char*)value);
    l = strlen((const char*)value);
	ddocDecodeX509PEMData((X509**)&pCert, (const char*)value, l);	
	ddocSigInfo_SetOCSPRespondersCert(pSig, pCert);
      } // EncapsulatedX509Certificate

      // content of <EncapsulatedOCSPValue>
      if(!xmlStrcmp(pCurrElem->tag, (xmlChar*)"EncapsulatedOCSPValue")) {
	OCSP_RESPONSE* pResp = 0;
	byte buf1[DIGEST_LEN+2];
	int len1;
	NotaryInfo* pNot;
	SignatureInfo *pSig;

	pNot = ddocGetLastNotaryInfo(*ppSigDoc);
	RETURN_IF_NULL(pNot);
	pSig = ddocGetLastSignature(*ppSigDoc);
	RETURN_IF_NULL(pSig);
	ddocDebug(4, "ddocParseProcessNode", "EncapsulatedOCSPValue: %s", (const char*)value);
	err = ddocDecodeOCSPResponsePEMData(&pResp, (const byte*)value, strlen((const char*)value));
	if(pResp) {
	  // in ver 1.2 we have correct OCP digest
	  // VS 1.76 - add 1.3 version too
	  if(!strcmp((*ppSigDoc)->szFormatVer, DIGIDOC_XML_1_2_VER) ||
	     !strcmp((*ppSigDoc)->szFormatVer, DIGIDOC_XML_1_3_VER)) 
	    err = initializeNotaryInfoWithOCSP(*ppSigDoc, pNot, pResp, ddocSigInfo_GetOCSPRespondersCert(pSig), 0);
	  else // in older versions the digest was incorrect
	    err = initializeNotaryInfoWithOCSP(*ppSigDoc, pNot, pResp, ddocSigInfo_GetOCSPRespondersCert(pSig), 1);
	} else
	  ddocDebug(1, "ddocParseProcessNode", "error decoding OCSP response");
	// check OCSP digest
	len1 = sizeof(buf1);
	err = calculateNotaryInfoDigest(*ppSigDoc, pNot, buf1, &len1);
	pMBuf = ddocNotInfo_GetOcspDigest(pNot);
	RETURN_IF_NULL(pMBuf);
	if(err || compareByteArrays(buf1, len1, (byte*)pMBuf->pMem, pMBuf->nLen)) {
	  SET_LAST_ERROR(ERR_BAD_OCSP_RESPONSE_DIGEST);
	  err = ERR_BAD_OCSP_RESPONSE_DIGEST;
	  ddocDebug(1, "ddocParseProcessNode", "Incorrect OCSP digest, len1: %d, len2: %d",
		    len1, pMBuf->nLen);
	}

      } // EncapsulatedOCSPValue

      xmlFree(value);
      value = 0;
    }
    break;
  case XML_READER_TYPE_WHITESPACE:
  case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
  case XML_READER_TYPE_CDATA:
    value = xmlTextReaderValue(reader);
    if(value) {
      // if we are collecting some xml content (<DataFile>, <SignedInfo>, etc.)
      ddocXRdrRecordElementContent(value, pBuf1);
      // if we are collecting <Signature> xml content
      ddocXRdrRecordElementContent(value, pBuf2);
      xmlFree(value);
      value = 0;
    }
    break;

  case XML_READER_TYPE_ENTITY_REFERENCE:
  case XML_READER_TYPE_ENTITY:
  case XML_READER_TYPE_PROCESSING_INSTRUCTION:
  case XML_READER_TYPE_COMMENT:
  case XML_READER_TYPE_DOCUMENT:
  case XML_READER_TYPE_DOCUMENT_TYPE:
  case XML_READER_TYPE_DOCUMENT_FRAGMENT:
  case XML_READER_TYPE_NOTATION:
    break;
  }
  //ddocDebug(4, "ddocParseProcessNode", "Err %d", err);
  return err;
}


//--------------------------------------------------
// Reads in signed XML document
// szFileName - name of digidoc file
// ppSigDoc - address of new signed doc pointer
// lMaxDFLen - maximum size of a cached DataFile
//--------------------------------------------------
EXP_OPTION int ddocXRdrReadSignedDocFromFile(const char* szFileName, 
					     SignedDoc** pSigDoc, long lMaxDFLen)
{
  char szConvDigiDocName[MAX_FILENAME];
  xmlTextReaderPtr reader;
  ElementEntry stack;
  DigiDocMemBuf memBuf1, memBuf2;
  int ret;

  ddocDebug(3, "ddocXRdrReadSignedDocFromFile", "digidoc: %s, filename charset %s, max df: %ld",
	    szFileName, lMaxDFLen);
  // check input params
  RETURN_IF_NULL_PARAM(szFileName);
  RETURN_IF_NULL_PARAM(pSigDoc);
  memset(&stack, 0, sizeof(ElementEntry));
  *pSigDoc = NULL; // mark as not read in
  ddocDebug(3, "ddocXRdrReadSignedDocFromFile", "digidoc: %s", szFileName);
  clearErrors();
  memBuf1.pMem = 0;
  memBuf1.nLen = 0;
  memBuf2.pMem = 0;
  memBuf2.nLen = 0;
  // converts file names if necessary
  ddocConvertFileName(szConvDigiDocName, sizeof(szConvDigiDocName), szFileName);

  // read digidoc
  reader = xmlNewTextReaderFilename(szConvDigiDocName);
  if(reader != NULL) {
    ret = xmlTextReaderRead(reader);
    // loop over all nodes...
    while(ret == 1) {
      ret = ddocParseProcessNode(reader, pSigDoc, &stack, 
				 &memBuf1, &memBuf2, lMaxDFLen);
      if(ret == ERR_OK)
	ret = 1;
      ret = xmlTextReaderRead(reader);
    }
    // cleanup
    ret = ddocStackPopElement(&stack, 1, NULL);
    if(memBuf1.pMem)
      free(memBuf1.pMem);
    if(memBuf2.pMem)
      free(memBuf2.pMem);
    xmlFreeTextReader(reader);
    if (ret != 0)
      ddocDebug(1, "ddocXRdrReadSignedDocFromFile", "%s : failed to parse: %d", szConvDigiDocName, ret);
    return ret;
  } else {
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_READ);
    ddocDebug(1, "ddocXRdrReadSignedDocFromFile", "Error reading file: %s", szConvDigiDocName);
  }
  return ERR_OK;
}

//--------------------------------------------------
// Reads in signed XML document from memory buffer
// szXml - memory buffer that contains digidoc data
// xmlLen - length of the input data
// szFileNameCharset - digidoc filename charset
// ppSigDoc - address of new signed doc pointer
// lMaxDFLen - maximum size of a cached DataFile
//--------------------------------------------------
EXP_OPTION int ddocXRdrReadSignedDocFromMemory(const char* szXml, int xmlLen,
					     SignedDoc** pSigDoc, long lMaxDFLen)
{
  xmlTextReaderPtr reader;
  ElementEntry stack;
  DigiDocMemBuf memBuf1, memBuf2;
  int ret;

  ddocDebug(3, "ddocXRdrReadSignedDocFromMemory", "data len: %d, max df: %ld", xmlLen, lMaxDFLen);
  // check input params
  RETURN_IF_NULL_PARAM(szXml);
  RETURN_IF_NULL_PARAM(pSigDoc);
  memset(&stack, 0, sizeof(ElementEntry));
  *pSigDoc = NULL; // mark as not read in
  clearErrors();
  memBuf1.pMem = 0;
  memBuf1.nLen = 0;
  memBuf2.pMem = 0;
  memBuf2.nLen = 0;
  // read digidoc
  reader = xmlReaderForMemory(szXml, xmlLen, "http://www.sk.ee/DigiDoc/", "UTF-8", 0);
  if(reader != NULL) {
    ret = xmlTextReaderRead(reader);
    // loop over all nodes...
    while(ret == 1) {
      ret = ddocParseProcessNode(reader, pSigDoc, &stack, 
				 &memBuf1, &memBuf2, lMaxDFLen);
      if(ret == ERR_OK)
	ret = 1;
      ret = xmlTextReaderRead(reader);
    }
    // cleanup
    ret = ddocStackPopElement(&stack, 1, NULL);
    if(memBuf1.pMem)
      free(memBuf1.pMem);
    if(memBuf2.pMem)
      free(memBuf2.pMem);
    xmlFreeTextReader(reader);
    if (ret != 0)
      ddocDebug(1, "ddocXRdrReadSignedDocFromMemory", "failed to parse: %d", ret);
    return ret;
  } else {
    SET_LAST_ERROR_RETURN_CODE(ERR_FILE_READ);
    ddocDebug(1, "ddocXRdrReadSignedDocFromMemory", "Error parsing digidoc");
  }
  return ERR_OK;
}

