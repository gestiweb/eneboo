
#include <qapplication.h>

#include <qseditor.h>
#include "console.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    Console *w = new Console;
    w->show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    int ret = a.exec();
    delete w;
    return ret;
}
