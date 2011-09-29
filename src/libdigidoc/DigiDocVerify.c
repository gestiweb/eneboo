//==================================================
// FILE:	DigiDocVerify.c
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

#include <libdigidoc/DigiDocVerify.h>
#include <libdigidoc/DigiDocError.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocConvert.h>
//#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocCert.h>
#include <libdigidoc/DigiDocGen.h>


#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/ocsp.h>
#include <openssl/pkcs12.h>
#include <openssl/rand.h>


//--------------------------------------------------
// Verifies files SHA1-RSA signature
// szFileName - file name
// nDigestType - digest type. Supports only SHA1 (0)
// pSigBuf - buffer to store the signature
// nSigLen - buffer size, must be at least 128
//			will be updated by actual signature length
// certfile - name of the certificate file
// returns error code or ERR_OK for success
//--------------------------------------------------
EXP_OPTION int verifyFileSignature(const char* szFileName, int nDigestType,
						byte* pSigBuf, int nSigLen,
						const char *certfile)
{
  int err = ERR_OK;
  EVP_MD_CTX  ctx;
  unsigned char buf[FILE_BUFSIZE];
  int i;
  FILE *f;
  EVP_PKEY* pkey = NULL;

  RETURN_IF_NULL_PARAM(szFileName);
  RETURN_IF_NULL_PARAM(pSigBuf);
  RETURN_IF_NULL_PARAM(certfile);
  
  if(nDigestType == DIGEST_SHA1) {
    if((err = ReadPublicKey(&pkey, certfile)) == ERR_OK) {
      if((f = fopen(szFileName,"rb")) != NULL) {
	EVP_VerifyInit(&ctx, EVP_sha1());
	for (;;) {
	  i = fread(buf, sizeof(char), FILE_BUFSIZE, f);
	  if (i <= 0) break;
	  EVP_VerifyUpdate (&ctx, buf, (unsigned long)i);
	}
	err = EVP_VerifyFinal(&ctx, pSigBuf, nSigLen, pkey);
	if(err == ERR_LIB_NONE)
	  err = ERR_OK;
	fclose(f);
	EVP_PKEY_free(pkey);								
      } // if - fopen
      else
	err = ERR_FILE_READ;
    }
    else
      err = ERR_CERT_READ;
  }
  else
    err = ERR_UNSUPPORTED_DIGEST;
  
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}


//--------------------------------------------------
// Verifies files SHA1-RSA signature
// szData - input data
// dataLen - input data length
// nDigestType - digest type
// pSigBuf - buffer to store the signature
// nSigLen - buffer size, must be at least 128
//			will be updated by actual signature length
// cert - certificate data
// returns error code or ERR_OK for success
//--------------------------------------------------
EXP_OPTION int verifySignature(const char* szData, unsigned long dataLen, int nDigestType,
					byte* pSigBuf, int nSigLen, X509* cert)
{
  int err = ERR_OK;
  EVP_MD_CTX  ctx;
  EVP_PKEY* pkey = NULL;
  
  RETURN_IF_NULL_PARAM(szData);
  RETURN_IF_NULL_PARAM(pSigBuf);
  RETURN_IF_NULL_PARAM(cert);
  
  if(nDigestType == DIGEST_SHA1) {
    if((err = GetPublicKey(&pkey, cert)) == ERR_OK) {
      checkErrors();
      EVP_VerifyInit(&ctx, EVP_sha1());
      checkErrors();
      EVP_VerifyUpdate (&ctx, szData, dataLen);
      checkErrors();
      err = EVP_VerifyFinal(&ctx, pSigBuf, nSigLen, pkey);
      if(err == ERR_LIB_NONE)
	err = ERR_OK;
      checkErrors();
      EVP_PKEY_free(pkey);
      checkErrors();
    }
    else
      err = ERR_CERT_READ;
  }
  else
    err = ERR_UNSUPPORTED_DIGEST;	
  
  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}

