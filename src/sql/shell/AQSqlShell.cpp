/***************************************************************************
AQSqlShell.cpp
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

#include <QSqlError>
#include <QSqlRecord>

#include <qslexer.h>
#include <qserrors.h>

#include <AQCore/AQShellCmd.h>
#include <AQCore/AQShellCmdInfo.h>
#include <AQCore/AQCoreUtil.h>
#include <AQScript/AQScriptEngineQSA.h>
#include <AQScript/AQCompilerQSA.h>

#include "AQSqlShell.h"
#include "../filesystem/AQSqlDir.h"
#include "../filesystem/AQSqlFileSystem.h"
#include "../filesystem/AQSqlNodeInfo.h"
#include "../manager/AQManagerModules.h"

class AQSqlShellCmdExec
{

public:

  AQSqlShellCmdExec( AQSqlShellPrivate * d_ ) : d( d_ )
  {}
  virtual ~AQSqlShellCmdExec()
  {}
  virtual AQShellCmdInfo info() const = 0;
  virtual QStringList exec( const AQShellCmd & shellCmd ) const = 0;
  virtual QStringList expand( const QStringList & ) const
  {
    return QStringList() << info().cmd;
  }

protected:

  AQSqlShellPrivate * d;
};

typedef QHash<QString, AQSqlShellCmdExec *> AQSqlShellCmdExecHash;

class AQSqlShellPrivate
{

public:

  AQSqlShellPrivate( AQSqlDir * dir_ );
  ~AQSqlShellPrivate();

  /**
  Auxiliares
  */
  QStringList expandWildcards( const QString & path ) const;
  QStringList formatHtmlTable( const QStringList & list ) const;
  QStringList consoleMsg( const AQShellCmd & shellCmd, AQMsgType type, AQMsgFormat format,
                          const QString & msg ) const;

  /**
  Registrar ejecutor de comando
  */
  void registerExec( AQSqlShellCmdExec * exe );

  /**
  Para el manejo de directorios y ficheros
  */
  AQSqlDir * dir;

  /**
  Comandos soportados
  */
  AQShellCmdInfoHash suppCmds;

  /**
  Ejecutores de comandos
  */
  AQSqlShellCmdExecHash execCmds;
};

AQSqlShellPrivate::AQSqlShellPrivate( AQSqlDir * dir_ ) : dir( dir_ )
{}

AQSqlShellPrivate::~AQSqlShellPrivate()
{
  delete dir;

  suppCmds.clear();

  while ( !execCmds.isEmpty() ) {
    AQSqlShellCmdExec * e = execCmds.begin().value();
    execCmds.erase( execCmds.begin() );
    if ( e )
      delete e;
  }
}

void AQSqlShellPrivate::registerExec( AQSqlShellCmdExec * exe )
{
  suppCmds.insert( exe->info().cmd, exe->info() );
  execCmds.insert( exe->info().cmd, exe );
}

QStringList AQSqlShellPrivate::expandWildcards( const QString & path ) const
{
  QStringList ret;
  QString curPath = dir->absolutePath();
  QStringList pathList( path.split( "/", QString::SkipEmptyParts ) );

  if ( !pathList.isEmpty() ) {
    QString prefix, name, suffix;

    name = pathList.takeLast();
    if ( !pathList.isEmpty() )
      suffix = "/";
    if ( path.startsWith( "/" ) )
      prefix = "/" + pathList.join( "/" ) + suffix;
    else
      prefix = pathList.join( "/" ) + suffix;

    if ( name.contains( '*' ) || name.contains( '?' ) ) {
      name.replace( '%', QLatin1String( "|%" ) );
      name.replace( '_', QLatin1String( "|_" ) );
      name.replace( '*', QLatin1String( "%" ) );
      name.replace( '?', QLatin1String( "_" ) );

      ret.clear();
      AQSqlNodeInfoList names( dir->ls( prefix, "nombre like '" + name + "' escape '|'" ) );
      for ( AQSqlNodeInfoList::const_iterator it = names.begin(); it != names.end(); ++it ) {
        if ( ( *it ).name == "." || ( *it ).name == ".." )
          continue;
        if ( ( *it ).flag.testFlag( AQSql::Dir ) )
          ret << QLatin1String( "D:" ) + prefix + ( *it ).name + "/";
        else
          ret << QLatin1String( "A:" ) + prefix + ( *it ).name;
      }
    }
  }

  dir->cd( curPath );
  return ret;
}

QStringList AQSqlShellPrivate::formatHtmlTable( const QStringList & list ) const
{
  QStringList ret;
  QStringList items;
  QString item;
  for ( int i = 0; i < list.size(); ++i ) {
    item = "<tr>";
    items = list.at( i ).split( "||", QString::SkipEmptyParts );
    for ( int j = 0; j < items.size(); ++j )
      item += "<td>" + items.at( j ) + "</td>";
    item += "</tr>";
    ret << item;
  }
  return ret;
}

