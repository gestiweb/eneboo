/***************************************************************************
           mreportdetail.cpp  -  Kugar report detail section
           -------------------
 begin     : Mon Aug 23 1999
 copyright : (C) 1999 by Mutiny Bay Software
 email     : info@mutinybaysoftware.com
***************************************************************************/

#include "mreportdetail.h"

/** Constructor */
MReportDetail::MReportDetail(const QString &strIdSec) : MReportSection(strIdSec)
{
}

/** Copy constructor */
MReportDetail::MReportDetail(const MReportDetail &mReportDetail) : MReportSection((MReportSection &) mReportDetail)
{
}

/** Assignment operator */
MReportDetail MReportDetail::operator=(const MReportDetail &mReportDetail)
{
  if (&mReportDetail == this)
    return * this;

  // Copy the base class's data
  ((MReportSection &) * this) = mReportDetail;

  return *this;
}

/** Destructor */
MReportDetail::~MReportDetail()
{
}
