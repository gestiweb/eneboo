/***************************************************************************
                   AQSql.h
               -------------------
begin                : 15/08/2007
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

#ifndef AQSQL_H_
#define AQSQL_H_

#include <QHash>
#include <QString>

class AQParameterQuery;
class AQGroupByQuery;
class AQSqlNodeInfo;

/**
Tipo de datos diccionario de parametros
*/
typedef QHash<QString, AQParameterQuery *> AQParameterQryHash;
/**
Tipo de datos diccionaro de grupos
*/
typedef QHash<QString, AQGroupByQuery *> AQGroupByQryHash;
/**
Lista de nodos de información del sistema de ficheros SQL
*/
typedef QList<AQSqlNodeInfo> AQSqlNodeInfoList;


namespace AQSql
{
  /**
  Constantes para indicar el tipo de acceso al cursor
  */
  enum Mode {
    /** Insertar, en este modo el buffer se prepara para crear un nuevo registro */
    INSERT = 0,
    Insert = 0,
    /** Edición, en este modo el buffer se prepara para editar el registro activo */
    EDIT = 1,
    Edit = 1,
    /** Borrar, en este modo el buffer se prepara para borrar el registro activo */
    DEL = 2,
    Del = 2,
    /** Navegacion, en este modo solo se puede visualizar el buffer */
    BROWSE = 3,
    Browse = 3,
    /** Insertar copiando actual, si el buffer actual es vacio funciona igual que INSERT
        sino es vacio primero lo guarda, prepara el buffer para crear un nuevo registro y
        despues copia el contenido del anteriormente guardado */
    INSERTCOPY = 4,
    InsertCopy = 4
  };

  /**
  Constantes para indicar el tipo de condicion para evaluar si se aplica o no el control de acceso
  */
  enum AcosConditionEval {
    /** evalua un valor fijo */
    VALUE = 0,
    Value = 0,
    /** evalua una expresion regular */
    REGEXP = 1,
    RegExp = 1,
    /** evalua el valor devuelto por una funcion de script */
    FUNCTION = 2,
    Function = 2
  };

  /**
  Enumeración de opciones de conexión
  */
  enum ConnOptions {
    /** Usuario */
    USER = 0,
    User = 0,
    /** Contraseña */
    PASSWORD = 1,
    Password = 1,
    /** Puerto */
    PORT = 2,
    Port = 2,
    /** Servidor */
    HOST = 3,
    Host = 3
  };

  /**
  Atributos para nodos del sistema de ficheros SQL
  */
  enum SqlFlagNode
  {
    NoSet = 0x00000000,
    Dir = 0x00100000,
    File = 0x00200000,
    BinFile = 0x00000400,
    CharFile = 0x00000800,
    Ui = 0x00001000,
    Qry = 0x00002000,
    Kut = 0x00004000,
    Mtd = 0x00008000,
    Ts = 0x00010000,
    Act = 0x00020000,
    Acl = 0x00040000,
    Img = 0x00080000
  };

  Q_DECLARE_FLAGS( SqlFlagsNode, SqlFlagNode )
}

Q_DECLARE_OPERATORS_FOR_FLAGS( AQSql::SqlFlagsNode )

#endif /*AQSQL_H_*/
