/***************************************************************************
 AQReports.h
 -------------------
 begin                : 09/06/2010
 copyright            : (C) 2003-2010 by InfoSiAL S.L.
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

#ifndef AQREPORTS_H_
#define AQREPORTS_H_

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// AQReports Interfaces
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AQReportsInterface
{
public:
  AQReportsInterface() {}
  virtual ~AQReportsInterface() {}
  virtual bool init(const char *) = 0;
  virtual const char *jarDeps() const = 0;
  virtual bool setConnection(const char *) = 0;
  virtual bool viewReport(const char *) = 0;
  virtual bool viewReport(const char *, const char *) = 0;
  virtual void installMsgHandler(AQMsgHandler, void *) = 0;
  virtual void *create(const char *) = 0;
};

class AQJasperCompileManagerInterface
{
public:
  virtual void compileReportToFile(const char *, const char *) = 0;
};

class AQJasperFillManagerInterface
{
public:
  virtual void fillReportToFile(const char *, const char *,
                                const char *, const char *,
                                const char *, const char *) = 0;
};

class AQJasperPrintManagerInterface
{
public:
  virtual void loadJasperPrint(const char *) = 0;
  virtual int pageCount() const = 0;
  virtual int pageWidth() const = 0;
  virtual int pageHeight() const = 0;
  virtual int pageOrientation() = 0;
  virtual void printPageToImage(const char *, int, const char *) = 0;
  virtual bool printReport(bool) = 0;
  virtual bool printReport(const char *, int = 1) = 0;
  virtual bool exportReportToFile(const char *, const char *) = 0;
};

class AQJavaPrintServiceInterface
{
public:
  virtual AQStdStringList printServiceNames() = 0;
private:
  virtual void *printService(const char *) = 0;
  friend class AQJasperPrintManagerInterface;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// AQReports Interfaces
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif /* AQREPORTS_H_ */
