/***************************************************************************
 AQJasperCompileManager_p.h
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

#ifndef AQJASPERCOMPILEMANAGER_P_H_
#define AQJASPERCOMPILEMANAGER_P_H_

class AQJasperCompileManager : public AQJasperCompileManagerInterface
{
public:

  AQJasperCompileManager(AQReportsPrivate *dd) : d(dd) {
    o_ = new JASPER::engine::JasperCompileManager(wrapperIntern);
  }
  ~AQJasperCompileManager() {
    delete o_;
  }

  void compileReportToFile(const char *src, const char *dst) {
    AQ_TRY(o_->compileReportToFile(src, dst));
  }

private:

  JASPER::engine::JasperCompileManager *o_;
  AQReportsPrivate *d;
};

#endif /* AQJASPERCOMPILEMANAGER_P_H_ */
