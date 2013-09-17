/***************************************************************************
AQObjectInterfaceQSA.h
-------------------
begin                : 20/10/2007
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

#ifndef AQOBJECTINTERFACEQSA_H_
#define AQOBJECTINTERFACEQSA_H_

#include <QObject>
#include <QString>

class AQScriptEngineQSA;
class AQSqlDatabase;

class AQObjectInterfaceQSA : public QObject
{
  Q_OBJECT

  Q_PROPERTY( QString objectName READ objectName )

protected:

  AQObjectInterfaceQSA( const AQScriptEngineQSA * eng, QObject * parent = 0 );

  virtual ~AQObjectInterfaceQSA()
  {}

  AQSqlDatabase dbCon( const QString & conn = "default" ) const;
  QString dbConName( const QString & conn = "default" ) const;

  const AQScriptEngineQSA * engine;

private:

  QString dbConName_;
};

#endif /*AQOBJECTINTERFACEQSA_H_*/
