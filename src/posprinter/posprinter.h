/***************************************************************************
                             posprinter.h
                         -------------------
begin                : Vie Mar 10 2006
copyright            : (C) 2006 by InfoSiAL S.L.
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

#ifndef POSPRINTER_H
#define POSPRINTER_H

#include <qpaintdevice.h>
#include <qpaintdevicemetrics.h>
#include <qprocess.h>
#include <qapplication.h>
#include <qmap.h>

#include <private/qtextengine_p.h>
#include <private/qtextlayout_p.h>

#ifndef AQ_LPR_EXTERNAL
extern "C" int lpr_main(int, char **);
#endif

class FLPosPrinter : public QPaintDevice
{

public:

  FLPosPrinter();
  ~FLPosPrinter();

  enum PaperWidth { P57_5MM, P69_5MM, P76MM, NPaperWidth = P76MM };

  PaperWidth paperWidth() const {
    return paperWidth_;
  }

  virtual void setPaperWidth(PaperWidth);

  QString printerName() const {
    return printerName_;
  }

  void setPrinterName(const QString &pName);

  void cleanup();
  void flush();
  void send(const QString &str, const int col = -1, const int row = -1);

protected:

  bool cmd(int, QPainter *, QPDevCmdParam *);
  int metric(int) const;

  int paperWidthToCols();

  void sendStr(const char c, const int col = -1, const int row = -1);
  void sendEsc(const QString &e, const int col = -1, const int row = -1);

  void initFile();
  void initStrBuffer();
  void initEscBuffer();

private:

  void parsePrinterName();

  PaperWidth paperWidth_;
  QMap< int, char > *strBuffer;
  QMap< int, QString > *escBuffer;
  int idxBuffer;
  QString fileName_;
  static QString printerName_;
  static QString server_;
  static QString queueName_;
#if defined(Q_OS_WIN32)
  static bool useLPT_;
  static QString nameLPT_;
#endif
};

#endif
