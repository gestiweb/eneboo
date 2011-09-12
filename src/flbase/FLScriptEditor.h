/***************************************************************************
                            FLScriptEditor.h
                          -------------------
 begin                : mie mar 3 2004
 copyright            : (C) 2004-2005 by InfoSiAL S.L.
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

#ifndef FLSCRIPTEDITOR_H
#define FLSCRIPTEDITOR_H

#include <qdialog.h>
#include <qseditor.h>
#include <qtextedit.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>

#include "FLWidgetScriptEditor.h"

#ifdef QSDEBUGGER
class QSReplaceText;
class QSPreferencesContainer;
class PreferencesBase;
#endif

/**
Editor para scripts.

@author InfoSiAL S.L.
*/
class FL_EXPORT FLScriptEditor : public FLWidgetScriptEditor
{

  Q_OBJECT

public:

  /**
  constructor

  @param nFS Nombre del fichero del script (*.qs). Si no corresponde con ninguno de los
  ficheros de script cargados, se utilizará su valor como contenido del editor, esto es
  útil para poder usar esta clase como editor general pasando en este parámetro el texto
  a editar.
  */
  FLScriptEditor(const QString &nFS, QWidget *parent = 0, const char *name = 0);

  /**
  destructor
  */
  ~FLScriptEditor();

  /**
  Establece el código a editar
  */
  void setCode(const QString &c);

  /**
  Obtiene el código en edición
  */
  QString code() const;

protected slots:

  /**
  Busca texto

  @param expr Texto a buscar
  */
  bool find(const QString &expr);

  /**
  Repite última búsqueda de texto
  */
  bool reFind();

  /**
  Mueve el cursor directamente a una línea

  @param line Número de la linea a la que saltar
  */
  void gotoLine(const QString &line);

  /**
  Muestra diálogo de reemplazar texto
  */
  void showReplace();

  /**
  Reemplazar primera ocurrencia de texto encontrada
  */
  void editReplace();

  /**
  Reemplaza todas las ocurrencias de texto encontradas
  */
  void editReplaceAll();

  /**
  Muestra diálogo para editar las preferencias
  */
  void editPreferences();

  /**
  Aplicar y guarda las preferencias editadas
  */
  void savePreferences();

protected:

  /**
  Para eventos de pulsación de teclas
  */
  void keyPressEvent(QKeyEvent *e);

private:

  /**
  Reemplaza ocurrencias de textos encontradas

  @param all Si es TRUE reemplaza todo lo encontrado, si es FALSE sólo la
     primera ocurrencia
  */
  void editReplace(bool all);

  /**
  QSA Editor
  */
  QSEditor *ed;

  /**
  Última búsqueda realizada en el texto
  */
  QString lastSearch;

#ifdef QSDEBUGGER
  /**
  Cuadro de diálogo de reemplazar texto
  */
  QSReplaceText *replaceText;

  /**
  Editor de preferencias
  */
  PreferencesBase *qsaEditorSyntax;

  /**
  Diálogo contenedor para el editor de preferencias
  */
  QSPreferencesContainer *preferencesContainer;
#endif
};

#endif
