/***************************************************************************
 main.cpp
 -------------------
 begin                : Sat Mar  1 19:39:06 CET 2014
 copyright            : (C) 2013-2013 by Gestiweb, S.L.
 email                : info#gestiweb.com
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

#ifndef FCGIOBJECTFACTORY_H
#define FCGIOBJECTFACTORY_H

// C Libraries ----
#include <stdlib.h>
#include <stdio.h>
// ----

#include <qsplashscreen.h>


#include <qsobjectfactory.h>
#include <qapplication.h>

#define NO_FCGI_DEFINES
#include "fcgi_stdio.h"

#include "AQConfig.h"
extern char **environ;


class FCgiObjectFactory : public QSObjectFactory
{
public:
  FCgiObjectFactory();
  ~FCgiObjectFactory();

  QObject *create(const QString &className, const QSArgumentList &arguments, QObject *);
};

class FLFastCgiInterface : public QObject
{

	Q_OBJECT

public:
	FLFastCgiInterface() : QObject(0) {
	}

	virtual ~FLFastCgiInterface() {}

public slots:
	QStringList getFullEnv() {
	  char *s = *environ;
	  int i = 1;
	  QStringList ret;
	  for (; s; i++) {
		QString qs = QString::fromAscii(s);  
		ret.append(qs);
		s = *(environ+i);
	  }
	  return ret;
	}

	QString getEnv(const QString &name) {
		char *cret = getenv(name.ascii());
		if (cret == NULL) return QString::null;
		QString ret = QString::fromAscii(cret); 
		return ret;
	}

//DLLAPI size_t     FCGI_fread(void *ptr, size_t size, size_t nmemb, FCGI_FILE *fp);


//DLLAPI size_t     FCGI_fwrite(void *ptr, size_t size, size_t nmemb, FCGI_FILE *fp);
	int write(QString data) {
		void *ptr = (void *) data.ascii();
		int sz = data.length();
		return FCGI_fwrite(ptr, sizeof(char), sz, FCGI_stdout);
	}
	
	int writeError(QString data) {
		void *ptr = (void *) data.ascii();
		int sz = data.length();
		return FCGI_fwrite(ptr, sizeof(char), sz, FCGI_stderr);
	}
};




#endif

