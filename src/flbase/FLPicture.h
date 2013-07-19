/***************************************************************************
 FLPicture.h
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

#ifndef FLPICTURE_H
#define FLPICTURE_H

#include <qobject.h>
#include <qpicture.h>
#include <qpainter.h>
#include <qpaintdevice.h>
#include <qpixmap.h>
#include <qdeepcopy.h>

#include "AQGlobal.h"

class FLPicturePrivate;

class AQ_EXPORT FLPicture : public QObject {

  Q_OBJECT
  Q_ENUMS( FLPenStyle )
  Q_ENUMS( FLBrushStyle )
  Q_ENUMS( FLBGMode )
  Q_ENUMS( FLRasterOp )
  Q_ENUMS( FLCoordinateMode );
  Q_ENUMS( FLTextDirection );

public:

  enum FLPenStyle {
    NoPen,
    SolidLine,
    DashLine,
    DotLine,
    DashDotLine,
    DashDotDotLine,
    MPenStyle = 0x0f
  };

  enum FLBrushStyle {
    NoBrush,
    SolidPattern,
    Dense1Pattern,
    Dense2Pattern,
    Dense3Pattern,
    Dense4Pattern,
    Dense5Pattern,
    Dense6Pattern,
    Dense7Pattern,
    HorPattern,
    VerPattern,
    CrossPattern,
    BDiagPattern,
    FDiagPattern,
    DiagCrossPattern,
    CustomPattern = 24
  };

  enum FLBGMode {
    TransparentMode,
    OpaqueMode
  };

  enum FLRasterOp {
    CopyROP,
    OrROP,
    XorROP,
    NotAndROP, EraseROP = NotAndROP,
    NotCopyROP,
    NotOrROP,
    NotXorROP,
    AndROP, NotEraseROP = AndROP,
    NotROP,
    ClearROP,
    SetROP,
    NopROP,
    AndNotROP,
    OrNotROP,
    NandROP,
    NorROP, LastROP = NorROP
  };

  enum FLCoordinateMode {
    CoordDevice,
    CoordPainter
  };

  enum FLTextDirection {
    Auto,
    RTL,
    LTR
  };

  enum FLAlignment {
    AlignAuto = Qt::AlignAuto,
    AlignLeft = Qt::AlignLeft,
    AlignRight = Qt::AlignRight,
    AlignHCenter = Qt::AlignHCenter,
    AlignJustify = Qt::AlignJustify,
    AlignTop = Qt::AlignTop,
    AlignBottom= Qt::AlignBottom,
    AlignVCenter = Qt::AlignVCenter,
    AlignCenter = Qt::AlignCenter,
    AlignHorizontal_Mask = Qt::AlignHorizontal_Mask,
    AlignVertical_Mask = Qt::AlignVertical_Mask
  };

  FLPicture( QObject * parent = 0, const char * name = 0 );
  FLPicture( FLPicture * other );
  FLPicture( const QPicture & pic );
  FLPicture( QPicture * pic, QObject * parent, const char * name = 0 );
  FLPicture( QPicture * pic, QPainter * pte, QObject * parent, const char * name = 0 );

  ~FLPicture();

public slots:

  QPicture * picture() const;
  void setPicture( const QPicture & pic );
  void setPicture( QPicture * pic );

  bool isNull() const;
  bool load( const QString & fileName, const char * format = 0 );
  bool save( const QString & fileName, const char * format = 0 );
  QRect boundingRect() const;
  void setBoundingRect( const QRect & r );
  bool begin();
  bool end() const;
  void cleanup();
  bool isActive() const;
  void flush() const;
  void savePainter() const;
  void restorePainter() const;

  const QFont & font() const;
  void setFont( const QFont & font ) const;
  void setPen( const QColor & color, uint width = 0, FLPenStyle style = FLPicture::SolidLine ) const;
  void setBrush( const QColor & color, FLBrushStyle style = FLPicture::SolidPattern ) const;

  const QColor & backgroundColor() const;
  void setBackgroundColor( const QColor & color ) const;
  FLPicture::FLBGMode backgroundMode() const;
  void setBackgroundMode( FLBGMode bgm ) const;
  FLPicture::FLRasterOp rasterOp() const;
  void setRasterOp( FLRasterOp rop ) const;
  const QPoint & brushOrigin() const;
  void setBrushOrigin( int x, int y ) const;
  void setBrushOrigin( const QPoint & pt ) const;

  //bool hasViewXForm() const;
  //bool hasWorldXForm() const;
  //void setViewXForm( bool );
  QRect window() const;
  void setWindow( const QRect & r ) const;
  void setWindow( int x, int y, int w, int h ) const;
  QRect viewport() const;
  void setViewport( const QRect & r ) const;
  void setViewport( int x, int y, int w, int h ) const;

  //void setWorldXForm( bool );
  //const QWMatrix &worldMatrix() const;
  //void setWorldMatrix( const QWMatrix &, bool combine=FALSE );

  //void saveWorldMatrix();
  //void restoreWorldMatrix();

  void scale( double sx, double sy ) const;
  void shear( double sh, double sv ) const;
  void rotate( double a ) const;
  void translate( double dx, double dy ) const;
  //void resetXForm();
  double translationX() const;
  double translationY() const;

  //QPoint xForm( const QPoint & ) const;
  //QRect xForm( const QRect & ) const;
  //QPointArray xForm( const QPointArray & ) const;
  //QPointArray xForm( const QPointArray &, int index, int npoints ) const;
  //QPoint xFormDev( const QPoint & ) const;
  //QRect xFormDev( const QRect & )  const;
  //QPointArray xFormDev( const QPointArray & ) const;
  //QPointArray xFormDev( const QPointArray &, int index, int npoints ) const;

  void setClipping( bool c ) const;
  bool hasClipping() const;
  //QRegion clipRegion( CoordinateMode = CoordDevice ) const;
  void setClipRect( const QRect & r, FLCoordinateMode co = CoordDevice ) const;
  void setClipRect( int x, int y, int w, int h, FLCoordinateMode co = CoordDevice ) const;
  //void setClipRegion( const QRegion &, CoordinateMode = CoordDevice );

  void drawLine( int x1, int y1, int x2, int y2 ) const;
  void drawLine( const QPoint & pt1, const QPoint & pt2 ) const;
  void drawPoint( int x, int y ) const;
  void drawPoint( const QPoint & pt ) const;
  //void drawPoints( const QPointArray& a, int index=0, int npoints=-1 );
  void moveTo( int x, int y ) const;
  void moveTo( const QPoint & pt ) const;
  void lineTo( int x, int y ) const;
  void lineTo( const QPoint & pt ) const;
  void drawRect( int x, int y, int w, int h ) const;
  void drawRect( const QRect & r ) const;
  void drawWinFocusRect( int x, int y, int w, int h ) const;
  void drawWinFocusRect( int x, int y, int w, int h, const QColor & bgColor ) const;
  void drawWinFocusRect( const QRect & r ) const;
  void drawWinFocusRect( const QRect & r, const QColor & bgColor ) const;
  void drawRoundRect( int x, int y, int w, int h, int xRnd = 25, int yRnd = 25 ) const;
  void drawRoundRect( const QRect & r, int xRnd = 25, int yRnd = 25 ) const;
  void drawEllipse( int x, int y, int w, int h ) const;
  void drawEllipse( const QRect & r ) const;
  void drawArc( int x, int y, int w, int h, int a, int alen ) const;
  void drawArc( const QRect & r, int a, int alen ) const;
  void drawPie( int x, int y, int w, int h, int a, int alen ) const;
  void drawPie( const QRect & r, int a, int alen ) const;
  void drawChord( int x, int y, int w, int h, int a, int alen ) const;
  void drawChord( const QRect & r, int a, int alen ) const;
  //void drawLineSegments( const QPointArray &, int index=0, int nlines=-1 );
  //void drawPolyline( const QPointArray &, int index=0, int npoints=-1 );
  //void drawPolygon( const QPointArray &, bool winding=FALSE, int index=0, int npoints=-1 );
  //void drawConvexPolygon( const QPointArray &, int index=0, int npoints=-1 );
  //void drawCubicBezier( const QPointArray &, int index=0 );
  void drawPixmap( int x, int y, const QPixmap & pix, int sx = 0, int sy = 0, int sw = -1, int sh = -1 ) const;
  void drawPixmap( const QPoint & pt, const QPixmap & pix, const QRect & r ) const;
  void drawPixmap( const QPoint & pt, const QPixmap & pix ) const;
  void drawPixmap( const QRect & r, const QPixmap & pix ) const;
  //void drawImage( int x, int y, const QImage &, int sx = 0, int sy = 0, int sw = -1, int sh = -1, int conversionFlags = 0 );
  //void drawImage( const QPoint &, const QImage &, const QRect &sr, int conversionFlags = 0 );
  //void drawImage( const QPoint &, const QImage &, int conversion_flags = 0 );
  //void drawImage( const QRect &, const QImage & );
  void drawTiledPixmap( int x, int y, int w, int h, const QPixmap & pix, int sx = 0, int sy = 0 ) const;
  void drawTiledPixmap( const QRect & r, const QPixmap & pix, const QPoint & pt ) const;
  void drawTiledPixmap( const QRect & r, const QPixmap & pix ) const;
  void drawPicture( FLPicture * pic );
  void drawPicture( int x, int y, FLPicture * pic ) const;
  void drawPicture( const QPoint & pt, FLPicture * pic ) const;

  void fillRect( int x, int y, int w, int h, const QColor & color, FLBrushStyle style = FLPicture::SolidPattern ) const;
  void fillRect( const QRect & r, const QColor & color, FLBrushStyle style = FLPicture::SolidPattern ) const;
  void eraseRect( int x, int y, int w, int h ) const;
  void eraseRect( const QRect & r ) const;

  void drawText( int x, int y, const QString & t, int len = -1, FLTextDirection dir = Auto ) const;
  void drawText( const QPoint & pt, const QString & t, int len = -1, FLTextDirection dir = Auto ) const;
  void drawText( int x, int y, const QString & t, int pos, int len, FLTextDirection dir = Auto ) const;
  void drawText( const QPoint & p, const QString & t, int pos, int len, FLTextDirection dir = Auto ) const;
  void drawText( int x, int y, int w, int h, int flags, const QString & t, int len = -1, QRect * br = 0/*, QTextParag **intern=0*/) const;
  void drawText( const QRect & r, int flags, const QString & t, int len = -1, QRect * br = 0/*, QTextParag **intern=0*/) const;

  //void drawTextItem( int x, int y, const QTextItem &ti, int textflags = 0 );
  //void drawTextItem( const QPoint& p, const QTextItem &ti, int textflags = 0 );

  QRect boundingRect( int x, int y, int w, int h, int flags, const QString & t, int len = -1/*, QTextParag **intern=0*/) const;
  QRect boundingRect( const QRect & r, int flags, const QString & t, int len = -1/*, QTextParag **intern=0*/) const;

  QPixmap * playOnPixmap( QPixmap * pix );
  FLPicture * playOnPicture( FLPicture * pic );

private:

  FLPicturePrivate * d;
};

#endif