//--------------------------------------------------
// Verifies files SHA1-RSA signature (EstID specific!!!)
// digest - digest data
// dataLen - digest data length
// nDigestType - digest type
// pSigBuf - buffer to store the signature
// nSigLen - buffer size, must be at least 128
//			will be updated by actual signature length
// cert - certificate data
// returns error code or ERR_OK for success
//--------------------------------------------------
EXP_OPTION int verifyEstIDSignature(const byte* digest, int digestLen, int nDigestType,
					byte* pSigBuf, int nSigLen, X509* cert)
{
  int err = ERR_OK;
  EVP_PKEY* pkey = 0;
  byte buf2[DIGEST_LEN+2], buf3[200], buf4[50], buf5[50];
  int l2 = 0, l1;

  RETURN_IF_NULL_PARAM(digest);
  RETURN_IF_NULL_PARAM(pSigBuf);
  RETURN_IF_NULL_PARAM(cert);
  if(nDigestType == DIGEST_SHA1) {
    if((err = GetPublicKey(&pkey, cert)) == ERR_OK) {
      l2 = sizeof(buf3);
      memset(buf3, 0, sizeof(buf3));
      ERR_clear_error();
      //swapBytes(pSigBuf, nSigLen);
      l2 = RSA_public_decrypt(nSigLen, pSigBuf, buf3, pkey->pkey.rsa, RSA_PKCS1_PADDING); //RSA_PKCS1_PADDING); //RSA_NO_PADDING);
      ddocDebug(3, "verifyEstIDSignature", "decryted sig-hash len: %d", l2);
	  // debug info
      l1 = sizeof(buf4);
	  if(digestLen > 0) {
      memset(buf4, 0, sizeof(buf4));
	  encode((const byte*)digest, digestLen, (byte*)buf4, &l1);
      ddocDebug(3, "verifyEstIDSignature", "calculated hash: %s len: %d", buf4, digestLen);
	  }
      l1 = sizeof(buf4);
	  if(l2 > 0) {
      memset(buf4, 0, sizeof(buf4));
	  encode((const byte*)buf3, l2, (byte*)buf4, &l1);
      ddocDebug(3, "verifyEstIDSignature", "decrypted hash: %s len: %d", buf4, l2);
	  }
      memset(buf2, 0, DIGEST_LEN);
      if(l2 > DIGEST_LEN)
		memcpy(buf2, buf3 + l2 - DIGEST_LEN, DIGEST_LEN);
      else
		memcpy(buf2, buf3, DIGEST_LEN);
      checkErrors();
      err = compareByteArrays(digest, digestLen, buf2, DIGEST_LEN);
      //debug
      l1 = sizeof(buf4);
      encode((const byte*)digest, digestLen, (byte*)buf4, &l1);
      l1 = sizeof(buf5);
      encode((const byte*)buf2, DIGEST_LEN, (byte*)buf5, &l1);
      ddocDebug(3, "verifyEstIDSignature", "comp-hash: %s sig-hash: %s, err: %d", buf4, buf5, err);

      EVP_PKEY_free(pkey);
      checkErrors();
    }
    else
      err = ERR_CERT_READ;
  }
  else
    err = ERR_UNSUPPORTED_DIGEST;	

  if (err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}

//============================================================
// Compares two byte arrays and returns 0 for OK
// dig1 - byte array 1
// len1 - byte array 1 length
// dig2 - byte array 2
// len2 - byte array 2 length
//============================================================
EXP_OPTION int compareByteArrays(const byte* dig1, int len1, const byte* dig2, int len2)
{
	int err = ERR_OK, i;

	RETURN_IF_NULL_PARAM(dig1);
	RETURN_IF_NULL_PARAM(dig2);
	RETURN_IF_NOT(len1 == len2, ERR_COMPARE);	
	for(i = 0; i < len1; i++) {
		if(dig1[i] != dig2[i]) {
			err = ERR_COMPARE;
			break;
		}
	}
	return err;
}

//============================================================
// Verifies the digest of the given doc in this signature
// pSigDoc - signed doc data
// pSigInfo - signature info object
// filename - file name for not embedded files
// szDataFile - name of the digidoc file
//============================================================
// FIXME : Hard to understand the logic
EXP_OPTION int verifySigDocDigest(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo, 
				 const DocInfo* pDocInfo, const char* szFileName, const char* szDataFile)
{
  int err = ERR_OK;
  int l1, l2;
  long l;
  DataFile *pDf = NULL ;
  byte buf1[DIGEST_LEN+2], buf2[50], buf3[50], buf4[50];
  char *attNames = NULL, *attValues = NULL, *pTmp1 = NULL, *pTmp2 = NULL;
  //FILE *hFile;

  RETURN_IF_NULL_PARAM(pSigInfo);
  RETURN_IF_NULL_PARAM(pSigDoc);
  RETURN_IF_NULL_PARAM(pDocInfo);
  pDf = getDataFileWithId(pSigDoc, pDocInfo->szDocId);
  RETURN_IF_NULL(pDf);
  RETURN_IF_NULL(pDf->szContentType);
  RETURN_IF_NULL(pDf->szDigestType);
  RETURN_IF_NULL(pDocInfo->szDigestType);
  // verify detached file signature
  ddocDebug(3, "verifySigDocDigest", "SigDoc: %s DF: %s len1: %d len2: %d, ctype: %s", 
	    pSigDoc->szFormatVer, pDf->szId, pDf->mbufDigest.nLen, 
	    pDocInfo->nDigestLen, pDf->szContentType);
  if(!strcmp(pDf->szContentType, CONTENT_HASHCODE)) {
    ddocDebug(3, "verifySigDocDigest", "DF: %s ContentType: %s len1: %d len2: %d, type1: %s type2: %s", 
	      pDf->szId, pDf->szContentType, 
	      pDf->mbufDetachedDigest.nLen, pDocInfo->nDigestLen, 
	      pDocInfo->szDigestType, pDf->szDigestType);
    if(strcmp(pDocInfo->szDigestType, pDf->szDigestType))
      err = ERR_DOC_DIGEST;
    else
      err = compareByteArrays(pDocInfo->szDigest, pDocInfo->nDigestLen, 
			      (byte*)pDf->mbufDetachedDigest.pMem, pDf->mbufDetachedDigest.nLen);
    if(err != ERR_OK)
      err = ERR_DOC_DIGEST;
  }
  if(!strcmp(pDf->szContentType, CONTENT_DETATCHED)) {
    if(szFileName) {
      l1 = sizeof(buf1);
      err = calculateFileDigest(szFileName, 
				DIGEST_SHA1, buf1, &l1, &l);
      RETURN_IF_NOT(err == ERR_OK, err);
      err = compareByteArrays(buf1, l1, 
	      (byte*)pDf->mbufDetachedDigest.pMem, pDf->mbufDetachedDigest.nLen);
      RETURN_IF_NOT(err == ERR_OK, err);
    } else
      SET_LAST_ERROR_RETURN_CODE(ERR_DOC_DIGEST);
  }
  // the new digest calculation on the fly doesn't
  // work for old 1.0 files
  if(!strcmp(pDf->szContentType, CONTENT_EMBEDDED) &&
     !strcmp(pSigDoc->szFormatVer, SK_XML_1_VER)) {
    attNames = "Id";
    attValues = pDf->szId;
    err = readTagContents(&pTmp2, szDataFile, "DataFile", 1, 
			  (const char**)&attNames, (const char**)&attValues, 0);
    if(err == ERR_OK) {
      pTmp1 = pTmp2;
      //skip leading newlines
      while(*pTmp1 && *pTmp1 != '<') pTmp1++;
      l1 = sizeof(buf1);
      err = calculateDigest((const byte*)pTmp1, strlen(pTmp1),
			    DIGEST_SHA1, (byte*)buf1, &l1);	
      if(err == ERR_OK) {
	err = ddocDataFile_SetDigestValue(pDf, (const char*)buf1, DIGEST_LEN);
	encode((const byte*)pDf->mbufDigest.pMem, pDf->mbufDigest.nLen, (byte*)buf3, &l2);
	ddocDebug(3, "verifySigDocDigest", "DF: %s calculated digest: %s", 
	      pDf->szId, buf3);
      }
      free(pTmp2);
    }
  }
  if(!strcmp(pDf->szContentType, CONTENT_EMBEDDED) ||
     !strcmp(pDf->szContentType, CONTENT_EMBEDDED_BASE64)) {
    buf2[0] = buf3[0] = buf4[0] = 0;
    l2 = sizeof(buf2);
    if(pDocInfo->szDigest)
      bin2hex((const byte*)pDocInfo->szDigest, pDocInfo->nDigestLen, (char*)buf2, &l2);
    l2 = sizeof(buf3);
    if(pDf->mbufDigest.pMem)
      bin2hex((const byte*)pDf->mbufDigest.pMem, pDf->mbufDigest.nLen, (char*)buf3, &l2);
    l2 = sizeof(buf4);
    if(pDf->mbufWrongDigest.pMem)
      bin2hex((const byte*)pDf->mbufWrongDigest.pMem, pDf->mbufWrongDigest.nLen, (char*)buf4, &l2);

    ddocDebug(3, "verifySigDocDigest", "DF: %s len1: %d len2: %d, type1: %s type2: %s, digest1: %s digest2: %s digest3: %s", 
	      pDf->szId, pDf->mbufDigest.nLen, pDocInfo->nDigestLen, pDocInfo->szDigestType, pDf->szDigestType, buf2, buf3, buf4);
    if(strcmp(pDocInfo->szDigestType, pDf->szDigestType))
      err = ERR_DOC_DIGEST;
    else
      err = compareByteArrays(pDocInfo->szDigest, pDocInfo->nDigestLen, 
			      (byte*)pDf->mbufDigest.pMem, pDf->mbufDigest.nLen);
    if(err) { // check also the wrong digest
      err = compareByteArrays(pDocInfo->szDigest, pDocInfo->nDigestLen, 
			      (byte*)pDf->mbufWrongDigest.pMem, pDf->mbufWrongDigest.nLen);
    }
    if(err != ERR_OK)
      err = ERR_DOC_DIGEST;
  }
  if (err != ERR_OK) SET_LAST_ERROR(err);
  ddocDebug(3, "verifySigDocDigest", "SigDoc DF: %s err: %d", 
	   pDf->szId, err);
  
  return err;
}

//============================================================
// Verifies the mime digest of the given doc in this signature
// pSigDoc - signed doc data
// pSigInfo - signature info object
// filename - file name for not embedded files
//============================================================
EXP_OPTION int verifySigDocMimeDigest(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo, 
				      const DocInfo* pDocInfo, const char* szFileName)
{
	int err = ERR_OK;
	int l1;
	DataFile* pDf;
	byte buf1[DIGEST_LEN+2];

	RETURN_IF_NULL_PARAM(pSigInfo);
	RETURN_IF_NULL_PARAM(pSigDoc);
	RETURN_IF_NULL_PARAM(pDocInfo);
	pDf = getDataFileWithId(pSigDoc, pDocInfo->szDocId);
	RETURN_IF_NULL(pDf);
	// we check mime digest only in ver 1.0
	if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER)) {
		l1 = sizeof(buf1);
		err = calculateDigest((const byte*)pDf->szMimeType, strlen(pDf->szMimeType), 
				DIGEST_SHA1, buf1, &l1);
		RETURN_IF_NOT(err == ERR_OK, err);
		err = compareByteArrays(pDocInfo->szMimeDigest, pDocInfo->nMimeDigestLen, 
				buf1, l1);
		if(err != ERR_OK)
			err = ERR_MIME_DIGEST;
	}
	if (err != ERR_OK) SET_LAST_ERROR(err);
	return err;
}


