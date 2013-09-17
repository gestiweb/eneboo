/***************************************************************************
main.cpp
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

#include <AQUi/AQApplication.h>

int main( int argc, char *argv[] )
{
#ifdef AQ_WUI
  sigset_t set;
  sigemptyset( &set );
  sigaddset( &set, SIGINT );
  sigaddset( &set, SIGQUIT );
  sigaddset( &set, SIGTERM );
  sigprocmask( SIG_BLOCK, &set, 0 );
  AQApplicationWeb abanq( argc, argv, &set );
#else
  AQApplication abanq( argc, argv );
#endif

  return abanq.exec();
}
