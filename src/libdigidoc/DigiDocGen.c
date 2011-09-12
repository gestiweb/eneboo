//==================================================
// FILE:	DigiDocGen.c
// PROJECT:     Digi Doc
// DESCRIPTION: DigiDoc helper routines for XML generation
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
//      11.04.2006      Veiko Sinivee
//                      Creation
//==================================================


#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocConvert.h>
#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/DigiDocSAXParser.h>
#include <libdigidoc/DigiDocDfExtract.h>
#include <libdigidoc/DigiDocGen.h>
#include <libdigidoc/DigiDocError.h>
#include <string.h>
#include <time.h>

#include <libxml/globals.h>
#include <libxml/xmlerror.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h> /* only for xmlNewInputFromFile() */
#include <libxml/tree.h>
#include <libxml/debugXML.h>
#include <libxml/xmlmemory.h>
#include <libxml/c14n.h>


#include <fcntl.h>

//-----------< helper functions >----------------------------


//--------------------------------------------------
// Appends an xml element start to buffer, but no ">"
// pBuf - memory buffer to store xml [REQUIRED]
// elemName - xml element name [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocGen_startElemBegin(DigiDocMemBuf* pBuf, const char* elemName)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pBuf)
  RETURN_IF_NULL_PARAM(elemName)
  err = ddocMemAppendData(pBuf, "<", -1);
  if(err) return err;
  err = ddocMemAppendData(pBuf, elemName, -1);
  return err;
}

//--------------------------------------------------
// Appends an xml element start tag end to buffer - ">"
// pBuf - memory buffer to store xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocGen_startElemEnd(DigiDocMemBuf* pBuf)
{
  RETURN_IF_NULL_PARAM(pBuf)
  return ddocMemAppendData(pBuf, ">", -1);
}

//--------------------------------------------------
// Appends an xml element start to buffer - <tag>
// pBuf - memory buffer to store xml [REQUIRED]
// elemName - xml element name [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocGen_startElem(DigiDocMemBuf* pBuf, const char* elemName)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pBuf)
  RETURN_IF_NULL_PARAM(elemName)
  err = ddocMemAppendData(pBuf, "<", -1);
  if(err) return err;
  err = ddocMemAppendData(pBuf, elemName, -1);
  if(err) return err;
  err = ddocMemAppendData(pBuf, ">", -1);
  return err;
}

//--------------------------------------------------
// Appends an xml element end to buffer
// pBuf - memory buffer to store xml [REQUIRED]
// elemName - xml element name [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocGen_endElem(DigiDocMemBuf* pBuf, const char* elemName)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pBuf)
  RETURN_IF_NULL_PARAM(elemName)
  err = ddocMemAppendData(pBuf, "</", -1);
  if(err) return err;
  err = ddocMemAppendData(pBuf, elemName, -1);
  if(err) return err;
  err = ddocMemAppendData(pBuf, ">", -1);
  return err;
}

//--------------------------------------------------
// Appends an xml element's atribute to buffer
// pBuf - memory buffer to store xml [REQUIRED]
// name - xml atribute name [REQUIRED]
// value - xml atribute value [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocGen_addAtribute(DigiDocMemBuf* pBuf, const char* name, const char* value)
{
  int err = ERR_OK;
  RETURN_IF_NULL_PARAM(pBuf)
  RETURN_IF_NULL_PARAM(name)
  RETURN_IF_NULL_PARAM(value)
  err = ddocMemAppendData(pBuf, " ", -1);
  if(err) return err;
  err = ddocMemAppendData(pBuf, name, -1);
  if(err) return err;
  err = ddocMemAppendData(pBuf, "=\"", -1);
  if(err) return err;
  err = ddocMemAppendData(pBuf, value, -1);
  if(err) return err;
  err = ddocMemAppendData(pBuf, "\"", -1);
  return err;
}

//================< functions Timestamp_st > =================================
#ifdef WITH_TIMETSTAMP_STRUCT

//===================================================================
// converts string to timestamp
// IN  const char* szTimestamp  - timestamp string
// OUT Timestamp* pTimestamp
//===================================================================
EXP_OPTION int convertStringToTimestamp(const SignedDoc* pSigDoc, const char* szTimestamp, Timestamp* pTimestamp)
{
  RETURN_IF_NULL_PARAM(szTimestamp);
  RETURN_IF_NULL_PARAM(pTimestamp);
  RETURN_IF_NULL_PARAM(pSigDoc);
  // in version 1.3 we use format CCYY-MM-DDTHH:MM:SS-TZ
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER)
#ifdef WITH_TS
    || !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)
#endif
	  ) {
    sscanf(szTimestamp, "%04d-%02d-%04dT%02d:%02d:%02dZ",
	   &(pTimestamp->year), &(pTimestamp->mon), &(pTimestamp->day), 
	   &(pTimestamp->hour), &(pTimestamp->min), &(pTimestamp->sec));
    pTimestamp->tz = 0;
  } else
    // in version 1.0 we use format CCYY.MM.DDTHH:MM:SS-TZ
    if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER)) {
      sscanf(szTimestamp, "%04d.%02d.%04dT%02d:%02d:%02d%3d:00",
	     &(pTimestamp->year), &(pTimestamp->mon), &(pTimestamp->day), 
	     &(pTimestamp->hour), &(pTimestamp->min), &(pTimestamp->sec), &(pTimestamp->tz));
    } else { // in version 1.1 we use format CCYY.MM.DDTHH:MM:SSZ and allways UTC time
      sscanf(szTimestamp, "%04d.%02d.%04dT%02d:%02d:%02dZ",
	     &(pTimestamp->year), &(pTimestamp->mon), &(pTimestamp->day), 
	     &(pTimestamp->hour), &(pTimestamp->min), &(pTimestamp->sec));
      pTimestamp->tz = 0;
    }
  return ERR_OK;
}

//===================================================================
// converts string to timestamp
// IN  const char* szTimestamp  - timestamp string
// OUT Timestamp* pTimestamp
//===================================================================
EXP_OPTION int convertTimestampToString(const SignedDoc* pSigDoc, const Timestamp* pTimestamp, 
	char* szTimestamp, int len)
{
  RETURN_IF_NULL_PARAM(szTimestamp);
  RETURN_IF_NULL_PARAM(pTimestamp);
  //RETURN_IF_NULL_PARAM(pSigDoc); // if null then latest format is used

  // in version 1.3 we use format CCYY-MM-DDTHH:MM:SS-TZ
  if(!pSigDoc || (pSigDoc && (
       !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER) 
#ifdef WITH_TS
       || !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)
#endif
       ))) {
    snprintf(szTimestamp, len, "%04d-%02d-%02dT%02d:%02d:%02dZ",pTimestamp->year,
	    pTimestamp->mon, pTimestamp->day, pTimestamp->hour , pTimestamp->min,
	    pTimestamp->sec);
  } else	// in version 1.0 we use format CCYY.MM.DDTHH:MM:SS-TZ
    if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER)) {
      snprintf(szTimestamp, len, "%04d.%02d.%02dT%02d:%02d:%02d%+03d:00",pTimestamp->year,
	      pTimestamp->mon, pTimestamp->day, pTimestamp->hour , pTimestamp->min,
	      pTimestamp->sec, pTimestamp->tz);
    } else {  // in version 1.1 we use format CCYY.MM.DDTHH:MM:SSZ and allways UTC time
      snprintf(szTimestamp, len, "%04d.%02d.%02dT%02d:%02d:%02dZ",pTimestamp->year,
	      pTimestamp->mon, pTimestamp->day, pTimestamp->hour , pTimestamp->min,
	      pTimestamp->sec);
    }
  return ERR_OK;
}

// converts date integers to timestamp object
EXP_OPTION int Timestamp_new(Timestamp **ppTimestamp, int year,int month,int day,int hour,int minute,int second,int timezone){
	Timestamp* pTimestamp;
	pTimestamp = (Timestamp*)malloc(sizeof(Timestamp));
	RETURN_IF_BAD_ALLOC(pTimestamp);
	memset(pTimestamp, 0, sizeof(Timestamp));
	pTimestamp->year=year;
	pTimestamp->mon=month;
	pTimestamp->day=day;
	pTimestamp->hour=hour;
	pTimestamp->min=minute;
	pTimestamp->sec=second;
	pTimestamp->tz=timezone;
	*ppTimestamp = pTimestamp;
	return ERR_OK;
}

//======================================================================
// frees timestamp object
//======================================================================
EXP_OPTION void Timestamp_free(Timestamp* pTimestamp){
	free(pTimestamp);
}


#endif

//===================================================================
// converts timestamp string to time_t value
// IN  const char* szTimestamp  - timestamp string
// OUT Timestamp* pTimestamp
//===================================================================
EXP_OPTION time_t convertStringToTimeT(const SignedDoc* pSigDoc, const char* szTimestamp)
{
  struct tm tm1;
  int tz, dmz = 0;
  time_t t2;

  memset(&tm1, 0, sizeof(tm1));
  tzset();
  t2 = 0;
  // in version 1.0 we use format CCYY.MM.DDTHH:MM:SS-TZ
  if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER)) {
    sscanf(szTimestamp, "%04d.%02d.%02dT%02d:%02d:%02d+%03d:00", 
	   &(tm1.tm_year), &(tm1.tm_mon), &(tm1.tm_mday), 
	   &(tm1.tm_hour) , &(tm1.tm_min), &(tm1.tm_sec), &tz);
  } else if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_1_VER) ||
	    !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_2_VER)) {  // in version 1.1 we use format CCYY.MM.DDTHH:MM:SSZ and allways UTC time
    sscanf(szTimestamp, "%04d.%02d.%02dT%02d:%02d:%02dZ", 
	   &(tm1.tm_year), &(tm1.tm_mon), &(tm1.tm_mday), 
	   &(tm1.tm_hour), &(tm1.tm_min), &(tm1.tm_sec));
  } else {  // in version 1.3 we use format CCYY-MM-DDTHH:MM:SSZ and allways UTC time
    sscanf(szTimestamp, "%04d-%02d-%02dT%02d:%02d:%02dZ", 
	   &(tm1.tm_year), &(tm1.tm_mon), &(tm1.tm_mday), 
	   &(tm1.tm_hour), &(tm1.tm_min), &(tm1.tm_sec));
  }
  tm1.tm_year -= 1900;
  tm1.tm_mon -= 1;
  tm1.tm_isdst = daylight;				
  t2 = mktime(&tm1);
  if(_daylight != 0) {
    if(_timezone < 0)
      dmz = (_timezone / 3600) - daylight;
    else
      dmz = (_timezone / 3600) + daylight;
  }
  else
    dmz = _timezone / 3600;
  t2 -= (dmz * 3600);
  return t2;
}


