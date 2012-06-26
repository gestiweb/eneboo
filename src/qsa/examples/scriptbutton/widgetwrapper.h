#ifndef WIDGETWRAPPER_H
#define WIDGETWRAPPER_H

#include <qwidget.h>
#include <qswrapperfactory.h>
#include <qsobjectfactory.h>

class WidgetWrapper : public QObject
{
    Q_OBJECT

public:
    WidgetWrapper( QWidget *w );

public slots:
    void sendMousePressEvent( const QPoint &pos, int button, int state );
    void sendMouseReleaseEvent( const QPoint &pos, int button, int state );

private:
    QWidget *widget;

};


class WidgetWrapperFactory : public QSWrapperFactory
{
public:
    WidgetWrapperFactory();
    QObject *create( const QString &className, void *ptr );
};

class QtNamespace : public QObject
{
    Q_OBJECT
    Q_OVERRIDE( QCString name SCRIPTABLE false )
    Q_PROPERTY( int NoButton READ noButton )
    Q_PROPERTY( int LeftButton READ leftButton )
    Q_PROPERTY( int RightButton READ rightButton )
    Q_PROPERTY( int MidButton READ midButton )
    Q_PROPERTY( int ShiftButton READ shiftButton )
    Q_PROPERTY( int ControlButton READ controlButton )
    Q_PROPERTY( int AltButton READ altButton )
    Q_PROPERTY( int MetaButton READ metaButton )
public:
    int noButton() const { return Qt::NoButton; }
    int leftButton() const { return Qt::LeftButton; }
    int rightButton() const { return Qt::RightButton; }
    int midButton() const { return Qt::MidButton; }
    int shiftButton() const { return Qt::ShiftButton; }
    int controlButton() const { return Qt::ControlButton; }
    int altButton() const { return Qt::AltButton; }
    int metaButton() const { return Qt::MetaButton; }
};

class QtNamespaceProvider : public QSObjectFactory
{

public:
    QtNamespaceProvider();
};


#endif
