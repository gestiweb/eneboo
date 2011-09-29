#ifndef __DIGIDOC_DF_EXTRACT_H__
#define __DIGIDOC_DF_EXTRACT_H__
//==================================================
// FILE:	DigiDocDfExtract.h
// PROJECT:     Digi Doc
// DESCRIPTION: Digi Doc functions for extracting <DataFile> contents
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
//      03.03.2008      Veiko Sinivee
//                      Creation
//==================================================

#include <libdigidoc/DigiDocDefs.h>

#ifdef  __cplusplus
extern "C" {
#endif

//--------------------------------------------------
// Reads in signed XML document and extracts the desired data file
// pSigDoc - signed document object if exists. Can be NULL
// szFileName - digidoc filename
// szDataFileName - name of the file where to store embedded data. 
// szDocId - DataFile Id atribute value
// szCharset - convert DataFile content to charset
//--------------------------------------------------
EXP_OPTION int ddocExtractDataFile(SignedDoc* pSigDoc, const char* szFileName, 
				      const char* szDataFileName, const char* szDocId, 
				      const char* szCharset);

#ifdef  __cplusplus
}
#endif


#endif
