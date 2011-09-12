#ifndef __DIGIDOC_SAX_PARSER_H__
#define __DIGIDOC_SAX_PARSER_H__
//==================================================
// FILE:	DigiDocSAXParser.h
// PROJECT:     Digi Doc
// DESCRIPTION: Digi Doc functions for xml parsing using SAX interface
//              This is the older parser and will probably
//              be removed in new versions
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
//      12.08.2004      Veiko Sinivee
//                      Creation
//      22.08.2004      Veiko Sinivee
//                      Renamed readSignedDoc() to ddocSaxReadSignedDocFromFile(). Params changed.
//                      Renamed extractDataFile() to ddocSaxExtractDataFile(). Uses chached content.
//==================================================

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocLib.h>

#ifdef  __cplusplus
extern "C" {
#endif

//--------------------------------------------------
// Reads in signed XML document info from digidoc file
// ppSigDoc - pointer to the buffer of newly read info pointer
// szFileName - documents filename
// checkFileDigest - indicates if digests of datafiles referred by the document must be checked
// lMaxDFLen - maximum size for a DataFile whose contents will be
// kept in memory
//--------------------------------------------------
EXP_OPTION int ddocSaxReadSignedDocFromFile(SignedDoc** ppSigDoc, const char* szFileName, 
					    int checkFileDigest, long lMaxDFLen);

//--------------------------------------------------
// Reads in signed XML document and extracts the desired data file
// pSigDoc - signed document object if exists. Can be NULL
// szFileName - digidoc filename
// szDataFileName - name of the file where to store embedded data. 
// szDocId - DataFile Id atribute value
// szCharset - convert DataFile content to charset
//--------------------------------------------------
EXP_OPTION int ddocSaxExtractDataFile(SignedDoc* pSigDoc, const char* szFileName, const char* szDataFileName, 
				      const char* szDocId, const char* szCharset);

//--------------------------------------------------
// Reads in signed XML document and returns the
// desired DataFile-s content in a memory buffer.
// caller is responsible for freeing the memory.
// pSigDoc - signed document object if cached
// szFileName - name of digidoc file
// szDocId - id if DataFile
// pBuf - address of buffer pointer
// bKeepBase64 - 1=don't decode base64, 0=decode base64
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocSAXGetDataFile(SignedDoc* pSigDoc, const char* szFileName,
				  const char* szDocId, DigiDocMemBuf* pBuf,
				  int bKeepBase64);

//--------------------------------------------------
// Reads in signed XML document info from memory buffer
// ppSigDoc - pointer to the buffer of newly read info pointer
// szFileName - documents filename
// checkFileDigest - indicates if digests of datafiles referred by the document must be checked
// lMaxDFLen - maximum size for a DataFile whose contents will be
// kept in memory
//--------------------------------------------------
EXP_OPTION int ddocSaxReadSignedDocFromMemory(SignedDoc** ppSigDoc, const void* pData, 
					      int len, long lMaxDFLen);

//--------------------------------------------------
// Reads new signatures from another digidoc file
// and adds to existing digidoc. Adds only those
// signatures that don't exist in old digidoc.
// pSigDoc - signed document object
// szFileName - name of digidoc file
// returns error code or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocReadNewSignaturesFromDdoc(SignedDoc* pSigDoc, const char* szFileName);


#ifdef  __cplusplus
}
#endif


#endif // __DIGIDOC_SAX_PARSER_H__

