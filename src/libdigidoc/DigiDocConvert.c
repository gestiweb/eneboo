//==================================================
// FILE:	DigiDocEnc.c
// PROJECT:     Digi Doc Encryption
// DESCRIPTION: DigiDoc character conversion routines
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
//      22.09.2004      Veiko Sinivee
//                      Creation
//==================================================

#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocConvert.h>
#include <libdigidoc/DigiDocError.h>
#include <libdigidoc/DigiDocMem.h>

#ifdef WIN32
  #include <windows.h>
#endif
#include <libxml/globals.h>
#include <libxml/xmlerror.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h> /* only for xmlNewInputFromFile() */
#include <libxml/tree.h>
#include <libxml/debugXML.h>
#include <libxml/xmlmemory.h>
#include <libxml/c14n.h>
#include <ctype.h>
#include <memory.h>



//==========< general fucntions >============

//--------------------------------------------------
// Helper function that converts ISO Latin1 to UTF8
// ascii - input data in ISO Latin1
// utf8out - output buffer for UTF8 text
// outlen - output buffer length
// returns output buffer
//--------------------------------------------------
EXP_OPTION char* ascii2utf8(const char* ascii, char* utf8out, int* outlen)
{
	int inlen = strlen(ascii);
	memset(utf8out, 0, *outlen);
	isolat1ToUTF8((unsigned char*)utf8out, outlen, (const unsigned char*)ascii, &inlen);
	return utf8out;
}

//--------------------------------------------------
// Helper function that converts UTF8 to ISO Latin1
// utf8in - input data in UTF8
// asciiout - output buffer for ISO Latin1 text
// outlen - output buffer length
// returns output buffer
//--------------------------------------------------
EXP_OPTION char* utf82ascii(const char* utf8in, char* asciiout, int* outlen)
{
	int inlen = strlen(utf8in);
	memset(asciiout, 0, *outlen);
	UTF8Toisolat1((unsigned char*)asciiout, outlen, (const unsigned char*)utf8in, &inlen);
	return asciiout;
}

int hex2char(char c) 
{
  static char unidigits[] = "0123456789ABCDEF";
  unsigned int i, k = 0;
  for(i = 0; i < sizeof(unidigits); i++) {
    if(unidigits[i] == c) {
      k = i;
      break;
    }
  }
  return k;
}


struct u2w {
  long uni;
  unsigned char cp1252; // cp 1252 charset
  unsigned char cp1257; // baltic charset
  unsigned char utf8_1; // UTF8 1. byte
  unsigned char utf8_2; // UTF8 2. byte
};

struct u2w uni2cp1252 [] = {
  { 8364, 128, 0, 0, 0 }, // euro sign  
  { 8218, 130, 0, 0, 0 },  // SINGLE LOW-9 QUOTATION MARK
  { 402, 131, 0, 0, 0 },   // LATIN SMALL LETTER F WITH HOOK
  { 8222, 132, 0, 0, 0 },   // DOUBLE LOW-9 QUOTATION MARK
  { 8230, 133, 0, 0, 0 },   // HORIZONTAL ELLIPSIS
  { 8224, 134, 0, 0, 0 },   // DAGGER
  { 8225, 135, 0, 0, 0 },   // DOUBLE DAGGER
  { 710, 136, 0, 0, 0 },   // MODIFIER LETTER CIRCUMFLEX ACCENT
  { 8240, 137, 0, 0, 0 },   // PER MILLE SIGN
  { 352, 138, 208, 197, 160 },   // LATIN CAPITAL LETTER S WITH CARON
  { 8249, 139, 0, 0, 0 },   // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
  { 338, 140, 0, 0, 0 },   // LATIN CAPITAL LIGATURE OE
  { 381, 142, 222, 197, 189 },   // LATIN CAPITAL LETTER Z WITH CARON
  { 8216, 145, 0, 0, 0 },   // LEFT SINGLE QUOTATION MARK
  { 8217, 146, 0, 0, 0 },   // RIGHT SINGLE QUOTATION MARK
  { 8220, 147, 0, 0, 0 },   // LEFT DOUBLE QUOTATION MARK
  { 8221, 148, 0, 0, 0 },   // RIGHT DOUBLE QUOTATION MARK
  { 8226, 149, 0, 0, 0 },   // BULLET
  { 8211, 150, 0, 0, 0 },   // EN DASH
  { 8212, 151, 0, 0, 0 },   // EM DASH
  { 732, 152, 0, 0, 0 },   // SMALL TILDE
  { 8482, 153, 0, 0, 0 },   // TRADE MARK SIGN
  { 353, 154, 240, 197, 161 },   // LATIN SMALL LETTER S WITH CARON
  { 8250, 155, 0, 0, 0 },   // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
  { 339, 156, 0, 0, 0 },   // LATIN SMALL LIGATURE OE
  { 382, 158, 254, 197, 190 },   // LATIN SMALL LETTER Z WITH CARON
  { 376, 159, 0, 0, 0 },   // LATIN CAPITAL LETTER Y WITH DIAERESIS
  { 196, 196, 196, 195, 132},  // Capital A with umlaut
  { 214, 214, 214, 195, 150},  // Calital O with umlaut
  { 220, 220, 220, 195, 156},  // Capital U with umlaut
  { 213, 213, 213, 195, 149},  // Capital O with tilde
  { 228, 228, 228, 195, 164},  // small a with umlaut
  { 246, 246, 246, 195, 132},  // small o with umlaut
  { 245, 245, 245, 195, 181},  // small o with tilde
  { 252, 252, 252, 195, 188},  // small u with umlaut

  { 0, 0, 0, 0, 0 }    // table end marker
};

