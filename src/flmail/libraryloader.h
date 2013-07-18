/***************************************************************************
                          libraryloader.h  -  description
                             -------------------
    begin                : Sat Apr 20 2002
    copyright            : (C) 2002 by theKompany.com
    author               : Eugen Constantinescu
    email                : eug@thekompany.com
 ***************************************************************************/

#ifndef LIBRARYLOADER_H
#define LIBRARYLOADER_H


namespace tkcSSL
{
  class Library;
  class LibraryLoaderPrivate;

  /**It loads a library at runtime and finds symbols.
    *@author Eugen Constantinescu
    * It's part of korelib too.
    */
  class LibraryLoader
  {
  public:

    enum { LAZY = 1, NOW, GLOBAL = 4};

    static LibraryLoader *ref();
    virtual ~LibraryLoader();

    virtual Library *loadLibrary(const char *libName, const char *libPath = 0, int libFlags = 0);
    virtual const char *lastError();

    virtual char *libName2fileName(const char *libName);
    virtual char *fileName2libName(const char *fileName);

  private:
    LibraryLoader();

    static LibraryLoader *_libraryLoaderRef;
    LibraryLoaderPrivate *_d;
  };
};

#endif
