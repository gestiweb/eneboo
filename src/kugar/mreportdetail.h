/***************************************************************************
             mreportdetail.h  -  Kugar report detail section
             -------------------
   begin     : Mon Aug 23 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#ifndef MREPORTDETAIL_H
#define MREPORTDETAIL_H

#include "mfieldobject.h"
#include "mreportsection.h"

/**
Kugar report detail section

@author Mutiny Bay Software
*/
class MReportDetail: public MReportSection
{

public:

  /**
  Constructor
  */
  MReportDetail(const QString &strIdSec);

  /**
  Copy constructor
  */
  MReportDetail(const MReportDetail &mReportDetail);

  /**
  Assignment operator
  */
  MReportDetail operator=(const MReportDetail &mReportDetail);

  /**
  Destructor
  */
  virtual ~ MReportDetail();
};

#endif