int isUmlaut(unsigned char c)
{
  int i;
  for(i = 0; uni2cp1252[i].uni; i++) {
    if(uni2cp1252[i].cp1252 == c) {
      return 1;
    }
  }
  return 0;
}

int hasUmlauts(const char* str)
{
  int i;
  for(i = 0; str[i]; i++) {
    if(isUmlaut((unsigned char)str[i]))
      return 1;
  }
  return 0;
}

int uni2char(long c) 
{
  int i, k = 0x20; // use space for all undisplayable unicode chars
  for(i = 0; uni2cp1252[i].uni; i++) {
    if(uni2cp1252[i].uni == c) {
      if(uni2cp1252[i].cp1257)
	k = uni2cp1252[i].cp1257;
      else
	k = uni2cp1252[i].cp1252;
      break;
    }
  }
  return k;
}

int uni2utf8(long c, int utfidx) 
{
  int i, k = 0x20; // use space for all undisplayable unicode chars
  for(i = 0; uni2cp1252[i].uni; i++) {
    if(uni2cp1252[i].uni == c) {
      if(uni2cp1252[i].utf8_1) {
	if(utfidx)
	  k = uni2cp1252[i].utf8_2;
	else
	  k = uni2cp1252[i].utf8_1;
      }
      else
	k = uni2cp1252[i].cp1252;
      break;
    }
  }
  return k;
}


EXP_OPTION void unicode2ascii(const char* uni, char* dest)
{
  int len, i, j, k;
  long c;
  
  RETURN_VOID_IF_NULL(uni);
  RETURN_VOID_IF_NULL(dest);

  len = strlen(uni);
  memset(dest, 0, len+1);
  for(i = j = 0; i < len; i++) {
    if(uni[i] == '\\' && uni[i+1] == 'x') {
      if(uni[i+2] == '0' && uni[i+3] == '0') { 
	if(uni[i+4] >= 'A' && uni[i+4] <= 'Z') { // simple char in form \x00A
	  dest[j] = (unsigned char)uni[i+4];
	  i += 4;
	  j++;
	  continue;
	} else {
	  i += 3;
	  continue;
	}
      }
      if(uni[i+2] == '0' && uni[i+3] != '0') { // more complex char like \x0}1
	c = 0;
	// first pos
	k = uni[i+2];
	k = (((k >= '0') && (k <= '9')) ? k - '0' : k);
	k *= 4096;
	c += k;
	// second pos
	k = uni[i+3];
	k = (((k >= '0') && (k <= '9')) ? k - '0' : k);
	k *= 256;
	c += k;
	// third pos
	k = uni[i+4];
	k = (((k >= '0') && (k <= '9')) ? k - '0' : k);
	c += k;
	if(c >= 0 && c <= 9) c+= '0';
	// get corresponding cp1252 char	  
	if(c > 256) {
#ifdef WIN32
	  dest[j] = (unsigned char)uni2char(c);
	  j++;
#else
	  dest[j] = uni2utf8(c, 0);
	  j++;
	  dest[j] = uni2utf8(c, 1);
	  j++;
#endif
	} else {
	  dest[j] = (unsigned char)c;
	  j++;
	}
	i += 4;
	continue;
      }
      if(uni[i+2] != '0' && uni[i+3] != '0') { // hex char code like \xC4
	c = hex2char(uni[i+2]) * 16 + hex2char(uni[i+3]);
	if(c > 256) {
#ifdef WIN32
	  dest[j] = (unsigned char)uni2char(c);
	  j++;
#else
	  dest[j] = uni2utf8(c, 0);
	  j++;
	  dest[j] = uni2utf8(c, 1);
	  j++;
#endif
	} else {
	  dest[j] = (unsigned char)c;
	  j++;
	}
	i += 3;	
	continue;
      }
    } else {
      dest[j] = uni[i];
      j++;
    }
  }
}

typedef struct xmlsym_st {
  char  sym;
  char* escape;
} xmlsym;

int g_xmlsyms = 5;
xmlsym g_xmlsym[] = {
  { '\n',  "&#xA;" },
  { '\r',  "&#xD;" },
  { '<',  "&lt;" },
  { '>',  "&gt;" },
  { '&',  "&amp;" }   // VS: test if really necessary!
};

int findEscape(const char* src, char* sym)
{
  int i;

  *sym = 0;
  for(i = 0; i < g_xmlsyms; i++) {
    if(!strncmp(g_xmlsym[i].escape, src, strlen(g_xmlsym[i].escape))) {
      *sym = g_xmlsym[i].sym;
      return strlen(g_xmlsym[i].escape);
    }
  }
  return 0;
}

//--------------------------------------------------
// Converts xml symbols to corresponding escapes
// src - string with xml special sybols
// returns string with esacpes
//--------------------------------------------------
char* escape2xmlsym(const char* src)
{
  char* dest = 0, c;
  int i, j, l, k;
  
  l = strlen(src);
  dest = (char*)malloc(l+1);
  SET_LAST_ERROR_RETURN_IF_NOT(dest, ERR_BAD_ALLOC, NULL); 
  memset(dest, 0, l+1);
  for(i = j = 0; i < l; i++, j++) {
    if(src[i] == '&') {
      k = findEscape(src + i, &c);
      if(k && c) {
	dest[j] = c;
	i += k - 1;
      }
      else
	dest[j] = src[i];
    }
    else
      dest[j] = src[i];
  }
  ddocDebug(4, "escape2xmlsym", "%s --> %s", src, dest);
  return dest;
}

char g_hexChars[] = "0123456789ABCDEF";