QStringList
AQSqlShellPrivate::consoleMsg( const AQShellCmd & shellCmd, AQMsgType type, AQMsgFormat format,
                               const QString & msg ) const
{
  QStringList ret;

  if ( shellCmd.msgHandler && shellCmd.objCreator )
    ( *shellCmd.msgHandler ) ( type, format, shellCmd.objCreator,
                               msg.toLatin1() );
  else
    ret << QString( "<tr><td>%1</td></tr>" ).arg( msg );

  return ret;
}

///////////////////////////////////////////////////////////////////

class pwdExec : public AQSqlShellCmdExec
{

public:

  pwdExec( AQSqlShellPrivate * d_ ) : AQSqlShellCmdExec( d_ )
  {}

  AQShellCmdInfo info() const
  {
    AQShellCmdInfo i;
    i.cmd = "pwd";
    i.numArgs = 0;
    return i;
  }

  QStringList exec( const AQShellCmd & ) const
  {
    QStringList ret;
    ret << d->dir->absolutePath();
    return ret;
  }
};

class lsExec : public AQSqlShellCmdExec
{

public:

  lsExec( AQSqlShellPrivate * d_ ) : AQSqlShellCmdExec( d_ )
  {}

  AQShellCmdInfo info() const
  {
    AQShellCmdInfo i;
    i.cmd = "ls";
    i.numArgs = 3;
    i.optArgs = 3;
    return i;
  }

  QStringList internalExec( const QString & arg1, const QString & arg2, const QString & arg3 ) const
  {
    QStringList ret;
    if ( arg1 != "-n" ) {
      AQSqlNodeInfoList list( d->dir->ls( arg1, arg2 ) );
      QString item;
      for ( AQSqlNodeInfoList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        if ( ( *it ).flag.testFlag( AQSql::Dir ) )
          item = "<tr><td>d";
        else
          item = "<tr><td>-";
        item += ( *it ).perms + "</td>";
        item += "<td></td>";
        item += "<td align=right>" + QString::number( ( *it ).idParentNode ) + "</td>";
        item += "<td></td>";
        item += "<td align=right>" + QString::number( ( *it ).idNode() ) + "</td>";
        item += "<td></td>";
        item += "<td>" + ( *it ).idUser.leftJustified( 10 ) + "</td>";
        item += "<td></td>";
        item += "<td>" + ( *it ).idGroup + "</td>";
        item += "<td></td>";
        item += "<td>" + ( *it ).dateMod() + "</td>";
        item += "<td></td>";
        if ( ( *it ).flag.testFlag( AQSql::Dir ) )
          item += "<td><b>" + ( *it ).name + "</b>" + "</td>";
        else
          item += "<td>" + ( *it ).name + "</td>";
        item += "</tr>";
        ret << item;
      }
    } else
      ret << "<tr><td>" + d->dir->lsNames( arg2, arg3 ).join( "</td><td></td><td>" ) + "</td></tr>";
    return ret;
  }

  QStringList exec( const AQShellCmd & shellCmd ) const
  {
    QStringList argList( shellCmd.argList );
    QString arg1, arg2, arg3;
    int nArgs = argList.size();
    if ( nArgs > 0 )
      arg1 = argList.at( 0 );
    if ( nArgs > 1 )
      arg2 = argList.at( 1 );
    if ( nArgs > 2 )
      arg3 = argList.at( 2 );

    QString dirName = ( arg1 != "-n" ? arg1 : arg2 );
    QStringList list( d->expandWildcards( dirName ) );

    if ( !list.isEmpty() ) {
      QStringList ret;
      QString item;
      list.sort();
      for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        item = *it;
        item.remove( 0, 2 );
        if ( ( *it ).left( 2 ) == "D:" ) {
          item.chop( 1 );
          ret << "<tr><tr><tr><td>" + item + ":</td></tr>";
        }
        if ( dirName == arg1 )
          ret << internalExec( item, arg2, arg3 );
        else
          ret << internalExec( arg1, item, arg3 );
      }
      return ret;
    } else
      return internalExec( arg1, arg2, arg3 );
  }

  QStringList expand( const QStringList & argList ) const
  {
    QString arg1, arg2, arg3;
    int nArgs = argList.size();
    QStringList ret;

    if ( nArgs > 0 )
      arg1 = argList.at( 0 );
    if ( nArgs > 1 )
      arg2 = argList.at( 1 );
    if ( nArgs > 2 )
      arg3 = argList.at( 2 );

    QString dirName = ( arg1 != "-n" ? arg1 : arg2 );
    QStringList list( d->expandWildcards( dirName + "*" ) );

    if ( !list.isEmpty() ) {
      QString item;
      list.sort();

      for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        item = *it;
        item.remove( 0, 2 );

        if ( dirName == arg1 )
          ret << QString( info().cmd + " " + item + " " + arg2 + " " + arg3 ).simplified();
        else
          ret << QString( info().cmd + " " + arg1 + " " + item + " " + arg3 ).simplified();
      }
    } else
      ret = QStringList() << QString( info().cmd + " " + arg1 + " " + arg2 + " " + arg3 ).simplified();

    return ret;
  }
};

