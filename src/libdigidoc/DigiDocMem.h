#ifndef __DIGIDOC_MEM_H__
#define __DIGIDOC_MEM_H__
//==================================================
// FILE:	DigiDocMem.h
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

#include <libdigidoc/DigiDocDefs.h>

#ifdef  __cplusplus
extern "C" {
#endif


  typedef struct DigiDocMemBuf_st {
    void* pMem; // functions will assign allocated memory address here
    long nLen;   // length of data in number of bytes
  } DigiDocMemBuf;

  //--------------------------------------------------
  // Helper function to append data to a memory buffer
  // and grow it as required.
  // pBuf - address of memory buffer pointer
  // data - new data to be appended
  // len - length of data or -1 for zero terminated strings
  //--------------------------------------------------
  EXP_OPTION int ddocMemAppendData(DigiDocMemBuf* pBuf, const char* data, long len);

  //--------------------------------------------------
  // Helper function to assign data to a memory buffer
  // and release old content if necessary
  // pBuf - address of memory buffer pointer
  // data - new data to be appended
  // len - length of data or -1 for zero terminated strings
  //--------------------------------------------------
  EXP_OPTION int ddocMemAssignData(DigiDocMemBuf* pBuf, const char* data, long len);

  //--------------------------------------------------
  // Helper function to set buffer length as required
  // It will fill acquired mem with zeros.
  // pBuf - address of memory buffer pointer
  // len - new length of buffer
  //--------------------------------------------------
  EXP_OPTION int ddocMemSetLength(DigiDocMemBuf* pBuf, long len);

  //--------------------------------------------------
  // Helper function to free/cleanup memory buffer
  // This does not attempt to release the buffer object
  // itself but only it's contents.
  // pBuf - memory buffer pointer
  //--------------------------------------------------
  EXP_OPTION int ddocMemBuf_free(DigiDocMemBuf* pBuf);

  //--------------------------------------------------
  // Helper function to assign zero terminated strings
  // and release old content if necessary
  // dest - destination address
  // src - new data to be assigned
  //--------------------------------------------------
  EXP_OPTION int ddocMemAssignString(char** dest, const char* src);

  //--------------------------------------------------
  // Replaces a substring with another substring 
  // pBuf1 - memory buffer to search in
  // pBuf2 - memory buffer to write converted value to
  // search - search value
  // replacement - replacement value
  //--------------------------------------------------
  EXP_OPTION int ddocMemReplaceSubstr(DigiDocMemBuf* pBuf1, DigiDocMemBuf* pBuf2, 
				      const char* search, const char* replacment);
  //--------------------------------------------------
  // Compares memory buffers
  // pBuf1 - memory buffer to value 1
  // pBuf2 - memory buffer to value 2
  // return 0 if both buffers are equal, 1 if not equal
  //--------------------------------------------------
  EXP_OPTION int ddocMemCompareMemBufs(DigiDocMemBuf* pBuf1, DigiDocMemBuf* pBuf2);


#ifdef  __cplusplus
}
#endif

#endif // __DIGIDOC_MEM_H__
