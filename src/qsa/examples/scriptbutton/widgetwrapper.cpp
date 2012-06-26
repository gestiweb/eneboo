#include "widgetwrapper.h"
#include <qapplication.h>

WidgetWrapper::WidgetWrapper( QWidget *w )
    : widget( w )
{
}

void WidgetWrapper::sendMousePressEvent( const QPoint &pos, int button, int state )
{
    QMouseEvent me( QEvent::MouseButtonPress, pos, button, state );
    QApplication::sendEvent( widget, &me );
}

void WidgetWrapper::sendMouseReleaseEvent( const QPoint &pos, int button, int state )
{
    QMouseEvent me( QEvent::MouseButtonRelease, pos, button, state );
    QApplication::sendEvent( widget, &me );
}


WidgetWrapperFactory::WidgetWrapperFactory()
{
    registerWrapper( "QWidget" );
}

QObject *WidgetWrapperFactory::create( const QString &className, void *ptr )
{
    if ( className == "QWidget" )
	return new WidgetWrapper( (QWidget*)ptr );
    return 0;
}

QtNamespaceProvider::QtNamespaceProvider()
{
    registerClass( "Qt", QString::null, new QtNamespace() );
}
