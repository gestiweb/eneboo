#include <qapplication.h>
#include "spreadsheet.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    SpreadSheet *w = new SpreadSheet;
    w->show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    int ret = a.exec();

    w->project->commitEditorContents();
    w->project->save();

    delete w;
    return ret;
}
