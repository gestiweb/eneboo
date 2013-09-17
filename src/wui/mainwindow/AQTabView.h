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

#include <AQUi/WQWidget.h>

class AQTabViewPrivate;

namespace Wt
{
  namespace Ext
  {
    class ToolBar;
  }
}

class AQTabView : public WQWidget
{
  Q_OBJECT
  AQ_DECLARE_PRIVATE( AQTabView )

public:

  explicit AQTabView( WQWidget * parent = 0 );
  AQTabView( WQWidget * parent, bool tabOnTop, bool createButtons );

  WWidget * ww();
  void addWQWidget( WQWidget * w );

  WQWidget * currentWidget();
  WQWidget * widget( int index ) const;

  void setTitle( WQWidget * widget, const QString & title ) const;
  int insertTab( int, WQWidget * widget, const QString & title );
  int count() const;
  void clear();
  Ext::ToolBar * toolBar();

Q_SIGNALS:

  void closeTabClicked();
  void newTabClicked();

public Q_SLOTS:

  void nextTab();
  void previousTab();
  void updateTitle( const QString & );
  void setCurrentWidget( WQWidget * );
  void transferFocus( int );
  void closeTab();
  void addTab();
  void closeTab( WQWidget * );
  void addTab( WQWidget *, const QString & = QString() );
  void hide();
  void show();

private:

  Q_PRIVATE_SLOT( d_func(), void closeFirstTab() );

  AQ_WUI_INVOKABLE( closeTabClicked )
  AQ_WUI_INVOKABLE( newTabClicked )
};

#endif /*AQTABVIEW_H_*/
