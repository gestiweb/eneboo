/****************************************************************************
**
** Definition of QGfxRaster (unaccelerated graphics context) class
**
** Created : 940721
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free Qt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef QGFXRASTER_QWS_H
#define QGFXRASTER_QWS_H

#ifndef QT_H
#include "qgfx_qws.h"
#include "qpen.h"
#include "qbrush.h"
#include "qimage.h"
#include "qfontmanager_qws.h"
#include "qmemorymanager_qws.h"
#include "qwsdisplay_qws.h"
#include "qpointarray.h"
#include "qpolygonscanner.h"
#include "qapplication.h"
#include "qregion.h"
#endif // QT_H

//===========================================================================
// Utility macros and functions

#if !defined(QT_NO_QWS_CURSOR) && !defined(QT_QWS_ACCEL_CURSOR)
# define GFX_START(r) bool swc_do_save=FALSE; \
		    if(this->is_screen_gfx && this->gfx_swcursor) { \
			if((*this->gfx_optype)) sync(); \
			swc_do_save = this->gfx_screencursor->restoreUnder(r,this); \
			this->beginDraw(); \
		    }
# define GFX_END if(this->is_screen_gfx && this->gfx_swcursor) { \
		    if((*this->gfx_optype)) sync(); \
		    this->endDraw(); \
		    if(swc_do_save) \
			this->gfx_screencursor->saveUnder(); \
		 }
#else //QT_NO_QWS_CURSOR

# define GFX_START(r) if(this->is_screen_gfx) \
			this->beginDraw();
# define GFX_END if(this->is_screen_gfx) \
		    this->endDraw();
#endif //QT_NO_QWS_CURSOR


#ifndef QT_NO_QWS_GFX_SPEED
# define QWS_EXPERIMENTAL_FASTPATH
# define GFX_INLINE inline
#else
# define GFX_INLINE
#endif

#if defined(QT_NO_QWS_GFX_SPEED)
#define QWS_NO_WRITE_PACKING
#endif

typedef unsigned int PackType;


#define GET_MASKED(rev, advance) \
		    if( amonolittletest ) { \
			if(amonobitval & 0x1) { \
			    masked=FALSE; \
			} \
			amonobitval=amonobitval >> 1; \
		    } else { \
			if(amonobitval & 0x80) { \
			    masked=FALSE; \
			} \
			amonobitval=amonobitval << 1; \
			amonobitval=amonobitval & 0xff; \
		    } \
		    if(amonobitcount<7) { \
			amonobitcount++; \
		    } else if (advance) { \
			amonobitcount=0; \
			if (rev) maskp--; \
			else maskp++; \
			amonobitval=*maskp; \
		    } \


/*
  Finds a pointer to pixel (\a x, \a y) in a bitmap that
  is \a w pixels wide and stored in \a base. \a is_bigendian determines
  endianness. \a linestep is the bitmap's linestep in bytes, \a
  rev is true if this is being used for a reverse blt.

  \a astat returns the bit number within the byte
  \a ahold holds the \c monobitval which is the byte pre-shifted
           to match the algorithm using this function

  This is used by blt() to set up the pointer to the mask for
  Little/BigEndianMask alpha types.
*/
inline  unsigned char * find_pointer(unsigned char * base,int x,int y,
					       int w, int linestep, int &astat,
					       unsigned char &ahold,
					       bool is_bigendian, bool rev)
{
    int nbits;
    int nbytes;

    if ( rev ) {
	is_bigendian = !is_bigendian;
	nbits = 7 - (x+w) % 8;
       	nbytes = (x+w) / 8;
    } else {
	nbits = x % 8;
       	nbytes = x / 8;
    }

    astat=nbits;

    unsigned char *ret = base + (y*linestep) + nbytes;

    ahold=*ret;
    if(is_bigendian) {
	ahold=ahold << nbits;
    } else {
	ahold=ahold >> nbits;
    }

    return ret;
}

//===========================================================================

class QGfxRasterBase : public QGfx {

public:

    QGfxRasterBase(unsigned char *,int w,int h);
    ~QGfxRasterBase();

    virtual void setPen( const QPen & );
    virtual void setBrushPixmap( const QPixmap * p ) { cbrushpixmap=p; }
    virtual void setBrushOffset( int x, int y );
    virtual void setBrush( const QBrush & );

    virtual void setClipRect( int,int,int,int );
    virtual void setClipRegion( const QRegion & );
    virtual void setClipDeviceRegion( const QRegion & );
    virtual void setClipping(bool);

    // These will be called from qwidget_qws or qwidget_mac
    // to update the drawing area when a widget is moved
    virtual void setOffset( int,int );
    virtual void setWidgetRect( int,int,int,int );
    virtual void setWidgetRegion( const QRegion & );
    virtual void setWidgetDeviceRegion( const QRegion & );
    virtual void setGlobalRegionIndex( int idx );

