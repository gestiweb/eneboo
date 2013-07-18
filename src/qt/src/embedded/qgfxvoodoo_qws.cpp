/****************************************************************************
**
** Implementation of QGfxVoodoo (graphics context) class for Voodoo 3 cards
**
** Created : 000503
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

#include "qgfxvoodoodefs_qws.h"

#ifndef QT_NO_QWS_VOODOO3

#include "qgfxvoodoo_qws.h"
#include "qgfxraster_qws.h"
#include "qimage.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>

// This is intended as an example of an accelerated driver, as is
// Mach64 driver. The Mach64 driver implements more functionality;
// this is a simpler driver which may be easier to follow. This driver
// should work with 3dfx Voodoo 3 and Voodoo Banshee cards (PCI or AGP)
// Note that the register-level specifications for these cards are freely
// available from the 3dfx web site

// Pointer to Voodoo 3 registers

//#define DEBUG_INIT

// An integer, lastop, is stored in shared memory and is set to one
// of these values. The reason for this is that if an accelerated
// LASTOP_RECT is followed by another LASTOP_RECT, for example,
// some register setup can be avoided

#define LASTOP_LINE 1
#define LASTOP_RECT 2
#define LASTOP_BLT 3
#define LASTOP_BLTPEN 4
#define LASTOP_STRETCHBLT 5
#define LASTOP_RESET 6
#define LASTOP_POLYGON 7
#define LASTOP_TEXT 8
#define LASTOP_ALPHA 9
#define LASTOP_TILEDBLT 10
#define LASTOP_TILEDBLTPEN 11
#define LASTOP_SYNC 12

template <const int depth, const int type>
class QGfxVoodoo : public QGfxRaster<depth,type> {

public:

    QGfxVoodoo(unsigned char *,int w,int h,unsigned char *);

    virtual void fillRect(int,int,int,int);
    virtual void blt(int,int,int,int,int,int);
#if !defined(QT_NO_MOVIE) || !defined(QT_NO_TRANSFORMATIONS)
    virtual void stretchBlt(int,int,int,int,int,int);
#endif
    virtual void drawLine(int,int,int,int);
    virtual void sync();

    unsigned int regr(volatile unsigned int);
    void regw(volatile unsigned int,unsigned long);
    void wait_for_fifo(short);

private:

    unsigned int getRop(RasterOp r);

    unsigned char * voodoo_regbase;

    bool checkSourceDest();
    void setDest();
    void do_scissors(QRect &);

};

// Oops, all these except CopyROP are wrong because they don't seem
// to be documented!

template<const int depth,const int type>
inline unsigned int QGfxVoodoo<depth,type>::getRop(RasterOp r)
{
  if(r==CopyROP) {
    return 0xcc;
  } else if(r==OrROP) {
    return 0xe;
  } else if(r==XorROP) {
    return 0x6;
  } else if(r==NotAndROP) {
    return 0x4;
  } else if(r==NotCopyROP) {
    return 0x3;
  } else if(r==NotOrROP) {
    return 0xd;
  } else if(r==NotXorROP) {
    return 0x9;
  } else if(r==AndROP) {
    return 0x8;
  } else if(r==NotROP) {
    return 0x5;
  } else if(r==ClearROP) {
    return 0x0;
  } else if(r==SetROP) {
    return 0xf;
  } else if(r==NopROP) {
    return 0xa;
  } else if(r==AndNotROP) {
    return 0x2;
  } else if(r==OrNotROP) {
    return 0xb;
  } else if(r==NandROP) {
    return 0x7;
  } else if(r==NorROP) {
    return 0x1;
  } else {
    qFatal("Unknown ROP!");
    return 0;
  }
}

// Read a 32-bit graphics card register from 2d engine register block
template<const int depth,const int type>
inline unsigned int QGfxVoodoo<depth,type>::regr(volatile unsigned int
						 regindex)
{
    unsigned long int val;
    val=*((volatile unsigned long *)(voodoo_regbase+regindex));
    return val;
}

// Write a 32-bit graphics card register to 2d engine register block
template<const int depth,const int type>
inline void QGfxVoodoo<depth,type>::regw(volatile unsigned int regindex,
					 unsigned long val)
{
    *((volatile unsigned long int *)(voodoo_regbase+regindex))=val;
}

// Wait <entry> FIFO entries. <entry> FIFO entries must be free
// before making <entry> regw's or regw2's, or you'll lock up the
// graphics card and your computer. The total number of FIFO entries
// varies from card to card.

template<const int depth,const int type>
inline void QGfxVoodoo<depth,type>::wait_for_fifo(short entries)
{
    QLinuxFb_Shared * tmp=(QLinuxFb_Shared *)shared_data;
    tmp->fifocount+=entries;
    if(tmp->fifocount<tmp->fifomax)
	return;

    for(int loopc=0;loopc<1000000;loopc++) {
	int fifoval=regr(VOODOOSTATUS);
	fifoval&=0x1f;
	if(fifoval==0x1f) {
	    tmp->fifocount=0;
	    return;
	}
    }

    qDebug("Wait for fifo timeout!");
}

template<const int depth,const int type>
inline void QGfxVoodoo<depth,type>::do_scissors(QRect & r)
{
    // Voodoo clipping includes minimum values but excludes maximum values

    QLinuxFb_Shared * tmp=(QLinuxFb_Shared *)shared_data;
    if(tmp->clipleft!=r.left() || tmp->cliptop!=r.top()) {
        wait_for_fifo(1);
        regw(CLIP0MIN,(r.top()) << 16 | r.left());
	tmp->clipleft=r.left();
	tmp->cliptop=r.top();
    }
    if(tmp->clipright!=r.right() ||
       tmp->clipbottom!=r.bottom()) {
        wait_for_fifo(1);
        regw(CLIP0MIN,(r.bottom()) << 16 | r.right());
	tmp->clipright=r.right();
	tmp->clipbottom=r.bottom();
    }
}

template<const int depth,const int type>
inline void QGfxVoodoo<depth,type>::sync()
{
    // NOP to avoid documented deadlock
    (*gfx_lastop)=LASTOP_SYNC;
    regw(COMMAND,0x100);

    // Need a slight pause - possibly to let the operation actually kick
    // off?

    usleep(100);

    // Now wait until we're told graphics engine is idle
    int loopc;
    for(loopc=0;loopc<1000;loopc++) {
        unsigned int stat=regr(VOODOOSTATUS);
	if((stat & ~0x40)==0x1f)
	    return;
    }
    qDebug("Idle timeout!");
}

// Figures out the 3-bit code for different depths
inline int voodoo_depthcode(int d)
{
    int ret;
    if(d==32) {
	ret=5;
    } else if(d==16) {
	ret=3;
    } else if(d==8) {
	ret=1;
    } else {
	qFatal("Unexpected depth %d",d);
	ret=0;
    }
    return ret;
}

// Set up DST_OFF_PITCH, return false if it's not on the card
// For lines, filled rects etc
// This is similar to checkSourceDest but is used when there is no
// source image data
template<const int depth,const int type>
inline void QGfxVoodoo<depth,type>::setDest()
{
    ulong buffer_offset;
    gfx_screen->onCard(buffer,buffer_offset);

    QRect tmprect(0,0,width,height);
    do_scissors(tmprect);

    QLinuxFb_Shared * tmp=(QLinuxFb_Shared *)shared_data;
    if(tmp->buffer_offset==buffer_offset && tmp->linestep==linestep()) {
      return;
    } else {
      tmp->buffer_offset=buffer_offset;
      tmp->linestep=linestep();
    }

    wait_for_fifo(2);
    regw(DSTBASEADDR,buffer_offset);
    regw(DSTFORMAT,linestep() | (voodoo_depthcode(depth) << 16));

    return;
}

// Sets up the graphics engine's idea of bits-per-pixel for destination
// and source, used for, for example, blt's
template<const int depth,const int type>
inline bool QGfxVoodoo<depth,type>::checkSourceDest()
{
    setDest();

    ulong src_buffer_offset;
    if (srctype == SourcePen) {
	src_buffer_offset = -1;
    } else {
	if (!gfx_screen->onCard(srcbits,src_buffer_offset)) {
	    return FALSE;
	}
	if(src_buffer_offset & 0x7) {
	    qDebug("Unaligned offset %lx",src_buffer_offset);
	    return FALSE;
	}
	int srcstep;
	if (srclinestep==0) {
	    srcstep=(width*srcdepth)/8;
	} else {
	    srcstep=srclinestep;
	}

	wait_for_fifo(2);
	regw(SRCBASEADDR, src_buffer_offset);
	regw(SRCFORMAT,(srcstep | (voodoo_depthcode(srcdepth) << 16)));
    }
    return TRUE;
}

template<const int depth,const int type>
QGfxVoodoo<depth,type>::QGfxVoodoo(unsigned char * a,int b,int c,
				   unsigned char * r)
    : QGfxRaster<depth,type>(a,b,c)
{
    voodoo_regbase=r;
}

template<const int depth,const int type>
void QGfxVoodoo<depth,type>::fillRect(int rx,int ry,int w,int h)
{
    // No point going any further if the window isn't visible
    if(ncliprect<1) {
	return;
    }

    // Only handle 'normal' rectangles
    if((cbrush.style()!=NoBrush) && (cbrush.style()!=SolidPattern) ) {
	QGfxRaster<depth,type>::fillRect(rx,ry,w,h);
	return;
    }

    // Stop anyone else trying to access optype/lastop/the graphics engine
    // to avoid synchronization problems with other processes


    // This is used by the software mouse cursor to prevent corruption
    // of the cursor if a drawing operation is performed under it.
    // GFX_START/END can be omitted if you know you'll only ever use
    // the hardware cursor

    GFX_START(QRect(rx+xoffs, ry+yoffs, w+1, h+1))
    setDest();

    int loopc;

    int xp=rx+xoffs;
    int yp=ry+yoffs;

    int x2,y2;

    x2=xp+(w-1);
    y2=yp+(h-1);

    int x3,y3,x4,y4;

    if(true /*(*gfx_lastop)!=LASTOP_RECT*/) {
	wait_for_fifo(2);
	regw(SRCFORMAT,3 << 16);
	// With the Voodoo 3 you write the command code into COMMAND
	// and then write parameters (usually x/y coordinates of some sort)
	// into LAUNCHAREA to kick off the operation
	regw(COMMAND,0x5 | (getRop(myrop) << 24));
    }

    (*gfx_optype)=1;
    (*gfx_lastop)=LASTOP_RECT;

