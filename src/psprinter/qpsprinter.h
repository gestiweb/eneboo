/***************************************************************************
                            qpsprinter.h
                         -------------------
begin                : dom ene 22 2006
copyright            : (C) 2002-2006 by InfoSiAL S.L.
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

/**********************************************************************
**
**
** Definition of internal QPSPrinter class.
** QPSPrinter implements PostScript (tm) output via PSPrinter.
**
** Created : 940927
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QPSPRINTER_P_H
#define QPSPRINTER_P_H

#include <qiodevice.h>

class QPSPrinterPrivate;
class PSPrinter;

class FL_EXPORT QPSPrinter : public QPaintDevice {
  private:
    QPSPrinter( PSPrinter *, int );
    QPSPrinter( PSPrinter *, QIODevice * );
    ~QPSPrinter();

    bool cmd ( int, QPainter *, QPDevCmdParam * );

    enum { NewPage = 100, AbortPrinting };

    friend class PSPrinter;
  private:
    QPSPrinterPrivate *d;

    QPSPrinter( const QPSPrinter & );
    QPSPrinter &operator=( const QPSPrinter & );
};

#endif // QPSPRINTER_P_H
