/****************************************************************************
** $Id: qsworkbench.cpp  1.1.5   edited 2006-04-18T10:11:27$
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

#include "qsworkbench.h"
#include "idewindow.h"

extern void qInitImages_qsa();

/*!
  \class QSWorkbench qsscripter.h

  \brief The QSWorkbench class provides an interface to QSA Workbench, the
  QSA simple scripting environment.

*/


class QSWorkbenchPrivate
{
public:
    IdeWindow *ideWindow;
};


/*!
  Creates the simple scripting environment for the project \a project,
  with parent \a parent and name \a name.
*/
QSWorkbench::QSWorkbench( QSProject *project, QWidget *parent, const char *name )
    : QObject( parent ), d( new QSWorkbenchPrivate() )
{
    qInitImages_qsa();
    d->ideWindow = new IdeWindow( parent, name );
    d->ideWindow->setProject( project );
}

/*!
  \internal
 */
QSWorkbench::~QSWorkbench()
{
    delete d->ideWindow;
    delete d;
}

/*!
  Returns the QSA Workbench main widget.
 */

QWidget *QSWorkbench::widget()
{
    return (QWidget *)d->ideWindow;
}

/*!
  Returns the project edited in QSA Workbench.
*/

QSProject *QSWorkbench::project()
{
    return d->ideWindow->project;
}

/*!
  Opens QSA Workbench, the QSA simple scripting environment.
*/
void QSWorkbench::open()
{
    d->ideWindow->show();
}

/*!
  Hides QSA Workbench. Hiding the Scripting environement does
  not trigger any change in the project.
 */
void QSWorkbench::close()
{
    d->ideWindow->close();
}

/*!
  Makes \a script the current script in the editor.
*/
void QSWorkbench::showScript( QSScript *script )
{
    d->ideWindow->showPage( script );
}

/*!
  Makes \a script the current script in the editor and highlights the
  declaration of \a function, if found.
*/
void QSWorkbench::showFunction( QSScript *script, const QString &function )
{
    d->ideWindow->showFunction( script, function );
}
