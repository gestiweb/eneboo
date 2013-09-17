/***************************************************************************
WQDialog.h
-------------------
begin                : 24/07/2008
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

#ifndef WQDIALOG_H_
#define WQDIALOG_H_

#include <WObject>

#include <AQCore/AQObject.h>

class WQWidget;
class WQEventLoop;

using namespace Wt;

class WQDialog : public AQObject, public WObject
{
  Q_OBJECT

public:

  explicit WQDialog();

  enum DialogCode {
    Rejected, Accepted
  };

  int result() const;
  void setResult( int r );
  int exec();

Q_SIGNALS:

  void finished( int );
  void accepted();
  void rejected();

public Q_SLOTS:

  virtual void done( int );
  virtual void accept();
  virtual void reject();

protected:

  virtual WQWidget * widget() = 0;

private:

  WQEventLoop * eventLoop_;
  int rCode_;
};

#endif /*WQDIALOG_H_*/
