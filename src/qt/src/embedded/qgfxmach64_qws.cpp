/****************************************************************************
**
** Implementation of QGfxMach64 (graphics context) class for Mach64 cards
*
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

#include "qgfxraster_qws.h"

#ifndef QT_NO_QWS_MACH64

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "qgfxmach64_qws.h"
#include "qgfxmach64defs_qws.h"
#include <qapplication.h>

// #define DEBUG_INIT

#ifndef __sparc__
#include <sys/io.h>
#endif

bool no3d=false;

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

// regbase points to the first set of Mach64 registers (the 2d drawing engine)
// regbase2 points to the second set (3d engine/scaler pipeline)
static unsigned char * regbase=0;
static unsigned char * regbase2=0;

// Wait for all FIFO entries to become free (so we know no commands are
// going to be queued up) then wait for the currently executing graphics
// command to finish. Used by sync()

template <const int depth, const int type>
class QGfxMach64 : public QGfxRaster<depth,type> {

public:

    QGfxMach64(unsigned char *,int w,int h);

    virtual void drawLine(int,int,int,int);
    virtual void fillRect(int,int,int,int);
    virtual void blt( int,int,int,int,int,int );
#if !defined(QT_NO_MOVIE) || !defined(QT_NO_TRANSFORMATIONS)
    virtual void stretchBlt( int,int,int,int,int,int );
#endif
    virtual void tiledBlt(int,int,int,int);

    virtual void drawAlpha(int,int,int,int,int,int,int,int);

    virtual void sync();

    void hsync(int);

    void setPixWidth(int,int,int=16,bool=true);

    void wait_for_fifo(short);
    unsigned int regr(volatile unsigned int);
    void regw(volatile unsigned int,unsigned long);
    void regw2(volatile unsigned int,unsigned long);
    void regwf2(volatile unsigned int,float);
    unsigned int regr2(volatile unsigned int);
    void reset_engine();
    void wait_for_idle();

private:

    unsigned char * regbase;
    unsigned char * regbase2;

    void do_scissors(QRect &);

    bool checkSourceDest();
    void setDest();

};

// Read a 32-bit graphics card register from 2d engine register block
template<const int depth,const int type>
inline unsigned int QGfxMach64<depth,type>::regr(volatile unsigned int
						 regindex)
{
    unsigned long int val;
    val=*((volatile unsigned long *)(regbase+regindex));
    return val;
}

// Write a 32-bit graphics card register to 2d engine register block
template<const int depth,const int type>
inline void QGfxMach64<depth,type>::regw(volatile unsigned int regindex,
					 unsigned long val)
{
    *((volatile unsigned long int *)(regbase+regindex))=val;
}

// Needed by card init function. This is non-ideal
inline unsigned int regr(volatile unsigned int regindex)
{
    unsigned long int val;
    val=*((volatile unsigned long *)(regbase+regindex));
    return val;
}

inline void regw(volatile unsigned int regindex,
					 unsigned long val)
{
    *((volatile unsigned long int *)(regbase+regindex))=val;
}

// Write a 32-bit graphics card register to 3d engine register block
template<const int depth,const int type>
inline void QGfxMach64<depth,type>::regw2(volatile unsigned int regindex,
					  unsigned long val)
{
    if(no3d)
	abort();
    *((volatile unsigned long int *)(regbase2+regindex))=val;
}

// Write a 32-bit floating point value to 3d engine register block
template<const int depth,const int type>
inline void QGfxMach64<depth,type>::regwf2(volatile unsigned int regindex,
					   float val)
{
    if(no3d)
	abort();
    unsigned int writeval;
    *((float *)&writeval)=val;
    *((volatile unsigned long int *)(regbase2+regindex))=writeval;
}

// Read a 32-bit value from 3d engine register block
template<const int depth,const int type>
inline unsigned int QGfxMach64<depth,type>::regr2(volatile unsigned int
						  regindex)
{
    if(no3d)
	abort();
    unsigned long int val;
    val=*((volatile unsigned long *)(regbase2+regindex));
    return val;
}

// Wait <entry> FIFO entries. <entry> FIFO entries must be free
// before making <entry> regw's or regw2's, or you'll lock up the
// graphics card and your computer. The total number of FIFO entries
// varies from card to card. There's some code to attempt to recover
// from engine lockups

template<const int depth,const int type>
inline void QGfxMach64<depth,type>::wait_for_fifo(short entries)
{
    QLinuxFb_Shared * tmp=(QLinuxFb_Shared *)this->shared_data;
    tmp->fifocount+=entries;
    if(tmp->fifocount<tmp->fifomax)
	return;

    int trycount=0;

    while(trycount++) {
	int fifoval=regr(FIFO_STAT);
	if(fifoval & 0x80000000) {
	    qDebug("Resetting engine");
            wait_for_fifo(1);
            regw(GEN_TEST_CNTL,(regr(GEN_TEST_CNTL) & 0xfffffeff));
	    wait_for_fifo(1);
	    regw(GEN_TEST_CNTL,(regr(GEN_TEST_CNTL) | 0x00000100));
	    wait_for_fifo(1);
	    regw(BUS_CNTL,regr(BUS_CNTL) | 0x08a00000);
	    return;
	}
	fifoval=fifoval & 0xffff;
	int loopc;
	int count=0;
	for(loopc=0;loopc<16;loopc++) {
	    if(!(fifoval & 0x1))
		count++;
	    fifoval=fifoval >> 1;
	}
	if(count>=tmp->fifomax) {
	    tmp->fifocount=0;
	    return;
	}
	if(trycount>10) {
	    qDebug("Resetting engine");
            wait_for_fifo(1);
            regw(GEN_TEST_CNTL,(regr(GEN_TEST_CNTL) & 0xfffffeff));
	    wait_for_fifo(1);
	    regw(GEN_TEST_CNTL,(regr(GEN_TEST_CNTL) | 0x00000100));
	    wait_for_fifo(1);
	    regw(BUS_CNTL,regr(BUS_CNTL) | 0x08a00000);
	}
    }
}


template<const int depth,const int type>
inline void QGfxMach64<depth,type>::reset_engine()
{
    // We use wait_for_fifo(1)'s in case the fifo queue has bunged up
    // for some reason; this is safer
    wait_for_fifo(1);
    regw(GEN_TEST_CNTL,(regr(GEN_TEST_CNTL) & 0xfffffeff));
    wait_for_fifo(1);
    regw(GEN_TEST_CNTL,(regr(GEN_TEST_CNTL) | 0x00000100));
    wait_for_fifo(1);
    regw(BUS_CNTL,regr(BUS_CNTL) | 0x08a00000);
}

template<const int depth,const int type>
inline void QGfxMach64<depth,type>::wait_for_idle()
{
    wait_for_fifo(16);

    int loopc;

    for(loopc=0;loopc<1000000;loopc++) {
	if(((regr(GUI_STAT) & 0x1)==0) && loopc>100)
	    return;
    }

    qDebug("Wait for idle timeout!");
    reset_engine();
}

#define vgabase 0x1000

template<const int depth,const int type>
QGfxMach64<depth,type>::QGfxMach64(unsigned char * a,int b,int c)
    : QGfxRaster<depth,type>(a,b,c)
{

    regbase=::regbase;
    regbase2=::regbase2;
}

// Sets up the graphics engine hardware scissors - these cut off
// hardware graphics operations that go outside their borders and
// can be used to implement clipping
template<const int depth,const int type>
inline void QGfxMach64<depth,type>::do_scissors(QRect & r)
{
    QLinuxFb_Shared * tmp=(QLinuxFb_Shared *)this->shared_data;
    if(tmp->clipleft!=r.left()) {
      tmp->clipleft=r.left();
      wait_for_fifo(1);
      regw(SC_LEFT,r.left());
    }
    if(tmp->cliptop!=r.top()) {
      tmp->cliptop=r.top();
      wait_for_fifo(1);
      regw(SC_TOP,r.top());
    }
    if(tmp->clipright!=r.right()) {
      tmp->clipright=r.right();
      wait_for_fifo(1);
      regw(SC_RIGHT,r.right());
    }
    if(tmp->clipbottom!=r.bottom()) {
      tmp->clipbottom=r.bottom();
      wait_for_fifo(1);
      regw(SC_BOTTOM,r.bottom());
    }
}

// Sets up the graphics engine's idea of bits-per-pixel for destination
// and different sources (normal 2d source and scaler/3d engine source)
// Can also handle different endianness - that isn't really used by the
// current example but could be useful for a LinuxPPC port of the driver,
// for example

template<const int depth,const int type>
inline void QGfxMach64<depth,type>::setPixWidth(int d,int s,int sc,bool b)
{
    unsigned int wmask;
    if(b) {
        wmask=BYTE_ORDER_LSB_TO_MSB;
    } else {
	wmask=BYTE_ORDER_MSB_TO_LSB;
    }
    if(d==16) {
	wmask|=DST_16BPP;
    } else if(d==8) {
	wmask|=DST_8BPP;
    } else {
	// 32
	wmask|=DST_32BPP;
    }
    if(s==16) {
	wmask|=SRC_16BPP | HOST_16BPP;
    } else if(s==8) {
	wmask|=SRC_8BPP | HOST_8BPP;
    } else if(s==1) {
	wmask|=SRC_1BPP | HOST_1BPP;
    } else {
	// 32
	wmask|=SRC_32BPP | HOST_32BPP;
    }
    if(sc==16) {
	wmask|=SCALE_16BPP;
    } else if(sc==8) {
	wmask|=SCALE_8BPP;
    } else if(sc==1) {
	wmask|=SCALE_1BPP;
    } else {
	wmask|=SCALE_32BPP;
    }
    wait_for_fifo(1);
    regw(DP_PIX_WIDTH,wmask);
}

// Set up SRC_OFF_PITCH and DST_OFF_PITCH
// Return true if they're both on the card
// (That's the source and destination's offset from graphics memory
// start and linestep) - both are specified as multiples of 64 bits
// This is also used to verify that both the source and destination
// of the operation are on the graphics card - if not then a fallback
// to the software rasterising code is necessary

template<const int depth,const int type>
inline bool QGfxMach64<depth,type>::checkSourceDest()
{
    setDest();

    int sourcepixelpitch;
    ulong src_buffer_offset;
    if (this->srctype == QGfx::SourcePen) {
	src_buffer_offset = -1;
	return FALSE;
    } else {
	if (!this->gfx_screen->onCard(this->srcbits,src_buffer_offset)) {
	    return FALSE;
	}
	if(src_buffer_offset & 0x7) {
	    qDebug("Unaligned offset %lx",src_buffer_offset);
	    return FALSE;
	}
	sourcepixelpitch=(this->srclinestep*8)/this->srcdepth;
	wait_for_fifo(1);
	regw(SRC_OFF_PITCH,( (sourcepixelpitch / 8 ) << 22) |
	     (src_buffer_offset / 8) );
    }

    return TRUE;
}

// Set up DST_OFF_PITCH, return false if it's not on the card
// For lines, filled rects etc
// This is similar to checkSourceDest but is used when there is no
// source image data

template<const int depth,const int type>
inline void QGfxMach64<depth,type>::setDest()
{
    ulong buffer_offset;
    this->gfx_screen->onCard(this->buffer,buffer_offset);
    QLinuxFb_Shared * tmp=(QLinuxFb_Shared *)this->shared_data;
    if(tmp->buffer_offset==buffer_offset && tmp->linestep==this->linestep()) {
      return;
    } else {
      tmp->buffer_offset=buffer_offset;
      tmp->linestep=this->linestep();
    }

    int pixelstep=(this->linestep()*8)/depth;
    setPixWidth(depth,depth);
    wait_for_fifo(1);
    regw(DST_OFF_PITCH,(( pixelstep / 8 ) << 22) | (buffer_offset / 8));
}

template<const int depth,const int type>
void QGfxMach64<depth,type>::drawLine(int x1,int y1,int x2,int y2)
{
    // No point going any further if the window isn't visible
    if(this->ncliprect<1)
        return;

    // Only handle 'normal' lines
    if ( this->cpen.style() != QGfx::SolidLine || this->myrop!=Qt::CopyROP ) {
	QGfxRaster<depth,type>::drawLine(x1,y1,x2,y2);
	return;
    }

    // Stop anyone else trying to access optype/lastop/the graphics engine
    // to avoid synchronization problems with other processes

    // Figure out distance between endpoints
    int dx,dy;
    dx=abs(x2-x1);
    dy=abs(y2-y1);

    GFX_START(QRect(x1, y1 < y2 ? y1 : y2, dx+1, QABS(dy)+1))

    if((*this->gfx_optype)!=1 || (*this->gfx_lastop)!=LASTOP_LINE) {
	setDest();
	// The scaler engine operates independently of the 2d engine
	// so we need to wait for it to finish if it's doing something
	if((*this->gfx_optype)>1)
	    wait_for_idle();

	// This is avoided if the last operation was a line
	wait_for_fifo(2);
	regw(DP_SRC,0x00000100);
	regw(DP_MIX,(MIX_SRC << 16) | MIX_DST);

	(*this->gfx_lastop)=LASTOP_LINE;
    }

    // Note that the last operation used the 2d engine
    (*this->gfx_optype)=1;

    int loopc;

    // Add the offset of the gfx - used to make the origin the right
    // place for windows
    x1=x1+this->xoffs;
    y1=y1+this->yoffs;
    x2=x2+this->xoffs;
    y2=y2+this->yoffs;

    // Only cope with lines going from left to right
    // - swap them round if this isn't true
    if(x1>x2) {
      int x3,y3;
      x3=x2;
      y3=y2;
      x2=x1;
      y2=y1;
      x1=x3;
      y1=y3;
      x1=x3;
      y1=y3;
    }

    wait_for_fifo(1);

#ifndef QT_NO_QWS_REPEATER
    QScreen * tmp=qt_screen;
    qt_screen=this->gfx_screen;
#endif
    QColor c=this->cpen.color();
    unsigned int tmpcol=c.alloc();
#ifndef QT_NO_QWS_REPEATER
    qt_screen=tmp;
#endif

    regw(DP_FRGD_CLR,tmpcol);

    // The clip region is defined as a series of rectangles
    // We repeatedly set up the hardware clip rectangle to one of
    // these rectangles and re-draw the line - an alternative approach
    // would be to clip to the rectangle in software

    for(loopc=0;loopc<this->ncliprect;loopc++) {
	// Code taken from Mach64 Programmer's Manual
        // Sets up Bresenham parameters
	int mindelta,maxdelta;
	int xdir,ydir,ymajor;

	mindelta=dx>dy ? dy : dx;      // min
	maxdelta=dx>dy ? dx : dy;	   // max
	if(x1<x2)
	    xdir=1;
	else
	    xdir=0;
	if(y1<y2)
	    ydir=0x0802;
	else
	    ydir=0;
	if(dx<dy)
	    ymajor=4;
	else
	    ymajor=0;

	unsigned int rval=0x00000003;
	rval=(rval & ~0x7) | (unsigned long)(ymajor | ydir | xdir);

	do_scissors(this->cliprect[loopc]);

	wait_for_fifo(6);
	regw(DST_CNTL,rval);
	regw(DST_Y_X,((unsigned long)x1 << 16) | y1);
	regw(DST_BRES_ERR,(2*mindelta)-maxdelta);
	regw(DST_BRES_INC,2*mindelta);
	regw(DST_BRES_DEC,2*(mindelta-maxdelta));
	regw(DST_BRES_LNTH,maxdelta+1);
    }

    // Software mouse cursor stuff
    GFX_END

    // Release display again - not doing so will cause Qt/Embedded applications
    // to deadlock
#if !defined(QT_NO_QWS_MULTIPROCESS) && !defined(QT_PAINTER_LOCKING)
    QWSDisplay::ungrab();
#endif

}

template<const int depth,const int type>
void QGfxMach64<depth,type>::fillRect(int rx,int ry,int w,int h)
{
    if(this->ncliprect<1) {
	return;
    }

    if( (this->cbrush.style()!=QBrush::NoBrush) && (this->cbrush.style()!=QBrush::SolidPattern) ) {
	QGfxRaster<depth,type>::fillRect(rx,ry,w,h);
	return;
    }

    if(this->myrop!=Qt::CopyROP) {
	QGfxRaster<depth,type>::fillRect(rx,ry,w,h);
	return;
    }

    GFX_START(QRect(rx+this->xoffs, ry+this->yoffs, w+1, h+1))
    setDest();

    if((*this->gfx_optype)!=1 || (*this->gfx_lastop)!=LASTOP_RECT) {

	// probably not needed
	// we reset the clip rectangle because we do our own software
	// clipping (rectangle<->rectangle intersections are fast)

        QRect tmprect(0,0,this->width,this->height);
        do_scissors(tmprect);

        wait_for_fifo(3);
        regw(DP_SRC,0x00000100);
        regw(DP_MIX,(MIX_SRC << 16) | MIX_DST);
        regw(DST_CNTL,0x00000003);

	(*this->gfx_lastop)=LASTOP_RECT;
    }

    if((*this->gfx_optype)>1)
	sync();
    (*this->gfx_optype)=1;

    int loopc;

    int xp=rx+this->xoffs;
    int yp=ry+this->yoffs;

    int x2,y2;

    x2=xp+(w-1);
    y2=yp+(h-1);

    int x3,y3,x4,y4;

    QColor tmp=this->cbrush.color();
    wait_for_fifo(1);

#ifndef QT_NO_QWS_REPEATER
    QScreen * tmp2=qt_screen;
    qt_screen=this->gfx_screen;
#endif
    regw(DP_FRGD_CLR,tmp.alloc());
#ifndef QT_NO_QWS_REPEATER
    qt_screen=tmp2;
#endif

    if(this->cbrush.style()!=QBrush::NoBrush) {
	int p=this->ncliprect;
	if(p<8) {
	    // We can wait for all our fifos at once
	    // (slight performance optimisation)
	    wait_for_fifo(p*2);
	    for(loopc=0;loopc<p;loopc++) {
		QRect r=this->cliprect[loopc];
		// Clip rectangle to current clip rectangle
		if(xp<=r.right() && yp<=r.bottom() &&
		   x2>=r.left() && y2>=r.top()) {
		    x3=r.left() > xp ? r.left() : xp;
		    y3=r.top() > yp ? r.top() : yp;
		    x4=r.right() > x2 ? x2 : r.right();
		    y4=r.bottom() > y2 ? y2 : r.bottom();
		    regw(DST_Y_X,(x3 << 16) | y3);
		    regw(DST_HEIGHT_WIDTH,( ( (x4-x3) +1) << 16) |
			 ( (y4-y3) +1));
		}
	    }
	} else {
	    for(loopc=0;loopc<p;loopc++) {
		QRect r=this->cliprect[loopc];
		if(xp<=r.right() && yp<=r.bottom() &&
		   x2>=r.left() && y2>=r.top()) {
		    x3=r.left() > xp ? r.left() : xp;
		    y3=r.top() > yp ? r.top() : yp;
		    x4=r.right() > x2 ? x2 : r.right();
		    y4=r.bottom() > y2 ? y2 : r.bottom();
		    wait_for_fifo(2);
		    regw(DST_Y_X,(x3 << 16) | y3);
		    regw(DST_HEIGHT_WIDTH,( ( (x4-x3) +1) << 16) |
			 ( (y4-y3) +1));
		}
	    }
	}
    }

    GFX_END


}

template<const int depth,const int type>
inline void QGfxMach64<depth,type>::blt(int rx,int ry,int w,int h,int sx, int sy)
{
    if(this->ncliprect<1)
	return;

    // We have no provision for cacheing these things in graphics card
    // memory at the moment
    if(this->alphatype==QGfx::BigEndianMask || this->alphatype==QGfx::LittleEndianMask ||
       this->alphatype==QGfx::SeparateAlpha || this->srctype==QGfx::SourcePen || (this->myrop!=Qt::CopyROP) ) {
	QGfxRaster<depth,type>::blt(rx,ry,w,h,sx,sy);
    }

    bool canaccel=false;

    // We can only handle inline/solid alpha values using the 3d pipeline
    // and a texture. Textures have to be a power of 2 in sizes so the
    // pixmap has to be the right size in order for this to be accelerated.
    // Also, only 32 bit source textures are alpha-blended in hardware
    if(this->srcdepth==32) {
	if(this->alphatype==QGfx::IgnoreAlpha || this->alphatype==QGfx::SolidAlpha ||
	   this->alphatype==QGfx::InlineAlpha) {
	    canaccel=true;
	}

	if(this->alphatype!=QGfx::IgnoreAlpha) {
	    // Mach64 requires textures to be these sizes
	    int p=this->srclinestep/4;
	    if(p!=1024 && p!=512 && p!=256 && p!=128 && p!=64 && p!=32 &&
	       p!=16 && p!=8) {
		canaccel=false;
	    }
	}
    } else if(this->srcdepth==16 || this->srcdepth==8) {
	if(this->alphatype==QGfx::IgnoreAlpha) {
	    canaccel=true;
	}
    }

    if(this->srctype==QGfx::SourceImage && canaccel==false) {
	QGfxRaster<depth,type>::blt(rx,ry,w,h,sx,sy);
	return;
    }

    // We can't handle 'real' alpha blending with a solid pen source
    // (this would be a separate alpha channel, as in anti-aliased text,
    // or possibly a solid alpha value)
    if(this->srctype==QGfx::SourcePen && !(this->alphatype==QGfx::BigEndianMask ||
			       this->alphatype==QGfx::LittleEndianMask) ) {
	QGfxRaster<depth,type>::blt(rx,ry,w,h,sx,sy);
	return;
    }

    if( (this->srcdepth!=32) && (this->srcdepth!=16) && (this->srcdepth!=8) ) {
	QGfxRaster<depth,type>::blt(rx,ry,w,h,sx,sy);
	return;
    }

    QRect cursRect(rx, ry, w+1, h+1);
    GFX_START(cursRect);
    bool check_result=checkSourceDest();

    if( (this->alphatype==QGfx::InlineAlpha || this->alphatype==QGfx::SolidAlpha)
	&& check_result ) {
	int x2=(rx+w)-1;
	int y2=(ry+h)-1;

	// This is special handling for using the 3d engine for
	// hardware accelerated alpha blending
	drawAlpha(rx,ry,x2,ry,rx,y2,x2,y2);
	return;
    }

    if(check_result) {

        // This is now a normal 2d engine blt

	int xp=this->xoffs+rx;
	int yp=this->yoffs+ry;
	int xp2=this->srcwidgetoffs.x() + sx;
	int yp2=this->srcwidgetoffs.y() + sy;

	if(this->srctype==QGfx::SourceImage) {

	    if((*this->gfx_optype)!=1 || (*this->gfx_lastop)!=LASTOP_BLT) {
		(*this->gfx_lastop)=LASTOP_BLT;
		wait_for_fifo(8);
		// Write to all bits of the pixel
		regw(DP_WRITE_MASK,0xffffffff);
		// CopyROP
		regw(DP_MIX,0x00070003);
		regw(DP_SRC,0x00000300);
		regw(CLR_CMP_CNTL,0x00000000);
		regw(DP_FRGD_CLR,0xffffffff);
	    }

	    // Set up graphics engine's idea of source and destination
	    // pixel sizes
	    setPixWidth(depth,this->srcdepth);
	    // Tell the engine whether to copy bits from left to right,
	    // top to bottom, right to left, bottom to top - this is
	    // important for getting the right results with an overlapping
	    // blt
	    if(yp>yp2) {
		// Down, reverse
		if(xp>xp2) {
		    // Right, reverse
		    xp+=(w-1);
		    xp2+=(w-1);
		    yp+=(h-1);
		    yp2+=(h-1);
		    regw(DST_CNTL,0x00000000);
		} else {
		    // Left, normal
		    yp+=(h-1);
		    yp2+=(h-1);
		    regw(DST_CNTL,0x00000001);
		}
	    } else {
		// Up, normal
		// Down, reverse
		if(xp>xp2) {
		    // Right, reverse
		    xp+=(w-1);
		    xp2+=(w-1);
		    regw(DST_CNTL,0x00000002);
		} else {
		    // Left, normal
		    regw(DST_CNTL,0x00000003);
		}
	    }
	    regw(SRC_CNTL,0x00000000);
	    regw(SRC_Y_X,(xp2 << 16) | yp2);
	} else {
	    // This is used for drawing a solid colour with a mask -
	    // used for brushes
	    if((*this->gfx_optype)!=1 || (*this->gfx_lastop)!=LASTOP_BLTPEN) {
		setPixWidth(depth,this->srcdepth,16,this->alphatype==QGfx::LittleEndianMask);
		QColor tmp=this->cpen.color();
		wait_for_fifo(8);
		regw(DP_WRITE_MASK,0xffffffff);
#ifndef QT_NO_QWS_REPEATER
		QScreen * tmp2=qt_screen;
		qt_screen=this->gfx_screen;
#endif
		regw(DP_FRGD_CLR,tmp.alloc());
#ifndef QT_NO_QWS_REPEATER
		qt_screen=tmp2;
#endif
		regw(DP_MIX,0x00070003);
		regw(DP_SRC,0x00030100);
		regw(CLR_CMP_CNTL,0x00000000);
		regw(SRC_CNTL,0x00000004);
		regw(DST_CNTL,0x00000003);
		regw(SRC_Y_X,0);
		(*this->gfx_lastop)=LASTOP_BLTPEN;
	    }
	}

	(*this->gfx_optype)=1;

	int loopc;
	for(loopc=0;loopc<this->ncliprect;loopc++) {

  	    // Now it's all set up, repeatedly set the scissors
	    // and perform the blt
	    do_scissors(this->cliprect[loopc]);

	    wait_for_fifo(3);
	    regw(SRC_WIDTH1,w);
	    regw(DST_Y_X,(xp << 16) | yp);
	    regw(DST_HEIGHT_WIDTH,(w << 16) | h);
	}

	GFX_END

	return;
    } else {
	GFX_END
	// software fallback
	QGfxRaster<depth,type>::blt(rx,ry,w,h,sx,sy);
    }
}

// A blt from one rectangle to another where each rectangle can be a different
// size - the image is stretched or shrunk to fit
// Overlapping is forbidden, so are clever alpha blending modes

#if !defined(QT_NO_MOVIE) || !defined(QT_NO_TRANSFORMATIONS)
template<const int depth,const int type>
void QGfxMach64<depth,type>::stretchBlt(int rx,int ry,int w,int h,
					int sw,int sh)
{
    // This doesn't use the 2d engine, it uses the 3d/scaler pipeline
    if(this->ncliprect<1)
	return;

    if(this->srctype!=QGfx::SourceImage) {
	QGfxRaster<depth,type>::stretchBlt(rx,ry,w,h,sw,sh);
	return;	
    }

    QRect cursRect(rx, ry, w+1, h+1);
    GFX_START(cursRect);

    if ( this->srctype!=QGfx::SourceImage || !checkSourceDest() ) {
	GFX_END
	QGfxRaster<depth,type>::stretchBlt(rx,ry,w,h,sw,sh);
	return;
    }

    (*this->gfx_lastop)=LASTOP_STRETCHBLT;

    if((*this->gfx_optype)!=2)
	wait_for_idle();
    (*this->gfx_optype)=2;

    int xp=this->xoffs+rx;
    int yp=this->yoffs+ry;

    int loopc;
    for(loopc=0;loopc<this->ncliprect;loopc++) {

	wait_for_fifo(3);
	regw(SCALE_3D_CNTL,0x00000040);
	regw(ALPHA_TEST_CNTL,0);
	regw(TEX_CNTL,0);
	if(this->srclinestep<1)
	    abort();

	// We need the offset from the start of video memory for a
	// different register than SRC_BUFFER_OFFSET
	unsigned long my_src_buffer_offset;
	if (!this->gfx_screen->onCard(this->srcbits,my_src_buffer_offset))
	    qFatal("checkSourceDest() lied!");

	int srcpixelpitch=this->srclinestep;
	srcpixelpitch=(srcpixelpitch*8)/this->srcdepth;
	srcpixelpitch=srcpixelpitch >> 3;
	srcpixelpitch=srcpixelpitch << 3;

	// Need to convert source width/width to
	// fixed-point 8.12 floating point format
	double tmp1=w;
	double tmp2=sw;
	double tmp3=h;
	double tmp4=sh;
	double screentosource=tmp4/tmp3;
	tmp1=tmp2/tmp1;
	tmp3=tmp4/tmp3;


	int mul=4096;
	tmp1*=mul;
	tmp3*=mul;
	unsigned long int h1=(unsigned long int)tmp1;
	unsigned long int h2=(unsigned long int)tmp3;

	// This is a workaround for a bug with hardware clipping -
	// if the stretchblt starts above the hardware clip
	// rectangle /none/ of it is drawn
	// therefore we twiddle the y starting position of the buffer
	// to put it inside the clip rectangle

	int sy1=this->cliprect[loopc].top();

	double mytmp=sy1-yp;
	mytmp*=screentosource;

	int add=(int)mytmp;
	add=add*this->srclinestep;

	my_src_buffer_offset+=add;

	if(my_src_buffer_offset & 0x7)
	    abort();

	wait_for_fifo(10);

	regw(SCALE_OFF,my_src_buffer_offset);  // Scaler source
	regw(SECONDARY_SCALE_OFF_ACC,srcpixelpitch);
	regw(SCALE_PITCH,srcpixelpitch);
	regw(SECONDARY_SCALE_PITCH,srcpixelpitch);

	regw(SCALE_WIDTH,sw);
	regw(SCALE_HEIGHT,sh);
	regw(SCALE_HACC,0x00000000);
	regw(SCALE_VACC,0x00000000);
	regw(SECONDARY_SCALE_HACC,0x00000000);
	regw(SECONDARY_SCALE_VACC,0x00000000);

	setPixWidth(depth,this->srcdepth);

	wait_for_fifo(9);
	regw(SCALE_X_INC,h1 << 4);
	regw(SECONDARY_SCALE_X_INC,h1 << 4);
	regw(SCALE_Y_INC,h2 << 4);
	regw(SECONDARY_SCALE_Y_INC,h2 << 4);

	regw(DP_SRC,0x00000505);
	regw(DP_WRITE_MASK,0xffffffff);
	regw(DP_MIX,0x00070003);
	regw(GUI_TRAJ_CNTL,0x00800003);

	wait_for_fifo(6);
	regw(CLR_CMP_CNTL,0x02000000);
	regw(DST_CNTL,0x00000003);

	do_scissors(this->cliprect[loopc]);

	regw(DST_X,xp);
	regw(DST_Y,sy1);
	regw(DST_HEIGHT,h);
	regw(DST_WIDTH,w);

	wait_for_idle();
	usleep(1);
	reset_engine();
    }
    GFX_END
}
#endif


// This is called by the software renderer when it's about to draw
// something - it needs to be sure that the hardware engine has finished
// drawing since otherwise the two graphics operations could collide
template<const int depth,const int type>
void QGfxMach64<depth,type>::sync()
{
    wait_for_idle();
}

// Like scroll, this is here for special optimisation. This time we
// make use of the hardware to handle it - this saves calling blt()
// for each individual tile. It might be possible to get even cleverer
// with hardware that supports a tiled blt in hardware
// We also support it for solid-colour-with-mask, this accelerates
// filled rectangles with a patterned brush, used for, e.g.,
// Windows scrollbars

template<const int depth,const int type>
void QGfxMach64<depth,type>::tiledBlt(int rx,int ry,int w,int h)
{
    if(this->ncliprect<1) {
	return;
    }

    if(this->srctype==QGfx::SourceImage && (this->alphatype!=QGfx::IgnoreAlpha) ) {
	QGfxRaster<depth,type>::tiledBlt(rx,ry,w,h);
	return;
    }

    if( (this->srcdepth!=16 && this->srcdepth!=32 && this->srcdepth!=8)
	  && this->srctype==QGfx::SourceImage ) {
	QGfxRaster<depth,type>::tiledBlt(rx,ry,w,h);
	return;
    }

    if ( this->srctype==QGfx::SourcePen ) {
	QGfxRaster<depth,type>::tiledBlt(rx,ry,w,h);
	return;
    }

    QRect cursRect(rx, ry, w+1, h+1);
    GFX_START(cursRect);

    if ( this->srctype==QGfx::SourceImage && !checkSourceDest() ) {
	GFX_END
	QGfxRaster<depth,type>::tiledBlt(rx,ry,w,h);
	return;
    }


    int xp=this->xoffs+rx;
    int yp=this->yoffs+ry;

    if(this->srctype==QGfx::SourceImage) {
	if((*this->gfx_optype)!=1 || (*this->gfx_lastop)!=LASTOP_TILEDBLT) {
	    wait_for_fifo(4);
	    regw(SRC_CNTL,0x00000003);
	    regw(DP_WRITE_MASK,0xffffffff);
	    regw(CLR_CMP_CNTL,0x00000000);
	    regw(DST_CNTL,0x00000003);
	    if(this->srcdepth==16 || this->srcdepth==32 || this->srcdepth==8) {
		setPixWidth(depth,this->srcdepth);
		wait_for_fifo(3);
		regw(DP_MIX,0x00070003);
		regw(DP_SRC,0x00000300);
		regw(DP_FRGD_CLR,0xffffffff);
	    } else {
		setPixWidth(depth,1,1,!this->src_little_endian);
		wait_for_fifo(4);
		regw(DP_MIX,0x00070007);
		regw(DP_SRC,0x00030100);
		regw(DP_FRGD_CLR,this->srcclut[0]);
		regw(DP_BKGD_CLR,this->srcclut[1]);
	    }
	    (*this->gfx_lastop)=LASTOP_TILEDBLT;
	}
    } else {
	if((*this->gfx_optype)!=1 || (*this->gfx_lastop)!=LASTOP_TILEDBLTPEN) {
	    setPixWidth(depth,1,1,this->alphatype==QGfx::LittleEndianMask);
	    wait_for_fifo(7);
	    regw(SRC_CNTL,0x00000001);
	    regw(DP_WRITE_MASK,0xffffffff);
	    QColor tmp=this->cpen.color();
#ifndef QT_NO_QWS_REPEATER
	    QScreen * tmp2=qt_screen;
	    qt_screen=this->gfx_screen;
#endif
	    regw(DP_FRGD_CLR,tmp.alloc());
#ifndef QT_NO_QWS_REPEATER
	    qt_screen=tmp2;
#endif
	    regw(DP_MIX,0x00070003);
	    regw(DP_SRC,0x00030100);
	    regw(CLR_CMP_CNTL,0x00000000);
	    regw(DST_CNTL,0x00000003);
	    (*this->gfx_lastop)=LASTOP_TILEDBLTPEN;
	}
    }

    (*this->gfx_optype)=1;

    QRect tmprect(0,0,this->width,this->height);
    do_scissors(tmprect);

    int xp2,yp2;
    int xp3,yp3;
    xp3=(xp+w)-1;
    yp3=(yp+h)-1;

    int loopc;
    for(loopc=0;loopc<this->ncliprect;loopc++) {
	xp2=this->srcwidgetoffs.x() + this->brushoffs.x();
	yp2=this->srcwidgetoffs.y() + this->brushoffs.y();

	QRect r=this->cliprect[loopc];
	int myxp=xp > r.left() ? xp : r.left();
	int myyp=yp > r.top() ? yp : r.top();
	int myxp2=xp3 > r.right() ? r.right() : xp3;
	int myyp2=yp3 > r.bottom() ? r.bottom() : yp3;

	int ww2=(myxp2-myxp)+1;
	int hh2=(myyp2-myyp)+1;

	int xo=(myxp-xp);
	int yo=(myyp-yp);

	xp2+=xo;
	yp2+=yo;

	while(xp2<0) {
	    xp2+=this->srcwidth;
	}
	while(yp2<0) {
	    yp2+=this->srcheight;
	}

	while(xp2>this->srcwidth)
	    xp2-=this->srcwidth;

	while(yp2>this->srcheight)
	    yp2-=this->srcheight;

	if(ww2>0 && hh2>0) {
	    wait_for_fifo(6);
	    regw(SRC_Y_X_START,0);
	    regw(SRC_HEIGHT2_WIDTH2,(this->srcwidth << 16) | this->srcheight);
	    regw(SRC_HEIGHT1_WIDTH1,( (this->srcwidth-xp2) << 16 ) | this->srcheight-yp2);
	    regw(SRC_Y_X,(xp2 << 16) | yp2);
	    regw(DST_Y_X,(myxp << 16) | myyp);
	    regw(DST_HEIGHT_WIDTH,(ww2 << 16) | hh2);
	}
    }

    GFX_END

}

// Wait for horizontal sync at line i - can be used to reduce flickering
// (once the CRT gun reaches line i you have almost a whole vsync period
// to do things to the part of the framebuffer just above before it gets
// updated on screen)

template<const int depth,const int type>
void QGfxMach64<depth,type>::hsync(int i)
{
    int loopc;
    int tmp;
    for(loopc=0;loopc<100000;loopc++) {
	tmp=regr(CRTC_VLINE_CRNT_VLINE);
	tmp=(tmp & 0x07ff0000) >> 16;
	if(tmp>i)
	    return;
    }
}

// Draw a quadrilateral at the following arbitrary coordinates
// filled with the texture of your choice, possibly
// alpha-blended - basically used for hardware alpha blending acceleration,
// could also be used for special effects, speeding up QPixmap::xForm
// and other cool things. This is implemented as two textured triangles
// using the Mach64 3d engine

template<const int depth,const int type>
void QGfxMach64<depth,type>::drawAlpha(int x1,int y1,int x2,int y2,
				       int x3,int y3,int x4,int y4)
{
    if(this->ncliprect<1) {
	return;
    }

    if(no3d)
	return;

    QRect cursRect(x1+this->xoffs, y1+this->yoffs, abs(x2-x1)+1, abs(y2-y1)+1);
    GFX_START(cursRect);

    if(!checkSourceDest()) {
	GFX_END
	return;
    }

    // Used the 3d/scaler pipeline, like stretchBlt

    int xx[4];
    int yy[4];

    // 2 fractional bits
    xx[0]=(x1+this->xoffs);
    yy[0]=(y1+this->yoffs);
    xx[1]=(x2+this->xoffs);
    yy[1]=(y2+this->yoffs);
    xx[2]=(x3+this->xoffs);
    yy[2]=(y3+this->yoffs);
    xx[3]=(x4+this->xoffs);
    yy[3]=(y4+this->yoffs);

    float xs=0.0;
    float ys=0.0;

    int loopc;
    for(loopc=0;loopc<4;loopc++) {
	if(xx[loopc]<0) {
	    float s1=x2-x1;
	    float p1=-(xx[loopc]);
	    xs=p1/s1;
	    xx[loopc]=0;
	}
	if(yy[loopc]<0) {
	    float s2=y3-y1;
	    float p2=-(yy[loopc]);
	    ys=p2/s2;
	    yy[loopc]=0;
	}
	xx[loopc]*=4;
	yy[loopc]*=4;
    }

    (*this->gfx_optype)=2;
    (*this->gfx_lastop)=LASTOP_ALPHA;

    for(loopc=0;loopc<this->ncliprect;loopc++) {

	do_scissors(this->cliprect[loopc]);

	// Used for one-over-area - magic needed by the triangle
	// setup engine
 	float ooa,ooa2;

	ooa = 0.25 * 0.25 * ( ( xx[1] - xx[0] ) * ( yy[0] - yy[2] ) +
			      ( yy[1] - yy[0] ) * ( xx[2] - xx[0] ) );

	ooa = -1.0 / ooa;

	ooa2 = 0.25 * 0.25 * ( ( xx[1] - xx[3] ) * ( yy[3] - yy[2] ) +
			       ( yy[1] - yy[3] ) * ( xx[2] - xx[3] ) );

	ooa2 = -1.0 / ooa2;

	int s3df=2;
	int afn=1;
	int asat=0;
	// These source and destination alpha values are guesswork but seem
	// to give the right effect
	// source blend factor is AsAsAs, destination blend factor is
	// 1-As,1-As,1-As. I wish I knew more maths...
	int asrc=4;     // 1
	int adst=5;
	int tlf=0;
	int tfilt=0;
	int tblend=0;
	int texalpha=0;
	if(this->srcdepth==32 && this->alphatype==QGfx::InlineAlpha)
	    texalpha=1;
	int split=0;

	// We only use inline alpha; Mach64 doesn't seem to easily support
	// separate alpha channel

	unsigned int scale3d=
	    ( 1 << 0 )              // 0 = zero extend, 1 = dynamic range
	                        // extend pixels to 32 bit
	    | ( 0 << 1 )            // 1 = 2D dither, 0 = error diffusion dither
	    | ( 0 << 2 )            // 1 = enable dither
	    | ( 0 << 3 )            // 1 = reset error diffusion each line
	    | ( 1 << 4 )            // 1 = round instead of dither
	    | ( 0 << 5 )            // 1 = disable texture cache
	    | ( s3df << 6 )         // 3=shading, 2=texture mapping, 1=scaling
	    | ( 0 << 8 )            // 1 = edge anti-alias
	    | ( split << 9 )        // 1 = split texture cache
	    | ( 0 << 10 )           // 1 = apple YUV mode
	    | ( afn << 11 )         // alpha / fog control
	    | ( asat << 13 )        // alpha saturate blending
	    | ( 0 << 14 )           // 1 = limit red dither range (what for?)
	    | ( 0 << 15 )           // 1 = signed dst blend clamp for mpeg
	    | ( asrc << 16 )        // blend src
	    | ( adst << 19 )        // blend dst
	    | ( tlf << 22 )         // texture environment
	    | ( 1 << 24 )           // 1 = disable mip-mapping (its broke on
	                        // all ragepros!)
	    | ( tfilt << 25 )       // 1 = bilinear filter texture on mag
	    | ( tblend << 26 )      // minification filtering
	    | ( 0 << 28 )           // 1 = LSB of alpha for texture masking
	    | ( 0 << 29 )           // alpha masking mode
	    | ( texalpha << 30 )    // 1 = texture has alpha
	    | ( 0 << 31 )           // 1 = source pixel from host register
	    ;

	setPixWidth(depth,this->srcdepth,this->srcdepth);

	wait_for_fifo(5);
	regw(DP_FRGD_CLR,0xffffffff);
	regw(DP_WRITE_MASK,0xffffffff);
	regw(DP_MIX,0x00070003);
	regw(CLR_CMP_CNTL,0);
	regw(GUI_TRAJ_CNTL,3);

	int p=this->srclinestep/4;

	// Pitch must be a power of 2 (in pixels)
	int logpitch;
	if(p==1024) {
	    logpitch=0xa;
	} else if(p==512) {
	    logpitch=0x9;
	} else if(p==256) {
	    logpitch=0x8;
	} else if(p==128) {
	    logpitch=0x7;
	} else if(p==64) {
	    logpitch=0x6;
	} else if(p==32) {
	    logpitch=0x5;
	} else if(p==16) {
	    logpitch=0x4;
	} else if(p==8) {
	    logpitch=0x3;
	} else {
	    logpitch=0x2;
	}

	double wfx=this->srcwidth;
	//double wf2=p;  //### not used
	wfx=wfx/p;
	double wfy=this->srcheight;
	wfy=wfy/p;

	wait_for_fifo(6);
	regw(TEX_SIZE_PITCH,logpitch | (logpitch << 4) | (logpitch << 8)
	     | (logpitch << 12) | (logpitch << 16) | (logpitch << 20) |
	     (logpitch << 24));

	unsigned long foffset;
	if (!this->gfx_screen->onCard(this->srcbits,foffset))
	    qFatal("checkSourceDest() lied!");

	// These registers are used for mip-mapping on Mach64. We don't need
	// that so we just set them all to the same texture
	regw(TEX_0_OFFSET,foffset);
	regw(TEX_1_OFFSET,foffset);
	regw(TEX_2_OFFSET,foffset);
	regw(TEX_3_OFFSET,foffset);
	regw(TEX_4_OFFSET,foffset);
	wait_for_fifo(7);
	regw(TEX_5_OFFSET,foffset);
	regw(TEX_6_OFFSET,foffset);
	regw(TEX_7_OFFSET,foffset);
	regw(TEX_8_OFFSET,foffset);
	regw(TEX_9_OFFSET,foffset);
	regw(TEX_10_OFFSET,foffset);

	unsigned int talpha=1;
	unsigned int ccf=1;
	unsigned int cblend=1;
	unsigned int cfilt=1;

	unsigned int tex;
	tex=(1 << 23) | (1 << 19) | (ccf << 9) | (cblend << 11) | (cfilt << 12)
	    | (talpha << 13);

	regw(TEX_CNTL,tex);

	// Fraction of pixmap to draw. Can go over 1.0 for tiling
	//float inv=1.0; //###not used
	//double wf=1.0;

	wait_for_fifo(5);
	regw(SCALE_3D_CNTL,scale3d);
	regw2(SETUP_CNTL,0x00000000);
	regw(Z_CNTL,0);
	regw(ALPHA_TEST_CNTL,0);
	regw(DP_SRC,DP_BKGD_SRC_3D | DP_FRGD_SRC_3D | DP_MONO_SRC_1);

	wait_for_fifo(5);
	regw2(VERTEX_1_ARGB,0x000000ff | this->calpha << 24);
	regw2(VERTEX_1_X_Y,xx[0] << 16 | yy[0]);
	regwf2(VERTEX_1_S,xs);
	regwf2(VERTEX_1_T,ys);
	regwf2(VERTEX_1_W,1.0);

	wait_for_fifo(5);
	regw2(VERTEX_2_ARGB,0x0000ff00 | this->calpha2 << 24);
	regw2(VERTEX_2_X_Y,xx[1] << 16 | yy[1]);
	regwf2(VERTEX_2_S,wfx);
	regwf2(VERTEX_2_T,ys);
	regwf2(VERTEX_2_W,1.0);

	wait_for_fifo(5);
	regw2(VERTEX_3_ARGB,0x00ff0000 | this->calpha4 << 24);
	regw2(VERTEX_3_X_Y,xx[2] << 16 | yy[2]);
	regwf2(VERTEX_3_S,xs);
	regwf2(VERTEX_3_T,wfy);
	regwf2(VERTEX_3_W,1.0);

	regwf2(ONE_OVER_AREA_UC,ooa);

	wait_for_fifo(6);

	regw2(VERTEX_1_ARGB,0x000000ff | this->calpha3 << 24);
	regw2(VERTEX_1_X_Y,xx[3] << 16 | yy[3]);
	regwf2(VERTEX_1_S,wfx);
	regwf2(VERTEX_1_T,wfy);
	regwf2(VERTEX_1_W,1.0);

	regwf2(ONE_OVER_AREA_UC,ooa2);

    }

    wait_for_fifo(2);
    regw(DP_SRC,0x300);
    regw(SCALE_3D_CNTL,0);

    GFX_END

}

// This does card-specific setup and constructs accelerated gfx's and
// the accelerated cursor

bool QMachScreen::useOffscreen() { return true; }
int QMachScreen::pixmapOffsetAlignment() { return 128; }
int QMachScreen::pixmapLinestepAlignment() { return 128; }

#ifndef QT_NO_QWS_CURSOR
class QMachCursor : public QScreenCursor
{
public:
    QMachCursor();
    ~QMachCursor();

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
    virtual bool supportsAlphaCursor() { return false; }

    static bool enabled() { return false; }

    inline unsigned int regr(volatile unsigned int);
    inline void regw(volatile unsigned int,unsigned long);

    unsigned char * regbase;
    unsigned char * regbase2;

private:

    int hotx;
    int hoty;

};

// Read a 32-bit graphics card register from 2d engine register block
inline unsigned int QMachCursor::regr(volatile unsigned int
						 regindex)
{
    unsigned long int val;
    val=*((volatile unsigned long *)(regbase+regindex));
    return val;
}

// Write a 32-bit graphics card register to 2d engine register block
inline void QMachCursor::regw(volatile unsigned int regindex,
					 unsigned long val)
{
    *((volatile unsigned long int *)(regbase+regindex))=val;
}

#endif // QT_NO_QWS_CURSOR

QMachScreen::QMachScreen( int display_id )
    : QLinuxFbScreen( display_id )
{
}

bool QMachScreen::connect( const QString &displaySpec )
{
    if ( !QLinuxFbScreen::connect( displaySpec ) )
	return FALSE;

    canaccel=false;

    // This is the 256-byte PCI config space information for the
    // card pointed to by QWS_CARD_SLOT, as read from /proc/bus/pci
    // (or in theory from a PCI bus scan - there is some code for this
    // but it's not how Qt/Embedded would normally work)
    // It only tests the vendor ID - so don't use it with other ATI
    // graphics cards, such as Rage128 or Radion variants, or Bad Things
    // May Happen
    const unsigned char* config = qt_probe_bus();

    if(!config)
	return false;

    unsigned short int * manufacturer=(unsigned short int *)config;
    unsigned short int * device=(unsigned short int *)(config+2);

    if(*manufacturer!=0x1002) {
	qDebug("This does not appear to be an ATI card");
	qDebug("Are you sure QWS_CARD_SLOT is pointing to the right entry in "
	       "/proc/bus/pci?");
	return FALSE;
    }

    if(*device==0x5654) {
	qDebug("Mach64 VT mapsize %x",mapsize);
	regbase2=(unsigned char *)0xdeadbeef;
	no3d=true;
    } else {
	qDebug("Device %d",*device);
	no3d=false;
    }

    // We expect the address pointer for the registers in config space
    // (the 3rd address specified) to be a memory one, so we do a simple
    // sanity check
    const unsigned char * bar=config+0x10;
    const unsigned long int * addr=(const unsigned long int *)bar;
    unsigned long int s;
    if(no3d) {
	s=*(addr);
    } else {
	s=*(addr+2);
    }
    unsigned long int olds=s;
    if(s & 0x1) {
#ifdef DEBUG_INIT
	printf("IO space - not right\n");
#endif
	return FALSE;
    } else {
#ifdef DEBUG_INIT
	qDebug("First address looks right");
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
	if(no3d) {
	    s+=(1024*1024*8)-4096;
	}
	// The registers block is 4k
	membase=(unsigned char *)mmap(0,no3d ? 1024 : 4096,PROT_READ |
				      PROT_WRITE,MAP_SHARED,
				      aperturefd,s);
	if(membase==0 || membase==(unsigned char *)-1) {
#ifdef DEBUG_INIT
	    qDebug("Failure to mmap /dev/mem, offset %lu %lx, %s",s,s,
		   strerror(errno));
#endif
	    close(aperturefd);
	    return FALSE;
	}
	if(no3d) {
	    regbase=membase+(1024*3);
	} else {
	    // 2d engine block is the second 1k block
	    regbase=membase+1024;
	    // 3d engine block is the first 1k block
	    regbase2=membase;
	}
    }

    qDebug("Detected Mach64");

    // Yes, we detected the card correctly so can safely make accelerated
    // gfxen
    canaccel=true;

    return TRUE;
}

QMachScreen::~QMachScreen()
{
}

// This is called when the Qt/Embedded server starts up but not when
// individual clients do. This is when you set the device to a known
// state

bool QMachScreen::initDevice()
{
    // Disable register reading in main aperture - normally the last
    // few k of the main framebuffer space is a duplication of the register
    // block - the accelerated Mach64 kernel framebuffer driver doesn't fix
    // this
    // Frees up 8k or so and is safer (if we scribble over the end of
    // our framebuffer area into this block then it won't be interpreted
    // as graphics commands)
    // Also enable register block 1 for the 3d engine

    if(!no3d) {
	qDebug("Buscntl %x",regr(BUS_CNTL));
	regw(BUS_CNTL,regr(BUS_CNTL) | 0x08000001);
    } else {
	qDebug("No3d buscntl %x",regr(BUS_CNTL));
	regw(BUS_CNTL,regr(BUS_CNTL) & ~0x08000001);
    }

    // However, that doesn't always work, so make sure it isn't mapped
    // anyway
    mapsize-=(1024*8);
    QLinuxFbScreen::initDevice();

    int tmp=regr(GUI_CNTL) & 0x3;
    if(tmp==0) {
        shared->fifomax=192;
    } else if(tmp==1) {
        shared->fifomax=128;
    } else if(tmp==2) {
        shared->fifomax=64;
    } else {
        qDebug("Reserved FIFO depth!");
        shared->fifomax=64;
    }

    // Lots of boilerplate from ATI manual, with some extra
    // from XFree Mach64 driver for good measure at the end

    regw(CONTEXT_MASK,0xffffffff);
    regw(DST_OFF_PITCH, (w / 8) << 22);
    regw(DST_Y_X,0);
    regw(DST_HEIGHT,0);
    regw(DST_BRES_ERR,0);
    regw(DST_BRES_INC,0);
    regw(DST_BRES_DEC,0);
    regw(SRC_OFF_PITCH, (w / 8 ) << 22);
    regw(SRC_Y_X,0);
    regw(SRC_HEIGHT1_WIDTH1,1);
    regw(SRC_Y_X_START,0);
    regw(SRC_HEIGHT2_WIDTH2,1);
    regw(SRC_CNTL,SRC_LINE_X_LEFT_TO_RIGHT);
    regw(HOST_CNTL,0);
    regw(PAT_REG0,0);
    regw(PAT_REG1,0);
    regw(PAT_CNTL,0);
    regw(DP_BKGD_CLR,0);
    regw(DP_FRGD_CLR,0xffffffff);
    regw(DP_WRITE_MASK,0xffffffff);
    regw(DP_SRC,FRGD_SRC_FRGD_CLR);
    regw(CLR_CMP_CLR,0);
    regw(CLR_CMP_MASK,0xffffffff);
    regw(CLR_CMP_CNTL,0);
    regw(DP_CHAIN_MASK,0x8410);
    regw(DST_X,0);
    regw(DST_Y,0);
    regw(DST_HEIGHT,760);
    regw(DP_FRGD_CLR,0xffffffff);
    regw(DP_WRITE_MASK,0xffffffff);
    regw(DP_SRC,0x00000100);
    regw(CLR_CMP_CNTL,0x00000000);
    regw(GUI_TRAJ_CNTL,0x00000003);
    regw(DST_CNTL,0x3);
    regw(DST_BRES_ERR,0);
    regw(DST_BRES_INC,0);
    regw(DST_BRES_DEC,0);
    regw(SRC_Y_X,0);
    regw(SRC_HEIGHT1_WIDTH1,0);
    regw(SRC_Y_X_START,0);
    regw(SRC_HEIGHT2_WIDTH2,0);
    regw(SRC_CNTL,0);
    regw(HOST_CNTL,regr(HOST_CNTL) & ~HOST_BYTE_ALIGN);
    regw(PAT_REG0,0);
    regw(PAT_REG1,0);
    regw(PAT_CNTL,0);
    regw(CUR_HORZ_VERT_OFF,0x00000000);
    regw(CUR_HORZ_VERT_POSN,0x00ff00ff);
    regw(DP_BKGD_CLR,0);
    regw(DP_FRGD_CLR,1);
    regw(DP_MIX,(MIX_SRC << 16) | MIX_DST);
    regw(DP_SRC,FRGD_SRC_FRGD_CLR);
    regw(CLR_CMP_CLR,0);
    regw(CLR_CMP_MASK,0xffffffff);
    regw(CLR_CMP_CNTL,0);

    // We use 58 fifos, at least 64 should be free
    shared->fifocount=58;

    return true;
}

// Create a hardware cursor if a software one wasn't explicitly requested
// via the QWS_SW_CURSOR environment variable

int QMachScreen::initCursor(void* e, bool init)
{
#ifndef QT_NO_QWS_CURSOR
    extern bool qws_sw_cursor;

    if(qws_sw_cursor==true) {
	return QLinuxFbScreen::initCursor(e,init);
    }
    qt_screencursor=new QMachCursor();
    qt_screencursor->init(0,false);
    ((QMachCursor *)qt_screencursor)->regbase=regbase;
    ((QMachCursor *)qt_screencursor)->regbase2=regbase2;
#endif
    return 0;
}

// Card shutdown - there's nothing really to do here

void QMachScreen::shutdownDevice()
{
    QLinuxFbScreen::shutdownDevice();
}

// Convert from a normal rgb value to the 2-bit encoding used by the
// Mach64 cursor. XOR isn't handled

int mach64_ngval(QRgb r)
{
    if(qAlpha(r)<255) {
        return 2;    // Transparent
    } else if(qBlue(r)>240) {
        return 0;    // White
    } else {
        return 1;    // Black
    }
}


QGfx * QMachScreen::createGfx(unsigned char * b,int w,int h,int d,int linestep)
{
    QGfx * ret=0;
    if( onCard(b) ) {
	if( d==16 ) {
	    ret = new QGfxMach64<16,0>(b,w,h);
	} else if ( d==32 ) {
	    ret = new QGfxMach64<32,0>(b,w,h);
	} else if ( d==8 ) {
	    ret = new QGfxMach64<8,0>(b,w,h);
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

QMachCursor::QMachCursor()
{
}

QMachCursor::~QMachCursor()
{
}

void QMachCursor::init(SWCursorData *,bool)
{
    myoffset=(qt_screen->width()*qt_screen->height()*qt_screen->depth())/8;
    myoffset+=8;
    fb_start=qt_screen->base();
    regbase=::regbase;
    regbase2=::regbase2;
}

// Set a new cursor image, with hotspot
void QMachCursor::set(const QImage& image,int hx,int hy)
{
    cursor=(QImage*)&image;
    hotx=hx;
    hoty=hy;

    if(cursor->isNull()) {
	static QImage *img = 0;
	if ( !img ) {
	    img = new QImage(8,8,8,3);
	    img->setColor(2, 0x00000000);
	    img->fill(2);
	}
	cursor = img;
    }

    unsigned int offset=myoffset;
    unsigned char * tmp=fb_start+offset;
    int loopc,loopc2;
    // 3=invert,binary 1==CLR1, 2==nothing(?), 0=CLR0

    // Write the cursor data in the image into the weird format
    // that Mach64 expects for cursors
    // We assume cursors are multiples of 8 pixels wide
    memset(tmp,0xaa,(16*64));
    for(loopc=0;loopc<cursor->height();loopc++) {
        for(loopc2=0;loopc2<(cursor->width()/4);loopc2++) {
            unsigned int v1,v2,v3,v4;
            unsigned int pos=loopc2*4;
            v1=mach64_ngval(cursor->pixel(pos,loopc));
            v2=mach64_ngval(cursor->pixel(pos+1,loopc));
            v3=mach64_ngval(cursor->pixel(pos+2,loopc));
            v4=mach64_ngval(cursor->pixel(pos+3,loopc));
            unsigned char put=(v4 << 6) | (v3 << 4) | (v2 << 2) | v1;
            *(tmp++)=put;
        }
        int add=16-(cursor->width()/4);
        tmp+=add;
    }
    QRgb a=cursor->color(1);
    QRgb b=cursor->color(0);
    unsigned int c,d;
    c=(qRed(a) << 8) | (qGreen(a) << 24) | (qBlue(a) << 16);
    d=(qRed(b) << 8) | (qGreen(b) << 24) | (qBlue(b) << 16);
    regw(CUR_CLR0,c);
    regw(CUR_CLR1,d);
    regw(CUR_OFFSET,offset/8);
}

// Make cursor disappear
void QMachCursor::hide()
{
    unsigned int cntlstat=regr(GEN_TEST_CNTL);
    cntlstat=cntlstat & 0xffffff7f;
    regw(GEN_TEST_CNTL,cntlstat);
}

// Make it come back
void QMachCursor::show()
{
    unsigned int cntlstat=regr(GEN_TEST_CNTL);
    cntlstat=cntlstat | 0x80;
    regw(GEN_TEST_CNTL,cntlstat);
}

// Move the cursor to point x,y - the hotspot should be at point x,y
void QMachCursor::move(int x,int y)
{
    x-=hotx;
    y-=hoty;
    unsigned int hold=x | (y << 16);
    regw(CUR_HORZ_VERT_POSN,hold);
}

#endif // QT_NO_QWS_CURSOR

#endif