//============================================================
// Verifies the SignedProperties digest
// pSigInfo - signature info object
// from original file and use it for hash function.
// This is usefull if the file has been generated by
// another library and possibly formats these elements
// differently.
//============================================================
EXP_OPTION int verifySigDocSigPropDigest(const SignatureInfo* pSigInfo)
{
	int err = ERR_OK;

	RETURN_IF_NULL_PARAM(pSigInfo);
	err = ddocCompareDigestValues(pSigInfo->pSigPropDigest, pSigInfo->pSigPropRealDigest);
	RETURN_IF_NOT(err == ERR_OK, ERR_SIGPROP_DIGEST);
	return ERR_OK;
}

//============================================================
// Verifies the certificates signed attributes
// pSigInfo - signature info object
//============================================================
EXP_OPTION int verifySigCert(const SignatureInfo* pSigInfo)
{
  int err = ERR_OK;
  int l1, l2;
  char szOtherSerial[100];
  byte buf1[DIGEST_LEN+2], buf2[DIGEST_LEN*2], buf3[DIGEST_LEN*2];
  DigiDocMemBuf* pMBuf;

  RETURN_IF_NULL_PARAM(pSigInfo);
  RETURN_IF_NULL_PARAM(ddocSigInfo_GetSignersCert(pSigInfo));
  l1 = sizeof(buf1);
  RETURN_IF_NOT(X509_digest(ddocSigInfo_GetSignersCert(pSigInfo), 
		EVP_sha1(), buf1, (unsigned int*)&l1), ERR_X509_DIGEST); 
  // debug
  memset(buf2, 0, sizeof(buf2));
  memset(buf3, 0, sizeof(buf3));
  pMBuf = ddocSigInfo_GetSignersCert_DigestValue(pSigInfo);
  RETURN_IF_NULL_PARAM(pMBuf);
  l2 = sizeof(buf2);
  encode((const byte*)pMBuf->pMem, pMBuf->nLen, (byte*)buf2, &l2);
  l2 = sizeof(buf3);
  encode((const byte*)buf1, l1, (byte*)buf3, &l2);
  ddocDebug(3, "verifySigCert", "SIG: %s cdig1: %d - %s cdig2: %d - %s", 
	    pSigInfo->szId, pMBuf->nLen, buf2, l1, buf3);
  err = compareByteArrays((const byte*)pMBuf->pMem, pMBuf->nLen, buf1, l1);	
  RETURN_IF_NOT(err == ERR_OK, ERR_WRONG_CERT);
  err = ReadCertSerialNumber(szOtherSerial, sizeof(szOtherSerial), ddocSigInfo_GetSignersCert(pSigInfo));
  RETURN_IF_NOT(((err == ERR_OK) && 
    !strcmp(ddocSigInfo_GetSignersCert_IssuerSerial(pSigInfo), szOtherSerial)), ERR_WRONG_CERT);
  return err;
}


//============================================================
// Verifies this signature
// pSigDoc - signed doc data
// pSigInfo - signature info object
// signerCA - direct signer CA certs filename
// szDateFile - name of the digidoc file
// bUseCA - use CA certs or not 1/0
// pPaths - file name / path pairs for detatched files
// nPaths - number of file name / path pairs
// from original file and use it for hash function.
// This is usefull if the file has been generated by
// another library and possibly formats these elements
// differently.
//============================================================
EXP_OPTION int verifySignatureInfo(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo, 
				   const char* signerCA, const char* szDataFile, int bUseCA,
				   FilePath* pPaths, int nPaths)
{
  int err = ERR_OK;
  int j, k, i;
  char *pFileName = NULL;
  X509* cert = NULL;
  DocInfo* pDocInfo = NULL;
  DataFile* pDf = NULL;
  DigiDocMemBuf *pMBuf1 = 0, *pMBuf2 = 0;

  RETURN_IF_NULL_PARAM(pSigInfo);
  clearErrors();
  pMBuf1 = ddocDigestValue_GetDigestValue(pSigInfo->pSigInfoRealDigest);
  RETURN_IF_NULL_PARAM(pMBuf1);
  pMBuf2 = ddocSigInfo_GetSignatureValue_Value((SignatureInfo*)pSigInfo);
  RETURN_IF_NULL_PARAM(pMBuf2);
  err = verifyEstIDSignature((const byte*)pMBuf1->pMem, pMBuf1->nLen, DIGEST_SHA1,
			     (byte*)pMBuf2->pMem, pMBuf2->nLen, ddocSigInfo_GetSignersCert(pSigInfo));
  if(err != ERR_OK)
    err = ERR_SIGNATURE;
  if(err == ERR_OK) {
    k = getCountOfDocInfos(pSigInfo);
    ddocDebug(4, "verifySignatureInfo", "DFs: %d", k);
    for(j = 0; (err == ERR_OK) && (j < k); j++) {
      pDocInfo = getDocInfo(pSigInfo, j);
      RETURN_IF_NULL(pDocInfo);
      ddocDebug(4, "verifySignatureInfo", "DocInfo: %s", pDocInfo->szDocId);      
      pDf = getDataFileWithId(pSigDoc, pDocInfo->szDocId);
      RETURN_IF_NULL(pDf);
      ddocDebug(4, "verifySignatureInfo", "DF: %s", pDf->szId);      
      // find data file path
      for(i = 0; i < nPaths; i++) {
	if(!strcmp(pDf->szFileName, pPaths->szFileName)) {
	  pFileName = pPaths->szPath;
	  break;
	}
      }
      err = verifySigDocDigest(pSigDoc, pSigInfo, pDocInfo, pFileName, szDataFile);
      ddocDebug(4, "verifySignatureInfo", "DF: %s verify: %d", pDf->szId, err);  
      if(err == ERR_OK)
	err = verifySigDocMimeDigest(pSigDoc, pSigInfo, pDocInfo, NULL);			
    }
  }
  if(err == ERR_OK)
    err = verifySigDocSigPropDigest(pSigInfo);
  if(err == ERR_OK)
    err = verifySigCert(pSigInfo);
  if(err == ERR_OK) {
    cert = getSignCertData(pSigInfo);
    // VS: ver 2.2.4 - removed this check as OCSP check is sufficient
    //if(err == ERR_OK) 
    //  err = isCertValid(cert, convertStringToTimeT(pSigDoc, pSigInfo->szTimeStamp));		
    if(bUseCA && (err == ERR_OK))
      err = isCertSignedBy(cert, signerCA);
  }
  if ( err != ERR_OK) SET_LAST_ERROR(err);
  return err;
}