//--------------------------------------------------
// Decodes a single hex digit
// h - hex digit
// return binary value
//--------------------------------------------------
byte h2b(char h)
{
  int i;
  for(i = 0; i < 16; i++) {
    if(g_hexChars[i] == h || g_hexChars[i] == toupper(h)) 
      return (byte)i;
  }
  // if not found then return an error
  ddocDebug(1, "h2b", "Invalid hex byte: %c", h);
  SET_LAST_ERROR(ERR_BAD_PARAM);
  return 0; // MSTERN: incorrect, VS - yes return -1 if not found. This is an error code
}

//--------------------------------------------------
// Converts a hex number (string) to binary value
// hex - hex number
// bin - buffer for binary value
// len - buffer length
//--------------------------------------------------
EXP_OPTION void hex2bin(const char* hex, byte* bin, int* len)
{
  int i, l;
  byte b;

  RETURN_VOID_IF_NULL(hex);
  RETURN_VOID_IF_NULL(bin);
  RETURN_VOID_IF_NULL(len);
  memset(bin, 0, *len);
  l = strlen(hex);
  for(i = 0; i < l; i += 2) {
    b = (byte) ((h2b(hex[i]) << 4) | h2b(hex[i+1]));
    bin[i/2] = b;
  }
  *len = i / 2;
}

//--------------------------------------------------
// Converts a single byte to two hex characters
// b - binary value
// dest - destination buffer
//--------------------------------------------------
void b2h(byte b, char* dest)
{
  dest[0] = g_hexChars[(b & 0xF0) >> 4];
  dest[1] = g_hexChars[b & 0x0F];
}

//--------------------------------------------------
// Converts a binary value to hex string
// bin - binary value
// blen - binary value length
// hex - buffer for hex string
// len - buffer length
//--------------------------------------------------
EXP_OPTION void bin2hex(const byte* bin, int blen, char* hex, int* len)
{
  int i;
  byte b;

  RETURN_VOID_IF_NULL(hex);
  RETURN_VOID_IF_NULL(bin);
  RETURN_VOID_IF_NULL(len);
  memset(hex, 0, *len);
  for(i = 0; i < blen; i++) {
    b = bin[i];
    hex[i*2] = g_hexChars[(b & 0xF0) >> 4];
    hex[i*2+1] = g_hexChars[b & 0x0F];
  }
  *len = i / 2;
}

//--------------------------------------------------
// Converts correct filename to incorrect encoding
// used in formats 1.0, 1.1 and 1.2
// src - input data
// dest - buffer for converted data
// len - length of destination buffer
//--------------------------------------------------
EXP_OPTION void convFNameToWin(const char* src, char* dest, int len)
{
  int i, j;
  for(i = j = 0; src[i] && (j < len - 1); i++, j++) {
    if(((unsigned char)src[i]) == 197 && ((unsigned char)src[i+1]) == 161) {
      dest[j] = (unsigned char)195; dest[j+1] = (unsigned char)176; j++, i++; continue;
    } else if(((unsigned char)src[i]) == 197 && ((unsigned char)src[i+1]) == 160) {
      dest[j] = (unsigned char)195; dest[j+1] = (unsigned char)144; j++, i++; continue;
    } else if(((unsigned char)src[i]) == 197 && ((unsigned char)src[i+1]) == 190) {
      dest[j] = (unsigned char)195; dest[j+1] = (unsigned char)190; j++, i++; continue;
    } else if(((unsigned char)src[i]) == 197 && ((unsigned char)src[i+1]) == 189) {
      dest[j] = (unsigned char)195; dest[j+1] = (unsigned char)158; j++, i++; continue;
    } else {
      dest[j] = src[i];
    }
  }
  dest[j] = 0;
}

//--------------------------------------------------
// Converts bad UTF-8 filename used in formats 1.0, 
// 1.1 and 1.2 to correct encoding
// src - input data
// dest - buffer for converted data
// len - length of destination buffer
//--------------------------------------------------
EXP_OPTION void convWinToFName(const char* src, char* dest, int len)
{
  int i, j;
  for(i = j = 0; src[i] && (j < len - 1); i++, j++) {
    if(((unsigned char)src[i]) == 195 && ((unsigned char)src[i+1]) == 176) {
      dest[j] = (unsigned char)197; dest[j+1] = (unsigned char)161; j++, i++; continue;
    } else if(((unsigned char)src[i]) == 195 && ((unsigned char)src[i+1]) == 144) {
      dest[j] = (unsigned char)197; dest[j+1] = (unsigned char)160; j++, i++; continue;
    } else if(((unsigned char)src[i]) == 195 && ((unsigned char)src[i+1]) == 190) {
      dest[j] = (unsigned char)197; dest[j+1] = (unsigned char)190; j++, i++; continue;
    } else if(((unsigned char)src[i]) == 195 && ((unsigned char)src[i+1]) == 158) {
      dest[j] = (unsigned char)197; dest[j+1] = (unsigned char)189; j++, i++; continue;
    } else {
      dest[j] = src[i];
    }
  }
  dest[j] = 0;
}


//==========< only win32 fucntions >============

#ifdef WIN32

//--------------------------------------------------
// Converts input OEM charset data to 16 bit unicode.
// oem - 8 bit oem charset input data
// unicode - address of pointer for allocated unicode string. Caller must free() !
// outlen - address of length variable for unicode string
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int oem2unicode(const char* oem, char** unicode, int* outlen)
{
  int len;

  *unicode = 0;
  // convert oem to unicode
  if(!oem || !strlen(oem))
    return ERR_OK;
  ddocDebug(4, "oem2unicode", "Convert: \'%s\' len: %d", oem, strlen(oem));
  len = MultiByteToWideChar(CP_ACP, 0, oem, -1, NULL, 0) * 2 + 10;
  ddocDebug(5, "oem2unicode", "Alloc: %d", len);
  *unicode = (char*)malloc(len);
  RETURN_IF_BAD_ALLOC(*unicode);
  memset(*unicode, 0, len);
  *outlen = MultiByteToWideChar(CP_ACP, 0, oem, -1, (LPWSTR)(*unicode), len);
  if(!(*outlen)) {
    SET_LAST_ERROR(ERR_CHARSET_CONVERT);
    return ERR_CHARSET_CONVERT;
  }
  else
    return ERR_OK;
}

