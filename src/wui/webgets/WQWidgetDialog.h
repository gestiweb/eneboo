/***************************************************************************
WQWidgetDialog.h
-------------------
begin                : 26/02/2010
copyright            : (C) 2003-2010 by InfoSiAL S.L.
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

#ifndef WQWIDGETDIALOG_H_
#define WQWIDGETDIALOG_H_

#include <AQUi/WQDialog.h>

class QWidget;

class WQWidgetDialog : public WQDialog
{
public:

  WQWidgetDialog( const QMetaObject & metaObject );

protected:

  WQWidget * widget();

private:

  const QMetaObject & metaObject_;
};

#endif /* WQWIDGETDIALOG_H_ */
