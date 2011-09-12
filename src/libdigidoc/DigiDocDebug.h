#ifndef __DIGIDOC_DEBUG_H__
#define __DIGIDOC_DEBUG_H__
//==================================================
// FILE:	DigiDocDebug.h
// PROJECT:     Digi Doc
// DESCRIPTION: Digi Doc functions for debug output
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
//      10.08.2004      Veiko Sinivee
//                      Creation
//==================================================

#include <libdigidoc/DigiDocMem.h>
#include <libdigidoc/DigiDocDefs.h>
#include <stdarg.h>

#ifdef  __cplusplus
extern "C" {
#endif

//-----------------------------------------
// Formats debug output
// level - debug level to output this message on
// func - name of the function
// format - message format and arguments
//-----------------------------------------
EXP_OPTION void ddocDebug(int level, const char* func, const char* format, ...);

//-----------------------------------------
// Formats debug output
// level - debug level to output this message on
// func - name of the function
// msg - message format and arguments
// args - va_list struct
//-----------------------------------------
EXP_OPTION  void ddocDebugVaArgs(int level, const char* func, const char* msg, va_list args);

//-----------------------------------------
// Deletes the log file if it exists.
//-----------------------------------------
EXP_OPTION void ddocDebugTruncateLog();

//-----------------------------------------
// Reads the contents of the log file
// pMemBuf - buffer for log data
//-----------------------------------------
EXP_OPTION int ddocDebugReadLog(DigiDocMemBuf *pMemBuf);

//-----------------------------------------
// Writes debug data in a file
// level - debug level to write on
// szFileName - target file name
// pMemBuf - buffer for log data
//-----------------------------------------
EXP_OPTION int ddocDebugWriteFile(int level, const char* szFileName, DigiDocMemBuf *pMemBuf);


#ifdef  __cplusplus
}
#endif

#endif // __DIGIDOC_DEBUG_H__

