/***************************************************************************
 AQReports_p.h
 -------------------
 begin                : 12/01/2011
 copyright            : (C) 2003-2011 by InfoSiAL S.L.
 email                : mail@infosial.com
 ***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 ***************************************************************************/
/***************************************************************************
 Este  programa es software libre. Puede redistribuirlo y/o modificarlo
 bajo  los  términos  de  la  Licencia  Pública General de GNU   en  su
 versión 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

#ifndef AQREPORTS_P_H_
#define AQREPORTS_P_H_

#include <java_magic.h>
#include <aq_jar_deps.h>

#include <cstring>
#include <string>
#include <list>

using namespace std;
typedef list<string> AQStdStringList;

enum AQReportsMsgType { AQErrorMsg, AQLogMsg, AQSysMsg };
typedef void (*AQMsgHandler)(AQReportsMsgType, const char *, void *);

#define AQ_ERR_MSG(M) if (d->msgHandler_) \
    d->msgHandler_(AQErrorMsg, M, d->receiver_); else printf("\nError: %s\n", M)
#define AQ_LOG_MSG(M) if (d->msgHandler_) \
    d->msgHandler_(AQLogMsg, M, d->receiver_); else printf("\nLog: %s\n", M)
#define AQ_SYS_MSG(M) if (d->msgHandler_) \
    d->msgHandler_(AQSysMsg, M, d->receiver_); else printf("\nSys:  %s\n", M)
#define JASPER net::sf::jasperreports

#define AQ_CATCH                                                  \
  catch (std::exception &e) {                                     \
    AQ_ERR_MSG(e.what());                                         \
  }                                                               \
  catch (jthrowable exc) {                                        \
    java::lang::Throwable *thr = new java::lang::Throwable(exc);  \
    AQ_ERR_MSG(thr->getMessage());                                \
    delete thr;                                                   \
  }

#define AQ_TRY(_f)        \
  try {                   \
    _f;                   \
  } AQ_CATCH

#define AQ_TRY_VAL(_f,V)  \
  try {                   \
    V = _f;               \
  } AQ_CATCH

class AQReportsPrivate
{
public:

  AQReportsPrivate() :
    conn_(0), msgHandler_(0), receiver_(0) {}
  ~AQReportsPrivate() {
    delete conn_;
  }

  static bool initDone_;

  java::sql::Connection *conn_;
  AQMsgHandler msgHandler_;
  void *receiver_;
};

bool AQReportsPrivate::initDone_ = false;

#endif /* AQREPORTS_P_H_ */
