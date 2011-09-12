#ifndef __DIGIDOCCSP_H__
#define __DIGIDOCCSP_H__

//==================================================
// FILE:	DigDocCsp.h
// PROJECT: Digi Doc
// DESCRIPTION: CSP Functions 
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
// 1.0	09.05.2002	Veiko Sinivee	
//==================================================


#ifdef  __cplusplus
extern "C" {
#endif

#define EST_EID_CSP "EstEID Card CSP"

typedef struct CSProvider_st {
	char* CSPName;
	int rsa_full; // if FALSE RSA_SIG will be used 
	int at_sig; //// if FALSE AT_KEYEXCHANGE will be used 
} CSProvider;

// general structure for a list of certificates
typedef struct CertItem_st {
	X509* pCert;
	struct CertItem_st* nextItem;
} CertItem;


typedef struct CertSearchStore_st {
	int searchType;
	char* storeName; // default is "My"
	long certSerial;
	int numberOfSubDNCriterias;
	char** subDNCriterias;
	int numberOfIssDNCriterias;
	char** issDNCriterias;
	void* publicKeyInfo;
} CertSearchStore;

typedef struct CertSearch_st {
	int searchType;
	char* x509FileName;
	char* keyFileName; 
	char* pkcs12FileName;
	char * pswd;
	CertSearchStore* certSearchStore;
} CertSearch;
  

//=====================================================================
// Hashes and signes data with EstId card, returns also cert
// which can be used in order to verify signature
// IN dataToBeSigned - source data buffer
// IN dataLen - how many bytes will be read from source buffer
// OUT cert - cert buffer( corresponding private key was used to sign.), migth be NULL if this parameter is not needed.
// OUT certLen - cert length in buffer, migth be NULL if cert parameter is not needed.
// OUT keyBlob - public key's buffer, migth be NULL if this parameter is not needed.
// OUT keyBlobLen - public key's length in buffer, migth be NULL if keyBlob parameter is not needed.
// OUT hash - hash buffer, migth be NULL if this parameter is not needed.
// OUT hashLen - hash length in buffer, migth be NULL if hash parameter is not needed.
// OUT sign - output data buffer for hashed and signed data
// OUT sigLen - data length in output buffer
//=====================================================================
int GetSignParametersWithEstIdCSP(byte * dataToBeSigned,unsigned long dataLen,
									  X509 **x509, int *needCert, 
									  byte *keyBlob, unsigned long *keyBlobLen,
									  byte *hash, unsigned long *hashLen,
									  byte *sign,unsigned long *sigLen);



EXP_OPTION int calculateSigInfoSignatureWithEstID(SignedDoc* pSigDoc, SignatureInfo* pSigInfo, 
                    int slot, const char* passwd);
//Added parameter iByKeyContainer by A.Amenberg 06062003
EXP_OPTION int calculateSigInfoSignatureWithCSPEstID(SignedDoc* pSigDoc, SignatureInfo* pSigInfo, int iByKeyContainer);

EXP_OPTION X509 * findIssuerCertificatefromMsStore(X509 *x509);


EXP_OPTION CertSearchStore* CertSearchStore_new();
EXP_OPTION void CertSearchStore_free(CertSearchStore* certSearchStore);

EXP_OPTION CertSearch* CertSearch_new();
EXP_OPTION void CertSearch_free(CertSearch* certSearch);
EXP_OPTION void CertList_free(CertItem* pListStart);
EXP_OPTION void CertSearch_setX509FileName(CertSearch* certSearch, const char* str);
EXP_OPTION void CertSearch_setKeyFileName(CertSearch* certSearch, const char* str);
EXP_OPTION void CertSearch_setPkcs12FileName(CertSearch* certSearch, const char* str);
EXP_OPTION void CertSearch_setPasswd(CertSearch* certSearch, const char* str);


//
EXP_OPTION CSProvider * getCurrentCSProvider(BOOL tryToFindIfMissing);
EXP_OPTION X509* findCertificate(const CertSearch * cS);
EXP_OPTION int findAllCertificates(const CertSearchStore *sS, X509 ***certsArray, int *numberOfCerts);

EXP_OPTION int Digi_readCertificateByPKCS12OnlyCertHandle(const char *pkcs12file, const char * passwd, X509 **x509);
EXP_OPTION int Digi_getConfirmationWithCertSearch(SignedDoc* pSigDoc, SignatureInfo* pSigInfo, char* pkcs12File, char* password,
                            char* notaryURL, char* proxyHost, char* proxyPort);
EXP_OPTION int Digi_setNotaryCertificate(NotaryInfo* pNotary, X509* notCert);
EXP_OPTION int Digi_verifyNotaryInfoWithCertSearch(const SignedDoc* pSigDoc, const NotaryInfo* pNotInfo);

// verifies this one signature
EXP_OPTION int Digi_verifySignatureInfo(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo, 
						const char* szDataFile, FilePath* pPaths, int nPaths);
// verifies the whole document (returns on first err)
EXP_OPTION int Digi_verifySigDoc(const SignedDoc* pSigDoc, const char* szDataFile, 
							FilePath* pPaths, int nPaths);
EXP_OPTION int Digi_verifySigDocWithCertSearch(const SignedDoc* pSigDoc, const char* szDataFile, FilePath* pPaths, int nPaths);

#ifdef  __cplusplus
}
#endif

#endif