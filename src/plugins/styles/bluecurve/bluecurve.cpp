/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <iostream>
using std::cerr;
using std::endl;
/*
  Bluecurve style.
  Copyright (c) 2002 Red Hat, Inc.
  Authors: Bernhard Rosenkränzer <bero@redhat.com>
           Preston Brown <pbrown@redhat.com>
           Than Ngo <than@redhat.com>
           Alexander Larsson <alexl@redhat.com>

  Released under the GNU General Public License (GPL) v2.

  Based on the KDE Light style, 2nd revision:
  Copyright (c) 2000-2001 Trolltech AS (info@trolltech.com)

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

#include "qmenubar.h"
#include "qapplication.h"
#include "qpainter.h"
#include "qpalette.h"
#include "qframe.h"
#include "qpushbutton.h"
#include "qdrawutil.h"
#include "qscrollbar.h"
#include "qtabbar.h"
#include "qguardedptr.h"
#include "qlayout.h"
#include "qlineedit.h"
#include "qlistview.h"
#include "qbitmap.h"
#include "qimage.h"
#include "qcombobox.h"
#include "qslider.h"
#include "qstylefactory.h"
#include "qcleanuphandler.h"
#include "qcheckbox.h"
#include "qradiobutton.h"
#include "qprogressbar.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define RADIO_SIZE 13
#define CHECK_SIZE 13

#include "bits.h"
#include "bluecurve.h"
const double BluecurveStyle::shadeFactors[8] = {1.065, 0.963, 0.896, 0.85, 0.768, 0.665, 0.4, 0.205};

static void
rgb_to_hls( double *r,
            double *g,
            double *b )
{
    double min;
    double max;
    double red;
    double green;
    double blue;
    double h, l, s;
    double delta;

    red = *r;
    green = *g;
    blue = *b;

    if ( red > green ) {
        if ( red > blue )
            max = red;
        else
            max = blue;

        if ( green < blue )
            min = green;
        else
            min = blue;
    } else {
        if ( green > blue )
            max = green;
        else
            max = blue;

        if ( red < blue )
            min = red;
        else
            min = blue;
    }

    l = ( max + min ) / 2;
    s = 0;
    h = 0;

    if ( max != min ) {
        if ( l <= 0.5 )
            s = ( max - min ) / ( max + min );
        else
            s = ( max - min ) / ( 2 - max - min );

        delta = max - min;
        if ( red == max )
            h = ( green - blue ) / delta;
        else if ( green == max )
            h = 2 + ( blue - red ) / delta;
        else if ( blue == max )
            h = 4 + ( red - green ) / delta;

        h *= 60;
        if ( h < 0.0 )
            h += 360;
    }

    *r = h;
    *g = l;
    *b = s;
}

static void
hls_to_rgb( double *h,
            double *l,
            double *s )
{
    double hue;
    double lightness;
    double saturation;
    double m1, m2;
    double r, g, b;

    lightness = *l;
    saturation = *s;

    if ( lightness <= 0.5 )
        m2 = lightness * ( 1 + saturation );
    else
        m2 = lightness + saturation - lightness * saturation;
    m1 = 2 * lightness - m2;

    if ( saturation == 0 ) {
        *h = lightness;
        *l = lightness;
        *s = lightness;
    } else {
        hue = *h + 120;
        while ( hue > 360 )
            hue -= 360;
        while ( hue < 0 )
            hue += 360;

        if ( hue < 60 )
            r = m1 + ( m2 - m1 ) * hue / 60;
        else if ( hue < 180 )
            r = m2;
        else if ( hue < 240 )
            r = m1 + ( m2 - m1 ) * ( 240 - hue ) / 60;
        else
            r = m1;

        hue = *h;
        while ( hue > 360 )
            hue -= 360;
        while ( hue < 0 )
            hue += 360;

        if ( hue < 60 )
            g = m1 + ( m2 - m1 ) * hue / 60;
        else if ( hue < 180 )
            g = m2;
        else if ( hue < 240 )
            g = m1 + ( m2 - m1 ) * ( 240 - hue ) / 60;
        else
            g = m1;

        hue = *h - 120;
        while ( hue > 360 )
            hue -= 360;
        while ( hue < 0 )
            hue += 360;

        if ( hue < 60 )
            b = m1 + ( m2 - m1 ) * hue / 60;
        else if ( hue < 180 )
            b = m2;
        else if ( hue < 240 )
            b = m1 + ( m2 - m1 ) * ( 240 - hue ) / 60;
        else
            b = m1;

        *h = r;
        *l = g;
        *s = b;
    }
}

static void
shade( const QColor &ca, QColor &cb, double k )
{
    int r, g, b;
    double red;
    double green;
    double blue;

    ca.rgb( &r, &g, &b );

    red = ( double ) r / 255.0;
    green = ( double ) g / 255.0;
    blue = ( double ) b / 255.0;

    rgb_to_hls( &red, &green, &blue );

    green *= k;
    if ( green > 1.0 )
        green = 1.0;
    else if ( green < 0.0 )
        green = 0.0;

    blue *= k;
    if ( blue > 1.0 )
        blue = 1.0;
    else if ( blue < 0.0 )
        blue = 0.0;

    hls_to_rgb( &red, &green, &blue );

    cb.setRgb(( int )( red*255.0 ), ( int )( green*255.0 ), ( int )( blue*255.0 ) );
}

static QImage *
generate_bit( unsigned char alpha[], const QColor &color, double mult )
{
    unsigned int r, g, b;
    QImage *image;
    QRgb *pixels;
    int w, h;
    int x, y;

    r = ( int )( color.red() * mult );
    r = MIN( r, 255 );
    g = ( int )( color.green() * mult );
    g = MIN( g, 255 );
    b = ( int )( color.blue() * mult );
    b = MIN( b, 255 );

    image = new QImage( RADIO_SIZE, RADIO_SIZE, 32 );
    image->setAlphaBuffer( true );

    w = image->width();
    h = image->height();

    for ( y = 0; y < h; y++ ) {
        pixels = ( QRgb * )image->scanLine( y );
        for ( x = 0; x < w; x++ )
            pixels[x] = qRgba( r, g, b, alpha ? alpha[y*w+x] : 255 );
    }

    return image;
}

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define CLAMP_UCHAR(v) ((unsigned char) (CLAMP (((int)v), (int)0, (int)255)))

static QImage *
colorize_bit( unsigned char *bit,
              unsigned char *alpha,
              const QColor  &new_color )
{
    QImage *image;
    double intensity;
    int x, y;
    const unsigned char *src, *asrc;
    QRgb *dest;
    int dest_rowstride;
    int width, height;
    unsigned char *dest_pixels;

    image = new QImage( RADIO_SIZE, RADIO_SIZE, 32, 0, QImage::BigEndian );
    image->setAlphaBuffer( true );

    if ( image == NULL )
        return NULL;

    dest_rowstride = image->bytesPerLine();
    width = image->width();
    height = image->height();
    dest_pixels = image->bits();

    for ( y = 0; y < RADIO_SIZE; y++ ) {
        src = bit + y * RADIO_SIZE;
        asrc = alpha + y * RADIO_SIZE;
        dest = ( QRgb * )image->scanLine( y );

        for ( x = 0; x < RADIO_SIZE; x++ ) {
            int dr, dg, db;

            intensity = src[x] / 255.0;

            if ( intensity <= 0.5 ) {
                /* Go from black at intensity = 0.0 to new_color at intensity = 0.5 */
                dr = int(( new_color.red() * intensity * 2.0 ) );
                dg = int(( new_color.green() * intensity * 2.0 ) );
                db = int(( new_color.blue() * intensity * 2.0 ) );
            } else {
                /* Go from new_color at intensity = 0.5 to white at intensity = 1.0 */
                dr = int(( new_color.red() + ( 255 - new_color.red() ) * ( intensity - 0.5 ) * 2.0 ) );
                dg = int(( new_color.green() + ( 255 - new_color.green() ) * ( intensity - 0.5 ) * 2.0 ) );
                db = int(( new_color.blue() + ( 255 - new_color.blue() ) * ( intensity - 0.5 ) * 2.0 ) );
            }

            dest[x] = qRgba( CLAMP_UCHAR( dr ), CLAMP_UCHAR( dg ), CLAMP_UCHAR( db ), asrc[x] );
        }
    }

    return image;
}


static void
composeImage( QImage *destImg, QImage *srcImg )
{
    int w, h, x, y;
    QRgb *src, *dest;
    unsigned int a;
    QRgb s, d;

    w = destImg->width();
    h = destImg->height();

    for ( y = 0; y < h; y++ ) {
        src = ( QRgb * )srcImg->scanLine( y );
        dest = ( QRgb * )destImg->scanLine( y );

        for ( x = 0; x < w; x++ ) {
            s = src[x];
            d = dest[x];

            a = qAlpha( s );

            dest[x] = qRgba(( qRed( s ) * a + ( 255 - a ) * qRed( d ) ) / 255,
                            ( qGreen( s ) * a + ( 255 - a ) * qGreen( d ) ) / 255,
                            ( qBlue( s ) * a + ( 255 - a ) * qBlue( d ) ) / 255,
                            a + (( 255 - a ) * qAlpha( d ) ) / 255 );
        }
    }
}

struct BluecurveStylePrivate {
    BluecurveStylePrivate()
            : hoverWidget( 0 ), hovering( FALSE ), sliderActive( FALSE ), mousePressed( FALSE ),
            scrollbarElement( 0 ), lastElement( 0 ), ref( 1 ) {
        ;
    }

    QGuardedPtr<QWidget> hoverWidget;
    bool hovering, sliderActive, mousePressed;
    int scrollbarElement, lastElement, ref;
    QPoint mousePos;
};

static BluecurveStylePrivate * singleton = 0;

/* We assume this seldom collides, since we can only cache one at a time */
static long
hashColorGroup( const QColorGroup &cg )
{
    return cg.button().rgb() << 8 ^ cg.highlight().rgb();
}

BluecurveStyle::BluecurveColorData::~BluecurveColorData()
{
    int i;

    for ( i = 0; i < 8; i++ ) {
        if ( radioPix[i] != 0 )
            delete radioPix[i];
    }
    if ( radioMask != 0 )
        delete radioMask;

    for ( i = 0; i < 6; i++ ) {
        if ( checkPix[i] != 0 )
            delete checkPix[i];
    }
}

BluecurveStyle::BluecurveStyle() : QCommonStyle(), m_dataCache()
{
    if ( !singleton )
        singleton = new BluecurveStylePrivate;
    else
        singleton->ref++;

    m_dataCache.setAutoDelete( true );

    basestyle = QStyleFactory::create( "MotifPlus" );
    if ( ! basestyle )
        basestyle = QStyleFactory::create( QStyleFactory::keys().first() );
    if ( ! basestyle )
        qFatal( "BluecurveStyle: couldn't find a base style!" );
}

BluecurveStyle::~BluecurveStyle()
{
    if ( singleton && singleton->ref-- <= 0 ) {
        delete singleton;
        singleton = 0;
    }
    delete basestyle;
}

void BluecurveStyle::polishPopupMenu( QPopupMenu * )
{
    // empty to satisy pure virtual requirements
}

/* from Qt's motifplus style */
void BluecurveStyle::polish( QWidget *widget )
{
    if ( widget->inherits( "QButton" ) ||
            widget->inherits( "QComboBox" ) )
        widget->installEventFilter( this );

    if ( widget->inherits( "QScrollBar" ) ||
            widget->inherits( "QSlider" ) ) {
        widget->setMouseTracking( TRUE );
        widget->installEventFilter( this );
    }
    QCommonStyle::polish( widget );
}

