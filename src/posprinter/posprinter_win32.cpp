/***************************************************************************
                         posprinter_win32.cpp
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

void FLPosPrinter::flush()
{
  if (!file)
    initFile();

  QString fileName(AQ_DISKCACHE_DIRPATH + "/outposprinter.tmp");

  for (int i = 0; i < idxBuffer; i++) {
    if (escBuffer && escBuffer->contains(i)) {
      const QString &esc = (*escBuffer)[i];
      for (int j = 0; j < esc.length(); j++)
        file->putch(esc[j]);
    }

    if (strBuffer && strBuffer->contains(i))
      file->putch((*strBuffer)[i]);
  }

  file->flush();
  file->close();

  if (printerName_.isNull())
    printerName_ = "localhost:tpv";

  int posdots = printerName_.find(":");
  QString server = printerName_.left(posdots);
  QString name = printerName_.right(printerName_.length() - posdots - 1);

#ifdef AQ_LPR_EXTERNAL
  QProcess *proc = new QProcess();

  proc->addArgument("lpr.exe");
  proc->addArgument("-S");
  proc->addArgument(server);
  proc->addArgument("-P");
  proc->addArgument(name);
  proc->addArgument("-o");
  proc->addArgument("l");
  proc->addArgument(fileName);

  if (!proc->start())
    qWarning("FLPosPrinter::flush() : Error escribiendo en impresora " + printerName_);

  while (proc->isRunning())
    qApp->processEvents();

  qApp->processEvents();

  if (proc)
    delete proc;
#else
  int optc = 6;
  char *optv[optc];

  optv[0] = (char *) "-H";
  optv[1] = (char *) server.latin1();
  optv[2] = (char *) "-P";
  optv[3] = (char *) name.latin1();
  optv[4] = (char *) "-l";
  optv[5] = (char *) fileName.latin1();

  if (lpr_main(optc, optv) != 0)
    qWarning("FLPosPrinter::flush() : Error escribiendo en impresora /usr/bin/lpr -P " + printerName_);
#endif
}