//============================================================
// Verifies the whole document, but returns on first error
// Use the functions defined earlier to verify all contents
// step by step.
// pSigDoc - signed doc data
// 
//============================================================
EXP_OPTION int verifySigDoc(const SignedDoc* pSigDoc, const char* signerCA, 
				 const char** caFiles, const char* caPath, const char* notCert, 
			     const char* szDataFile, int bUseCA, FilePath* pPaths, int nPaths)

{
  SignatureInfo* pSigInfo;
  int i, d, err = ERR_OK;
	
  RETURN_IF_NULL_PARAM(pSigDoc);
  d = getCountOfSignatures(pSigDoc);
  for(i = 0; i < d; i++) {
    pSigInfo = getSignature(pSigDoc, i);
    err = verifySignatureInfo(pSigDoc, pSigInfo, signerCA, 
			      szDataFile, bUseCA, pPaths, nPaths);
    RETURN_IF_NOT(err == ERR_OK, err);
    err = verifyNotaryInfo(pSigDoc, pSigInfo, pSigInfo->pNotary, caFiles, caPath, notCert);
    RETURN_IF_NOT(err == ERR_OK, err);
  }
  return err;
}

//============================================================
// Verifies the certificates signed attributes
// pNotInfo - notary info object
//============================================================
EXP_OPTION int verifyNotCert(const SignatureInfo* pSigInfo, const NotaryInfo* pNotInfo)
{
  int err = ERR_OK;
  int l1;
  char szOtherSerial[100];
  byte buf1[DIGEST_LEN+2];
  CertID* pCertID;
  DigiDocMemBuf* pMBuf;
  X509* pCert = 0;

  RETURN_IF_NULL_PARAM(pSigInfo);
  RETURN_IF_NULL_PARAM(pNotInfo);
  pCertID = ddocSigInfo_GetCertIDOfType((SignatureInfo*)pSigInfo, CERTID_TYPE_RESPONDERS_CERTID);
  RETURN_IF_NULL(pCertID);
  pMBuf = ddocCertID_GetDigestValue(pCertID);
  RETURN_IF_NULL(pMBuf);

  l1 = sizeof(buf1);
  pCert = ddocSigInfo_GetOCSPRespondersCert(pSigInfo);
  RETURN_IF_NOT(pCert, ERR_WRONG_CERT);
  RETURN_IF_NOT(X509_digest(pCert, EVP_sha1(), buf1, (unsigned int*)&l1), ERR_X509_DIGEST); 
  err = compareByteArrays((const byte*)pMBuf->pMem, pMBuf->nLen, buf1, l1);
  RETURN_IF_NOT(err == ERR_OK, ERR_WRONG_CERT);
  err = ReadCertSerialNumber(szOtherSerial, sizeof(szOtherSerial), ddocSigInfo_GetOCSPRespondersCert(pSigInfo));
  RETURN_IF_NOT(err == ERR_OK, err);
  RETURN_IF_NOT(!strcmp(ddocCertID_GetIssuerSerial(pCertID), szOtherSerial), ERR_WRONG_CERT);
  return ERR_OK;
}

//--------------------------------------------------
// Sets digest algorithm type object
// type - SHA1
//--------------------------------------------------
X509_ALGOR* setCIDAlgorithm(const EVP_MD * type)
{
	X509_ALGOR* alg = NULL;
	int nid;

	alg = X509_ALGOR_new();
	RETURN_OBJ_IF_NULL(alg, 0);
	if((alg->parameter == NULL) || 
		(alg->parameter->type != V_ASN1_NULL)) {
		ASN1_TYPE_free(alg->parameter);
		alg->parameter=ASN1_TYPE_new();
		RETURN_OBJ_IF_NULL(alg->parameter, NULL);
		alg->parameter->type=V_ASN1_NULL;
	}
	ASN1_OBJECT_free(alg->algorithm);
	if ((nid = EVP_MD_type(type)) != NID_undef) {
		alg->algorithm=OBJ_nid2obj(nid);
	}
	return alg;
}

//--------------------------------------------------
// Sets signature algorithm type object
// type - RSA+SHA1
//--------------------------------------------------
X509_ALGOR* setSignAlgorithm(const EVP_MD * type)
{
	X509_ALGOR* alg;
	//int nid;

	alg = X509_ALGOR_new();
	RETURN_OBJ_IF_NULL(alg, NULL);
	if((alg->parameter == NULL) || 
		(alg->parameter->type != V_ASN1_NULL)) {
		ASN1_TYPE_free(alg->parameter);
		alg->parameter=ASN1_TYPE_new();
		RETURN_OBJ_IF_NULL(alg->parameter, 0);
		alg->parameter->type=V_ASN1_NULL;
	}
	ASN1_OBJECT_free(alg->algorithm);
	/*if ((nid = EVP_MD_type(type)) != NID_undef) {
		alg->algorithm=OBJ_nid2obj(nid);
	}*/
	alg->algorithm = OBJ_nid2obj(type->pkey_type);
	return alg;
}