void BluecurveStyle::unPolish( QWidget *widget )
{
    widget->removeEventFilter( this );
    QCommonStyle::unPolish( widget );
}

BluecurveStyle::BluecurveColorData *
BluecurveStyle::realizeData( const QColorGroup &cg ) const
{
    BluecurveColorData *cdata;
    int i, j;

    cdata = new BluecurveColorData;
    cdata->buttonColor = cg.button().rgb();
    cdata->spotColor = cg.highlight().rgb();

    for ( i = 0; i < 8; i++ ) {
        shade( cg.button(), cdata->shades[i], shadeFactors[i] );
    }

    shade( cg.highlight(), cdata->spots[0], 1.62 );
    shade( cg.highlight(), cdata->spots[1], 1.05 );
    shade( cg.highlight(), cdata->spots[2], 0.72 );

    QImage *dot, *inconsistent, *outline, *circle, *check, *base;

    dot = colorize_bit( dot_intensity, dot_alpha, cg.highlight() );
    outline = generate_bit( outline_alpha, cdata->shades[6], 1.0 );

    QImage composite( RADIO_SIZE, RADIO_SIZE, 32, 0, QImage::BigEndian );

    for ( i = 0; i < 2; i++ ) {
        for ( j = 0; j < 2; j++ ) {
            if ( i == 0 )
                composite.fill( cg.button().rgb() );
            else
                composite.fill( cg.midlight().rgb() );
            composeImage( &composite, outline );

            if ( j == 0 )
                circle = generate_bit( circle_alpha, Qt::white, 1.0 );
            else
                circle = generate_bit( circle_alpha, cdata->shades[1], 1.0 );

            composeImage( &composite, circle );
            delete circle;

            cdata->radioPix[i*4+j*2+0] = new QPixmap( composite );

            composeImage( &composite, dot );
            cdata->radioPix[i*4+j*2+1] = new QPixmap( composite );
        }
    }

    QImage mask = outline->createAlphaMask();
    cdata->radioMask = new QPixmap( mask );

    check = generate_bit( check_alpha, cg.highlight(), 1.0 );
    inconsistent = generate_bit( check_inconsistent_alpha, cg.highlight(), 1.0 );

    for ( i = 0; i < 2; i++ ) {
        if ( i == 0 )
            base = generate_bit( check_base_alpha, Qt::white, 1.0 );
        else
            base = generate_bit( check_base_alpha, cdata->shades[1], 1.0 );

        composite.fill( cdata->shades[6].rgb() );
        composeImage( &composite, base );
        cdata->checkPix[i*3+0] = new QPixmap( composite );

        composeImage( &composite, check );
        cdata->checkPix[i*3+1] = new QPixmap( composite );

        composite.fill( cdata->shades[6].rgb() );
        composeImage( &composite, base );
        composeImage( &composite, inconsistent );
        cdata->checkPix[i*3+2] = new QPixmap( composite );

        delete base;
    }

    delete dot;
    delete inconsistent;
    delete outline;
    delete check;

    return cdata;
}

const BluecurveStyle::BluecurveColorData *
BluecurveStyle::lookupData( const QColorGroup &cg ) const
{
    BluecurveColorData *cdata;
    long h;
    QIntCache<BluecurveColorData> *cache;

    h = hashColorGroup( cg );

    // Cast away the const crap
    // Nasty, but how else would i do it?
    cache = ( QIntCache<BluecurveColorData> * ) & m_dataCache;

    cdata = cache->find( h );

    if ( cdata == 0 || !cdata->isGroup( cg ) ) {
        if ( cdata != 0 ) {
            cache->remove( h );
        }

        cdata = realizeData( cg );
        cache->insert( h, cdata );
    }

    return cdata;
}


void
BluecurveStyle::getShade( const QColorGroup &cg, int shadenr, QColor &res ) const
{
    const BluecurveColorData *cdata = lookupData( cg );

    res = cdata->shades[shadenr];
}

bool BluecurveStyle::eventFilter( QObject *object, QEvent *event )
{

    switch ( event->type() ) {
    case QEvent::MouseButtonPress: {
        singleton->mousePressed = TRUE;

        if ( ! object->inherits( "QSlider" ) )
            break;

        singleton->sliderActive = TRUE;
        break;
    }

    case QEvent::MouseButtonRelease: {
        singleton->mousePressed = FALSE;

        if ( ! object->inherits( "QSlider" ) )
            break;

        singleton->sliderActive = FALSE;
        (( QWidget * ) object )->repaint( FALSE );
        break;
    }

    case QEvent::Enter: {
        if ( ! object->isWidgetType() )
            break;

        singleton->hoverWidget = ( QWidget * ) object;
        if ( ! singleton->hoverWidget->isEnabled() ) {
            singleton->hoverWidget = 0;
            break;
        }
        singleton->hoverWidget->repaint( FALSE );
        break;
    }

    case QEvent::Leave: {
        if ( object != singleton->hoverWidget )
            break;
        QWidget *w = singleton->hoverWidget;
        singleton->hoverWidget = 0;
        w->repaint( FALSE );
        break;
    }

    case QEvent::MouseMove: {
        if ( ! object->isWidgetType() || object != singleton->hoverWidget )
            break;

        if ( ! object->inherits( "QScrollBar" ) && ! object->inherits( "QSlider" ) )
            break;

        singleton->mousePos = (( QMouseEvent * ) event )->pos();
        if ( ! singleton->mousePressed ) {
            singleton->hovering = TRUE;
            singleton->hoverWidget->repaint( FALSE );
            singleton->hovering = FALSE;
        }

        break;
    }

    default:
        break;
    }

    return QCommonStyle::eventFilter( object, event );
}

void
BluecurveStyle::drawTextRect( QPainter *p, const QRect &r,
                              const QColorGroup &cg,
                              QStyle::SFlags flags,
                              const QBrush *fill ) const
{
    QRect br = r;

    const BluecurveColorData *cdata = lookupData( cg );

    p->setPen( cdata->shades[5] );
    p->drawRect( r );

    // button bevel
    p->setPen( cdata->shades[1] );
    p->drawLine( r.x() + 1, r.y() + 2,
                 r.x() + 1, r.y() + r.height() - 2 ); // left
    p->drawLine( r.x() + 1, r.y() + 1,
                 r.x() + r.width() - 2, r.y() + 1 ); // top

    p->setPen( cg.light() );
    p->drawLine( r.x() + r.width() - 2, r.y() + 2,
                 r.x() + r.width() - 2, r.y() + r.height() - 3 ); // right
    p->drawLine( r.x() + 2, r.y() + r.height() - 2,
                 r.x() + r.width() - 2, r.y() + r.height() - 2 ); // bottom

    br.addCoords( 2, 2, -2, -2 );

    // fill
    if ( fill )
        p->fillRect( br, *fill );
}


void
BluecurveStyle::drawLightBevel( QPainter *p, const QRect &r,
                                const QColorGroup &cg,
                                QStyle::SFlags flags,
                                const QBrush *fill ) const
{
    QRect br = r;
    QColor col;

    bool sunken = ( flags & ( QStyle::Style_Down | QStyle::Style_On |
                              QStyle::Style_Sunken ) );

    const BluecurveColorData *cdata = lookupData( cg );

    p->setPen( cdata->shades[6] );
    p->drawRect( r );

    if ( flags & ( QStyle::Style_Down | QStyle::Style_On |
                   QStyle::Style_Sunken | QStyle::Style_Raised ) ) {
        // button bevel
        p->setPen( sunken ? cdata->shades[2] : cg.light() );
        p->drawLine( r.x() + 1, r.y() + 2,
                     r.x() + 1, r.y() + r.height() - 3 ); // left
        p->drawLine( r.x() + 1, r.y() + 1,
                     r.x() + r.width() - 2, r.y() + 1 ); // top

        p->setPen( sunken ? cg.light() : cdata->shades[2] );
        p->drawLine( r.x() + r.width() - 2, r.y() + 2,
                     r.x() + r.width() - 2, r.y() + r.height() - 3 ); // right
        p->drawLine( r.x() + 1, r.y() + r.height() - 2,
                     r.x() + r.width() - 2, r.y() + r.height() - 2 ); // bottom

        br.addCoords( 2, 2, -2, -2 );
    } else
        br.addCoords( 1, 1, -1, -1 );

    // fill
    if ( fill ) p->fillRect( br, *fill );
}

