/***************************************************************************
 mpagedisplay.cpp  -  Kugar page display widget
 -------------------
 begin     : Sun Aug 15 1999
 copyright : (C) 1999 by Mutiny Bay Software
 email     : info@mutinybaysoftware.com
 ***************************************************************************/

#include "mpagedisplay.h"

/** Constructor */
MPageDisplay::MPageDisplay(QWidget *parent, const char *name) :
  QWidget(parent, name)
{
  buffer = new QPixmap();
  buffer->resize(1, 1);
  bufferCopy = new QPixmap();
  bufferCopy->resize(1, 1);
}

/** Destructor */
MPageDisplay::~MPageDisplay()
{
  if (buffer)
    delete buffer;
  if (bufferCopy)
    delete bufferCopy;
}

/** Sets the report page image */
void MPageDisplay::setPage(QPicture *image)
{
  buffer->fill(white);
  QPainter p(buffer);
  p.setClipRect(0, 0, buffer->width(), buffer->height());
  image->play(&p);
  *bufferCopy = *buffer;
}

/** Display object's paint event */
void MPageDisplay::paintEvent(QPaintEvent *event)
{
  bitBlt(this, 0, 0, buffer);
}

/** Sets the page display dimensions */
void MPageDisplay::setPageDimensions(QSize size)
{
  buffer->resize(size);
  resize(size);
}

// Return the preferred size of the display.

QSize MPageDisplay::sizeHint() const
{
  return buffer->size();
}

// Return the size policy.

QSizePolicy MPageDisplay::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void MPageDisplay::zoomUp()
{
  QImage img;
  img = *bufferCopy;
  int width = buffer->width() * 1.25;
  int height = buffer->height() * 1.25;
  if (!img.isNull()) {
    buffer->resize(width, height);
    resize(width, height);
    *buffer = img.smoothScale(width, height);
  }
}

void MPageDisplay::zoomDown()
{
  QImage img;
  img = *bufferCopy;
  int width = buffer->width() / 1.25;
  int height = buffer->height() / 1.25;
  if (!img.isNull()) {
    buffer->resize(width, height);
    resize(width, height);
    *buffer = img.smoothScale(width, height);
  }
}
