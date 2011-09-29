/***************************************************************************
 mlabelobject.cpp  -  Kugar report label object
 -------------------
 begin     : Wed Aug 11 1999
 copyright : (C) 1999 by Mutiny Bay Software
 email     : info@mutinybaysoftware.com
 copyright : (C) 2002-2003 InfoSiAL S.L.
 email     : mail@infosial.com
 ***************************************************************************/

#include <qapplication.h>
#include <qpaintdevicemetrics.h>

#include "FLObjectFactory.h"

#include "mlabelobject.h"

/** Constructor */
MLabelObject::MLabelObject() :
  MReportObject()
{
  // Set the default label text
  text = "";

  // Set the default font
  fontFamily = "times";
  fontSize = 10;
  fontWeight = MLabelObject::Normal;
  fontItalic = false;
  adjustFontSize = false;

  // Set the default alignment
  hAlignment = MLabelObject::Left;
  vAlignment = MLabelObject::Top;
  wordWrap = false;
  labelFunction = QString::null;
  changeHeight = false;
  paintFunction = QString::null;
  pixmap = 0;
}

/** Copy constructor */
MLabelObject::MLabelObject(const MLabelObject &mLabelObject) :
  MReportObject((MReportObject &) mLabelObject)
{
  copy(&mLabelObject);
}

/** Assignment operator */
MLabelObject MLabelObject::operator =(const MLabelObject &mLabelObject)
{
  if (&mLabelObject == this)
    return *this;

  // Copy the derived class's data
  copy(&mLabelObject);

  // Copy the base class's data
  ((MReportObject &) *this) = mLabelObject;

  return *this;
}

/** Destructor */
MLabelObject::~MLabelObject()
{
  if (pixmap)
    delete pixmap;
}

/** Sets the label's text string */
void MLabelObject::setText(const QString txt)
{
  if (labelFunction.isEmpty()) {
    if (aqApp->multiLangEnabled() && !txt.isEmpty()) {
      text = trUtf8(txt.utf8());
      if (text == txt)
        text = tr(txt);
    } else
      text = txt;
  } else {
    FLDomNodeInterface *dni = 0;
    QSArgumentList l;
    l << QVariant(txt);
    if (!domNodeData.isNull()) {
      dni = new FLDomNodeInterface(domNodeData);
      l << dni;
    }

    QVariant v(aqApp->call(labelFunction, l, 0).variant());
    if (v.isValid()) {
      QString txtFun = v.toString();

      if (aqApp->multiLangEnabled() && !txtFun.isEmpty()) {
        text = trUtf8(txtFun.utf8());
        if (text == txtFun)
          text = tr(txtFun);
      } else
        text = txtFun;
    }

    if (dni)
      delete dni;
  }
}

/** Gets the label's text string */
QString MLabelObject::getText()
{
  return text;
}

/** Sets the label's pixmap */
void MLabelObject::setPixmap(const QPixmap &pix)
{
  if (paintFunction.isEmpty()) {
    if (pixmap) {
      delete pixmap;
      pixmap = 0;
    }
    if (!pix.isNull())
      pixmap = new QPixmap(pix);
  } else {
    if (pixmap) {
      delete pixmap;
      pixmap = 0;
    }
  }
}

/** Sets the label's text font */
void MLabelObject::setFont(const QString family, float size, int weight, bool italic)
{
  fontFamily = family;
  fontSize = size;
  fontWeight = weight;
  fontItalic = italic;
}

