/***************************************************************************
AQMainWindow_p.h
-------------------
begin                : 12/08/2008
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

#ifndef AQMAINWINDOW_P_H_
#define AQMAINWINDOW_P_H_

#include "AQUi/WQWidget_p.h"

class AQMainWindowPrivate : public WQWidgetPrivate
{
  AQ_DECLARE_PUBLIC( AQMainWindow )

public:

  AQMainWindowPrivate();
  ~AQMainWindowPrivate();

  void setupUi();

  Ext::Panel * ww_;
  WBorderLayout * lay_;

  void newConsole();
  void initTabView();
  void initTreeMenuModules();
  void initDocks();
  void initGuiObjects();

  void tests();
  AQFormDB * createForm( const QString & idAction );

  QActionGroup * agModule( const QString & idModule );
  QActionGroup * agModules( const QString & idArea );
  QList<QActionGroup *> listAgModules();

  AQTabView * tabView_;
  AQSqlDatabase db_;
  QStringList actsExecDefault_;
  QStringList actsExecMain_;
  int consoleCount_;
  Ext::Panel * dockModules_;
  WQTree * treeModules_;
  Ext::Panel * dockMarks_;
  WQTree * treeMarks_;
  Ext::Panel * dockRecent_;
  WQTree * treeRecent_;
  QIcon folderIcon_;
  QIcon defaultIcon_;

  WQMenu * menuAbanQ_;
  QAction * actExit_;

#ifdef AQ_DEBUG
  QAction * actTests_;
#endif

  WQMenu * menuHelp_;
  QAction * actAboutAbanQ_;
};

Q_DECLARE_METATYPE( QAction* )

#endif /*AQMAINWINDOW_P_H_*/
