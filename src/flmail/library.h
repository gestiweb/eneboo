/***************************************************************************
                          library.h  -  description
                             -------------------
    begin                : Sat Apr 20 2002
    copyright            : (C) 2002 by theKompany.com
    author               : Eugen Constantinescu
    email                : eug@thekompany.com
 ***************************************************************************/


// The base code is from korelib
// I have just changed KORE to TKCSSL
// and namespace name for avoiding any conflicts with korelib
// if both are used at the same time.


#ifndef TKCSSL_LIBRARY_H
#define TKCSSL_LIBRARY_H

#include <sslbase.h>

namespace tkcSSL
{
  class LibraryPrivate;

  /**It has the library handler and it return the symbols address.
    *@author Eugen Constantinescu
    * It is part of korelib too.
    */

  class Library
  {
  public:
    Library();
    Library(HMODULE handle, const char *libname, const char *libpath, int flags);
    virtual ~Library();

    virtual HMODULE libHandle() const;
    virtual const char *name() const;
    virtual const char *path() const;

    virtual void *symbolAddress(const char *name);

  private:
    LibraryPrivate *_d;
  };
};

#endif