//--------------------------------------------------
// Converts input 16 bit unicode data to UTF8.
// unicode - 16 bit unicode input data
// utf8 - address of pointer for allocated utf8 string. Caller must free() !
// outlen - address of length variable for utf8 string
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int unicode2utf8(const char* unicode, char** utf8, int* outlen)
{
  int len;

  *utf8 = 0;
  // now convert unicode to UTF8
  if(!unicode || !strlen(unicode))
    return ERR_OK;
  len = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)unicode, -1, NULL, 0, NULL, NULL) + 10;
  ddocDebug(5, "unicode2utf8", "Alloc: %d", len);
  *utf8 = (char*)malloc(len);
  RETURN_IF_BAD_ALLOC(*utf8);
  memset(*utf8, 0, len);
  *outlen = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)unicode, -1, *utf8, len, NULL, NULL);
  ddocDebug(5, "unicode2utf8", "Convert: \'%s\' len: %d", *utf8, strlen(*utf8));
  if(!(*outlen)) {
    SET_LAST_ERROR(ERR_CHARSET_CONVERT);
    return ERR_CHARSET_CONVERT;
  }
  else
    return ERR_OK;
}

//--------------------------------------------------
// Converts input OEM charset data to UTF-8
// oem - 8 bit oem charset input data
// utf8 - address of buffer allocated utf8 string. 
// len - size of buffer
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int oem2utf8(const char* oem, char* utf8, int len)
{
  int err = ERR_OK, len1, len2;
  char *pTmp1 = 0, *pTmp2 = 0;

  // Kaido: 2.1.13 - initialize return value
  *utf8 = 0;
  // convert oem to unicode
  if(!oem || !strlen(oem))
    return ERR_OK;
  err = oem2unicode(oem, &pTmp1, &len1);
  if(err) return err;
  // now convert unicode to UTF8
  err = unicode2utf8(pTmp1, &pTmp2, &len2);
  if(len2 < len)
    memcpy(utf8, pTmp2, len2);
  else
    err = ERR_CHARSET_CONVERT;
  if(pTmp1)
    free(pTmp1);
  if(pTmp2)
    free(pTmp2);
  return err;
}

//--------------------------------------------------
// Converts input UTF-8 data to 16 bit unicode data.
// utf8 - UTF-8 input data
// unicode - address of pointer for allocated unicode string. Caller must free() !
// outlen - address of length variable for unicode string
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int utf82unicode(const char* utf8, char** unicode, int* outlen)
{
  int len;

  *unicode = 0;
  // convert unicode to UTF-8
  if(!utf8 || !strlen(utf8))
    return ERR_OK;
  ddocDebug(5, "utf82unicode", "Convert: \'%s\' len: %d", utf8, strlen(utf8));
  len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0) * 2 + 10;
  ddocDebug(5, "utf82unicode", "Alloc: %d", len);
  *unicode = (char*)malloc(len);
  RETURN_IF_BAD_ALLOC(*unicode);
  memset(*unicode, 0, len);
  *outlen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, (LPWSTR)(*unicode), len);
  if(!(*outlen)) {
    SET_LAST_ERROR(ERR_CHARSET_CONVERT);
    return ERR_CHARSET_CONVERT;
  }
  else
    return ERR_OK;
}

//--------------------------------------------------
// Converts input 16 bit unicode data to oem charset data.
// unicode - 16 bit unicode input data
// oem - address of pointer for allocated oem string. Caller must free() !
// outlen - address of length variable for oem string
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int unicode2oem(const char* unicode, char** oem, int* outlen)
{
  int len;
  
  *oem = 0;
  if(!unicode || !strlen(unicode))
    return ERR_OK;
  // now convert unicode to OEM
  len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)unicode, -1, NULL, 0, NULL, NULL) + 10;
  ddocDebug(5, "unicode2oem", "Alloc: %d", len);
  *oem = (char*)malloc(len);
  RETURN_IF_BAD_ALLOC(*oem);
  memset(*oem, 0, len);
  *outlen = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)unicode, -1, *oem, len, NULL, NULL);
  ddocDebug(5, "unicode2oem", "Convert: \'%s\' len: %d", *oem, strlen(*oem));
  if(!(*outlen)) {
    SET_LAST_ERROR(ERR_CHARSET_CONVERT);
    return ERR_CHARSET_CONVERT;
  }
  else
    return ERR_OK;
}


//--------------------------------------------------
// Converts input UTF-8 data to OEM charset data
// utf8 - UTF-8 input data
// oem - address of buffer for oem string. 
// len - size of buffer
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int utf82oem(const char* utf8, char* oem, int len)
{
  int err = ERR_OK, len1, len2;
  char *pTmp1 = 0, *pTmp2 = 0;

  // Kaido: 2.1.13 - initialize return value
  *oem = 0;
  // convert utf8 to unicode
  if(!utf8 || !strlen(utf8))
    return ERR_OK;
  err = utf82unicode(utf8, &pTmp1, &len1);
  if(err) return err;
  // now convert unicode to OEM
  err = unicode2oem(pTmp1, &pTmp2, &len2);
  if(len2 < len)
    memcpy(oem, pTmp2, len2);
  else
    err = ERR_CHARSET_CONVERT;
  if(pTmp1)
    free(pTmp1);
  if(pTmp2)
    free(pTmp2);
  return err;
}

