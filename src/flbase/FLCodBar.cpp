/***************************************************************************
                            FLCodBar.cpp
                         -------------------
begin                : Mar Jun 17 2003
copyright            : (C) 2003-2005 by InfoSiAL S.L.
email                : mail@infosial.com
***************************************************************************/
/***************************************************************************
                                barcode.cpp
                             -------------------
    begin                : Die Apr 23 2002
    copyright            : (C) 2002,2003 by Dominik Seichter
    email                : domseichter@web.de
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

#include "FLCodBar.h"
#include "FLApplication.h"
#include "barcode.h"

FLCodBar::FLCodBar(barcodeData *data)
{
  copyBarcode(data, &barcode);
}

FLCodBar::FLCodBar(const QString &value, int type, int margin,
                   double scale, double cut, int rotation,
                   bool text_flag, QColor fg, QColor bg, int res) :
  proc(0), readingStdout(false), writingStdout(false)
{
  barcode.value = value;
  barcode.type = type;
  barcode.margin = margin;
  barcode.scale = scale;
  barcode.cut = cut;
  barcode.rotation = rotation;
  barcode.text = text_flag;
  barcode.fg = fg;
  barcode.bg = bg;
  barcode.valid = false;
  barcode.res = res;
}

FLCodBar::FLCodBar() : proc(0), readingStdout(false), writingStdout(false)
{
  fillDefault(&barcode);
}

FLCodBar::~FLCodBar()
{
  if (proc)
    delete proc;
}

QPixmap FLCodBar::pixmap()
{
  if (p.isNull()) {
    QString key(barcode.value + QString::number(barcode.type) +
                QString::number(barcode.res));
    if (!QPixmapCache::find(key, p)) {
      createBarcode();
      if (barcode.valid) {
        key = barcode.value + QString::number(barcode.type) +
              QString::number(barcode.res);
        QPixmapCache::insert(key, p);
      }
    } else
      barcode.valid = true;
  }
  return p;
}

QPixmap FLCodBar::pixmapError()
{
  return pError;
}

void FLCodBar::createBarcode()
{
  if (barcode.value.isEmpty()) {
    barcode.valid = true;
    return ;
  }

  QString psBarcode;

  barcode.valid = false;
  cleanUp();

  struct Barcode_Item *bc;

  bc = Barcode_Create((char *) barcode.value.ascii());
  if (!bc)
    return ;

  bc->scalef = barcode.scale;

  if (Barcode_Encode(bc, barcode.type) == -1) {
    pError.resize(350, 50);
    pError.fill(barcode.bg);
    QPainter pter(&pError);
    pter.setFont(QFont("Helvetica", 11));
    pter.setBrush(barcode.fg);
    pter.drawText(
      15, 30,
      tr("El valor no se puede codificar para %1").arg(typeToName(barcode.type))
    );
    return ;
  }

  if (Barcode_Print(bc, &psBarcode, BARCODE_OUT_PS)) {
    pError.resize(300, 50);
    pError.fill(barcode.bg);
    QPainter pter(&pError);
    pter.setFont(QFont("Helvetica", 11));
    pter.setBrush(barcode.fg);
    pter.drawText(15, 30, tr("Error en la generación del código de barras"));
    return ;
  }

  proc = new QProcess();
  proc->addArgument(aqApp->gsExecutable());
  proc->addArgument("-q");
  proc->addArgument("-dBATCH");
  proc->addArgument("-dNOPAUSE");
  proc->addArgument("-sDEVICE=pnggray");
  proc->addArgument(
    QString("-g%1x%2")
    .arg(bc->xoff + (bc->width * barcode.res / 72) +
         (2 * barcode.margin * barcode.res / 72))
    .arg(bc->yoff + (bc->height * barcode.res / 72) +
         (2 * barcode.margin * barcode.res / 72))
  );
  QString res = QString::number(barcode.res);
  proc->addArgument("-r" + res + "x" + res);
  proc->addArgument("-sOutputFile=-");
  proc->addArgument("-");

  Barcode_Delete(bc);

  writingStdout = true;
  if (!proc->launch(psBarcode)) {
    pError.resize(300, 50);
    pError.fill(barcode.bg);
    QPainter pter(&pError);
    pter.setFont(QFont("Helvetica", 11));
    pter.setBrush(barcode.fg);
    pter.drawText(27, 20, tr("Necesita tener instalado GhostScript"));
    pter.drawText(27, 39, tr("para poder generar códigos de barras"));
    writingStdout = false;
    return ;
  } else {
    readingStdout = true;
    while (readingStdout) {
      qApp->processEvents();
      if (!proc->isRunning())
        writingStdoutFinished();
    }
  }
}

void FLCodBar::readPixmapStdout()
{
  if (writingStdout) {
    qApp->processEvents();
    return ;
  }

  p.loadFromData(proc->readStdout(), "PNG");

  if (!p.isNull()) {
    QWMatrix m;
    m.rotate((double) barcode.rotation);
    p.xForm(m);
    barcode.valid = true;
  }

  readingStdout = false;
}

void FLCodBar::writingStdoutFinished()
{
  writingStdout = false;
  readPixmapStdout();
}

void FLCodBar::fillDefault(barcodeData *data)
{
  data->bg = Qt::white;
  data->fg = Qt::black;
  data->margin = 10;
  data->text = true;
  data->value = "1234567890";
  data->type = BARCODE_39;
  data->scale = 1.0;
  data->cut = 1.0;
  data->rotation = 0;
  data->caption = "Static";
  data->valid = false;
  data->res = 72;
}

void FLCodBar::copyBarcode(barcodeData *src, barcodeData *dest)
{
  dest->value = src->value;
  dest->type = src->type;
  dest->margin = src->margin;
  dest->scale = src->scale;
  dest->cut = src->cut;
  dest->rotation = src->rotation;
  dest->text = src->text;
  dest->caption = src->caption;
  dest->valid = src->valid;
  dest->fg = src->fg;
  dest->bg = src->bg;
  dest->x = src->x;
  dest->y = src->y;
  dest->res = src->res;
}

void FLCodBar::cleanUp()
{
  p.resize(0, 0);
  pError.resize(0, 0);
}

int FLCodBar::nameToType(const QString &name)
{
  QString n = name.lower();
  if (n == "any")
    return BARCODE_ANY;
  else if (n == "ean")
    return BARCODE_EAN;
  else if (n == "upc")
    return BARCODE_UPC;
  else if (n == "isbn")
    return BARCODE_ISBN;
  else if (n == "code39")
    return BARCODE_39;
  else if (n == "code128")
    return BARCODE_128;
  else if (n == "code128c")
    return BARCODE_128C;
  else if (n == "code128b")
    return BARCODE_128B;
  else if (n == "codei25")
    return BARCODE_I25;
  else if (n == "code128r")
    return BARCODE_128RAW;
  else if (n == "cbr")
    return BARCODE_CBR;
  else if (n == "msi")
    return BARCODE_MSI;
  else if (n == "pls")
    return BARCODE_PLS;
  else if (n == "code93")
    return BARCODE_93;
  else
    return BARCODE_ANY;
}

QString FLCodBar::typeToName(const int type)
{
  switch (type) {
    case BARCODE_ANY:
      return "ANY";
    case BARCODE_EAN:
      return "EAN";
    case BARCODE_UPC:
      return "UPC";
    case BARCODE_ISBN:
      return "ISBN";
    case BARCODE_39:
      return "Code39";
    case BARCODE_128:
      return "Code128";
    case BARCODE_128C:
      return "Code128C";
    case BARCODE_128B:
      return "Code128B";
    case BARCODE_I25:
      return "CodeI25";
    case BARCODE_128RAW:
      return "Code128RAW";
    case BARCODE_CBR:
      return "CBR";
    case BARCODE_MSI:
      return "MSI";
    case BARCODE_PLS:
      return "PLS";
    case BARCODE_93:
      return "Code93";
    default:
      return "ANY";
  }
}
