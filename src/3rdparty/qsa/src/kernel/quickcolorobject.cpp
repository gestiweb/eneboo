/****************************************************************************
**
** Copyright (C) 1992-2006 Trolltech AS. All rights reserved.
**
** This file is part of the QSA of the Qt Toolkit.
**
** For QSA Commercial License Holders (non-open source):
** 
** Licensees holding a valid Qt Script for Applications (QSA) License Agreement
** may use this file in accordance with the rights, responsibilities and
** obligations contained therein.  Please consult your licensing agreement or
** contact sales@trolltech.com if any conditions of the Licensing Agreement are
** not clear to you.
** 
** Further information about QSA licensing is available at:
** http://www.trolltech.com/products/qsa/licensing.html or by contacting
** info@trolltech.com.
** 
** 
** For Open Source Edition:  
** 
** This file may be used under the terms of the GNU General Public License
** version 2 as published by the Free Software Foundation and appearing in the
** file LICENSE.GPL included in the packaging of this file.  Please review the
** following information to ensure GNU General Public Licensing requirements
** will be met:  http://www.trolltech.com/products/qt/opensource.html 
** 
** If you are unsure which license is appropriate for your use, please review
** the following information:
** http://www.trolltech.com/products/qsa/licensing.html or contact the 
** sales department at sales@trolltech.com.

**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "quickcolorobject.h"
#include "quickinterpreter.h"
#include "qslexer.h"
#include <QColor>
#include <QPalette>
#include <QColorGroup>
#include <assert.h>

class ColorShared : public QSShared
{
public:
    ColorShared( const QColor &c ) : color( c )
    { }
    QColor color;
};

class PaletteShared : public QSShared
{
public:
    PaletteShared( const QPalette &p ) : palette( p )
    {}
    QPalette palette;
};

class ColorGroupShared : public QSShared
{
public:
    ColorGroupShared( const QSColorGroup &cg ) : colorGroup( cg )
    {}
    QSColorGroup colorGroup;
};

enum { Red, Green, Blue, Alpha, Name, Rgb, Rgba, Light, Dark, Hue, Saturation, Value };

void QSColorGroup::copyToPalette( QPalette *pal, QPalette::ColorGroup group ) const
{
    Q_ASSERT( pal != 0 );

    pal->setColorGroup( group,
                        palette.brush( QPalette::Foreground ),
                        palette.brush( QPalette::Button ),
                        palette.brush( QPalette::Light ),
                        palette.brush( QPalette::Dark ),
                        palette.brush( QPalette::Mid ),
                        palette.brush( QPalette::Text ),
                        palette.brush( QPalette::BrightText ),
                        palette.brush( QPalette::Base ),
                        palette.brush( QPalette::Background ) );
}

QSColorClass::QSColorClass( QSClass *b )
        : QSSharedClass( b )
{
    // custom properties
    addMember( REG_USTR( QLatin1String( "red" )), QSMember( QSMember::Custom, Red, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "green" )), QSMember( QSMember::Custom, Green, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "blue" )), QSMember( QSMember::Custom, Blue, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "alpha" )), QSMember( QSMember::Custom, Alpha, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "name" )), QSMember( QSMember::Custom, Name, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "rgb" )), QSMember( QSMember::Custom, Rgb, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "rgba" )), QSMember( QSMember::Custom, Rgba, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "hue" )), QSMember( QSMember::Custom, Hue, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "saturation" )), QSMember( QSMember::Custom, Saturation, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "value" )), QSMember( QSMember::Custom, Value, AttributeNone ) );

    // member functions
    addMember( REG_USTR( QLatin1String( "light" )), QSMember( &light ) );
    addMember( REG_USTR( QLatin1String( "dark" )), QSMember( &dark ) );
    addMember( REG_USTR( QLatin1String( "setRgb" )), QSMember( &setRgb ) );
    addMember( REG_USTR( QLatin1String( "setRgba" )), QSMember( &setRgba ) );
}

QColor *QSColorClass::color( const QSObject *obj )
{
    Q_ASSERT( obj->objectType() ->name() == QLatin1String( "Color" ) );
    return &( static_cast<ColorShared *>( obj->shVal() ) ->color );
}

#if QT_VERSION >= 0x030200
#define QSA_HSV_FUNCTION(a, b, c) getHsv( (a), (b), (c) )
#else
#define QSA_HSV_FUNCTION(a, b, c) hsv( (a), (b), (c) )
#endif

static inline int getHSV( QColor *col, int pos )
{
    int hsv[ 3 ];
    col->QSA_HSV_FUNCTION( &hsv[ 0 ], &( hsv[ 1 ] ), &( hsv[ 2 ] ) );
    return hsv[ pos ];
}

QSObject QSColorClass::fetchValue( const QSObject *objPtr,
                                   const QSMember &mem ) const
{
    if ( mem.type() != QSMember::Custom )
        return QSClass::fetchValue( objPtr, mem );
    QColor *col = color( objPtr );
    switch ( mem.idx ) {
    case Red:
        return createNumber( col->red() );
    case Green:
        return createNumber( col->green() );
    case Blue:
        return createNumber( col->blue() );
    case Alpha:
        return createNumber( col->alpha() );
    case Rgb:
        return createNumber( col->rgb() );
    case Rgba:
        return createNumber( col->rgba() );
    case Name:
        return createString( col->name() );
    case Hue:
        return createNumber( getHSV( col, 0 ) );
    case Saturation:
        return createNumber( getHSV( col, 1 ) );
    case Value:
        return createNumber( getHSV( col, 2 ) );
    default:
        qFatal( "QSColorClass::fetchValue: unhandled case" );
        return createUndefined();
    }
}

void QSColorClass::write( QSObject *objPtr, const QSMember &mem,
                          const QSObject &val ) const
{
    if ( mem.type() == QSMember::Custom ) {
        QColor * col = color( objPtr );
        if ( mem.idx == Name ) {
            col->setNamedColor( val.toString() );
        } else if ( mem.idx == Rgb ) {
            col->setRgb( val.toUInt32() );
        } else if ( mem.idx == Rgba ) {
            col->setRgba( val.toUInt32() );
        } else if ( mem.idx == Hue ) {
            int h, s, v;
            col->QSA_HSV_FUNCTION( &h, &s, &v );
            col->setHsv( ( int ) val.toNumber(), s, v );
        } else if ( mem.idx == Saturation ) {
            int h, s, v;
            col->QSA_HSV_FUNCTION( &h, &s, &v );
            col->setHsv( h, ( int ) val.toNumber(), v );
        } else if ( mem.idx == Value ) {
            int h, s, v;
            col->QSA_HSV_FUNCTION( &h, &s, &v );
            col->setHsv( h, s, ( int ) val.toNumber() );
        } else {
            int r = col->red();
            int g = col->green();
            int b = col->blue();
            int a = col->alpha();
            if ( mem.idx == Red )
                r = val.toInteger();
            else if ( mem.idx == Green )
                g = val.toInteger();
            else if ( mem.idx == Blue )
                b = val.toInteger();
            else if ( mem.idx == Alpha )
                a = val.toInteger();
            col->setRgb( r, g, b, a );
        }
    } else {
        QSClass::fetchValue( objPtr, mem );
    }
}

QString QSColorClass::toString( const QSObject *obj ) const
{
    return color( obj ) ->name();
}

double QSColorClass::toNumber( const QSObject *obj ) const
{
    return ( uint ) color( obj ) ->rgba();
}

QVariant QSColorClass::toVariant( const QSObject *obj, QVariant::Type type ) const
{
    switch ( type ) {
    case QVariant::Int: return QVariant( ( int ) color( obj ) ->rgba() );
    default: break;
    }
    return *color( obj );
}

QSObject QSColorClass::construct( const QSList &args ) const
{
    if ( args.size() > 0 ) {
        if ( args.size() == 1 ) {
            QSObject v( args[ 0 ] );
            if ( v.isA( "Color" ) ) {
                QSObject col = args.at( 0 );
                return construct( QColor( *color( &col ) ) );
            } else {
                return construct( QColor( v.toString() ) );
            }
        } else if ( args.size() == 3 ) {
            QColor c( args[ 0 ].toInteger(),
                      args[ 1 ].toInteger(),
                      args[ 2 ].toInteger() );
            return construct( c );
        } else if ( args.size() == 4 ) {
            QColor c( args[ 0 ].toInteger(),
                      args[ 1 ].toInteger(),
                      args[ 2 ].toInteger(),
                      args[ 3 ].toInteger() );
            return construct( c );
        }
    }

    return construct( QColor() );
}

QSObject QSColorClass::construct( const QColor &c ) const
{
    return QSObject( this, new ColorShared( c ) );
}

void QSColorClass::setRgb( QSEnv *env )
{
    QSObject t = env->thisValue();
    QColor *col = static_cast<const QSColorClass *>( t.objectType() ) ->color( &t );

    if ( env->numArgs() == 1 ) {
        col->setRgb( env->arg( 0 ).toInteger() );
    } else if ( env->numArgs() == 3 ) {
        col->setRgb( env->arg( 0 ).toInteger(),
                     env->arg( 1 ).toInteger(),
                     env->arg( 2 ).toUInt32() );
    }
}


void QSColorClass::setRgba( QSEnv *env )
{
    QSObject t = env->thisValue();
    QColor *col = static_cast<const QSColorClass *>( t.objectType() ) ->color( &t );

    if ( env->numArgs() == 1 ) {
        col->setRgba( env->arg( 0 ).toInteger() );
    } else if ( env->numArgs() == 4 ) {
        col->setRgba( qRgba( env->arg( 0 ).toInteger(),
                             env->arg( 1 ).toInteger(),
                             env->arg( 2 ).toInteger(),
                             env->arg( 3 ).toInteger() ) );
    }
}

QSObject QSColorClass::light( QSEnv *env )
{
    QSColorClass * cl = static_cast<QuickInterpreter *>( env->engine() ) ->colorClass();
    QSObject that = env->thisValue();
    return cl->construct( color( &that ) ->light() );
}

QSObject QSColorClass::dark( QSEnv *env )
{
    QSColorClass * cl = static_cast<QuickInterpreter *>( env->engine() ) ->colorClass();
    QSObject that = env->thisValue();
    return cl->construct( color( &that ) ->dark() );
}

QSPaletteClass::QSPaletteClass( QSClass *b )
        : QSSharedClass( b )
{
    addMember( REG_USTR( QLatin1String( "active" )), QSMember( QSMember::Custom, Active, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "inactive" )), QSMember( QSMember::Custom, Inactive, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "disabled" )), QSMember( QSMember::Custom, Disabled, AttributeNone ) );
};

QSObject QSPaletteClass::fetchValue( const QSObject *objPtr, const QSMember &mem ) const
{
    if ( mem.type() != QSMember::Custom )
        return QSClass::fetchValue( objPtr, mem );
    QSColorGroupClass *cgClass = static_cast<QuickInterpreter *>( env() ->engine() ) ->colorGroupClass();
    switch ( mem.idx ) {
    case Active:
        return cgClass->construct( QSColorGroup( *palette( objPtr ), QPalette::Active ) );
    case Inactive:
        return cgClass->construct( QSColorGroup( *palette( objPtr ), QPalette::Inactive ) );
    case Disabled:
        return cgClass->construct( QSColorGroup( *palette( objPtr ), QPalette::Disabled ) );
    default:
        qFatal( "QSPaletteClass::fetchValue() :: unhandled case" );
    }
    return QSObject();
}

void QSPaletteClass::write( QSObject *objPtr, const QSMember &mem, const QSObject &val ) const
{
    if ( mem.type() != QSMember::Custom ) {
        QSClass::write( objPtr, mem, val );
        return ;
    }

    QSColorGroupClass *cgClass = static_cast<QuickInterpreter *>( env() ->engine() ) ->colorGroupClass();
    if ( val.objectType() != cgClass ) {
        env() ->throwError( QString( QLatin1String( "Palette.%1: Value must be of type ColorGroup" ) )
                            .arg( mem.name() ) );
        return ;
    }
    QSColorGroup *colorGroup = QSColorGroupClass::colorGroup( &val );

    switch ( mem.idx ) {
    case Active:
        colorGroup->copyToPalette( palette( objPtr ), QPalette::Active );
        break;
    case Inactive:
        colorGroup->copyToPalette( palette( objPtr ), QPalette::Inactive );
        break;
    case Disabled:
        colorGroup->copyToPalette( palette( objPtr ), QPalette::Disabled );
        break;
    default:
        break;
    }
}


QSObject QSPaletteClass::construct( const QSList &args ) const
{
    if ( args.size() < 3 )
        return env() ->throwError( QLatin1String( "Palette constructor requires 3 parameters "
                                   "of type ColorGroup" ) );

    QSColorGroupClass *cgClass = static_cast<QuickInterpreter *>( env() ->engine() ) ->colorGroupClass();

    QSObject active = args[ 0 ];
    if ( active.objectType() != cgClass )
        return env() ->throwError( QLatin1String( "Palette constructor: Argument 1 is not "
                                   "of type ColorGroup" ) );

    QSObject disabled = args[ 1 ];
    if ( disabled.objectType() != cgClass )
        return env() ->throwError( QLatin1String( "Palette constructor: Argument 2 is not "
                                   "of type ColorGroup" ) );

    QSObject inactive = args[ 2 ];
    if ( inactive.objectType() != cgClass )
        return env() ->throwError( QLatin1String( "Palette constructor: Argument 3 is not "
                                   "of type ColorGroup" ) );

    QPalette tmppal;
    QSColorGroup *activecg = QSColorGroupClass::colorGroup( &active );
    QSColorGroup *inactivecg = QSColorGroupClass::colorGroup( &inactive );
    QSColorGroup *disabledcg = QSColorGroupClass::colorGroup( &disabled );
    activecg->copyToPalette( &tmppal, QPalette::Active );
    inactivecg->copyToPalette( &tmppal, QPalette::Inactive );
    disabledcg->copyToPalette( &tmppal, QPalette::Disabled );

    return construct( tmppal );
};


QSObject QSPaletteClass::construct( const QPalette &p ) const
{
    return QSObject( this, new PaletteShared( p ) );
}


QPalette *QSPaletteClass::palette( const QSObject *obj )
{
    Q_ASSERT( obj->objectType() ->name() == QLatin1String( "Palette" ) );
    return &( static_cast<PaletteShared *>( obj->shVal() ) ->palette );
};


QVariant QSPaletteClass::toVariant( const QSObject *obj, QVariant::Type type ) const
{
    switch ( type ) {
    case QVariant::Invalid:
    case QVariant::Palette:
        return QVariant( *palette( obj ) );
    default:
        break;
    }
    return QVariant();
}

QSColorGroupClass::QSColorGroupClass( QSClass *b )
        : QSSharedClass( b )
{
    addMember( REG_USTR( QLatin1String( "foreground" )), QSMember( QSMember::Custom, QPalette::Foreground, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "button" )), QSMember( QSMember::Custom, QPalette::Button, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "light" )), QSMember( QSMember::Custom, QPalette::Light, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "midlight" )), QSMember( QSMember::Custom, QPalette::Midlight, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "dark" )), QSMember( QSMember::Custom, QPalette::Dark, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "mid" )), QSMember( QSMember::Custom, QPalette::Mid, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "text" )), QSMember( QSMember::Custom, QPalette::Text, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "brightText" )), QSMember( QSMember::Custom, QPalette::BrightText, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "buttonText" )), QSMember( QSMember::Custom, QPalette::ButtonText, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "base" )), QSMember( QSMember::Custom, QPalette::Base, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "background" )), QSMember( QSMember::Custom, QPalette::Background, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "shadow" )), QSMember( QSMember::Custom, QPalette::Shadow, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "highlight" )), QSMember( QSMember::Custom, QPalette::Highlight, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "highlightedText" )), QSMember( QSMember::Custom, QPalette::HighlightedText,
               AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "link" )), QSMember( QSMember::Custom, QPalette::Link, AttributeNone ) );
    addMember( REG_USTR( QLatin1String( "linkVisited" )), QSMember( QSMember::Custom, QPalette::LinkVisited, AttributeNone ) );
}


QSObject QSColorGroupClass::fetchValue( const QSObject *objPtr, const QSMember &mem ) const
{
    if ( mem.type() != QSMember::Custom )
        return QSClass::fetchValue( objPtr, mem );
    QSColorClass *cl = static_cast<QuickInterpreter *>( env() ->engine() ) ->colorClass();
    QSColorGroup *cg = colorGroup( objPtr );
    Q_ASSERT( mem.idx >= 0 );

    return cl->construct( cg->color( QPalette::ColorRole( mem.idx ) ) );
}


void QSColorGroupClass::write( QSObject *objPtr, const QSMember &mem, const QSObject &val ) const
{
    if ( mem.type() != QSMember::Custom ) {
        QSClass::write( objPtr, mem, val );
        return ;
    }
    QSColorClass *cl = static_cast<QuickInterpreter *>( env() ->engine() ) ->colorClass();
    if ( val.objectType() != cl ) {
        env() ->throwError( QString( QLatin1String( "Property %1 must be of type Color" ) ).arg( mem.name() ) );
        return ;
    }
    QColor color = *cl->color( &val );
    QSColorGroup *cg = colorGroup( objPtr );
    Q_ASSERT( mem.idx >= 0 );
    cg->setColor( QPalette::ColorRole( mem.idx ), color );
}


QSObject QSColorGroupClass::construct( const QSList & ) const
{
    return construct( QSColorGroup() );
}


QSObject QSColorGroupClass::construct( const QSColorGroup &cg ) const
{
    return QSObject( this, new ColorGroupShared( cg ) );
}


QSColorGroup *QSColorGroupClass::colorGroup( const QSObject *obj )
{
    Q_ASSERT( obj->objectType() ->name() == QLatin1String( "ColorGroup" ) );
    return &( static_cast<ColorGroupShared *>( obj->shVal() ) ->colorGroup );
}

QVariant QSColorGroupClass::toVariant( const QSObject *, QVariant::Type ) const
{
    return QVariant();
}
