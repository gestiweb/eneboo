/***************************************************************************
                            AQ3MainWindowPlugin.h
                        -------------------
begin                : 11/09/2007
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

#ifndef AQ3MAINWINDOWPLUGIN_H_
#define AQ3MAINWINDOWPLUGIN_H_

#include <QtDesigner/QDesignerCustomWidgetInterface>

class AQ3MainWindowPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES( QDesignerCustomWidgetInterface )

public:

    AQ3MainWindowPlugin( QObject * parent = 0 );
    virtual ~AQ3MainWindowPlugin();

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
};

#endif /*AQ3MAINWINDOWPLUGIN_H_*/
