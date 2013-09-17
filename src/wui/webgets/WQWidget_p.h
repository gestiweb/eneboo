/***************************************************************************
WQWidget_p.h
-------------------
begin                : 11/08/2008
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

#ifndef WQWIDGET_P_H_
#define WQWIDGET_P_H_

#include <QQueue>
#include <QMutex>

#include "AQCore/AQObject_p.h"

class QTimer;
class AQComponentDB;

class WQWidgetPrivate : public AQObjectPrivate
{
  AQ_DECLARE_PUBLIC( WQWidget )

public :

  WQWidgetPrivate();
  virtual ~WQWidgetPrivate();

  virtual void init();
  virtual void finish()
  {}

  int fetchAndAddGrandChildren( int value );
  int fetchAndAddChildren( int value );
  bool isDirty() const;
  void processChildrenAdded();

  WFlags<AlignmentFlag> calcAlign( WQWidget * wq ) const;

  WContainerWidget * wc_;
  WQWidget * layout_;
  QWidget * qWidget_;
  QLayoutItem * qLayItem_;
  QList<QAction *> actions_;

  bool canContain_;
  bool isSpacer_;
  bool isWidget_;
  bool isLayout_;
  bool isProxy_;

  AQComponentDB * component_;
  bool componentInitialized_;

  WQWidgetList widgets_;
  WQWidgetList items_;

  QQueue<QObject *> childObjectsAdded_;
  QMutex mutexChildObjectsAdded_;
  QAtomicInt childrenAdded_;
  QAtomicInt grandChildrenAdded_;

  bool invalidated_;
};

#endif /*WQWIDGET_P_H_*/
