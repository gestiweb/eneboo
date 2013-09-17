/***************************************************************************
AQAQDispatchThread.h
-------------------
begin                : 19/06/2008
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

#ifndef AQDISPATCHTHREAD_H_
#define AQDISPATCHTHREAD_H_

#include <QThread>

class AQDispatchThread;
class WQApplication;

class AQDispatchObject : public QObject
{
  Q_OBJECT

public:

  AQDispatchObject( AQDispatchThread * thread ) :
      QObject(), thread_( thread )
  {}

private Q_SLOTS:

  void methodCreate();
  void methodDestroy();

private:

  AQDispatchThread * thread_;

  friend class AQDispatchThread;
};

class AQDispatchThread : public QThread
{
  Q_OBJECT

public:

  AQDispatchThread();

  void notifyStart();
  void notifyEnd();

  WQApplication * wqApp() const
  {
    return wqApp_;
  }

protected:

  virtual void doCreate() = 0;
  virtual void doDestroy() = 0;

private:

  void run();
  void setPriority( Priority priority );
  Priority priority() const;
  void setStackSize( uint stackSize );
  uint stackSize() const;
  void exit( int retcode = 0 );
  void start( Priority = InheritPriority );
  void terminate();
  void quit();

  WQApplication * wqApp_;
  AQDispatchObject * dispatchObject_;

  friend class AQDispatchObject;
};

#endif /*AQDISPATCHTHREAD_H_*/
