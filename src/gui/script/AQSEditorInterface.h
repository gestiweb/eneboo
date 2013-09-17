/***************************************************************************
AQSEditorInterface.h
-------------------
begin                : 01/03/2009
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

#ifndef AQSEDITORINTERFACE_H_
#define AQSEDITORINTERFACE_H_

#include <AQScript/AQComponentDBInterface.h>
#include <qseditor.h>

typedef QSEditor AQSEditor;


#ifdef AQ_OBJ_FACTORY_IMPL
static void deleteSharedAQSEditor( AQSEditor * obj )
{
  if ( !obj->parent() )
    obj->deleteLater();
}
#endif

class AQSEditorInterfaceAQ2 : public AQComponentDBInterface
{
  Q_OBJECT

public:

  QSharedPointer<AQSEditor> obj_;

public Q_SLOTS:

  void setCode( const QString & )
  {
    AQ_NO_PORT_AQ2( AQSEditor::setCode( const QString & ) );
  }
  QString code() const
  {
    AQ_NO_PORT_AQ2( AQSEditor::code() const );
    return QString();
  }
  int exec()
  {
    AQ_NO_PORT_AQ2( AQSEditor::exec() );
    return 0;
  }
  QSEditor * obj()
  {
    AQ_NO_PORT_AQ2( AQSEditor::obj() );
    return 0;
  }

protected:

  friend class AQUiObjectFactoryQSA;

  /**
  Constructor
  */
  AQSEditorInterfaceAQ2( const AQScriptEngineQSA * eng,
                         const QString & ) :
      AQComponentDBInterface( eng )
  {
    AQ_NO_PORT_AQ2( AQSEditor::AQSEditor( const QString & ) );
  }

  /**
  Constructor
  */
  AQSEditorInterfaceAQ2( const AQScriptEngineQSA * eng,
                         QSharedPointer<AQSEditor> aqSEditor );

  /**
  Destructor
  */
  virtual ~AQSEditorInterfaceAQ2();
};

#endif /*AQSEDITORINTERFACE_H_*/