#ifndef QT_NO_QWS_REPEATER
    QScreen * tmp=qt_screen;
    qt_screen=gfx_screen;
#endif
    QColor c=cbrush.color();
    srccol=c.alloc();
#ifndef QT_NO_QWS_REPEATER
    qt_screen=tmp;
#endif

    if(((QLinuxFb_Shared *)shared_data)->forecol!=srccol) {
	wait_for_fifo(1);
	regw(COLORFORE,srccol);
	((QLinuxFb_Shared *)shared_data)->forecol=srccol;
    }

    // We clip in software here because rectangle-rectangle intersections
    // are very fast, probably much more so than writing graphics card
    // registers to set up the clip

    if(cbrush.style()!=NoBrush) {
	int p=ncliprect;
	if(p<8) {
	    // We can wait for all our fifos at once
	    // (slight performance optimisation)
	    wait_for_fifo(p*2);
	    for(loopc=0;loopc<p;loopc++) {
		QRect r=cliprect[loopc];
		// Clip rectangle to current clip rectangle
		if(xp<=r.right() && yp<=r.bottom() &&
		   x2>=r.left() && y2>=r.top()) {
		    x3=r.left() > xp ? r.left() : xp;
		    y3=r.top() > yp ? r.top() : yp;
		    x4=r.right() > x2 ? x2 : r.right();
		    y4=r.bottom() > y2 ? y2 : r.bottom();
		    int ww=(x4-x3)+1;
		    int hh=(y4-y3)+1;
		    regw(DSTSIZE,(hh << 16) | ww);
		    regw(LAUNCHAREA,x3 | (y3 << 16));
		}
	    }
	} else {
	    for(loopc=0;loopc<p;loopc++) {
		QRect r=cliprect[loopc];
		if(xp<=r.right() && yp<=r.bottom() &&
		   x2>=r.left() && y2>=r.top()) {
		    x3=r.left() > xp ? r.left() : xp;
		    y3=r.top() > yp ? r.top() : yp;
		    x4=r.right() > x2 ? x2 : r.right();
		    y4=r.bottom() > y2 ? y2 : r.bottom();
		    int ww=(x4-x3)+1;
		    int hh=(y4-y3)+1;
		    wait_for_fifo(2);
		    regw(DSTSIZE,(hh << 16) | ww);
		    regw(LAUNCHAREA,x3 | (y3 << 16));
		}
	    }
	}
    }
    GFX_END
