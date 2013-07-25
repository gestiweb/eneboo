// hmm

#include <qglobal.h>

#ifdef QT_WS_WIN
__declspec(dllexport) char * AppName = 0L;
__declspec(dllexport) char * AppMIME = 0L;
__declspec(dllexport) char * AppVersion = 0L;
__declspec(dllexport) char * FullAppName = 0L;
#else
char * AppName = 0L;
char * AppMIME = 0L;
char * AppVersion = 0L;
char * FullAppName = 0L;
#endif

