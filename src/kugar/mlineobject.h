/***************************************************************************
             mlineobject.h  -  Kugar report line object
             -------------------
   begin     : Wed Feb 23 2000
   copyright : (C) 2000 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#ifndef MLINEOBJECT_H
#define MLINEOBJECT_H

#include <qobject.h>
#include <qpainter.h>
#include <qpaintdevice.h>
#include <qcolor.h>
#include <FLStylePainter.h>

/**
Kugar report line object

@author Mutiny Bay Software
*/

class MLineObject: public QObject
{

public:

  /**
  Line style constants
  */
  enum Style {
    NoPen = 0,
    SolidLine,
    DashLine,
    DotLine,
    DashDotLine,
    DashDotDotLine
  };

  /**
  Constructor
  */
  MLineObject();

  /**
  Copy constructor
  */
  MLineObject(const MLineObject &mLineObject);

  /**
  Assignment operator
  */
  MLineObject operator=(const MLineObject &mLineObject);

  /**
  Destructor
  */
  virtual ~ MLineObject();

protected:

  /**
  Object's x start position
  */
  int xpos1;

  /**
  Object's y start postition
  */
  int ypos1;

  /**
  Object's x end position
  */
  int xpos2;

  /**
  Object's y end position
  */
  int ypos2;

  /**
  Object's width
  */
  int penWidth;

  /**
  Object's color
  */
  QColor penColor;

  /**
  Object's style
  */
  int penStyle;

  /**
  Object's id
  */
  int objectId;

public:

  /**
  Gets the object's id
  */
  int getObjectId() const;

  /**
  Sets the start and end points for the line
  */
  void setLine(int xStart, int yStart, int xEnd, int yEnd);

  /**
  Sets the object's width
  */
  void setWidth(int width);

  /**
  Sets the object's style
  */
  void setStyle(int style);

  /**
  Sets the object's color
  */
  void setColor(int r, int g, int b);

  /**
  Sets the object's id
  */
  void setObjectId(int id);

  /**
  Draws the object to the specified painter & x/y offsets
  */
  virtual void draw(FLStylePainter *p);

protected:

  /**
  Draws the object to the specified painter & x/y offsets
  */
  void drawBase(FLStylePainter *p);

private:

  /**
  Copies member data from one object to another.

  Used by the copy constructor and assignment operator
  */
  void copy(const MLineObject *mLineObject);

  friend class MReportSection;
};

/** Gets the object's id */
inline int MLineObject::getObjectId() const
{
  return objectId;
}

/** Sets the object's color */
inline void MLineObject::setColor(int r, int g, int b)
{
  penColor.setRgb(r, g, b);
}

/** Sets the object's style */
inline void MLineObject::setStyle(int style)
{
  penStyle = style;
}

/** Sets the object's width */
inline void MLineObject::setWidth(int width)
{
  penWidth = width;
}

/** Sets the object's id */
inline void MLineObject::setObjectId(int id)
{
  objectId = id;
}
#endif
