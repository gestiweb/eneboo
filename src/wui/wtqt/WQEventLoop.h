/***************************************************************************
WQEventLoop.h
-------------------
begin                : 17/07/2008
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

#ifndef WQEVENTLOOP_H_
#define WQEVENTLOOP_H_

#include <WObject>

class WQWidget;
class WQEventLoopThread;

using namespace Wt;

class WQEventLoop : public WObject
{
public:

  WQEventLoop( WQWidget * widget );

  void execLoop();
  void exitLoop();

private:

  ~WQEventLoop();

  WQEventLoopThread * thread_;
  WQWidget * widget_;
};

#endif /*WQEVENTLOOP_H_*/
