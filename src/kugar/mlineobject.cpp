/***************************************************************************
           mlineobject.cpp  -  Kugar report line object
           -------------------
 begin     : Wed Feb 23 2000
 copyright : (C) 2000 by Mutiny Bay Software
 email     : info@mutinybaysoftware.com
***************************************************************************/

#include "mlineobject.h"

/** Constructor */
MLineObject::MLineObject() : QObject()
{
  // Set the object's default geometry
  xpos1 = 0;
  ypos1 = 0;
  xpos2 = 0;
  ypos2 = 0;

  // Set the object's default attributes
  penWidth = 1;
  penColor.setRgb(0, 0, 0);
  penStyle = MLineObject::SolidLine;

  objectId = 0;
}

/** Copy constructor */
MLineObject::MLineObject(const MLineObject &mLineObject)
{
  copy(&mLineObject);
}

/** Assignment operator */
MLineObject MLineObject::operator=(const MLineObject &mLineObject)
{
  if (&mLineObject == this)
    return * this;

  // Copy the derived class's data
  copy(&mLineObject);

  return *this;
}

/** Destructor */
MLineObject::~MLineObject()
{
}

/** Sets the start and end points for the line */
void
MLineObject::setLine(int xStart, int yStart, int xEnd, int yEnd)
{
  xpos1 = xStart;
  ypos1 = yStart;
  xpos2 = xEnd;
  ypos2 = yEnd;
}

/** Draws the object to the specified painter & x/y offsets */
void MLineObject::draw(FLStylePainter *p)
{
  drawBase(p);
}

/** Draws the base object to the specified painter & x/y offsets */
void MLineObject::drawBase(FLStylePainter *p)
{
  if (p->drawLine(this))
    return;

  bool restore = false;
  if (p->errCode() == FLStylePainter::IdNotFound) {
    p->painter()->save(QObject::name());
    p->applyTransforms();
    p->painter()->translate(xpos1, ypos1);
    restore = true;
  }

  QPen linePen(penColor, penWidth, (QPen::PenStyle) penStyle);
  p->painter()->setPen(linePen);
  p->painter()->drawLine(0, 0, xpos2 - xpos1, ypos2 - ypos1);

  if (restore)
    p->painter()->restore();
}

/** Copies member data from one object to another.
    Used by the copy constructor and assignment operator */
void MLineObject::copy(const MLineObject *mLineObject)
{
  // Copy the object's geometry
  xpos1 = mLineObject->xpos1;
  ypos1 = mLineObject->ypos1;
  xpos2 = mLineObject->xpos2;
  ypos2 = mLineObject->ypos2;

  // copy the object's attributes
  penWidth = mLineObject->penWidth;
  penColor = mLineObject->penColor;
  penStyle = mLineObject->penStyle;
}
