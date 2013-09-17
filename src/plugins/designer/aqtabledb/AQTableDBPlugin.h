/***************************************************************************
                            AQTableDBPlugin.h
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

#ifndef AQTABLEDBPLUGIN_H_
#define AQTABLEDBPLUGIN_H_

#include <QtDesigner/QDesignerCustomWidgetInterface>

class AQTableDBPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES( QDesignerCustomWidgetInterface )

public:

    AQTableDBPlugin( QObject * parent = 0, bool initAsFLTableDB = false );
    virtual ~AQTableDBPlugin();

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
    Para inicializar el plugin como el antiguo FLTableDB.

    Este atributo se utiliza para soporte y migración de los antiguos
    formularios de Qt3 a Qt4
    */
    bool initAsFLTableDB_;
};

/**
Soporte para el antiguo plugin FLTableDB
 
Para soporte y migración de los antiguos formularios
de Qt3 a Qt4
*/
class FLTableDBPlugin : public AQTableDBPlugin
{
    Q_OBJECT
    Q_INTERFACES( QDesignerCustomWidgetInterface )

public:

    FLTableDBPlugin( QObject * parent = 0 );
    virtual ~FLTableDBPlugin();
};

#endif /*AQTABLEDBPLUGIN_H_*/
