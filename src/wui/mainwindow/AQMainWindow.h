/***************************************************************************
AQMainWindow.h
-------------------
begin                : 26/08/2007
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

#ifndef AQMAINWINDOW_H_
#define AQMAINWINDOW_H_

#include <AQUi/WQWidget.h>
#include <AQSql/AQSqlDatabase.h>

class AQMainWindowPrivate;
class WQTree;

namespace Wt
{
  class WTreeNode;
}

/**
Ventanas principales de AbanQ

@author InfoSiAL S.L.
*/
class AQMainWindow : public WQWidget
{
  Q_OBJECT
  AQ_DECLARE_PRIVATE( AQMainWindow )

public:

  explicit AQMainWindow( WQWidget * parent = 0 );
  AQMainWindow( WQWidget * parent, AQSqlDatabase db );

  WWidget * ww();
  void addWQWidget( WQWidget * w );

  QIcon actionIcon( const QString & idAction ) const;
  const AQSqlDatabase & db() const;

Q_SIGNALS:

  void closeRequest( AQMainWindow * );

public Q_SLOTS:

  void activateAction( const QString & );

  void init();
  void finish();
  void closeTab();
  void closeTab( WQWidget * );
  void closeOtherTabs();
  void closeOtherTabsOfAction();
  void setCurrentWidget( WQWidget * );
  void newTreeFolder();
  void renameTreeItem();
  void deleteTreeItem();
  void addItemFromAction( const QString & );
  void addRecentItem( const QString & );
  void saveTreeMarks();
  void restoreTreeMarks();
  void saveTreeRecent();
  void restoreTreeRecent();
  void exportTreeMarks();
  void importTreeMarks();
  void exportTree( WQTree *, const QString & = QString() );
  void importTree( WQTree *, const QString & = QString() );
  void openActionsOfTreeMarks();

private Q_SLOTS:

  void activateAction( QAction * );
  void activateItem( WTreeNode *, int );

private:

  AQ_WUI_INVOKABLE_1( activateAction, QAction* )
};

#endif /*AQMAINWINDOW_H_*/