//================< functions generating DigiDoc formats 1.0 - 1.3 > =================================



//============================================================
// Creates a timestamp string
// buf - output buffer
// len - length of output buffer
// returns number of output bytes written
//============================================================
int createTimestamp(const SignedDoc* pSigDoc, char* buf, int len)
{
  time_t t;
  struct tm tm1;
  Timestamp *pTimestamp;
  int dmz=0;

  RETURN_OBJ_IF_NULL(buf, 0);
  _tzset();
  time(&t);
  // in version 1.0 we use format CCYY.MM.DDTHH:MM:SS-TZ
  if(pSigDoc && !strcmp(pSigDoc->szFormatVer, "1.0")) {
    ddocLocalTime(&t, &tm1, 1);
    if(_daylight != 0) {
      /*if(_timezone<0){*/
      dmz=(_timezone - (_daylight*3600))/ 3600;
      /*}else{
	dmz=(_timezone + (_daylight*3600))/ 3600;
	}*/
    }else{
      dmz = _timezone / 3600;
    }		
  } else { // in version 1.1 we use UTC time
    ddocLocalTime(&t, &tm1, 0);
  }
  (void)Timestamp_new(&pTimestamp, tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday,
		      tm1.tm_hour, tm1.tm_min, tm1.tm_sec, dmz);
  (void)convertTimestampToString(pSigDoc, pTimestamp, buf, len);
  Timestamp_free(pTimestamp);
  return strlen(buf);
}

//============================================================
// Canonicalizes XML
// source - input data
// len - input length
// returns a newly allocated buffer with canonicalized XML
// Caller must free() the result.
//============================================================
char* canonicalizeXML(char* source, int len)
{
  xmlDocPtr doc = NULL;
  xmlChar* pBuf = NULL;
  int rc, n;
  char* dest = NULL;
	
  ddocDebug(5, "canonicalizeXML", "Canonicalizing: %d bytes", len);
  if((doc = xmlParseMemory(source, len)) != NULL) {
    ddocDebug(5, "canonicalizeXML", "Canonicalizing parse: %s", (doc ? "OK" : "ERROR"));
    rc = xmlC14NDocDumpMemory(doc, NULL, 0, NULL, 0, &pBuf); 
    ddocDebug(5, "canonicalizeXML", "Canonicalizing RC: %d: BUF: %s", rc, (pBuf ? "OK" : "ERROR"));
    if(pBuf) {
      n = strlen((char*)pBuf);
      dest = (char*)malloc(n + 1);
      if(dest)
	strncpy(dest, (char*)pBuf, n+1);
      else
	SET_LAST_ERROR_IF_NOT(dest, ERR_BAD_ALLOC);
      xmlFree(pBuf);
    }
    xmlFreeDoc(doc);
  }
  return dest;
}

//--------------------------------------------------
// Helper function that escapes XML special chars
// src - input data
// srclen - length of input data. Use -1 for 0 terminated strings
// dest - address of output buffer. Caller is responsible for deallocating it!
// returns error code or ERR_OK
//--------------------------------------------------
int escapeXMLSymbols(const char* src, int srclen, char** dest)
{
  int j, i, n = srclen, l;
  char *p = 0;

  RETURN_IF_NULL_PARAM(src);
  RETURN_IF_NULL_PARAM(dest);
  *dest = 0;
  if(n < 0)
    n = strlen(src);
  if(n > 0 && ConfigItem_lookup_int("DEBUG_LEVEL", 1) >= 5) {
    p = (char*)malloc(n+1);
    if(p) {
      memset(p, 0, n+1);
      memcpy(p, src, n);
      ddocDebug(5, "escapeXMLSymbols", "src: \"%s\" len: %d", p, n);
      free(p);
    }
  }
  else
    ddocDebug(5, "escapeXMLSymbols", "src: \"%s\" len: %d", src, n);
  // count the amount of memory needed for conversion
  for(i = l = 0; i < n; i++) {
     switch(src[i]) {
     case '<': l += 4; break;
     case '>': l += 4; break;
     case '&': l += 5; break;
     case '\r': l += 5; break;
     case '\'': l += 6; break;
     case '\"': l += 6; break;
     default: l ++; break;
     }
  }
  // count the last terminator char
  l++;
  ddocDebug(5, "escapeXMLSymbols", "allocating: %d bytes", l);
  *dest = (char*)malloc(l);
  memset(*dest, 0, l);
  // now convert the data
  for(i = j = 0; i < n; i++) {
     switch(src[i]) {
     case '<': strncat(*dest, "&lt;", l - strlen(*dest)); j += 4; break;
     case '>': strncat(*dest, "&gt;", l - strlen(*dest)); j += 4; break;
     case '&': strncat(*dest, "&amp;", l - strlen(*dest)); j += 5; break;
     case '\r': strncat(*dest, "&#xD;", l - strlen(*dest)); j += 5; break;
     case '\'': strncat(*dest, "&apos;", l - strlen(*dest)); j += 6; break;
     case '\"': strncat(*dest, "&quot;", l - strlen(*dest)); j += 6; break;
     default: (*dest)[j] = src[i]; j++; break;
     }
  }
  ddocDebug(5, "escapeXMLSymbols", "Converted: \'%s\' len: %d", *dest, j);
  return ERR_OK;
}


//============================================================
// Creates a <SignedProperties> XML block
// pSigDoc - signed document pointer
// pSigInfo - signature info data
// bWithEscapes - 1=escape xml sümbols, 0=don't escape
// returns new <SignedProperties> node
//============================================================
char* createXMLSignedProperties(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo, int bWithEscapes)
{
  char buf1[300], *pRet = 0, *p1, *p2;
  int len1, i, err;
  xmlNodePtr pSigProp, pSigSigProp, pSigCert, pN1, pN2, pN3;
  static xmlChar nl[] = "\n";
  xmlDocPtr doc;
  xmlChar* pBuf;
  DigiDocMemBuf *pMBuf1;

  // XML doc
  doc = xmlNewDoc((const xmlChar*)"1.0");
  doc->children = xmlNewDocNode(doc, NULL, (const xmlChar*)"", NULL);
  // <SignedProperties>
  pSigProp = xmlNewChild(doc->children, NULL, (const xmlChar*)"SignedProperties", nl);
  // Ver 1.76 - in format 1.3 xmlns atribute is not used
  // in ver 1.1 we need this namespace def
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_1_VER) ||
     !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_2_VER)) {
    xmlNewNs(pSigProp, (const xmlChar*)NAMESPACE_XML_DSIG, NULL);
  } else if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER)) { // in 1.3 we use the correct etsi namespace
    xmlNewNs(pSigProp, (const xmlChar*)NAMESPACE_XADES_111, NULL);
  } 
#ifdef WITH_TS
  else if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)) { // in 1.4 we use 1.3.2 namespace
    xmlNewNs(pSigProp, (const xmlChar*)NAMESPACE_XADES_132, NULL);
  }
#endif
  // in 1.0 we had this buggy URI
  if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER))
    snprintf(buf1, sizeof(buf1), "#%s-SignedProperties", pSigInfo->szId);
  else // current version is 1.1
    snprintf(buf1, sizeof(buf1), "%s-SignedProperties", pSigInfo->szId);
  xmlSetProp(pSigProp, (const xmlChar*)"Id", (const xmlChar*)buf1);
  
  // Ver 1.76 - in format 1.3 Target atribute is not used
  if(strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER)
#ifdef WITH_TS
     && strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)
#endif
     ) {
    snprintf(buf1, sizeof(buf1), "#%s", pSigInfo->szId);
    xmlSetProp(pSigProp, (const xmlChar*)"Target", (const xmlChar*)buf1);
  }
  // <SignedSignatureProperties>
  pSigSigProp = xmlNewChild(pSigProp, NULL, (const xmlChar*)"SignedSignatureProperties", nl);
  // <SigningTime>
  pN1 = xmlNewChild(pSigSigProp, NULL, (const xmlChar*)"SigningTime", (const xmlChar*)pSigInfo->szTimeStamp);
  xmlNodeAddContent(pSigSigProp, nl);
  // <SigningCertificate>
  pSigCert = xmlNewChild(pSigSigProp, NULL, (const xmlChar*)"SigningCertificate", nl);
  // <Cert>	
  pN1 = xmlNewChild(pSigCert, NULL, (const xmlChar*)"Cert", nl);
  // Ver 1.76 - in format 1.3 Id atribute is not used
  if(strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER)
#ifdef WITH_TS
     && strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)
#endif
     ) {
    // in 1.0 we had this buggy URI
    if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER))
      snprintf(buf1, sizeof(buf1), "#%s-CERTINFO", pSigInfo->szId);
    else
      snprintf(buf1, sizeof(buf1), "%s-CERTINFO", pSigInfo->szId);
    xmlSetProp(pN1, (const xmlChar*)"Id", (const xmlChar*)buf1);
  }
  // <CertDigest>
  pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"CertDigest", nl);
  // <DigestMethod>
  pN3 = xmlNewChild(pN2, NULL, (const xmlChar*)"DigestMethod", nl);
#ifdef WITH_TS
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)) 
    xmlSetProp(pN3, (const xmlChar*)"xmlns", (const xmlChar*)NAMESPACE_XML_DSIG);