//--------------------------------------------------
// Helper function. Converts Notary info to an OCSP
// response structure. Used in verify and file writing
// functions
// pNotInfo - NotaryInfo object
// notCert - OCSP responder certificate
// pBasResp - pointer to a pointer of the new response structure
//--------------------------------------------------
int notary2ocspBasResp(const SignedDoc* pSigDoc, const NotaryInfo* pNotInfo, X509* notCert, OCSP_BASICRESP** pBasResp)
{
  OCSP_SINGLERESP * single = 0;
  SignatureInfo* pSigInfo;
  CertID* pCertID;
  CertValue* pCertVal;
  // ASN1_GENERALIZEDTIME *tp = NULL;
  int err = ERR_OK;
  const DigiDocMemBuf *pMBuf;
  DigiDocMemBuf mbuf1;
  const char *p1 = NULL;

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  RETURN_IF_NULL_PARAM(notCert);
  // new basic response
  *pBasResp = OCSP_BASICRESP_new();
  RETURN_IF_NULL(*pBasResp);
  str2asn1time(pSigDoc, pNotInfo->timeProduced, (*pBasResp)->tbsResponseData->producedAt); 
  p1 = ddocNotInfo_GetResponderId_Type(pNotInfo);
  RETURN_IF_NULL(p1);
  if(!strcmp(p1, RESPID_NAME_VALUE)) {
    (*pBasResp)->tbsResponseData->responderId->type = V_OCSP_RESPID_NAME;
    (*pBasResp)->tbsResponseData->responderId->value.byName = 
      X509_NAME_dup(X509_get_subject_name(notCert));
  } else {
    (*pBasResp)->tbsResponseData->responderId->type = V_OCSP_RESPID_KEY;
    (*pBasResp)->tbsResponseData->responderId->value.byKey = 
      ASN1_OCTET_STRING_new();
    pMBuf = ddocNotInfo_GetResponderId(pNotInfo);
    RETURN_IF_NULL(pMBuf);
    ASN1_OCTET_STRING_set((*pBasResp)->tbsResponseData->responderId->value.byKey,
			  (unsigned char*)pMBuf->pMem, pMBuf->nLen);
  }
  // new single response
  single = OCSP_SINGLERESP_new();
  single->certStatus->type = V_OCSP_CERTSTATUS_GOOD;
  single->certStatus->value.good = ASN1_NULL_new();
  single->certId->hashAlgorithm = setCIDAlgorithm(EVP_sha1());
  err = ddocNotInfo_GetIssuerNameHash(pNotInfo, &mbuf1);
  ASN1_OCTET_STRING_set(single->certId->issuerNameHash, (char*)mbuf1.pMem, mbuf1.nLen);
  ddocMemBuf_free(&mbuf1);
  err = ddocNotInfo_GetIssuerKeyHash(pNotInfo, &mbuf1);
  ASN1_OCTET_STRING_set(single->certId->issuerKeyHash, (char*)mbuf1.pMem, mbuf1.nLen);
  ddocMemBuf_free(&mbuf1);
  pSigInfo = ddocGetSignatureForNotary(pSigDoc, pNotInfo);
  RETURN_IF_NULL(pSigInfo);
  pCertID = ddocSigInfo_GetCertIDOfType(pSigInfo, CERTID_TYPE_RESPONDERS_CERTID);
  if(pCertID) {
	  ddocMemAppendData(&mbuf1, ddocCertID_GetIssuerSerial(pCertID), -1);
  } else  {
	pCertVal = ddocCertValueList_GetCertValueOfType(pSigInfo->pCertValues, CERTID_VALUE_RESPONDERS_CERT);
	if(pCertVal) {
		ddocMemSetLength(&mbuf1, 100);
		ReadCertSerialNumber((char*)mbuf1.pMem, mbuf1.nLen-1, pCertVal->pCert);
	}
  }
  ASN1_INTEGER_set(single->certId->serialNumber, atol((const char*)mbuf1.pMem));
  ddocMemBuf_free(&mbuf1);
  err = ddocNotInfo_GetThisUpdate(pNotInfo, &mbuf1);
  if(mbuf1.pMem && strlen((char*)mbuf1.pMem))
     str2asn1time(pSigDoc, (char*)mbuf1.pMem, single->thisUpdate);
  ddocMemBuf_free(&mbuf1);
  err = ddocNotInfo_GetNextUpdate(pNotInfo, &mbuf1);
  if(mbuf1.pMem && strlen((char*)mbuf1.pMem))
    str2asn1time(pSigDoc, (char*)mbuf1.pMem, single->nextUpdate);
  ddocMemBuf_free(&mbuf1);
  sk_OCSP_SINGLERESP_push((*pBasResp)->tbsResponseData->responses, single);
  // add nonce
  err = ddocNotInfo_GetOcspRealDigest(pNotInfo, &mbuf1);
  if(!err)
    err = OCSP_basic_add1_nonce((*pBasResp), (char*)mbuf1.pMem, mbuf1.nLen);
  ddocMemBuf_free(&mbuf1);
  if (err == ERR_LIB_NONE){
    err = ERR_OK;
    // set signature 
    (*pBasResp)->signatureAlgorithm = setSignAlgorithm(EVP_sha1()); 
    err = ddocNotInfo_GetOcspSignatureValue(pNotInfo, &mbuf1);
    ASN1_OCTET_STRING_set((*pBasResp)->signature, (byte*)mbuf1.pMem, mbuf1.nLen);
    ddocMemBuf_free(&mbuf1);
  } else {
    OCSP_BASICRESP_free(*pBasResp);
    SET_LAST_ERROR_RETURN_CODE(ERR_OCSP_NO_NONCE);
  }
  //	checkErrors();
  return ERR_OK;
}

//--------------------------------------------------
// Verfies NotaryInfo signature
// pSigDoc - signed doc object
// pNotInfo - NotaryInfo object
// caFiles - array of CA file names terminated with NULL
// CApath - path to (directory) all certs
// notCertFile - Notary (e.g. OCSP responder) cert file 
//--------------------------------------------------
EXP_OPTION int verifyNotaryInfo(const SignedDoc* pSigDoc, 
				const SignatureInfo* pSigInfo,
				const NotaryInfo* pNotInfo, 
				// VS - ver 1.66
				const char** caFiles, const char *CApath, 
				const char* notCertFile)
{
  X509** caCerts;
  X509* notCert = NULL;
  int err = ERR_OK, l1, i;
	
  RETURN_IF_NULL_PARAM(caFiles);
  RETURN_IF_NULL_PARAM(CApath);
  RETURN_IF_NULL_PARAM(notCertFile);
  // find the chain length
  // VS - ver 1.67
  for(l1 = 0; caFiles && caFiles[l1]; l1++);
  caCerts = (X509**)malloc(sizeof(void*) * (l1 + 1));
  RETURN_IF_BAD_ALLOC(caCerts);
  memset(caCerts, 0, sizeof(void*) * (l1 + 1));
  for(i = 0; i < l1; i++) {
    err = ReadCertificate(&(caCerts[i]),caFiles[i]);
    if (err != ERR_OK) {
      err = ERR_CERT_READ;
      goto cleanup;
    }
  }
  err = ReadCertificate(&notCert, notCertFile);
  if (err != ERR_OK) {
    err = ERR_CERT_READ;
    goto cleanup;
  }
  err = verifyNotaryInfoCERT(pSigDoc, pSigInfo, pNotInfo,
			     (const X509**)caCerts, CApath, notCert);
  if (err != ERR_OK) SET_LAST_ERROR(err);
  // cleanup
cleanup:
  if(notCert)
    X509_free(notCert);
  for(i = 0; i < l1; i++)
    if(caCerts[i])   
      X509_free(caCerts[i]);
  free(caCerts);
  return err;
}

//--------------------------------------------------
// Setup X509 store for verification purposes
// CApath - directory of all certs
// CA1file - highest root cert
// CA2file - actual parent cert
//--------------------------------------------------
int setup_verifyCERT(X509_STORE **newX509_STORE,
				const char *CApath, 
			     	// VS - ver 1.66
				const X509** certs)
{
  X509_STORE *store;
  X509_LOOKUP *lookup;
  int i;

  if((store = X509_STORE_new()) == NULL) goto end;
  lookup = X509_STORE_add_lookup(store,X509_LOOKUP_file());
  if (lookup == NULL) goto end;
  // VS - ver 1.66
  for(i = 0; certs && certs[i]; i++) 
    X509_STORE_add_cert(store, (X509*)certs[i]);
  
  lookup=X509_STORE_add_lookup(store,X509_LOOKUP_hash_dir());
  if (lookup == NULL) goto end;
  if (CApath) {
    if(!X509_LOOKUP_add_dir(lookup,CApath,X509_FILETYPE_PEM)) {
      //BIO_printf(bp, "Error loading directory %s\n", CApath);
      printf("Error loading directory %s\n", CApath);
      goto end;
    }
  } else X509_LOOKUP_add_dir(lookup,NULL,X509_FILETYPE_DEFAULT);
  *newX509_STORE = store;
  // VS - ver 1.67
  ERR_clear_error();
  return ERR_OK;
end:
  if (store) X509_STORE_free(store);
  SET_LAST_ERROR_RETURN_CODE(ERR_CERT_STORE_READ);
}

