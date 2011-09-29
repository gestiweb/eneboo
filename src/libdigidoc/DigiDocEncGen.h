#ifndef __DIGIDOC_ENC_GEN_H__
#define __DIGIDOC_ENC_GEN_H__
//==================================================
// FILE:	DigiDocEncGen.h
// PROJECT:     Digi Doc Encryption 
// DESCRIPTION: DigiDocEnc XML generation
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
#include <libdigidoc/DigiDocEnc.h>

#ifdef  __cplusplus
extern "C" {
#endif

  //--------------------------------------------------
  // Generates XML for <EncryptedData> element
  // pEncData - encrypted data object [REQUIRED]
  // pBuf - memory buffer for storing xml [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  int dencGenEncryptedData_toXML(DEncEncryptedData* pEncData, DigiDocMemBuf* pBuf);

  //--------------------------------------------------
  // Writes encrypted data to a file
  // pEncData - encrypted data object [REQUIRED]
  // szFileName - name of the file to write the data [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencGenEncryptedData_writeToFile(DEncEncryptedData* pEncData, const char* szFileName);

  //--------------------------------------------------
  // Generates the header of XML for <EncryptedData> element
  // This contains everything upto the start of base64 encoded cipher data
  // pEncData - encrypted data object [REQUIRED]
  // pBuf - memory buffer for storing xml [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  int dencGenEncryptedData_header_toXML(DEncEncryptedData* pEncData, DigiDocMemBuf* pBuf);

  //--------------------------------------------------
  // Generates the trailer of XML for <EncryptedData> element
  // These are all the XML constructs following the 
  // base64 encoded cipher data.
  // pEncData - encrypted data object [REQUIRED]
  // pBuf - memory buffer for storing xml [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  int dencGenEncryptedData_trailer_toXML(DEncEncryptedData* pEncData, DigiDocMemBuf* pBuf);

#ifdef  __cplusplus
}
#endif


#endif // __DIGIDOC_ENC_GEN_H__