#if !defined(QT_NO_QWS_MULTIPROCESS) && !defined(QT_PAINTER_LOCKING)
    QWSDisplay::ungrab();
#endif
}

template<const int depth,const int type>
inline void QGfxVoodoo<depth,type>::blt(int rx,int ry,int w,int h, int sx, int sy)
{
    if(ncliprect<1)
	return;

    if(srctype==SourceImage && alphatype!=IgnoreAlpha) {
	QGfxRaster<depth,type>::blt(rx,ry,w,h,sx,sy);
	return;
    }

    if(srctype==SourcePen) {
	QGfxRaster<depth,type>::blt(rx,ry,w,h,sx,sy);
	return;
    }

    if( (srcdepth!=32) && (srcdepth!=16) && (srcdepth!=8) ) {
	QGfxRaster<depth,type>::blt(rx,ry,w,h,sx,sy);
	return;
    }

    int xp=xoffs+rx;
    int yp=yoffs+ry;
    QRect cursRect(xp, yp, w+1, h+1);
    GFX_START(cursRect)
	
    if(checkSourceDest()) {

	int xp2=srcwidgetoffs.x() + sx;
	int yp2=srcwidgetoffs.y() + sy;

	(*gfx_optype)=1;
	(*gfx_lastop)=LASTOP_BLT;
	
        unsigned int dirmask=0;

	// Tell the engine whether to copy bits from left to right,
	// top to bottom, right to left, bottom to top - this is
	// important for getting the right results with an overlapping
	// blt

	int mx = QMIN(xp,xp2);
	if ( mx < 0 ) {
	    xp -= mx;
	    xp2 -= mx;
	    w += mx;
	}

	bool down, right;

	if(yp>yp2) {
	    // Down, reverse
	    down=TRUE;
	    if(xp>xp2) {
		// Right, reverse
		right=TRUE;
		dirmask|=0x4000 | 0x8000;
	    } else {
		// Left, normal
		right=FALSE;
		dirmask|=0x8000;
	    }
	} else {
	    // Up, normal
	    // Down, reverse
	    down=FALSE;
	    if(xp>xp2) {
		// Right, reverse
		dirmask|=0x4000;
		right=TRUE;
	    } else {
		// Left, normal
		right=FALSE;
	    }
	}

	int loopc=down ? ncliprect-1 : 0;

	while ( loopc >=0 && loopc < ncliprect ) {

	    QRect r1(cliprect[loopc]);
	    QRect r2(xp,yp,w,h);
	    r2=r2.intersect(r1);
	    int ww=r2.width();
	    int hh=r2.height();
	    int xp3=r2.left();
	    int yp3=r2.top();
	    int dx=r2.left()-xp;
	    int dy=r2.top()-yp;
	    int xp4=xp2+dx;
	    int yp4=yp2+dy;


	    if(ww>0 && hh>0) {

		if(right) {
		    xp3+=(ww-1);
		    xp4+=(ww-1);
		}
		if(down) {
		    yp3+=(hh-1);
		    yp4+=(hh-1);
		}

		wait_for_fifo(4);
		regw(SRCXY,xp4 | (yp4 << 16));
		regw(DSTSIZE,ww | (hh << 16));
		regw(DSTXY,xp3 | (yp3 << 16));
		regw(COMMAND,0x1 | (0x1cc << 24) | dirmask | 0x100);
	    }
	    if(down) {
		loopc--;
	    } else {
		loopc++;
	    }
	}

	QRect r(0,0,width,height);
	do_scissors(r);

	GFX_END

	return;
    } else {
	GFX_END
	
	// software fallback
	QGfxRaster<depth,type>::blt(rx,ry,w,h,sx,sy);
    }
}

