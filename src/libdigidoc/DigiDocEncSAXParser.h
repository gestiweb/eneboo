#ifndef __DIGIDOC_ENC_SAX_PARSER_H__
#define __DIGIDOC_ENC_SAX_PARSER_H__
//==================================================
// FILE:	DigiDocEncSAXParser.h
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


#include <libdigidoc/DigiDocEnc.h>
#include <libdigidoc/DigiDocDefs.h>

#ifdef  __cplusplus
extern "C" {
#endif

  //--------------------------------------------------
  // Reads in encrypted XML document.
  // ppEncData - address for new encrypted data object [REQUIRED]
  // szFileName - input file name
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencSaxReadEncryptedData(DEncEncryptedData** ppEncData, const char* szFileName);

  //--------------------------------------------------
  // Decrypts an encrypted XML document and stores the
  // cleartext data in another document. Please note that szCertCN 
  // parameter is no longer required as the users cert will
  // bea read from smartcard using PKCS#11
  // szInputFileName - input file name [REQUIRED]
  // szOutputFileName - output file name [REQUIRED]
  // szPin - PIN1 of the id-card to decrypt the transport key [REQUIRED]
  // returns error code or ERR_OK
  //--------------------------------------------------
  EXP_OPTION int dencSaxReadDecryptFile(const char* szInputFileName, 
					const char* szOutputFileName, 
					const char* szPin);


#ifdef  __cplusplus
}
#endif


#endif // __DIGIDOC_ENC_SAX_PARSER_H__
