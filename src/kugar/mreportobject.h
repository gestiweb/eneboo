/***************************************************************************
             mreportobject.h  -  Kugar report object baseclass
             -------------------
   begin     : Wed Aug 11 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#ifndef MREPORTOBJECT_H
#define MREPORTOBJECT_H

#include <qobject.h>
#include <qpainter.h>
#include <qpaintdevice.h>
#include <qcolor.h>
#include <qdom.h>
#include <qfile.h>
#include <FLStylePainter.h>

/**
Kugar report object baseclass

@author Mutiny Bay Software
*/
class MReportObject: public QObject
{

public:

  /**
  Border style constants
  */
  enum BorderStyle {
    NoPen = 0,
    SolidLine,
    DashLine,
    DotLine,
    DashDotLine,
    DashDotDotLine
  };

  enum ReportObjectType {
    Invalid,
    Label,
    Field,
    Calc,
    Special
  };

  /**
  Constructor
  */
  MReportObject();

  /**
  Copy constructor
  */
  MReportObject(const MReportObject &mReportObject);

  /**
  Assignment operator
  */
  MReportObject operator=(const MReportObject &mReportObject);

  /**
  Destructor
  */
  virtual ~MReportObject();

protected:

  /**
  Object's x position
  */
  int xpos;

  /**
  Object's y postition
  */
  int ypos;

  /**
  Object's width
  */
  int width;

  /**
  Object's height
  */
  int height;

  /**
  Object's background color
  */
  QColor backgroundColor;

  /**
  True if the object has no background color
  */
  bool transparent;

  /**
  True if the object must be drawed at the bottom of the section
  */
  bool drawAtBottom;

  /**
  Object's foreground color
  */
  QColor foregroundColor;

  /**
  Object's border color
  */
  QColor borderColor;

  /**
  Object's border width
  */
  int borderWidth;

  /**
  Object's border style
  */
  int borderStyle;
	
	/**
  Report section index. -1 when is not yet in a report section
  */
  int sectionIndex;

  /**
  Object's id
  */
  int objectId;

public:

  /**
  Draws the object to the specified painter & x/y offsets
  */
  virtual int draw(FLStylePainter *p);

  /**
  Sets the object's position and size
  */
  void setGeometry(int x, int y, int w, int h);

  /**
  Sets the object's position
  */
  void move(int x, int y);

  /**
  Gets the object's x position
  */
  int getX() const;

  /**
  Gets the object's y position
  */
  int getY() const;

  /**
  Gets the object's width
  */
  int getWidth() const;

  /**
  Gets the object's height
  */
  int getHeight() const;

  /**
  Gets the object's report section index
  */
  int getSectionIndex() const;

  /**
  Gets the object's drawAtBottom property
  */
  bool getDrawAtBottom() const;

  /**
  Gets the object's id
  */
  int getObjectId() const;

  /**
  Sets the object's background color - default is white
  */
  void setBackgroundColor(int r, int g, int b);

  /**
  Sets the object's foreground color - default is black
  */
  void setForegroundColor(int r, int g, int b);

  /**
  Sets the object's border color - default is black
  */
  void setBorderColor(int r, int g, int b);

  /**
  Sets the object's border width - default is 1
  */
  void setBorderWidth(int width);

  /**
  Sets the object's border style - default is SolidLine
  */
  void setBorderStyle(int style);

  /**
  Sets the object's transparent mode
  */
  void setTransparent(bool t);

  /**
  Sets the object's drawAtBottom property
  */
  void setDrawAtBottom(bool b);

  /**
  Sets the object's report section index
  */
  void setSectionIndex(int idx);

  /**
  Sets the object's id
  */
  void setObjectId(int id);

  /**
  Run-time type information
  */
  virtual int RTTI() const;

#ifdef FL_DEBUG
  static long countRefReportObject;
#endif

protected:

  /**
  Draws the base object to the specified painter & x/y offsets
  */
  void drawBase(FLStylePainter *p);

private:

  /**
  Copies member data from one object to another.

  Used by the copy constructor and assignment operator
  */
  void copy(const MReportObject *mReportObject);
};

/** Gets the object's x position */
inline int MReportObject::getX() const
{
  return xpos;
}

/** Gets the object's y position */
inline int MReportObject::getY() const
{
  return ypos;
}

/** Gets the object's height */
inline int MReportObject::getHeight() const
{
  return height;
}

/** Gets the object's width */
inline int MReportObject::getWidth() const
{
  return width;
}

/** Gets the object's drawAtBottom property */
inline bool MReportObject::getDrawAtBottom() const
{
  return drawAtBottom;
}

/** Gets the object's report section index */
inline int MReportObject::getSectionIndex() const
{
  return sectionIndex;
}

/** Gets the object's id */
inline int MReportObject::getObjectId() const
{
  return objectId;
}

/** Sets the object's border width */
inline void MReportObject::setBorderWidth(int width)
{
  borderWidth = width;
}

/** Sets the object's border style */
inline void MReportObject::setBorderStyle(int style)
{
  borderStyle = style;
}

/** Sets the object's transparent mode */
inline void MReportObject::setTransparent(bool t)
{
  transparent = t;
}

/** Sets the object's draw at bottom property */
inline void MReportObject::setDrawAtBottom(bool b)
{
  drawAtBottom = b;
}

/** Sets the object's report section index */
inline void MReportObject::setSectionIndex(int idx)
{
  sectionIndex = idx;
}

/** Sets the object's id */
inline void MReportObject::setObjectId(int id)
{
  objectId = id;
}
#endif