void BluecurveStyle::drawPrimitive( PrimitiveElement pe,
                                    QPainter *p,
                                    const QRect &r,
                                    const QColorGroup &cg,
                                    SFlags flags,
                                    const QStyleOption &data ) const
{
    const BluecurveColorData *cdata = lookupData( cg );
    switch ( pe ) {
    case PE_HeaderSection: {
        const QBrush *fill;
        flags = (( flags | Style_Sunken ) ^ Style_Sunken ) | Style_Raised;
        if ( !( flags & Style_Down ) && ( flags & Style_On ) )
            fill = &cg.brush( QColorGroup::Midlight );
        else
            fill = &cg.brush( QColorGroup::Button );
        drawLightBevel( p, r, cg, flags, fill );
        p->setPen( cg.buttonText() );
        break;
    }
    case PE_HeaderArrow: {

        drawPrimitive(( flags & Style_Up ) ? PE_ArrowUp : PE_ArrowDown, p,
                      r, cg, Style_Enabled );
        break;
    }
    case PE_ButtonCommand:
    case PE_ButtonBevel:
    case PE_ButtonTool: {
        const QBrush *fill;
        if ( flags & QStyle::Style_Enabled ) {
            if (( flags & ( QStyle::Style_Down |
                            QStyle::Style_On |
                            QStyle::Style_Sunken ) ) ||
                    ( flags & QStyle::Style_MouseOver ) )
                fill = &cg.brush( QColorGroup::Midlight );
            else
                fill = &cg.brush( QColorGroup::Button );
        } else
            fill = &cg.brush( QColorGroup::Background );
        drawLightBevel( p, r, cg, flags, fill );
        break;
    }

    case PE_ButtonDropDown: {
        QBrush thefill;
        bool sunken =
            ( flags & ( QStyle::Style_Down | QStyle::Style_On | QStyle::Style_Sunken ) );

        if ( flags & QStyle::Style_Enabled ) {
            if ( sunken )
                thefill = cg.brush( QColorGroup::Midlight );
            else
                thefill = cg.brush( QColorGroup::Button );
        } else
            thefill = cg.brush( QColorGroup::Background );

        p->setPen( cg.dark() );
        p->drawLine( r.topLeft(),  r.topRight() );
        p->drawLine( r.topRight(), r.bottomRight() );
        p->drawLine( r.bottomRight(), r.bottomLeft() );

        if ( flags & ( QStyle::Style_Down | QStyle::Style_On |
                       QStyle::Style_Sunken | QStyle::Style_Raised ) ) {
            // button bevel
            if ( sunken )
                p->setPen( cg.mid() );
            else
                p->setPen( cg.light() );

            p->drawLine( r.x(), r.y() + 2,
                         r.x(), r.y() + r.height() - 3 ); // left
            p->drawLine( r.x(), r.y() + 1,
                         r.x() + r.width() - 2, r.y() + 1 ); // top

            if ( sunken )
                p->setPen( cg.light() );
            else
                p->setPen( cg.mid() );

            p->drawLine( r.x() + r.width() - 2, r.y() + 2,
                         r.x() + r.width() - 2, r.y() + r.height() - 3 ); // right
            p->drawLine( r.x() + 1, r.y() + r.height() - 2,
                         r.x() + r.width() - 2, r.y() + r.height() - 2 ); // bottom
        }

        p->fillRect( r.x() + 1, r.y() + 2, r.width() - 3, r.height() - 4, thefill );
        break;
    }

    case PE_ButtonDefault: {
        p->setPen( cg.shadow() );
        p->setBrush( NoBrush );
        p->drawRect( r );
        break;
    }

//     case PE_CheckMark:
//     {
//  QRect mark(r.x()+(r.width()-6)/2, r.y()+(r.height()-6)/2, 6, 6);
//  p->setBrush((flags & Style_Active) ? cg.highlightedText() : cg.highlight());
//  p->setPen(Qt::NoPen);
//  p->drawEllipse(mark);
//  break;
//     }

    case PE_Indicator: {
        int pix = 0;
        if ( flags & Style_Down )
            pix += 3;

        if ( flags & Style_On )
            pix += 1;
        else if ( flags & Style_NoChange )
            pix += 2;

        p->drawPixmap( r.topLeft(), *cdata->checkPix[pix] );
        break;
    }
    case PE_IndicatorMask: {
        p->fillRect( r, color1 );
        break;
    }

    case PE_ExclusiveIndicator: {
        int pix = 0;
        if ( flags & Style_MouseOver ) {
            pix += 4;
            p->fillRect( r, cg.brush( QColorGroup::Midlight ) );
        } else {
            p->fillRect( r, cg.brush( QColorGroup::Background ) );
        }

        if ( flags & Style_Down )
            pix += 2;
        if ( flags & Style_On )
            pix += 1;

        p->drawPixmap( r.topLeft(), *cdata->radioPix[pix] );

        break;
    }
    case PE_ExclusiveIndicatorMask:
        p->setPen( color1 );
        p->setBrush( color1 );
        p->drawPixmap( r.topLeft(), *cdata->radioMask );
        break;


    case PE_DockWindowHandle: {
        QString title;
        bool drawTitle = FALSE;
        if ( p && p->device()->devType() == QInternal::Widget ) {
            QWidget *w = ( QWidget * ) p->device();
            QWidget *p = w->parentWidget();
            if ( p->inherits( "QDockWindow" ) && ! p->inherits( "QToolBar" ) ) {
                drawTitle = TRUE;
                title = p->caption();
            }
        }

        flags |= Style_Raised;
        if ( flags & Style_Horizontal ) {
            if ( drawTitle ) {
                QPixmap pm( r.height(), r.width() );
                QPainter p2( &pm );
                p2.fillRect( 0, 0, pm.width(), pm.height(),
                             cg.brush( QColorGroup::Highlight ) );
                p2.setPen( cg.highlightedText() );
                p2.drawText( 0, 0, pm.width(), pm.height(), AlignCenter, title );
                p2.end();

                QWMatrix m;
                m.rotate( 270.0 );
                pm = pm.xForm( m );
                p->drawPixmap( r.x(), r.y(), pm );
            } else {
                p->fillRect( r, cg.button() );
                p->setPen( cdata->shades[3] );
                p->drawLine( r.right() - 1, r.top() + 1,
                             r.right() - 1, r.bottom() - 2 );
                p->setPen( cdata->shades[0] );
                p->drawLine( r.right(), r.top() + 1,
                             r.right(), r.bottom() - 2 );

                int yy = r.top() + 3;
                int nLines = ( r.height() - 4 ) / 4;

                for ( int i = 0; i < nLines; yy += 4, i++ ) {
                    p->setPen( cdata->shades[6] );
                    p->drawLine( 1, yy + 3,
                                 1 + 3, yy );
                    p->setPen( cg.light() );
                    p->drawLine( 2, yy + 3,
                                 1 + 3, yy + 1 );
                }
            }
        } else {
            if ( drawTitle ) {
                p->fillRect( r, cg.brush( QColorGroup::Highlight ) );
                p->setPen( cg.highlightedText() );
                p->drawText( r, AlignCenter, title );
            } else {
                p->fillRect( r, cg.button() );
                p->setPen( cdata->shades[3] );
                p->drawLine( r.left() + 1, r.bottom() - 1,
                             r.right() - 2, r.bottom() - 1 );
                p->setPen( cdata->shades[0] );
                p->drawLine( r.left() + 1, r.bottom(),
                             r.right() - 2, r.bottom() );

                int xx = r.left() + 3;
                int nLines = ( r.width() - 5 ) / 4;

                for ( int i = 0; i < nLines; xx += 4, i++ ) {
                    p->setPen( cdata->shades[6] );
                    p->drawLine( xx + 3, 1,
                                 xx, 1 + 3 );
                    p->setPen( cg.light() );
                    p->drawLine( xx + 3, 2,
                                 xx + 1, 1 + 3 );
                }
            }
        }
        break;
    }

    case PE_DockWindowSeparator: {
        if ( r.width() > 20 || r.height() > 20 ) {
            if ( flags & Style_Horizontal ) {
                p->setPen( cg.mid().dark( 120 ) );
                p->drawLine( r.left() + 1, r.top() + 6, r.left() + 1, r.bottom() - 6 );
                p->setPen( cg.light() );
                p->drawLine( r.left() + 2, r.top() + 6, r.left() + 2, r.bottom() - 6 );
            } else {
                p->setPen( cg.mid().dark( 120 ) );
                p->drawLine( r.left() + 6, r.top() + 1, r.right() - 6, r.top() + 1 );
                p->setPen( cg.light() );
                p->drawLine( r.left() + 6, r.top() + 2, r.right() - 6, r.top() + 2 );
            }
        } else
            QCommonStyle::drawPrimitive( pe, p, r, cg, flags, data );
        break;
    }

    case PE_Splitter:
        if ( flags & Style_Horizontal ) {
            int y_mid = r.height() / 2;
            for ( int i = 0; i < 21; i = i + 5 ) {
                p->setPen( cdata->shades[5] );
                p->drawLine( r.x() + 2, y_mid - 10 + i, r.right() - 2, y_mid - 10 + i - 3 );
                p->setPen( cg.light() );
                p->drawLine( r.x() + 3, y_mid - 10 + i, r.right() - 1, y_mid - 10 + i - 3 );
            }
        } else {
            int x_mid = r.width() / 2;
            for ( int i = 0; i < 21; i = i + 5 ) {
                p->setPen( cdata->shades[5] );
                p->drawLine( x_mid - 10 + i + 3, r.y() + 2, x_mid - 10 + i, r.bottom() - 2 );
                p->setPen( cg.light() );
                p->drawLine( x_mid - 10 + i + 4, r.y() + 2, x_mid - 10 + i + 1, r.bottom() - 2 );
            }
        }

        if ( flags & Style_Horizontal )
            flags &= ~Style_Horizontal;
        else
            flags |= Style_Horizontal;
        break;
        // fall through intended

    case PE_DockWindowResizeHandle: {
        p->fillRect( r, cg.background() );
        if ( flags & Style_Horizontal ) {
            p->setPen( cg.highlight().light() );
            p->drawLine( r.left() + 1, r.top() + 1, r.right() - 1, r.top() + 1 );
            p->setPen( cg.highlight() );
            p->drawLine( r.left() + 1, r.top() + 2, r.right() - 1, r.top() + 2 );
            p->setPen( cg.highlight().dark() );
            p->drawLine( r.left() + 1, r.top() + 3, r.right() - 1, r.top() + 3 );
        } else {
            p->setPen( cg.highlight().light() );
            p->drawLine( r.left() + 1, r.top() + 1, r.left() + 1, r.bottom() - 1 );
            p->setPen( cg.highlight() );
            p->drawLine( r.left() + 2, r.top() + 1, r.left() + 2, r.bottom() - 1 );
            p->setPen( cg.highlight().dark() );
            p->drawLine( r.left() + 3, r.top() + 1, r.left() + 3, r.bottom() - 1 );
        }
        break;
    }

    case PE_PanelLineEdit: {
        drawTextRect( p, r, cg, flags );
        break;
    }
    case PE_Panel:
    case PE_PanelPopup:
    case PE_PanelTabWidget:
    case PE_WindowFrame: {
        int lw = data.isDefault() ?
                 pixelMetric( PM_DefaultFrameWidth ) : data.lineWidth();

        if ( !( flags & Style_Sunken ) )
            flags |= Style_Raised;
        if ( lw == 2 )
            drawLightBevel( p, r, cg, flags );
        else
            QCommonStyle::drawPrimitive( pe, p, r, cg, flags, data );
        break;
    }

    case PE_PanelDockWindow: {
        int lw = data.isDefault() ?
                 pixelMetric( PM_DockWindowFrameWidth ) : data.lineWidth();
        if ( lw == 2 )
            if ( flags & Style_MouseOver )
                drawLightBevel( p, r, cg, flags | Style_Raised,
                                &cg.brush( QColorGroup::Midlight ) );
            else
                drawLightBevel( p, r, cg, flags | Style_Raised,
                                &cg.brush( QColorGroup::Button ) );
        else
            QCommonStyle::drawPrimitive( pe, p, r, cg, flags, data );
        break;
    }

    case PE_PanelMenuBar: {
        int lw = data.isDefault() ?
                 pixelMetric( PM_MenuBarFrameWidth ) : data.lineWidth();

        if ( lw == 2 ) {
            p->fillRect( r, cg.button() );
            p->setPen( cg.light() );
            p->drawLine( r.left(), r.top(), r.right(), r.top() );
            p->setPen( cg.mid() );
            p->drawLine( r.left(), r.bottom(), r.right(), r.bottom() );
        }  else
            QCommonStyle::drawPrimitive( pe, p, r, cg, flags, data );


        break;
    }

    case PE_ScrollBarAddLine:
    case PE_ScrollBarSubLine: {
        drawLightBevel( p, r, cg, (( flags | Style_Down ) ^ Style_Down ) |
                        (( flags & Style_Enabled ) ? Style_Raised : Style_Default ),
                        &cg.brush( QColorGroup::Button ) );

        if (( pe == PE_ScrollBarAddLine ) && ( flags & Style_Horizontal ) ) {
            pe = PE_ArrowRight;
        } else if ( pe == PE_ScrollBarAddLine ) {
            pe = PE_ArrowDown;
        } else if ( flags & Style_Horizontal ) {
            pe = PE_ArrowLeft;
        } else {
            pe = PE_ArrowUp;
        }
        drawPrimitive( pe, p, r, cg, flags );
        break;
    }

    case PE_ScrollBarSubPage:
    case PE_ScrollBarAddPage: {
        if ( flags & Style_Down )
            p->fillRect( r, cg.brush( QColorGroup::Midlight ) );
        else
            p->fillRect( r, cdata->shades[3] );
        p->setPen( cdata->shades[5] );
        if ( flags & Style_Horizontal ) {
            p->drawLine( r.left(), r.top(), r.right(), r.top() );
            p->drawLine( r.left(), r.bottom(), r.right(), r.bottom() );
        } else {
            p->drawLine( r.left(), r.top(), r.left(), r.bottom() );
            p->drawLine( r.right(), r.top(), r.right(), r.bottom() );
        }
        break;
    }

    case PE_ScrollBarSlider: {
        int x1, y1;
        p->setPen( cg.dark() );
        drawLightBevel( p, r, cg, (( flags | Style_Down ) ^ Style_Down ) |
                        (( flags & Style_Enabled ) ? Style_Raised : Style_Default ),
                        &cg.brush( QColorGroup::Button ) );

        if ( flags & Style_Horizontal && r.width() < 31 )
            break;
        if ( !( flags & Style_Horizontal ) && r.height() < 31 )
            break;

        p->setPen( cg.mid() );
        if ( flags & Style_Horizontal ) {
            x1 = ( r.left() + r.right() ) / 2 - 8;
            y1 = (( r.top() + r.bottom() ) - 6 ) / 2;
            p->drawLine( x1 + 5, y1,
                         x1, y1 + 5 );
            p->drawLine( x1 + 5 + 5, y1,
                         x1 + 5, y1 + 5 );
            p->drawLine( x1 + 5 + 5*2, y1,
                         x1 + 5*2, y1 + 5 );
        } else {
            x1 = (( r.left() + r.right() ) - 6 ) / 2;
            y1 = ( r.top() + r.bottom() ) / 2 - 8;
            p->drawLine( x1 + 5, y1,
                         x1, y1 + 5 );
            p->drawLine( x1 + 5, y1 + 5,
                         x1, y1 + 5 + 5 );
            p->drawLine( x1 + 5, y1 + 5*2,
                         x1, y1 + 5 + 5*2 );
        }

        p->setPen( cg.light() );
        if ( flags & Style_Horizontal ) {
            x1 = ( r.left() + r.right() ) / 2 - 8;
            y1 = (( r.top() + r.bottom() ) - 6 ) / 2;
            p->drawLine( x1 + 5, y1 + 1,
                         x1 + 1, y1 + 5 );
            p->drawLine( x1 + 5 + 5, y1 + 1,
                         x1 + 1 + 5, y1 + 5 );
            p->drawLine( x1 + 5 + 5*2, y1 + 1,
                         x1 + 1 + 5*2, y1 + 5 );
        } else {
            x1 = (( r.left() + r.right() ) - 6 ) / 2;
            y1 = ( r.top() + r.bottom() ) / 2 - 8;
            p->drawLine( x1 + 5, y1 + 1,
                         x1 + 1, y1 + 5 );
            p->drawLine( x1 + 5, y1 + 1 + 5,
                         x1 + 1, y1 + 5 + 5 );
            p->drawLine( x1 + 5, y1 + 1 + 5*2,
                         x1 + 1, y1 + 5 + 5*2 );
        }

        break;
    }

    case PE_FocusRect: {
        p->drawWinFocusRect( r );
        break;
    }

    case PE_ProgressBarChunk: {
        drawGradientBox( p, r, cg, cdata, false, 0.92, 1.66 );
        break;
    }

    case PE_ArrowUp:
    case PE_ArrowDown:
    case PE_ArrowRight:
    case PE_ArrowLeft: {
        QPointArray a;

        switch ( pe ) {
        case PE_ArrowUp:
            a.setPoints( 11,   3, 1,  0, -2,  -3, 1,  -2, 0,  -2, 2,  0, -1,  2, 1,  2, 2,  0, 0,  -1, 1,  1, 1 );
            break;

        case PE_ArrowDown:
            a.setPoints( 11,   3, -1,  0, 2,  -3, -1,  -2, 0,  -2, -2,  0, 1,  2, -1,  2, -2,  0, 0,  -1, -1,  1, -1 );
            break;

        case PE_ArrowRight:
            a.setPoints( 13,  0, -3,  -1, -2,  1, -2,  2, -1,  0, -1,  1, 0,  3, 0,  2, 1,  0, 1,  -1, 2,  1, 2,  0, 3,  0, 0 );
            break;

        case PE_ArrowLeft:
            a.setPoints( 13,  0, -3,  1, -2,  -1, -2,  -2, -1,  0, -1,  -1, 0,  -3, 0,  -2, 1,  0, 1,  1, 2,  -1, 2,  0, 3,  0, 0 );
            break;

        default:
            break;
        }

        if ( a.isNull() )
            return;

        p->save();
        a.translate(( r.x() + r.width() / 2 ),
                    ( r.y() + r.height() / 2 ) );
        if ( flags & Style_Enabled ) {
            p->setPen( flags & ( Style_Active | Style_MouseOver ) ? cg.highlightedText() : cg.buttonText() );
        } else {
            p->setPen( cdata->shades[7] );
        }
        p->drawPolyline( a );
        p->restore();

        break;
    }

    case PE_SpinWidgetUp:
    case PE_SpinWidgetDown: {
        QCommonStyle::drawPrimitive( PE_ButtonDefault, p, r, cg, flags, data );
        p->setPen( cdata->shades[7] );
        p->setBrush( QBrush::NoBrush );
        QPointArray a;
        if ( pe == PE_SpinWidgetUp ) {
            a.setPoints( 8,  -3, 0,  -1, -2,  1, 0,  1, 1,  -1, -1,  -3, 1,  -3, 0,  1, 0 );
        } else {
            a.setPoints( 8,  -3, -1,  -1, 1,  1, -1,  1, -2,  -1, 0,  -3, -2,  -3, -1,  1, -1 );
        }
        a.translate( r.x() + r.width() / 2, r.y() + r.height() / 2 );
        p->drawPolyline( a );
        break;
    }

    default:
        QCommonStyle::drawPrimitive( pe, p, r, cg, flags, data );
        break;
    }
}