//--------------------------------------------------
// Verfies NotaryInfo signature
// pSigDoc - signed doc object
// pNotInfo - NotaryInfo object
// caCerts - CA certificate pointer array terminated with NULL
// CApath - path to (directory) all certs
// notCertFile - Notary (e.g. OCSP responder) cert file 
//--------------------------------------------------
EXP_OPTION int verifyNotaryInfoCERT(const SignedDoc* pSigDoc,
				    const SignatureInfo* pSigInfo,
				    const NotaryInfo* pNotInfo,
				    const X509** caCerts, const char *CApath, 
				    const X509* notCert)
{
  X509_STORE *store;
  OCSP_RESPONSE* pResp = NULL;
  OCSP_BASICRESP* bs = NULL;
  STACK_OF(X509)* ver_certs = NULL;
  int err = ERR_OK, l1;
  byte buf[DIGEST_LEN+2];
  X509* certNotaryDirectCA = 0;
  const DigiDocMemBuf *pMBuf1 = 0;
  DigiDocMemBuf mbuf1;

  mbuf1.pMem = 0;
  mbuf1.nLen = 0;
  RETURN_IF_NULL_PARAM(pSigDoc);
  RETURN_IF_NULL_PARAM(pSigInfo);
  RETURN_IF_NULL_PARAM(pNotInfo);
  RETURN_IF_NULL_PARAM(notCert);
  RETURN_IF_NULL_PARAM(caCerts);
  
  // find the chain length
  for(l1 = 0; caCerts && caCerts[l1]; l1++);
  if(l1 < 1)
    SET_LAST_ERROR_RETURN_CODE(ERR_CERT_INVALID);
  certNotaryDirectCA = (X509*)caCerts[l1-1];	
  // do the signature values match?
  // not to be checked in format 1.4
#ifdef WITH_TS
  if(strcmp(pSigDoc->szFormatVer, DIGIDOC_XML_1_4_VER)) {
#endif
  l1 = sizeof(buf);
  pMBuf1 = ddocSigInfo_GetSignatureValue_Value((SignatureInfo*)pSigInfo);
  RETURN_IF_NULL_PARAM(pMBuf1);
  calculateDigest((byte*)pMBuf1->pMem, pMBuf1->nLen, DIGEST_SHA1, buf, &l1);
  err = ddocNotInfo_GetOcspRealDigest(pNotInfo, &mbuf1);
  if(compareByteArrays(buf, l1, (byte*)mbuf1.pMem, mbuf1.nLen)) {
    SET_LAST_ERROR(ERR_NOTARY_SIG_MATCH);
    err = ERR_NOTARY_SIG_MATCH;
  }
  ddocMemBuf_free(&mbuf1);
#ifdef WITH_TS
  }
#endif
  if(err) return err;
  // now create an OCSP object and check its validity
  // VS - ver 1.66
  pResp = ddocNotInfo_GetOCSPResponse_Value(pNotInfo);
  RETURN_IF_NULL_PARAM(pResp);
  // debug
  //WriteOCSPResponse("test2.resp", pResp);
  if((setup_verifyCERT(&store, CApath, caCerts)) == ERR_OK) {
    // new basic response
    // create OCSP basic response
    // in version 1.0 we calculated digest over tbsResponseData
    bs = OCSP_response_get1_basic(pResp);
    if (!bs) err = ERR_OCSP_WRONG_RESPID;
    if (err == ERR_OK) {
      ver_certs = sk_X509_new_null();
      if (ver_certs) {
	sk_X509_push(ver_certs, notCert);
	err = OCSP_basic_verify(bs, ver_certs, store, OCSP_TRUSTOTHER); //OCSP_NOCHAIN);
	if(err == ERR_LIB_NONE) {
	  err = ERR_OK;
	} else {
	  err = ERR_OCSP_CERT_NOTFOUND;
	  SET_LAST_ERROR(err);
	}
	// cleanup
	sk_X509_free(ver_certs);
      }
      if(bs) OCSP_BASICRESP_free(bs);
    }
    X509_STORE_free(store);
  } else {
    err = ERR_CERT_STORE_READ;
    SET_LAST_ERROR(err);
  }
  if(err == ERR_OK) {
	  if(!notCert) // ???
    notCert = ddocSigInfo_GetOCSPRespondersCert(pSigInfo);
    if(notCert) { // VS: ver 1.66
      err = isCertValid((X509*)notCert, convertStringToTimeT(pSigDoc, pNotInfo->timeProduced));
      if (err != ERR_OK)
	SET_LAST_ERROR(err);
    } else {
      err = ERR_CERT_INVALID;
      SET_LAST_ERROR(err);
    }
    if(err == ERR_OK) {
      err = isCertSignedByCERT(notCert, certNotaryDirectCA);
      if (err != ERR_OK) 
	SET_LAST_ERROR(err);
    }
    if(err == ERR_OK) {
      err = verifyNotCert(pSigInfo, pNotInfo);
      if (err != ERR_OK)
	SET_LAST_ERROR(err);
    }
	if(err == ERR_OK) {
		err = verifyNotaryDigest(pSigDoc, pNotInfo);
		if (err != ERR_OK)
			SET_LAST_ERROR(err);
	}
  }
  if(pResp)
    OCSP_RESPONSE_free(pResp);
  return err;
}


//--------------------------------------------------
// Verfies NotaryInfo digest
// pNotInfo - NotaryInfo object
//--------------------------------------------------
EXP_OPTION int verifyNotaryDigest(const SignedDoc* pSigDoc, const NotaryInfo* pNotInfo)
{
  int err, l1;
  byte buf1[DIGEST_LEN+2];
  const DigiDocMemBuf *pMBuf;

  l1 = sizeof(buf1);
  err = calculateNotaryInfoDigest(pSigDoc, pNotInfo, buf1, &l1);
  RETURN_IF_NOT(err == ERR_OK, err);
  pMBuf = ddocNotInfo_GetOcspDigest(pNotInfo);
  RETURN_IF_NULL(pMBuf);
  err = compareByteArrays(buf1, l1, (byte*)pMBuf->pMem, pMBuf->nLen);
  RETURN_IF_NOT(err == ERR_OK, err);
  return ERR_OK;
}


//============================================================
// Verifies the whole document, but returns on first error
// Use the functions defined earlier to verify all contents
// step by step.
// pSigDoc - signed doc data
// signerCA - direct signer CA certs filename
// szDateFile - name of the digidoc file
// bUseCA - use CA certs or not 1/0
// pPaths - file name / path pairs for detatched files
// nPaths - number of file name / path pairs
//============================================================
EXP_OPTION int verifySigDocCERT(const SignedDoc* pSigDoc, 
								// VS - ver 1.66
				const void* signerCA, const X509** caCerts,
				 const char* caPath, const X509* notCert,
				 const char* szDataFile, int bUseCA, FilePath* pPaths, int nPaths)
{
	SignatureInfo* pSigInfo;
	int i, d, err = ERR_OK;

	RETURN_IF_NULL_PARAM(pSigDoc);
	//assert(pSigDoc);
	d = getCountOfSignatures(pSigDoc);
	for(i = 0; i < d; i++) {
		pSigInfo = getSignature(pSigDoc, i);
		err = verifySignatureInfoCERT(pSigDoc, pSigInfo, signerCA, 
			szDataFile, bUseCA, pPaths, nPaths);
		RETURN_IF_NOT(err == ERR_OK, err);
		err = verifyNotaryInfoCERT(pSigDoc, pSigInfo, pSigInfo->pNotary, caCerts, caPath, notCert);
		RETURN_IF_NOT(err == ERR_OK, err);
	}
	return err;
}

