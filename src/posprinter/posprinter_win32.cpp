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
#include "FLApplication.h"

void FLPosPrinter::setPrinterName(const QString &pName)
{
  if (pName == printerName_)
    return;

  printerName_ = pName;
  parsePrinterName();

  useLPT_ = server_.startsWith("LPT1@") ||
            server_.startsWith("LPT2@") ||
            server_.startsWith("LPT3@");
  if (useLPT_) {
    server_ = server_.mid(5);
    nameLPT_ = printerName_.left(4);

    QString resource("\\\\");
    resource += server_;
    resource += "\\" + queueName_;

    QProcess *proc = new QProcess();
    proc->addArgument("cmd.exe");
    proc->addArgument("/C");
    proc->addArgument("net use");
    proc->addArgument(nameLPT_);
    proc->addArgument("/delete");

    QString cmd("cmd.exe /C ");
    cmd += "net use ";
    cmd += nameLPT_ + " /delete";
    if (aqApp->consoleShown())
      qWarning(cmd);

    if (!proc->start())
      qWarning("FLPosPrinter: Error ejecutando: " + cmd);
    while (proc->isRunning())
      qApp->processEvents();
    qApp->processEvents();
    if (proc)
      proc->deleteLater();


    proc = new QProcess();
    proc->addArgument("cmd.exe");
    proc->addArgument("/C");
    proc->addArgument("net use");
    proc->addArgument(nameLPT_ + ":");
    proc->addArgument(resource);
    proc->addArgument("/persistent:yes");

    cmd = "cmd.exe /C ";
    cmd += "net use ";
    cmd += nameLPT_ + ": ";
    cmd += resource + " ";
    cmd += "/persistent:yes";
    if (aqApp->consoleShown())
      qWarning(cmd);

    if (!proc->start())
      qWarning("FLPosPrinter: Error ejecutando: " + cmd);
    while (proc->isRunning())
      qApp->processEvents();
    qApp->processEvents();
    if (proc)
      proc->deleteLater();
  }
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

  if (!useLPT_) {
#ifdef AQ_LPR_EXTERNAL
    QProcess *proc = new QProcess();

    proc->addArgument("lpr.exe");
    proc->addArgument("-S");
    proc->addArgument(server_);
    proc->addArgument("-P");
    proc->addArgument(queueName_);
    proc->addArgument("-o");
    proc->addArgument("l");
    proc->addArgument(fileName_);

    QString cmd("lpr.exe -S ");
    cmd += server_ + " ";
    cmd += "-P ";
    cmd += queueName_ + " -o l ";
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
  } else {
    QString fileName(QDir::cleanDirPath(fileName_));
    fileName = QDir::convertSeparators(fileName);

    QProcess *proc = new QProcess();
    proc->addArgument("cmd.exe");
    proc->addArgument("/C");
    proc->addArgument("copy");
    proc->addArgument(fileName);
    proc->addArgument(nameLPT_);

    QString cmd("cmd.exe /C ");
    cmd += "copy ";
    cmd += fileName + " ";
    cmd += nameLPT_;
    if (aqApp->consoleShown())
      qWarning(cmd);

    if (!proc->start())
      qWarning("FLPosPrinter: Error ejecutando: " + cmd);
    qApp->processEvents();
    if (proc)
      proc->deleteLater();
  }
}
