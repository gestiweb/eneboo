#ifndef MODULEFACTORY_H
#define MODULEFACTORY_H

#include <qsobjectfactory.h>
#include <qvariant.h>

class QSInterpreter;

class ModuleFactory : public QSObjectFactory
{

public:
    ModuleFactory();

    virtual QObject *create( const QString &type,
			     const QSArgumentList &args,
			     QObject *context );
};

#endif