void BluecurveStyle::drawControl( ControlElement control,
                                  QPainter *p,
                                  const QWidget *widget,
                                  const QRect &r,
                                  const QColorGroup &cg,
                                  SFlags flags,
                                  const QStyleOption &data ) const
{
    const BluecurveColorData *cdata = lookupData( cg );

    if ( widget == singleton->hoverWidget ) {
        flags |= Style_MouseOver;
    }

    switch ( control ) {
    case CE_TabBarTab: {
        const QTabBar* tb = static_cast<const QTabBar*>( widget );
        bool below = false;
        QRect tr( r );
        QRect fr( r );

        tr.addCoords( 0, 0,  0, -1 );
        fr.addCoords( 2, 2, -2, -2 );

        if ( tb->shape() == QTabBar::RoundedBelow || tb->shape() == QTabBar::TriangularBelow ) {
            tr = r;
            tr.addCoords( 0, 1, 0, 0 );
            fr = r;
            fr.addCoords( 2, 2, -2, -2 );
            below = true;
        }

        if ( !( flags & Style_Selected ) ) {
            if ( below ) {
                tr.addCoords( 0, 0, 0, -1 );
                fr.addCoords( 0, 0, 0, -1 );
            } else {
                tr.addCoords( 0, 1, 0, 0 );
                fr.addCoords( 0, 1, 0, 0 );
            }

            p->setPen( cdata->shades[6] );
            p->drawRect( tr );

            if ( tr.left() == 0 )
                if ( below )
                    p->drawPoint( tr.left(), tr.top() - 1 );
                else
                    p->drawPoint( tr.left(), tr.bottom() + 1 );

            p->setPen( cg.light() );
            if ( !below ) {
                p->drawLine( tr.left() + 1, tr.top() + 1,
                             tr.right() - 1, tr.top() + 1 );
                if ( tr.left() == 0 )
                    p->drawLine( tr.left() + 1, tr.bottom() + 1,
                                 tr.right(), tr.bottom() + 1 );
                else
                    p->drawLine( tr.left(), tr.bottom() + 1,
                                 tr.right(), tr.bottom() + 1 );
            }

            p->setPen( cdata->shades[2] );
            if ( below ) {
                p->drawLine( tr.left() + 1, tr.bottom() - 1,
                             tr.right() - 1, tr.bottom() - 1 );
                if ( tr.left() == 0 )
                    p->drawLine( tr.left() + 1, tr.top() - 1,
                                 tr.right(), tr.top() - 1 );
                else
                    p->drawLine( tr.left(), tr.top() - 1,
                                 tr.right(), tr.top() - 1 );
                p->drawLine( tr.left() + 1, tr.top() + 1,
                             tr.left() + 1, tr.bottom() - 2 );
                p->drawLine( tr.right() - 1, tr.top() + 1,
                             tr.right() - 1, tr.bottom() - 2 );
            } else {
                p->drawLine( tr.left() + 1, tr.top() + 2,
                             tr.left() + 1, tr.bottom() - 1 );
                p->drawLine( tr.right() - 1, tr.top() + 2,
                             tr.right() - 1, tr.bottom() - 1 );
            }
        } else {
            p->setPen( cdata->shades[6] );
            if ( below ) {
                p->drawLine( tr.left(), tr.bottom() - 1,
                             tr.left(), tr.top() - 1 );
                p->drawLine( tr.left(), tr.bottom(),
                             tr.right(), tr.bottom() );
                p->drawLine( tr.right(), tr.top(),
                             tr.right(), tr.bottom() - 1 );
            } else {
                p->drawLine( tr.left(), tr.bottom() + 1,
                             tr.left(), tr.top() + 1 );
                p->drawLine( tr.left(), tr.top(),
                             tr.right(), tr.top() );
                p->drawLine( tr.right(), tr.top() + 1,
                             tr.right(), tr.bottom() );
            }

            p->setPen( cg.light() );
            if ( below ) {
                p->drawLine( tr.left() + 1, tr.bottom() - 1,
                             tr.left() + 1, tr.top() - 2 );
                p->drawPoint( tr.right(), tr.top() - 1 );
            } else {
                p->drawLine( tr.left() + 1, tr.bottom() + 2,
                             tr.left() + 1, tr.top() + 2 );
                if ( tr.left() != 0 )
                    p->drawPoint( tr.left(), tr.bottom() + 1 );

                p->drawLine( tr.left() + 1, tr.top() + 1,
                             tr.right() - 1, tr.top() + 1 );
                p->drawPoint( tr.right(), tr.bottom() + 1 );
            }

            p->setPen( cdata->shades[2] );
            if ( below ) {
                if ( tr.left() != 0 )
                    p->drawPoint( tr.left(), tr.top() - 1 );
                p->drawLine( tr.left() + 2, tr.bottom() - 1,
                             tr.right() - 1, tr.bottom() - 1 );
                p->drawLine( tr.right() - 1, tr.top() - 1,
                             tr.right() - 1, tr.bottom() - 2 );
            } else {
                p->drawLine( tr.right() - 1, tr.top() + 2,
                             tr.right() - 1, tr.bottom() + 1 );
            }
        }

        p->fillRect( fr, (( flags & Style_Selected ) ?
                          cg.background() : cdata->shades[2] ) );
        break;
    }

    case CE_PopupMenuItem: {
        if ( ! widget || data.isDefault() )
            break;

        const QPopupMenu *popupmenu = static_cast< const QPopupMenu * >( widget );
        QMenuItem *mi = data.menuItem();
        int tab = data.tabWidth();
        int maxpmw = data.maxIconWidth();

        if ( mi && mi->isSeparator() ) {
            // draw separator
            p->fillRect( r, cg.brush( QColorGroup::Button ) );
            p->setPen( cg.mid().dark( 120 ) );
            p->drawLine( r.left(), r.top() + 1,
                         r.right(), r.top() + 1 );
            p->setPen( cg.light() );
            p->drawLine( r.left(),  r.top() + 2,
                         r.right(), r.top() + 2 );
            break;
        }

        if (( flags & Style_Active ) && ( flags & Style_Enabled ) ) {
            drawGradientBox( p, r, cg, cdata, false, 0.9, 1.2 );
        } else
            p->fillRect( r, cg.brush( QColorGroup::Button ) );

        if ( !mi )
            break;

        maxpmw = QMAX( maxpmw, 16 );

        QRect cr, ir, tr, sr;
        // check column
        cr.setRect( r.left(), r.top(), maxpmw, r.height() );
        // submenu indicator column
        sr.setCoords( r.right() - maxpmw, r.top(), r.right(), r.bottom() );
        // tab/accelerator column
        tr.setCoords( sr.left() - tab - 4, r.top(), sr.left(), r.bottom() );
        // item column
        ir.setCoords( cr.right() + 4, r.top(), tr.right() - 4, r.bottom() );

        bool reverse = QApplication::reverseLayout();
        if ( reverse ) {
            cr = visualRect( cr, r );
            sr = visualRect( sr, r );
            tr = visualRect( tr, r );
            ir = visualRect( ir, r );
        }

        if ( mi->iconSet() ) {
            QIconSet::Mode mode =
                ( flags & Style_Enabled ) ? QIconSet::Normal : QIconSet::Disabled;
            if (( flags & Style_Active ) && ( flags & Style_Enabled ) )
                mode = QIconSet::Active;
            QPixmap pixmap;
            if ( popupmenu->isCheckable() && mi->isChecked() )
                pixmap = mi->iconSet()->pixmap( QIconSet::Small, mode, QIconSet::On );
            else
                pixmap = mi->iconSet()->pixmap( QIconSet::Small, mode );
            QRect pmr( QPoint( 0, 0 ), pixmap.size() );
            pmr.moveCenter( cr.center() );
            p->setPen( cg.text() );
            p->drawPixmap( pmr.topLeft(), pixmap );
        } else if ( popupmenu->isCheckable() && mi->isChecked() )
            drawPrimitive( PE_CheckMark, p, cr, cg,
                           ( flags & ( Style_Enabled | Style_Active ) ) | Style_On );

        QColor textcolor;
        QColor embosscolor;
        if ( flags & Style_Active ) {
            if ( !( flags & Style_Enabled ) ) {
                textcolor = cg.text();
                embosscolor = cg.light();
            } else {
                textcolor = cg.highlightedText();
                embosscolor = cg.midlight().light();
            }
        } else if ( !( flags & Style_Enabled ) ) {
            textcolor = cg.text();
            embosscolor = cg.light();
        } else
            textcolor = embosscolor = cg.buttonText();
        p->setPen( textcolor );

        if ( mi->custom() ) {
            p->save();
            if ( !( flags & Style_Enabled ) ) {
                p->setPen( cg.light() );
                mi->custom()->paint( p, cg, ( flags & Style_Enabled ) ? ( flags & Style_Active ) : 0,
                                     flags & Style_Enabled,
                                     ir.x() + 1, ir.y() + 1,
                                     ir.width() - 1, ir.height() - 1 );
                p->setPen( textcolor );
            }
            mi->custom()->paint( p, cg, ( flags & Style_Enabled ) ? ( flags & Style_Active ) : 0,
                                 flags & Style_Enabled,
                                 ir.x(), ir.y(),
                                 ir.width(), ir.height() );
            p->restore();
        }

        QString text = mi->text();
        if ( ! text.isNull() ) {
            int t = text.find( '\t' );

            // draw accelerator/tab-text
            if ( t >= 0 ) {
                int alignFlag = AlignVCenter | ShowPrefix | DontClip | SingleLine;
                alignFlag |= ( reverse ? AlignLeft : AlignRight );
                if ( !( flags & Style_Enabled ) ) {
                    p->setPen( embosscolor );
                    tr.moveBy( 1, 1 );
                    p->drawText( tr, alignFlag, text.mid( t + 1 ) );
                    tr.moveBy( -1, -1 );
                    p->setPen( textcolor );
                }

                p->drawText( tr, alignFlag, text.mid( t + 1 ) );
            }

            int alignFlag = AlignVCenter | ShowPrefix | DontClip | SingleLine;
            alignFlag |= ( reverse ? AlignRight : AlignLeft );

            if ( !( flags & Style_Enabled ) ) {
                p->setPen( embosscolor );
                ir.moveBy( 1, 1 );
                p->drawText( ir, alignFlag, text, t );
                ir.moveBy( -1, -1 );
                p->setPen( textcolor );
            }

            p->drawText( ir, alignFlag, text, t );
        } else if ( mi->pixmap() ) {
            QPixmap pixmap = *mi->pixmap();
            if ( pixmap.depth() == 1 )
                p->setBackgroundMode( OpaqueMode );
            p->drawPixmap( ir.x(), ir.y() + ( ir.height() - pixmap.height() ) / 2, pixmap );
            if ( pixmap.depth() == 1 )
                p->setBackgroundMode( TransparentMode );
        }

        if ( mi->popup() )
            drawPrimitive(( reverse ? PE_ArrowLeft : PE_ArrowRight ), p, sr, cg, flags );
        break;
    }

#if (QT_VERSION-0 >= 0x030100)
    case CE_MenuBarEmptyArea: {
        p->fillRect( r, cg.brush( QColorGroup::Button ) );
        break;
    }
#endif

#if (QT_VERSION-0 >= 0x030100)
    case CE_DockWindowEmptyArea: {
        p->fillRect( r, cg.brush( QColorGroup::Button ) );
        break;
    }
#endif

    case CE_MenuBarItem: {
        if (( flags & Style_Enabled ) && ( flags & Style_Active ) &&
                ( flags & Style_Down ) )
            drawGradientBox( p, r, cg, cdata, false, 0.9, 1.2 );
        else
            p->fillRect( r, cg.brush( QColorGroup::Button ) );

        if ( data.isDefault() )
            break;

        QMenuItem *mi = data.menuItem();
        if ( flags & Style_Active && ( flags & Style_Down ) )
            drawItem( p, r, AlignCenter | ShowPrefix | DontClip | SingleLine, cg, flags & Style_Enabled, mi->pixmap(), mi->text(), -1, &cg.highlightedText() );
        else
            drawItem( p, r, AlignCenter | ShowPrefix | DontClip | SingleLine, cg, flags & Style_Enabled, mi->pixmap(), mi->text(), -1, &cg.buttonText() );
        break;
    }

    case CE_ProgressBarGroove:
        p->setBrush( cdata->shades[3] );
        p->setPen( cdata->shades[5] );
        p->drawRect( r );
        break;

    case CE_ProgressBarContents: {
        const QProgressBar *progressbar = ( const QProgressBar * ) widget;
        bool reverse = QApplication::reverseLayout();

        QRect pr;

        if ( !progressbar->totalSteps() ) {
            int w, remains;

            // draw busy indicator

            w = MIN( 25, r.width() / 2 );
            w = MAX( w, 1 );

            remains = r.width() - w;
            remains = MAX( remains, 1 );
            int x = progressbar->progress() % ( remains * 2 );
            if ( x > remains )
                x = 2 * remains - x;

            x = reverse ? r.right() - x - w : x + r.left();
            pr.setRect( x, r.top(), w, r.height() );
        } else {
            int pos = progressbar->progress();
            int total = progressbar->totalSteps() ? progressbar->totalSteps() : 1;
            int w = ( int )((( double )pos * r.width() ) / total );

            if ( reverse )
                pr.setRect( r.right() - w, r.top(), w, r.height() );
            else
                pr.setRect( r.left(), r.top(), w, r.height() );
        }
        drawGradientBox( p, pr, cg, cdata, false, 0.92, 1.66 );

        break;
    }

    case CE_CheckBox:
        drawPrimitive( PE_Indicator, p, r, cg, flags, data );
        break;

    case CE_CheckBoxLabel: {
        const QCheckBox *checkbox = ( const QCheckBox * ) widget;

        if ( flags & Style_MouseOver ) {
            QRegion r( checkbox->rect() );
            r -= visualRect( subRect( SR_CheckBoxIndicator, widget ), widget );
            p->setClipRegion( r );
            p->fillRect( checkbox->rect(), cg.brush( QColorGroup::Midlight ) );
            p->setClipping( FALSE );
        }
        int alignment = QApplication::reverseLayout() ? AlignRight : AlignLeft;
        drawItem( p, r, alignment | AlignVCenter | ShowPrefix, cg,
                  flags & Style_Enabled, checkbox->pixmap(), checkbox->text() );

        if ( checkbox->hasFocus() ) {
            QRect fr = visualRect( subRect( SR_CheckBoxFocusRect, widget ), widget );
            drawPrimitive( PE_FocusRect, p, fr, cg, flags );
        }
    }
    break;

    case CE_RadioButtonLabel: {
        const QRadioButton *radiobutton = ( const QRadioButton * ) widget;

        if ( flags & Style_MouseOver ) {
            QRegion r( radiobutton->rect() );
            r -= visualRect( subRect( SR_RadioButtonIndicator, widget ), widget );
            p->setClipRegion( r );
            p->fillRect( radiobutton->rect(), cg.brush( QColorGroup::Midlight ) );
            p->setClipping( FALSE );
        }

        int alignment = QApplication::reverseLayout() ? AlignRight : AlignLeft;
        drawItem( p, r, alignment | AlignVCenter | ShowPrefix, cg,
                  flags & Style_Enabled, radiobutton->pixmap(), radiobutton->text() );

        if ( radiobutton->hasFocus() ) {
            QRect fr = visualRect( subRect( SR_RadioButtonFocusRect, widget ), widget );
            drawPrimitive( PE_FocusRect, p, fr, cg, flags );
        }
    }
    break;
    default:
        QCommonStyle::drawControl( control, p, widget, r, cg, flags, data );
        break;
    }
}