#if !defined(QT_NO_MOVIE) || !defined(QT_NO_TRANSFORMATIONS)
template<const int depth,const int type>
inline void QGfxVoodoo<depth,type>::stretchBlt(int rx,int ry,int w,int h,
					       int sw,int sh)
{
    // On Voodoo3's, as opposed to Mach64, stretchBlt is a normal 2d operation
    // and not part of a separate pipeline
    // Hence optype is set to 1, since a sequence like blt/stretchBlt/blt
    // is guaranteed to be performed in that order; on Mach64 without a
    // sync() before the second blt the stretchBlt and second blt might overlap
    // and both write to the same area simultaneously

    if(ncliprect)
	return;

    if(srctype==SourceImage && alphatype!=IgnoreAlpha) {
	QGfxRaster<depth,type>::stretchBlt(rx,ry,w,h,sw,sh);
	return;
    }

    if(srctype==SourcePen) {
	QGfxRaster<depth,type>::stretchBlt(rx,ry,w,h,sw,sh);
	return;
    }

    if( (srcdepth!=32) && (srcdepth!=16) && (srcdepth!=8) ) {
	QGfxRaster<depth,type>::stretchBlt(rx,ry,w,h,sw,sh);
	return;
    }

    int xp=xoffs+rx;
    int yp=yoffs+ry;
    QRect cursRect(xp, yp, w+1, h+1);
    GFX_START(cursRect)
	
    if(checkSourceDest()) {

	int xp2=srcwidgetoffs.x(); // + sx;
	int yp2=srcwidgetoffs.y(); // + sy;

	(*gfx_optype)=1;
	(*gfx_lastop)=LASTOP_STRETCHBLT;

	wait_for_fifo(4);
	regw(COMMAND,0x2 | (getRop(myrop) << 24));
	regw(SRCSIZE,sw | (sh << 16));
	regw(DSTSIZE,w | (h << 16));
	regw(DSTXY,xp | (yp << 16));

	int loopc;
	for(loopc=0;loopc<ncliprect;loopc++) {
	    do_scissors(cliprect[loopc]);
	    wait_for_fifo(1);
	    regw(LAUNCHAREA,xp2 | (yp2 << 16));
	}
	QRect tmprect(0,0,width,height);
	do_scissors(tmprect);

	GFX_END

	return;
    } else {
	GFX_END
	QGfxRaster<depth,type>::stretchBlt(rx,ry,w,h,sw,sh);
    }
}
#endif

