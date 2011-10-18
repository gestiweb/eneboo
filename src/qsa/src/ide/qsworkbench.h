/****************************************************************************
** $Id: qsworkbench.h  1.1.5   edited 2006-02-23T15:39:57$
**
** Copyright (C) 2001-2006 Trolltech AS.  All rights reserved.
**
** This file is part of the Qt Script for Applications framework (QSA).
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding a valid Qt Script for Applications license may use
** this file in accordance with the Qt Script for Applications License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about QSA Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
*****************************************************************************/

#ifndef QSWORKBENCH_H
#define QSWORKBENCH_H

#include "qsaglobal.h"
#include "qsproject.h"
#include <qobject.h>

#ifndef QSA_NO_IDE

class QSWorkbenchPrivate;

class QSA_EXPORT QSWorkbench : public QObject
{
    Q_OBJECT

public:
    QSWorkbench( QSProject *project, QWidget *parent = 0, const char *name = 0 );
    ~QSWorkbench();

    QWidget *widget();
    QSProject *project();

public slots:
    void open();
    void close();
    void showScript( QSScript *script );
    void showFunction( QSScript *script, const QString &function );

private:
    QSWorkbenchPrivate *d;
};

#endif
#endif