#endif
  xmlNodeAddContent(pN2, nl);
  xmlSetProp(pN3, (const xmlChar*)"Algorithm", (const xmlChar*)DIGEST_METHOD_SHA1);
  // <DigestValue>
  len1 = sizeof(buf1);
  buf1[0] = 0;
  pMBuf1 = ddocSigInfo_GetSignersCert_DigestValue(pSigInfo);
  if(pMBuf1) 
    encode((const byte*)pMBuf1->pMem, pMBuf1->nLen, (byte*)buf1, &len1);
  pN3 = xmlNewChild(pN2, NULL, (const xmlChar*)"DigestValue", (const xmlChar*)buf1);
#ifdef WITH_TS
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)) 
    xmlSetProp(pN3, (const xmlChar*)"xmlns", (const xmlChar*)NAMESPACE_XML_DSIG);
#endif
  xmlNodeAddContent(pN2, nl);
  // <IssuerSerial>
  xmlNodeAddContent(pN1, nl);
  // In 1.3 and 1.4 we use subelements of <IssuerSerial>
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER)
#ifdef WITH_TS
     || !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)
#endif
     ) {
    pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"IssuerSerial", nl);
    pN3 = xmlNewChild(pN2, NULL, (const xmlChar*)"X509IssuerName", 
		      (const xmlChar*)ddocSigInfo_GetSignersCert_IssuerName((SignatureInfo*)pSigInfo));
    xmlSetProp(pN3, (const xmlChar*)"xmlns", (const xmlChar*)NAMESPACE_XML_DSIG);
    xmlNodeAddContent(pN2, nl);
    pN3 = xmlNewChild(pN2, NULL, (const xmlChar*)"X509SerialNumber", 
		      (const xmlChar*)ddocSigInfo_GetSignersCert_IssuerSerial((SignatureInfo*)pSigInfo));
    xmlSetProp(pN3, (const xmlChar*)"xmlns", (const xmlChar*)NAMESPACE_XML_DSIG);
    xmlNodeAddContent(pN2, nl);
  } else {
    pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"IssuerSerial", 
		      (const xmlChar*)ddocSigInfo_GetSignersCert_IssuerSerial((SignatureInfo*)pSigInfo));
  }
  // <SignaturePolicyIdentifier>
  xmlNodeAddContent(pSigSigProp, nl);
  if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER) ||
     !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_1_VER) ||
     !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_2_VER) ||
     !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER)) {
    pN1 = xmlNewChild(pSigSigProp, NULL, (const xmlChar*)"SignaturePolicyIdentifier", nl);
    // <SignaturePolicyImplied>
    pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"SignaturePolicyImplied", nl);
    xmlNodeAddContent(pN1, nl);
  }
  // <SignatureProductionPlace>
  if(pSigInfo->sigProdPlace.szCity || pSigInfo->sigProdPlace.szStateOrProvince ||
     pSigInfo->sigProdPlace.szPostalCode || pSigInfo->sigProdPlace.szCountryName) {
    xmlNodeAddContent(pSigSigProp, nl);
    pN1 = xmlNewChild(pSigSigProp, NULL, (const xmlChar*)"SignatureProductionPlace", nl);
    if(pSigInfo->sigProdPlace.szCity) {
      if(bWithEscapes) {
	escapeXMLSymbols(pSigInfo->sigProdPlace.szCity, -1, &p2);
	pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"City", (const xmlChar*)p2);
	free(p2);
      }
      else
	pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"City", (const xmlChar*)pSigInfo->sigProdPlace.szCity);
      xmlNodeAddContent(pN1, nl);
    }
    if(pSigInfo->sigProdPlace.szStateOrProvince) {
      if(bWithEscapes) {
	escapeXMLSymbols(pSigInfo->sigProdPlace.szStateOrProvince, -1, &p2);
	pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"StateOrProvince", (const xmlChar*)p2);
	free(p2);
      }
      else
	pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"StateOrProvince", (const xmlChar*)pSigInfo->sigProdPlace.szStateOrProvince);
      xmlNodeAddContent(pN1, nl);
    }
    if(pSigInfo->sigProdPlace.szPostalCode) {
      if(bWithEscapes) {
	escapeXMLSymbols(pSigInfo->sigProdPlace.szPostalCode, -1, &p2);
	pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"PostalCode", (const xmlChar*)p2);
	free(p2);
      }
      else
	pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"PostalCode", (const xmlChar*)pSigInfo->sigProdPlace.szPostalCode);
      xmlNodeAddContent(pN1, nl);
    }
    if(pSigInfo->sigProdPlace.szCountryName) {
      if(bWithEscapes) {
	escapeXMLSymbols(pSigInfo->sigProdPlace.szCountryName, -1, &p2);
	pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"CountryName", (const xmlChar*)p2);
	free(p2);
      }
      else
	pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"CountryName", (const xmlChar*)pSigInfo->sigProdPlace.szCountryName);
      xmlNodeAddContent(pN1, nl);
    }
    xmlNodeAddContent(pSigSigProp, nl);
  }
  // <SignerRole>
  if(pSigInfo->signerRole.nClaimedRoles ||
     pSigInfo->signerRole.nCertifiedRoles) {
    pN1 = xmlNewChild(pSigSigProp, NULL, (const xmlChar*)"SignerRole", nl);
    // <ClaimedRoles>
    if(pSigInfo->signerRole.nClaimedRoles) {
      pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"ClaimedRoles", nl);
      for(i = 0; i < pSigInfo->signerRole.nClaimedRoles; i++) {
	if(bWithEscapes) {
	  escapeXMLSymbols(pSigInfo->signerRole.pClaimedRoles[i], -1, &p2);
	  ddocDebug(4, "createXMLSignedProperties", "file: %s --> %s", pSigInfo->signerRole.pClaimedRoles[i], p2);
	  pN3 = xmlNewChild(pN2, NULL, (const xmlChar*)"ClaimedRole", (const xmlChar*)p2);
	  free(p2);
	} else 
	  pN3 = xmlNewChild(pN2, NULL, (const xmlChar*)"ClaimedRole", (const xmlChar*)pSigInfo->signerRole.pClaimedRoles[i]);
	xmlNodeAddContent(pN2, nl);
      }
      xmlNodeAddContent(pN1, nl);
    }
    // <CertifiedRoles>
    if(pSigInfo->signerRole.nCertifiedRoles) {
      pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"CertifiedRoles", nl);
      for(i = 0; i < pSigInfo->signerRole.nClaimedRoles; i++) {
	escapeXMLSymbols(pSigInfo->signerRole.pCertifiedRoles[i], -1, &p2);
	pN3 = xmlNewChild(pN2, NULL, (const xmlChar*)"CertifiedRole", (const xmlChar*)p2);
	free(p2);
	xmlNodeAddContent(pN2, nl);
      }
      xmlNodeAddContent(pN1, nl);
    }
    xmlNodeAddContent(pSigSigProp, nl);
  }
  // <SignedDataObjectProperties>
  xmlNodeAddContent(pSigProp, nl);
  pN1 = xmlNewChild(pSigProp, NULL, (const xmlChar*)"SignedDataObjectProperties", nl);
  xmlNodeAddContent(pSigProp, nl);
  // convert to string
  pBuf = NULL;
  err = xmlC14NDocDumpMemory(doc, NULL, 0, NULL, 0, &pBuf);	
  p1 = strstr((const char*)pBuf, "<SignedProperties");
  if(p1) {
    p2 = strstr((const char*)pBuf, "</SignedProperties>");
    if(p2) {
      p2[strlen("</SignedProperties>")] = 0;
      pRet = strdup(p1);
    }
  }
  xmlFree(pBuf);
  xmlFreeDoc(doc);
  // return <SignedProperties> node
  return pRet;
}


