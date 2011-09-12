/***************************************************************************
                            FLImageViewer.h
                          -------------------
 begin                : vie mar 23 2007
 copyright            : (C) 2007 by InfoSiAL S.L.
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
** $Id: showimg.h,v 1.1.1.9 2006/05/07 17:31:27 chehrlic Exp $
**
** Copyright (C) 1992-2005 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <qwidget.h>
#include <qimage.h>
#include <qmenubar.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qclipboard.h>

class FLImageViewerInterface;

class FLImageViewer : public QWidget {
  Q_OBJECT

  friend class FLImageViewerInterface;

public:
  FLImageViewer( QWidget *parent = 0, const char *name = 0, WFlags fl = 0 );
  ~FLImageViewer();
  bool loadImage( const QString& );
protected:
  void paintEvent( QPaintEvent * );
  void resizeEvent( QResizeEvent * );
  void mousePressEvent( QMouseEvent * );
  void mouseReleaseEvent( QMouseEvent * );
  void mouseMoveEvent( QMouseEvent * );

private:
  void scale();
  int  conversion_flags;
  bool smooth() const;
  bool useColorContext() const;
  int  alloc_context;
  bool convertEvent( QMouseEvent* e, int& x, int& y );
  QString     filename;
  QImage image;   // the loaded image
  QPixmap pm;   // the converted pixmap
  QPixmap pmScaled;  // the scaled pixmap

  QMenuBar   *menubar;
  QPopupMenu  *file;
  QPopupMenu   *saveimage;
  QPopupMenu   *savepixmap;
  QPopupMenu  *edit;
  QPopupMenu  *options;

  QWidget    *helpmsg;
  QLabel     *status;
  int         si, sp, ac, co, mo, fd, bd, // Menu item ids
  td, ta, ba, fa, au, ad, dd,
  ss, cc, t1, t8, t32;
  void updateStatus();
  void setMenuItemFlags();
  bool  reconvertImage();
  int  pickx, picky;
  int  clickx, clicky;
  bool may_be_other;
  static FLImageViewer* other;
  void setImage( const QImage& newimage );

public slots:

  void openFile();

private slots:
  void to1Bit();
  void to8Bit();
  void to32Bit();
  void toBitDepth( int );

  void copy();
  void paste();

  void hFlip();
  void vFlip();
  void rot180();

  void editText();

  void newWindow();
  void saveImage( int );
  void savePixmap( int );
  void giveHelp();
  void doOption( int );
  void copyFrom( FLImageViewer* );
};


#endif // IMAGEVIEWER_H