    virtual void setDashedLines(bool d);
    virtual void setDashes(char *, int);

    virtual void moveTo( int,int );
    virtual void lineTo( int,int );

    virtual QPoint pos() const;

    virtual void setOpaqueBackground(bool b) { opaque=b; }
    virtual void setBackgroundColor(QColor c) { backcolor=c; }

    virtual void setAlphaType(AlphaType);
    virtual void setAlphaSource(unsigned char *,int);
    virtual void setAlphaSource(int,int=-1,int=-1,int=-1);
    virtual void drawGlyphs( QMemoryManager::FontID font, glyph_t *glyphs, QPoint *positions, int num_glyphs );

    virtual void sync();

    virtual void setLineStep(int i) { lstep=i; }
    int linestep() const { return lstep; }

    int pixelWidth() const { return width; }
    int pixelHeight() const { return height; }
    virtual int bitDepth() = 0;

    virtual void setScreen(QScreen * t,QScreenCursor * c,bool swc,int * ot,
			   int * lo) {
	gfx_screen=t;
#ifndef QT_NO_QWS_CURSOR
	gfx_screencursor=c;
	gfx_swcursor=swc;
#endif
	gfx_lastop=lo;
	gfx_optype=ot;
	setClut(gfx_screen->clut(),gfx_screen->numCols());
    }

    void save();
    void restore();

    virtual void setRop(RasterOp r) { myrop=r; }

    void setClut(QRgb * cols,int numcols) { clut=cols; clutcols=numcols;  }

protected:

#ifdef DEBUG_POINTERS
    void checkSource(unsigned char * c,int i) {
      if(i<0) {
	qFatal("Negative source coordinate");
      }
      if(i>=srcheight) {
	qFatal("Source pointer height overrun");
      }
      unsigned char * tmp1=srcbits+(i*srclinestep);
      unsigned char * tmp2=tmp1+srclinestep;
      if(c<tmp1) {
	qFatal("Source pointer underrun");
      }
      if(c>=tmp2) {
	qFatal("Source pointer overrun");
      }
    }

    void checkMask(unsigned char * c,int i) {
      unsigned char * tmp1=alphabits+(i*alphalinestep);
      unsigned char * tmp2=tmp1+alphalinestep;
      if(i<0) {
	qFatal("Negative mask coordinate");
      }
      if(i>=srcheight) {
	qFatal("Mask height overrun");
      }
      if(c<tmp1) {
	qFatal("Alpha pointer underrun");
      }
      if(c>=tmp2) {
	qFatal("Alpha pointer overrun");
      }
    }

    void checkDest(unsigned char * c,int i) {
      if(i<0) {
	qFatal("Negative dest coordinate");
      }
      if(i>=height) {
	qFatal("Destination height overrun");
      }
      unsigned char * tmp1=buffer+(i*lstep);
      unsigned char * tmp2=tmp1+lstep;
      if(c<tmp1) {
	qFatal("Destination pointer underrun");
      }
      if(c>=tmp2) {
	qFatal("Destination pointer overrun");
      }
    }

#endif


    void* beginTransaction( const QRect& );
    void endTransaction(void*);

    inline void beginDraw()
    {
#if !defined(QT_NO_QWS_MULTIPROCESS) && !defined(QT_PAINTER_LOCKING)
	QWSDisplay::grab();
#endif
	if ( globalRegionRevision &&
		*globalRegionRevision != currentRegionRevision ) {
	    fixClip();
	}
    }
    inline void endDraw()
    {
#if !defined(QT_NO_QWS_MULTIPROCESS) && !defined(QT_PAINTER_LOCKING)
	QWSDisplay::ungrab();
#endif
    }
    void fixClip();
    void update_clip();

    bool inClip(int x, int y, QRect* cr=0, bool know_to_be_outside=FALSE);

    virtual void setSourceWidgetOffset( int x, int y );

    virtual void setSourcePen();
    unsigned char *scanLine(int i) { return buffer+(i*lstep); }
    unsigned char *srcScanLine(int i) { return srcbits + (i*srclinestep); }

    // Convert to/from different bit depths
    unsigned int get_value_32(int sdepth,unsigned char **srcdata,
			   bool reverse=FALSE);
    unsigned int get_value_24(int sdepth,unsigned char **srcdata,
			   bool reverse=FALSE);
    unsigned int get_value_16(int sdepth,unsigned char **srcdata,
			   bool reverse=FALSE);
    unsigned int get_value_15(int sdepth,unsigned char **srcdata,
			   bool reverse=FALSE);
    unsigned int get_value_8(int sdepth,unsigned char **srcdata,
			   bool reverse=FALSE);
    unsigned int get_value_4(int sdepth,unsigned char **srcdata,
			   bool reverse=FALSE);
    unsigned int get_value_1(int sdepth,unsigned char **srcdata,
			   bool reverse=FALSE);

protected:
    QScreen * gfx_screen;
#ifndef QT_NO_QWS_CURSOR
    QScreenCursor * gfx_screencursor;
#endif
    bool gfx_swcursor;
    volatile int * gfx_lastop;
    volatile int * gfx_optype;

