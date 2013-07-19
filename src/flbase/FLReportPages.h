/***************************************************************************
 FLReportPages.h
 -------------------
 begin                : 02/01/2011
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

#ifndef FLREPORTPAGES_H_
#define FLREPORTPAGES_H_

#include <qobject.h>

class FLPicture;
class FLReportEngine;
class FLJasperEngine;
class MPageCollection;

class FLReportPages : public QObject
{
  Q_OBJECT
  Q_ENUMS(PageOrientation)
  Q_ENUMS(PageSize)

public:

  enum PageOrientation {
    Portrait,
    Landscape
  };

  enum PageSize {
    A4, B5, Letter, Legal, Executive,
    A0, A1, A2, A3, A5, A6, A7, A8, A9, B0, B1,
    B10, B2, B3, B4, B6, B7, B8, B9, C5E, Comm10E,
    DLE, Folio, Ledger, Tabloid, Custom, NPageSize = Custom, CustomOld = 31
  };

  void setPageCollection(MPageCollection *pages);
  MPageCollection *pageCollection() const {
    return pages_;
  }

public slots:

  FLPicture *getCurrentPage();
  FLPicture *getFirstPage();
  FLPicture *getPreviousPage();
  FLPicture *getNextPage();
  FLPicture *getLastPage();
  FLPicture *getPageAt(uint i);

  void  clearPages();
  void  appendPage();
  int   getCurrentIndex();
  void  setCurrentPage(int idx);
  void  setPageSize(int s);
  void  setPageOrientation(int o);
  void  setPageDimensions(QSize dim);
  int   pageSize();
  int   pageOrientation();
  QSize pageDimensions();
  int   pageCount();

private:

  FLReportPages() : QObject(), pages_(0) {}

  MPageCollection *pages_;

  friend class FLReportEngine;
  friend class FLJasperEngine;
};

#endif /* FLREPORTPAGES_H_ */
