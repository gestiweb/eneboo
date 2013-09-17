/***************************************************************************
AQComponentDB.h
-------------------
begin                : 31/08/2007
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

#ifndef AQCOMPONENTDB_H_
#define AQCOMPONENTDB_H_

#include <QFrame>

#include <AQUi/AQUi.h>

class AQFormDB;

/**
Clase base abstracta para componentes de formularios

@author InfoSiAL S.L.
*/
class AQComponentDB : public QFrame
{
  Q_OBJECT

protected:

  /**
  constructor
  */
  AQComponentDB( QWidget * parent = 0 );

public:

  /**
  destructor
  */
  virtual ~AQComponentDB();

  /**
  Construir la interfaz de usuario, se debe llamar antes que linkUi e initRefreshUi

  @param form El formulario al que pertenece el componente
  */
  virtual void setupUi( AQFormDB * form );

  /**
  Establece la accion que debe utilizar el componente.

  Este método se debe llamar siempre antes que setupUi y linkUi, ya que a
  través de la accion los componente pueden conocer los metadatos que necesitan
  para autoconfigurarse.

  @param act Accion del tipo AQAction a utilizar. Si es vacia desvincula.
  */
  virtual void setAQAction( AQSharedAction act ) = 0;

  /**
  @return Objeto AQAction usada por el componente.
  */
  virtual AQSharedAction aqAction() const = 0;

  /**
  Vincular Ui al cursor de la accion, conecta las señales oportunas del cursor para actualizar
  los datos cuando se actualiza el registro de cursor, se debe llamar antes que initRefreshUi.

  Para desvincular setAQAction( AQSharedAction() )
  */
  virtual void linkUi() = 0;

  /**
  Si se necesita aquí se puede hacer un refresco inicial,
  este método se llama el último para tener todo lo necesario disponible
  */
  virtual void initRefreshUi() = 0;

  /**
  Para gestionar eventos internos predefinidos.

  Es utilizado para recibir eventos de la capa web cuando la
  aplicación se ha construido en modo WUI ( Web User Interface ).
  */
  virtual bool eventFilter( QObject * obj, QEvent * ev ) = 0;

protected:

  /**
  El formulario al que pertenece el componente
  */
  AQFormDB * form_;
};

#endif /*AQCOMPONENTDB_H_*/
