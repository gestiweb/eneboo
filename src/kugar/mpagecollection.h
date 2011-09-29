/***************************************************************************
             mpagecollection.h  -  Kugar report page collection
             -------------------
   begin     : Fri Aug 20 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#ifndef MPAGECOLLECTION_H
#define MPAGECOLLECTION_H

#include <qobject.h>
#include <qptrlist.h>
#include <qpicture.h>
#include <qsize.h>

/**
Kugar report page collection

@author Mutiny Bay Software
*/
class MPageCollection: public QObject
{

public:

  /**
  Constructor
  */
  MPageCollection(QObject *parent = 0);

  /**
  Copy constructor
  */
  MPageCollection(const MPageCollection &mPageCollection);

  /**
  Assignment operator
  */
  MPageCollection operator=(const MPageCollection &mPageCollection);

  /**
  Destructor
  */
  virtual ~ MPageCollection();

private:

  /**
  The report page list
  */
  QPtrList<QPicture> *pages;

  /**
  Page dimensions
  */
  QSize dimensions;

  /**
  Page size
  */
  int size;

  /**
  Page orientation
  */
  int orientation;

  /**
  Print to POS Printer
  */
  bool printToPos_;

public:

  /**
  Clears the page collection
  */
  void clear();

  /**
  Appends a new page to the page collection
  */
  void appendPage();

  /**
  Gets the current page in the page collection,
  the current page may be null
  */
  QPicture *getCurrentPage() const;

  /**
  Gets the first page in the page collection,
  returns NULL if the list is empty
  */
  QPicture *getFirstPage();

  /**
  Get the previous page in the page collection,
  returns NULL if the beginning of the list has been reached
  */
  QPicture *getPreviousPage();

  /**
  Gets the next page in the page collection,
  returns NULL if the end of the list has been reached
    */
  QPicture *getNextPage();

  /**
  Gets the last page in the page collection,
  returns NULL if the list empty
  */
  QPicture *getLastPage();

  /**
  Gets the page at the given index in the page collection,
  returns NULL if the list empty
  */
  QPicture *getPageAt(uint i);

  /**
  Get the index of the current page
  */
  int getCurrentIndex() const;

  /**
  Set the current page to page at idx
  */
  void setCurrentPage(int idx);

  /**
  Sets the page size
  */
  void setPageSize(int s);

  /**
  Sets the page orientation
  */
  void setPageOrientation(int o);

  /**
  Sets the page dimensions
  */
  void setPageDimensions(QSize dim);

  /**
  Returns the page size
  */
  int pageSize() const;

  /**
  Returns the page orientation
  */
  int pageOrientation() const;

  /**
  Returns the page dimensions
  */
  QSize pageDimensions() const;

  /**
  Returns the number of pages in the page collection
  */
  int pageCount() const;

  /**
  Returns if print to POS printer
  */
  bool printToPos() const;

  /**
  Set print to POS printer
  */
  void setPrintToPos(bool ptp);

private:

  /**
  Copies member data from one object to another.

  Used by the copy constructor and assignment operator
  */
  void copy(const MPageCollection *mPageCollection);
};

/** Gets the current page in the page collection,
  * the current page may be null
  */
inline QPicture *MPageCollection::getCurrentPage() const
{
  return pages->current();
}

/** Gets the first page in the page collection,
  * returns NULL if the list is empty
  */
inline QPicture *MPageCollection::getFirstPage()
{
  return pages->first();
}

/** Gets the next page in the page collection,
  * returns NULL if the end of the list has been reached
  */
inline QPicture *MPageCollection::getNextPage()
{
  return pages->next();
}

/** Get the previous page in the page collection,
  * returns NULL if the beginning of the list has been reached
  */
inline QPicture *MPageCollection::getPreviousPage()
{
  return pages->prev();
}

/** Gets the last page in the page collection,
  * returns NULL if the list is empty
  */
inline QPicture *MPageCollection::getLastPage()
{
  return pages->last();
}

/** Gets the page at the given index in the page collection,
  * returns NULL if the list is empty
  */
inline QPicture *MPageCollection::getPageAt(uint i)
{
  return pages->at(i);
}

/** Set the current page to page at idx */
inline void MPageCollection::setCurrentPage(int idx)
{
  pages->at(idx);
}

/** Get the index of the current page */
inline int MPageCollection::getCurrentIndex() const
{
  return pages->at();
}

/** Sets the page size */
inline void MPageCollection::setPageSize(int s)
{
  size = s;
}

/** Sets the page orientation */
inline void MPageCollection::setPageOrientation(int o)
{
  orientation = o;
}

/** Sets the page dimensions  */
inline void MPageCollection::setPageDimensions(QSize dim)
{
  dimensions = dim;
}

/** Returns the page size */
inline int MPageCollection::pageSize() const
{
  return size;
}

/** Returns the page orientation */
inline int MPageCollection::pageOrientation() const
{
  return orientation;
}

/** Returns the page dimensions  */
inline QSize MPageCollection::pageDimensions() const
{
  return dimensions;
}

/** Returns the number of pages in the page collection */
inline int MPageCollection::pageCount() const
{
  return pages->count();
}

inline bool MPageCollection::printToPos() const
{
  return printToPos_;
}

inline void MPageCollection::setPrintToPos(bool ptp)
{
  printToPos_ = ptp;
}
#endif
