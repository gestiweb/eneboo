/***************************************************************************
                     AQSqlShell.h
                 -------------------
begin                : 04/09/2007
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

#ifndef AQSQLCONSOLE_H_
#define AQSQLCONSOLE_H_

#include <AQCore/AQCore.h>
#include <AQCore/AQShell.h>

#include <QString>

class AQSqlShellPrivate;
class AQSqlDir;
class AQSqlDatabase;
class AQShellCmd;

/**
Intérprete de comandos para el motor SQL de AbanQ
Utiliza el sistema de ficheros SQL
 
@author InfoSiAL S.L.
*/
class AQSqlShell : public AQShell
{

public:

  /**
  constructor

  @param  db Base de datos sobre la que montar el sistema de ficheros
  @param path Ruta del directorio inicial. Si es vacia se inicia en el raiz.
  */
  AQSqlShell( AQSqlDatabase db, const QString & entryDir = QString() );

  /**
  destructor
  */
  ~AQSqlShell();

  /**
  Ejecuta comando

  @param cmd Comando a ejecutar
  @return Texto con salida del comando, en formato Html
  */
  QStringList cmd( const AQShellCmd & cmd ) const;

  /**
  Para autocomplementar comandos

  @param cmd Comando a expandir
  @return Lista de posible expansiones
  */
  QStringList expand( const AQShellCmd & cmd ) const;

  /**
  @return Hash por nombre de los comandos soportados
  */
  AQShellCmdInfoHash suppCmds() const;

private:

  /**
  Privado
  */
  AQSqlShellPrivate * d;
};

#endif /*AQSQLCONSOLE_H_*/