//--------------------------------------------------
// Converts input UTF-8 data to OEM charset data
// pSigDoc - signed doc object
// pDf - data file obejct 
// outFileName - output buffer
// len - length of output buffer
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int getDataFileFileName(SignedDoc* pSigDoc, DataFile* pDf, char* outFileName, int len)
{
  int err = ERR_OK;

  RETURN_IF_NULL_PARAM(pSigDoc);
  RETURN_IF_NULL_PARAM(pDf);
  RETURN_IF_NULL_PARAM(outFileName);

  // Kaido: 2.1.13 - initialize return value
  *outFileName = 0;
  if(!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER)) {
    strncpy(outFileName, pDf->szFileName, len);
  } else {
    convWinToFName(pDf->szFileName, outFileName, len);
  }
  return err;
}


#endif


//--------------------------------------------------
// Converts input data to UTF-8
// src - input data
// returns converted string or NULL. Caller must free it.
//--------------------------------------------------
EXP_OPTION int ddocConvertInput(const char* src, char** dest)
{
  int l1, err = ERR_OK;

  *dest = 0;
  l1 = strlen(src) * 2 + 3;
  *dest = (char*)malloc(l1);
  if(*dest) {
    memset(*dest, 0, l1);
#ifdef WIN32
    RETURN_IF_NULL(*dest);
    err = oem2utf8(src, *dest, l1);
#else
    strncpy(*dest, src, l1);
#endif
  }
  return err;
}

//--------------------------------------------------
// Releases mem-block allocated by lib. In win32
// this must be done since the mem was allocated by dll
// and must also be released by dll that allocated it.
// p - mem to be freed
//--------------------------------------------------
EXP_OPTION void freeLibMem(void* p)
{
  if(p)
    free(p);
}


//--------------------------------------------------
// Converts a filename according to platform rules
// dest - destination buffer
// destlen - destination buffer length
// src - source filename
// returns error code or ERR_OK
//--------------------------------------------------
int ddocConvertFileName(char* dest, int destlen, const char* src)
{
  int err = ERR_OK;
  
  RETURN_IF_NULL_PARAM(dest);
  RETURN_IF_NULL_PARAM(src);

  *dest = 0; // initialize
#ifdef WIN32
  err = utf82oem(src, dest, destlen);
#else
  SET_LAST_ERROR_IF_NOT(strlen(src) < destlen, ERR_BUF_LEN);
  strncpy(dest, src, destlen);
#endif
  return err;
}

//==========< Base64 functions >================


//============================================================
// Encode a byte array in Base64 format.
// raw - input data
// rawlen - length of input data
// buf - destination buffer
// buflen - destination buffer length (will be modified by catual number of bytes)
//============================================================
EXP_OPTION void encode(const byte* raw, int rawlen, byte* buf, int* buflen) 
{
  EVP_ENCODE_CTX ectx;

  RETURN_VOID_IF_NULL(raw);
  RETURN_VOID_IF_NULL(buf);
  RETURN_VOID_IF_NULL(buflen);
	
  memset(buf, 0, *buflen);
  EVP_EncodeInit(&ectx);
  EVP_EncodeUpdate(&ectx, buf, buflen, (byte*)raw, rawlen);
  EVP_EncodeFinal(&ectx, (unsigned char*)strchr((const char*)buf, 0), buflen); 
  *buflen = strlen((const char*)buf);
  while(buf[*buflen-1] == '\n' || buf[*buflen-1] == '\r')
    buf[*buflen-1] = 0;
  *buflen = strlen((const char*)buf);
}


byte* breakToLinesOf64(byte* raw, int rawlen)
{
  int i, l1, j;
  byte* p = raw;
	
  // if it's not divide in lines
  l1 = rawlen;
  j = l1 + (l1/64) * 3;
  p = (byte*)malloc(j);
  if (!p) return NULL;
  memset(p, 0, j);
  for(i = j = 0; i < l1; i += 64) {
    strncpy(strchr((const char*)p,0), (const char*)raw+i, ((i+64<l1) ? 64 : l1-i));
    if(i + 64 < l1)
      strncat((char*)p, "\n", j - strlen((char*)p));
  }
  l1 = strlen((const char*)p);
  return p;
}

//============================================================
// Decodes input data in Base64 format.
// raw - input data
// rawlen - length of input data
// buf - destination buffer
// buflen - destination buffer length (will be modified by catual number of bytes)
//============================================================
EXP_OPTION void decode(const byte* raw, int rawlen, byte* buf, int* buflen) 
{
  EVP_ENCODE_CTX ectx;
  int l1 = 0;
  byte* p;

  RETURN_VOID_IF_NULL(raw);
  RETURN_VOID_IF_NULL(buf);
  RETURN_VOID_IF_NULL(buflen);
  
  memset(buf, 0, *buflen);
  *buflen = 0;
  EVP_DecodeInit(&ectx);
  if((!strstr((const char*)raw, "\n") ||
      !strstr((const char*)raw, "\r")) && 
     strlen((const char*)raw) > 64) {
    p = breakToLinesOf64((byte*)raw, rawlen);
    l1 = strlen((const char*)p);
    EVP_DecodeUpdate(&ectx, (unsigned char*)buf, &l1, 
		     (unsigned char*)p, 
		     strlen((const char*)p));
    *buflen += l1;
    free(p);
  }
  else
    EVP_DecodeUpdate(&ectx, buf, buflen, (byte*)raw, rawlen);
  EVP_DecodeFinal(&ectx, buf, &l1);
  *buflen += l1;
}


//============================================================
// Helper function to determine if this is a base64 char
//============================================================
int isb64char(int c)
{
  return ( (c >= 'A' && c <= 'Z') ||
	   (c >= 'a' && c <= 'z') ||
	   (c >= '0' && c <= '9') ||
	   (c == '+') ||
	   (c == '/') ||
	   (c == '=')
	   );
}

