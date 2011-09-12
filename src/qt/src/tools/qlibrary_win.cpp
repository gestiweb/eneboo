/****************************************************************************
** $Id: qlibrary_win.cpp,v 1.1.2.4.2.9 2005/12/13 17:57:55 chehrlic Exp $
**
** Implementation of QLibraryPrivate class
**
** Created : 20040628
**
** Copyright (C) 2004 Ralf Habacker
** Copyright (C) 2004 Christian Ehrlicher
**
** This file is part of the tools module of the Qt GUI Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** Contact kde-cygwin@kde.org if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qplatformdefs.h"
#include "private/qlibrary_p.h"

#include "qdir.h"
#include "qmessagebox.h"

/*
  The platform dependent implementations of
  - loadLibrary
  - freeLibrary
  - resolveSymbol
  It's not too hard to guess what the functions do.
*/

bool QLibraryPrivate::loadLibrary()
{
    QString fileName(QDir::convertSeparators(library->library())); 
    UINT oldmode = SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);
    pHnd = LoadLibrary((wchar_t*)fileName.ucs2());
    SetErrorMode(oldmode);
    if ( !pHnd ) {
#ifndef QT_NO_DEBUG
      QMessageBox::warning( 0, "Warn", QString("LoadLibrary(%1) failed with error %2").arg(fileName).arg(GetLastError()),
                            QMessageBox::Ok, 0, 0 );
#endif
      return false;
    }
    return (true);
}

bool QLibraryPrivate::freeLibrary()
{
    if (!FreeLibrary(pHnd)) {
#ifndef QT_NO_DEBUG
        QMessageBox::warning( 0, "Warn", QString("FreeLibrary(%1) failed with error %2").arg(library->library()).arg(GetLastError()),
                              QMessageBox::Ok, 0, 0 );
#endif
        return false;
    }
    return true;
}

void* QLibraryPrivate::resolveSymbol( const char* symbol )
{
  void* address = (void*)GetProcAddress(pHnd, symbol);
  if ( !address ) {
#ifndef QT_NO_DEBUG
    QMessageBox::warning( 0, "Warn", QString("GetProcAddress(%1) failed with error code: %2").arg(symbol).arg(GetLastError()),
                          QMessageBox::Ok, 0, 0 );
#endif
  }
  return address;
}

