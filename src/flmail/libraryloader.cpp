/***************************************************************************
                          libraryloader.cpp  -  description
                             -------------------
    begin                : Sat Apr 20 2002
    copyright            : (C) 2002 by theKompany.com
    author               : Eugen Constantinescu
    email                : eug@thekompany.com
 ***************************************************************************/

#include "libraryloader.h"
#include "library.h"

#ifndef _WIN32
#include <dlfcn.h>
#endif

#include <iostream>
#include <string>

using namespace std;
using namespace tkcSSL;

namespace tkcSSL
{
  class LibraryLoaderPrivate
  {
  public:
    LibraryLoaderPrivate() {
      lastError = new char[201];
    }
    ~LibraryLoaderPrivate() {
      delete lastError;
    }

    char *lastError;
  };
};

LibraryLoader *LibraryLoader::_libraryLoaderRef = 0;

LibraryLoader *LibraryLoader::ref()
{
  if (!_libraryLoaderRef) {
    _libraryLoaderRef = new LibraryLoader();
  }

  return _libraryLoaderRef;
}

LibraryLoader::LibraryLoader()
{
  _d = new LibraryLoaderPrivate();
}

LibraryLoader::~LibraryLoader()
{
  delete _d;
}

Library *LibraryLoader::loadLibrary(const char *libName, const char *libPath, int libFlags)
{
  HMODULE handle;
  char *fileName = libName2fileName(libName);
  char *filePath = 0;
  Library *res = 0;
  if (libPath && libPath[0]) {
    int n = strlen(libPath);
    int slash = libPath[n - 1] == '/' ? 0 : 1;
    filePath = new char[n + strlen(fileName) + slash + 1];
    strcpy(filePath, libPath);
    if (slash) {
      filePath[n] = '/';
      filePath[n + 1] = 0;
    }
  } else {
    filePath = new char[strlen(fileName) + 1];
    filePath[0] = 0;
  }
  strcat(filePath, fileName);

  //cerr << "Loading: " << filePath << endl;

  delete[] fileName;
  //#if defined( TKCSSL_WIN32 )
#if defined( TKCSSL_WIN32_LD ) // ### AbanQ
  handle = LoadLibrary((wchar_t *)QString(filePath).ucs2());
#elif defined( TKCSSL_BEOS )
  handle = load_add_on(filePath);
#elif defined( TKCSSL_ATHEOS )
  handle = load_library(filePath, 0);
#else
  int flags = 0;
  if (libFlags & LAZY)
    flags |= RTLD_LAZY;
  if (libFlags & NOW)
    flags |= RTLD_NOW;
  if (libFlags & GLOBAL)
    flags |= RTLD_GLOBAL;
  //
  if (!(flags & (RTLD_LAZY | RTLD_NOW)))
    flags |= RTLD_LAZY;
  handle = dlopen(filePath, flags);
#endif
  delete[] filePath;
  //cout << "Loaded: " << handle << endl;
#if defined( TKCSSL_BEOS )
  if (!(handle < B_NO_ERROR))
#elif defined( TKCSSL_ATHEOS )
  if (!(handle < 0))
#else
  if (handle)
#endif
    res = new Library(handle, libName, libPath, libFlags);
  return res;
}

// "my_plugin" -> "libmy_plugin.so"/"my_plugin.dll"/"libmy_plugin.dylib"
char *LibraryLoader::libName2fileName(const char *libName)
{
  int n = strlen(libName);
  //#ifdef TKCSSL_WIN32
#ifdef TKCSSL_WIN32_LD // ### AbanQ
  char *fn = new char[n + 1 + 4];
  return strcat(strcpy(fn, libName), ".dll");
#elif defined( TKCSSL_MAC )
  char *fn = new char[n + 1 + 9];
  return strcat(strcat(strcpy(fn, "lib"), libName), ".dylib");
#else
  char *fn = new char[n + 1 + 6];
  return strcat(strcat(strcpy(fn, "lib"), libName), ".so");
#endif
}

// "libmy_plugin.so"/"my_plugin.dll"/"libmy_plugin.dylib" -> "my_plugin"
char *LibraryLoader::fileName2libName(const char *fileName)
{
  //#ifdef TKCSSL_WIN32
#ifdef TKCSSL_WIN32_LD // ### AbanQ
  int n = strlen(fileName) - 4;
  char *ln = new char[n + 1];
#elif defined( TKCSSL_MAC )
  char *pos = strstr(fileName, ".dylib");
  int n = pos - fileName - 6;
  char *ln = new char[n + 1];
  fileName += 6;
#else
  const char *pos = strstr(fileName, ".so");
  int n = pos - fileName - 3;
  char *ln = new char[n + 1];
  fileName += 3;
#endif
  strncpy(ln, fileName, n);
  ln[n] = 0;
  return ln;
}

const char *LibraryLoader::lastError()
{
  _d->lastError[0] = 0;
  //#if defined( TKCSSL_WIN32 )
#if defined( TKCSSL_WIN32_LD ) // ### AbanQ
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0,
                GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (wchar_t *)_d->lastError, 200, NULL);
#elif defined( TKCSSL_BEOS )
  strncpy(_d->lastError, strerror(errno), 200);
#elif defined( TKCSSL_ATHEOS )
  strncpy(_d->lastError, strerror(errno), 200);
#else
  strncpy(_d->lastError, dlerror(), 200);
#endif
  return _d->lastError;
}