template<const int depth,const int type>
void QGfxVoodoo<depth,type>::drawLine(int x1,int y1,int x2,int y2)
{
    if(ncliprect<1 || cpen.style()!=SolidLine
       || x1+xoffs<0 || y1+yoffs<0 || x2+xoffs<0 || y2+yoffs<0 || TRUE) {
	QGfxRaster<depth,type>::drawLine(x1,y1,x2,y2);
	return;
    }

    x1+=xoffs;
    y1+=yoffs;
    x2+=xoffs;
    y2+=yoffs;

    int dx,dy;
    dx=abs(x2-x1);
    dy=abs(y2-y1);

    // On the Voodoo3, unlike the Mach64, Bresenham parameters
    // for the line are calculated automatically

    GFX_START(QRect(x1, y1 < y2 ? y1 : y2, dx+1, QABS(dy)+1))
    setDest();
    (*gfx_optype)=1;
    (*gfx_lastop)=LASTOP_LINE;

#ifndef QT_NO_QWS_REPEATER
    QScreen * tmpscreen=qt_screen;
    qt_screen=gfx_screen;
#endif

#ifndef QT_NO_QWS_REPEATER
    qt_screen=tmpscreen;
#endif

    int loopc;

    if(((QLinuxFb_Shared *)shared_data)->forecol!=srccol) {
      wait_for_fifo(1);
      regw(COLORFORE,srccol);
      ((QLinuxFb_Shared *)shared_data)->forecol=srccol;
    }

    wait_for_fifo(1);
    regw(COMMAND,0x6 | getRop(myrop));

    for(loopc=0;loopc<ncliprect;loopc++) {
      do_scissors(cliprect[loopc]);
      wait_for_fifo(2);
      regw(SRCXY,x1 | (y1 << 16));
      regw(LAUNCHAREA,x2 | (y2 << 16));
    }
    QRect tmprect(0,0,width,height);
    do_scissors(tmprect);

    GFX_END
    return;
}

// This does card-specific setup and constructs accelerated gfx's and
// the accelerated cursor

#ifndef QT_NO_QWS_CURSOR
class QVoodooCursor : public QScreenCursor
{
public:
    QVoodooCursor();
    ~QVoodooCursor();

    virtual void init(SWCursorData *,bool=FALSE);

    virtual void set( const QImage &image, int hotx, int hoty );
    virtual void move( int x, int y );
    virtual void show();
    virtual void hide();

