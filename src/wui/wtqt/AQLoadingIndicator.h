/***************************************************************************
AQLoadingIndicator.h
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

#ifndef AQLOADINGINDICATOR_H_
#define AQLOADINGINDICATOR_H_

#include <WLoadingIndicator>
#include <WContainerWidget>

namespace Wt
{
  class WText;
}

using namespace Wt;

class AQLoadingIndicator : public Wt::WContainerWidget, public Wt::WLoadingIndicator
{
public:

  AQLoadingIndicator();

  virtual WWidget *widget()
  {
    return this;
  }

  virtual void setMessage( const WString & text );

private:

  WContainerWidget * cover_;
  WContainerWidget * center_;
  WText * text_;
};

#endif /* AQLOADINGINDICATOR_H_ */
