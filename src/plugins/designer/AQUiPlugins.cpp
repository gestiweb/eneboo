/***************************************************************************
                        AQUiPlugins.cpp
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

#include "aqfielddb/AQFieldDBPlugin.h"
#include "aqtabledb/AQTableDBPlugin.h"
#include "aq3mainwindow/AQ3MainWindowPlugin.h"

#include <QtDesigner/QDesignerCustomWidgetCollectionInterface>
#include <QtCore/qplugin.h>
#include <QtCore/qdebug.h>

class AQUiPlugins : public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES( QDesignerCustomWidgetCollectionInterface )

public:

    AQUiPlugins( QObject * parent = 0 );

    virtual QList<QDesignerCustomWidgetInterface *> customWidgets() const;

private:

    QList<QDesignerCustomWidgetInterface *> plugins;
};

AQUiPlugins::AQUiPlugins( QObject * parent )
        : QObject( parent )
{
    plugins.append( new AQFieldDBPlugin( this ) );
    plugins.append( new AQTableDBPlugin( this ) );
    plugins.append( new FLFieldDBPlugin( this ) );
    plugins.append( new FLTableDBPlugin( this ) );
    plugins.append( new AQ3MainWindowPlugin( this ) );
}

QList<QDesignerCustomWidgetInterface *> AQUiPlugins::customWidgets() const
{
    return plugins;
}

Q_EXPORT_PLUGIN2( AQUiPlugins, AQUiPlugins )

#include "AQUiPlugins.moc"
