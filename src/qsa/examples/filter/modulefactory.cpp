#include <qpushbutton.h>

#include "modulefactory.h"
#include "modules.h"

ModuleFactory::ModuleFactory()
{
    registerClass( "ImageSource", "ImgSource" );
    registerClass( "Threshold" );
    registerClass( "BumpMapper" );
    registerClass( "Smudger" );
}

QObject *ModuleFactory::create( const QString &type,
				const QSArgumentList &,
				QObject * )
{
    if ( type == "ImageSource" )
	return new ImgSource( interpreter() );
    else if ( type == "Threshold" )
	return new Threshold();
    else if( type == "BumpMapper" )
	return new BumpMapper();
    else if( type == "Smudger" )
	return new Smudger;
    return 0;
}