/** Draws the label */
int MLabelObject::draw(FLStylePainter *p)
{
  // Draw the pixmap
  if (!paintFunction.isEmpty()) {
    FLDomNodeInterface *dni = 0;
    QSArgumentList l;
    l << QVariant(text);
    if (!domNodeData.isNull()) {
      dni = new FLDomNodeInterface(domNodeData);
      l << dni;
    }

    QSArgument v = aqApp->call(paintFunction, l, 0);
    QSArgument::Type tp = v.type();
    if (tp != QSArgument::Invalid) {
      QPixmap pix;
      if (tp == QSArgument::VoidPointer) {
        QPixmap *vPix = static_cast<QPixmap *>(v.ptr());
        if (vPix)
          pix = *vPix;
      } else if (tp == QSArgument::Variant)
        pix = v.variant().toPixmap();

      if (!pix.isNull() && drawPixmap(p, &pix))
        return (changeHeight ? height : 0);
    }
  }

  if (pixmap && pixmap->isNull()) {
    delete pixmap;
    pixmap = 0;
  } else if (pixmap && drawPixmap(p, pixmap))
    return (changeHeight ? height : 0);

  if (text.isEmpty()) {
    drawBase(p);
    return 0;
  }

#if defined(Q_OS_MACX)
  FLStylePainter *pt = new FLStylePainter;
  int retVal = 0;
  uint originalHeight = height;
  QFont fnt;
  int tf;

  // Horizontal
  switch (hAlignment) {
    case MLabelObject::Left:
      tf = QPainter::AlignLeft;
      break;
    case MLabelObject::Center:
      tf = QPainter::AlignHCenter;
      break;
    case MLabelObject::Right:
      tf = QPainter::AlignRight;
  }

  // Vertical
  switch (vAlignment) {
    case MLabelObject::Top:
      tf = tf | QPainter::AlignTop;
      break;
    case MLabelObject::Bottom:
      tf = tf | QPainter::AlignBottom;
      break;
    case MLabelObject::Middle:
      tf = tf | QPainter::AlignVCenter;
  }

  // Word wrap
  if (wordWrap)
    tf = tf | QPainter::WordBreak;

  int nw = width * 4;
  int nh = height * 4;
  QPixmap pm(nw, nh);
  pm.fill(backgroundColor);
  pt->painter()->begin(&pm);

  fnt.setFamily(fontFamily);
  fnt.setPointSizeFloat(fontSize * 4);
  fnt.setWeight(fontWeight);
  fnt.setItalic(fontItalic);
  pt->painter()->setFont(fnt);

  if (changeHeight) {
    QRect maxRect(p->painter()->boundingRect(0, 0, nw, nh, tf, text));
    if (maxRect.height() > height) {
      height = maxRect.height();
      retVal = height;
    }
  }

  drawBase(pt);

  pt->painter()->setPen(foregroundColor);

  if (!transparent) {
    pt->painter()->setBackgroundColor(backgroundColor);
    pt->painter()->setBackgroundMode(Qt::OpaqueMode);
  }

  if (adjustFontSize && !wordWrap && !changeHeight) {
    float factor = (float)nw / (float)p->painter()->fontMetrics().width(text);
    if (factor < 1.0) {
      QFont f = p->painter()->font();
      f.setPointSizeFloat(f.pointSizeFloat() * factor);
      p->painter()->setFont(f);
    }
  }

  pt->painter()->drawText(0, 0, nw, nh, tf, text);

  pt->painter()->end();
  delete pt;

  drawPixmap(p, &pm);

  height = originalHeight;
  return retVal;
#else
  int retVal = 0;
  uint originalHeight = height;
  QFont fnt;
  int tf;
  Qt::BGMode oldBgMode;
  QColor oldBgColor;
  bool restoreBg = false;

  // Horizontal
  switch (hAlignment) {
    case MLabelObject::Left:
      tf = QPainter::AlignLeft;
      break;
    case MLabelObject::Center:
      tf = QPainter::AlignHCenter;
      break;
    case MLabelObject::Right:
      tf = QPainter::AlignRight;
  }

  // Vertical
  switch (vAlignment) {
    case MLabelObject::Top:
      tf = tf | QPainter::AlignTop;
      break;
    case MLabelObject::Bottom:
      tf = tf | QPainter::AlignBottom;
      break;
    case MLabelObject::Middle:
      tf = tf | QPainter::AlignVCenter;
  }

  // Word wrap
  if (wordWrap)
    tf = tf | QPainter::WordBreak;

  fnt.setFamily(fontFamily);
  fnt.setPointSizeFloat(fontSize);
  fnt.setWeight(fontWeight);
  fnt.setItalic(fontItalic);
  p->painter()->setFont(fnt);

  if (changeHeight) {
    QRect maxRect(p->painter()->boundingRect(0, 0, width, height, tf, text));
    if (maxRect.height() > height) {
      height = maxRect.height();
      retVal = height;
    }
  }

  drawBase(p);

  p->painter()->setPen(foregroundColor);

  if (!transparent) {
    restoreBg = true;
    oldBgMode = p->painter()->backgroundMode();
    oldBgColor = p->painter()->backgroundColor();
    p->painter()->setBackgroundColor(backgroundColor);
    p->painter()->setBackgroundMode(Qt::OpaqueMode);
  }

  if (!p->drawText(text, tf, this)) {
    bool restore = false;
    if (p->errCode() == FLStylePainter::IdNotFound) {
      p->painter()->save(QObject::name());
      p->applyTransforms();
      p->painter()->translate(xpos, ypos);
      restore = true;
    }

    if (adjustFontSize && !wordWrap && !changeHeight) {
      float factor = (float)width / (float)p->painter()->fontMetrics().width(text);
      if (factor < 1.0) {
        QFont f = p->painter()->font();
        f.setPointSizeFloat(f.pointSizeFloat() * factor);
        p->painter()->setFont(f);
      }
    }

    p->painter()->drawText(0, 0, width, height, tf, text);

    if (restore)
      p->painter()->restore();
  }

  if (restoreBg) {
    p->painter()->setBackgroundMode(oldBgMode);
    p->painter()->setBackgroundColor(oldBgColor);
  }

  height = originalHeight;
  return retVal;
#endif
}

