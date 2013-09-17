/***************************************************************************
AQLoadingIndicator.cpp
-------------------
begin                : 09/02/2010
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

#include <WEnvironment>
#include <WImage>
#include <WText>

#include <AQCore/AQCoreUtil.h>

#include "../AQUi.h"
#include "WQApplication.h"
#include "AQLoadingIndicator.h"

AQLoadingIndicator::AQLoadingIndicator()
{
  setInline( false );

  WApplication *app = WApplication::instance();

  cover_ = new WContainerWidget( this );
  center_ = new WContainerWidget( this );

  WImage *img = new WImage( "images/logo_abanq.png", center_ );
  img->setMargin( 7, Top | Bottom );

  text_ = new WText( toWString( AQCoreUtil::translate( "LoadingIndicator", "Cargando..." ) ), center_ );
  text_->setInline( false );
  text_->setMargin( WLength::Auto, Left | Right );

  WImage *img2 = new WImage( WApplication::resourcesUrl() + "ajax-loading.gif", center_ );
  img2->setMargin( 7, Top | Bottom );

  if ( app->environment().agentIsIE() )
    app->styleSheet().addRule( "body", "height: 100%; margin: 0;" );

  app->styleSheet().addRule( "div#" + cover_->id(), std::string() + "background: #DDDDDD;"
                             "height: 100%; width: 100%;"
                             "top: 0px; left: 0px;"
                             "opacity: 0.5; position: absolute;"
                             "-khtml-opacity: 0.5;"
                             "z-index: 10000;" + ( app->environment().agentIsIE() ? "filter: alpha(opacity=50);" : "-moz-opacity:0.5;"
                                                   "-moz-background-clip: -moz-initial;"
                                                   "-moz-background-origin: -moz-initial;"
                                                   "-moz-background-inline-policy: -moz-initial;" ) );

  app->styleSheet().addRule( "div#" + center_->id(), "background: #dfe8f6;"
                             "border: 3px solid #333333;"
                             "z-index: 10001; visibility: visible;"
                             "position: absolute; left: 50%; top: 50%;"
                             "margin-left: -100px; margin-top: -85px;"
                             "width: 200px; height: 170px;"
                             "font-family: arial,sans-serif;"
                             "text-align: center" );
}

void AQLoadingIndicator::setMessage( const WString& text )
{
  text_->setText( text );
}
