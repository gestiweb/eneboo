/***************************************************************************
AQConsole.cpp
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

#include <qsworkbench.h>

#include <AQCore/AQShellCmd.h>
#include <AQCore/AQShellCmdInfo.h>
#include <AQSql/AQSqlShell.h>

#include "AQConsole.h"
#include "../AQApplication.h"
#include "../AQUi.h"

void aqConsoleMsgHandler( AQMsgType type, AQMsgFormat format,
                          QObject * console, const char * msg )
{
  AQConsole * console_ = qobject_cast<AQConsole *>( console );

  if ( console_ )
    console_->showMsg( type, format, msg );
}

AQConsole::AQConsole( QWidget * parent, AQSqlDatabase db ) : QWidget( parent ), db_( db )
{
  sqlSh = new AQSqlShell( db_ );
  ui.setupUi( this );
  ui.tedInput->installEventFilter( this );
  ui.tedInput->viewport() ->installEventFilter( this );
  ui.tedOutput->textCursor().setVisualNavigation( true );
  ui.tedOutput->textCursor().insertText( prompt() );
  setFocusProxy( ui.tedInput );
}

AQConsole::~AQConsole()
{
  delete sqlSh;
}

void AQConsole::showMsg( AQMsgType type, AQMsgFormat format, const char * msg )
{
  switch ( type ) {
  case AQLogMsg:
    break;
  case AQDebugMsg:
    aqDebug( msg );
    break;
  case AQWarningMsg:
    aqWarn( msg );
    break;
  case AQCriticalMsg:
    aqCrit( msg );
    break;
  case AQFatalMsg:
    fprintf( stderr, "Fatal: %s\n", msg );
    abort();
    break;
  }

  switch ( format ) {
  case AQPlainMsg:
    ui.tedOutput->textCursor().insertText( QString( "\n" + QLatin1String( msg ) ) );
    break;
  case AQHtmlMsg:
    ui.tedOutput->textCursor().insertHtml( QString( "<br>" + QLatin1String( msg ) ) );
    break;
  }

  ui.tedOutput->moveCursor( QTextCursor::End );
  AbanQ->processEvents();
}

void AQConsole::closeEvent( QCloseEvent * e )
{
  e->ignore();
  Q_EMIT closeRequest( this );
}

bool AQConsole::eventFilter( QObject * o, QEvent * e )
{
  if ( o != ui.tedInput && o != ui.tedInput->viewport() )
    return false;

  if ( e->type() == QEvent::KeyPress ) {
    QKeyEvent * ke = static_cast<QKeyEvent *>( e ) ;
    switch ( ke->key() ) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
      on_pbExec_clicked();
      return true;
    case Qt::Key_Up:
      on_pbHistoUp_clicked();
      return true;
    case Qt::Key_Down:
      on_pbHistoDown_clicked();
      return true;
    case Qt::Key_Tab:
      on_pbAutoCompletion_clicked();
      return true;
    case Qt::Key_Escape:
      ke->ignore();
      return true;
    }
  }
  return false;
}

void AQConsole::on_pbExec_clicked()
{
  QString cmd = ui.tedInput->toPlainText();

  if ( !cmd.isEmpty() ) {
    cmd = cmd.simplified();
    itHisto = histo.insert( histo.end(), cmd );
    ++itHisto;

    if ( cmd == "exit" ) {
      Q_EMIT closeRequest( this );
      return ;
    }

    if ( cmd == "clear" ) {
      ui.tedInput->clear();
      ui.tedOutput->clear();
      ui.tedOutput->textCursor().insertText( prompt() );
      ui.tedOutput->moveCursor( QTextCursor::End );
      return ;
    }

    if ( cmd == "wb" ) {
      db_.scriptEngine() ->workbench() ->open();
      return ;
    }

    QStringList argList( buildArgs( cmd ) );
    QStringList res;
    AQShellCmd shCmd;

    shCmd.objCreator = this;
    shCmd.msgHandler = aqConsoleMsgHandler;

    cmd = argList.takeFirst();
    shCmd.cmd = cmd;
    shCmd.argList = argList;
    res = sqlSh->cmd( shCmd );

    if ( !res.isEmpty() ) {
      QString output_( "<table border=0>" );
      for ( QStringList::const_iterator it = res.begin(); it != res.end(); ++it )
        output_ += ( *it );
      output_ += "</table>";
      output_ += "<br>" + prompt();
      ui.tedOutput->textCursor().insertHtml( output_ );
    } else
      ui.tedOutput->textCursor().insertText( QString( "\n" + prompt() ) );

    ui.tedOutput->moveCursor( QTextCursor::End );
    ui.tedInput->clear();
  }
}

void AQConsole::on_pbHistoUp_clicked()
{
  if ( !histo.isEmpty() ) {
    if ( histo.size() > 1 ) {
      if ( itHisto == histo.begin() ) {
        itHisto = histo.end();
        --itHisto;
      } else
        --itHisto;
    } else
      itHisto = histo.begin();
    ui.tedInput->setPlainText( *itHisto );
    ui.tedInput->moveCursor( QTextCursor::End );
  }
}

void AQConsole::on_pbHistoDown_clicked()
{
  if ( !histo.isEmpty() ) {
    if ( histo.size() > 1 ) {
      ++itHisto;
      if ( itHisto == histo.end() )
        itHisto = histo.begin();
    } else
      itHisto = histo.begin();
    ui.tedInput->setPlainText( *itHisto );
    ui.tedInput->moveCursor( QTextCursor::End );
  }
}

void AQConsole::on_pbAutoCompletion_clicked()
{
  QString cmd = ui.tedInput->toPlainText().simplified();
  QString input = ui.tedInput->toPlainText().simplified();
  QStringList res;
  QString exitStr( "exit " );
  QString clearStr( "clear " );
  QStringList argList;

  if ( !cmd.isEmpty() ) {
    if ( exitStr.startsWith( cmd ) )
      res << exitStr;
    if ( clearStr.startsWith( cmd ) )
      res << clearStr;
    argList = buildArgs( cmd );
    cmd = argList.takeFirst();
  } else {
    res << exitStr;
    res << clearStr;
  }

  AQShellCmd shCmd;

  shCmd.cmd = cmd;
  shCmd.argList = argList;
  res << sqlSh->expand( shCmd );

  if ( !res.isEmpty() ) {
    if ( res.size() == 1 ) {
      QString item = res.first();
      if ( !item.endsWith( "/" ) )
        item += " ";
      ui.tedInput->setPlainText( item );
      ui.tedInput->moveCursor( QTextCursor::End );
    } else {
      QString first = res.at( 0 ), rest;
      int i = 0, siz = first.size();
      bool more = true;
      while ( i < siz && more ) {
        QChar c = first.at( i );
        for ( QStringList::const_iterator it = res.begin(); it != res.end(); ++it ) {
          if ( c != ( *it ).at( i ) ) {
            more = false;
            break;
          }
        }
        if ( more ) {
          rest += c;
          ++i;
        }
      }
      if ( i )
        ui.tedInput->setPlainText( rest );

      QString output_( " " + ui.tedInput->toPlainText() + "<br>" );
      QString item;

      for ( QStringList::const_iterator it = res.begin(); it != res.end(); ++it ) {
        item = *it;
        output_ += item.simplified() + "<br>";
      }

      output_ += prompt();
      ui.tedInput->setPlainText( ui.tedInput->toPlainText() );
      ui.tedInput->moveCursor( QTextCursor::End );

      ui.tedOutput->textCursor().insertHtml( output_ );
      ui.tedOutput->moveCursor( QTextCursor::End );
    }
  }
}

QString AQConsole::prompt()
{
  QString ret;
  AQShellCmd cmdPwd;

  cmdPwd.cmd = "pwd";
  ret = db_.user() + QLatin1String( "@" ) + db_.host();
  ret += QLatin1String( ":" ) + sqlSh->cmd( cmdPwd ).first() + "$";

  return ret;
}

QStringList AQConsole::buildArgs( const QString & cmdStr ) const
{
  QStringList ret;
  if ( cmdStr.contains( "\"" ) ) {
    QStringList list( cmdStr.split( "\"" ) );
    int i = 1;
    for ( QStringList::const_iterator it = list.begin(); it != list.end(); ++it ) {
      if ( !( *it ).isEmpty() ) {
        if ( i % 2 )
          ret << ( *it ).split( " ", QString::SkipEmptyParts );
        else
          ret << ( *it );
      }
      ++i;
    }
  } else
    ret = cmdStr.split( " ", QString::SkipEmptyParts );
  return ret;
}