//============================================================
// Creates a <SignedInfo> XML block for a signature
// This is the actual data to be signed
// pSigInfo - signature info data
// buf - output buffer
// returns number of output bytes written
//============================================================
EXP_OPTION char* createXMLSignedInfo(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo)
{
  char buf1[100], *pRet = 0, *p1, *p2;
  int i, err, l1;
  xmlNodePtr pnSigInfo, pN1, pN2, pN3;
  static xmlChar nl[] = "\n";
  xmlDocPtr doc;
  xmlChar* pBuf;
  //FILE* hFile;
  DataFile* pDF;
  DigiDocMemBuf mbuf1;
  
  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  RETURN_OBJ_IF_NULL(pSigInfo, 0);
  // XML doc
  doc = xmlNewDoc((const xmlChar*)"1.0");
  doc->children = xmlNewDocNode(doc, NULL, (const xmlChar*)"", NULL);
  // <SignedInfo>
  pnSigInfo = xmlNewChild(doc->children, NULL, (const xmlChar*)"SignedInfo", nl);
  // in ver 1.1 we need this namespace def
  xmlNewNs(pnSigInfo, (const xmlChar*)NAMESPACE_XML_DSIG, NULL);
  // <CanonicalizationMethod>
  pN1 = xmlNewChild(pnSigInfo, NULL, (const xmlChar*)"CanonicalizationMethod", nl);
  xmlSetProp(pN1, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/TR/2001/REC-xml-c14n-20010315");
  xmlNodeAddContent(pnSigInfo, nl);
  // <SignatureMethod>
  pN1 = xmlNewChild(pnSigInfo, NULL, (const xmlChar*)"SignatureMethod", nl);
  xmlSetProp(pN1, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2000/09/xmldsig#rsa-sha1");
  xmlNodeAddContent(pnSigInfo, nl);
  // <Reference>
  for(i = 0; i < pSigInfo->nDocs; i++) {
    // documents digest
    pN1 = xmlNewChild(pnSigInfo, NULL, (const xmlChar*)"Reference", nl);
    snprintf(buf1, sizeof(buf1), "#%s", pSigInfo->pDocs[i]->szDocId);
    xmlSetProp(pN1, (const xmlChar*)"URI", (const xmlChar*)buf1);
    pDF = getDataFileWithId(pSigDoc, pSigInfo->pDocs[i]->szDocId);
    // in ver 1.1 we use Transforms only for detached documents
    if((!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_1_VER) ||
	!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_2_VER)) &&
       pDF && !strcmp(pDF->szContentType, CONTENT_DETATCHED)) {
      pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"Transforms", NULL);
      pN3 = xmlNewChild(pN2, NULL, (const xmlChar*)"Transform", NULL);
      xmlSetProp(pN3, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.sk.ee/2002/10/digidoc#detatched-document-signature");
      xmlNodeAddContent(pN1, nl);
    }
    pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"DigestMethod", nl);
    xmlSetProp(pN2, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2000/09/xmldsig#sha1");
    xmlNodeAddContent(pN1, nl);
    // in ver 1.0 we use Transforms
    if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER)) {
      pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"Transforms", NULL);
      pN3 = xmlNewChild(pN2, NULL, (const xmlChar*)"Transform", NULL);
      xmlSetProp(pN3, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2000/09/xmldsig#enveloped-signature");
      xmlNodeAddContent(pN1, nl);
    }
    l1 = sizeof(buf1);
    encode(pSigInfo->pDocs[i]->szDigest, 
	   pSigInfo->pDocs[i]->nDigestLen, (byte*)buf1, &l1);
    pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"DigestValue", (const xmlChar*)buf1);
    xmlNodeAddContent(pN1, nl);
    xmlNodeAddContent(pnSigInfo, nl);
    // in ver 1.1 we don't use mime digest Reference blocks
    if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER)) {
      // document mime type digest
      pN1 = xmlNewChild(pnSigInfo, NULL, (const xmlChar*)"Reference", nl);
      if(!strcmp(pSigDoc->szFormatVer, "1.0"))
	snprintf(buf1, sizeof(buf1), "#%s-MIME", pSigInfo->pDocs[i]->szDocId);
      else // current version is 1.1
	snprintf(buf1, sizeof(buf1), "#%s@MimeType", pSigInfo->pDocs[i]->szDocId);
      xmlSetProp(pN1, (const xmlChar*)"URI", (const xmlChar*)buf1);
      pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"DigestMethod", nl);
      xmlSetProp(pN2, (const xmlChar*)"Algorithm", (const xmlChar*)DIGEST_METHOD_SHA1);
      xmlNodeAddContent(pN1, nl);
      pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"Transforms", NULL);
      pN3 = xmlNewChild(pN2, NULL, (const xmlChar*)"Transform", NULL);
      xmlSetProp(pN3, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2000/09/xmldsig#enveloped-signature");
      xmlNodeAddContent(pN1, nl);
      l1 = sizeof(buf1);
      encode(pSigInfo->pDocs[i]->szMimeDigest, 
	     pSigInfo->pDocs[i]->nMimeDigestLen, (byte*)buf1, &l1);
      pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"DigestValue", (const xmlChar*)buf1);
      xmlNodeAddContent(pN1, nl);
    }
  }
  // signed properties digest
  pN1 = xmlNewChild(pnSigInfo, NULL, (const xmlChar*)"Reference", nl);
  snprintf(buf1, sizeof(buf1), "#%s-SignedProperties", pSigInfo->szId);
  xmlSetProp(pN1, (const xmlChar*)"URI", (const xmlChar*)buf1);
  // in version 1.2 we ise the Type atribute
  // for References that contain <SignedProperties> digest
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_2_VER) ||
     !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER)) {
    xmlSetProp(pN1, (const xmlChar*)"Type", (const xmlChar*)"http://uri.etsi.org/01903/v1.1.1#SignedProperties");
  }
#ifdef WITH_TS
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)) {
    xmlSetProp(pN1, (const xmlChar*)"Type", (const xmlChar*)"http://uri.etsi.org/01903#SignedProperties");
  }
#endif
  pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"DigestMethod", nl);
  xmlSetProp(pN2, (const xmlChar*)"Algorithm", (const xmlChar*)DIGEST_METHOD_SHA1);
  xmlNodeAddContent(pN1, nl);
  // in 1.0 we used transforms here
  if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER)) {
    pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"Transforms", NULL);
    pN3 = xmlNewChild(pN2, NULL, (const xmlChar*)"Transform", NULL);
    xmlSetProp(pN3, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2000/09/xmldsig#enveloped-signature");
    xmlNodeAddContent(pN1, nl);
  }
  ddocEncodeBase64(ddocDigestValue_GetDigestValue(pSigInfo->pSigPropDigest), &mbuf1);
  pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"DigestValue", (const xmlChar*)mbuf1.pMem);
  ddocMemBuf_free(&mbuf1);
  xmlNodeAddContent(pN1, nl);
  xmlNodeAddContent(pnSigInfo, nl);
  // convert to string
  pBuf = NULL;
  err = xmlC14NDocDumpMemory(doc, NULL, 0, NULL, 0, &pBuf);	
  p1 = strstr((const char*)pBuf, "<SignedInfo");
  if(p1) {
    p2 = strstr((const char*)pBuf, "</SignedInfo>");
    if(p2) {
      p2[strlen("</SignedInfo>")] = 0;
      pRet = strdup(p1);
    }
  }
  xmlFree(pBuf);
  xmlFreeDoc(doc);
  return pRet;
}

//============================================================
// Calculates the digest of OCSP_RESPONSE
// pResp - OCSP_RESPONSE data 
// digBuf - signature buffer
// digLen - signature buffer length
// returns error code
//============================================================
int calculateOcspBasicResponseDigest(OCSP_BASICRESP* pBsResp, byte* digBuf, int* digLen)
{
  int err = ERR_OK, l1;
  byte *buf, *p;

  RETURN_IF_NULL_PARAM(pBsResp);
  l1 = i2d_OCSP_BASICRESP(pBsResp, NULL);
  buf = (byte*)malloc(l1+10);
  RETURN_IF_BAD_ALLOC(buf);
  p = buf;
  i2d_OCSP_BASICRESP(pBsResp, &p);
  err = calculateDigest(buf, l1, DIGEST_SHA1, digBuf, digLen);
  free(buf);
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}

//============================================================
// Writes OCSP_RESPONSE to file without the usual PEM headers
// bout - output file
// pResp - OCSP_RESPONSE
// returns error code
//============================================================
/*int writeOcspToXMLFile(DigiDocMemBuf* pBuf, OCSP_RESPONSE* pResp)
{
  int l1, l2;
  char *p1, *p2;

  RETURN_IF_NULL_PARAM(pBuf);
  RETURN_IF_NULL_PARAM(pResp);
  l1 = i2d_OCSP_RESPONSE(pResp, NULL);
  p1 = (char*)malloc(l1+10);
  RETURN_IF_BAD_ALLOC(p1);
  p2 = p1;
  i2d_OCSP_RESPONSE(pResp, (unsigned char**)&p2);
  l2 = l1 * 2;
  p2 = (char*)malloc(l2);
  if(p2 == NULL) {
    free(p1);
    RETURN_IF_BAD_ALLOC(p2);
  }
  encode((const byte*)p1, l1, (byte*)p2, &l2);
  ddocMemAppendData(pBuf, p2, -1);
  free(p2);
  free(p1);
  return ERR_OK;
}*/


//============================================================
// Adds a notary signature to signed document buffer
// pMBufXML - output buffer
// pSigDoc - signed doc pointer
// pNotInfo - notary signature info 
// returns error code
//============================================================
int addNotaryInfoXML(DigiDocMemBuf *pMBufXML, const SignedDoc *pSigDoc, const SignatureInfo* pSigInfo, const NotaryInfo* pNotInfo)
{
  int err = ERR_OK, i;
  DigiDocMemBuf mbuf1;
  const DigiDocMemBuf *pMBuf;
  CertValue *pCertValue = 0;

  RETURN_IF_NULL_PARAM(pSigInfo);
  RETURN_IF_NULL_PARAM(pNotInfo);
  RETURN_IF_NULL_PARAM(pMBufXML);
  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  // unsigned prop begin
  // Ver 1.76 - in format 1.3 we don't use the Target atribute
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER)) {
    ddocMemAppendData(pMBufXML,"<UnsignedProperties>\n<UnsignedSignatureProperties>", -1);
#ifdef WITH_TS
  } else if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)) {
    ddocMemAppendData(pMBufXML, "<UnsignedProperties xmlns=\"", -1);
    ddocMemAppendData(pMBufXML,NAMESPACE_XADES_132,  -1);
    ddocMemAppendData(pMBufXML,"\">\n<UnsignedSignatureProperties>", -1);
#endif
  } else {
    ddocMemAppendData(pMBufXML,"<UnsignedProperties Target=\"", -1);
    ddocMemAppendData(pMBufXML,pSigInfo->szId, -1);
    ddocMemAppendData(pMBufXML,"\">\n<UnsignedSignatureProperties>", -1);
  }
#ifdef WITH_TS
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)) {
  err = ddocSigInfo_SignatureTimeStamp_toXML(pSigInfo, &mbuf1);
  if(!err) {
    ddocMemAppendData(pMBufXML,(const char*)mbuf1.pMem, -1);
    ddocMemAppendData(pMBufXML,"\n", -1);
  }
  ddocMemBuf_free(&mbuf1);
  }
#endif
  // <CompleteCertificateRefs>
  err = ddocCompleteCertificateRefs_toXML((SignedDoc*)pSigDoc, (SignatureInfo*)pSigInfo, &mbuf1);
  ddocMemAppendData(pMBufXML,(const char*)mbuf1.pMem, -1);
  ddocMemBuf_free(&mbuf1);
  // <CompleteRevocationRefs>
  err = ddocCompleteRevocationRefs_toXML((SignedDoc*)pSigDoc, (SignatureInfo*)pSigInfo, &mbuf1);
  ddocMemAppendData(pMBufXML,(const char*)mbuf1.pMem, -1);
  ddocMemBuf_free(&mbuf1);

#ifdef WITH_TS
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)) {
  err = ddocSigInfo_SigAndRefsTimeStamp_toXML(pSigInfo, &mbuf1);
  if(!err) {
    ddocMemAppendData(pMBufXML,(const char*)mbuf1.pMem, -1);
    ddocMemAppendData(pMBufXML,"\n", -1);
  }
  ddocMemBuf_free(&mbuf1);
  }
