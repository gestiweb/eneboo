/***************************************************************************
AQShell.h
-------------------
begin                : 05/09/2007
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

#ifndef AQSHELL_H_
#define AQSHELL_H_

#include <AQCore/AQCore.h>

class AQShellCmd;

/**
Clase abstracta para crear intérpretes de
comandos sobre los motores de AbanQ.

Ver como ejemplo AQSqlShell, como intérprete de
comandos para el motor SQL de AbanQ.

@author InfoSiAL S.L.
*/
class AQShell
{
public:

  /**
  Constructor
  */
  AQShell();

  /**
  Destructor
  */
  virtual ~AQShell();

  /**
  Ejecuta comando

  @param  cmd Comando a ejecutar
  @return Texto con salida del comando, en formato Html
  */
  virtual QStringList cmd( const AQShellCmd & cmd ) const = 0;

  /**
  Para autocomplementar comandos

  @param  cmd Comando a expandir
  @return Lista de posible expansiones
  */
  virtual QStringList expand( const AQShellCmd & cmd ) const = 0;

  /**
  @return Lista de los comandos soportados
  */
  virtual AQShellCmdInfoHash suppCmds() const = 0;
};

#endif /*AQSHELL_H_*/