void BluecurveStyle::drawControlMask( ControlElement control,
                                      QPainter *p,
                                      const QWidget *widget,
                                      const QRect &r,
                                      const QStyleOption &data ) const
{
    switch ( control ) {
    case CE_PushButton:
        p->fillRect( r, color1 );
        break;

    default:
        QCommonStyle::drawControlMask( control, p, widget, r, data );
        break;
    }
}

QRect BluecurveStyle::subRect( SubRect subrect, const QWidget *widget ) const
{
    QRect rect, wrect( widget->rect() );

    switch ( subrect ) {
    case SR_PushButtonFocusRect: {
        const QPushButton *button = ( const QPushButton * ) widget;
        int dbw1 = 0, dbw2 = 0;
        if ( button->isDefault() || button->autoDefault() ) {
            dbw1 = pixelMetric( PM_ButtonDefaultIndicator, widget );
            dbw2 = dbw1 * 2;
        }

        rect.setRect( wrect.x()   + 3 + dbw1,
                      wrect.y()   + 3 + dbw1,
                      wrect.width()  - 6 - dbw2,
                      wrect.height() - 6 - dbw2 );
        break;
    }

    case SR_CheckBoxIndicator: {
        int h = pixelMetric( PM_IndicatorHeight );
        rect.setRect(( widget->rect().height() - h ) / 2,
                     ( widget->rect().height() - h ) / 2,
                     pixelMetric( PM_IndicatorWidth ), h );
        break;
    }

    case SR_RadioButtonIndicator: {
        int h = pixelMetric( PM_ExclusiveIndicatorHeight );
        rect.setRect(( widget->rect().height() - h ) / 2,
                     ( widget->rect().height() - h ) / 2,
                     pixelMetric( PM_ExclusiveIndicatorWidth ), h );
        break;
    }

    default:
        rect = QCommonStyle::subRect( subrect, widget );
    }

    return rect;
}

