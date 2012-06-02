#include "gamewindow.h"
#include <qapplication.h>

int main( int argc, char ** argv )
{
    QApplication app( argc, argv );
    GameWindow window;
    app.setMainWidget( &window );
    window.show();
    return app.exec();
}


