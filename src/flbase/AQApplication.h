/***************************************************************************
 AQApplication.h
 -------------------
 begin                : 29/03/2011
 copyright            : (C) 2003-2011 by InfoSiAL S.L.
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

#ifndef AQAPPLICATION_H_
#define AQAPPLICATION_H_

#include "AQGlobal.h"
#include "FLApplication_p.h"

class AQApplicationPrivate;

extern AQ_EXPORT FLApplication *aqApp;

class AQ_EXPORT AQApplication : public FLApplication
{
  Q_OBJECT

public:
  AQApplication(int &argc, char **argv);
  virtual ~AQApplication();

  void init(const QString &n = QString::null,
            const QString &callFunction = QString::null,
            const QString &arguments = QString::null,
            bool quitAfterCall = false, bool noMax = false);

  void reinit();

public slots:
  void setMainWidget(QWidget *mainWidget);

  QSArgument call(const QString &function,
                  const QString &nameObjectContext) const;

  AQApplication *self();

  void setProxyDesktop(QWidget *w);

private slots:
  void callInitScript();
  void callReinitScript();
  void callReinitScriptDelayed();
  void resizeProxyDesktop(int s);

private:
  AQApplicationPrivate *d;
};

#endif /* AQAPPLICATION_H_ */
