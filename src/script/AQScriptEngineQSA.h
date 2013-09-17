/***************************************************************************
AQScriptEngineQSA.h
-------------------
begin                : 24/09/2007
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

#ifndef AQSCRIPTENGINEQSA_H_
#define AQSCRIPTENGINEQSA_H_

#include <QVariant>

#include <AQScript/AQScript.h>

class AQScriptEngineQSAPrivate;
class AQSqlDatabase;
class AQSqlCursor;
class AQObjectInterfaceQSA;
class QSWorkbench;

class AQScriptEngineQSA
{
public:

  QVariant call( const QString & function,
                 const QVariantList & arguments = QVariantList(),
                 QObject * objectContext = 0 ) const;

  QVariant call( const QString & function,
                 const QVariantList & arguments = QVariantList(),
                 const QString & nameObjectContext = QString() ) const;

  QVariant call( AQSqlCursor * cursor, const QString & function,
                 const QVariantList & arguments = QVariantList(),
                 const QString & nameObjectContext = QString() ) const;

  AQObjectInterfaceQSA *
  createInterface( const QString & className,
                   const QVariantList & arguments = QVariantList(),
                   QObject * context = 0 ) const;

  AQObjectInterfaceQSA *
  createInterfaceAQ2( const QString & className,
                      const QVariantList & arguments = QVariantList(),
                      QObject * context = 0 ) const;

  AQObjectInterfaceQSA * createInterface( AQSharedObjectQSA obj ) const;

  AQObjectInterfaceQSA * createInterfaceAQ2( AQSharedObjectQSA obj ) const;

  AQObjectInterfaceQSA * createInterface( QObject * obj ) const;

  AQObjectInterfaceQSA * createInterfaceAQ2( QObject * obj ) const;

  QSWorkbench * workbench() const;

  bool isReady() const;

  QString connectionName() const;

  QString aqScriptAndLineState() const;

  QObject * currentContext() const;

  void reevaluate();

  static void registerInterfaceMetaObjects();

private:

  inline AQScriptEngineQSA()
  {
    d = 0;
  }

  inline AQScriptEngineQSA( const AQScriptEngineQSA & other )
  {
    d = other.d;
  }

  inline AQScriptEngineQSA & operator=( const AQScriptEngineQSA & other )
  {
    d = other.d;
    return *this;
  }

  inline ~AQScriptEngineQSA()
  {}

  bool init( const AQSqlDatabase & db );
  void finish();

  AQSqlDatabase db() const;

  AQScriptEngineQSAPrivate * d;

  friend class AQSqlDatabase;
  friend class AQSqlDatabasePrivate;
  friend class AQObjectInterfaceQSA;
};

inline bool AQScriptEngineQSA::isReady() const
{
  return !!d;
}

class AQWorkbench : public QObject
{
  Q_OBJECT

public:

  AQWorkbench( AQScriptEngineQSA * eng ) :
      eng_( eng ),
      wb_( 0 )
  {}

  ~AQWorkbench();

  QSWorkbench * wb() const;

  void setWb( QSWorkbench * wb );

public Q_SLOTS:

  void evaluate();

private:

  AQScriptEngineQSA * eng_;
  QSWorkbench * wb_;
};

inline QSWorkbench * AQWorkbench::wb() const
{
  return wb_;
}

inline void AQWorkbench::setWb( QSWorkbench * wb )
{
  wb_ = wb;
}

#endif /*AQSCRIPTENGINEQSA_H_*/
