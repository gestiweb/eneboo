/***************************************************************************
WQWidget.h
-------------------
begin                : 21/06/2008
copyright            : (C) 2003-2008 by InfoSiAL S.L.
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

#ifndef WQWIDGET_H_
#define WQWIDGET_H_

#include <WObject>

#include <AQCore/AQObject.h>
#include <AQUi/AQUi.h>

class WQWidgetPrivate;
class QAction;
class QLayoutItem;
class WQGridLayout;
class WQBoxLayout;

static WQWidget * loadWQWidget( QLayoutItem *, WQWidget * = 0 );

namespace Wt
{
  class WWidget;
  class WLayout;
}

using namespace Wt;

class WQWidget : public AQObject, public WObject
{
  Q_OBJECT
  AQ_DECLARE_PRIVATE( WQWidget )

public :

  explicit WQWidget( WQWidget * parent = 0 );
  WQWidget( WObject * parent );

  virtual ~WQWidget();

  virtual void qWidgetEvent( QEvent * ev );

  virtual void init();
  virtual void finish();

  virtual WWidget * ww();
  virtual WLayout * wl();

  virtual void addWQWidget( WQWidget * w );

  WQWidgetList widgets() const;
  WQWidgetList items() const;
  WQWidget * parentWidget() const;
  WQWidget * widgetLayout() const;
  QWidget * qWidget() const;
  QLayoutItem * qLayItem() const;

  void addAction( QAction * action );
  void removeAction( QAction * action, bool up = true );
  void addActions( QList<QAction *> actions );
  void insertAction( QAction *before, QAction *action, bool up = true );
  void insertActions( QAction *before, QList<QAction*> actions );
  QList<QAction *> actions() const;

  bool canContain() const;
  bool isSpacer() const;
  bool isWidget() const;
  bool isLayout() const;
  bool isProxy() const;

  bool invalidated() const;
  bool setInvalidated( bool inv = true );

  static WQWidget * create( QWidget * widget );
  static WQWidget * webget( QObject * obj );
  static bool isWebget( QObject * obj );

protected:

  WQWidget( WQWidgetPrivate & dd, WObject * parent );

  virtual void update();
  virtual void updateActions();

  void addChildWidget( WQWidget * w );

private:

  void setQWidget( QWidget * qWidget );
  void setQLayItem( QLayoutItem * qLayItem );

  friend WQWidget * loadWQWidget( QLayoutItem *, WQWidget * );

  friend class WQGridLayout;
  friend class WQBoxLayout;
};

#endif /*WQWIDGET_H_*/
