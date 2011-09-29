/***************************************************************************
 AQSPrinter_p.h
 -------------------
 begin                : 14/02/2011
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

#ifndef AQSPRINTER_P_H_
#define AQSPRINTER_P_H_

#include "AQSVoidPtr_p.h"
#include <qprinter.h>

class AQSPrinter : public AQSVoidPtr
{
  Q_OBJECT

  AQ_DECLARE_AQS_VOID(Printer, VoidPtr);

public slots:
  QPaintDevice *paintDevice() const {
    return static_cast<QPaintDevice *>(o_);
  }

  //@AQ_BEGIN_DEF_PUB_SLOTS@
public slots:
  QString printerName() const;
  void setPrinterName(const QString &);
  bool outputToFile() const;
  void setOutputToFile(bool);
  QString outputFileName() const;
  void setOutputFileName(const QString &);
  QString printProgram() const;
  void setPrintProgram(const QString &);
  QString printerSelectionOption() const;
  void setPrinterSelectionOption(const QString &);
  QString docName() const;
  void setDocName(const QString &);
  QString creator() const;
  void setCreator(const QString &);
  uint orientation() const;
  void setOrientation(uint);
  uint pageSize() const;
  void setPageSize(uint);
  void setPageOrder(uint);
  uint pageOrder() const;
  void setResolution(int);
  int resolution() const;
  void setColorMode(uint);
  uint colorMode() const;
  void setFullPage(bool);
  bool fullPage() const;
  QSize margins() const;
  void setMargins(uint, uint, uint, uint);
  int fromPage() const;
  int toPage() const;
  void setFromTo(int, int);
  int minPage() const;
  int maxPage() const;
  void setMinMax(int, int);
  int numCopies() const;
  void setNumCopies(int);
  uint printRange() const;
  void setPrintRange(uint);
  bool newPage();
  bool abort();
  bool aborted() const;
  bool setup(QWidget* = 0);
  uint paperSource() const;
  void setPaperSource(uint);
  void setOptionEnabled(uint, bool);
  bool isOptionEnabled(uint);

protected:
  static void *construct(const QSArgumentList &args) {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QPrinter::PrinterMode"));
    QString sgt(castArgsSignature(args, candidates));
    if (sgt.isEmpty())
      return new QPrinter;
    if (sgt == QString::fromLatin1("QPrinter::PrinterMode"))
      return new QPrinter(static_cast<QPrinter::PrinterMode>(args[0].variant().toUInt()));
    return 0;
  }

public:
  static QMap<int, QStringList> candidateConstructors() {
    QMap<int, QStringList> candidates;
    candidates[0].append(QString::null);
    candidates[1].append(QString::fromLatin1("QPrinter::PrinterMode"));
    return candidates;
  }
  //@AQ_END_DEF_PUB_SLOTS@
};

//@AQ_BEGIN_IMP_PUB_SLOTS@
inline QString AQSPrinter::printerName() const
{
  AQ_CALL_RET_V(printerName(), QString);
}
inline void AQSPrinter::setPrinterName(const QString &arg0)
{
  AQ_CALL_VOID(setPrinterName(arg0));
}
inline bool AQSPrinter::outputToFile() const
{
  AQ_CALL_RET_V(outputToFile(), bool);
}
inline void AQSPrinter::setOutputToFile(bool arg0)
{
  AQ_CALL_VOID(setOutputToFile(arg0));
}
inline QString AQSPrinter::outputFileName() const
{
  AQ_CALL_RET_V(outputFileName(), QString);
}
inline void AQSPrinter::setOutputFileName(const QString &arg0)
{
  AQ_CALL_VOID(setOutputFileName(arg0));
}
inline QString AQSPrinter::printProgram() const
{
  AQ_CALL_RET_V(printProgram(), QString);
}
inline void AQSPrinter::setPrintProgram(const QString &arg0)
{
  AQ_CALL_VOID(setPrintProgram(arg0));
}
inline QString AQSPrinter::printerSelectionOption() const
{
  AQ_CALL_RET_V(printerSelectionOption(), QString);
}
inline void AQSPrinter::setPrinterSelectionOption(const QString &arg0)
{
  AQ_CALL_VOID(setPrinterSelectionOption(arg0));
}
inline QString AQSPrinter::docName() const
{
  AQ_CALL_RET_V(docName(), QString);
}
inline void AQSPrinter::setDocName(const QString &arg0)
{
  AQ_CALL_VOID(setDocName(arg0));
}
inline QString AQSPrinter::creator() const
{
  AQ_CALL_RET_V(creator(), QString);
}
inline void AQSPrinter::setCreator(const QString &arg0)
{
  AQ_CALL_VOID(setCreator(arg0));
}
inline uint AQSPrinter::orientation() const
{
  AQ_CALL_RET_V(orientation(), uint);
}
inline void AQSPrinter::setOrientation(uint arg0)
{
  AQ_CALL_VOID(setOrientation(static_cast<QPrinter::Orientation>(arg0)));
}
inline uint AQSPrinter::pageSize() const
{
  AQ_CALL_RET_V(pageSize(), uint);
}
inline void AQSPrinter::setPageSize(uint arg0)
{
  AQ_CALL_VOID(setPageSize(static_cast<QPrinter::PageSize>(arg0)));
}
inline void AQSPrinter::setPageOrder(uint arg0)
{
  AQ_CALL_VOID(setPageOrder(static_cast<QPrinter::PageOrder>(arg0)));
}
inline uint AQSPrinter::pageOrder() const
{
  AQ_CALL_RET_V(pageOrder(), uint);
}
inline void AQSPrinter::setResolution(int arg0)
{
  AQ_CALL_VOID(setResolution(arg0));
}
inline int AQSPrinter::resolution() const
{
  AQ_CALL_RET_V(resolution(), int);
}
inline void AQSPrinter::setColorMode(uint arg0)
{
  AQ_CALL_VOID(setColorMode(static_cast<QPrinter::ColorMode>(arg0)));
}
inline uint AQSPrinter::colorMode() const
{
  AQ_CALL_RET_V(colorMode(), uint);
}
inline void AQSPrinter::setFullPage(bool arg0)
{
  AQ_CALL_VOID(setFullPage(arg0));
}
inline bool AQSPrinter::fullPage() const
{
  AQ_CALL_RET_V(fullPage(), bool);
}
inline QSize AQSPrinter::margins() const
{
  AQ_CALL_RET_V(margins(), QSize);
}
inline void AQSPrinter::setMargins(uint arg0,  uint arg1,  uint arg2,  uint arg3)
{
  AQ_CALL_VOID(setMargins(arg0, arg1, arg2, arg3));
}
inline int AQSPrinter::fromPage() const
{
  AQ_CALL_RET_V(fromPage(), int);
}
inline int AQSPrinter::toPage() const
{
  AQ_CALL_RET_V(toPage(), int);
}
inline void AQSPrinter::setFromTo(int arg0,  int arg1)
{
  AQ_CALL_VOID(setFromTo(arg0, arg1));
}
inline int AQSPrinter::minPage() const
{
  AQ_CALL_RET_V(minPage(), int);
}
inline int AQSPrinter::maxPage() const
{
  AQ_CALL_RET_V(maxPage(), int);
}
inline void AQSPrinter::setMinMax(int arg0,  int arg1)
{
  AQ_CALL_VOID(setMinMax(arg0, arg1));
}
inline int AQSPrinter::numCopies() const
{
  AQ_CALL_RET_V(numCopies(), int);
}
inline void AQSPrinter::setNumCopies(int arg0)
{
  AQ_CALL_VOID(setNumCopies(arg0));
}
inline uint AQSPrinter::printRange() const
{
  AQ_CALL_RET_V(printRange(), uint);
}
inline void AQSPrinter::setPrintRange(uint arg0)
{
  AQ_CALL_VOID(setPrintRange(static_cast<QPrinter::PrintRange>(arg0)));
}
inline bool AQSPrinter::newPage()
{
  AQ_CALL_RET_V(newPage(), bool);
}
inline bool AQSPrinter::abort()
{
  AQ_CALL_RET_V(abort(), bool);
}
inline bool AQSPrinter::aborted() const
{
  AQ_CALL_RET_V(aborted(), bool);
}
inline bool AQSPrinter::setup(QWidget *arg0)
{
  AQ_CALL_RET_V(setup(arg0), bool);
}
inline uint AQSPrinter::paperSource() const
{
  AQ_CALL_RET_V(paperSource(), uint);
}
inline void AQSPrinter::setPaperSource(uint arg0)
{
  AQ_CALL_VOID(setPaperSource(static_cast<QPrinter::PaperSource>(arg0)));
}
inline void AQSPrinter::setOptionEnabled(uint arg0,  bool arg1)
{
  AQ_CALL_VOID(setOptionEnabled(static_cast<QPrinter::PrinterOption>(arg0), arg1));
}
inline bool AQSPrinter::isOptionEnabled(uint arg0)
{
  AQ_CALL_RET_V(isOptionEnabled(static_cast<QPrinter::PrinterOption>(arg0)), bool);
}
//@AQ_END_IMP_PUB_SLOTS@

#endif /* AQSPRINTER_P_H_ */
// @AQSVOIDPTR@
