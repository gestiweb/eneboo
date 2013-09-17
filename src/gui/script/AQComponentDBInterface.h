/***************************************************************************
AQComponentDBInterface.h
-------------------
begin                : 25/02/2009
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

#ifndef AQCOMPONENTDBINTERFACE_H_
#define AQCOMPONENTDBINTERFACE_H_

#include <AQScript/AQObjectInterfaceQSA.h>

class QWidget;

#ifdef AQ_WUI
class WQWidget;
#endif

/**
Interfaz común a todos los componentes gráficos
*/
class AQComponentDBInterface : public AQObjectInterfaceQSA
{
  Q_OBJECT

  Q_PROPERTY( bool enabled READ isEnabled WRITE setEnabled )
  Q_PROPERTY( bool undefined READ isUndefined )

public Q_SLOTS:

  /**
  @return El componente hijo que tiene el nombre indicado. Notar que sólo
          busca componentes que heredan de AQComponentDB, cualquier otro 'widget'
          que no se halla definido como subclase de AQComponentDB no se tendrá
          en cuenta
  */
  AQComponentDBInterface * child( const QString & name );

  void show();
  void close();
  void hide();
  void setDisabled( bool disable );
  void setFocus();

  void setWindowState( Qt::WindowStates windowState );
  Qt::WindowStates windowState() const;

protected Q_SLOTS:

  /**
  @return Si la interfaz es indefinida, es decir, no tiene objeto subyacente
  */
  bool isUndefined() const;

  bool isEnabled () const;
  void setEnabled ( bool enabled );

protected:

  AQComponentDBInterface( const AQScriptEngineQSA * eng, QWidget * wid = 0 );

#ifdef AQ_WUI
  AQComponentDBInterface( const AQScriptEngineQSA * eng, WQWidget * wid );
#endif

  QWidget * wid_;

private:

  Q_DISABLE_COPY( AQComponentDBInterface )
};

inline bool AQComponentDBInterface::isUndefined() const
{
  return !wid_;
}

#endif /*AQCOMPONENTDBINTERFACE_H_*/
