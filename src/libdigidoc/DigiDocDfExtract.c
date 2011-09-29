//==================================================
// FILE:	DigiDocDfExtract.c
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
#include <libdigidoc/DigiDocError.h>
#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocConvert.h>
#include <libdigidoc/DigiDocMem.h>
#include <libdigidoc/DigiDocLib.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>

#ifdef WIN32
  #define snprintf  _snprintf
#endif

#define ST_START 0
#define ST_XML   1
#define ST_TAG_NM 2
#define ST_TAG_WS 3
#define ST_ATTR_NM 4
#define ST_ATTR_WS 5
#define ST_ATTR_CON 6
#define ST_CON 7
#define ST_DF_START 8
#define ST_DF_CON 9
#define ST_DF_TAG 10
#define ST_DF_END 11
#define ST_DF_END_END 12

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
				      const char* szCharset)
{
  FILE *fIn = 0, *fOut = 0;
  int err = ERR_OK, i, nRead, lt, la, lc, j, ld, lb, l, eState = 0; 
  long len, lExtr = 0, lSize = 0;
  char chars[1050], convFileName[250], convDataFileName[250], 
	  tag[100], attr[100], con[1030], dec[70], b64line[70];
  unsigned char b64 = 0, nNc = 0, bFound = 0;
  void *pBuf;  
  EVP_ENCODE_CTX ectx;

  RETURN_IF_NULL_PARAM(szFileName);
  RETURN_IF_NULL_PARAM(szDataFileName);
  RETURN_IF_NULL_PARAM(szDocId);
  RETURN_IF_NULL_PARAM(szCharset);
  clearErrors();
  ddocDebug(3, "ddocExtractDataFile", "SigDoc: %s, docid: %s, digidoc: %s, file: %s, charset: %s", (pSigDoc ? "OK" : "NULL"), szDocId, szFileName, szDataFileName, szCharset);
  memset(convFileName, 0, sizeof(convFileName));
  memset(convDataFileName, 0, sizeof(convDataFileName));
  ddocConvertFileName(convDataFileName, sizeof(convDataFileName), szDataFileName);
  ddocConvertFileName(convFileName, sizeof(convFileName), szFileName);
  if(szCharset && !strcmp(szCharset, "NO-CHANGE"))
	  nNc = 1;
  // try reading from memory if already cached?
  nRead = ddocGetDataFileCachedData(pSigDoc, szDocId, &pBuf, &len);
  if(pBuf) { // gotcha
    ddocDebug(3, "ddocSaxExtractDataFile", "Using cached data: %d bytes", len);
    if((fOut = fopen(convDataFileName, "wb")) != NULL) {
      fwrite(pBuf, 1, len, fOut);
      fclose(fOut);
    } else {
      ddocDebug(1, "ddocSaxExtractDataFile", "Error writing file: %s", convDataFileName);
      SET_LAST_ERROR_RETURN_CODE(ERR_FILE_WRITE);
    }
    free(pBuf);
    return nRead;
  }
  // open ddoc file
  if((fIn = fopen(convFileName, "r")) != NULL) {
	  do {
		nRead = fread(chars, 1, 1024, fIn);
		chars[nRead] = 0;
		ddocDebug(6, "ddocExtractDataFile", "Parsing %d bytes: \n%s\n", nRead, chars);
		// handle read data
		for(i = 0; i < nRead; i++) {
			switch(eState) {
			case ST_START: // search '<?xml'
				if(chars[i] == '<' && 
					!strncmp(chars+i, "<?xml", 5)) {
					eState = ST_XML;
					i += 4;
				}
				break;
			case ST_XML: // search '<'
				if(chars[i] == '<') {
					eState = ST_TAG_NM;
					lt = 0;
					tag[lt] = 0;
				}
				break;
			case ST_TAG_NM: // read tag name
				if(isalnum(chars[i]) || chars[i] == ':' || chars[i] == '/') {
					tag[lt] = chars[i];
					tag[++lt] = 0;
				} else if(chars[i] == '>') { // tag ended - content
					eState = ST_CON;
				} else { // expecting atributes
					eState = ST_TAG_WS;
				}
				break;
			case ST_TAG_WS:
				if(chars[i] == '>') {
					if(bFound) {
						eState = ST_DF_CON;
						if(b64)
							EVP_DecodeInit(&ectx);
					} else
						eState = ST_CON; // tag endded - content
					lc = 0;
					con[lc] = 0;
				} else if(isalnum(chars[i])) {
					eState = ST_ATTR_NM; // attr name started
					la = 0;
					attr[la] = chars[i];
					attr[++la] = 0;
				}
				break;
			case ST_ATTR_NM:
				if(isalnum(chars[i])) {
					attr[la] = chars[i];
					attr[++la] = 0;
					break;
				} else if(chars[i] == '\"') {
					eState = ST_ATTR_CON;
					lc = 0;
					con[lc] = 0;
				} else {
					eState = ST_ATTR_WS;
				}
				break;
			case ST_ATTR_WS:
				if(chars[i] == '\"') {
					eState = ST_ATTR_CON;
					lc = 0;
					con[lc] = 0;
				} else {
					eState = ST_TAG_WS;
				}
				break;
			case ST_ATTR_CON:
				if(chars[i] != '\"') {
					con[lc] = chars[i];
					con[++lc] = 0;
				} else {
					eState = ST_TAG_WS;
					// attribute value complete
					if(!strcmp(tag, "DataFile")) {
						if(!strcmp(attr, "ContentType")) {
							b64 = (!strcmp(con, "EMBEDDED_BASE64")) ? 1 : 0;
							lb = 0;
							b64line[0] = 0;
						}
						if(!strcmp(attr, "Size") && bFound) {
							lSize = atol(con);
						}
						if(!strcmp(attr, "Id")) {
							if(!strcmp(con, szDocId)) {
								bFound = 1;
								fOut = fopen(convDataFileName, "wb");
								if(!fOut) {
									SET_LAST_ERROR(ERR_FILE_WRITE);
									err = ERR_FILE_WRITE;
								}
							}
						}
					}
				}
				break;
			case ST_CON:
				if(chars[i] == '<') {
					eState = ST_TAG_NM;
					lt = 0;
					tag[lt] = 0;
				} else {
					//con[lc] = chars[i];
					//con[++lc] = 0;
				}
				break;
			case ST_DF_START: // find tag end
				if(chars[i] == '>') {
					eState = ST_DF_CON;
					lc = 0;
					con[lc] = 0;
					if(b64)
						EVP_DecodeInit(&ectx);
				} 
				break;
			case ST_DF_CON:
				if(chars[i] == '<') {
					eState = ST_DF_TAG;
					lt = 0;
					tag[lt] = 0;
				} else {
					if(lc > sizeof(con) - 1) {
						if(b64 && !nNc) {
							for(l = 0; l < lc; ) {
								while(lb < 64 && l < lc) {
									if(con[l] != '\n' && con[l] != '\r')
										b64line[lb++] = con[l];
									l++;
								}
								if(lb == 64) {
									b64line[lb++] = '\n';
									b64line[lb] = 0;
									ld = sizeof(dec);
									dec[0] = 0;
									EVP_DecodeUpdate(&ectx, (unsigned char*)dec, &ld, (unsigned char*)b64line, lb);
									lExtr += ld;
									if(ld > 0)
									fwrite(dec, 1, ld, fOut);
									lb = 0;
								}
							}
						} else if(nNc || !b64) {
							lExtr += lc;
							fwrite(con, 1, lc, fOut);
						}
						lc = 0;
					}
					con[lc] = chars[i];
					con[++lc] = 0;
				}
				break;
			case ST_DF_TAG:
				if(/*isalnum(chars[i]) || chars[i] == ':' || chars[i] == '/' ||*/ chars[i] != '>') {
					tag[lt] = chars[i];
					tag[++lt] = 0;
				} else { // DF intenal tag name ready
					if(!strcmp(tag, "/DataFile")) { // end of DF
						eState = ST_DF_END;
					} else { // wrong tag - this is content
						con[lc] = '<';
						for(j = 0; j < lt; j++)
							con[++lc] = tag[j];
						con[++lc] = '>';
						con[++lc] = 0;
						eState = ST_DF_CON;
					}
				}
				if(eState != ST_DF_END)
				break;
			case ST_DF_END:
				if(b64 && !nNc) {
					if(lc > 0) {
						for(l = 0; l < lc; ) {
							while(lb < 64 && l < lc) {
								if(con[l] != '\n' && con[l] != '\r')
									b64line[lb++] = con[l];
								l++;
							}
							b64line[lb++] = '\n';
							b64line[lb] = 0;
							ld = sizeof(dec);
							dec[0] = 0;
							EVP_DecodeUpdate(&ectx, (unsigned char*)dec, &ld, (unsigned char*)b64line, lb);
							lExtr += ld;
							if(ld > 0)
								fwrite(dec, 1, ld, fOut);
							lb = 0;
						}
					}
					ld = 0;
					dec[ld] = 0;
					EVP_DecodeFinal(&ectx, (unsigned char*)dec, &ld);
					lExtr += ld;
					if(ld)
						fwrite(dec, 1, ld, fOut);
				} else if(nNc || !b64) {
					if(lc) {
						lExtr += lc;
						fwrite(con, 1, lc, fOut);
						lc = 0;
					}
				}
				i = sizeof(chars);
				//AM 24.09.08 RIK
				eState = ST_DF_END_END;
				break;
			}
		}
		//AM 24.09.08 RIK ST_DF_END to ST_DF_END_END_END
	  } while(nRead > 0 && !err && eState < ST_DF_END_END);

  } else {
    ddocDebug(1, "ddocExtractDataFile", "Error reading file: %s", convFileName);
    SET_LAST_ERROR(ERR_FILE_READ);
  }
  if(fIn)
	  fclose(fIn);
  if(fOut)
	  fclose(fOut);
  if(!nNc && lSize != lExtr) {
	  ddocDebug(1, "ddocExtractDataFile", "Warning! Extracted: %ld bytes but expected: %ld bytes", lExtr, lSize);
    //SET_LAST_ERROR(ERR_FILE_READ);
	//err = ERR_FILE_READ;
  }
  ddocDebug(3, "ddocExtractDataFile", "Extracted DF: %s to %s size: %ld expected: %ld", szDocId, convDataFileName, lExtr, lSize);
  return err;
}
