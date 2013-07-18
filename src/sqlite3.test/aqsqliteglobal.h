/***************************************************************************
 aqsqliteglobal.h
 -------------------
 begin                : 09/10/2012
 copyright            : (C) 2003-2012 by InfoSiAL S.L.
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

#ifndef AQSQLITEGLOBAL_H_
#define AQSQLITEGLOBAL_H_

#if defined(Q_OS_WIN32) && !defined(AQSQLITE_NO_DLL)
#  if defined(AQSQLITE_DLL)
#    define AQSQLITE_EXPORT __declspec(dllexport)
#  else
#    define AQSQLITE_EXPORT __declspec(dllimport)
#  endif
#else
#  define AQSQLITE_EXPORT
#endif

#endif /* AQSQLITEGLOBAL_H_ */