    virtual bool restoreUnder( const QRect &, QGfxRasterBase * = 0 )
                { return FALSE; }
    virtual void saveUnder() {}
    virtual void drawCursor() {}
    virtual void draw() {}
    virtual bool supportsAlphaCursor() { return FALSE; }

    static bool enabled() { return FALSE; }


    void regw(volatile unsigned int, unsigned long);
    unsigned int regr(volatile unsigned int);
    void wait_for_fifo(short);

    unsigned char * voodoo_regbase;
    QLinuxFb_Shared * shared_data;

private:

    int hotx;
    int hoty;

};

// Write a 32-bit graphics card register to 2d engine register block
inline void QVoodooCursor::regw(volatile unsigned int regindex,
					 unsigned long val)
{
    *((volatile unsigned long int *)(voodoo_regbase+regindex))=val;
}

inline unsigned int QVoodooCursor::regr(volatile unsigned int
					regindex)
{
    unsigned long int val;
    val=*((volatile unsigned long *)(voodoo_regbase+regindex));
    return val;
}

// Wait <entry> FIFO entries. <entry> FIFO entries must be free
// before making <entry> regw's or regw2's, or you'll lock up the
// graphics card and your computer. The total number of FIFO entries
// varies from card to card.

inline void QVoodooCursor::wait_for_fifo(short entries)
{
    QLinuxFb_Shared * tmp=(QLinuxFb_Shared *)shared_data;
    tmp->fifocount+=entries;
    if(tmp->fifocount<tmp->fifomax)
	return;

    for(int loopc=0;loopc<1000000;loopc++) {
	int fifoval=regr(VOODOOSTATUS);
	fifoval&=0x1f;
	if(fifoval==0x1f) {
	    tmp->fifocount=0;
	    return;
	}
    }

    qDebug("Wait for fifo timeout!");
}

#endif // QT_NO_QWS_CURSOR

QVoodooScreen::QVoodooScreen( int display_id  )
    : QLinuxFbScreen( display_id )
{
}

bool QVoodooScreen::useOffscreen() { return false; }

bool QVoodooScreen::connect( const QString &spec )
{
    if (!QLinuxFbScreen::connect( spec )) {
	return FALSE;
    }

    canaccel=FALSE;

    // This is the 256-byte PCI config space information for the
    // card pointed to by QWS_CARD_SLOT, as read from /proc/bus/pci
    // (or in theory from a PCI bus scan - there is some code for this
    // but it's not how Qt/Embedded would normally work)
    // It only tests the vendor ID - so don't use it with other 3dfx
    // graphics cards, such as Voodoo 4/5, or Bad Things
    // May Happen
    const unsigned char* config = qt_probe_bus();

    if(!config)
	return FALSE;

    unsigned short int * manufacturer=(unsigned short int *)config;
    if(*manufacturer!=0x121a) {
	qDebug("This does not appear to be a 3Dfx card");
	qDebug("Are you sure QWS_CARD_SLOT is pointing to the right entry in "
	       "/proc/bus/pci?");
	return FALSE;
    }

    const unsigned char * bar=config+0x10;
    const unsigned long int * addr=(const unsigned long int *)bar;
    // We expect the address pointer for the registers in config space
    // (the 1st address specified) to be a memory one, so we do a simple
    // sanity check
    unsigned long int s=*(addr+0); // First registers pointer
    unsigned long int olds=s;
    if(s & 0x1) {
#ifdef DEBUG_INIT
	printf("IO space - not right\n");
#endif
	return FALSE;
    } else {
#ifdef DEBUG_INIT
	qDebug("First address thing look right");
#endif
	s=s >> 1;
	s=s >> 2;
	s=olds;
	unsigned char * membase;
	int aperturefd;
	// We map in the registers from /dev/mem, which is memory
	// as seen from a physical-address point of view (rather than
	// the application's virtual address space) but including PCI-mapped
	// memory
	aperturefd=open("/dev/mem",O_RDWR);
	if(aperturefd==-1) {
#ifdef DEBUG_INIT
	    qDebug("Can't open /dev/mem");
#endif
	    return FALSE;
	}
	s=(s >> 4) << 4;
#ifdef DEBUG_INIT
	qDebug("Using physical address %lx, mapping %d",s,0x5fffff);
#endif
	membase=(unsigned char *)mmap(0,0x5fffff,PROT_READ |
				      PROT_WRITE,MAP_SHARED,
				      aperturefd,s);
	if(membase==0 || membase==(unsigned char *)-1) {
#ifdef DEBUG_INIT
	    qDebug("Failure to mmap /dev/mem, offset %ld, %s",s,
		   strerror(errno));
#endif
	    close(aperturefd);
	    return FALSE;
	}
	voodoo_regbase=membase;
    }

    qDebug("Detected Voodoo 3");

    // Yes, we detected the card correctly so can safely make accelerated
    // gfxen
    canaccel=TRUE;

    return TRUE;
}