class mkdirExec : public AQSqlShellCmdExec
{

public:

  mkdirExec( AQSqlShellPrivate * d_ ) : AQSqlShellCmdExec( d_ )
  {}

  AQShellCmdInfo info() const
  {
    AQShellCmdInfo i;
    i.cmd = "mkdir";
    i.numArgs = 1;
    i.optArgs = 0;
    i.reqArgs = 1;
    return i;
  }

  QStringList exec( const AQShellCmd & shellCmd ) const
  {
    QStringList argList( shellCmd.argList );
    QStringList ret;
    QString dirName;

    if ( !argList.isEmpty() )
      dirName = argList.first();

    if ( !d->dir->mkdir( dirName ) ) {
      ret = d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                           AQ_TR( "SqlShell",
                                  "mkdir: %1: El directorio no se puede crear"
                                ).arg( dirName ) );
    }

    return ret;
  }

  QStringList expand( const QStringList & argList ) const
  {
    QString arg1;
    int nArgs = argList.size();
    QStringList ret;

    if ( nArgs > 0 )
      arg1 = argList.at( 0 );

    QStringList list( d->expandWildcards( arg1 + "*" ) );

    if ( !list.isEmpty() ) {
      QString item;
      list.sort();

      for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        item = *it;
        item.remove( 0, 2 );
        ret << QString( info().cmd + " " + item ).simplified();
      }
    } else
      ret = QStringList() << QString( info().cmd + " " + arg1 ).simplified();

    return ret;
  }
};

class mkfileExec : public AQSqlShellCmdExec
{

public:

  mkfileExec( AQSqlShellPrivate * d_ ) : AQSqlShellCmdExec( d_ )
  {}

  AQShellCmdInfo info() const
  {
    AQShellCmdInfo i;
    i.cmd = "mkfile";
    i.numArgs = 2;
    i.optArgs = 1;
    i.reqArgs = 1;
    return i;
  }

  QStringList exec( const AQShellCmd & shellCmd ) const
  {
    QStringList argList( shellCmd.argList );
    QStringList ret;
    QString fileName;
    QString content;
    int nArgs = argList.size();

    if ( nArgs > 0 )
      fileName = argList.at( 0 );

    if ( nArgs > 1 )
      content = argList.at( 1 );

    if ( !d->dir->mkfile( fileName, content ) ) {
      ret = d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                           AQ_TR( "SqlShell",
                                  "mkfile: %1: El fichero no se puede crear"
                                ).arg( fileName ) );
    }

    return ret;
  }

  QStringList expand( const QStringList & argList ) const
  {
    QString arg1;
    int nArgs = argList.size();
    QStringList ret;

    if ( nArgs > 0 )
      arg1 = argList.at( 0 );

    QStringList list( d->expandWildcards( arg1 + "*" ) );

    if ( !list.isEmpty() ) {
      QString item;
      list.sort();

      for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        item = *it;
        item.remove( 0, 2 );
        ret << QString( info().cmd + " " + item ).simplified();
      }
    } else
      ret = QStringList() << QString( info().cmd + " " + arg1 ).simplified();

    return ret;
  }
};

class cdExec : public AQSqlShellCmdExec
{

public:

  cdExec( AQSqlShellPrivate * d_ ) : AQSqlShellCmdExec( d_ )
  {}

  AQShellCmdInfo info() const
  {
    AQShellCmdInfo i;
    i.cmd = "cd";
    i.numArgs = 1;
    i.optArgs = 1;
    i.reqArgs = 0;
    return i;
  }

  QStringList exec( const AQShellCmd & shellCmd ) const
  {
    QStringList argList( shellCmd.argList );
    QStringList ret;
    QString dirName;

    if ( !argList.isEmpty() )
      dirName = argList.first();

    if ( !d->dir->cd( dirName ) ) {
      ret = d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                           AQ_TR( "SqlShell",
                                  "cd: %1: No existe el directorio"
                                ).arg( dirName ) );
    }

    return ret;
  }

  QStringList expand( const QStringList & argList ) const
  {
    QString arg1;
    int nArgs = argList.size();
    QStringList ret;

    if ( nArgs > 0 )
      arg1 = argList.at( 0 );

    QStringList list( d->expandWildcards( arg1 + "*" ) );

    if ( !list.isEmpty() ) {
      QString item;
      list.sort();

      for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        item = *it;
        item.remove( 0, 2 );
        ret << QString( info().cmd + " " + item ).simplified();
      }
    } else
      ret = QStringList() << QString( info().cmd + " " + arg1 ).simplified();

    return ret;
  }
};

