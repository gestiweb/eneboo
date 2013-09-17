/***************************************************************************
AQVarInterface.h
-------------------
begin                : 01/03/2009
copyright            : (C) 2003-2009 by InfoSiAL S.L.
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

#ifndef AQVARINTERFACE_H_
#define AQVARINTERFACE_H_

#include <AQScript/AQObjectInterfaceQSA.h>

class AQVar;

/**
Interfaz para AQVar
*/
class AQVarInterface : public AQObjectInterfaceQSA
{
  Q_OBJECT

  Q_PROPERTY( bool undefined READ isUndefined )

public:

  AQVar * obj_;

public Q_SLOTS:

  /**
  Establece un valor en la tabla de variables de sesión

  @param  n Nombre de la variable
  @param  v Valor asociado
  @return Verdadero en caso de establecer el valor con éxito, falso en cualquier otro caso
  */
  bool set( const QString & n, const QVariant & v );

  /**
  Lee un valor en la tabla de variables de sesión

  @param  n Nombre de la variable a leer
  @return Valor asociado, en caso de leer el valor con éxito, falso en cualquier otro caso
  */
  QVariant get( const QString & n ) const;

protected Q_SLOTS:

  /**
  @return Si la interfaz es indefinida, es decir, no tiene objeto subyacente
  */
  bool isUndefined() const;

protected:

  friend class AQSqlObjectFactoryQSA;

  /**
  Constructor
  */
  AQVarInterface( const AQScriptEngineQSA * eng,
                  const QString & connName = "default" );

  /**
  Destructor
  */
  virtual ~AQVarInterface();
};

inline bool AQVarInterface::isUndefined() const
{
  return !obj_;

}

#endif /*AQVARINTERFACE_H_*/
