//==================================================
// FILE:	DigiDocMem.c
// PROJECT:     Digi Doc
// DESCRIPTION: Digi Doc functions for memory buffer management
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
//      09.09.2004      Veiko Sinivee
//                      Creation
//==================================================

#include <libdigidoc/DigiDocMem.h>
#include <libdigidoc/DigiDocLib.h>
#include <libdigidoc/DigiDocError.h>
#include <libdigidoc/DigiDocDebug.h>
#include <string.h>

//--------------------------------------------------
// Helper function to append data to a memory buffer
// and grow it as required.
// pBuf - address of memory buffer pointer
// data - new data to be appended
// len - length of data or -1 for zero terminated strings
//--------------------------------------------------
EXP_OPTION int ddocMemAppendData(DigiDocMemBuf* pBuf, const char* data, long len)
{
  long addLen = len;

  RETURN_IF_NULL_PARAM(pBuf);
  RETURN_IF_NULL_PARAM(data);
  if(addLen == -1)
    addLen = strlen(data);
  // ddocDebug(7, "ddocAppendData", "Len: %ld data: \'%s\'", addLen, data);
  pBuf->pMem = realloc(pBuf->pMem, pBuf->nLen + addLen + 1);
  if(!pBuf->pMem)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC);
  memset((char*)pBuf->pMem + pBuf->nLen, 0, addLen+1);
  memcpy((char*)pBuf->pMem + pBuf->nLen, data, addLen);
  pBuf->nLen += addLen;
  // ddocDebug(8, "ddocAppendData", "BUFFER Len: %ld data:\'%s\'", pBuf->nLen, pBuf->pMem);
  return ERR_OK;
}

//--------------------------------------------------
// Helper function to set buffer length as required
// It will fill acquired mem with zeros.
// pBuf - address of memory buffer pointer
// len - new length of buffer
//--------------------------------------------------
EXP_OPTION int ddocMemSetLength(DigiDocMemBuf* pBuf, long len)
{
  long addLen = len;

  RETURN_IF_NULL_PARAM(pBuf);
  addLen = len - pBuf->nLen;
  // ddocDebug(7, "ddocMemSetLength", "Len: %ld", addLen);
  pBuf->pMem = realloc(pBuf->pMem, pBuf->nLen + addLen + 1);
  if(!pBuf->pMem)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC);
  memset((char*)pBuf->pMem + pBuf->nLen, 0, addLen+1);
  pBuf->nLen += addLen;
  // ddocDebug(8, "ddocMemSetLength", "BUFFER Len: %ld data:\'%s\'", pBuf->nLen, pBuf->pMem);
  return ERR_OK;
}

//--------------------------------------------------
// Helper function to assign data to a memory buffer
// and release old content if necessary
// pBuf - address of memory buffer pointer
// data - new data to be appended
// len - length of data or -1 for zero terminated strings
//--------------------------------------------------
EXP_OPTION int ddocMemAssignData(DigiDocMemBuf* pBuf, const char* data, long len)
{
  RETURN_IF_NULL_PARAM(pBuf);
  RETURN_IF_NULL_PARAM(data);
  // ddocDebug(7, "ddocAssignData", "Len: %d data: \'%s\'", len, data);
  if(pBuf->pMem)
    free(pBuf->pMem);
  pBuf->pMem = 0;
  pBuf->nLen = 0;
  return ddocMemAppendData(pBuf, data, len);
}

//--------------------------------------------------
// Helper function to free/cleanup memory buffer
// This does not attempt to release the buffer object
// itself but only it's contents.
// pBuf - memory buffer pointer
//--------------------------------------------------
EXP_OPTION int ddocMemBuf_free(DigiDocMemBuf* pBuf)
{
  RETURN_IF_NULL_PARAM(pBuf);
  if(pBuf->pMem)
    free(pBuf->pMem);
  pBuf->pMem = 0;
  pBuf->nLen = 0;
  return ERR_OK;
}


//--------------------------------------------------
// Helper function to assign zero terminated strings
// and release old content if necessary
// dest - destination address
// src - new data to be assigned
//--------------------------------------------------
EXP_OPTION int ddocMemAssignString(char** dest, const char* src)
{
  RETURN_IF_NULL_PARAM(dest);
  RETURN_IF_NULL_PARAM(src);

  if(*dest)
    free(*dest);
  *dest = (char*)strdup(src);
  if(!dest)
    SET_LAST_ERROR_RETURN(ERR_BAD_ALLOC, ERR_BAD_ALLOC)
  else
    return ERR_OK; 
}

//--------------------------------------------------
// Replaces a substring with another substring 
// pBuf1 - memory buffer to search in
// pBuf2 - memory buffer to write converted value to
// search - search value
// replacement - replacement value
//--------------------------------------------------
EXP_OPTION int ddocMemReplaceSubstr(DigiDocMemBuf* pBuf1, DigiDocMemBuf* pBuf2, 
				    const char* search, const char* replacement)
{
  int err = ERR_OK, i, n;

  RETURN_IF_NULL_PARAM(pBuf1);
  RETURN_IF_NULL_PARAM(pBuf1->pMem);
  RETURN_IF_NULL_PARAM(pBuf2);
  RETURN_IF_NULL_PARAM(search);
  RETURN_IF_NULL_PARAM(replacement);
  //ddocDebug(7, "ddocMemReplaceSubstr", "Replace: \'%s\' with: \'%s\' in: \'%s\'", 
  //	    search, replacement, (const char*)pBuf1->pMem);
  ddocMemBuf_free(pBuf2);
  n = strlen(search);
  for(i = 0; !err && (i < pBuf1->nLen); i++) {
    if(!strncmp((char*)pBuf1->pMem + i, search, n)) { // match
      err = ddocMemAppendData(pBuf2, replacement, -1);
      i += strlen(search) - 1;
    } else { // no match
      err = ddocMemAppendData(pBuf2, (char*)pBuf1->pMem + i, 1);
    }
  }
  return err;
}

//--------------------------------------------------
// Compares memory buffers
// pBuf1 - memory buffer to value 1
// pBuf2 - memory buffer to value 2
// return 0 if both buffers are equal, 1 if not equal
//--------------------------------------------------
EXP_OPTION int ddocMemCompareMemBufs(DigiDocMemBuf* pBuf1, DigiDocMemBuf* pBuf2)
{
  int err = ERR_OK, i;

  RETURN_IF_NULL_PARAM(pBuf1);
  RETURN_IF_NULL_PARAM(pBuf1->pMem);
  RETURN_IF_NULL_PARAM(pBuf2);
  RETURN_IF_NULL_PARAM(pBuf2->pMem);
  if(pBuf1->nLen != pBuf2->nLen)
	  return 1;
  for(i = 0; (i < pBuf1->nLen); i++) {
    if(((char*)pBuf1->pMem)[i] != ((char*)pBuf2->pMem)[i])
		return 1;
  }
  return 0;
}
