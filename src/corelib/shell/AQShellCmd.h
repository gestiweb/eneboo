/***************************************************************************
AQShellCmd.h
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

#ifndef AQSHELLCMD_H_
#define AQSHELLCMD_H_

#include <QString>
#include <QStringList>

#include <AQCore/AQCore.h>

/**
Estructura general para crear
comandos de shell
 
@authos InfoSiAL S.L.
*/
class AQShellCmd
{
public:

  AQShellCmd();
  ~AQShellCmd();

  /**
  Nombre del comando
  */
  QString cmd;

  /**
  Argumentos
  */
  QStringList argList;

  /**
  Manejador de mensajes de salida del comando hacia la consola
  */
  AQConsoleMsgHandler msgHandler;

  /**
  Objeto creador del comando
  */
  QObject * objCreator;
};

#endif /*AQSHELLCMD_H_*/
