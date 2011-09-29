#ifndef __DIGIDOC_MSSP_GW_H__
#define __DIGIDOC_MSSP_GW_H__
//==================================================
// FILE:	DigiDocMsspGw.h
// PROJECT:     Digi Doc
// DESCRIPTION: Digi Doc functions for MSSP_GW client
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
//      25.09.2005      Veiko Sinivee
//                      Creation
//==================================================

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/mssp/stdsoap2.h>
#include <libdigidoc/mssp/MsspErrors.h>
#include <libdigidoc/DigiDocMem.h>
#include <openssl/x509.h>

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct MSSP_st
{
  struct soap soap;
  const char *endpoint;
  time_t  lApTxnId;
  unsigned long lMsspTxnId;
  int nStatusCode;
} MSSP;

//--------------------------------------------------
// Initializes MSSP connection
// pMssp - pointer to MSSP context structure
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int ddocMsspConnect(MSSP** pMssp);

//--------------------------------------------------
// Reconnects to MSSP service if necessary
// pMssp - pointer to MSSP context structure
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int ddocMsspReconnect(MSSP* pMssp);

//--------------------------------------------------
// Cleanup MSSP connection. Free memory
// pMssp - pointer to MSSP context structure
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int ddocMsspDisconnect(MSSP* pMssp);

//--------------------------------------------------
// Cleanup MSSP connection but don't disconnect
// pMssp - pointer to MSSP context structure
// returns ERR_OK on success
//--------------------------------------------------
EXP_OPTION int ddocMsspCleanup(MSSP* pMssp);

//--------------------------------------------------
// Sends an MSSP request to sign this data
// pMssp - pointer to MSSP context structure
// szPhoneNo - phone number on which to sign
// pHash - pointer to binary hash to sign
// nHashLen - length of hash data
// szDesc - description what user is signing (file name)
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocMsspSignatureReq(MSSP* pMssp, const char* szPhoneNo,
			 const char* pHash, int nHashLen, const char* szDesc);

//--------------------------------------------------
// Sends an MSSP request to find the status of signature
// operation and possibly it's value
// pMssp - pointer to MSSP context structure
// pMbufSignature - memory buffer to store signature value
// returns error code or SOAP_OK
//--------------------------------------------------
EXP_OPTION int ddocMsspStatusReq(MSSP* pMssp, DigiDocMemBuf* pMbufSignature);

//--------------------------------------------------
// Reads the signers certificate based on the signers 
// phone number.
// szPhoneNo - phone number
// szCertUrl - full url where to get cert. Use NULL for default url
// ppCert - address to store the certificate
// bAuthCert - 1=auth cert, 0=sign cert
// returns error coder or ERR_OK
//--------------------------------------------------
EXP_OPTION int ddocMsspReadCertificate(const char* szPhoneNo, const char* szCertUrl, 
				       X509 **ppCert, int bAuthCert);

//--------------------------------------------------
// Rerieves SOAP fault string
// pMssp - pointer to MSSP context structure
// returns SOAP fault string or NULL
//--------------------------------------------------
EXP_OPTION char* ddocGetSoapFaultString(MSSP* pMssp);

//--------------------------------------------------
// Rerieves SOAP fault code
// pMssp - pointer to MSSP context structure
// returns SOAP fault code or NULL
//--------------------------------------------------
EXP_OPTION char* ddocGetSoapFaultCode(MSSP* pMssp);

//--------------------------------------------------
// Rerieves SOAP fault detail
// pMssp - pointer to MSSP context structure
// returns SOAP fault detail or NULL
//--------------------------------------------------
EXP_OPTION char* ddocGetSoapFaultDetail(MSSP* pMssp);



#ifdef  __cplusplus
}
#endif

#endif // __DIGIDOC_MSSP_GW_H__

