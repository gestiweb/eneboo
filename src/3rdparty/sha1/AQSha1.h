/***************************************************************************
                         AQSha1.h
                     -------------------
begin                : 06/08/2007
copyright            : (C) 2003-2007 by InfoSiAL S.L.
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

#ifndef AQSHA1_H_
#define AQSHA1_H_

#include <QString>
#include <QBuffer>
#include <QTextStream>

#include <stdio.h>  // Needed for file access
#include <memory.h> // Needed for memset and memcpy
#include <string.h> // Needed for strcat and strcpy

// If you're compiling big endian, just comment out the following line
#define SHA1_LITTLE_ENDIAN

typedef union {
  quint8 c[ 64 ];
  quint32 l[ 16 ];
} SHA1_WORKSPACE_BLOCK;

class AQSha1
{

public:

  // Two different formats for ReportHash(...)
  enum {
    REPORT_HEX = 0,
    REPORT_DIGIT = 1
  };

  // Constructor and Destructor
  AQSha1();
  virtual ~AQSha1();

  /**
  Obtiene la clave SHA1 de una cadena de texto.

  @param str Cadena de la que obtener la clave SHA1
  @return Clave correspondiente en digitos hexadecimales 
  */
  QString sha1( const QString & str );

  /**
  Hace lo mismo que el anterior método pero sobre un ByteArray
  */
  QString sha1( const QByteArray & ba );

  /**
  Hace lo mismo que el anterior método pero sobre un fichero de disco
  */
  QString sha1File( char *szFileName );

  quint32 m_state[ 5 ];
  quint32 m_count[ 2 ];
  quint8 m_buffer[ 64 ];
  quint8 m_digest[ 20 ];

  void Reset();

  // Update the hash value
  void Update( quint8 *data, unsigned int len );
  bool HashFile( char *szFileName );

  // Finalize hash and report
  void Final();
  void ReportHash( char *szReport, quint8 uReportType = REPORT_HEX );
  void GetHash( quint8 *uDest );

private:

  // Private SHA-1 transformation
  void Transform( quint32 state[ 5 ], quint8 buffer[ 64 ] );

  // Member variables
  quint8 m_workspace[ 64 ];
  SHA1_WORKSPACE_BLOCK *m_block; // SHA1 pointer to the byte array above
};

#endif /*AQSHA1_H_*/
