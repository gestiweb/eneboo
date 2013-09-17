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

#include <QtGui>

#include "AQUiUtil.h"
#include "../AQApplication.h"
#include "../mainwindow/AQMainWindow.h"

class AQUiUtilPrivate
{

private:

class AQProgressDialog : public QProgressDialog
  {
  public:

    AQProgressDialog() :
        QProgressDialog( QString(), QString(), 0, 100, AbanQ->activeMainWindow(), Qt::Dialog ),
        ignoreHide_( false )
    {
      setWindowModality( Qt::ApplicationModal );
    }

    bool ignoreHide_;

  protected:

    void closeEvent( QCloseEvent * e )
    {
      e->ignore();
    }

    void hideEvent( QHideEvent * e )
    {
      if ( ignoreHide_ ) {
        e->ignore();
        QTimer::singleShot( 0, this, SLOT( show() ) );
      } else
        QProgressDialog::hideEvent( e );
    }
  };

public:

  AQUiUtilPrivate();

  void createProgressDialog( const QString & text, int tS,
                             const QString & title, const QString & cancelButtonText );
  void destroyProgressDialog();
  void setProgress( int p );
  void setLabelText( const QString & l );
  void setTotalSteps( int tS );

  AQProgressDialog progressD_;
};

AQUiUtilPrivate::AQUiUtilPrivate()
{
  progressD_.setAutoReset( false );
  progressD_.setAutoClose( false );
  progressD_.setWindowIcon( QIcon( ":/images/icon.png" ) );
}

void
AQUiUtilPrivate::createProgressDialog( const QString & text, int tS,
                                       const QString & title, const QString & cancelButtonText )
{
  progressD_.ignoreHide_ = true;
  progressD_.setRange( 0, tS );
  progressD_.setWindowTitle( title );
  progressD_.setLabelText( text );
  if ( !cancelButtonText.isEmpty() )
    progressD_.setCancelButtonText( cancelButtonText );
  progressD_.show();
}

void AQUiUtilPrivate::destroyProgressDialog()
{
  progressD_.ignoreHide_ = false;
  progressD_.hide();
}

void AQUiUtilPrivate::setProgress( int p )
{
  progressD_.setValue( p );
}

void AQUiUtilPrivate::setLabelText( const QString & l )
{
  progressD_.setLabelText( l );
}

void AQUiUtilPrivate::setTotalSteps( int tS )
{
  progressD_.setRange( 0, tS );
}

AQUiUtil::AQUiUtil() : d( 0 )
{}

AQUiUtil::~AQUiUtil()
{
  if ( d )
    delete d;
}

void
AQUiUtil::createProgressDialog( const QString & text, int tS,
                                const QString & title, const QString & cancelButtonText )
{
  if ( !d )
    d = new AQUiUtilPrivate();
  d->createProgressDialog( text, tS, title, cancelButtonText );
}

void AQUiUtil::destroyProgressDialog() const
{
  if ( d )
    d->destroyProgressDialog();
}

void AQUiUtil::setProgress( int p ) const
{
  if ( d )
    d->setProgress( p );
}

void AQUiUtil::setLabelText( const QString & l ) const
{
  if ( d )
    d->setLabelText( l );
}

void AQUiUtil::setTotalSteps( int tS ) const
{
  if ( d )
    d->setTotalSteps( tS );
}

QImage AQUiUtil::snapShotUI( const QString & /*n*/ ) const
{
  return QImage();
}

void AQUiUtil::saveSnapShotUI( const QString & /*n*/, const QString & /*pathFile*/ ) const
  {}

void AQUiUtil::saveIconFile( const QString & /*data*/, const QString & /*pathFile*/ ) const
  {}

AQUi::StandardButton AQUiUtil::msgBoxInfo( const QString & title, const QString & text,
    AQUi::StandardButtons buttons,
    AQUi::StandardButton defaultButton ) const
{
  return ( AQUi::StandardButton )
         QMessageBox::information( ( AbanQ->activeMainWindow() ?
                                     AbanQ->activeMainWindow() : AbanQ->activeWindow() ),
                                   title, text, buttons, defaultButton );
}

AQUi::StandardButton AQUiUtil::msgBoxQuestion( const QString & title, const QString & text,
    AQUi::StandardButtons buttons,
    AQUi::StandardButton defaultButton ) const
{
  return ( AQUi::StandardButton )
         QMessageBox::question( ( AbanQ->activeMainWindow() ?
                                  AbanQ->activeMainWindow() : AbanQ->activeWindow() ),
                                title, text, buttons, defaultButton );
}

AQUi::StandardButton AQUiUtil::msgBoxWarn( const QString & title, const QString & text,
    AQUi::StandardButtons buttons,
    AQUi::StandardButton defaultButton ) const
{
  return ( AQUi::StandardButton )
         QMessageBox::warning( ( AbanQ->activeMainWindow() ?
                                 AbanQ->activeMainWindow() : AbanQ->activeWindow() ),
                               title, text, buttons, defaultButton );
}

AQUi::StandardButton AQUiUtil::msgBoxCritical( const QString & title, const QString & text,
    AQUi::StandardButtons buttons,
    AQUi::StandardButton defaultButton ) const
{
  return ( AQUi::StandardButton )
         QMessageBox::critical( ( AbanQ->activeMainWindow() ?
                                  AbanQ->activeMainWindow() : AbanQ->activeWindow() ),
                                title, text, buttons, defaultButton );
}