void BluecurveStyle::drawComplexControl( ComplexControl control,
        QPainter* p,
        const QWidget* widget,
        const QRect& r,
        const QColorGroup& cg,
        SFlags flags,
        SCFlags controls,
        SCFlags active,
        const QStyleOption &data ) const
{
    const BluecurveColorData *cdata = lookupData( cg );

    if ( widget == singleton->hoverWidget )
        flags |= Style_MouseOver;

    switch ( control ) {
    case CC_ListView: {
        if ( controls & SC_ListView )
            QCommonStyle::drawComplexControl( control, p, widget, r, cg, flags, controls, active, data );
        if ( controls & ( SC_ListViewBranch | SC_ListViewExpand ) ) {
            if ( data.isDefault() )
                break;

            QListViewItem *item = data.listViewItem(),
                                  *child = item->firstChild();

            int y = r.y();
            int c;
            int dotoffset = 0;
            QPointArray dotlines;
            if ( active == SC_All && controls == SC_ListViewExpand ) {
                c = 2;
                dotlines.resize( 2 );
                dotlines[0] = QPoint( r.right(), r.top() );
                dotlines[1] = QPoint( r.right(), r.bottom() );
            } else {
                int linetop = 0, linebot = 0;
                // each branch needs at most two lines, ie. four end points
                dotoffset = ( item->itemPos() + item->height() - y ) % 2;
                dotlines.resize( item->childCount() * 4 );
                c = 0;

                // skip the stuff above the exposed rectangle
                while ( child && y + child->height() <= 0 ) {
                    y += child->totalHeight();
                    child = child->nextSibling();
                }

                int bx = r.width() / 2;

                // paint stuff in the magical area
                QListView* v = item->listView();
                int lh;
                if ( !item->multiLinesEnabled() )
                    lh = child ? child->height() : 0;
                else
                    lh = p->fontMetrics().height() + 2 * v->itemMargin();
                lh = QMAX( lh, QApplication::globalStrut().height() );
                if ( lh % 2 > 0 )
                    lh++;
                while ( child && y < r.height() ) {
                    linebot = y + lh / 2;
                    if (( child->isExpandable() || child->childCount() ) &&
                            ( child->height() > 0 ) ) {
                        // needs a box
                        p->setPen( cg.mid() );
                        p->drawRect( bx - 4, linebot - 4, 9, 9 );
                        // plus or minus
                        p->setPen( cg.text() );
                        p->drawLine( bx - 2, linebot, bx + 2, linebot );
                        if ( !child->isOpen() )
                            p->drawLine( bx, linebot - 2, bx, linebot + 2 );
                        // dotlinery
                        p->setPen( cg.mid() );
                        dotlines[c++] = QPoint( bx, linetop );
                        dotlines[c++] = QPoint( bx, linebot - 4 );
                        dotlines[c++] = QPoint( bx + 5, linebot );
                        dotlines[c++] = QPoint( r.width(), linebot );
                        linetop = linebot + 5;
                    } else {
                        // just dotlinery
                        dotlines[c++] = QPoint( bx + 1, linebot );
                        dotlines[c++] = QPoint( r.width(), linebot );
                    }

                    y += child->totalHeight();
                    child = child->nextSibling();
                }

                if ( child ) // there's a child, so move linebot to edge of rectangle
                    linebot = r.height();

                if ( linetop < linebot ) {
                    dotlines[c++] = QPoint( bx, linetop );
                    dotlines[c++] = QPoint( bx, linebot );
                }
            }
            p->setPen( cg.text() );

            static QBitmap *verticalLine = 0, *horizontalLine = 0;
            static QCleanupHandler<QBitmap> qlv_cleanup_bitmap;
            if ( !verticalLine ) {
                // make 128*1 and 1*128 bitmaps that can be used for
                // drawing the right sort of lines.
                verticalLine = new QBitmap( 1, 129, TRUE );
                horizontalLine = new QBitmap( 128, 1, TRUE );
                QPointArray a( 64 );
                QPainter p;
                p.begin( verticalLine );
                int i;
                for ( i = 0; i < 64; i++ )
                    a.setPoint( i, 0, i*2 + 1 );
                p.setPen( color1 );
                p.drawPoints( a );
                p.end();
                QApplication::flushX();
                verticalLine->setMask( *verticalLine );
                p.begin( horizontalLine );
                for ( i = 0; i < 64; i++ )
                    a.setPoint( i, i*2 + 1, 0 );
                p.setPen( color1 );
                p.drawPoints( a );
                p.end();
                QApplication::flushX();
                horizontalLine->setMask( *horizontalLine );
                qlv_cleanup_bitmap.add( &verticalLine );
                qlv_cleanup_bitmap.add( &horizontalLine );
            }

            int line; // index into dotlines
            for ( line = 0; line < c; line += 2 ) {
                // assumptions here: lines are horizontal or vertical.
                // lines always start with the numerically lowest
                // coordinate.

                // point ... relevant coordinate of current point
                // end ..... same coordinate of the end of the current line
                // other ... the other coordinate of the current point/line
                if ( dotlines[line].y() == dotlines[line+1].y() ) {
                    int end = dotlines[line+1].x();
                    int point = dotlines[line].x();
                    int other = dotlines[line].y();
                    while ( point < end ) {
                        int i = 128;
                        if ( i + point > end )
                            i = end - point;
                        p->drawPixmap( point, other, *horizontalLine,
                                       0, 0, i, 1 );
                        point += i;
                    }
                } else {
                    int end = dotlines[line+1].y();
                    int point = dotlines[line].y();
                    int other = dotlines[line].x();
                    int pixmapoffset = (( point & 1 ) != dotoffset ) ? 1 : 0;
                    while ( point < end ) {
                        int i = 128;
                        if ( i + point > end )
                            i = end - point;
                        p->drawPixmap( other, point, *verticalLine,
                                       0, pixmapoffset, 1, i );
                        point += i;
                    }
                }
            }
        }
        break;
    }
    case CC_ComboBox: {
        const QComboBox *combobox = ( const QComboBox * ) widget;
        QRect frame, arrow, field;

        frame = QStyle::visualRect( querySubControlMetrics( CC_ComboBox, widget, SC_ComboBoxFrame, data ), widget );
        arrow = QStyle::visualRect( querySubControlMetrics( CC_ComboBox, widget, SC_ComboBoxArrow, data ), widget );
        field = QStyle::visualRect( querySubControlMetrics( CC_ComboBox, widget, SC_ComboBoxEditField, data ), widget );

        if (( controls & SC_ComboBoxFrame ) && frame.isValid() ) {
            if ( flags & QStyle::Style_MouseOver )
                drawLightBevel( p, frame, cg, flags | Style_Raised, &cg.brush( QColorGroup::Midlight ) );
            else
                drawLightBevel( p, frame, cg, flags | Style_Raised, &cg.brush( QColorGroup::Button ) );
        }

        if (( controls & SC_ComboBoxArrow ) && arrow.isValid() ) {
            if ( active == SC_ComboBoxArrow )
                p->fillRect( arrow, cg.brush( QColorGroup::Dark ) );
            //arrow.addCoords(4, 2, -2, -6);
            drawPrimitive( PE_ArrowDown, p, arrow, cg, flags & ~Style_MouseOver );
            p->setPen( cdata->shades[3] );
            p->drawLine(( arrow.left() + arrow.right() ) / 2 - 1,
                        ( arrow.top() + arrow.bottom() ) / 2 + 5,
                        ( arrow.left() + arrow.right() ) / 2 + 3,
                        ( arrow.top() + arrow.bottom() ) / 2 + 5 );
            p->drawLine(( arrow.left() + arrow.right() ) / 2 - 1,
                        ( arrow.top() + arrow.bottom() ) / 2 + 6,
                        ( arrow.left() + arrow.right() ) / 2 + 3,
                        ( arrow.top() + arrow.bottom() ) / 2 + 6 );
        }

        if (( controls & SC_ComboBoxEditField ) && field.isValid() ) {
            p->setPen( cdata->shades[4] );
            if ( combobox->editable() ) {
                field.addCoords( -1, -1, 1, 1 );
                p->drawRect( field );
            } else {
                p->drawLine( field.right() + 1, field.top() - 2,
                             field.right() + 1, field.bottom() + 2 );
                p->setPen( cg.light() );
                p->drawLine( field.right() + 2, field.top() - 1,
                             field.right() + 2, field.bottom() + 1 );
            }

            if ( flags & Style_HasFocus ) {
                if ( ! combobox->editable() ) {
                    QRect fr = QStyle::visualRect( subRect( SR_ComboBoxFocusRect, widget ), widget );
                    drawPrimitive( PE_FocusRect, p, fr, cg, flags | Style_FocusAtBorder, QStyleOption( cg.highlight() ) );
                }

            }
            if ( flags & Style_Enabled )
                p->setPen( cg.buttonText() );
            else
                p->setPen( cg.mid() );
        }

        break;
    }

    case CC_SpinWidget: {
        const QSpinWidget *spinwidget = ( const QSpinWidget * ) widget;
        QRect frame, up, down;

        frame = querySubControlMetrics( CC_SpinWidget, widget, SC_SpinWidgetFrame, data );
        up = spinwidget->upRect();
        down = spinwidget->downRect();

        if (( controls & SC_SpinWidgetFrame ) && frame.isValid() )
            drawTextRect( p, frame, cg, flags | Style_Sunken, &cg.brush( QColorGroup::Base ) );

        p->setPen( cdata->shades[5] );
        p->drawLine( up.topLeft(), down.bottomLeft() );
        p->drawLine( up.left(), up.bottom() + 1, up.right(), up.bottom() + 1 );

        if (( controls & SC_SpinWidgetUp ) && up.isValid() ) {
            PrimitiveElement pe = PE_SpinWidgetUp;
            if ( spinwidget->buttonSymbols() == QSpinWidget::PlusMinus )
                pe = PE_SpinWidgetPlus;

            up.addCoords( 1, 0, 0, 0 );
            p->fillRect( up, cg.brush( QColorGroup::Button ) );
            if ( active == SC_SpinWidgetUp )
                p->setPen( cdata->shades[2] );
            else
                p->setPen( cg.light() );
            p->drawLine( up.left(), up.top(), up.right(), up.top() );
            p->drawLine( up.left(), up.top(), up.left(), up.bottom() );
            if ( active == SC_SpinWidgetUp )
                p->setPen( cg.light() );
            else
                p->setPen( cdata->shades[2] );
            p->drawLine( up.right(), up.top() + 1, up.right(), up.bottom() );
            p->drawLine( up.left() + 1, up.bottom(), up.right(), up.bottom() );

            up.addCoords( 1, 0, 0, 0 );
            drawPrimitive( pe, p, up, cg, flags |
                           (( active == SC_SpinWidgetUp ) ?
                            Style_On | Style_Sunken : Style_Raised ) );
        }

        if (( controls & SC_SpinWidgetDown ) && down.isValid() ) {
            PrimitiveElement pe = PE_SpinWidgetDown;
            if ( spinwidget->buttonSymbols() == QSpinWidget::PlusMinus )
                pe = PE_SpinWidgetMinus;

            down.addCoords( 1, 0, 0, 0 );
            p->fillRect( down, cg.brush( QColorGroup::Button ) );
            if ( active == SC_SpinWidgetDown )
                p->setPen( cdata->shades[2] );
            else
                p->setPen( cg.light() );
            p->drawLine( down.left(), down.top(), down.right(), down.top() );
            p->drawLine( down.left(), down.top(), down.left(), down.bottom() );
            if ( active == SC_SpinWidgetDown )
                p->setPen( cg.light() );
            else
                p->setPen( cdata->shades[2] );
            p->drawLine( down.right(), down.top() + 1,
                         down.right(), down.bottom() );
            p->drawLine( down.left() + 1, down.bottom(),
                         down.right(), down.bottom() );

            down.addCoords( 1, 0, 0, 0 );
            drawPrimitive( pe, p, down, cg, flags |
                           (( active == SC_SpinWidgetDown ) ?
                            Style_On | Style_Sunken : Style_Raised ) );
        }

        break;
    }

    case CC_ScrollBar: {
        const QScrollBar *scrollbar = ( const QScrollBar * ) widget;
        QRect addline, subline, addpage, subpage, slider, first, last;
        bool maxedOut = ( scrollbar->minValue() == scrollbar->maxValue() );

        subline = querySubControlMetrics( control, widget, SC_ScrollBarSubLine, data );
        addline = querySubControlMetrics( control, widget, SC_ScrollBarAddLine, data );
        subpage = querySubControlMetrics( control, widget, SC_ScrollBarSubPage, data );
        addpage = querySubControlMetrics( control, widget, SC_ScrollBarAddPage, data );
        slider  = querySubControlMetrics( control, widget, SC_ScrollBarSlider,  data );
        first   = querySubControlMetrics( control, widget, SC_ScrollBarFirst,   data );
        last = querySubControlMetrics( control, widget, SC_ScrollBarLast, data );

        if (( controls & SC_ScrollBarSubLine ) && subline.isValid() ) {
            drawPrimitive( PE_ScrollBarSubLine, p, subline, cg, Style_Enabled | (( active == SC_ScrollBarSubLine ) ? Style_Down : Style_Default ) | (( scrollbar->orientation() == Qt::Horizontal ) ? Style_Horizontal : 0 ) );
        }
        if (( controls & SC_ScrollBarAddLine ) && addline.isValid() )
            drawPrimitive( PE_ScrollBarAddLine, p, addline, cg, Style_Enabled | (( active == SC_ScrollBarAddLine ) ? Style_Down : Style_Default ) | (( scrollbar->orientation() == Qt::Horizontal ) ? Style_Horizontal : 0 ) );
        if (( controls & SC_ScrollBarSubPage ) && subpage.isValid() )
            drawPrimitive( PE_ScrollBarSubPage, p, subpage, cg, Style_Enabled | (( active == SC_ScrollBarSubPage ) ? Style_Down : Style_Default ) | (( scrollbar->orientation() == Qt::Horizontal ) ? Style_Horizontal : 0 ) );
        if (( controls & SC_ScrollBarAddPage ) && addpage.isValid() )
            drawPrimitive( PE_ScrollBarAddPage, p, addpage, cg, (( maxedOut ) ? Style_Default : Style_Enabled ) | (( active == SC_ScrollBarAddPage ) ? Style_Down : Style_Default ) | (( scrollbar->orientation() == Qt::Horizontal ) ? Style_Horizontal : 0 ) );
        if (( controls & SC_ScrollBarFirst ) && first.isValid() )
            drawPrimitive( PE_ScrollBarFirst, p, first, cg, Style_Enabled | (( active == SC_ScrollBarFirst ) ? Style_Down : Style_Default ) | (( scrollbar->orientation() == Qt::Horizontal ) ? Style_Horizontal : 0 ) );
        if (( controls & SC_ScrollBarLast ) && last.isValid() )
            drawPrimitive( PE_ScrollBarLast, p, last, cg, Style_Enabled | (( active == SC_ScrollBarLast ) ? Style_Down : Style_Default ) | (( scrollbar->orientation() == Qt::Horizontal ) ? Style_Horizontal : 0 ) );
        if (( controls & SC_ScrollBarSlider ) && slider.isValid() ) {
            drawPrimitive( PE_ScrollBarSlider, p, slider, cg, Style_Enabled | (( active == SC_ScrollBarSlider ) ? Style_Down : Style_Default ) | (( scrollbar->orientation() == Qt::Horizontal ) ? Style_Horizontal : 0 ) );

            // ### perhaps this should not be able to accept focus if maxedOut?
            if ( scrollbar->hasFocus() ) {
                QRect fr( slider.x() + 2, slider.y() + 2, slider.width() - 5, slider.height() - 5 );
                drawPrimitive( PE_FocusRect, p, fr, cg, Style_Default );
            }
        }
        break;
    }

    case CC_Slider: {
        const QSlider *slider = ( const QSlider * ) widget;
        QRect groove = querySubControlMetrics( CC_Slider, widget, SC_SliderGroove, data ),
                       handle = querySubControlMetrics( CC_Slider, widget, SC_SliderHandle, data );

        if (( controls & SC_SliderGroove ) && groove.isValid() ) {
            if ( flags & Style_HasFocus )
                drawPrimitive( PE_FocusRect, p, groove, cg );

            if ( slider->orientation() == Qt::Horizontal ) {
                int dh = ( groove.height() - 5 ) / 2;
                groove.addCoords( 0, dh, 0, -dh );
                handle.addCoords( 0, 1, 0, -1 );
            } else {
                int dw = ( groove.width() - 5 ) / 2;
                groove.addCoords( dw, 0, -dw, 0 );
                handle.addCoords( 1, 0, -1, 0 );
            }

            p->setPen( cdata->shades[5] );
            p->setBrush( cg.mid() );
            p->drawRect( groove );
            p->setPen( cdata->shades[4] );
            p->drawLine( groove.left() + 1, groove.top() + 1,
                         groove.left() + 1, groove.bottom() - 1 );
            p->drawLine( groove.left() + 1, groove.top() + 1,
                         groove.right() - 1, groove.top() + 1 );
        }

        if (( controls & SC_SliderHandle ) && handle.isValid() ) {
            p->setPen( cdata->shades[6] );
            p->drawLine( handle.x() + 2, handle.y(),
                         handle.right() - 2, handle.y() );
            p->drawLine( handle.x(), handle.y() + 2,
                         handle.x(), handle.bottom() - 2 );
            p->drawLine( handle.right(), handle.y() + 2,
                         handle.right(), handle.bottom() - 2 );
            p->drawLine( handle.x() + 2, handle.bottom(),
                         handle.right() - 2, handle.bottom() );
            p->drawPoint( handle.x() + 1, handle.y() + 1 );
            p->drawPoint( handle.right() - 1, handle.y() + 1 );
            p->drawPoint( handle.right() - 1, handle.bottom() - 1 );
            p->drawPoint( handle.x() + 1, handle.bottom() - 1 );


            p->setPen( cdata->shades[2] );
            p->drawLine( handle.x() + 2, handle.bottom() - 1,
                         handle.right() - 2, handle.bottom() - 1 );
            p->drawLine( handle.right() - 1, handle.top() + 2,
                         handle.right() - 1, handle.bottom() - 2 );
            p->drawPoint( handle.x() + 1, handle.y() );
            p->drawPoint( handle.right() - 1, handle.y() );
            p->drawPoint( handle.x(), handle.y() + 1 );
            p->drawPoint( handle.right(), handle.y() + 1 );
            p->drawPoint( handle.x(), handle.bottom() - 1 );
            p->drawPoint( handle.right(), handle.bottom() - 1 );
            p->drawPoint( handle.x() + 1, handle.bottom() );
            p->drawPoint( handle.right() - 1, handle.bottom() );

            p->setPen( Qt::white );
            p->drawLine( handle.x() + 2, handle.y() + 1,
                         handle.right() - 2, handle.y() + 1 );
            p->drawLine( handle.x() + 1, handle.y() + 2,
                         handle.x() + 1, handle.bottom() - 2 );


            p->setBrush( cg.button() );
            QRect fillr( handle );
            fillr.addCoords( 2, 2, -2, -2 );
            p->fillRect( fillr, cg.brush( QColorGroup::Button ) );


            if ( slider->orientation() == Qt::Horizontal ) {
                int x1 = handle.x() + handle.width() / 2 - 8;
                int y1 = handle.y() + ( handle.height() - 6 ) / 2;
                p->setPen( cdata->shades[5] );
                p->drawLine( x1 + 5, y1,
                             x1, y1 + 5 );
                p->drawLine( x1 + 5 + 5, y1,
                             x1 + 5, y1 + 5 );
                p->drawLine( x1 + 5 + 5*2, y1,
                             x1 + 5*2, y1 + 5 );

                p->setPen( Qt::white );
                p->drawLine( x1 + 5, y1 + 1,
                             x1 + 1, y1 + 5 );
                p->drawLine( x1 + 5 + 5, y1 + 2,
                             x1 + 5 + 1, y1 + 5 );
                p->drawLine( x1 + 5 + 5*2, y1 + 1,
                             x1 + 5*2 + 1, y1 + 5 );
            } else {
                int x1 = handle.x() + ( handle.width() - 6 ) / 2;
                int y1 = handle.y() + handle.height() / 2 - 8;
                p->setPen( cdata->shades[5] );
                p->drawLine( x1 + 5, y1,
                             x1, y1 + 5 );
                p->drawLine( x1 + 5, y1 + 5,
                             x1, y1 + 5 + 5 );
                p->drawLine( x1 + 5, y1 + 5*2,
                             x1, y1 + 5 + 5*2 );
                p->setPen( Qt::white );
                p->drawLine( x1 + 5, y1 + 1,
                             x1 + 1, y1 + 5 );
                p->drawLine( x1 + 5, y1 + 5 + 1,
                             x1 + 1, y1 + 5 + 5 );
                p->drawLine( x1 + 5, y1 + 5*2 + 1,
                             x1 + 1, y1 + 5 + 5*2 );
            }
        }

        if ( controls & SC_SliderTickmarks )
            QCommonStyle::drawComplexControl( control, p, widget, r, cg, flags, SC_SliderTickmarks, active, data );
        break;
    }

    default:
        QCommonStyle::drawComplexControl( control, p, widget, r, cg, flags,
                                          controls, active, data );
        break;
    }
}