#endif
  // responder cert
  ddocMemAppendData(pMBufXML,"<CertificateValues>\n", -1);
  // TODO format cert without header
  for(i = 0; i < ddocCertValueList_GetCertValuesCount(pSigInfo->pCertValues); i++) {
    pCertValue = ddocCertValueList_GetCertValue(pSigInfo->pCertValues, i);
    if(pCertValue && pCertValue->nType != CERTID_VALUE_SIGNERS_CERT) {
      ddocCertValue_toXML(pCertValue, &mbuf1);
      ddocMemAppendData(pMBufXML, (const char*)mbuf1.pMem, -1);
      ddocMemAppendData(pMBufXML, "\n", -1); 
      ddocMemBuf_free(&mbuf1);
    }
  }
  ddocMemAppendData(pMBufXML, "</CertificateValues>\n", -1);
  // revocation values
  ddocMemAppendData(pMBufXML, "<RevocationValues>", -1);
  // Ver 1.76 - in format 1.3 we use here additionally element <OCSPValues>
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER)
#ifdef WITH_TS
     || !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)
#endif
)
    ddocMemAppendData(pMBufXML, "<OCSPValues>", -1);
  // OCSP response
  ddocMemAppendData(pMBufXML, "<EncapsulatedOCSPValue Id=\"", -1);
  ddocMemAppendData(pMBufXML, pNotInfo->szId, -1);
  ddocMemAppendData(pMBufXML, "\">\n", -1);	
  pMBuf = ddocNotInfo_GetOCSPResponse(pNotInfo);
  RETURN_IF_NULL(pMBuf);
  // fix for backward compatibility with 2.1.5
  // remove trailing \n after base64 content
  if(mbuf1.pMem && ((char*)mbuf1.pMem)[strlen((const char*)mbuf1.pMem)-1] == '\n')
    ((char*)mbuf1.pMem)[strlen((const char*)mbuf1.pMem)-1] = 0;
  ddocEncodeBase64(pMBuf, &mbuf1);
  ddocMemAppendData(pMBufXML, (const char*)mbuf1.pMem, -1);
  ddocMemBuf_free(&mbuf1);
  ddocMemAppendData(pMBufXML, "</EncapsulatedOCSPValue>\n", -1);
  // Ver 1.76 - in format 1.3 we use here additionally element <OCSPValues>
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER)
#ifdef WITH_TS
     || !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)
#endif
)
    ddocMemAppendData(pMBufXML,"</OCSPValues>", -1);
  ddocMemAppendData(pMBufXML,"</RevocationValues>", -1);
  // unsigned prop end
  ddocMemAppendData(pMBufXML, "</UnsignedSignatureProperties>\n</UnsignedProperties>", -1);
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}

//============================================================
// Swaps bytes in this byte array. Length must be an even number
// used for big-endian <--> small-endian conversion
//============================================================
void swapBytes(byte* src, int len)
{
  byte b;
  int i;

  if(len % 2 == 0) {
    for(i = 0; i < (len / 2); i++) {
      b = src[i];
      src[i] = src[len - i - 1];
      src[len - i - 1] = b;
    }
  }
}


//============================================================
// Adds a signature to signed document file
// hFile - output file
// pSigInfo - signature info 
// cert - signers certificate
// returns error code
//============================================================
int addSignatureInfoXML(DigiDocMemBuf *pMBufXML, SignedDoc* pSigDoc, SignatureInfo* pSigInfo)
{
  int err = ERR_OK;
  unsigned char buf2[1024], *buf1 = 0;
  int len2, len1;
  EVP_PKEY* pubKey;
  SignatureValue *pSigVal;
  DigiDocMemBuf mbuf1, *pMBuf2 = 0;

  RETURN_IF_NULL_PARAM(pMBufXML);
  RETURN_IF_NULL_PARAM(pSigDoc);
  RETURN_IF_NULL_PARAM(pSigInfo);
  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  //TODO: xmlns = http://uri.etsi.org/01903/v1.3.2#
  ddocMemAppendData(pMBufXML,"<Signature Id=\"", -1);
  ddocMemAppendData(pMBufXML,pSigInfo->szId, -1);
  ddocMemAppendData(pMBufXML,"\" xmlns=\"http://www.w3.org/2000/09/xmldsig#\">\n", -1);
  buf1 = (unsigned char*)createXMLSignedInfo(pSigDoc, pSigInfo);
  RETURN_IF_NULL(buf1);
  ddocMemAppendData(pMBufXML,(const char*)buf1, -1);
  free(buf1);
  len1 = 512;
  buf1 = (unsigned char*)malloc(len1);
  //RETURN_IF_BAD_ALLOC(buf1);
  memset(buf1, 0, len1);
  // <SignatureValue>
  pSigVal = ddocSigInfo_GetSignatureValue(pSigInfo);
  //RETURN_IF_NULL(pSigVal);
  ddocSignatureValue_toXML(pSigVal, &mbuf1);
  ddocMemAppendData(pMBufXML,(const char*)mbuf1.pMem, -1);
  ddocMemBuf_free(&mbuf1);
  // cert data...
  if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER)) {
    ddocMemAppendData(pMBufXML,"<KeyInfo><X509Data><X509Certificate Id=\"", -1);
    ddocMemAppendData(pMBufXML,pSigInfo->szId, -1);
    ddocMemAppendData(pMBufXML,"-CERT\">\n", -1);	
  } else {
    // RSA KEY value
    ddocMemAppendData(pMBufXML,"<KeyInfo>\n", -1);
    ddocMemAppendData(pMBufXML,"<KeyValue>\n<RSAKeyValue>\n", -1);
    err = GetPublicKey(&pubKey, ddocSigInfo_GetSignersCert(pSigInfo));
    // FIXME
    // modulus
    len1 = BN_bn2bin(pubKey->pkey.rsa->n, buf1);
    // in version 1.1 we output modulus as it is
    // starting from 1.2 we convert it to big-endian
    /*len2 = sizeof(buf2);
      memset(buf2, 0, len2);
      encode(buf1, len1, buf2, &len2);
      printf("Old modulus: %s\n", buf2);
      if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_1_VER)) {
      swapBytes((byte*)buf1, len1);
      }*/
    len2 = sizeof(buf2);
    memset(buf2, 0, len2);
    encode(buf1, len1, buf2, &len2);
    //printf("New modulus: %s len: %d\n", buf2, len1);
    ddocMemAppendData(pMBufXML, "<Modulus>", -1);
    ddocMemAppendData(pMBufXML,buf2, -1);
    ddocMemAppendData(pMBufXML,"</Modulus>\n", -1);
    // exponent
    memset(buf1, 0, len1);
    len1 = BN_bn2bin(pubKey->pkey.rsa->e, buf1);
    len2 = sizeof(buf2);
    memset(buf2, 0, len2);
    encode(buf1, len1, buf2, &len2);
    ddocMemAppendData(pMBufXML,"<Exponent>", -1);
    ddocMemAppendData(pMBufXML,buf2, -1);
    ddocMemAppendData(pMBufXML,"</Exponent>\n", -1);
    EVP_PKEY_free(pubKey);
    ddocMemAppendData(pMBufXML,"</RSAKeyValue>\n</KeyValue>\n", -1);
    // cert data
    ddocMemAppendData(pMBufXML,"<X509Data><X509Certificate>\n", -1);
  }
  free(buf1);
  
  err = getCertPEM(ddocSigInfo_GetSignersCert(pSigInfo), 0, (char**)&buf1);
  RETURN_IF_NULL(buf1);
  ddocMemAppendData(pMBufXML,buf1, -1);
  free(buf1);
  ddocMemAppendData(pMBufXML,"</X509Certificate></X509Data></KeyInfo>\n", -1);
  // VS in releases prior to 1.76 we had incorrect <QualifyingProperties> atributes
#ifdef WITH_TS
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)) {
    ddocMemAppendData(pMBufXML,"<Object><QualifyingProperties xmlns=\"http://uri.etsi.org/01903/v1.3.2#\" Target=\"#", -1);
    ddocMemAppendData(pMBufXML,pSigInfo->szId, -1);
    ddocMemAppendData(pMBufXML,"\">\n", -1);
    ddocMemAppendData(pMBufXML,(const char*)buf2, -1);
  } else
#endif
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER)) {
    ddocMemAppendData(pMBufXML,"<Object><QualifyingProperties xmlns=\"http://uri.etsi.org/01903/v1.1.1#\" Target=\"#", -1);
    ddocMemAppendData(pMBufXML,pSigInfo->szId, -1);
    ddocMemAppendData(pMBufXML,"\">\n", -1);
    ddocMemAppendData(pMBufXML,(const char*)buf2, -1);
  } else {
    ddocMemAppendData(pMBufXML,"<Object><QualifyingProperties>\n", -1);
  }
  // signed properties	
  buf1 = (unsigned char*)createXMLSignedProperties(pSigDoc, pSigInfo, 1);
  RETURN_IF_NULL(buf1);
  ddocMemAppendData(pMBufXML,(const char*)buf1, -1);
  free(buf1);
  // unsigned properties
  if(pSigInfo->pNotary)
    addNotaryInfoXML(pMBufXML, pSigDoc, pSigInfo, pSigInfo->pNotary);
  // qualifying properties end
  ddocMemAppendData(pMBufXML,"</QualifyingProperties></Object>\n", -1);
  // signature end
  ddocMemAppendData(pMBufXML,"</Signature>\n", -1);
  if (err != ERR_OK) SET_LAST_ERROR(err);	
  ddocMemBuf_free(&mbuf1);
  return err;
}


//============================================================
// Canonicalizes PCDATA text value. Simple caninicalization
// only that replaces "\n\r" with "\n", by removing the "\r"
// and shifting the data left. Modification is done in place.
// src - input data. will be modified
// return error code or ERR_OK
//============================================================
int ddocCanonicalizePCDATA(char * src)
{
	int i, j;

	RETURN_IF_NULL_PARAM(src);
	for(i = j = 0; src[j]; ) {
		if(src[j] == '\r') {
			j++;
		} else {
			if(i != j)
				src[i] = src[j];
			i++;
			j++;
		}
	}
	src[i] = 0;
	return ERR_OK;
}

