/***************************************************************************
AQUiUtil.cpp
-------------------
begin                : 09/08/2007
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

#include <WTimer>

#include <Ext/ProgressDialog>
#include <Ext/MessageBox>

#include <QSemaphore>

#include <AQUi/AQUiUtil.h>

#include "../wtqt/WQApplication.h"
#include "../wtqt/WQEventLoop.h"
#include "../webgets/WQWidget.h"
#include "../AQApplication.h"

class WQMsgBoxWidget;
class WQProgressWidget;

class AQUiUtilPrivate : public WObject
{
public:

  AQUiUtilPrivate() :
      WObject(), msgBoxWidget_( 0 ), progressWidget_( 0 )
  {}

  AQUi::StandardButton execMsgBox( const QString & title, const QString & text, AQUi::Icon icon,
                                   AQUi::StandardButtons buttons, AQUi::StandardButton defaultButton );
  void exitMsgBox();

  void createProgressDialog( const QString & l, int tS, const QString & title, const QString & cancelButtonText );
  void destroyProgressDialog();
  void setProgress( int p );
  void setLabelText( const QString & l );
  void setTotalSteps( int tS );

private:

  WQProgressWidget * progressWidget_;
  WQMsgBoxWidget * msgBoxWidget_;
};

class WQMsgBoxWidget : public WQWidget
{
public:

  WQMsgBoxWidget( AQUiUtilPrivate * qq ) :
      WQWidget( static_cast<WObject *> ( 0 ) ), q( qq ), mBox_( 0 ), eventLoop_( 0 ), rCode_( Wt::NoButton )
  {}

  void init()
  {
    mBox_ = new Ext::MessageBox( title_, text_, icon_, buttons_ );
    WT_SIGNAL( mBox_->buttonClicked ).connect( q, &AQUiUtilPrivate::exitMsgBox );
    mBox_->WWidget::show();
  }

  void finish()
  {
    if ( mBox_ ) {
      mBox_->WWidget::hide();
      delete mBox_;
    }
  }

  WWidget * ww()
  {
    return mBox_;
  }

  AQUi::StandardButton exec( const QString & title, const QString & text, AQUi::Icon icon,
                             AQUi::StandardButtons buttons, AQUi::StandardButton defaultButton )
  {
    title_ = toWString( title );
    text_ = toWString( QString( text ).replace( '\n', "<br>" ) );
    icon_ = ( Wt::Icon ) icon;
    buttons_ = ( Wt::StandardButton ) int( buttons | defaultButton );

    eventLoop_ = new WQEventLoop( this );

    eventLoop_->execLoop();

    return ( AQUi::StandardButton ) rCode_;
  }

  void exit()
  {
    if ( eventLoop_ ) {
      rCode_ = mBox_->result();
      eventLoop_->exitLoop();
    }
  }

  AQUiUtilPrivate * q;
  Ext::MessageBox * mBox_;
  WQEventLoop * eventLoop_;
  Wt::StandardButton rCode_;

  WString title_;
  WString text_;
  Wt::Icon icon_;
  Wt::StandardButton buttons_;
};

AQUi::StandardButton AQUiUtilPrivate::execMsgBox( const QString & title, const QString & text, AQUi::Icon icon,
    AQUi::StandardButtons buttons, AQUi::StandardButton defaultButton )
{
  msgBoxWidget_ = new WQMsgBoxWidget( this );
  return msgBoxWidget_->exec( title, text, icon, buttons, defaultButton );
}

void AQUiUtilPrivate::exitMsgBox()
{
  if ( msgBoxWidget_ )
    msgBoxWidget_->exit();
}

#define AQ_NO_PROGD if ( !progressWidget_ || !progressWidget_->progD_ ) return

class WQProgressWidget : public WQWidget
{
public:

  WQProgressWidget( AQUiUtilPrivate * qq ) :
      WQWidget( static_cast<WObject *> ( 0 ) ), q( qq ), progD_( 0 ), tS_( 100 ), p_( 0 )
  {}

  void init()
  {
    progD_ = new Ext::ProgressDialog( text_, cancelButtonText_, 0, tS_ );
    if ( cancelButtonText_.empty() )
      progD_->setButtons( NoButton );
    progD_->setWindowTitle( title_ );
    progD_->WWidget::show();
  }

  void finish()
  {
    if ( progD_ ) {
      progD_->WWidget::hide();
      delete progD_;
    }
  }

public:

  WWidget * ww()
  {
    return progD_;
  }

  void exec( const QString & text, int tS, const QString & title, const QString & cancelButtonText )
  {
    text_ = toWString( QString( text ).replace( '\n', "<br>" ) );
    tS_ = tS;
    title_ = toWString( title );
    cancelButtonText_ = toWString( cancelButtonText );

    init();

    AbanQWT->releaseInvocationLast();
  }

  void exit()
  {
    finish();
  }

  void setProgress( int p )
  {
    progD_->setValue( p );
    if ( p % 2 ) {
      WTimer::singleShot( 0, this, &WQProgressWidget::update );
      sem_.acquire();
    }
  }

  void setLabelText( const QString & text )
  {
    progD_->setText( toWString( text ) );
  }

  void setTotalSteps( int tS )
  {
    progD_->setMaximum( tS );
  }

  void update()
  {
    sem_.release();
    AbanQWT->processEvents();
  }

  AQUiUtilPrivate * q;
  Ext::ProgressDialog * progD_;

  WString text_;
  WString title_;
  WString cancelButtonText_;
  int tS_;
  int p_;

  QSemaphore sem_;
};

void AQUiUtilPrivate::createProgressDialog( const QString & text, int tS, const QString & title,
    const QString & cancelButtonText )
{
  progressWidget_ = new WQProgressWidget( this );
  progressWidget_->exec( text, tS, title, cancelButtonText );
}

void AQUiUtilPrivate::destroyProgressDialog()
{
  AQ_NO_PROGD;
  progressWidget_->exit();
}

void AQUiUtilPrivate::setProgress( int p )
{
  AQ_NO_PROGD;
  progressWidget_->setProgress( p );
}

void AQUiUtilPrivate::setLabelText( const QString & text )
{
  AQ_NO_PROGD;
  progressWidget_->setLabelText( text );
}

void AQUiUtilPrivate::setTotalSteps( int tS )
{
  AQ_NO_PROGD;
  progressWidget_->setTotalSteps( tS );
}

AQUiUtil::AQUiUtil() :
    d( new AQUiUtilPrivate )
{}

AQUiUtil::~AQUiUtil()
{
  delete d;
}

void AQUiUtil::createProgressDialog( const QString & text, int tS, const QString & title,
                                     const QString & cancelButtonText )
{
  d->createProgressDialog( text, tS, title, cancelButtonText );
}

void AQUiUtil::destroyProgressDialog() const
{
  d->destroyProgressDialog();
}

void AQUiUtil::setProgress( int p ) const
{
  d->setProgress( p );
}

void AQUiUtil::setLabelText( const QString & text ) const
{

  d->setLabelText( text );
}

void AQUiUtil::setTotalSteps( int tS ) const
{
  d->setTotalSteps( tS );
}

AQUi::StandardButton AQUiUtil::msgBoxInfo( const QString & title, const QString & text, AQUi::StandardButtons buttons,
    AQUi::StandardButton defaultButton ) const
{
  return d->execMsgBox( title, text, AQUi::Information, buttons, defaultButton );
}

AQUi::StandardButton AQUiUtil::msgBoxQuestion( const QString & title, const QString & text,
    AQUi::StandardButtons buttons, AQUi::StandardButton defaultButton ) const
{
  return d->execMsgBox( title, text, AQUi::Question, buttons, defaultButton );
}

AQUi::StandardButton AQUiUtil::msgBoxWarn( const QString & title, const QString & text, AQUi::StandardButtons buttons,
    AQUi::StandardButton defaultButton ) const
{
  return d->execMsgBox( title, text, AQUi::Warning, buttons, defaultButton );
}

AQUi::StandardButton AQUiUtil::msgBoxCritical( const QString & title, const QString & text,
    AQUi::StandardButtons buttons, AQUi::StandardButton defaultButton ) const
{
  return d->execMsgBox( title, text, AQUi::Critical, buttons, defaultButton );
}