    SourceType srctype;
    PixelType srcpixeltype;
    unsigned char * srcbits;
    unsigned char * const buffer;

    PixelType pixeltype;
    int width;
    int height;
    int xoffs;
    int yoffs;
    unsigned int lstep;

    bool opaque;
    QColor backcolor;

    QPen cpen;
    QBrush cbrush;
    QPoint brushoffs;
    bool patternedbrush;
    const QPixmap * cbrushpixmap;
    bool dashedLines;
    char *dashes;
    int numDashes;

    QPen savepen;
    QBrush savebrush;

    bool regionClip;
    bool clipDirty;
    QRegion widgetrgn;
    QRegion cliprgn;
    QRect clipbounds;

    int penx;
    int peny;

    int srcwidth;
    int srcheight;
    int srcdepth;
    int srclinestep;
    int srccol;
    QPoint srcwidgetoffs;	    // Needed when source is widget
    bool src_little_endian;
    bool src_normal_palette;
    unsigned int srcclut[256];	    // Source color table - r,g,b values
    unsigned int transclut[256];    // Source clut transformed to destination
                                    // values - speed optimisation

    QRgb * clut;      		    // Destination color table - r,g,b values
    int clutcols;		    // Colours in clut

    int monobitcount;
    unsigned char monobitval;

    AlphaType alphatype;
    unsigned char * alphabits;
    unsigned int * alphabuf;
    int alphalinestep;
    bool ismasking;
    int amonobitcount;
    unsigned char amonobitval;
    bool amonolittletest;
    int calpha;       		 // Constant alpha value
    int calpha2,calpha3,calpha4; // Used for groovy accelerated effect
    unsigned char * maskp;

    int clipcursor;
    QRect* cliprect;
    int ncliprect;

    int globalRegionIndex;
    const int *globalRegionRevision;
    int currentRegionRevision;

    RasterOp myrop;

    unsigned long int pixel; // == cpen.pixel() or cbrush.pixel()

    QPolygonScanner::Edge stitchedges;

    friend class QScreenCursor;
    friend class QFontEngine;
};

template <const int depth, const int type>
class QGfxRaster : public QGfxRasterBase, protected QPolygonScanner {

public:

    QGfxRaster(unsigned char *,int w,int h);
    ~QGfxRaster();

    void useBrush();
    void usePen();

    virtual void drawPoint( int,int );
    virtual void drawPoints( const QPointArray &,int,int );
    virtual void drawLine( int,int,int,int );
    virtual void fillRect( int,int,int,int );
    virtual void drawPolyline( const QPointArray &,int,int );
    virtual void drawPolygon( const QPointArray &,bool,int,int );
    virtual void blt( int,int,int,int,int,int );
    virtual void scroll( int,int,int,int,int,int );
#if !defined(QT_NO_MOVIE) || !defined(QT_NO_TRANSFORMATIONS) || !defined(QT_NO_PIXMAP_TRANSFORMATION)
    virtual void stretchBlt( int,int,int,int,int,int );
#endif
    virtual void tiledBlt( int,int,int,int );

    virtual int bitDepth() { return depth; }

    virtual void setSource(const QImage *);
    virtual void setSource(const QPaintDevice *);
    virtual void setSource(unsigned char *,int,int,int,int,QRgb *,int);

protected:

    virtual void drawThickLine( int,int,int,int );
    virtual void drawThickPolyline( const QPointArray &,int,int );

    void buildSourceClut(QRgb *,int);
    void processSpans( int n, QPoint* point, int* width );

    // Optimised vertical line drawing
    void vline(int,int,int );

    // Optimised horizontal line drawing
    void hline(int,int,int );
    void hlineUnclipped(int,int,unsigned char* );
#if defined(Q_OS_QNX6) // need a different signature for QNX acceleration, override to accel
    virtual void hlineUnclipped(int x,int x1,int y){unsigned char *l=scanLine(y);hlineUnclipped(x,x1,l);};
#endif
    void hImageLineUnclipped(int,int,unsigned char *,unsigned char *,bool);
    void hAlphaLineUnclipped(int,int,unsigned char *,unsigned char *,
			     unsigned char *);
    void drawPointUnclipped( int, unsigned char* );

    void calcPacking(void *,int,int,int&,int&,int&);
};

#endif // QGFXRASTER_QWS_H