/** Calculates the height of the drawed object */
int MLabelObject::calcHeight(FLStylePainter *p) const
{
  if (!changeHeight)
    return 0;

  if (pixmap && !pixmap->isNull())
    return height;

  // Font
  QFont fnt;
  int tf;
  fnt.setFamily(fontFamily);
  fnt.setPointSizeFloat(fontSize);
  fnt.setWeight(fontWeight);
  fnt.setItalic(fontItalic);
  p->painter()->setFont(fnt);

  // Horizontal
  switch (hAlignment) {
    case MLabelObject::Left:
      tf = QPainter::AlignLeft;
      break;
    case MLabelObject::Center:
      tf = QPainter::AlignHCenter;
      break;
    case MLabelObject::Right:
      tf = QPainter::AlignRight;
  }

  // Vertical
  switch (vAlignment) {
    case MLabelObject::Top:
      tf = tf | QPainter::AlignTop;
      break;
    case MLabelObject::Bottom:
      tf = tf | QPainter::AlignBottom;
      break;
    case MLabelObject::Middle:
      tf = tf | QPainter::AlignVCenter;
  }

  // Word wrap
  if (wordWrap)
    tf = tf | QPainter::WordBreak;

  QRect maxRect = p->painter()->boundingRect(0, 0, width, height, tf, text);
  return (maxRect.height() > height ? maxRect.height() : height);
}

bool MLabelObject::drawPixmap(FLStylePainter *p, QPixmap *pixmap)
{

  if (!p->drawPixmap(*pixmap, 0, 0, -1, -1, this)) {
    if (width > 0 && height > 0) {
      p->painter()->save(QObject::name());
      p->painter()->scale((double) width / (double) pixmap->width(), (double) height / (double) pixmap->height());
    } else {
      qWarning("MLabelObject::drawPixmap : width and/or height are not valid");
      return false;
    }

    p->painter()->drawPixmap(0, 0, *pixmap);
    p->painter()->restore();
  }

  return true;
}

/** Copies member data from one object to another.
 Used by the copy constructor and assignment operator */
void MLabelObject::copy(const MLabelObject *mLabelObject)
{
  // Copy the label's text
  text = mLabelObject->text;

  // Copy the label's font data
  fontFamily = mLabelObject->fontFamily;
  fontSize = mLabelObject->fontSize;
  fontWeight = mLabelObject->fontWeight;
  fontItalic = mLabelObject->fontItalic;

  // Copy the label's alignment data
  vAlignment = mLabelObject->vAlignment;
  hAlignment = mLabelObject->hAlignment;
  wordWrap = mLabelObject->wordWrap;
  labelFunction = mLabelObject->labelFunction;
  changeHeight = mLabelObject->changeHeight;
  paintFunction = mLabelObject->paintFunction;

  if (mLabelObject->pixmap && !mLabelObject->pixmap->isNull()) {
    if (!pixmap)
      pixmap = new QPixmap();
    *pixmap = *(mLabelObject->pixmap);
  }

  domNodeData = mLabelObject->domNodeData;
}
