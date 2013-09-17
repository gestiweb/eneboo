/***************************************************************************
AQUiUtil.h
-------------------
begin                : 09/08/2007
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

#ifndef AQUIUTIL_H_
#define AQUIUTIL_H_

#include <AQUi/AQUi.h>

#include <QString>

class AQUiUtilPrivate;
class QImage;

/**
Clase con métodos, herramientas y utiles necesarios para ciertas operaciones.
 
Es esta clase se encontrarán métodos genéricos que
realizan operaciones muy específicas pero que
son necesarios para ciertos procesos habituales
en las distintas tareas a desempeñar en la gestión
empresarial.
 
@author InfoSiAL S.L.
*/
class AQUiUtil
{

public:

  /**
  constructor
  */
  AQUiUtil();

  /**
  destructor
  */
  ~AQUiUtil();

  /**
  Crea un diálogo de progreso

  @param text Texto del diálogo
  @param tS Número total de pasos a realizar
  @param title Titulo del diálogo
  @param cancelButtonText Texto para botón cancelar. Si es vacio no muestra el botón.
  */
  void createProgressDialog( const QString & text, int tS,
                             const QString & title = "AbanQ",
                             const QString & cancelButtonText = QString() );

  /**
  Destruye el diálogo de progreso
  */
  void destroyProgressDialog() const;

  /**
  Establece el grado de progreso del diálogo

  @param p Grado de progreso
  */
  void setProgress( int p ) const;

  /**
  Cambia el texto del diálogo

  @param text Texto del diálogo
  */
  void setLabelText( const QString & text ) const;

  /**
  Establece el número total de pasos del diálogo

  @param tS Número total de pasos
  */
  void setTotalSteps( int tS ) const;

  /**
  Obtiene la imagen o captura de pantalla de un formulario.

  @param n Nombre del fichero que contiene la descricpción del formulario.
  */
  QImage snapShotUI( const QString & n ) const;

  /**
  Salva en un fichero con formato PNG la imagen o captura de pantalla de un formulario.

  @param n Nombre del fichero que contiene la descricpción del formulario.
  @param pathFile Ruta y nombre del fichero donde guardar la imagen
  */
  void saveSnapShotUI( const QString & n, const QString & pathFile ) const;

  /**
  Guarda la imagen de icono de un botón de un formulario en un ficher png. 
  Utilizado para documentación

  @param data Contenido de la imagen en una cadena de caracteres
  @param pathFile Ruta completa al fichero donde se guadará la imagen
  */
  void saveIconFile( const QString & data, const QString & pathFile ) const;

  /**
  Cajas de mensajes típicas. Wrapper de QMessageBox.
  */
  AQUi::StandardButton msgBoxInfo( const QString & title, const QString & text,
                                   AQUi::StandardButtons buttons = AQ_Ok,
                                   AQUi::StandardButton defaultButton = AQ_NoB ) const;

  AQUi::StandardButton msgBoxQuestion( const QString & title, const QString & text,
                                       AQUi::StandardButtons buttons = AQ_Yes,
                                       AQUi::StandardButton defaultButton = AQ_No ) const;

  AQUi::StandardButton msgBoxWarn( const QString & title, const QString & text,
                                   AQUi::StandardButtons buttons = AQ_Ok,
                                   AQUi::StandardButton defaultButton = AQ_NoB ) const;

  AQUi::StandardButton msgBoxCritical( const QString & title, const QString & text,
                                       AQUi::StandardButtons buttons = AQ_Ok,
                                       AQUi::StandardButton defaultButton = AQ_NoB ) const;

private:

  /**
  Privado
  */
  AQUiUtilPrivate * d;
};

#endif /*AQUIUTIL_H_*/