class rmExec : public AQSqlShellCmdExec
{

public:

  rmExec( AQSqlShellPrivate * d_ ) : AQSqlShellCmdExec( d_ )
  {}

  AQShellCmdInfo info() const
  {
    AQShellCmdInfo i;
    i.cmd = "rm";
    i.numArgs = 2;
    i.optArgs = 1;
    i.reqArgs = 1;
    return i;
  }

  QStringList internalExec( const AQShellCmd & shellCmd,
                            const QString & arg1, const QString & arg2 ) const
  {
    QStringList ret;
    if ( arg1 == "-r" ) {
      if ( !d->dir->rm( arg2, true ) ) {
        ret = d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                             AQ_TR( "SqlShell",
                                    "rm: %1: El fichero o directorio no se puede borrar"
                                  ).arg( arg2 ) );
      }
    } else if ( !d->dir->rm( arg1 ) ) {
      ret = d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                           AQ_TR( "SqlShell",
                                  "rm: %1: El fichero o directorio no se puede borrar"
                                ).arg( arg1 ) );
    }

    return ret;
  }

  QStringList exec( const AQShellCmd & shellCmd ) const
  {
    QStringList argList( shellCmd.argList );
    QString arg1, arg2;
    QStringList ret;
    int nArgs = argList.size();

    if ( nArgs > 0 )
      arg1 = argList.at( 0 );
    if ( nArgs > 1 )
      arg2 = argList.at( 1 );

    QString dirName = ( arg1 != "-r" ? arg1 : arg2 );
    QStringList list( d->expandWildcards( dirName + "*" ) );

    if ( !list.isEmpty() ) {
      QString item;
      QString msgStatus( "rm %1" );
      list.sort();

      for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        item = *it;
        item.remove( 0, 2 );

        d->consoleMsg( shellCmd, AQLogMsg, AQPlainMsg, msgStatus.arg( item ) );

        if ( dirName == arg1 )
          ret << internalExec( shellCmd, item, arg2 );
        else
          ret << internalExec( shellCmd, arg1, item );
      }
    } else
      ret = internalExec( shellCmd, arg1, arg2 );

    return ret;
  }

  QStringList expand( const QStringList & argList ) const
  {
    QString arg1, arg2;
    int nArgs = argList.size();
    QStringList ret;

    if ( nArgs > 0 )
      arg1 = argList.at( 0 );
    if ( nArgs > 1 )
      arg2 = argList.at( 1 );

    QString dirName = ( arg1 != "-r" ? arg1 : arg2 );
    QStringList list( d->expandWildcards( dirName + "*" ) );

    if ( !list.isEmpty() ) {
      QString item;
      list.sort();

      for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        item = *it;
        item.remove( 0, 2 );
        if ( dirName == arg1 )
          ret << QString( info().cmd + " " + item + " " + arg2 ).simplified();
        else
          ret << QString( info().cmd + " " + arg1 + " " + item ).simplified();
      }
    } else
      ret = QStringList() << QString( info().cmd + " " + arg1 + " " + arg2 ).simplified();

    return ret;
  }
};

class mvExec : public AQSqlShellCmdExec
{

public:

  mvExec( AQSqlShellPrivate * d_ ) : AQSqlShellCmdExec( d_ )
  {}

  AQShellCmdInfo info() const
  {
    AQShellCmdInfo i;
    i.cmd = "mv";
    i.numArgs = 2;
    i.optArgs = 0;
    i.reqArgs = 2;
    return i;
  }

  QStringList exec( const AQShellCmd & shellCmd ) const
  {
    QStringList argList( shellCmd.argList );
    QStringList ret;
    QString arg1, arg2;
    int nArgs = argList.size();
    if ( nArgs > 0 )
      arg1 = argList.at( 0 );
    if ( nArgs > 1 )
      arg2 = argList.at( 1 );

    QStringList list( d->expandWildcards( arg1 + "*" ) );

    if ( !list.isEmpty() ) {
      QString item;
      QString msgStatus( "mv %1 --> " + arg2 );

      list.sort();

      for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        item = *it;
        item.remove( 0, 2 );

        d->consoleMsg( shellCmd, AQLogMsg, AQPlainMsg, msgStatus.arg( item ) );

        if ( !d->dir->mv( item, arg2 ) ) {
          ret << d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                                AQ_TR( "SqlShell",
                                       "mv: %1: %2: No se puede mover el origen al destino"
                                     ).arg( item ).arg( arg2 ) );
        }
      }
    } else if ( !d->dir->mv( arg1, arg2 ) ) {
      ret = d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                           AQ_TR( "SqlShell",
                                  "mv: %1: %2: No se puede mover el origen al destino"
                                ).arg( arg1 ).arg( arg2 ) );
    }

    return ret;
  }

  QStringList expand( const QStringList & argList ) const
  {
    QStringList ret;
    QString arg1, arg2;
    int nArgs = argList.size();
    if ( nArgs > 0 )
      arg1 = argList.at( 0 );
    if ( nArgs > 1 )
      arg2 = argList.at( 1 );

    if ( arg2.isEmpty() ) {
      QStringList list( d->expandWildcards( arg1 + "*" ) );
      if ( !list.isEmpty() ) {
        QString item;
        list.sort();
        for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
          item = *it;
          item.remove( 0, 2 );
          ret << QString( info().cmd + " " + item + " " + arg2 ).simplified();
        }
      }
    }

    if ( !arg2.isEmpty() ) {
      QStringList list = d->expandWildcards( arg2 + "*" );
      if ( !list.isEmpty() ) {
        QString item;
        list.sort();
        ret.clear();

        for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
          item = *it;
          item.remove( 0, 2 );
          ret << QString( info().cmd + " " + arg1 + " " + item ).simplified();
        }
      }
    }

    if ( ret.isEmpty() )
      ret << QString( info().cmd + " " + arg1 + " " + arg2 ).simplified();

    return ret;
  }
};

