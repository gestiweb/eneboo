/***************************************************************************
                           FLWidgetAction.h
                          -------------------
 begin                : Mar Jun 17 2003
 copyright            : (C) 2003-2005 by InfoSiAL S.L.
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
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef FLWIDGETACTION_H
#define FLWIDGETACTION_H

#include <qaction.h>

class FLWidgetAction : public QAction {
  Q_OBJECT

public:

  FLWidgetAction( const QString &grp, QObject* parent, const char* name = 0, bool toggle = false ) : QAction( parent, name, toggle ) {
    init( grp );
  }

  FLWidgetAction( const QString &grp, const QString& text, const QIconSet& icon, const QString& menuText, QKeySequence accel, QObject* parent, const char* name = 0, bool toggle = false ) : QAction( text, icon, menuText, accel, parent, name, toggle ) {
    init( grp );
  }

  FLWidgetAction( const QString &grp, const QString& text, const QString& menuText, QKeySequence accel, QObject* parent, const char* name = 0, bool toggle = false ) : QAction( text, menuText, accel, parent, name, toggle ) {
    init( grp );
  }

  ~FLWidgetAction();

#if !defined(Q_NO_USING_KEYWORD)

  using QAction::addedTo;
#endif

  void addedTo( QWidget *, QWidget * );

  QString group() const {
    return grp;
  }

  /**
  Establece el identificador de módulo asociado
  */
  void setIdModule( const QString & idM ) {
    idModule_ = idM;
  }

  /**
  Devuelve el identificador de módulo asociado
  */
  const QString & idModule() const {
    return idModule_;
  }

private:

  void init( const QString &g );
  QString grp;
  QPopupMenu *popupMenu;

  /**
  Identificador de módulo asociado
  */
  QString idModule_;
};

#endif
