/***************************************************************************
AQInvokerThreadContext.h
-------------------
begin                : 04/02/2010
copyright            : (C) 2003-2010 by InfoSiAL S.L.
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

#ifndef AQINVOKERTHREADCONTEXT_H_
#define AQINVOKERTHREADCONTEXT_H_

#include <QCoreApplication>
#include <QThread>
#include <QEvent>
#include <QSemaphore>

#define AQ_EVENT_TYPE (QEvent::Type)1999
#define AQ_INVOKER AQInvokerThreadContext

struct AQNoClass
{
  AQNoClass()
  {}
  static AQNoClass none;
};

template <class T, class R = AQNoClass, class A1 = AQNoClass, class A2 = AQNoClass, class A3 = AQNoClass,
class A4 = AQNoClass, class A5 = AQNoClass, class A6 = AQNoClass>
class AQInvokerThreadContext : public QObject
{
public:

  AQInvokerThreadContext( QThread * thread = QCoreApplication::instance() ->thread(), bool noWait = false ) :
      QObject(), dispatcher_( new AQInvokerThreadContext( this, thread ) ), parent_( 0 ), argc_( 0 ), noRet_( false )
  {
    if ( noWait )
      dispatcher_->sem_.release();
  }

  R invoke( T * target, R( T::*method ) () )
  {
    if ( dispatcher_ ) {
      dispatcher_->argc_ = 0;
      dispatcher_->target_ = target;
      dispatcher_->method_ = ( R( T::* ) () ) method;
      QCoreApplication::postEvent( dispatcher_, new QEvent( AQ_EVENT_TYPE ), Qt::QueuedConnection );
      dispatcher_->sem_.acquire();
      return dispatcher_->ret_;
    } else
      return ret_;
  }

  template <class AA1>
  R invoke( T * target, R( T::*method ) ( AA1 ), A1 a1 )
  {
    dispatcher_->argc_ = 1;
    dispatcher_->target_ = target;
    dispatcher_->a1_ = a1;
    dispatcher_->method1_ = ( R( T::* ) ( A1 ) ) method;
    QCoreApplication::postEvent( dispatcher_, new QEvent( AQ_EVENT_TYPE ), Qt::QueuedConnection );
    dispatcher_->sem_.acquire();
    return dispatcher_->ret_;
  }

  template <class AA1, class AA2>
  R invoke( T * target, R( T::*method ) ( AA1, AA2 ), A1 a1, A2 a2 )
  {
    dispatcher_->argc_ = 2;
    dispatcher_->target_ = target;
    dispatcher_->a1_ = a1;
    dispatcher_->a2_ = a2;
    dispatcher_->method2_ = ( R( T::* ) ( A1, A2 ) ) method;
    QCoreApplication::postEvent( dispatcher_, new QEvent( AQ_EVENT_TYPE ), Qt::QueuedConnection );
    dispatcher_->sem_.acquire();
    return dispatcher_->ret_;
  }

  template <class AA1, class AA2, class AA3>
  R invoke( T * target, R( T::*method ) ( AA1, AA2, AA3 ), A1 a1, A2 a2, A3 a3 )
  {
    dispatcher_->argc_ = 3;
    dispatcher_->target_ = target;
    dispatcher_->a1_ = a1;
    dispatcher_->a2_ = a2;
    dispatcher_->a3_ = a3;
    dispatcher_->method3_ = ( R( T::* ) ( A1, A2, A3 ) ) method;
    QCoreApplication::postEvent( dispatcher_, new QEvent( AQ_EVENT_TYPE ), Qt::QueuedConnection );
    dispatcher_->sem_.acquire();
    return dispatcher_->ret_;
  }

  template <class AA1, class AA2, class AA3, class AA4>
  R invoke( T * target, R( T::*method ) ( AA1, AA2, AA3, AA4 ), A1 a1, A2 a2, A3 a3, A4 a4 )
  {
    dispatcher_->argc_ = 4;
    dispatcher_->target_ = target;
    dispatcher_->a1_ = a1;
    dispatcher_->a2_ = a2;
    dispatcher_->a3_ = a3;
    dispatcher_->a4_ = a4;
    dispatcher_->method4_ = ( R( T::* ) ( A1, A2, A3, A4 ) ) method;
    QCoreApplication::postEvent( dispatcher_, new QEvent( AQ_EVENT_TYPE ), Qt::QueuedConnection );
    dispatcher_->sem_.acquire();
    return dispatcher_->ret_;
  }

  template <class AA1, class AA2, class AA3, class AA4, class AA5>
  R invoke( T * target, R( T::*method ) ( AA1, AA2, AA3, AA4, AA5 ), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 )
  {
    dispatcher_->argc_ = 5;
    dispatcher_->target_ = target;
    dispatcher_->a1_ = a1;
    dispatcher_->a2_ = a2;
    dispatcher_->a3_ = a3;
    dispatcher_->a4_ = a4;
    dispatcher_->a5_ = a5;
    dispatcher_->method5_ = ( R( T::* ) ( A1, A2, A3, A4, A5 ) ) method;
    QCoreApplication::postEvent( dispatcher_, new QEvent( AQ_EVENT_TYPE ), Qt::QueuedConnection );
    dispatcher_->sem_.acquire();
    return dispatcher_->ret_;
  }

  template <class AA1, class AA2, class AA3, class AA4, class AA5, class AA6>
  R invoke( T * target, R( T::*method ) ( AA1, AA2, AA3, AA4, AA5, AA6 ), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6 )
  {
    dispatcher_->argc_ = 6;
    dispatcher_->target_ = target;
    dispatcher_->a1_ = a1;
    dispatcher_->a2_ = a2;
    dispatcher_->a3_ = a3;
    dispatcher_->a4_ = a4;
    dispatcher_->a5_ = a5;
    dispatcher_->a6_ = a6;
    dispatcher_->method6_ = ( R( T::* ) ( A1, A2, A3, A4, A5, A6 ) ) method;
    QCoreApplication::postEvent( dispatcher_, new QEvent( AQ_EVENT_TYPE ), Qt::QueuedConnection );
    dispatcher_->sem_.acquire();
    return dispatcher_->ret_;
  }

  void invoke( T * target, void( T::*method ) () )
  {
    dispatcher_->noRet_ = true;
    dispatcher_->argc_ = 0;
    dispatcher_->target_ = target;
    dispatcher_->voidmethod_ = ( void( T::* ) () ) method;
    QCoreApplication::postEvent( dispatcher_, new QEvent( AQ_EVENT_TYPE ), Qt::QueuedConnection );
    dispatcher_->sem_.acquire();
  }

  template <class AA1>
  void invoke( T * target, void( T::*method ) ( AA1 ), A1 a1 )
  {
    dispatcher_->noRet_ = true;
    dispatcher_->argc_ = 1;
    dispatcher_->target_ = target;
    dispatcher_->a1_ = a1;
    dispatcher_->voidmethod1_ = ( void( T::* ) ( A1 ) ) method;
    QCoreApplication::postEvent( dispatcher_, new QEvent( AQ_EVENT_TYPE ), Qt::QueuedConnection );
    dispatcher_->sem_.acquire();
  }

  template <class AA1, class AA2>
  void invoke( T * target, void( T::*method ) ( AA1, AA2 ), A1 a1, A2 a2 )
  {
    dispatcher_->noRet_ = true;
    dispatcher_->argc_ = 2;
    dispatcher_->target_ = target;
    dispatcher_->a1_ = a1;
    dispatcher_->a2_ = a2;
    dispatcher_->voidmethod2_ = ( void( T::* ) ( A1, A2 ) ) method;
    QCoreApplication::postEvent( dispatcher_, new QEvent( AQ_EVENT_TYPE ), Qt::QueuedConnection );
    dispatcher_->sem_.acquire();
  }

  template <class AA1, class AA2, class AA3>
  void invoke( T * target, void( T::*method ) ( AA1, AA2, AA3 ), A1 a1, A2 a2, A3 a3 )
  {
    dispatcher_->noRet_ = true;
    dispatcher_->argc_ = 3;
    dispatcher_->target_ = target;
    dispatcher_->a1_ = a1;
    dispatcher_->a2_ = a2;
    dispatcher_->a3_ = a3;
    dispatcher_->voidmethod3_ = ( void( T::* ) ( A1, A2, A3 ) ) method;
    QCoreApplication::postEvent( dispatcher_, new QEvent( AQ_EVENT_TYPE ), Qt::QueuedConnection );
    dispatcher_->sem_.acquire();
  }

  template <class AA1, class AA2, class AA3, class AA4>
  void invoke( T * target, void( T::*method ) ( AA1, AA2, AA3, AA4 ), A1 a1, A2 a2, A3 a3, A4 a4 )
  {
    dispatcher_->noRet_ = true;
    dispatcher_->argc_ = 4;
    dispatcher_->target_ = target;
    dispatcher_->a1_ = a1;
    dispatcher_->a2_ = a2;
    dispatcher_->a3_ = a3;
    dispatcher_->a4_ = a4;
    dispatcher_->voidmethod4_ = ( void( T::* ) ( A1, A2, A3, A4 ) ) method;
    QCoreApplication::postEvent( dispatcher_, new QEvent( AQ_EVENT_TYPE ), Qt::QueuedConnection );
    dispatcher_->sem_.acquire();
  }

  template <class AA1, class AA2, class AA3, class AA4, class AA5>
  void invoke( T * target, void( T::*method ) ( AA1, AA2, AA3, AA4, AA5 ), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 )
  {
    dispatcher_->noRet_ = true;
    dispatcher_->argc_ = 5;
    dispatcher_->target_ = target;
    dispatcher_->a1_ = a1;
    dispatcher_->a2_ = a2;
    dispatcher_->a3_ = a3;
    dispatcher_->a4_ = a4;
    dispatcher_->a5_ = a5;
    dispatcher_->voidmethod5_ = ( void( T::* ) ( A1, A2, A3, A4, A5 ) ) method;
    QCoreApplication::postEvent( dispatcher_, new QEvent( AQ_EVENT_TYPE ), Qt::QueuedConnection );
    dispatcher_->sem_.acquire();
  }

  template <class AA1, class AA2, class AA3, class AA4, class AA5, class AA6>
  void invoke( T * target, void( T::*method ) ( AA1, AA2, AA3, AA4, AA5, AA6 ), A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6 )
  {
    dispatcher_->noRet_ = true;
    dispatcher_->argc_ = 6;
    dispatcher_->target_ = target;
    dispatcher_->a1_ = a1;
    dispatcher_->a2_ = a2;
    dispatcher_->a3_ = a3;
    dispatcher_->a4_ = a4;
    dispatcher_->a5_ = a5;
    dispatcher_->a6_ = a6;
    dispatcher_->voidmethod6_ = ( void( T::* ) ( A1, A2, A3, A4, A5, A6 ) ) method;
    QCoreApplication::postEvent( dispatcher_, new QEvent( AQ_EVENT_TYPE ), Qt::QueuedConnection );
    dispatcher_->sem_.acquire();
  }

  QObject * dispatcher() const
  {
    return dispatcher_;
  }

protected:

  void customEvent( QEvent * event )
  {
    if ( event->type() == AQ_EVENT_TYPE && parent_ ) {
      switch ( argc_ ) {
        case 0:
          if ( noRet_ )
            ( target_->*voidmethod_ ) ();
          else
            ret_ = ( target_->*method_ ) ();
          break;
        case 1:
          if ( noRet_ )
            ( target_->*voidmethod1_ ) ( a1_ );
          else
            ret_ = ( target_->*method1_ ) ( a1_ );
          break;
        case 2:
          if ( noRet_ )
            ( target_->*voidmethod2_ ) ( a1_, a2_ );
          else
            ret_ = ( target_->*method2_ ) ( a1_, a2_ );
          break;
        case 3:
          if ( noRet_ )
            ( target_->*voidmethod3_ ) ( a1_, a2_, a3_ );
          else
            ret_ = ( target_->*method3_ ) ( a1_, a2_, a3_ );
          break;
        case 4:
          if ( noRet_ )
            ( target_->*voidmethod4_ ) ( a1_, a2_, a3_, a4_ );
          else
            ret_ = ( target_->*method4_ ) ( a1_, a2_, a3_, a4_ );
          break;
        case 5:
          if ( noRet_ )
            ( target_->*voidmethod5_ ) ( a1_, a2_, a3_, a4_, a5_ );
          else
            ret_ = ( target_->*method5_ ) ( a1_, a2_, a3_, a4_, a5_ );
          break;
        case 6:
          if ( noRet_ )
            ( target_->*voidmethod6_ ) ( a1_, a2_, a3_, a4_, a5_, a6_ );
          else
            ret_ = ( target_->*method6_ ) ( a1_, a2_, a3_, a4_, a5_, a6_ );
          break;
      }
      sem_.release();
    }
  }

private:

  AQInvokerThreadContext( AQInvokerThreadContext * parent, QThread * thread ) :
      QObject(), dispatcher_( 0 ), parent_( parent ), argc_( 0 ), noRet_( false )
  {
    if ( thread != QThread::currentThread() )
      moveToThread( thread );

    connect( parent_, SIGNAL( destroyed() ), this, SLOT( deleteLater() ) );
  }

  T * target_;
  R ret_;

  R ( T::*method_ ) ();
  R ( T::*method1_ ) ( A1 );
  R ( T::*method2_ ) ( A1, A2 );
  R ( T::*method3_ ) ( A1, A2, A3 );
  R ( T::*method4_ ) ( A1, A2, A3, A4 );
  R ( T::*method5_ ) ( A1, A2, A3, A4, A5 );
  R ( T::*method6_ ) ( A1, A2, A3, A4, A5, A6 );

  void ( T::*voidmethod_ ) ();
  void ( T::*voidmethod1_ ) ( A1 );
  void ( T::*voidmethod2_ ) ( A1, A2 );
  void ( T::*voidmethod3_ ) ( A1, A2, A3 );
  void ( T::*voidmethod4_ ) ( A1, A2, A3, A4 );
  void ( T::*voidmethod5_ ) ( A1, A2, A3, A4, A5 );
  void ( T::*voidmethod6_ ) ( A1, A2, A3, A4, A5, A6 );

  A1 a1_;
  A2 a2_;
  A3 a3_;
  A4 a4_;
  A5 a5_;
  A6 a6_;

  QSemaphore sem_;
  AQInvokerThreadContext * dispatcher_;
  AQInvokerThreadContext * parent_;
  int argc_;
  bool noRet_;
};

#endif /* AQINVOKERTHREADCONTEXT_H_ */
