/***************************************************************************
 FLReportPages.cpp
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

#include "mpagecollection.h"
#include "FLPicture.h"
#include "FLReportPages.h"

void FLReportPages::setPageCollection(MPageCollection *pages)
{
  if (pages_)
    pages_->deleteLater();
  if (pages)
    insertChild(pages);
  pages_ = pages;
}

FLPicture *FLReportPages::getCurrentPage()
{
  return (pages_ ? new FLPicture(pages_->getCurrentPage(), this) : 0);
}

FLPicture *FLReportPages::getFirstPage()
{
  return (pages_ ? new FLPicture(pages_->getFirstPage(), this) : 0);
}

FLPicture *FLReportPages::getPreviousPage()
{
  return (pages_ ? new FLPicture(pages_->getPreviousPage(), this) : 0);
}

FLPicture *FLReportPages::getNextPage()
{
  return (pages_ ? new FLPicture(pages_->getNextPage(), this) : 0);
}

FLPicture *FLReportPages::getLastPage()
{
  return (pages_ ? new FLPicture(pages_->getLastPage(), this) : 0);
}

FLPicture *FLReportPages::getPageAt(uint i)
{
  return (pages_ ? new FLPicture(pages_->getPageAt(i), this) : 0);
}

void FLReportPages::clearPages()
{
  if (pages_)
    pages_->clear();
}

void FLReportPages::appendPage()
{
  if (pages_)
    pages_->appendPage();
}

int FLReportPages::getCurrentIndex()
{
  if (pages_)
    return pages_->getCurrentIndex();
  return -1;
}

void FLReportPages::setCurrentPage(int idx)
{
  if (pages_)
    pages_->setCurrentPage(idx);
}

void FLReportPages::setPageSize(int s)
{
  if (pages_)
    pages_->setPageSize(s);
}

void FLReportPages::setPageOrientation(int o)
{
  if (pages_)
    pages_->setPageOrientation(o);
}

void FLReportPages::setPageDimensions(QSize dim)
{
  if (pages_)
    pages_->setPageDimensions(dim);
}

int FLReportPages::pageSize()
{
  if (pages_)
    return pages_->pageSize();
  return -1;
}

int FLReportPages::pageOrientation()
{
  if (pages_)
    return pages_->pageOrientation();
  return -1;
}

QSize FLReportPages::pageDimensions()
{
  if (pages_)
    return pages_->pageDimensions();
  return QSize();
}

int FLReportPages::pageCount()
{
  if (pages_)
    return pages_->pageCount();
  return -1;
}
