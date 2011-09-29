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

#include <libdigidoc/DigiDocDebug.h>
#include <libdigidoc/DigiDocConfig.h>
#include <libdigidoc/DigiDocLib.h>

#if defined(GNUCPP) || !defined(WIN32)
  #include <unistd.h>
  #define _mkdir mkdir
  #define _rmdir rmdir
  #define _unlink unlink
  #define _tzset tzset
  #define _getcwd getcwd
#endif


//-----------------------------------------
// Formats debug output
// level - debug level to output this message on
// func - name of the function
// format - message format and arguments
//-----------------------------------------
EXP_OPTION  void ddocDebug(int level, const char* func, const char* format, ...)
{
  time_t tNow;
  struct tm tm1;
  int nLevel = ConfigItem_lookup_int("DEBUG_LEVEL", 1);
  const char * szDebugFile = ConfigItem_lookup("DEBUG_FILE");
  va_list args;
  FILE* hFile = 0;

  if(level <= nLevel) {
    time(&tNow);
    ddocLocalTime(&tNow, &tm1, 1);
    va_start(args, format);
    if(szDebugFile && ((hFile = fopen(szDebugFile, "ab")) != NULL)) {
      fprintf(hFile, "%s\t[%04d-%02d-%02d %02d:%02d:%02d] ", 
	      func, tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday,
	      tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
      vfprintf(hFile, format, args);
      fprintf(hFile, "\n");
      fclose(hFile);
    } else {
      fprintf(stderr, "%s\t[%04d-%02d-%02d %02d:%02d:%02d] ",
	      func, tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday,
	      tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
      vfprintf(stderr, format, args);
      fprintf(stderr, "\n");
    }
  }
}

//-----------------------------------------
// Writes debug data in a file
// level - debug level to write on
// szFileName - target file name
// pMemBuf - buffer for log data
//-----------------------------------------
EXP_OPTION int ddocDebugWriteFile(int level, const char* szFileName, DigiDocMemBuf *pMemBuf)
{
  int err = ERR_OK;
  FILE* hFile;
  int nLevel = ConfigItem_lookup_int("DEBUG_LEVEL", 1);

  RETURN_IF_NULL_PARAM(szFileName);
  RETURN_IF_NULL_PARAM(pMemBuf);  
  if(level <= nLevel) {
    if((hFile = fopen(szFileName, "ab")) != NULL) {
      fwrite(pMemBuf->pMem, 1, pMemBuf->nLen, hFile);
      fclose(hFile);
    }
  }
  return err;
}

//-----------------------------------------
// Formats debug output
// level - debug level to output this message on
// func - name of the function
// msg - message format and arguments
// args - va_list struct
//-----------------------------------------
EXP_OPTION  void ddocDebugVaArgs(int level, const char* func, const char* msg, va_list args)
{
  time_t tNow;
  struct tm tm1;
  int nLevel = ConfigItem_lookup_int("DEBUG_LEVEL", 1);
  const char * szDebugFile = ConfigItem_lookup("DEBUG_FILE");
  FILE* hFile = 0;

  if(level <= nLevel) {
    time(&tNow);
    ddocLocalTime(&tNow, &tm1, 1);
    if(szDebugFile && ((hFile = fopen(szDebugFile, "ab")) != NULL)) {
      fprintf(hFile, "%s\t[%04d-%02d-%02d %02d:%02d:%02d] ", 
	      func, tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday,
	      tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
      vfprintf(hFile, msg, args);
      fprintf(hFile, "\n");
      fclose(hFile);
    } else {
      fprintf(stderr, "%s\t[%04d-%02d-%02d %02d:%02d:%02d] ",
	      func, tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday,
	      tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
      vfprintf(stderr, msg, args);
      fprintf(stderr, "\n");
    }
  }
}

//-----------------------------------------
// Deletes the log file if it exists.
//-----------------------------------------
EXP_OPTION void ddocDebugTruncateLog()
{
  char * szDebugFile = (char*)ConfigItem_lookup("DEBUG_FILE");
  if(szDebugFile && checkFileExists(szDebugFile)) {
    _unlink(szDebugFile);
  }
}

//-----------------------------------------
// Reads the contents of the log file
// pMemBuf - buffer for log data
//-----------------------------------------
EXP_OPTION int ddocDebugReadLog(DigiDocMemBuf *pMemBuf)
{
  char * szDebugFile = (char*)ConfigItem_lookup("DEBUG_FILE");
  FILE *hFile;
  int err = ERR_OK, l2, l1;
  char buf1[1025];

  RETURN_IF_NULL_PARAM(pMemBuf);
  pMemBuf->pMem = 0;
  pMemBuf->nLen = 0;
  if(szDebugFile) {
    if((hFile = fopen(szDebugFile, "rt")) != NULL) {
      l1 = sizeof(buf1);
      do {
	l2 = fread(buf1, 1, l1, hFile);
	err = ddocMemAppendData(pMemBuf, buf1, l2);
      } while(l2 > 0);
      fclose(hFile);
    }
  }
  else
    err = ERR_FILE_READ;
  return err;
}

