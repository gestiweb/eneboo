#include <qapplication.h>

#include <qsinterpreter.h>
#include <qsproject.h>

#ifndef QSA_NO_IDE
# include <qsworkbench.h>
#else
# include <qmessagebox.h>
#endif

#include "modulefactory.h"
#include "modules.h"

int main( int argc, char **argv )
{
    QApplication app( argc, argv );

#ifndef QSA_NO_IDE
    QObject::connect( &app, SIGNAL( lastWindowClosed() ),
		      &app, SLOT( quit() ) );

    QSProject proj( 0, "FilterModules" );
    proj.load( "filter.qsa" );

    Renderer ren( proj.interpreter() );
    proj.addObject( &ren );

    ModuleFactory factory;
    proj.interpreter()->addObjectFactory( &factory );

    QSWorkbench workbench( &proj );
    workbench.open();
    app.exec();

    proj.commitEditorContents();
    proj.save( "filter.qsa" );
#else
    QMessageBox::information( 0, "Disabled feature",
			      "QSA Workbench has been disabled. Reconfigure to enable",
			      QMessageBox::Ok );
#endif
    return 0;
}
