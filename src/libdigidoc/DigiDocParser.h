#ifndef __DIGIDOC_PARSER_H__
#define __DIGIDOC_PARSER_H__
//==================================================
// FILE:	DigiDocParser.h
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
//      22.08.2004      Veiko Sinivee
//                      Renamed ddocExtractDataFile to ddocXRdrExtractDataFile
//                      Renamed ddocGetDataFile to ddocXRdrGetDataFile
//                      Renamed ddocCopyDataFile to ddocXRdrCopyDataFile
//                      introduced ddocXRdrReadSignedDocFromFile()
//==================================================

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocMem.h>

#ifdef  __cplusplus
extern "C" {
#endif



  //--------------------------------------------------
  // Reads in signed XML document and extracts the desired data file
  // pSigDoc - signed document object if cached
  // szFileName - name of digidoc file
  // szDataFileName - name of new data file 
  // szDocId - id if DataFile
  // szCharset - output charset
  // szFileNameCharset - digidoc filename charset
  //--------------------------------------------------
  EXP_OPTION int ddocXRdrExtractDataFile(SignedDoc* pSigDoc, const char* szFileName, 
					 const char* szDataFileName, 
					 const char* szDocId, const char* szCharset);

  //--------------------------------------------------
  // Reads in signed XML document and returns the
  // desired DataFile-s content in a memory buffer.
  // caller is responsible for freeing the memory.
  // pSigDoc - signed document object if cached
  // szFileName - name of digidoc file
  // szDocId - id if DataFile
  // pBuf - address of buffer pointer
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int ddocXRdrGetDataFile(SignedDoc* pSigDoc, const char* szFileName,
			       const char* szDocId, DigiDocMemBuf* pBuf);

  //--------------------------------------------------
  // Reads in signed XML document and extracts the desired data file
  // This function keeps also <DataFile> xml tags. It is used
  // internally to copy data content from an old digidoc file
  // to a temp file while saving a the file
  // pSigDoc - signed document object if cached
  // szFileName - name of digidoc file
  // szDataFileName - name of new data file 
  // szDocId - id if DataFile
  // szCharset - output charset
  //--------------------------------------------------
  int ddocXRdrCopyDataFile(SignedDoc* pSigDoc, const char* szFileName, const char* szDataFileName, 
		       const char* szDocId, const char* szCharset);

  //--------------------------------------------------
  // Reads in signed XML document
  // szFileName - name of digidoc file
  // ppSigDoc - address of new signed doc pointer
  // lMaxDFLen - maximum size of a cached DataFile
  //--------------------------------------------------
  EXP_OPTION int ddocXRdrReadSignedDocFromFile(const char* szFileName, 
					       SignedDoc** pSigDoc, long lMaxDFLen);

  
  //--------------------------------------------------
  // Reads in signed XML document from memory buffer
  // szXml - memory buffer that contains digidoc data
  // xmlLen - length of the input data
  // szFileNameCharset - digidoc filename charset
  // ppSigDoc - address of new signed doc pointer
  // lMaxDFLen - maximum size of a cached DataFile
  //--------------------------------------------------
  EXP_OPTION int ddocXRdrReadSignedDocFromMemory(const char* szXml, int xmlLen,
						 SignedDoc** pSigDoc, long lMaxDFLen);

#ifdef  __cplusplus
}
#endif


#endif // __DIGIDOC_PARSER_H__