//============================================================
// Decodes input data in Base64 format.
// pMBufSrc - input data
// pMBufDest - destination buffer
//============================================================
EXP_OPTION int ddocDecodeBase64(DigiDocMemBuf* pMBufSrc, DigiDocMemBuf* pMBufDest) 
{
  int err = ERR_OK, n;
  long lPos1 = 0, lPos2 = 0;
  EVP_ENCODE_CTX ectx;
  char buf1[70];
  RETURN_IF_NULL(pMBufSrc);
  RETURN_IF_NULL(pMBufDest);
	
  pMBufDest->pMem = 0;
  pMBufDest->nLen = 0;
  // alloc mem for result - it will get smaller so original length must be enough
  err = ddocMemSetLength(pMBufDest, pMBufSrc->nLen);
  if(err) return err;
  EVP_DecodeInit(&ectx);
  // decode base64
  while(lPos1 < pMBufSrc->nLen) {
    // copy next input row
    memset(buf1, 0, sizeof(buf1));
    n = 0;
    while(n < 64 && lPos1 < pMBufSrc->nLen) {
      if(isb64char(((char*)pMBufSrc->pMem)[lPos1])) {
	buf1[n] = ((char*)pMBufSrc->pMem)[lPos1];
	n++;
      }
      lPos1++;
    }
    strncat(buf1, "\n", sizeof(buf1) - strlen(buf1));
    // decode this chunk
    n = pMBufDest->nLen - lPos2;
    EVP_DecodeUpdate(&ectx, (unsigned char*)((char*)pMBufDest->pMem + lPos2), &n, 
		     (unsigned char*)buf1, strlen((const char*)buf1));
    lPos2 += n;
  }
  memset(buf1, 0, sizeof(buf1));
  n = sizeof(buf1);
  EVP_DecodeFinal(&ectx, (unsigned char*)buf1, &n);
  lPos2 += n;
  pMBufDest->nLen = lPos2;
  return err;
}


//============================================================
// Decodes input data in Base64 format.
// data - input data
// len - length of input data. Use -1 for zero terminated strings
// pMBufDest - destination buffer
//============================================================
EXP_OPTION int ddocDecodeBase64Data(void* data, long len, DigiDocMemBuf* pMBufDest) 
{
  DigiDocMemBuf mbuf1;

  mbuf1.pMem = data;
  if(len == -1)
    mbuf1.nLen = strlen((const char*)data);
  else
    mbuf1.nLen = len;
  return ddocDecodeBase64(&mbuf1, pMBufDest);
}


//============================================================
// Encodes input data in Base64 format.
// pMBufSrc - input data
// pMBufDest - destination buffer
//============================================================
EXP_OPTION int ddocEncodeBase64(const DigiDocMemBuf* pMBufSrc, DigiDocMemBuf* pMBufDest) 
{
  int err = ERR_OK, nLen;
  EVP_ENCODE_CTX ectx;

  RETURN_IF_NULL(pMBufSrc);
  RETURN_IF_NULL(pMBufDest);
	
  pMBufDest->pMem = 0;
  pMBufDest->nLen = 0;
  // alloc mem for result
  err = ddocMemSetLength(pMBufDest, pMBufSrc->nLen * 2 + 10);
  if(err) return err;
  EVP_EncodeInit(&ectx);
  // encode base64
  nLen = pMBufDest->nLen;
  EVP_EncodeUpdate(&ectx, (unsigned char*)pMBufDest->pMem, &nLen, 
		   (byte*)pMBufSrc->pMem, pMBufSrc->nLen);
  pMBufDest->nLen = nLen;
  nLen = (pMBufSrc->nLen * 2 + 10) - pMBufDest->nLen;
  EVP_EncodeFinal(&ectx, (unsigned char*)pMBufDest->pMem + pMBufDest->nLen, &nLen);
  pMBufDest->nLen += nLen; //strlen((const char*)pMBufDest->pMem);
  return err;
}

//==========< conversion fucntions >================

//--------------------------------------------------
// Decodes an ASN1 generalized time
// tm - ASN1 generalized time
// y - year
// M - month
// d - day of month
// h - hour
// m - minute
// s - second
// returns error code or ERR_OK
//--------------------------------------------------
int decodeGeneralizedTime(ASN1_GENERALIZEDTIME *tm, 
			   int* y, int* M, int* d, 
			   int* h, int* m, int* s)
{
  char *v = NULL;
  int gmt=0;
  int i, err = ERR_OK;

  RETURN_IF_NULL_PARAM(tm);
  RETURN_IF_NULL_PARAM(y);
  RETURN_IF_NULL_PARAM(M);
  RETURN_IF_NULL_PARAM(d);
  RETURN_IF_NULL_PARAM(h);
  RETURN_IF_NULL_PARAM(m);
  RETURN_IF_NULL_PARAM(s);
  
  i=tm->length;
  v=(char *)tm->data;
  if (i < 12) 
    err = ERR_TIMESTAMP_DECODE;
  if (v[i-1] == 'Z') gmt=1;
  for (i=0; i<12; i++)
    if ((v[i] > '9') || (v[i] < '0')) 
      err = ERR_TIMESTAMP_DECODE;
  *y= (v[0]-'0')*1000+(v[1]-'0')*100 + (v[2]-'0')*10+(v[3]-'0');
  *M= (v[4]-'0')*10+(v[5]-'0');
  if ((*M > 12) || (*M < 1)) 
    err = ERR_TIMESTAMP_DECODE;
  *d= (v[6]-'0')*10+(v[7]-'0');
  *h= (v[8]-'0')*10+(v[9]-'0');
  *m=  (v[10]-'0')*10+(v[11]-'0');
  if (	(v[12] >= '0') && (v[12] <= '9') &&
	(v[13] >= '0') && (v[13] <= '9'))
    *s=  (v[12]-'0')*10+(v[13]-'0');
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}

