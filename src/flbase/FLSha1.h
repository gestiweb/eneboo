/***************************************************************************
                               FLSha1.h
                          -------------------
 begin                : lun ago 02 2004
 copyright            : (C) 2004-2005 by InfoSiAL, S.L.
 email                : mail@infosial.com
***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 ***************************************************************************/
/***************************************************************************
   Este  programa es software libre. Puede redistribuirlo y/o modificarlo
   bajo  los  términos  de  la  Licencia  Pública General de GNU   en  su
   versión 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

#ifndef FLSHA1_H
#define FLSHA1_H

#include <stdio.h>  // Needed for file access
#include <memory.h> // Needed for memset and memcpy
#include <string.h> // Needed for strcat and strcpy

#include <qglobal.h>

// If you're compiling big endian, just comment out the following line
#define SHA1_LITTLE_ENDIAN

typedef union
{
  Q_UINT8 c[ 64 ];
  Q_UINT32 l[ 16 ];
} SHA1_WORKSPACE_BLOCK;

class FL_EXPORT FLSha1 {
public:
  // Two different formats for ReportHash(...)
  enum
  {
    REPORT_HEX = 0,
    REPORT_DIGIT = 1
  };

  // Constructor and Destructor
  FLSha1();
  virtual ~FLSha1();

  Q_UINT32 m_state[ 5 ];
  Q_UINT32 m_count[ 2 ];
  Q_UINT8 m_buffer[ 64 ];
  Q_UINT8 m_digest[ 20 ];

  void Reset();

  // Update the hash value
  void Update( Q_UINT8 *data, unsigned int len );
  bool HashFile( char *szFileName );

  // Finalize hash and report
  void Final();
  void ReportHash( char *szReport, Q_UINT8 uReportType = REPORT_HEX );
  void GetHash( Q_UINT8 *uDest );

private:
  // Private SHA-1 transformation
  void Transform( Q_UINT32 state[ 5 ], Q_UINT8 buffer[ 64 ] );

  // Member variables
  Q_UINT8 m_workspace[ 64 ];
  SHA1_WORKSPACE_BLOCK *m_block; // SHA1 pointer to the byte array above
}
;

#endif