//============================================================
// Verifies this signature
// pSigDoc - signed doc data
// pSigInfo - signature info object
// signerCA - direct signer CA certs filename
// szDataFile - provide to read <SignedInfo> and <SignedProperties>
// from original file and use it for hash function.
// This is usefull if the file has been generated by
// another library and possibly formats these elements
// differently.
// bUseCA - use CA certs or not 1/0
// pPaths - file name / path pairs for detatched files
// nPaths - number of file name / path pairs
//============================================================
EXP_OPTION int verifySignatureInfoCERT(const SignedDoc* pSigDoc, const SignatureInfo* pSigInfo, 
					const void* signerCACert, const char* szDataFile, int bUseCA,
					FilePath* pPaths, int nPaths)
{
	int err = ERR_OK;
	int j, k, i;
	char* pFileName = NULL;
	X509* cert;
	DocInfo* pDocInfo = NULL;
	DataFile* pDf = NULL;
	DigiDocMemBuf *pMBuf1, *pMBuf2;

	RETURN_IF_NULL_PARAM(pSigInfo);
	pMBuf1 = ddocSigInfo_GetSigInfoRealDigest((SignatureInfo*)pSigInfo);
	RETURN_IF_NULL_PARAM(pMBuf1);
	pMBuf2 = ddocSigInfo_GetSignatureValue_Value((SignatureInfo*)pSigInfo);
	RETURN_IF_NULL_PARAM(pMBuf2);
	err = verifyEstIDSignature((const byte*)pMBuf1->pMem, pMBuf1->nLen, DIGEST_SHA1,
				(byte*)pMBuf2->pMem, pMBuf2->nLen, ddocSigInfo_GetSignersCert(pSigInfo));
	RETURN_IF_NOT(err == ERR_OK, ERR_SIGNATURE);
	// check that this signature signs all DataFiles
    for(i = 0; i < getCountOfDataFiles(pSigDoc); i++) {
		pDf = getDataFile(pSigDoc, i);
		ddocDebug(4, "verifySignatureInfoCERT", "Check sig \'%s\' of doc: \'%s\'", pSigInfo->szId, pDf->szId);
		k = 0; // not found yet
		for(j = 0; j < getCountOfDocInfos(pSigInfo); j++) {
			pDocInfo = getDocInfo(pSigInfo, j);
			if(!strcmp(pDocInfo->szDocId, pDf->szId)) {
				k = 1; // found
				break;
			}
		}
		if(!k) {
			ddocDebug(1, "verifySignatureInfoCERT", "Signature \'%s\' does not sign doc: \'%s\'", pSigInfo->szId, pDf->szId);
			err = ERR_DOC_DIGEST;
			SET_LAST_ERROR(err);
			return err;
		}
	}
	// verify DataFile hashes
	k = getCountOfDocInfos(pSigInfo);
	ddocDebug(4, "verifySignatureInfoCERT", "DFs: %d", k);
	for(j = 0; (err == ERR_OK) && (j < k); j++) {
		pDocInfo = getDocInfo(pSigInfo, j);
		ddocDebug(4, "verifySignatureInfoCERT", "Verify doc: %d - \'%s\'", j, pDocInfo->szDocId);
		RETURN_IF_NULL(pDocInfo);
		pDf = getDataFileWithId(pSigDoc, pDocInfo->szDocId);
		RETURN_IF_NULL(pDf);
		// find data file path
		for(i = 0; i < nPaths; i++) {
			if(!strcmp(pDf->szFileName, pPaths->szFileName)) {
				pFileName = pPaths->szPath;
				break;
			}
		}
		if(!pFileName)
		  pFileName = pDf->szFileName;
		err = verifySigDocDigest(pSigDoc, pSigInfo, pDocInfo, pFileName, szDataFile);
		ddocDebug(4, "verifySignatureInfoCERT", "Verify doc: %s - %d", pDocInfo->szDocId, err);
		RETURN_IF_NOT(err == ERR_OK, err);
		err = verifySigDocMimeDigest(pSigDoc, pSigInfo, pDocInfo, NULL);
		RETURN_IF_NOT(err == ERR_OK, err);
	}
		
	err = verifySigDocSigPropDigest(pSigInfo);
	RETURN_IF_NOT(err == ERR_OK, err);
	err = verifySigCert(pSigInfo);
	RETURN_IF_NOT(err == ERR_OK, err);

	cert = getSignCertData(pSigInfo);
	// VS: ver 2.2.4 - removed this check as OCSP check is sufficient
	//err = isCertValid(cert, convertStringToTimeT(pSigDoc, pSigInfo->szTimeStamp));
	RETURN_IF_NOT(err == ERR_OK, err);
	if(bUseCA)
	  err = isCertSignedByCERT((const X509*)cert, (const X509*)signerCACert);
	RETURN_IF_NOT(err == ERR_OK, err);
	return ERR_OK;
}

//--------------------------------------------------
// Finds all detatched files and returns their
// names. The user can now set the path portion.
// Allocaes new memory that caller must free 
// with FilePaths_free()
// pSigDoc - signed document
// pPaths - pointer to FilePaths array
// nPaths - pointer to number of paths
//--------------------------------------------------
EXP_OPTION int findDetatchedDataFiles(SignedDoc* pSigDoc, FilePath** pPaths, int* nPaths)
{
	FilePath *pPath, *pTmp;
	int err = ERR_OK, d;

	RETURN_IF_NULL_PARAM(pSigDoc);
	RETURN_IF_NULL_PARAM(pPaths);
	RETURN_IF_NULL_PARAM(nPaths);
	*pPaths = NULL;
	*nPaths = 0;
	for(d = 0; d < pSigDoc->nDataFiles; d++) {
		DataFile* pDf = pSigDoc->pDataFiles[d];
		if (!strcmp(pDf->szContentType, CONTENT_DETATCHED)) {
			if (*pPaths && *nPaths) {
				pTmp = (FilePath*)realloc(*pPaths, sizeof(FilePath) * (*nPaths+1));
				if (pTmp != NULL) { //malloc failure!
					*pPaths	= pTmp;
					*nPaths = *nPaths + 1;	
					pPath = (*pPaths) + ((*nPaths) - 1);
				} else 
					SET_LAST_ERROR_RETURN_CODE(ERR_BAD_ALLOC);
			} else {
				pTmp = (FilePath*)malloc(sizeof(FilePath));
				if (pTmp != NULL) { //malloc failure!
					*pPaths = pTmp;
					pPath = *pPaths;
					*nPaths = 1;
				} else 
					 SET_LAST_ERROR_RETURN_CODE(ERR_BAD_ALLOC);
			}
			pPath->szFileName = pPath->szPath = NULL;
			pPath->szFileName = strdup(pDf->szFileName);
		}
	}
	return err;
}


