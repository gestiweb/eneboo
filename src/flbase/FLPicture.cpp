/***************************************************************************
 FLPicture.cpp
 -------------------
 begin                : 30/01/2008
 copyright            : (C) 2003-2008 by InfoSiAL S.L.
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

#include "FLPicture.h"

#define PIC_NEW_D     if ( !d ) d = new FLPicturePrivate();

#define PIC_CHK_D(R)  if ( !d || ( d && !d->pte->isActive() ) ) { \
    qWarning( tr( "Picture no está activo, para activarlo llama a la función begin()" ) ); \
    return R; \
  }

class FLPicturePrivate
{

public:

  FLPicturePrivate();
  ~FLPicturePrivate();

  bool begin();
  bool end();
  void setPainter(QPainter * pt);

  QPicture * pic;
  QPainter * pte;

  bool ownerPic;
  bool ownerPte;
  bool endPte;
};

FLPicturePrivate::FLPicturePrivate()
{
  qWarning("FLPicturePrivate::FLPicturePrivate()");
  pic = new QPicture();
  pte = new QPainter();
  ownerPic = true;
  ownerPte = true;
  endPte = true;
}

FLPicturePrivate::~FLPicturePrivate()
{
  qWarning("FLPicturePrivate::~FLPicturePrivate()");
  if (pte) {
    end();
    if (ownerPte) {
      qWarning("FLPicturePrivate:: delete pte");
      delete pte;
    }
  }
  if (pic) {
    end();
    if (ownerPic) {
      qWarning("FLPicturePrivate:: delete pic");
      delete pic;
    }
  }
}

bool FLPicturePrivate::begin()
{
  if (!pte->isActive())
    return pte->begin(pic);
  else 
    qWarning("WARN: FLPicturePrivate::begin() -> pte->isActive() !!");
  
  return false;
}

bool FLPicturePrivate::end()
{
  if (!pte->isActive()) return false;
  if (ownerPte)
    return pte->end();
  else if (!ownerPte && endPte)
    return pte->end();
  else
    qWarning("WARN: FLPicturePrivate::end() -> not owner and endPte = False  error!!");
    
  return false;
}

void FLPicturePrivate::setPainter(QPainter * pt)
{
  qWarning("FLPicturePrivate::setPainter(pt)");
  if (pic && pt) {
    if (pte) {
      end();
      if (ownerPte)
        delete pte;
    }
    pte = pt;
    ownerPte = false;
    endPte = !pte->isActive();
  }
}

FLPicture::FLPicture(QObject * parent, const char * name) :
  QObject(parent, name), d(0)
{
  qWarning("FLPicture::FLPicture(QObject * parent, const char * name)");
  PIC_NEW_D
}

FLPicture::FLPicture(FLPicture * other) :
  QObject(0), d(0)
{
  qWarning("FLPicture::FLPicture(FLPicture * other)");
  if (other && other != this && other->d && other->d->pic) {
    d = new FLPicturePrivate();
    qWarning("WARN: FLPicture::FLPicture(FLPicture * other) -> replacing picture !!)");
    
    *(d->pic) = *(other->d->pic);
  }
}

FLPicture::FLPicture(const QPicture & pic) :
  QObject(0), d(0)
{
  qWarning("FLPicture::FLPicture(const QPicture & pic)");
  setPicture(pic);
}

FLPicture::FLPicture(QPicture * pic, QObject * parent, const char * name) :
  QObject(parent, name), d(0)
{
  qWarning("FLPicture::FLPicture(QPicture * pic, QObject * parent, const char * name)");
  setPicture(pic);
}

FLPicture::FLPicture(QPicture * pic, QPainter * pte, QObject * parent, const char * name) :
  QObject(parent, name), d(0)
{
  qWarning("FLPicture::FLPicture(QPicture * pic, QPainter * pte, QObject * parent, const char * name)");
  setPicture(pic);
  d->setPainter(pte);
}

FLPicture::~FLPicture()
{
  qWarning("FLPicture::~FLPicture()");

  cleanup();
}

QPicture * FLPicture::picture() const
{
  if (!d) {
    qWarning("WARN: QPicture * FLPicture::picture() const - no picture found, returning NULL");
    return 0;
  }
  return d->pic;
}

void FLPicture::setPicture(const QPicture & pic)
{
  qWarning("WARN: FLPicture::setPicture(const QPicture & pic) - replacing picture");
  cleanup();
  PIC_NEW_D
  *(d->pic) = pic;
}

void FLPicture::setPicture(QPicture * pic)
{
  if (pic) {
    qWarning("WARN: FLPicture::setPicture(QPicture * pic) - replacing picture - new picture not owned!");
    cleanup();
    PIC_NEW_D
    delete d->pic;
    d->pic = pic;
    d->ownerPic = false;
  } else {
    qWarning("WARN: FLPicture::setPicture(QPicture * pic) - pic == NULL - nothing changed.");
  }
}

bool FLPicture::isNull() const
{
  return (d && d->pic->isNull());
}

bool FLPicture::load(const QString & fileName, const char * format)
{
  PIC_NEW_D
  d->pic->load(fileName, format);
}

bool FLPicture::save(const QString & fileName, const char * format)
{
  if (!d)
    return false;
  return d->pic->save(fileName, format);
}

QRect FLPicture::boundingRect() const
{
  PIC_CHK_D( QRect() )
  return d->pic->boundingRect();
}

void FLPicture::setBoundingRect(const QRect & r)
{
  PIC_NEW_D
  d->pic->setBoundingRect(r);
}

bool FLPicture::begin()
{
  qWarning("FLPicture::begin()");
  PIC_NEW_D
  return d->begin();
}

bool FLPicture::end() const
{
  qWarning("FLPicture::end()");
  PIC_CHK_D( false )
  return d->end();
}

void FLPicture::cleanup()
{
  qWarning("FLPicture::cleanup()");
  if (d)
    delete d;
  d = 0;
}

bool FLPicture::isActive() const
{
  PIC_CHK_D( false )
  return d->pte->isActive();
}

void FLPicture::flush() const
{
  PIC_CHK_D()
  d->pte->flush();
}

void FLPicture::savePainter() const
{
  PIC_CHK_D()
  d->pte->save();
}

void FLPicture::restorePainter() const
{
  PIC_CHK_D()
  d->pte->restore();
}

const QFont & FLPicture::font() const
{
  PIC_CHK_D( QFont() )
  return d->pte->font();
}

void FLPicture::setFont(const QFont & font) const
{
  PIC_CHK_D()
  d->pte->setFont(font);
}

void FLPicture::setPen(const QColor & color, uint width, FLPenStyle style) const
{
  PIC_CHK_D()
  d->pte->setPen(QPen(color, width, (Qt::PenStyle) style));
}

void FLPicture::setBrush(const QColor & color, FLBrushStyle style) const
{
  PIC_CHK_D()
  d->pte->setBrush(QBrush(color, (Qt::BrushStyle) style));
}

const QColor & FLPicture::backgroundColor() const
{
  PIC_CHK_D( QColor() )
  return d->pte->backgroundColor();
}

void FLPicture::setBackgroundColor(const QColor & color) const
{
  PIC_CHK_D()
  d->pte->setBackgroundColor(color);
}

FLPicture::FLBGMode FLPicture::backgroundMode() const
{
  PIC_CHK_D( FLPicture::OpaqueMode )
  return (FLBGMode) d->pte->backgroundMode();
}

void FLPicture::setBackgroundMode(FLBGMode bgm) const
{
  PIC_CHK_D()
  d->pte->setBackgroundMode((Qt::BGMode) bgm);
}

FLPicture::FLRasterOp FLPicture::rasterOp() const
{
  PIC_CHK_D( FLPicture::NotROP )
  return (FLRasterOp) d->pte->rasterOp();
}

void FLPicture::setRasterOp(FLRasterOp rop) const
{
  PIC_CHK_D()
  d->pte->setRasterOp((Qt::RasterOp) rop);
}

const QPoint & FLPicture::brushOrigin() const
{
  PIC_CHK_D( QPoint() )
  return d->pte->brushOrigin();
}

void FLPicture::setBrushOrigin(int x, int y) const
{
  PIC_CHK_D()
  d->pte->setBrushOrigin(x, y);
}

void FLPicture::setBrushOrigin(const QPoint & pt) const
{
  PIC_CHK_D()
  d->pte->setBrushOrigin(pt);
}

QRect FLPicture::window() const
{
  PIC_CHK_D( QRect() )
  return d->pte->window();
}

void FLPicture::setWindow(const QRect & r) const
{
  PIC_CHK_D()
  d->pte->setWindow(r);
}

void FLPicture::setWindow(int x, int y, int w, int h) const
{
  PIC_CHK_D()
  d->pte->setWindow(x, y, w, h);
}

QRect FLPicture::viewport() const
{
  PIC_CHK_D( QRect() )
  return d->pte->viewport();
}

void FLPicture::setViewport(const QRect & r) const
{
  PIC_CHK_D()
  d->pte->setViewport(r);
}

void FLPicture::setViewport(int x, int y, int w, int h) const
{
  PIC_CHK_D()
  d->pte->setViewport(x, y, w, h);
}

void FLPicture::scale(double sx, double sy) const
{
  PIC_CHK_D()
  d->pte->scale(sx, sy);
}

void FLPicture::shear(double sh, double sv) const
{
  PIC_CHK_D()
  d->pte->shear(sh, sv);
}

void FLPicture::rotate(double a) const
{
  PIC_CHK_D()
  d->pte->rotate(a);
}

void FLPicture::translate(double dx, double dy) const
{
  PIC_CHK_D()
  d->pte->translate(dx, dy);
}

double FLPicture::translationX() const
{
  PIC_CHK_D( 0 )
  return d->pte->translationX();
}

double FLPicture::translationY() const
{
  PIC_CHK_D( 0 )
  return d->pte->translationY();
}

void FLPicture::setClipping(bool c) const
{
  PIC_CHK_D()
  d->pte->setClipping(c);
}

bool FLPicture::hasClipping() const
{
  PIC_CHK_D( false )
  return d->pte->hasClipping();
}

void FLPicture::setClipRect(const QRect & r, FLCoordinateMode co) const
{
  PIC_CHK_D()
  d->pte->setClipRect(r, (QPainter::CoordinateMode) co);
}

void FLPicture::setClipRect(int x, int y, int w, int h, FLCoordinateMode co) const
{
  PIC_CHK_D()
  d->pte->setClipRect(x, y, w, h, (QPainter::CoordinateMode) co);
}

void FLPicture::drawLine(const QPoint & pt1, const QPoint & pt2) const
{
  PIC_CHK_D()
  d->pte->drawLine(pt1, pt2);
}

void FLPicture::drawLine(int x1, int y1, int x2, int y2) const
{
  PIC_CHK_D()
  d->pte->drawLine(x1, y1, x2, y2);
}

void FLPicture::drawPoint(int x, int y) const
{
  PIC_CHK_D()
  d->pte->drawPoint(x, y);
}

void FLPicture::drawPoint(const QPoint & pt) const
{
  PIC_CHK_D()
  d->pte->drawPoint(pt);
}

void FLPicture::moveTo(int x, int y) const
{
  PIC_CHK_D()
  d->pte->moveTo(x, y);
}

void FLPicture::moveTo(const QPoint & pt) const
{
  PIC_CHK_D()
  d->pte->moveTo(pt);
}

void FLPicture::lineTo(int x, int y) const
{
  PIC_CHK_D()
  d->pte->lineTo(x, y);
}

void FLPicture::lineTo(const QPoint & pt) const
{
  PIC_CHK_D()
  d->pte->lineTo(pt);
}

void FLPicture::drawRect(int x, int y, int w, int h) const
{
  PIC_CHK_D()
  d->pte->drawRect(x, y, w, h);
}

void FLPicture::drawRect(const QRect & r) const
{
  PIC_CHK_D()
  d->pte->drawRect(r);
}

void FLPicture::drawWinFocusRect(int x, int y, int w, int h) const
{
  PIC_CHK_D()
  d->pte->drawWinFocusRect(x, y, w, h);
}

void FLPicture::drawWinFocusRect(int x, int y, int w, int h, const QColor & bgColor) const
{
  PIC_CHK_D()
  d->pte->drawWinFocusRect(x, y, w, h, bgColor);
}

void FLPicture::drawWinFocusRect(const QRect & r) const
{
  PIC_CHK_D()
  d->pte->drawWinFocusRect(r);
}

void FLPicture::drawWinFocusRect(const QRect & r, const QColor & bgColor) const
{
  PIC_CHK_D()
  d->pte->drawWinFocusRect(r, bgColor);
}

void FLPicture::drawRoundRect(int x, int y, int w, int h, int xRnd, int yRnd) const
{
  PIC_CHK_D()
  d->pte->drawRoundRect(x, y, w, h, xRnd, yRnd);
}

void FLPicture::drawRoundRect(const QRect & r, int xRnd, int yRnd) const
{
  PIC_CHK_D()
  d->pte->drawRoundRect(r, xRnd, yRnd);
}

void FLPicture::drawEllipse(int x, int y, int w, int h) const
{
  PIC_CHK_D()
  d->pte->drawEllipse(x, y, w, h);
}

void FLPicture::drawEllipse(const QRect & r) const
{
  PIC_CHK_D()
  d->pte->drawEllipse(r);
}

void FLPicture::drawArc(int x, int y, int w, int h, int a, int alen) const
{
  PIC_CHK_D()
  d->pte->drawArc(x, y, w, h, a, alen);
}

void FLPicture::drawArc(const QRect & r, int a, int alen) const
{
  PIC_CHK_D()
  d->pte->drawArc(r, a, alen);
}

void FLPicture::drawPie(int x, int y, int w, int h, int a, int alen) const
{
  PIC_CHK_D()
  d->pte->drawPie(x, y, w, h, a, alen);
}

void FLPicture::drawPie(const QRect & r, int a, int alen) const
{
  PIC_CHK_D()
  d->pte->drawPie(r, a, alen);
}

void FLPicture::drawChord(int x, int y, int w, int h, int a, int alen) const
{
  PIC_CHK_D()
  d->pte->drawChord(x, y, w, h, a, alen);
}

void FLPicture::drawChord(const QRect & r, int a, int alen) const
{
  PIC_CHK_D()
  d->pte->drawChord(r, a, alen);
}

void FLPicture::drawPixmap(int x, int y, const QPixmap & pix, int sx, int sy, int sw, int sh) const
{
  PIC_CHK_D()
  d->pte->drawPixmap(x, y, pix, sx, sy, sw, sh);
}

void FLPicture::drawPixmap(const QPoint & pt, const QPixmap & pix, const QRect & r) const
{
  PIC_CHK_D()
  d->pte->drawPixmap(pt, pix, r);
}

void FLPicture::drawPixmap(const QPoint & pt, const QPixmap & pix) const
{
  PIC_CHK_D()
  d->pte->drawPixmap(pt, pix);
}

void FLPicture::drawPixmap(const QRect & r, const QPixmap & pix) const
{
  PIC_CHK_D()
  d->pte->drawPixmap(r, pix);
}

void FLPicture::drawTiledPixmap(int x, int y, int w, int h, const QPixmap & pix, int sx, int sy) const
{
  PIC_CHK_D()
  d->pte->drawTiledPixmap(x, y, w, h, pix, sx, sy);
}

void FLPicture::drawTiledPixmap(const QRect & r, const QPixmap & pix, const QPoint & pt) const
{
  PIC_CHK_D()
  d->pte->drawTiledPixmap(r, pix, pt);
}

void FLPicture::drawTiledPixmap(const QRect & r, const QPixmap & pix) const
{
  PIC_CHK_D()
  d->pte->drawTiledPixmap(r, pix);
}

void FLPicture::drawPicture(FLPicture * pic)
{
  PIC_CHK_D()
  if (pic && pic->picture())
    d->pte->drawPicture(*(pic->picture()));
}

void FLPicture::drawPicture(int x, int y, FLPicture * pic) const
{
  PIC_CHK_D()
  if (pic && pic->picture())
    d->pte->drawPicture(x, y, *(pic->picture()));
}

void FLPicture::drawPicture(const QPoint & pt, FLPicture * pic) const
{
  PIC_CHK_D()
  if (pic && pic->picture())
    d->pte->drawPicture(pt, *(pic->picture()));
}

void FLPicture::fillRect(int x, int y, int w, int h, const QColor & color, FLBrushStyle style) const
{
  PIC_CHK_D()
  d->pte->fillRect(x, y, w, h, QBrush(color, (Qt::BrushStyle) style));
}

void FLPicture::fillRect(const QRect & r, const QColor & color, FLBrushStyle style) const
{
  PIC_CHK_D()
  d->pte->fillRect(r, QBrush(color, (Qt::BrushStyle) style));
}

void FLPicture::eraseRect(int x, int y, int w, int h) const
{
  PIC_CHK_D()
  d->pte->eraseRect(x, y, w, h);
}

void FLPicture::eraseRect(const QRect & r) const
{
  PIC_CHK_D()
  d->pte->eraseRect(r);
}

void FLPicture::drawText(int x, int y, const QString & t, int len, FLTextDirection dir) const
{
  PIC_CHK_D()
  d->pte->drawText(x, y, t, len, (QPainter::TextDirection) dir);
}

void FLPicture::drawText(const QPoint & pt, const QString & t, int len, FLTextDirection dir) const
{
  PIC_CHK_D()
  d->pte->drawText(pt, t, len, (QPainter::TextDirection) dir);
}

void FLPicture::drawText(int x, int y, const QString & t, int pos, int len, FLTextDirection dir) const
{
  PIC_CHK_D()
  d->pte->drawText(x, y, t, pos, len, (QPainter::TextDirection) dir);
}

void FLPicture::drawText(const QPoint & p, const QString & t, int pos, int len, FLTextDirection dir) const
{
  PIC_CHK_D()
  d->pte->drawText(p, t, pos, len, (QPainter::TextDirection) dir);
}

void FLPicture::drawText(int x, int y, int w, int h, int flags, const QString & t, int len, QRect * br) const
{
  PIC_CHK_D()
  d->pte->drawText(x, y, w, h, flags, t, len, br);
}

void FLPicture::drawText(const QRect & r, int flags, const QString & t, int len, QRect * br) const
{
  PIC_CHK_D()
  d->pte->drawText(r, flags, t, len, br);
}

QRect FLPicture::boundingRect(int x, int y, int w, int h, int flags, const QString & t, int len) const
{
  PIC_CHK_D( QRect() )
  return d->pte->boundingRect(x, y, w, h, flags, t, len);
}

QRect FLPicture::boundingRect(const QRect & r, int flags, const QString & t, int len) const
{
  PIC_CHK_D( QRect() )
  return d->pte->boundingRect(r, flags, t, len);
}

QPixmap * FLPicture::playOnPixmap(QPixmap * pix)
{
  PIC_CHK_D( 0 )
  if (!pix)
    return 0;
  end();
  //QPicture cpyPic;
  QPainter pa(pix);
  //cpyPic.setData(d->pic->data(), d->pic->size());
  pa.setClipRect(0, 0, pix->width(), pix->height());
  d->pic->play(&pa);
  //cpyPic.play(&pa);
  pa.end();
  begin();
  //d->pte->drawPicture(0, 0, cpyPic);
  return pix;
}

FLPicture * FLPicture::playOnPicture(FLPicture * pic)
{
  PIC_CHK_D( 0 )
  if (pic && pic->picture()) {
    end();
    QPicture cpyPic;
    cpyPic.setData(d->pic->data(), d->pic->size());
    QPainter pa(pic->picture());
    cpyPic.play(&pa);
    pa.end();
    begin();
    d->pte->drawPicture(0, 0, cpyPic);
    return pic;
  }
  return 0;
}