class cpExec : public AQSqlShellCmdExec
{

public:

  cpExec( AQSqlShellPrivate * d_ ) : AQSqlShellCmdExec( d_ )
  {}

  AQShellCmdInfo info() const
  {
    AQShellCmdInfo i;
    i.cmd = "cp";
    i.numArgs = 2;
    i.optArgs = 0;
    i.reqArgs = 2;
    return i;
  }

  QStringList exec( const AQShellCmd & shellCmd ) const
  {
    QStringList argList( shellCmd.argList );
    QStringList ret;
    QString arg1, arg2;
    int nArgs = argList.size();

    if ( nArgs > 0 )
      arg1 = argList.at( 0 );
    if ( nArgs > 1 )
      arg2 = argList.at( 1 );

    QStringList list( d->expandWildcards( arg1 + "*" ) );

    if ( !list.isEmpty() ) {
      QString item;
      QString msgStatus( "cp %1 --> " + arg2 );

      list.sort();

      for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        item = *it;
        item.remove( 0, 2 );

        d->consoleMsg( shellCmd, AQLogMsg, AQPlainMsg, msgStatus.arg( item ) );

        if ( !d->dir->cp( item, arg2 ) ) {
          ret << d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                                AQ_TR( "SqlShell",
                                       "cp: %1: %2: No se puede copiar el origen al destino"
                                     ).arg( item ).arg( arg2 ) );
        }
      }
    } else if ( !d->dir->cp( arg1, arg2 ) ) {
      ret = d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                           AQ_TR( "SqlShell",
                                  "cp: %1: %2: No se puede copiar el origen al destino"
                                ).arg( arg1 ).arg( arg2 ) );
    }

    return ret;
  }

  QStringList expand( const QStringList & argList ) const
  {
    QStringList ret;
    QString arg1, arg2;
    int nArgs = argList.size();
    if ( nArgs > 0 )
      arg1 = argList.at( 0 );
    if ( nArgs > 1 )
      arg2 = argList.at( 1 );

    if ( arg2.isEmpty() ) {
      QStringList list( d->expandWildcards( arg1 + "*" ) );
      if ( !list.isEmpty() ) {
        QString item;
        list.sort();
        for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
          item = *it;
          item.remove( 0, 2 );
          ret << QString( info().cmd + " " + item + " " + arg2 ).simplified();
        }
      }
    }

    if ( !arg2.isEmpty() ) {
      QStringList list = d->expandWildcards( arg2 + "*" );
      if ( !list.isEmpty() ) {
        QString item;
        list.sort();
        ret.clear();
        for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
          item = *it;
          item.remove( 0, 2 );
          ret << QString( info().cmd + " " + arg1 + " " + item ).simplified();
        }
      }
    }

    if ( ret.isEmpty() )
      ret << QString( info().cmd + " " + arg1 + " " + arg2 ).simplified();

    return ret;
  }
};

class catExec : public AQSqlShellCmdExec
{

public:

  catExec( AQSqlShellPrivate * d_ ) : AQSqlShellCmdExec( d_ )
  {}

  AQShellCmdInfo info() const
  {
    AQShellCmdInfo i;
    i.cmd = "cat";
    i.numArgs = 1;
    i.optArgs = 0;
    i.reqArgs = 1;
    return i;
  }

  QStringList exec( const AQShellCmd & shellCmd ) const
  {
    QStringList argList( shellCmd.argList );
    QStringList ret;
    QString fileName;

    if ( !argList.isEmpty() )
      fileName = argList.first();

    QString shaKey, cacheKey;
    QString content( d->dir->fileData( fileName , shaKey, cacheKey ) );

    if ( !content.isEmpty() ) {
      ret << "<tr><td>" + fileName + ":</td></tr>";
      ret << "</table>" + content + "<br><table border=0>";
    }

    return ret;
  }