//--------------------------------------------------
// Frees file paths array
// pPaths - FilePaths array
// nPaths - number of paths
//--------------------------------------------------
EXP_OPTION void FilePaths_free(FilePath* pPaths, int nPaths)
{
	int i;
	for(i = 0; i < nPaths; i++) {
		free(pPaths[i].szFileName);
		free(pPaths[i].szPath);
	}
	free(pPaths);
}

//--------------------------------------------------
// Checks if this element tag contains the
// required attributes with the given values
// data - input data, XML tags data (not content but the attributes)
// nAttrs - number of attributes to check
// attNames - array of attribute names
// attValues - array of attribute values
// returns 0 if OK (all atributes found or none desired)
//--------------------------------------------------
int checkAttrs(const char* data, int nAttrs,
				const char** attNames, const char** attValues)
{
  int remains = 0, i;
  char *pTmp1 = 0, *pTmp2 = 0;

  RETURN_IF_NULL_PARAM(data);

  if(nAttrs) {
    RETURN_IF_NULL_PARAM(attNames);
    RETURN_IF_NULL_PARAM(attValues);
    remains = nAttrs; // must find nAttrs values
    for(i = 0; i < nAttrs; i++) {
      RETURN_IF_NULL(attNames[i]);
      RETURN_IF_NULL(attValues[i]);
      if((pTmp1 = strstr(data, attNames[i])) != 0) {
	if((pTmp2 = strstr(pTmp1, "\"")) != 0) {
	  if(!strncmp(pTmp2+1, attValues[i], strlen(attValues[i])))
	    remains--; // found one
	}
      }
    }
  }
  if (remains == 0) 
    return ERR_OK;
  else 
    return remains;
}

char* findString(char* mainBuf, char* search)
{
	char* pTmp = NULL;
	// first find in the latest 2KB
	pTmp = strstr(mainBuf+2048, search);
	// if not found check the previous buffer
	// as well because the tag could have been broken
	// between two buffer borders
	if(!pTmp)
		pTmp = strstr(mainBuf, search);
	return pTmp;
}

//--------------------------------------------------
// Finds the contents of a given XML tag
// in the given file.
// data - buffer for tag content data (caller must deallocate)
// tagName - tag name to search
// nAttrs - number of attributes to check
// attNames - array of attribute names
// attValues - array of attribute values
// withTags - 1 if include tags themselves, else 0
// returns 0 if tag was found and data read.
//--------------------------------------------------
int readTagContents(char** data, const char* fileName, 
		    const char* tagName, int nAttrs,
		    const char** attNames, const char** attValues,
		    int withTags)
{
  int err = ERR_OK, status, len, level;
  FILE *hFile = 0;
  char *pTmp1 = 0, *pTmp2 = 0, *pTmp3 = 0, *pBegin = 0, *pData = NULL;
  char buf1[4097], buf2[100];

  RETURN_IF_NULL_PARAM(data);
  RETURN_IF_NULL_PARAM(fileName);
  RETURN_IF_NULL_PARAM(tagName);
  RETURN_IF_NULL_PARAM(attNames);
  RETURN_IF_NULL_PARAM(attValues);

  if((hFile = fopen(fileName, "rb")) != 0) {
    status = 0; // nothing found yet
    level = 0;
    memset(buf1, 0, sizeof(buf1));
    // allways load the second half of the buffer 
    // warning - assignment in conditional expression -> yes but the code seems clearer this way!
    while((len = fread(buf1+2048, 1, 2048, hFile)) && status < 2) {
      switch(status) {
      case 0:
	// find <tagName
	snprintf(buf2, sizeof(buf2), "<%s ", tagName);	
	pTmp1 = findString(buf1, buf2);
	while(pTmp1 && (status == 0) && ((int)(pTmp1-buf1) < (int)sizeof(buf1))) {
	  pTmp2 = strstr(pTmp1, ">");
	  if(pTmp2) {
	    *pTmp2 = 0;
	    err = checkAttrs(pTmp1, nAttrs, attNames, attValues);
	    *pTmp2 = '>';
	    if(!err) {
	      // mark the found tag
	      // in order not to later mistake this 
	      // for a new level. Take also buffer moving
	      // in account
	      pBegin = pTmp1-2048;
	      status = 1; // now search for...
	      if(withTags) {								
		snprintf(buf2, sizeof(buf2), "</%s>", tagName);				
		if((pTmp3 = strstr(pTmp1, buf2)) != 0) 
		  *(pTmp3+strlen(buf2)) = 0;
		len = strlen(pTmp1)+1;
		pData = (char*)malloc(len);
		memset(pData, 0, len);
		RETURN_IF_BAD_ALLOC(pData);
		strncpy(pData, pTmp1, len);
		if(pTmp3) {
		  *data = pData;
		  status = 2;
		}
	      } else {
		pTmp2++; // first byte of content data
		// find </tagName>
		snprintf(buf2, sizeof(buf2), "</%s>", tagName);				
		if((pTmp3 = strstr(pTmp1, buf2)) != 0) 
		  *pTmp3 = 0;								
		len = strlen(pTmp2);
		pData = (char*)malloc(len+1);
		RETURN_IF_BAD_ALLOC(pData);
		strncpy(pData, pTmp2, len);
		if(pTmp3) {
		  *data = pData;
		  status = 2;
		}
	      } // else
	    } // if(!err)
	    else
	      pTmp1 = strstr(pTmp2, buf2);
	  } // if(pTmp2)
	  else
	    pTmp1++;
	} // if(pTmp1)
	break;
      case 1:
	snprintf(buf2, sizeof(buf2), "</%s>", tagName);
	pTmp3 = findString(buf1, buf2);
	// if the found end-tag is fully in the 
	// previous buffer then if cannot be the right 
	// one because I would have noticed it in
	// the last step
	if((pTmp3+strlen(buf2)) < (buf1+2048))
	  pTmp3 = NULL;
	snprintf(buf2, sizeof(buf2), "<%s ", tagName);				
	pTmp1 = findString(buf1, buf2);
	if(pTmp1 && pTmp1 > pBegin && !pTmp3) 
	  level++;
	if(pTmp3 && !level) {
	  if(withTags) {
	    snprintf(buf2, sizeof(buf2), "</%s>", tagName);
	    *(pTmp3 + strlen(buf2)) = 0;
	  } else
	    *pTmp3 = 0;
	  *data = pData;
	  status = 2;
	}
	if(pTmp3 && level > 0)
	  level--;
	len = strlen(buf1+2048);
	if(len) {
	  RETURN_IF_NULL(pData);
	  pData = (char*)realloc(pData, strlen(pData)+len+1);
	  strncpy(strchr(pData, 0), buf1+2048, strlen(pData)+len+1);
	  *data = pData;
	}
	break;
	
      default:
	break;
      }
      memcpy(buf1, buf1+2048, 2048);
      memset(buf1+2048, 0, 2049);
    } // while
    fclose(hFile);
  } // if(hFile
  else
    err = ERR_FILE_READ;
  return (pData == NULL);
}

