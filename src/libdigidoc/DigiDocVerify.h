#ifndef __DIGIDOC_VERIFY_H__
#define __DIGIDOC_VERIFY_H__
//==================================================
// FILE:	DigiDocVerify.h
// PROJECT:     Digi Doc
// DESCRIPTION: DigiDoc verification routines
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
//      26.04.2006      Veiko Sinivee
//                      Creation
//==================================================

#include <libdigidoc/DigiDocDefs.h>
#include <libdigidoc/DigiDocObj.h>
#include <libdigidoc/DigiDocMem.h>
#include <openssl/x509.h>
#include <openssl/ocsp.h>

//==========< XML generation routines >========================

#ifdef  __cplusplus
extern "C" {
#endif

// structure used to report paths of data files
typedef struct FilePath_st {
	char* szFileName;
	char* szPath;
} FilePath;

//--------------------------------------------------
// Frees file paths array
// pPaths - FilePaths array
// nPaths - number of paths
//--------------------------------------------------
EXP_OPTION void FilePaths_free(FilePath* pPaths, int nPaths);

//--------------------------------------------------
// Finds all detatched files and returns their
// names. The user can now set the path portion.
// Allocaes new memory that caller must free 
// with FilePaths_free()
// pSigDoc - signed document
// pPaths - pointer to FilePaths array
// nPaths - pointer to number of paths
//--------------------------------------------------
EXP_OPTION int findDetatchedDataFiles(SignedDoc* pSigDoc, FilePath** pPaths, int* nPaths);

// verifies files signature
EXP_OPTION int verifyFileSignature(const char* szFileName, int nDigestType,
						byte* pSigBuf, int nSigLen,
						const char *certfile);
EXP_OPTION int verifySignature(const char* szData, unsigned long dataLen, int nDigestType,
          byte* pSigBuf, int nSigLen, X509* cert);

// Compares two byte arrays and returns 0 for OK
EXP_OPTION int compareByteArrays(const byte* dig1, int len1, const byte* dig2, int len2);

// verifies one doc's check digests in this signature
EXP_OPTION int verifySigDocDigest(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo, 
				 const DocInfo* pDocInfo, const char* szFileName, 
				 const char* szDataFile);
// verifies the mime digest of this doc in this signature
EXP_OPTION int verifySigDocMimeDigest(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo, 
				       const DocInfo* pDocInfo, const char* szFileName);

// verifies this one signature
EXP_OPTION int verifySignatureInfo(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo, 
						const char* signerCA, const char* szDataFile, int bUseCA,
						FilePath* pPaths, int nPaths);
// verifies the whole document (returns on first err)
EXP_OPTION int verifySigDoc(const SignedDoc* pSigDoc, const char* signerCA, 
							const char** caFiles, const char* caPath, const char* notCert, 
							const char* szDataFile, int bUseCA,
							FilePath* pPaths, int nPaths);


// Verifies the certificates signed attributes
EXP_OPTION int verifySigCert(const SignatureInfo* pSigInfo);


// Verfies NotaryInfo signature
EXP_OPTION int verifyNotaryInfo(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo,
				const NotaryInfo* pNotInfo,  
				const char ** caFiles, const char *CApath, const char* notCertFile);

// Verifies the certificates signed attributes
EXP_OPTION int verifyNotCert(const SignatureInfo* pSigInfo, const NotaryInfo* pNotInfo);

// Verfies NotaryInfo digest
EXP_OPTION int verifyNotaryDigest(const SignedDoc* pSigDoc, const NotaryInfo* pNotInfo);

// verifies signed doc 
EXP_OPTION int verifySigDocCERT(const SignedDoc* pSigDoc, const void* signerCA, 
				const X509** caCerts, 
				const char* caPath, const X509* notCert, 
				const char* szDataFile, int bUseCA,
				FilePath* pPaths, int nPaths);


// Verifies this signature
  EXP_OPTION int verifySignatureInfoCERT(const SignedDoc* pSigDoc, 
					 const SignatureInfo* pSigInfo, 
					 const void* signerCACert, const char* szDataFile, int bUseCA,
					 FilePath* pPaths, int nPaths);
// Checks if the cert has been signed by this CA-cert
EXP_OPTION int isCertSignedByCERT(const X509* cert, const X509* caCert);


// Verfies NotaryInfo signature
EXP_OPTION int verifyNotaryInfoCERT(const SignedDoc* pSigDoc, 
				    const SignatureInfo* pSigInfo,
				    const NotaryInfo* pNotInfo,  
				    const X509** caCerts, 
				    const char *CApath, const X509* notCert);

EXP_OPTION int verifySigDocSigPropDigest(const SignatureInfo* pSigInfo);

// Calculates the digest of NotaryInfo
EXP_OPTION int calculateNotaryInfoDigest(const SignedDoc* pSigDoc, 
					const NotaryInfo* pNotInfo, byte* digBuf, int* digLen);

int readTagContents(char** data, const char* fileName, 
					const char* tagName, int nAttrs,
					const char** attNames, const char** attValues,
					int withTags);

  X509_ALGOR* setSignAlgorithm(const EVP_MD * type);

int setup_verifyCERT(X509_STORE **newX509_STORE,
		     const char *CApath, 
		     const X509** certs);
		     
EXP_OPTION int verifyEstIDSignature(const byte* digest, int digestLen, int nDigestType,
					byte* pSigBuf, int nSigLen, X509* cert);

#ifdef  __cplusplus
}
#endif

#endif // __DIGIDOC_VERIFY_H__


