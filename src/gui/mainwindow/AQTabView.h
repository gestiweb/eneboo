/***************************************************************************
                         AQTabView.h
                     -------------------
begin                : 12/09/2007
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

#ifndef AQTABVIEW_H_
#define AQTABVIEW_H_

#include <uic/ui_AQTabView.h>

class AQTabView : public QWidget
{
  Q_OBJECT

public:

  AQTabView( QWidget * parent = 0 );
  ~AQTabView();

  QWidget * currentWidget() const;
  QWidget * widget( int index ) const;
  QWidgetList widgets() const;
  void setTitle( QWidget * widget, const QString & title ) const;
  int insertTab( int index, QWidget * widget, const QString & title ) const;
  int count() const;
  void setTabPosition( QTabWidget::TabPosition pos );
  void setTabShape( QTabWidget::TabShape s );

protected:

  void closeEvent( QCloseEvent * e );
#ifdef AQ_LOGO

  void paintEvent( QPaintEvent * pe );
#endif

Q_SIGNALS:

  void closeTabClicked();
  void newTabClicked();
  void closeRequest();

public Q_SLOTS:

  void nextTab() const;
  void previousTab() const;
  void updateTitle( const QString & title ) const;
  void setCurrentWidget( QWidget * widget ) const;
  void transferFocus() const;
  void closeTab() const;
  void closeTab( QWidget * widget ) const;
  void addTab( QWidget * widget ) const;

private:

  void init();

  Ui::AQTabView ui;
#ifdef AQ_LOGO

  QImage logo;
#endif
};

#endif /*AQTABVIEW_H_*/