//============================================================
// Generates DataFile elements XML form and stores it in a file
// pSigDoc - signed document
// pDataFile - data file object to be converted
// szDataFile - input file name
// hFile - output file handle
// pMBufDigest - pointer to buffer for digest if we only want the digest
// pMBufXML - output buffer if we want data to be returned in mem buf
//============================================================
int generateDataFileXML(SignedDoc* pSigDoc, DataFile* pDataFile, 
				const char* szDataFile, FILE* hFile, DigiDocMemBuf* pMBufXML)
{
  int err = ERR_OK, len1, len2, j, k;
  char buf1[2048], buf2[4096], fixedFileName[200], *p, *p2;
  char *name, *value, *fName;
  FILE *fIn = 0;
  EVP_ENCODE_CTX ectx;
  SHA_CTX sctx;
  DigiDocMemBuf mbuf1, mbuf2, mbuf3;

  
  RETURN_IF_NULL_PARAM(pSigDoc);
  RETURN_IF_NULL_PARAM(pDataFile);
  RETURN_IF_NULL_PARAM(szDataFile);
  
  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  mbuf2.pMem = 0;
  mbuf2.nLen = 0;
  len1 = sizeof(buf1);
  // replaces '&' with '&amp;'
  memset(fixedFileName, 0, sizeof(fixedFileName));
  fName = (char*)getSimpleFileName(pDataFile->szFileName);
  for(k = j = 0; k < (int)strlen(fName); k++) {
    if(fName[k] == '&') {
      strncat(fixedFileName, "&amp;", sizeof(fixedFileName) - strlen(fixedFileName));
      j += 5;
    } else {
      fixedFileName[j] = fName[k];
      j++;
    }
  }
  //in versions 1.0, 1.1 and 1.2 we used bad encoding 
  if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER) ||
	!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_1_VER) ||
	!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_2_VER)) {
#ifdef WIN32
	utf82oem(fixedFileName, buf1, sizeof(buf1));
	len2 = sizeof(buf2);
	ascii2utf8(buf1, buf2, &len2);
#else
	convFNameToWin(fixedFileName, buf2, sizeof(buf2));
#endif
	strncpy(fixedFileName, buf2, sizeof(fixedFileName));
  } 
  // in version 1.0 we use DigestType and DigestValue attributes
  ddocMemSetLength(&mbuf2, 300);
  if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER) ||
     !strcmp(pDataFile->szContentType, CONTENT_HASHCODE)) {
    ddocEncodeBase64(ddocDataFile_GetDigestValue(pDataFile), &mbuf1);
    snprintf((char*)mbuf2.pMem, mbuf2.nLen, 
		"<DataFile ContentType=\"%s\" Filename=\"%s\" Id=\"%s\" MimeType=\"%s\" Size=\"%ld\" DigestType=\"%s\" DigestValue=\"%s\"", 
		   pDataFile->szContentType, 			 
	       fixedFileName, pDataFile->szId, pDataFile->szMimeType, 
	       pDataFile->nSize, pDataFile->szDigestType, (char*)mbuf1.pMem);
    ddocMemBuf_free(&mbuf1);
  } else { // in version 1.2 we don't use those attributes except for detached files
    if(!strcmp(pDataFile->szContentType, CONTENT_DETATCHED)) {
      ddocEncodeBase64(ddocDataFile_GetDetachedDigestValue(pDataFile), &mbuf1);
      snprintf((char*)mbuf2.pMem, mbuf2.nLen, 
		  "<DataFile ContentType=\"%s\" Filename=\"%s\" Id=\"%s\" MimeType=\"%s\" Size=\"%ld\" DigestType=\"%s\" DigestValue=\"%s\"", 
		 pDataFile->szContentType,
		 fixedFileName, pDataFile->szId, pDataFile->szMimeType, pDataFile->nSize,
		 pDataFile->szDigestType, (char*)mbuf1.pMem);
      ddocMemBuf_free(&mbuf1);
    } else
      snprintf((char*)mbuf2.pMem, mbuf2.nLen, "<DataFile ContentType=\"%s\" Filename=\"%s\" Id=\"%s\" MimeType=\"%s\" Size=\"%ld\"", 
	     pDataFile->szContentType,
		 fixedFileName, pDataFile->szId, pDataFile->szMimeType, pDataFile->nSize);
  }
  mbuf2.nLen = strlen((const char*)mbuf2.pMem);
  k = getCountOfDataFileAttributes(pDataFile);
  for(j = 0; j < k; j++) {
    getDataFileAttribute(pDataFile, j, &name, &value);
    escapeXMLSymbols(value, -1, &p);
	ddocMemAppendData(&mbuf2, " ", -1);
	ddocMemAppendData(&mbuf2, name, -1);
	ddocMemAppendData(&mbuf2, "=\"", -1);
	ddocMemAppendData(&mbuf2, p, -1);
	ddocMemAppendData(&mbuf2, "\"", -1);
    free(p);
  }
  // VS - ver 1.80 - in format 1.3 we started using SignedDoc schema
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER))
    ddocMemAppendData(&mbuf2, " xmlns=\"http://www.sk.ee/DigiDoc/v1.3.0#\"", -1);
#ifdef WITH_TS
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER))
    ddocMemAppendData(&mbuf2, " xmlns=\"http://www.sk.ee/DigiDoc/v1.4.0#\"", -1);
#endif
  ddocMemAppendData(&mbuf2, ">", -1); // end of generating <DataFile> header
  if(hFile)
    fputs((const char*)mbuf2.pMem, hFile);
#ifdef WITH_BASE64_HASHING_HACK
  SHA1_Init(&sctx);
  if(!strcmp(pDataFile->szContentType, CONTENT_EMBEDDED_BASE64)) {
	ddocMemAppendData(&mbuf2, "</DataFile>", -1);
	p = canonicalizeXML((char*)mbuf2.pMem, mbuf2.nLen);
	RETURN_IF_NULL(p);
	p[strlen(p)-11] = 0;
	ddocDebug(4, "generateDataFileXML", "sha1 initial update: \'%s\'", p);
	mbuf3.pMem = p;
	mbuf3.nLen = strlen(p);
	ddocDebugWriteFile(4, "df-data0.txt", &mbuf3);
	SHA1_Update(&sctx, (const char*)p, strlen(p));
	free(p); p = 0;
  }
#endif
  if(pMBufXML) { // in memory we use only for composing hashcode DataFile header for service
    // we still have to calculate hash code ower the original DataFile header
    p = strstr((const char*)mbuf2.pMem, "ContentType");
    if(!p) return ERR_FILE_READ;
    p += 13;
    *p = 0;
    ddocMemAppendData(pMBufXML, (const char*)mbuf2.pMem, -1);
    ddocMemAppendData(pMBufXML, CONTENT_HASHCODE, -1);
    p++;
    p = strchr((const char*)p, '\"');
    if(p) {
	p2 = strchr((const char*)p, '>');
	if(p2) {
	  *p2 = 0;
	  ddocMemAppendData(pMBufXML, (const char*)p, -1);
	  // add digest
	  ddocMemAppendData(pMBufXML, " DigestType=\"sha1\" DigestValue=\"", -1);
	  // now calculate total DataFile value...
	}
    }
  }
// in base64 hashing hack mode we don't keep DF content constantly in memory
#ifdef WITH_BASE64_HASHING_HACK
   if(!strcmp(pDataFile->szContentType, CONTENT_EMBEDDED_BASE64))
  	ddocMemBuf_free(&mbuf2); 
