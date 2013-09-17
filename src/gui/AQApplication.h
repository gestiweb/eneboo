/***************************************************************************
AQApplication.h
-------------------
begin                : 06/08/2007
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

#ifndef AQAPPLICATION_H_
#define AQAPPLICATION_H_

#include <QApplication>
#include <QString>
#include <QMap>

class QActionGroup;
class AQMainWindow;
class AQTabView;

/**
 Aplicación gráfica de escritorio de AbanQ.

 Mantiene y gestiona a través de una vista de pestañas,
 un conjunto de ventanas principales (AQMainWindow) para
 distintas conexiones a bases de datos.

 @author InfoSiAL S.L.
 */
class AQApplication: public QApplication
{
  Q_OBJECT

public:

  /**
   Constructor invocado desde el programa principal
   */
  AQApplication( int &argc, char **argv );

  /**
   Destructor
   */
  ~AQApplication();

  /**
   @return Grupo de acciones con estilos disponibles
   */
  QActionGroup * agStyles( QObject * parent ) const;

  /**
   @return Grupo de acciones con hojas de estilo disponibles
   */
  QActionGroup * agStyleSheets( QObject * parent ) const;

  /**
   @return Ventana principal actualmente activa
   */
  AQMainWindow * activeMainWindow() const;

  /**
   @return Ventana principal asociada a la conexión de base de datos indicada
   */
  AQMainWindow * mainWindow( const QString & connectionName ) const;

  /**
   @return Vista de pestañas que contienen las ventanas principales
   */
  AQTabView * tabView() const;

public Q_SLOTS:

  /**
   Establece el estilo global para toda la aplicación
   */
  void setAQStyle( const QString & styleName = QString() );

  /**
   Establece la hoja de estilo global para toda la aplicación
   */
  void setAQStyleSheet( const QString & styleSheet = QString() );

  /**
   Muestra diálogo 'Acerca de..'
   */
  void aboutAbanQ();

  /**
   Muestra diálogo para solicitar al usuario salir de la aplicación,
   si acepta cierra la aplicación
   */
  bool tryExit();

  /**
   Crea una nueva ventana principal abriendo una conexion a una base de datos
   para ella.

   Muestra al usuario el diálogo de conexión a base de datos, si la conexión
   tiene éxito abre una ventana principal para esa base de datos.

   La ventana principal creada pasa a ser la activa
   */
  void newMainWindow();

  /**
   Cierra la ventana principal indicada como parámetro.

   Al cerrar una ventana principal también se cierra su conexión a la base de datos
   */
  void closeMainWindow( AQMainWindow * win );

  /**
   Cierra la ventana principal asociada a la conexión de base de datos indicada.

   Equivalente a closeMainWindow( mainWindow( connectionName ) )
   */
  void closeMainWindow( const QString & connectionName );

  /**
   Cierra la ventana principal actualmente activa.

   Equivalente a closeMainWindow( activeMainWindow() )
   */
  void closeActiveMainWindow();

  /**
   Abre un menu contextual en la posición indicada, con distintas opciones para manejar
   la vista de pestañas
   */
  void openTabMenu( const QPoint & pos );

  /**
   Cierra todas las pestañas y las ventanas principales que contienen, excepto la pestaña
   que contiene la ventana pricipal actualmente activa
   */
  void closeOtherTabs();

private:

  /**
   Uso interno
   */
  void init();
  bool finish();
  bool openConnectionDB( const QString & connectionName = "default" );
  void loadTranslation( const QString & prefixFileTs,
                        const QString & connectionName = "default" );

  AQTabView * tabView_;
  int connCount_;
  QString curStyle_;
  QString curStyleSheet_;
  QMap<QString, AQMainWindow *> mapMainWindow_;
};

#endif /*AQAPPLICATION_H_*/
