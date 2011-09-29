/***************************************************************************
         FLHelpWindow.h  -  Modificaciones para adaptar a AbanQ
                            Iconos incluido en código
                            Mensajes originales en español
                            Modifiaciones en barra de menus
                            Historial y favoritos se almacenan en el directorio
                            HOME del usuario
                          -------------------
 begin                : Mon May 13 2002
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
/****************************************************************************
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef FLHELPWINDOW_H
#define FLHELPWINDOW_H

#include <qmainwindow.h>
#include <qtextbrowser.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qdir.h>
#include <stdlib.h>

class QComboBox;
class QPopupMenu;

class FLHelpWindow: public QMainWindow
{
  Q_OBJECT

public:

  FLHelpWindow(const QString &home_, const QString &path, QWidget *parent =
                 0, const char *name = 0);
  ~FLHelpWindow();

private slots:
  void setBackwardAvailable(bool);
  void setForwardAvailable(bool);

  void textChanged();
  void about();
  void aboutQt();
  void openFile();
  void newWindow();
  void print();

  void pathSelected(const QString &);
  void histChosen(int);
  void bookmChosen(int);
  void addBookmark();

private:
  void readHistory();
  void readBookmarks();

  QTextBrowser *browser;
  QComboBox *pathCombo;
  int backwardId, forwardId;
  QString selectedURL;
  QStringList history, bookmarks;
  QMap < int, QString > mHistory, mBookmarks;
  QPopupMenu *hist, *bookm;
};

#endif