#endif
  err = ddocConvertFileName(fixedFileName, sizeof(fixedFileName), pDataFile->szFileName);
  if(err) return err;
  // if this is our temp file not a real input file
  // then don't change anything in it.
  if(strcmp(fixedFileName, szDataFile) != 0) {
    if((fIn = fopen(szDataFile, "rb")) != NULL) {
      ddocDebug(4, "generateDataFileXML", "Opened FILE: %s", szDataFile);
      while((len1 = fread(buf1, 1, sizeof(buf1), fIn)) > 0) {
#ifdef WITH_BASE64_HASHING_HACK
	if(!strcmp(pDataFile->szContentType, CONTENT_EMBEDDED_BASE64)) {
	  ddocDebug(4, "generateDataFileXML", "sha1 update: \'%s\'", buf1);
	  SHA1_Update(&sctx, (const char*)buf1, len1);
	  mbuf3.pMem = buf1;
	  mbuf3.nLen = strlen(buf1);
	  ddocDebugWriteFile(4, "df-data0.txt", &mbuf3);
	} else {
#endif
	  ddocMemAppendData(&mbuf2, buf1, len1);
#ifdef WITH_BASE64_HASHING_HACK
	}
#endif
	if(hFile)
	  fwrite(buf1, sizeof(char), len1, hFile);
	 //if(pMBufXML)  // do nothing for Hashcode dataFile body
    	 //ddocMemAppendData(pMBufXML, buf1, len1);
      }
      fclose(fIn);
      ddocDebug(4, "generateDataFileXML", "Closed FILE: %s", szDataFile);
      fIn = 0;
    }
  } else {
    // if the file must be embedded
    if(!strcmp(pDataFile->szContentType, CONTENT_EMBEDDED_BASE64) ||
       !strcmp(pDataFile->szContentType, CONTENT_EMBEDDED)) {
	  if((fIn = fopen(fixedFileName, "rb")) != NULL) {
          ddocDebug(4, "generateDataFileXML", "Opened FILE: %s", fixedFileName);
	// if encoded
	if(!strcmp(pDataFile->szContentType, CONTENT_EMBEDDED_BASE64)) {
	  EVP_EncodeInit(&ectx);
	  while((len1 = fread(buf1, 1, sizeof(buf1), fIn)) > 0) {
	    len2 = sizeof(buf2);
	    EVP_EncodeUpdate(&ectx, (unsigned char*)buf2, &len2, (unsigned char*)buf1, len1);
	    buf2[len2] = 0;
#ifdef WITH_BASE64_HASHING_HACK
		ddocCanonicalizePCDATA(buf2);
		len2 = strlen(buf2);
		ddocDebug(4, "generateDataFileXML", "sha1 update: \'%s\'", buf2);
		SHA1_Update(&sctx, (const char*)buf2, len2);
		mbuf3.pMem = buf2;
		mbuf3.nLen = len2;
		ddocDebugWriteFile(4, "df-data0.txt", &mbuf3);
#else
	    ddocMemAppendData(&mbuf2, buf2, len2);
#endif
		if(hFile)
			fwrite(buf2, sizeof(char), len2, hFile);
		//if(pMBufXML) // do nothing for Hashcode dataFile body
		//	ddocMemAppendData(pMBufXML, buf2, len2);
	  }
	  EVP_EncodeFinal(&ectx, (unsigned char*)buf2, &len2);
	  buf2[len2] = 0;
#ifdef WITH_BASE64_HASHING_HACK
	  ddocCanonicalizePCDATA(buf2);
	  len2 = strlen(buf2);
	  ddocDebug(4, "generateDataFileXML", "sha1 update: \'%s\'", buf2);
	  SHA1_Update(&sctx, (const char*)buf2, len2);
	  mbuf3.pMem = buf2;
	  mbuf3.nLen = len2;
	  ddocDebugWriteFile(4, "df-data0.txt", &mbuf3);
#else
	  ddocMemAppendData(&mbuf2, buf2, len2);
#endif
	  if(hFile)
		fwrite(buf2, sizeof(char), len2, hFile);
	  //if(pMBufXML) // do nothing for Hashcode dataFile body
		//	ddocMemAppendData(pMBufXML, buf2, len2);
	} else
	  if(!strcmp(pDataFile->szContentType, CONTENT_EMBEDDED)) {
	    while((len1 = fread(buf1, 1, sizeof(buf1), fIn)) > 0) {
	      if(!strcmp(pDataFile->szCharset, CHARSET_UTF_8)) {
		ddocMemAppendData(&mbuf2, buf1, len1);
			if(hFile)
			fwrite(buf1, sizeof(char), len1, hFile);
			//if(pMBufXML)
			//ddocMemAppendData(pMBufXML, buf1, len1);
	      } else 
		if(!strcmp(pDataFile->szCharset, CHARSET_ISO_8859_1)) {
		  len2 = sizeof(buf2);
		  memset(buf2, 0, len2);
		  isolat1ToUTF8((unsigned char*)buf2, &len2, 
				(const unsigned char*)buf1, &len1);
		  ddocMemAppendData(&mbuf2, buf2, len2);
		  if(hFile)
			fwrite(buf2, sizeof(char), len2, hFile);
		  //if(pMBufXML)
		//	ddocMemAppendData(pMBufXML, buf2, len2);
		} else
		  SET_LAST_ERROR(ERR_UNSUPPORTED_CHARSET);
	    }
	    if(!strcmp(pDataFile->szCharset, CHARSET_UTF_8)) {
	      ddocMemAppendData(&mbuf2, buf1, len1);
		  if(hFile)
			fwrite(buf1, sizeof(char), len1, hFile);
		  //if(pMBufXML)
		//	ddocMemAppendData(pMBufXML, buf1, len1);
	    } else 
	      if(!strcmp(pDataFile->szCharset, CHARSET_ISO_8859_1)) {
		len2 = sizeof(buf2);
		memset(buf2, 0, len2);
		isolat1ToUTF8((unsigned char*)buf2, &len2, 
			      (const unsigned char*)buf1, &len1);
		ddocMemAppendData(&mbuf2, buf2, len2);
		if(hFile)
			fwrite(buf2, sizeof(char), len2, hFile);
		//if(pMBufXML)
		//	ddocMemAppendData(pMBufXML, buf2, len2);
	      } else
		SET_LAST_ERROR(ERR_UNSUPPORTED_CHARSET);			
	  }
	fclose(fIn);
        ddocDebug(4, "generateDataFileXML", "Closed FILE: %s", szDataFile);
        fIn = 0;
      } else
	err = ERR_FILE_READ;
    }
  } // not temp file
  // print suffix-whitespace
  //if(pDataFile->szDataSuffix)
  //	BIO_puts(bOutFile, pDataFile->szDataSuffix);
  // else for DETATCHED do nothing!
  setString(&(pDataFile->szDigestType), DIGEST_SHA1_NAME, -1);
#ifdef WITH_BASE64_HASHING_HACK
   if(!strcmp(pDataFile->szContentType, CONTENT_EMBEDDED_BASE64)) {
	ddocDebug(4, "generateDataFileXML", "sha1 update: \'%s\'", "</DataFile>");
	SHA1_Update(&sctx, "</DataFile>", 11);
	mbuf3.pMem = "</DataFile>";
	mbuf3.nLen = strlen("</DataFile>");
	ddocDebugWriteFile(4, "df-data0.txt", &mbuf3);
        memset(buf2, 0, sizeof(buf2));
        SHA1_Final((unsigned char*)buf2, &sctx);
        ddocDataFile_SetDigestValue(pDataFile, buf2, DIGEST_LEN);
        ddocEncodeBase64(ddocDataFile_GetDigestValue(pDataFile), &mbuf2);
	if(pMBufXML) {
	    if(((char*)mbuf2.pMem)[strlen((char*)mbuf2.pMem)-1] == '\n')
			((char*)mbuf2.pMem)[strlen((char*)mbuf2.pMem)-1] = 0;
		ddocMemAppendData(pMBufXML, (const char*)mbuf2.pMem, -1);
	}
        ddocDebug(4, "generateDataFileXML", "DF digest: %s", (char*)mbuf2.pMem);
        ddocMemBuf_free(&mbuf2);
   } else {
#endif
    ddocMemAppendData(&mbuf2, "</DataFile>", -1);
    memset(buf2, 0, sizeof(buf2));
    p = canonicalizeXML((char*)mbuf2.pMem, mbuf2.nLen);
    ddocMemBuf_free(&mbuf2);
    RETURN_IF_NULL(p);
    SHA1((const unsigned char*)p, strlen(p), (unsigned char*)buf2);
    ddocDebug(4, "generateDataFileXML", "CANONICAL XML: \'%s\'", p);
    free(p);
    ddocDataFile_SetDigestValue(pDataFile, buf2, DIGEST_LEN);
    ddocEncodeBase64(ddocDataFile_GetDigestValue(pDataFile), &mbuf2);
    if(pMBufXML) {
	if(((char*)mbuf2.pMem)[strlen((char*)mbuf2.pMem)-1] == '\n')
	  ((char*)mbuf2.pMem)[strlen((char*)mbuf2.pMem)-1] = 0;
	  ddocMemAppendData(pMBufXML, (const char*)mbuf2.pMem, -1);
	}
    ddocDebug(4, "generateDataFileXML", "DF digest: %s", (char*)mbuf2.pMem);
    ddocMemBuf_free(&mbuf2);
#ifdef WITH_BASE64_HASHING_HACK
   }
#endif
  if(hFile)
	  fputs("</DataFile>", hFile);
  if(pMBufXML) {
	ddocMemAppendData(pMBufXML, (const char*)"\">", -1);
	ddocMemAppendData(pMBufXML, "</DataFile>", -1);
  }
  if (err != ERR_OK) SET_LAST_ERROR(err);	
  return err;
}

#define DD_TEMP_FILE_MAX 200

