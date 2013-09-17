/***************************************************************************
AQConsole.h
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

#ifndef AQCONSOLE_H_
#define AQCONSOLE_H_

#include <AQCore/AQCore.h>
#include <AQSql/AQSqlDatabase.h>

#include <uic/ui_AQConsole.h>

class AQSqlShell;

class AQConsole : public QWidget
{
  Q_OBJECT

public:

  /**
  constructor
  */
  AQConsole( QWidget * parent, AQSqlDatabase db );

  /**
  destructor
  */
  ~AQConsole();

public Q_SLOTS:

  /**
  Muestra el mesaje por la salida
  */
  void showMsg( AQMsgType, AQMsgFormat, const char * );

protected:

  void closeEvent( QCloseEvent * e );
  bool eventFilter( QObject * o, QEvent * e );

Q_SIGNALS:

  void closeRequest( AQConsole * con );

private Q_SLOTS:

  /**
  Autoconexiones de acciones de la Ui
  */
  void on_pbExec_clicked();
  void on_pbHistoUp_clicked();
  void on_pbHistoDown_clicked();
  void on_pbAutoCompletion_clicked();

private:

  /**
  @return El prompt de la consola
  */
  QString prompt();

  /**
  Dada una entrada de comando construye la lista de argumentos.

  Considera que los argumentos están separados por espacios.
  Si hay texto encerrado entre dobles comilla ( " ) lo considera
  como un sólo argumento aunque contenga espacios en su interior,
  de esta manera se pueden pasar agumentos únicos que contienen
  espacios, por ejemplo el nombre de un directorio que contiene
  espacios.

  @param  cmdStr  Cadena simple con la entrada de comando
  @return Lista de cadenas de texto donde cada elemento es un argumento
  */
  QStringList buildArgs( const QString & cmdStr ) const;

  /**
  Histórico
  */
  QStringList histo;
  QStringList::const_iterator itHisto;

  /**
  Shells
  */
  AQSqlShell * sqlSh;

  /**
  Base de datos
  */
  AQSqlDatabase db_;

  /**
  Interfaz de Usuario
  */
  Ui::AQConsole ui;
};

#endif /*AQCONSOLE_H_*/