//--------------------------------------------------
// Decodes an ASN1 UTC time
// tm - ASN1 generalized time
// y - year
// M - month
// d - day of month
// h - hour
// m - minute
// s - second
// returns error code or ERR_OK
//--------------------------------------------------
int decodeUTCTime(ASN1_UTCTIME *tm, 
		  int* y, int* M, int* d, 
		  int* h, int* m, int* s)
{
  char *v;
  int gmt=0;
  int i, err = 0;
  
  RETURN_IF_NULL_PARAM(tm);
  RETURN_IF_NULL_PARAM(y);
  RETURN_IF_NULL_PARAM(M);
  RETURN_IF_NULL_PARAM(d);
  RETURN_IF_NULL_PARAM(h);
  RETURN_IF_NULL_PARAM(m);
  RETURN_IF_NULL_PARAM(s);

  i=tm->length;
  v=(char *)tm->data;
	
  if (i < 10) 
    err = ERR_TIMESTAMP_DECODE;
  if (v[i-1] == 'Z') gmt=1;
  for (i=0; i<10; i++)
    if ((v[i] > '9') || (v[i] < '0')) 
      err = ERR_TIMESTAMP_DECODE;
  *y= (v[0]-'0')*10+(v[1]-'0');
  if (*y < 50) *y+=100;
  *M= (v[2]-'0')*10+(v[3]-'0');
  if ((*M > 12) || (*M < 1)) 
    err = ERR_TIMESTAMP_DECODE;
  *d= (v[4]-'0')*10+(v[5]-'0');
  *h= (v[6]-'0')*10+(v[7]-'0');
  *m=  (v[8]-'0')*10+(v[9]-'0');
  if (	(v[10] >= '0') && (v[10] <= '9') &&
	(v[11] >= '0') && (v[11] <= '9'))
    *s=  (v[10]-'0')*10+(v[11]-'0');
  
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}

//==========================================================
// converts ASN1 time to time_t
//==========================================================
int asn1time2time_t(ASN1_TIME* tm, time_t* pT)
{
  int err = ERR_OK;
  struct tm tm1;
  int dmz=0;
	
  RETURN_IF_NULL_PARAM(tm);
  RETURN_IF_NULL_PARAM(pT);	
  _tzset();
  memset(&tm1, 0, sizeof(tm1));
  if(tm->type == V_ASN1_UTCTIME)
    err = decodeUTCTime(tm, &tm1.tm_year, &tm1.tm_mon, 
			&tm1.tm_mday, &tm1.tm_hour, &tm1.tm_min, &tm1.tm_sec);
  if(tm->type == V_ASN1_GENERALIZEDTIME)
    err = decodeGeneralizedTime(tm, &tm1.tm_year, &tm1.tm_mon, 
				&tm1.tm_mday, &tm1.tm_hour, &tm1.tm_min, &tm1.tm_sec);
  tm1.tm_year -= 1900;
  tm1.tm_mon -= 1;
  tm1.tm_isdst = _daylight;
  (*pT) = mktime(&tm1);
  /*
  if(_daylight!=0) {
    if(_timezone<0){
      dmz = (_timezone / 3600) - _daylight;
    }else{
      dmz = (_timezone / 3600) + _daylight;
    }
  }else{
    dmz=_timezone / 3600;
  }
  (*pT) = (*pT) - (dmz * 3600);	
  */
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}


//==========================================================
// converts ASN1 time to time_t
//==========================================================
int asn1time2time_t_local(ASN1_TIME* tm, time_t* pT)
{
  int err = ERR_OK;
  struct tm tm1;
  int dmz=0;
	
  RETURN_IF_NULL_PARAM(tm);
  RETURN_IF_NULL_PARAM(pT);	
  _tzset();
  memset(&tm1, 0, sizeof(tm1));
  if(tm->type == V_ASN1_UTCTIME)
    err = decodeUTCTime(tm, &tm1.tm_year, &tm1.tm_mon, 
			&tm1.tm_mday, &tm1.tm_hour, &tm1.tm_min, &tm1.tm_sec);
  if(tm->type == V_ASN1_GENERALIZEDTIME)
    err = decodeGeneralizedTime(tm, &tm1.tm_year, &tm1.tm_mon, 
				&tm1.tm_mday, &tm1.tm_hour, &tm1.tm_min, &tm1.tm_sec);
  //tm1.tm_year -= 1900;
  tm1.tm_mon -= 1;
  tm1.tm_isdst = _daylight;
  (*pT) = mktime(&tm1);
  if(_daylight != 0) {
    if(_timezone<0){
      dmz = (_timezone / 3600) - _daylight;
    }else{
      dmz = (_timezone / 3600) + _daylight;
    }
  }else{
    dmz=_timezone / 3600;
  }
  (*pT) = (*pT) - (dmz * 3600);	
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}


