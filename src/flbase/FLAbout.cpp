/***************************************************************************
                            FLAbout.cpp
                         -------------------
begin                : Sat Jan 26 2002
copyright            : (C) 2002-2005 by InfoSiAL S.L.
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

#include <qlabel.h>

#include "FLAbout.h"

FLAbout::FLAbout(const QString &v,
                 QWidget *parent,
                 const char *name) :
  FLWidgetAbout(parent, name)
{
  labelVersion->setText(v);
  lblCreditos->setText("<p align=\"center\"><b>Eneboo</b><br>"
                       "<b>Open Source ERP Software</b><br><br>"
                       "Publicado bajo los términos de la<br>"
                       "<b>GNU GENERAL PUBLIC LICENSE<br>(version 2)</b><br>"
                       "Este software se distribuye \"como está\", <br>"
                       "<b>sin garantías de ninguna clase</b><br>"
                       );
// lblCompilacion->setText(ENB_DATOS_COMP);
}

FLAbout::~FLAbout() {}