  QStringList expand( const QStringList & argList ) const
  {
    QString arg1;
    int nArgs = argList.size();
    if ( nArgs > 0 )
      arg1 = argList.at( 0 );

    QStringList list( d->expandWildcards( arg1 + "*" ) );

    if ( !list.isEmpty() ) {
      QStringList ret;
      QString item;
      list.sort();
      for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        item = *it;
        item.remove( 0, 2 );
        ret << QString( info().cmd + " " + item ).simplified();
      }
      return ret;
    } else
      return QStringList() << QString( info().cmd + " " + arg1 ).simplified();
  }
};

class cpToFsExec : public AQSqlShellCmdExec
{

public:

  cpToFsExec( AQSqlShellPrivate * d_ ) : AQSqlShellCmdExec( d_ )
  {}

  AQShellCmdInfo info() const
  {
    AQShellCmdInfo i;
    i.cmd = "cptofs";
    i.numArgs = 2;
    i.optArgs = 0;
    i.reqArgs = 2;
    return i;
  }

  QStringList exec( const AQShellCmd & shellCmd ) const
  {
    QStringList argList( shellCmd.argList );
    QStringList ret;
    QString arg1, arg2;

    int nArgs = argList.size();
    if ( nArgs > 0 )
      arg1 = argList.at( 0 );
    if ( nArgs > 1 )
      arg2 = argList.at( 1 );

    QStringList list( d->expandWildcards( arg1 + "*" ) );

    if ( !list.isEmpty() ) {
      QString item;
      QString msgStatus( "cptofs %1 --> " + arg2 );

      list.sort();

      for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        item = *it;
        item.remove( 0, 2 );

        d->consoleMsg( shellCmd, AQLogMsg, AQPlainMsg, msgStatus.arg( item ) );

        if ( !d->dir->cpFileToFs( item, arg2 ) ) {
          ret << d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                                AQ_TR( "SqlShell",
                                       "cptofs: %1: %2: No se puede copiar el origen al destino"
                                     ).arg( item ).arg( arg2 ) );
        }
      }
    } else if ( !d->dir->cpFileToFs( arg1, arg2 ) ) {
      ret = d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                           AQ_TR( "SqlShell",
                                  "cptofs: %1: %2: No se puede copiar el origen al destino"
                                ).arg( arg1 ).arg( arg2 ) );
    }

    return ret;
  }

  QStringList expand( const QStringList & argList ) const
  {
    QStringList ret;
    QString arg1, arg2;
    int nArgs = argList.size();
    if ( nArgs > 0 )
      arg1 = argList.at( 0 );
    if ( nArgs > 1 )
      arg2 = argList.at( 1 );

    if ( arg2.isEmpty() ) {
      QStringList list( d->expandWildcards( arg1 + "*" ) );
      if ( !list.isEmpty() ) {
        QString item;
        list.sort();
        for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
          item = *it;
          item.remove( 0, 2 );
          ret << QString( info().cmd + " " + item + " " + arg2 ).simplified();
        }
      }
    }

    if ( !arg2.isEmpty() ) {
      QStringList list = d->expandWildcards( arg2 + "*" );
      if ( !list.isEmpty() ) {
        QString item;
        list.sort();
        ret.clear();
        for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
          item = *it;
          item.remove( 0, 2 );
          ret << QString( info().cmd + " " + arg1 + " " + item ).simplified();
        }
      }
    }

    if ( ret.isEmpty() )
      ret << QString( info().cmd + " " + arg1 + " " + arg2 ).simplified();

    return ret;
  }
};

class qsaExec : public AQSqlShellCmdExec
{

public:

  qsaExec( AQSqlShellPrivate * d_, const AQScriptEngineQSA * e ) : AQSqlShellCmdExec( d_ ), eng( e )
  {}

  AQShellCmdInfo info() const
  {
    AQShellCmdInfo i;
    i.cmd = "qsa";
    i.numArgs = 3;
    i.optArgs = 0;
    i.reqArgs = 3;
    return i;
  }

  QStringList exec( const AQShellCmd & shellCmd ) const
  {
    QStringList argList( shellCmd.argList );
    QStringList ret( "\n" );
    QVariantList arguments;
    if ( argList.size() > 2 ) {
      QStringList list = argList.at( 2 ).split( "," );
      for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it )
        arguments << *it;
    }
    if ( argList.size() > 1 )
      ret << eng->call( argList.at( 1 ), arguments, argList.at( 0 ) ).toString();
    return d->formatHtmlTable( ret );
  }

private:

  const AQScriptEngineQSA * eng;
};

class qsacExec : public AQSqlShellCmdExec
{

public:

  qsacExec( AQSqlShellPrivate * d_, const AQScriptEngineQSA * e ) : AQSqlShellCmdExec( d_ ), eng( e )
  {}