QVoodooScreen::~QVoodooScreen()
{
}

// Set up some known values; unlike the Mach64 driver we rely more on
// the Linux framebuffer driver or Video BIOS to set up sensible default
// values; it seems to work

bool QVoodooScreen::initDevice()
{
    qDebug("initDevice");
    QLinuxFbScreen::initDevice();

    shared->fifomax=32;
    shared->fifocount=0;
    shared->forecol=0;

    /*
    wait_for_fifo(3);
    regw(LINESTIPPLE,0xffffffff);
    regw(LINESTYLE,0);
    regw(COMMANDEXTRA,0);
    */

    return TRUE;
}

void QVoodooScreen::shutdownDevice()
{
    QLinuxFbScreen::shutdownDevice();
}

int QVoodooScreen::initCursor(void* e, bool init)
{
#ifndef QT_NO_QWS_CURSOR
    extern bool qws_sw_cursor;

    if(qws_sw_cursor==TRUE) {
	return QLinuxFbScreen::initCursor(e,init);
    }
    qt_screencursor=new QVoodooCursor();
    qt_screencursor->init(0,FALSE);
#endif
    return 0;
}

QGfx * QVoodooScreen::createGfx(unsigned char * b,int w,int h,int d,
				int linestep)
{
    QGfx * ret=0;
    if( onCard(b) ) {
	if( d==16 ) {
	    ret = new QGfxVoodoo<16,0>(b,w,h,voodoo_regbase);
	} else if ( d==32 ) {
	    ret = new QGfxVoodoo<32,0>(b,w,h,voodoo_regbase);
	} else if ( d==8 ) {
	    ret = new QGfxVoodoo<8,0>(b,w,h,voodoo_regbase);
	}
	if(ret) {
	    ret->setShared(shared);
	    ret->setLineStep(linestep);
	    return ret;
	}
    }
    return QLinuxFbScreen::createGfx(b,w,h,d,linestep);
}

extern bool qws_accel;

#ifndef QT_NO_QWS_CURSOR

QVoodooCursor::QVoodooCursor()
{
}

QVoodooCursor::~QVoodooCursor()
{
}

void QVoodooCursor::init(SWCursorData *,bool)
{
    myoffset=(qt_screen->width()*qt_screen->height()*qt_screen->depth())/8;
    myoffset+=sizeof(int)*4;
    fb_start=qt_screen->base();
    qDebug("Cursor init");
    shared_data=((QVoodooScreen *)qt_screen)->shared;
    voodoo_regbase=((QVoodooScreen *)qt_screen)->voodoo_regbase;
    wait_for_fifo(4);
    regw(COMMANDEXTRA,0);
    regw(LINESTIPPLE,0xffffffff);
    regw(LINESTYLE,0);
    regw(COLORFORE,0);
}

// Encode RGB values into 2-bit cursor encoding - similar to
// Mach64, but the actual values are different

int voodoo_ngval(QRgb r)
{
    if(qAlpha(r)<255) {
	return 1;        // Transparent
    } else if(qBlue(r)>240) {
        return 0;        // White
    } else {
        return 2;        // Black
    }
}

