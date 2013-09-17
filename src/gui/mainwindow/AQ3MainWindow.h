/***************************************************************************
                         AQ3MainWindow.h
                     -------------------
begin                : 11/09/2007
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

#ifndef AQ3MAINWINDOW_H_
#define AQ3MAINWINDOW_H_

#include <Q3MainWindow>

class AQ3MainWindow : public Q3MainWindow
{
  Q_OBJECT

public:

  AQ3MainWindow( QWidget * parent = 0, const char * name = 0,
                 Qt::WindowFlags f = Qt::WType_TopLevel );
  virtual ~AQ3MainWindow();

  QStringList actsExecDefault;
  QStringList actsExecMain;

public Q_SLOTS:

  virtual void openDefaultForm();
  virtual void execDefaultScript();
  virtual void execMainScript( const QString & n );
};

#endif /*AQ3MAINWINDOW_H_*/
