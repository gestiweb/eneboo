/***************************************************************************
                         posprinter_unix.cpp
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

#include "posprinter.h"
#include "FLDiskCache.h"
#include "FLApplication.h"

void FLPosPrinter::setPrinterName(const QString &pName)
{
  if (pName == printerName_)
    return;

  printerName_ = pName;
  parsePrinterName();
}

void FLPosPrinter::flush()
{
  initFile();

  QFile file(fileName_);

  if (!file.open(IO_Raw | IO_WriteOnly)) {
    qWarning(file.errorString());
    return;
  }

  for (int i = 0; i < idxBuffer; i++) {
    if (escBuffer && escBuffer->contains(i)) {
      const QString &esc = (*escBuffer)[i];
      for (int j = 0; j < esc.length(); j++)
        file.putch(esc[j]);
    }

    if (strBuffer && strBuffer->contains(i))
      file.putch((*strBuffer)[i]);
  }

  file.flush();
  file.close();

  if (printerName_.isNull())
    setPrinterName("localhost:tpv");

#ifdef AQ_LPR_EXTERNAL
  QProcess *proc = new QProcess();

  proc->addArgument("/usr/bin/lpr");
  proc->addArgument("-P");
  proc->addArgument(queueName_);
  proc->addArgument(fileName_);

  QString cmd("/usr/bin/lpr ");
  cmd += "-P ";
  cmd += queueName_ + " ";
  cmd += fileName_;
  if (aqApp->consoleShown())
    qWarning(cmd);

  if (!proc->start())
    qWarning("FLPosPrinter::flush() : Error escribiendo en impresora: " + cmd);
  qApp->processEvents();
  if (proc)
    proc->deleteLater();
#else
  int optc = 6;
  char *optv[optc];

  optv[0] = (char *) "-H";
  optv[1] = (char *) server_.latin1();
  optv[2] = (char *) "-P";
  optv[3] = (char *) queueName_.latin1();
  optv[4] = (char *) "-l";
  optv[5] = (char *) fileName_.latin1();

  if (lpr_main(optc, optv) != 0)
    qWarning("FLPosPrinter::flush() : Error ejecutando lpr_main");
#endif
}