//--------------------------------------------------
// Creates a new signed XML document
// pSigDoc - signed doc info
// szSigDocFile - output XML file name. If the file exists,
// pMBufXML - output buffer if required to pass back in memory
// then it will be used to read in embedded DataFile contents.
// returns error code or ERR_OK for success
//--------------------------------------------------
int createSignedXMLDoc(SignedDoc* pSigDoc, const char* szOldFile, const char* szSigDocFile, DigiDocMemBuf* pMBufXML)
{
  int err = ERR_OK, i, nFiles;
  FILE *hFile = 0;
  DataFile* pDf = NULL;
  char ** arrTempFiles = NULL;
  char buf1[200];
  DigiDocMemBuf mbuf1;
  
  RETURN_IF_NULL_PARAM(pSigDoc);
  RETURN_IF_NULL_PARAM(szSigDocFile || pMBufXML);
  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  buf1[0] = 0;
  nFiles = getCountOfDataFiles(pSigDoc);
  ddocDebug(3, "createSignedXMLDoc", "Old file: %s new file: %s mbuf: %s, DFs: %d", 
	    szOldFile, (szSigDocFile ? szSigDocFile : "NULL"), (pMBufXML ? "Y" : "N"), nFiles);
  // check if the file exists allready 
  // and extracts data files. They
  // will be used later to construct
  // a new document and then removed
  if(szOldFile && checkFileExists(szOldFile)) {
    arrTempFiles = (char**)malloc(nFiles * sizeof(void*));
    RETURN_IF_BAD_ALLOC(arrTempFiles);
    memset(arrTempFiles, 0, nFiles * sizeof(void*));
    for(i = 0; i < nFiles; i++) {
      pDf = getDataFile(pSigDoc, i);
      ddocDebug(3, "createSignedXMLDoc", "DataFile: %s - %s", pDf->szId, pDf->szFileName);
      arrTempFiles[i] = (char*)malloc(DD_TEMP_FILE_MAX); 
      arrTempFiles[i][0] = 0;
      // do not copy newly added files
      if(!strchr((const char*)pDf->szFileName, '/') &&
	 !strchr((const char*)pDf->szFileName, '\\')) {
	err = getTempFileName(arrTempFiles[i], DD_TEMP_FILE_MAX);
	// VS: test the new parser based on xmlReader interface
	//err = ddocXRdrCopyDataFile(pSigDoc, szOldFile, (const char*)arrTempFiles[i], pDf->szId, CHARSET_ISO_8859_1, CHARSET_ISO_8859_1);
	ddocDebug(3, "createSignedXMLDoc", "Store DataFile: %s to: %s", pDf->szId, (const char*)arrTempFiles[i]);
	err = ddocExtractDataFile(pSigDoc, szOldFile, (const char*)arrTempFiles[i], pDf->szId, "NO-CHANGE");
	  }
    }
  }
  if(szSigDocFile) {
	err = ddocConvertFileName(buf1, sizeof(buf1), szSigDocFile);
	if(err) return err;
  }
  // now create the new document
  ddocDebug(3, "createSignedXMLDoc", "Opening FILE: %s", buf1);
  if((szSigDocFile && (hFile = fopen(buf1, "wb")) != NULL) || pMBufXML) {
    if(szSigDocFile)
      fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", hFile);
    if(pMBufXML)
      ddocMemAppendData(pMBufXML, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", -1);
    // VS: ver 1.80 - in version 1.3 we started using SignedDoc namespace
#ifdef WITH_TS
    if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)) {
      if(szSigDocFile)
        fprintf(hFile, "<SignedDoc format=\"%s\" version=\"%s\" xmlns=\"http://www.sk.ee/DigiDoc/v1.4.0#\">\n", pSigDoc->szFormat, pSigDoc->szFormatVer);
      if(pMBufXML) {
        ddocMemAppendData(pMBufXML, "<SignedDoc format=\"", -1);
        ddocMemAppendData(pMBufXML, pSigDoc->szFormat, -1);
        ddocMemAppendData(pMBufXML, "\" version=\"", -1);
        ddocMemAppendData(pMBufXML, pSigDoc->szFormatVer, -1);
        ddocMemAppendData(pMBufXML, "\" xmlns=\"http://www.sk.ee/DigiDoc/v1.4.0#\">\n", -1);        
      }
    } else
#endif
    if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER)) {
      if(szSigDocFile)
        fprintf(hFile, "<SignedDoc format=\"%s\" version=\"%s\" xmlns=\"http://www.sk.ee/DigiDoc/v1.3.0#\">\n", pSigDoc->szFormat, pSigDoc->szFormatVer);
      if(pMBufXML) {
        ddocMemAppendData(pMBufXML, "<SignedDoc format=\"", -1);
        ddocMemAppendData(pMBufXML, pSigDoc->szFormat, -1);
        ddocMemAppendData(pMBufXML, "\" version=\"", -1);
        ddocMemAppendData(pMBufXML, pSigDoc->szFormatVer, -1);
        ddocMemAppendData(pMBufXML, "\" xmlns=\"http://www.sk.ee/DigiDoc/v1.3.0#\">\n", -1);        
      }
    } else {
      if(szSigDocFile)
        fprintf(hFile, "<SignedDoc format=\"%s\" version=\"%s\">\n", pSigDoc->szFormat, pSigDoc->szFormatVer);
      if(pMBufXML) {
        ddocMemAppendData(pMBufXML, "<SignedDoc format=\"", -1);
        ddocMemAppendData(pMBufXML, pSigDoc->szFormat, -1);
        ddocMemAppendData(pMBufXML, "\" version=\"", -1);
        ddocMemAppendData(pMBufXML, pSigDoc->szFormatVer, -1);
        ddocMemAppendData(pMBufXML, "\">\n", -1);        
      }
    }
    // DataFile objects
    for(i = 0; i < nFiles; i++) {
      pDf = getDataFile(pSigDoc, i);
      // if the file must be embedded
      if(arrTempFiles && arrTempFiles[i] && checkFileExists(arrTempFiles[i])) {
	ddocDebug(3, "createSignedXMLDoc", "Use temp file: %s", arrTempFiles[i]);
	err = generateDataFileXML(pSigDoc, pDf, (const char*)arrTempFiles[i], hFile, pMBufXML);
      } else {
	err = ddocConvertFileName(buf1, sizeof(buf1), pDf->szFileName);
	if(err) {
	  if(hFile) fclose(hFile);
	  return err;
	}
	ddocDebug(3, "createSignedXMLDoc", "Create new data file: %s", buf1);
	err = generateDataFileXML(pSigDoc, pDf, (const char*)buf1, hFile, pMBufXML);
      }
      if(szSigDocFile)
        fputs("\n", hFile);
      if(pMBufXML)
        ddocMemAppendData(pMBufXML, "\n", -1);
    }
    for(i = 0; i < pSigDoc->nSignatures; i++) {
	// VS: deny writing orphoned signatures !
	/*	if(pSigDoc->pSignatures[i]->pSigValue && 
			(pSigDoc->pSignatures[i]->pSigValue->mbufSignatureValue.pMem ||
			 pSigDoc->pSignatures[i]->mbufOrigContent.pMem))  {*/
      // VS: if Signature has been read from file then 
      // use the original content
      if(pSigDoc->pSignatures[i]->mbufOrigContent.pMem) {
        if(szSigDocFile)
		fwrite(pSigDoc->pSignatures[i]->mbufOrigContent.pMem, sizeof(char),
		  			pSigDoc->pSignatures[i]->mbufOrigContent.nLen, hFile);
		if(pMBufXML)
        	err = ddocMemAppendData(pMBufXML, (const char*)pSigDoc->pSignatures[i]->mbufOrigContent.pMem, 
        		pSigDoc->pSignatures[i]->mbufOrigContent.nLen);
      } else {
      	err = addSignatureInfoXML(&mbuf1, pSigDoc, pSigDoc->pSignatures[i]);
		if(szSigDocFile)
			fputs((char*)mbuf1.pMem, hFile);
		if(pMBufXML)
        	err = ddocMemAppendData(pMBufXML, (const char*)mbuf1.pMem, mbuf1.nLen);
		ddocMemBuf_free(&mbuf1);
	 }
	//	}
    } // for i < pSigDoc->nSignatures			
    if(szSigDocFile) {
        fputs("</SignedDoc>", hFile);
    }
    if(hFile) {
	ddocDebug(3, "createSignedXMLDoc", "Closing FILE: %s", buf1);
	fclose(hFile);
    }
    if(pMBufXML)
        ddocMemAppendData(pMBufXML, "</SignedDoc>", -1);
    // delete temporary files we created when
    // extracting the data files from original XML signed doc
    // VS: fix the bug of deleting input files
    // This happened because prefix was empty and
    // thus doc names where the same as original
    // input file names
    if(szOldFile && arrTempFiles) { // check if temp files were created
      for(i = 0; i < nFiles; i++) {
	pDf = getDataFile(pSigDoc, i);
	// ignore not being able to delete temp file. It returns -1
	_unlink((const char*)arrTempFiles[i]);
	free(arrTempFiles[i]);
      }
      free(arrTempFiles);
    }
  } else
    err = ERR_FILE_WRITE;
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}


//--------------------------------------------------
// Creates a new signed document
// pSigDoc - signed doc info
// returns error code or ERR_OK for success
//--------------------------------------------------
EXP_OPTION int createSignedDoc(SignedDoc* pSigDoc, const char* szOldFile, const char* szOutputFile)
{
  int err = ERR_OK;

  RETURN_IF_NULL_PARAM(pSigDoc);
  RETURN_IF_NULL_PARAM(szOutputFile);
  RETURN_IF_NULL_PARAM(pSigDoc->szFormat);
	
  clearErrors();	
  if(!strcmp(pSigDoc->szFormat, SK_XML_1_NAME) ||
     !strcmp(pSigDoc->szFormat, DIGIDOC_XML_1_1_NAME)) { 
    err = createSignedXMLDoc(pSigDoc, szOldFile, szOutputFile, NULL);
  }
  else
    err = ERR_UNSUPPORTED_FORMAT;
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}

//--------------------------------------------------
// Creates a new signed document in memory buffer
// pSigDoc - signed doc info
// szOldFile - name of old file on disk to copy DataFile contents
// pMBuf - buffer for new digidoc document
// returns error code or ERR_OK for success
//--------------------------------------------------
EXP_OPTION int createSignedDocInMemory(SignedDoc* pSigDoc, const char* szOldFile, DigiDocMemBuf* pMBuf)
{
  int err = ERR_OK;

  RETURN_IF_NULL_PARAM(pSigDoc);
  RETURN_IF_NULL_PARAM(pMBuf);
  RETURN_IF_NULL_PARAM(pSigDoc->szFormat);
  clearErrors();	
  if(!strcmp(pSigDoc->szFormat, SK_XML_1_NAME) ||
     !strcmp(pSigDoc->szFormat, DIGIDOC_XML_1_1_NAME)) { 
    err = createSignedXMLDoc(pSigDoc, szOldFile, NULL, pMBuf);
  }
  else
    err = ERR_UNSUPPORTED_FORMAT;
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}


//--------------------------------------------------
// Removes incomplete or orphoned signatures.
// Signature is incomplete if it hasn't got the signature
// value
// pSigDoc - signed doc info
// returns error code or ERR_OK for success
//--------------------------------------------------
EXP_OPTION int removeIncompleteSignatures(SignedDoc* pSigDoc)
{
	int err = ERR_OK, i, n, b;
	SignatureInfo *pSigInfo;

	RETURN_IF_NULL_PARAM(pSigDoc);
	do {
		b = 0;
		n = getCountOfSignatures(pSigDoc);
		for(i = 0; i < n; i++){
			pSigInfo = getSignature(pSigDoc, i);
			if(!pSigInfo->pSigValue ||
				(pSigInfo->pSigValue && !pSigInfo->pSigValue->mbufSignatureValue.pMem)) {
				SignatureInfo_free(pSigInfo); // remove incomplete signature
				break;
			}
		}
	} while(b);
	return err;
}

//--------------------------------------------------
// Checks for incomplete or orphoned signatures.
// Signature is incomplete if it hasn't got the signature
// value
// pSigDoc - signed doc info
// returns error code if DigiDoc has orphoned signature or ERR_OK for success
//--------------------------------------------------
EXP_OPTION int hasIncompleteSignatures(SignedDoc* pSigDoc)
{
	int i;
	SignatureInfo *pSigInfo;

	RETURN_IF_NULL_PARAM(pSigDoc);
	for(i = 0; i < getCountOfSignatures(pSigDoc); i++){
		pSigInfo = getSignature(pSigDoc, i);
		if(!pSigInfo->pSigValue ||
			(pSigInfo->pSigValue && !pSigInfo->pSigValue->mbufSignatureValue.pMem)) {
			return ERR_ORPHONED_SIGNATURE;
		}
	}
	return ERR_OK;
}

