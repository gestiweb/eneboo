/***************************************************************************
                          library.cpp  -  description
                             -------------------
    begin                : Sat Apr 20 2002
    copyright            : (C) 2002 by theKompany.com
    author               : Eugen Constantinescu
    email                : eug@thekompany.com
 ***************************************************************************/

#include "library.h"
#include "sslbase.h"

#ifndef _WIN32
#include <dlfcn.h>
#endif

#ifndef _WIN32
using namespace std;
#endif

using namespace tkcSSL;

namespace tkcSSL
{
  class LibraryPrivate
  {
  public:
    LibraryPrivate() {
      libname = 0;
      libpath = 0;
      flags = 0;
    }
    ~LibraryPrivate() {
    }

    HMODULE handle;
    const char *libname;
    const char *libpath;
    int flags;
  };
};


Library::Library()
{
  _d = new LibraryPrivate();
}

Library::Library(HMODULE handle, const char *libname, const char *libpath, int flags)
{
  _d = new LibraryPrivate();
  _d->handle = handle;
  _d->libname = libname;
  _d->libpath = libpath;
  _d->flags = flags;
}

Library::~Library()
{
  //#if defined( TKCSSL_WIN32 )
#if defined( TKCSSL_WIN32_LD ) // ### AbanQ
  FreeLibrary(_d->handle);
#elif defined( TKCSSL_BEOS )
  unload_add_on(_d->handle);
#elif defined( TKCSSL_ATHEOS )
  unload_library(_d->handle);
#elif defined( TKCSSL_HP )
  shl_unload(_d->handle);
#else
  dlclose(_d->handle);
#endif

  delete _d;
}

HMODULE Library::libHandle() const
{
  return _d->handle;
}

const char *Library::name() const
{
  return _d->libname;
}

const char *Library::path() const
{
  return _d->libpath;
}

void *Library::symbolAddress(const char *name)
{
  void *sym = 0;
  //#if defined( TKCSSL_WIN32 )
#if defined( TKCSSL_WIN32_LD ) // ### AbanQ
  sym = GetProcAddress(_d->handle, name);
#elif defined( TKCSSL_BEOS )
  if (B_OK != get_image_symbol(_d->handle, name, B_SYMBOL_TYPE_TEXT, &sym))
    sym = 0;
#elif defined( TKCSSL_ATHEOS )
  if (0 != get_symbol_address(_d->handle, name, -1, &sym))
    sym = 0;
#elif defined( TKCSSL_HP )
  if (0 > shl_findsym(_d->handle, name, TYPE_UNDEFINED, &sym))
    sym = 0;
#else
  sym = dlsym(_d->handle, name);
#endif
  return sym;
}
