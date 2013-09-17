/***************************************************************************
 AQJasperFillManager_p.h
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

#ifndef AQJASPERFILLMANAGER_P_H_
#define AQJASPERFILLMANAGER_P_H_

class AQJasperFillManager : public AQJasperFillManagerInterface
{
public:

  AQJasperFillManager(AQReportsPrivate *dd) : d(dd) {
    o_ = new JASPER::engine::JasperFillManager(wrapperIntern);
  }
  ~AQJasperFillManager() {
    delete o_;
  }

  void fillReportToFile(const char *src, const char *dst,
                        const char *srcXmlData, const char *selectExpr,
                        const char *datePattern, const char *numberPattern) {
    AQ_TRY(fillReportToFile_(src, dst, srcXmlData, selectExpr, datePattern, numberPattern));
  }

private:

  JASPER::engine::JasperFillManager *o_;
  AQReportsPrivate *d;

  void fillReportToFile_(const char *src, const char *dst,
                         const char *srcXmlData, const char *selectExpr,
                         const char *datePattern, const char *numberPattern) {
    java::io::File *xmlData = 0;
    JASPER::engine::data::JRXmlDataSource *xmlDatSrc = 0;
    JASPER::engine::JRDataSource *datSrc = 0;

    xmlData = new java::io::File(srcXmlData);
    xmlDatSrc = new JASPER::engine::data::JRXmlDataSource(xmlData, selectExpr);
    xmlDatSrc->setDatePattern(datePattern);
    xmlDatSrc->setNumberPattern(numberPattern);
    datSrc = new JASPER::engine::JRDataSource(xmlDatSrc->getJavaObject());

    o_->fillReportToFile(src, dst, 0, datSrc);

    delete datSrc;
    delete xmlDatSrc;
    delete xmlData;
  }
};

#endif /* AQJASPERFILLMANAGER_P_H_ */
