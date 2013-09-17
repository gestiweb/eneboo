/***************************************************************************
AQSqlInterface.h
-------------------
begin                : 23/02/2009
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

#ifndef AQSQLINTERFACE_H_
#define AQSQLINTERFACE_H_

#include <AQSql/AQSql.h>

/**
Interfaz estática para módulo AQSql
*/
class AQSqlInterfaceStatic : public QObject
{
  Q_OBJECT

  Q_ENUMS( Mode )
  Q_ENUMS( AcosConditionEval )
  Q_ENUMS( ConnOptions )

  Q_PROPERTY( QString objectName READ objectName WRITE setObjectName SCRIPTABLE false )

public:

  /**
  Constantes para indicar el tipo de acceso al cursor
  */
  enum Mode {
    /** Insertar, en este modo el buffer se prepara para crear un nuevo registro */
    Insert = AQSql::INSERT,
    /** Edición, en este modo el buffer se prepara para editar el registro activo */
    Edit = AQSql::EDIT,
    /** Borrar, en este modo el buffer se prepara para borrar el registro activo */
    Del = AQSql::DEL,
    /** Navegacion, en este modo solo se puede visualizar el buffer */
    Browse = AQSql::BROWSE,
    /** Insertar copiando actual, si el buffer actual es vacio funciona igual que INSERT
        sino es vacio primero lo guarda, prepara el buffer para crear un nuevo registro y
        despues copia el contenido del anteriormente guardado */
    InsertCopy = AQSql::INSERTCOPY
  };

  /**
  Constantes para indicar el tipo de condicion para evaluar si se aplica o no el control de acceso
  */
  enum AcosConditionEval {
    /** evalua un valor fijo */
    Value = AQSql::VALUE,
    /** evalua una expresion regular */
    Regexp = AQSql::REGEXP,
    /** evalua el valor devuelto por una funcion de script */
    Function = AQSql::FUNCTION
  };

  /**
  Enumeración de opciones de conexión
  */
  enum ConnOptions {
    /** Usuario */
    User = AQSql::USER,
    /** Contraseña */
    Password = AQSql::PASSWORD,
    /** Puerto */
    Port = AQSql::PORT,
    /** Servidor */
    Host = AQSql::HOST
  };
};

#endif /*AQSQLINTERFACE_H_*/