  AQShellCmdInfo info() const
  {
    AQShellCmdInfo i;
    i.cmd = "qsac";
    i.numArgs = 1;
    i.optArgs = 0;
    i.reqArgs = 1;
    return i;
  }

  QStringList internalExec( const AQShellCmd & shellCmd, const QString & arg1 ) const
  {
    QStringList ret;

    if ( arg1.isEmpty() || arg1.length() <= 3 || !arg1.endsWith( ".qs" ) ) {
      ret = d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                           AQ_TR( "SqlShell", "Nombre de script no válido; %1" ).arg( arg1 ) );
    }

    if ( ret.isEmpty() ) {
      QString shaKey, cacheKey, fileSrcCode( arg1 ), fileByteCode( arg1 + ".qso" );
      QSLexer lex;
      AQCompilerQSA compiler( &lex );
      QByteArray byteCode( compiler.byteCode( d->dir->fileData( fileSrcCode, shaKey, cacheKey ) ) );

      if ( compiler.errorType() != QSErrNoError ) {

        for ( int i = 0; i < compiler.errorLines().size(); ++i ) {
          ret = d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                               AQ_TR( "SqlShell",
                                      "%1 : Linea %2 : %3" )
                               .arg( fileSrcCode )
                               .arg( compiler.errorLines().at( i ) )
                               .arg( compiler.errorMessages().at( i ) ) );
        }
      } else if ( byteCode.isEmpty() ) {
        ret = d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                             AQ_TR( "SqlShell",
                                    "Error general de compilación, el fichero fuente no existe ( %1 ) "
                                    "o no se ha podido guardar el fichero compilado ( %1 ) " )
                             .arg( fileSrcCode ).arg( fileByteCode ) );
      }

      if ( compiler.errorType() == QSErrNoError ) {
        d->dir->rm( fileByteCode );
        d->dir->mkfile( fileByteCode, byteCode );
      }
    }

    return ret;
  }

  QStringList exec( const AQShellCmd & shellCmd ) const
  {
    QStringList argList( shellCmd.argList );
    QStringList ret;
    QString arg1;
    int nArgs = argList.size();

    if ( nArgs > 0 )
      arg1 = argList.at( 0 );

    QStringList list( d->expandWildcards( arg1 + "*" ) );

    if ( !list.isEmpty() ) {
      QString item;
      QString msgStatus( "qsac %1 --> %1.qso" );

      list.sort();

      for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        item = *it;
        item.remove( 0, 2 );

        d->consoleMsg( shellCmd, AQLogMsg, AQPlainMsg, msgStatus.arg( item ) );

        ret << internalExec( shellCmd, item );
      }
    } else
      ret = internalExec( shellCmd, arg1 );

    return ret;
  }

  QStringList expand( const QStringList & argList ) const
  {
    QString arg1;
    int nArgs = argList.size();
    if ( nArgs > 0 )
      arg1 = argList.at( 0 );

    QStringList list( d->expandWildcards( arg1 + "*.qs" ) );

    if ( !list.isEmpty() ) {
      QStringList ret;
      QString item;
      list.sort();
      for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
        item = *it;
        item.remove( 0, 2 );
        ret << QString( info().cmd + " " + item ).simplified();
      }
      return ret;
    } else
      return QStringList() << QString( info().cmd + " " + arg1 ).simplified();
  }

private:

  const AQScriptEngineQSA * eng;
};

class fsckExec : public AQSqlShellCmdExec
{

public:

  fsckExec( AQSqlShellPrivate * d_, const AQManagerModules * m ) : AQSqlShellCmdExec( d_ ), mm( m )
  {}

  AQShellCmdInfo info() const
  {
    AQShellCmdInfo i;
    i.cmd = "fsck";
    i.numArgs = 0;
    i.optArgs = 0;
    i.reqArgs = 0;
    return i;
  }

  QStringList exec( const AQShellCmd & shellCmd ) const
  {
    QStringList ret;
    QStringList deleted;
    bool ok = d->dir->cleanOrphanData( deleted );

    if ( !ok )
      ret = d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                           AQ_TR( "SqlShell", "Error limpiando datos huérfanos" ) );

    for ( int i = 0; i < deleted.size(); ++i )
      ret = d->consoleMsg( shellCmd, AQLogMsg, AQPlainMsg,
                           AQ_TR( "SqlShell", "Limpiando datos huérfanos : %1" ).arg( deleted[ i ] ) );

    ok = d->dir->cleanVoidFiles( deleted );

    if ( !ok )
      ret = d->consoleMsg( shellCmd, AQWarningMsg, AQPlainMsg,
                           AQ_TR( "SqlShell", "Error limpiando ficheros vacíos" ) );

    for ( int i = 0; i < deleted.size(); ++i )
      ret = d->consoleMsg( shellCmd, AQLogMsg, AQPlainMsg,
                           AQ_TR( "SqlShell", "Limpiando ficheros vacíos : %1" ).arg( deleted[ i ] ) );

