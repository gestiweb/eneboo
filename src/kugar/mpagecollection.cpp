/***************************************************************************
           mpagecollection.cpp  -  Kugar report page collection
           -------------------
 begin     : Fri Aug 20 1999
 copyright : (C) 1999 by Mutiny Bay Software
 email     : info@mutinybaysoftware.com
***************************************************************************/

#include "mpagecollection.h"
#include "mreportengine.h"

/** Constructor */
MPageCollection::MPageCollection(QObject *parent) : QObject(parent)
{
  // Set page list to AutoDelete
  pages = new QPtrList < QPicture > ();
  pages->setAutoDelete(true);

  // Set the metrics
  size = MReportEngine::Letter;
  orientation = MReportEngine::Portrait;
  dimensions.setWidth(0);
  dimensions.setHeight(0);

  printToPos_ = false;
}

/** Copy constructor */
MPageCollection::MPageCollection(const MPageCollection &mPageCollection)
{
  copy(&mPageCollection);
}

/** Assignment operator */
MPageCollection MPageCollection::operator=(const MPageCollection &mPageCollection)
{
  if (&mPageCollection == this)
    return * this;

  // Copy the derived class's data
  copy(&mPageCollection);

  return *this;
}

/** Destructor */
MPageCollection::~MPageCollection()
{
  if (pages) {
    pages->clear();
    delete pages;
  }
}

/** Clears the page collection */
void
MPageCollection::clear()
{
  pages->clear();
}

/** Appends a new page to the page collection */
void MPageCollection::appendPage()
{
  pages->append(new QPicture());
}

/** Copies member data from one object to another.
  Used by the copy constructor and assignment operator */
void MPageCollection::copy(const MPageCollection *mPageCollection)
{
  // Copy the page collection
  pages = mPageCollection->pages;

  // Copy the page metrics
  dimensions = mPageCollection->dimensions;
  size = mPageCollection->size;
  orientation = mPageCollection->orientation;
}