QRect BluecurveStyle::querySubControlMetrics( ComplexControl control,
        const QWidget *widget,
        SubControl sc,
        const QStyleOption &data ) const
{
    QRect ret;

    switch ( control ) {
    case CC_ScrollBar: {
        const QScrollBar *scrollbar = ( const QScrollBar * ) widget;
        int sliderstart = scrollbar->sliderStart();
        int sbextent = pixelMetric( PM_ScrollBarExtent, widget );
        int maxlen = (( scrollbar->orientation() == Qt::Horizontal ) ?
                      scrollbar->width() : scrollbar->height() ) - ( sbextent * 3 );
        int sliderlen;

        // calculate slider length
        if ( scrollbar->maxValue() != scrollbar->minValue() ) {
            uint range = scrollbar->maxValue() - scrollbar->minValue();
            sliderlen = ( scrollbar->pageStep() * maxlen ) /
                        ( range + scrollbar->pageStep() );

            int slidermin = pixelMetric( PM_ScrollBarSliderMin, widget );
            if ( sliderlen < slidermin || range > INT_MAX / 2 )
                sliderlen = slidermin;
            if ( sliderlen > maxlen )
                sliderlen = maxlen;
        } else
            sliderlen = maxlen;

        switch ( sc ) {
        case SC_ScrollBarSubLine:
            // top/left button
            ret.setRect( 0, 0, sbextent, sbextent );
            break;

        case SC_ScrollBarAddLine:
            // bottom/right button
            if ( scrollbar->orientation() == Qt::Horizontal )
                ret.setRect( scrollbar->width() - sbextent, 0, sbextent, sbextent );
            else
                ret.setRect( 0, scrollbar->height() - sbextent, sbextent, sbextent );
            break;

        case SC_ScrollBarSubPage:
            // between top/left button and slider
            if ( scrollbar->orientation() == Qt::Horizontal )
                ret.setRect( sbextent, 0, sliderstart - sbextent, sbextent );
            else
                ret.setRect( 0, sbextent, sbextent, sliderstart - sbextent );
            break;

        case SC_ScrollBarAddPage:
            // between bottom/right button and slider
            if ( scrollbar->orientation() == Qt::Horizontal )
                ret.setRect( sliderstart + sliderlen, 0,
                             maxlen - sliderstart - sliderlen + sbextent * 2, sbextent );
            else
                ret.setRect( 0, sliderstart + sliderlen,
                             sbextent, maxlen - sliderstart - sliderlen + sbextent * 2 );
            break;

        case SC_ScrollBarGroove:
            if ( scrollbar->orientation() == Qt::Horizontal )
                ret.setRect( sbextent, 0, scrollbar->width() - sbextent * 2,
                             scrollbar->height() );
            else
                ret.setRect( 0, sbextent, scrollbar->width(),
                             scrollbar->height() - sbextent * 2 );
            break;

        case SC_ScrollBarSlider:
            if ( scrollbar->orientation() == Qt::Horizontal )
                ret.setRect( sliderstart, 0, sliderlen, sbextent );
            else
                ret.setRect( 0, sliderstart, sbextent, sliderlen );
            break;

        default:
            break;
        }

        break;
    }

    case CC_SpinWidget: {
        int fw = pixelMetric( PM_SpinBoxFrameWidth, widget );
        QSize bs;
        bs.setHeight( widget->height() / 2 - fw );
        if ( bs.height() < 8 )
            bs.setHeight( 8 );
        bs.setWidth( bs.height() * 8 / 6 );
        bs = bs.expandedTo( QApplication::globalStrut() );
        int y = fw;
        int x, lx, rx;
        x = widget->width() - y - bs.width() + 1;
        lx = fw;
        rx = x - fw;
        switch ( sc ) {
        case SC_SpinWidgetUp:
            ret.setRect( x, y - 1, bs.width(), bs.height() + 1 );
            break;
        case SC_SpinWidgetDown:
            ret.setRect( x, y + bs.height() + 1, bs.width(), bs.height() + 1 );
            break;
        case SC_SpinWidgetButtonField:
            ret.setRect( x, y, bs.width() + 1, widget->height() - 2*fw );
            break;
        case SC_SpinWidgetEditField:
            ret.setRect( lx, fw, rx, widget->height() - 2*fw );
            break;
        case SC_SpinWidgetFrame:
            ret = widget->rect();
        default:
            break;
        }

        break;
    }
    default:
        ret = QCommonStyle::querySubControlMetrics( control, widget, sc, data );
        break;
    }

    return ret;
}