//==========================================================
// converts ASN1 time to string
//==========================================================
int asn1time2strYear(const SignedDoc* pSigDoc, ASN1_TIME* tm, char* buf, int year, int len)
{
  int err = ERR_OK;
  struct tm tm1, tm2;
  time_t t2;
  int dmz=0;
	
  RETURN_IF_NULL_PARAM(tm);
  RETURN_IF_NULL_PARAM(buf);	
  //RETURN_IF_NULL_PARAM(pSigDoc);	
  _tzset();
  memset(&tm1, 0, sizeof(tm1));
  if(tm->type == V_ASN1_UTCTIME)
    err = decodeUTCTime(tm, &tm1.tm_year, &tm1.tm_mon, 
			&tm1.tm_mday, &tm1.tm_hour, &tm1.tm_min, &tm1.tm_sec);
  if(tm->type == V_ASN1_GENERALIZEDTIME)
    err = decodeGeneralizedTime(tm, &tm1.tm_year, &tm1.tm_mon, 
				&tm1.tm_mday, &tm1.tm_hour, &tm1.tm_min, &tm1.tm_sec);

  // V 1.76 - in format 1.3 we use CCYY-MM-DDTHH:MM:SSZ
  if(!pSigDoc || (pSigDoc && 
		  (!strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_3_VER) 
#ifdef WITH_TS
		   || !strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)
#endif
		   ))) {
    snprintf(buf, len, "%04d-%02d-%02dT%02d:%02d:%02dZ",
	    year + tm1.tm_year, tm1.tm_mon, tm1.tm_mday, 
	    tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
  } else
    // in version 1.0 we use format CCYY.MM.DDTHH:MM:SS-TZ
    if(pSigDoc && !strcmp(pSigDoc->szFormatVer, SK_XML_1_VER)) {
      tm1.tm_year -= 1900 - year;
      tm1.tm_mon -= 1;
      tm1.tm_isdst = _daylight;
      t2 = mktime(&tm1);
      if (err == ERR_OK) {
	if (_daylight!=0) {
	  if (_timezone<0) {
	    dmz = (_timezone / 3600) - _daylight;
	  } else {
	    dmz = (_timezone / 3600) + _daylight;
	  }
	} else {
	  dmz=_timezone / 3600;
	}
	t2 -= (dmz * 3600);
	ddocLocalTime(&t2, &tm2, 1);
	snprintf(buf, len, "%04d.%02d.%02dT%02d:%02d:%02d%+03d:00",
		tm2.tm_year + 1900, tm2.tm_mon + 1, 
		tm2.tm_mday, tm2.tm_hour, tm2.tm_min, 
		tm2.tm_sec, dmz * -1);
	ddocDebug(5, "asn1time2strYear", 
		  "ASN1 time: %s, tz: %d, string: %s\n", tm->data, dmz * -1, buf);		
      }
    } else { // in version 1.1 we use format CCYY.MM.DDTHH:MM:SSZ and allways UTC time
      snprintf(buf, len, "%04d.%02d.%02dT%02d:%02d:%02dZ",
	      year + tm1.tm_year, tm1.tm_mon, tm1.tm_mday, 
	      tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
    }
  
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}

//==========================================================
// converts ASN1 time to string
//==========================================================
int asn1time2str(const SignedDoc* pSigDoc, ASN1_TIME* tm, char* buf, int len)
{
  return asn1time2strYear(pSigDoc, tm, buf, 0, len);
}



//==============================================================================
// converts ASN1_GENERALIZEDTOME object to string
//==============================================================================
int str2asn1time(const SignedDoc* pSigDoc, const char* str, ASN1_GENERALIZEDTIME* asn1tm)
{
  char buf[50];
  int rc, dmz = 0;
  struct tm tm1, tm2;
  time_t t2;

	
  RETURN_IF_NULL_PARAM(str);
  RETURN_IF_NULL_PARAM(asn1tm);
  _tzset();
  memset(&tm1, 0, sizeof(tm1));
  // in version 1.0 we use format CCYY.MM.DDTHH:MM:SS-TZ
  if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER)) {
    sscanf(str, "%04d.%02d.%04dT%02d:%02d:%02d%3d:00",
	   &(tm1.tm_year), &(tm1.tm_mon), &(tm1.tm_mday), 
	   &(tm1.tm_hour), &(tm1.tm_min), &(tm1.tm_sec), &dmz);
    tm1.tm_year -= 1900;
    tm1.tm_mon -= 1;
    tm1.tm_isdst = _daylight;
    t2 = mktime(&tm1);
    t2 -= (dmz * 3600);
    ddocLocalTime(&t2, &tm2, 1);
    snprintf(buf, sizeof(buf), "%04d%02d%02d%02d%02d%02dZ", 
	    tm2.tm_year + 1900, tm2.tm_mon + 1, tm2.tm_mday, 
	    tm2.tm_hour, tm2.tm_min, tm2.tm_sec);
  } else { // in version 1.1 we use format CCYY.MM.DDTHH:MM:SSZ and allways UTC time
    sscanf(str, "%04d.%02d.%04dT%02d:%02d:%02dZ",
	   &(tm1.tm_year), &(tm1.tm_mon), &(tm1.tm_mday), 
	   &(tm1.tm_hour), &(tm1.tm_min), &(tm1.tm_sec));
    snprintf(buf, sizeof(buf), "%04d%02d%02d%02d%02d%02dZ", 
	    tm1.tm_year, tm1.tm_mon, tm1.tm_mday, 
	    tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
  }
  rc = ASN1_GENERALIZEDTIME_set_string(asn1tm, buf);
  //printf("String: %s, tz: %d, ASN1 time: %s\n", str, dmz, asn1tm->data);
  return ERR_OK;
}

//===================================================================
// converts time_t to timestamp string
// t - time_t input value
// szTimestamp - output buffer
// len - length of buffer
// returns error code or ERR_OK
//===================================================================
EXP_OPTION int time_t2str(time_t t, char* szTimestamp, int len)
{
  struct tm tm1;

  RETURN_IF_NULL_PARAM(szTimestamp);
  ddocLocalTime(&t, &tm1, 0);
  snprintf(szTimestamp, len, "%04d-%02d-%02dT%02d:%02d:%02dZ", 
	  tm1.tm_year + 1900, tm1.tm_mon + 1, 
	  tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
  return ERR_OK;
}
