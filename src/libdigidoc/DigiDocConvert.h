#ifndef __DIGIDOC_CONVERT_H__
#define __DIGIDOC_CONVERT_H__
//==================================================
// FILE:	DigiDocEnc.h
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

#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocDefs.h>

#ifdef  __cplusplus
extern "C" {
#endif

//==========< general functions >============

//--------------------------------------------------
// Decodes a single hex digit
// h - hex digit
// return binary value
//--------------------------------------------------
byte h2b(char h);

//--------------------------------------------------
// Converts a single byte to two hex characters
// b - binary value
// dest - destination buffer
//--------------------------------------------------
void b2h(byte b, char* dest);

//--------------------------------------------------
// Converts correct filename to incorrect encoding
// used in formats 1.0, 1.1 and 1.2
// src - input data
// dest - buffer for converted data
// len - length of destination buffer
//--------------------------------------------------
EXP_OPTION void convFNameToWin(const char* src, char* dest, int len);

//--------------------------------------------------
// Converts bad UTF-8 filename used in formats 1.0, 
// 1.1 and 1.2 to correct encoding
// src - input data
// dest - buffer for converted data
// len - length of destination buffer
//--------------------------------------------------
EXP_OPTION void convWinToFName(const char* src, char* dest, int len);

// Converts a hex number (string) to binary value
EXP_OPTION void hex2bin(const char* hex, byte* bin, int* len);

// Converts a binary value to hex string
EXP_OPTION void bin2hex(const byte* bin, int blen, char* hex, int* len);

// Helper function that converts ISO Latin1 to UTF8
EXP_OPTION char* ascii2utf8(const char* ascii, char* utf8out, int* outlen);

// Helper function that converts UTF8 to ISO Latin1
EXP_OPTION char* utf82ascii(const char* utf8in, char* asciiout, int* outlen);

// converts string from unicode to ascii
EXP_OPTION void unicode2ascii(const char* uni, char* dest);
// converts unicode escapes string to UTF8
EXP_OPTION void unicodeEscapes2utf8(const char* uni, char* dest);

// replaces xml special symbols with escapes
int escapeXMLSymbols(const char* src, int srclen, char** dest);
int hasUmlauts(const char* str);
int str2asn1time(const SignedDoc* pSigDoc, const char* str, ASN1_GENERALIZEDTIME* asn1tm);

//--------------------------------------------------
// Converts xml symbols to corresponding escapes
// src - string with xml special sybols
// returns string with esacpes
//--------------------------------------------------
char* escape2xmlsym(const char* src);

//--------------------------------------------------
// Converts input data to UTF-8
// src - input data
// returns converted string or NULL. Caller must free it.
//--------------------------------------------------
EXP_OPTION int ddocConvertInput(const char* src, char** dest);

// Base64 encode some data
EXP_OPTION void encode(const byte* raw, int rawlen, byte* buf, int* buflen);
// Base64 decode some data
EXP_OPTION void decode(const byte* raw, int rawlen, byte* buf, int* buflen);


//============================================================
// Decodes input data in Base64 format.
// pMBufSrc - input data
// pMBufDest - destination buffer
//============================================================
EXP_OPTION int ddocDecodeBase64(DigiDocMemBuf* pMBufSrc, DigiDocMemBuf* pMBufDest);

//============================================================
// Decodes input data in Base64 format.
// data - input data
// len - length of input data. Use -1 for zero terminated strings
// pMBufDest - destination buffer
//============================================================
EXP_OPTION int ddocDecodeBase64Data(void* data, long lLen, DigiDocMemBuf* pMBufDest);

//============================================================
// Encodes input data in Base64 format.
// pMBufSrc - input data
// pMBufDest - destination buffer
//============================================================
EXP_OPTION int ddocEncodeBase64(const DigiDocMemBuf* pMBufSrc, DigiDocMemBuf* pMBufDest);

//=======< time convesrion >=======================

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
			  int* h, int* m, int* s);

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
		  int* h, int* m, int* s);

//==========================================================
// converts ASN1 time to time_t
//==========================================================
int asn1time2time_t(ASN1_TIME* tm, time_t* pT);

int asn1time2time_t_local(ASN1_TIME* tm, time_t* pT);

//==========================================================
// converts ASN1 time to string
//==========================================================
int asn1time2strYear(const SignedDoc* pSigDoc, ASN1_TIME* tm, char* buf, int year, int len);

//==========================================================
// converts ASN1 time to string
//==========================================================
int asn1time2str(const SignedDoc* pSigDoc, ASN1_TIME* tm, char* buf, int len);

//===================================================================
// converts time_t to timestamp string
// t - time_t input value
// szTimestamp - output buffer
// len - length of buffer
// returns error code or ERR_OK
//===================================================================
  EXP_OPTION int time_t2str(time_t t, char* szTimestamp, int len);

//==========< only win32 fucntions >============

#ifdef WIN32

//--------------------------------------------------
// Converts input OEM charset data to 16 bit unicode.
// oem - 8 bit oem charset input data
// unicode - address of pointer for allocated unicode string. Caller must free() !
// outlen - address of length variable for unicode string
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int oem2unicode(const char* oem, char** unicode, int* outlen);

//--------------------------------------------------
// Converts input 16 bit unicode data to UTF8.
// unicode - 16 bit unicode input data
// utf8 - address of pointer for allocated utf8 string. Caller must free() !
// outlen - address of length variable for utf8 string
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int unicode2utf8(const char* unicode, char** utf8, int* outlen);

//--------------------------------------------------
// Converts input OEM charset data to UTF-8
// oem - 8 bit oem charset input data
// utf8 - address of buffer allocated utf8 string. 
// len - size of buffer
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int oem2utf8(const char* oem, char* utf8, int len);

//--------------------------------------------------
// Converts input UTF-8 data to 16 bit unicode data.
// utf8 - UTF-8 input data
// unicode - address of pointer for allocated unicode string. Caller must free() !
// outlen - address of length variable for unicode string
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int utf82unicode(const char* utf8, char** unicode, int* outlen);

//--------------------------------------------------
// Converts input 16 bit unicode data to oem charset data.
// unicode - 16 bit unicode input data
// oem - address of pointer for allocated oem string. Caller must free() !
// outlen - address of length variable for oem string
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int unicode2oem(const char* unicode, char** oem, int* outlen);

//--------------------------------------------------
// Converts input UTF-8 data to OEM charset data
// utf8 - UTF-8 input data
// oem - address of buffer for oem string. 
// len - size of buffer
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int utf82oem(const char* utf8, char* oem, int len);

//--------------------------------------------------
// Converts input UTF-8 data to OEM charset data
// pSigDoc - signed doc object
// pDf - data file obejct 
// outFileName - output buffer
// len - length of output buffer
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int getDataFileFileName(SignedDoc* pSigDoc, DataFile* pDf, char* outFileName, int len);

//--------------------------------------------------
// Converts a filename according to platform rules
// dest - destination buffer
// destlen - destination buffer length
// src - source filename
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocConvertFileName(char* dest, int destlen, const char* src);

//--------------------------------------------------
// Releases mem-block allocated by lib. In win32
// this must be done since the mem was allocated by dll
// and must also be released by dll that allocated it.
// p - mem to be freed
//--------------------------------------------------
EXP_OPTION void freeLibMem(void* p);

#endif // WIN32

#ifdef  __cplusplus
}
#endif


#endif // __DIGIDOC_CONVERT_H__