QStyle::SubControl BluecurveStyle::querySubControl( ComplexControl control,
        const QWidget *widget,
        const QPoint &pos,
        const QStyleOption &data ) const
{
    QStyle::SubControl ret = QCommonStyle::querySubControl( control, widget, pos, data );

    return ret;
}

int BluecurveStyle::pixelMetric( PixelMetric metric, const QWidget *widget ) const
{
    int ret;

    switch ( metric ) {
    case PM_ButtonMargin:
        ret = 10;
        break;

    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:
        ret = 0;
        break;

    case PM_ButtonDefaultIndicator:
        ret = 1;
        break;
    case PM_DefaultFrameWidth:
        ret = 2;
        break;

    case PM_IndicatorWidth:
    case PM_IndicatorHeight:
    case PM_ExclusiveIndicatorWidth:
    case PM_ExclusiveIndicatorHeight:
        ret = 13;
        break;

    case PM_TabBarTabOverlap:
        ret = 1;
        break;

    case PM_ScrollBarExtent:
        ret = 15;
        break;

    case PM_MenuBarFrameWidth:
        ret = 2;
        break;

    case PM_ProgressBarChunkWidth:
        ret = 2;
        break;

    case PM_DockWindowSeparatorExtent:
        ret = 4;
        break;
    case PM_DockWindowHandleExtent:
        ret = 10;

    case PM_SplitterWidth:
        ret = 8;
        break;

    case PM_ScrollBarSliderMin:
        ret = 31;
        break;

    case PM_SliderControlThickness:
        ret = basestyle->pixelMetric( metric, widget );
        break;

    case PM_SliderLength: {
        ret = 31;
        if ( widget->inherits( "QSlider" ) ) {
            const QSlider *slider = static_cast<const QSlider*>( widget );
            if ( slider->orientation() == Qt::Horizontal ) {
                if ( widget->width() < ret )
                    ret = widget->width();
            } else {
                if ( widget->height() < ret )
                    ret = widget->height();
            }
        }
    }
    break;

    case PM_MaximumDragDistance:
        ret = -1;
        break;

    default:
        ret = QCommonStyle::pixelMetric( metric, widget );
        break;
    }

    return ret;
}

QSize BluecurveStyle::sizeFromContents( ContentsType contents,
                                        const QWidget *widget,
                                        const QSize &contentsSize,
                                        const QStyleOption &data ) const
{
    QSize ret;

    switch ( contents ) {
    case CT_PushButton: {
        const QPushButton *button = ( const QPushButton * ) widget;
        ret = QCommonStyle::sizeFromContents( contents, widget, contentsSize, data );
        int w = ret.width(), h = ret.height();

        // only expand the button if we are displaying text...
        if ( ! button->pixmap() ) {
            if ( button->isDefault() || button->autoDefault() ) {
                // default button minimum size
                if ( w < 80 )
                    w = 80;
                if ( h < 25 )
                    h = 25;
            } else {
                // regular button minimum size
                if ( w < 76 )
                    w = 76;
                if ( h < 21 )
                    h = 21;
            }
        }

        ret = QSize( w, h );
        break;
    }

    case CT_PopupMenuItem: {
        if ( ! widget || data.isDefault() )
            break;

        QMenuItem *mi = data.menuItem();
        const QPopupMenu *popupmenu = ( const QPopupMenu * ) widget;
        int maxpmw = data.maxIconWidth();
        int w = contentsSize.width(), h = contentsSize.height();

        if ( mi->custom() ) {
            w = mi->custom()->sizeHint().width();
            h = mi->custom()->sizeHint().height();
            if ( ! mi->custom()->fullSpan() && h < 22 )
                h = 22;
        } else if ( mi->widget() ) {
        } else if ( mi->isSeparator() ) {
            w = 10;
            h = 4;
        } else {
            // check is at least 16x16
            if ( h < 16 )
                h = 16;
            if ( mi->pixmap() )
                h = QMAX( h, mi->pixmap()->height() );
            else if ( ! mi->text().isNull() )
                h = QMAX( h, popupmenu->fontMetrics().height() + 2 );
            if ( mi->iconSet() != 0 )
                h = QMAX( h, mi->iconSet()->pixmap( QIconSet::Small,
                                                    QIconSet::Normal ).height() );
            h += 2;
        }

        // check | 4 pixels | item | 8 pixels | accel | 4 pixels | check

        // check is at least 16x16
        maxpmw = QMAX( maxpmw, 16 );
        w += ( maxpmw * 2 ) + 8;

        if ( ! mi->text().isNull() && mi->text().find( '\t' ) >= 0 )
            w += 8;

        ret = QSize( w, h );
        break;
    }

    case CT_ComboBox: {
        ret = QCommonStyle::sizeFromContents( contents, widget,
                                              contentsSize, data );
        int w = ret.width(), h = ret.height();
        if ( h < 27 )
            h = 27;

        ret = QSize( w, h );
        break;
    }

    default:
        ret = QCommonStyle::sizeFromContents( contents, widget, contentsSize, data );
        break;
    }

    return ret;
}

int BluecurveStyle::styleHint( StyleHint stylehint,
                               const QWidget *widget,
                               const QStyleOption &option,
                               QStyleHintReturn* returnData ) const
{
    int ret;

    switch ( stylehint ) {
    case SH_EtchDisabledText:
    case SH_Slider_SnapToValue:
    case SH_PrintDialog_RightAlignButtons:
    case SH_FontDialog_SelectAssociatedText:
    case SH_MenuBar_AltKeyNavigation:
    case SH_MenuBar_MouseTracking:
    case SH_PopupMenu_MouseTracking:
    case SH_PopupMenu_SpaceActivatesItem:
    case SH_ComboBox_ListMouseTracking:
    case SH_ScrollBar_MiddleClickAbsolutePosition:
        ret = 1;
        break;

    case SH_MainWindow_SpaceBelowMenuBar:
        ret = 0;
        break;

    case SH_PopupMenu_AllowActiveAndDisabled:
        ret = 0;
        break;

    default:
        ret = QCommonStyle::styleHint( stylehint, widget, option, returnData );
        break;
    }

    return ret;
}

QPixmap BluecurveStyle::stylePixmap( StylePixmap stylepixmap,
                                     const QWidget *widget,
                                     const QStyleOption &data ) const
{
    return basestyle->stylePixmap( stylepixmap, widget, data );
}

void BluecurveStyle::drawGradient( QPainter *p, QRect const &rect,
                                   const QColorGroup &cg,
                                   double shade1, double shade2,
                                   bool horiz ) const
{
    QColor c, c1, c2;
    int r, g, b;
    int c2r, c2g, c2b;
    int dr, dg, db, size;
    int start, end, left, right, top, bottom;

    if ( horiz ) {
        start = rect.left();
        end = rect.right();
        top = rect.top();
        bottom = rect.bottom();
    } else {
        start = rect.top();
        end = rect.bottom();
        left = rect.left();
        right = rect.right();
    }

    if ( end == start )
        return;

    shade( cg.highlight(), c1, shade1 );
    shade( cg.highlight(), c2, shade2 );

    c1.rgb( &r, &g, &b );
    c2.rgb( &c2r, &c2g, &c2b );

    size = end - start;
    dr = ( c2r - r ) / size;
    dg = ( c2g - g ) / size;
    db = ( c2b - b ) / size;

    for ( int i = start; i <= end; i++ ) {
        c.setRgb( r, g, b );
        p->setPen( c );
        if ( horiz )
            p->drawLine( i, top, i, bottom );
        else
            p->drawLine( left, i, right, i );

        r += dr;
        g += dg;
        b += db;
    }
}
void BluecurveStyle::drawGradientBox( QPainter *p, QRect const &r,
                                      const QColorGroup &cg,
                                      const BluecurveColorData *cdata,
                                      bool horiz,
                                      double shade1, double shade2 ) const
{
    QRect grad( r.left() + 2, r.top() + 2, r.width() - 3, r.height() - 3 );
    drawGradient( p, grad, cg, shade1, shade2, horiz );

    // 3d border effect...
    p->setPen( cdata->spots[1] );
    p->setBrush( NoBrush );
    p->drawRect( r );
    p->setPen( cdata->spots[0] );
    p->drawLine( r.left() + 1, r.top() + 1, r.right() - 1, r.top() + 1 );
    p->drawLine( r.left() + 1, r.top() + 1, r.left() + 1, r.bottom() - 1 );
    p->setPen( cdata->spots[2] );
    p->drawLine( r.left() + 1, r.bottom() - 1, r.right() - 1, r.bottom() - 1 );
    p->drawLine( r.right() - 1, r.top() + 1, r.right() - 1, r.bottom() - 1 );
}

// vim:ts=4:sw=4
