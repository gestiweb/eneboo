#ifndef __DIGIDOC_GEN_H__
#define __DIGIDOC_GEN_H__
//==================================================
// FILE:	DigiDocGen.h
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
#include <libdigidoc/DigiDocObj.h>
#include <libdigidoc/DigiDocMem.h>


//==========< XML generation routines >========================

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef WITH_TIMETSTAMP_STRUCT
// contains timestamp data
typedef struct Timestamp_st {
  int year;
  int mon;
  int day;
  int hour;
  int min;
  int sec;
  int tz;
} Timestamp;

// converts timestamp string to it's components
EXP_OPTION int convertStringToTimestamp(const SignedDoc* pSigDoc, const char* szTimestamp, Timestamp* pTimestamp);
// converts timestamp to string 
EXP_OPTION int convertTimestampToString(const SignedDoc* pSigDoc, const Timestamp* pTimestamp, char* szTimestamp, int len);
// creates new timestamp object
EXP_OPTION int Timestamp_new(Timestamp **, int year,int month,int day,int hour,int minute,int second,int timezone);
// frees this timestamp object
EXP_OPTION void Timestamp_free(Timestamp* pTimestamp);

#endif

//============================================================
// Creates a timestamp string
// buf - output buffer
// len - length of output buffer
// returns number of output bytes written
//============================================================
int createTimestamp(const SignedDoc* pSigDoc, char* buf, int len);

// converts timestamp string to time_t value
EXP_OPTION time_t convertStringToTimeT(const SignedDoc* pSigDoc, const char* szTimestamp);


//--------------------------------------------------
// Appends an xml element start to buffer, but no ">"
// pBuf - memory buffer to store xml [REQUIRED]
// elemName - xml element name [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocGen_startElemBegin(DigiDocMemBuf* pBuf, const char* elemName);

//--------------------------------------------------
// Appends an xml element start tag end to buffer - ">"
// pBuf - memory buffer to store xml [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocGen_startElemEnd(DigiDocMemBuf* pBuf);

//--------------------------------------------------
// Appends an xml element start to buffer - <tag>
// pBuf - memory buffer to store xml [REQUIRED]
// elemName - xml element name [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocGen_startElem(DigiDocMemBuf* pBuf, const char* elemName);

//--------------------------------------------------
// Appends an xml element end to buffer
// pBuf - memory buffer to store xml [REQUIRED]
// elemName - xml element name [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocGen_endElem(DigiDocMemBuf* pBuf, const char* elemName);

//--------------------------------------------------
// Appends an xml element's atribute to buffer
// pBuf - memory buffer to store xml [REQUIRED]
// name - xml atribute name [REQUIRED]
// value - xml atribute value [REQUIRED]
// returns error code or ERR_OK
//--------------------------------------------------
int ddocGen_addAtribute(DigiDocMemBuf* pBuf, const char* name, const char* value);

//================< functions generating DigiDoc formats 1.0 - 1.3 > =================================

// writes the signed doc to a file
EXP_OPTION int createSignedDoc(SignedDoc* pSigDoc, const char* szOldFile, const char* szSigDocFile);

//============================================================
// Canonicalizes XML
// source - input data
// len - input length
// returns a newly allocated buffer with canonicalized XML
// Caller must free() the result.
//============================================================
char* canonicalizeXML(char* source, int len);

//============================================================
// Creates a <SignedProperties> XML block
// pSigDoc - signed document pointer
// pSigInfo - signature info data
// bWithEscapes - 1=escape xml sümbols, 0=don't escape
// returns new <SignedProperties> node
//============================================================
char* createXMLSignedProperties(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo, int bWithEscapes);

//============================================================
// Generates DataFile elements XML form and stores it in a file
// pSigDoc - signed document
// pDataFile - data file object to be converted
// szDataFile - input file name
// hFile - output file handle
// pMBufXML - output buffer if we want data to be returned in mem buf
//============================================================
int generateDataFileXML(SignedDoc* pSigDoc, DataFile* pDataFile, 
			const char* szDataFile, FILE* hFile, DigiDocMemBuf* pMBufXML);

//--------------------------------------------------
// Creates a new signed document in memory buffer
// pSigDoc - signed doc info
// szOldFile - name of old file on disk to copy DataFile contents
// pMBuf - buffer for new digidoc document
// returns error code or ERR_OK for success
//--------------------------------------------------
EXP_OPTION int createSignedDocInMemory(SignedDoc* pSigDoc, const char* szOldFile, DigiDocMemBuf* pMBuf);

//--------------------------------------------------
// Removes incomplete or orphoned signatures.
// Signature is incomplete if it hasn't got the signature
// value
// pSigDoc - signed doc info
// returns error code or ERR_OK for success
//--------------------------------------------------
EXP_OPTION int removeIncompleteSignatures(SignedDoc* pSigDoc);

//--------------------------------------------------
// Checks for incomplete or orphoned signatures.
// Signature is incomplete if it hasn't got the signature
// value
// pSigDoc - signed doc info
// returns error code if DigiDoc has orphoned signature or ERR_OK for success
//--------------------------------------------------
EXP_OPTION int hasIncompleteSignatures(SignedDoc* pSigDoc);

#ifdef  __cplusplus
}
#endif


#endif // __DIGIDOC_GEN_H__
