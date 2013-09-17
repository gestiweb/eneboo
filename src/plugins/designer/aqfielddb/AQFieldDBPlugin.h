/***************************************************************************
                      AQFieldDBPlugin.h
                     -------------------
begin                : 30/08/2007
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

#ifndef AQFIELDDBPLUGIN_H_
#define AQFIELDDBPLUGIN_H_

#include <QtDesigner/QDesignerCustomWidgetInterface>

class AQFieldDBPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES( QDesignerCustomWidgetInterface )

public:

    AQFieldDBPlugin( QObject * parent = 0, bool initAsFLFieldDB = false );
    virtual ~AQFieldDBPlugin();

    virtual QString name() const;
    virtual QString group() const;
    virtual QString toolTip() const;
    virtual QString whatsThis() const;
    virtual QString includeFile() const;
    virtual QIcon icon() const;

    virtual bool isContainer() const;

    virtual QWidget * createWidget( QWidget * parent );

    virtual bool isInitialized() const;
    virtual void initialize( QDesignerFormEditorInterface * core );

private:

    bool initialized;

    /**
    Para inicializar el plugin como el antiguo FLFieldDB.

    Este atributo se utiliza para soporte y migración de los antiguos
    formularios de Qt3 a Qt4
    */
    bool initAsFLFieldDB_;
};

/**
Soporte para el antiguo plugin FLFieldDB
 
Para soporte y migración de los antiguos formularios
de Qt3 a Qt4
*/
class FLFieldDBPlugin : public AQFieldDBPlugin
{
    Q_OBJECT
    Q_INTERFACES( QDesignerCustomWidgetInterface )

public:

    FLFieldDBPlugin( QObject * parent = 0 );
    virtual ~FLFieldDBPlugin();
};

#endif /*AQFIELDDBPLUGIN_H_*/