    mm->markPublicScripts();

    return ret;
  }

private:

  const AQManagerModules * mm;
};

class qryExec : public AQSqlShellCmdExec
{

public:

  qryExec( AQSqlShellPrivate * d_, QSqlDatabase db )
      : AQSqlShellCmdExec( d_ ), db_( db )
  {}

  AQShellCmdInfo info() const
  {
    AQShellCmdInfo i;
    i.cmd = "qry";
    i.numArgs = 1;
    i.optArgs = 0;
    i.reqArgs = 1;
    return i;
  }

  QStringList exec( const AQShellCmd & shellCmd ) const
  {
    QStringList argList( shellCmd.argList );
    QStringList ret;

    if ( !argList.isEmpty() ) {
      QSqlQuery q( argList.first(), db_ );

      if ( !q.lastError().isValid() ) {
        QSqlRecord rec = q.record();
        QString item;
        int recCount = rec.count() > 10 ? 10 : rec.count();
        int count = 0;
        bool hasMsgHandler = ( shellCmd.msgHandler && shellCmd.objCreator );

        for ( int i = 0; i < recCount; ++i )
          item += "<b>" + rec.fieldName( i ) + "</b> ||";

        ret << item;

        while ( q.next() ) {
          item.clear();

          for ( int i = 0; i < recCount; ++i )
            item += q.value( i ).toString() + "||";

          ret << item;
          ++count;

          if ( hasMsgHandler && count > 20 ) {
            count = 0;

            ret = d->formatHtmlTable( ret );
            ret.prepend( "<table style=\"border-style:dotted\" border=1>" );
            ret << "</table>";

            ( *shellCmd.msgHandler ) ( AQLogMsg, AQHtmlMsg, shellCmd.objCreator,
                                       ret.join( " " ).toLatin1() );

            ret.clear();
          }
        }

      } else
        ret << q.lastError().text();
    }

    if ( !ret.isEmpty() ) {
      ret = d->formatHtmlTable( ret );
      ret.prepend( "<table style=\"border-style:dotted\" border=1>" );
      ret << "</table>";
    }

    return ret;
  }

private:

  QSqlDatabase db_;
};

/**
 * Esqueleto para ejecutores
 * 
class Exec : public AQSqlShellCmdExec
{
 
public:
 
    Exec( AQSqlShellPrivate * d_ ) : AQSqlShellCmdExec( d_ )
    {}
 
    AQShellCmdInfo info() const
    {
        AQShellCmdInfo i;
        return i;
    }
 
    QStringList exec( const AQShellCmd & shellCmd ) const
    {
        QStringList ret;
        return ret;
    }
};
*/

///////////////////////////////////////////////////////////////////

AQSqlShell::AQSqlShell( AQSqlDatabase db, const QString & entryDir ) : AQShell()
{
  d = new AQSqlShellPrivate( new AQSqlDir( db, entryDir ) );

  d->registerExec( new pwdExec( d ) );
  d->registerExec( new lsExec( d ) );
  d->registerExec( new mkdirExec( d ) );
  d->registerExec( new mkfileExec( d ) );
  d->registerExec( new cdExec( d ) );
  d->registerExec( new rmExec( d ) );
  d->registerExec( new mvExec( d ) );
  d->registerExec( new cpExec( d ) );
  d->registerExec( new catExec( d ) );
  d->registerExec( new cpToFsExec( d ) );
  d->registerExec( new qsaExec( d, db.scriptEngine() ) );
  d->registerExec( new qsacExec( d, db.scriptEngine() ) );
  d->registerExec( new fsckExec( d, db.managerModules() ) );
  d->registerExec( new qryExec( d, db.dbAux() ) );
}

AQSqlShell::~AQSqlShell( )
{
  delete d;
}

QStringList AQSqlShell::cmd( const AQShellCmd & cmd ) const
{
  AQSqlShellCmdExec * cmdExec = d->execCmds.value( cmd.cmd );
  if ( cmdExec )
    return cmdExec->exec( cmd );
  else
    return QStringList();
}

QStringList AQSqlShell::expand( const AQShellCmd & cmd ) const
{
  AQSqlShellCmdExec * cmdExec = d->execCmds.value( cmd.cmd );
  if ( cmdExec )
    return cmdExec->expand( cmd.argList );
  else {
    QStringList ret;
    for ( AQSqlShellCmdExecHash::const_iterator it = d->execCmds.begin(); it != d->execCmds.end(); ++it ) {
      if ( ( *it ) && ( it.key().startsWith( cmd.cmd ) || cmd.cmd.isEmpty() ) )
        ret << ( *it ) ->info().cmd;
    }
    return ret;
  }
}

AQShellCmdInfoHash AQSqlShell::suppCmds() const
{
  return d->suppCmds;
}