void QVoodooCursor::set(const QImage& image,int hx,int hy)
{
    cursor=(QImage*)&image;
    hotx=hx;
    hoty=hy;

    if(cursor->isNull()) {
        qDebug("Null cursor image!");
	abort();
        return;
    }

    // 1k-align it
    unsigned int offset=myoffset;
    while(offset & 0x40)
	offset++;

    int loopc,loopc2;

    unsigned char * tmp;

    for(loopc=0;loopc<64;loopc++) {
	tmp=fb_start+offset+(loopc*16);
	for(loopc2=0;loopc2<8;loopc2++) {
	    *(tmp++)=0xff;
	}
	for(loopc2=0;loopc2<8;loopc2++) {
	    *(tmp++)=0x00;
	}
    }

    // Write the cursor data in the image into the weird format
    // that Voodoo3 expects for cursors, which is some truly weird
    // planar format (hence the two inner loops)
    // We assume cursors are multiples of 8 pixels wide
    for(loopc=0;loopc<cursor->height();loopc++) {
	tmp=fb_start+offset+(loopc*16);
        for(loopc2=0;loopc2<(cursor->width()/8);loopc2++) {
            unsigned int v1,v2,v3,v4,v5,v6,v7,v8;
            unsigned int pos=loopc2*8;
            v8=voodoo_ngval(cursor->pixel(pos,loopc)) & 1;
            v7=voodoo_ngval(cursor->pixel(pos+1,loopc)) & 1;
            v6=voodoo_ngval(cursor->pixel(pos+2,loopc)) & 1;
            v5=voodoo_ngval(cursor->pixel(pos+3,loopc)) & 1;
            v4=voodoo_ngval(cursor->pixel(pos+4,loopc)) & 1;
            v3=voodoo_ngval(cursor->pixel(pos+5,loopc)) & 1;
            v2=voodoo_ngval(cursor->pixel(pos+6,loopc)) & 1;
            v1=voodoo_ngval(cursor->pixel(pos+7,loopc)) & 1;
            unsigned char put=(v8 << 7) | (v7 << 6) | (v6 << 5) |
			      (v5 << 4) | (v4 << 3) | (v3 << 2) |
			      (v2 << 1) | v1;
            *(tmp++)=put;
        }
	int add=8-(cursor->width()/8);
	tmp+=add;
        for(loopc2=0;loopc2<(cursor->width()/8);loopc2++) {
            unsigned int v1,v2,v3,v4,v5,v6,v7,v8;
            unsigned int pos=loopc2*8;
            v8=voodoo_ngval(cursor->pixel(pos,loopc)) >> 1;
            v7=voodoo_ngval(cursor->pixel(pos+1,loopc)) >> 1;
            v6=voodoo_ngval(cursor->pixel(pos+2,loopc)) >> 1;
            v5=voodoo_ngval(cursor->pixel(pos+3,loopc)) >> 1;
            v4=voodoo_ngval(cursor->pixel(pos+4,loopc)) >> 1;
            v3=voodoo_ngval(cursor->pixel(pos+5,loopc)) >> 1;
            v2=voodoo_ngval(cursor->pixel(pos+6,loopc)) >> 1;
            v1=voodoo_ngval(cursor->pixel(pos+7,loopc)) >> 1;
            unsigned char put=(v8 << 7) | (v7 << 6) | (v6 << 5) |
			      (v5 << 4) | (v4 << 3) | (v3 << 2) |
			      (v2 << 1) | v1;
            *(tmp++)=put;
        }
        add=8-(cursor->width()/8);
	tmp+=add;
    }
    QRgb a=cursor->color(1);
    QRgb b=cursor->color(0);
    unsigned int c,d;
    c=(qRed(a) << 16) | (qGreen(a) << 8) | (qBlue(a) << 0);
    d=(qRed(b) << 16) | (qGreen(b) << 8) | (qBlue(b) << 0);
    //wait_for_fifo(3);
    regw(HWCURC0,c);
    regw(HWCURC1,d);
    regw(HWCURPATADDR,offset);
    show();
}

// Make the accelerated cursor disappear
void QVoodooCursor::hide()
{
    unsigned int cntlstat=regr(VIDPROCCFG);
    cntlstat=cntlstat & ~0x08000000;
    //wait_for_fifo(1);
    regw(VIDPROCCFG,cntlstat);
}

// Make it come back
void QVoodooCursor::show()
{
    unsigned int cntlstat=regr(VIDPROCCFG);
    cntlstat=cntlstat | 0x08000000;
    cntlstat=cntlstat & ~0x2;
    //wait_for_fifo(1);
    regw(VIDPROCCFG,cntlstat);
}

// Move it to x,y, such that the hotspot is at x,y
void QVoodooCursor::move(int x,int y)
{
    x-=hotx;
    y-=hoty;
    x+=64;
    y+=64;
    unsigned int hold=x | (y << 16);
    //wait_for_fifo(1);
    regw(HWCURLOC,hold);
}

#endif // QT_NO_QWS_CURSOR

#endif // QT_NO_QWS_VOODOO3
