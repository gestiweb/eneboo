/***************************************************************************
 aqadvglobal.h
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

#ifndef AQADVGLOBAL_H_
#define AQADVGLOBAL_H_

#if defined(Q_OS_WIN32) && !defined(AQADV_NO_DLL)
#  if defined(AQADV_DLL)
#    define AQADV_EXPORT __declspec(dllexport)
#  else
#    define AQADV_EXPORT __declspec(dllimport)
#  endif
#else
#  define AQADV_EXPORT
#endif

#endif /* AQADVGLOBAL_H_ */
