/*
odf-gen: Simple API to generate OpenDocument documents.
    Copyright (C) 2012  Pablo Jorge, FuDePAN

    This file is part of the odf-gen project.

    odf-gen is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    odf-gen is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with odf-gen.  If not, see <http://www.gnu.org/licenses/>.
*/

/*  As a special exception, you may create a larger work that contains
    part of all of the odf-gen project and distribute that work under
    the terms of the GNU General Public License as published by the
    Free Software Foundation; version 2 of the License

    Alternatively, if you modify or redistribute the ODF generator
    itself, you may (ar your option) remove this special exception

    This special exception was added by Pablo Jorge in 
    version 1.3 of odf-gen. 
*/

#ifndef COLOR_H
#define COLOR_H

#include <iostream>
#include <iomanip>
#include <cstdlib>

class Color
{
public:
    Color()
        : _red( 0 ),
          _green( 0 ),
          _blue( 0 ),
	  _valid( false )
    {}

    Color( unsigned char red,
           unsigned char green,
           unsigned char blue )
        : _red( red ),
          _green( green ),
          _blue( blue ),
	  _valid( true )
    {}

    Color( unsigned int color )
        : _red( color >> 16 ),
          _green( color >> 8 ),
          _blue( color ),
	  _valid( true )
    {}

    bool isValid() const 
    {
        return _valid;
    }

    std::ostream& operator << ( std::ostream& ostream ) const
    {
        ostream << "#" 
                << std::hex 
                << std::setfill('0') 
                << std::setw(2) << static_cast< int >( _red )
                << std::setw(2) << static_cast< int >( _green )
                << std::setw(2) << static_cast< int >( _blue )
                << std::dec;
        return ostream;
    }

private:
    unsigned char _red,
                  _green,
                  _blue;
   bool _valid;
};

inline
std::ostream& operator << ( std::ostream &ostream, const Color& color ) 
{
    return color.operator << (ostream);
}

class ColorGenerator
{
public:
    ColorGenerator()
        : _idx( 0 )
    {
#if defined(Q_OS_WIN)
        srand( time( NULL ) );
#else
        srandom( time( NULL ) );
#endif
    }
    
    Color next()
    {
        static unsigned int palette[] = { 0x0000ff, 0xff0000, 0x00ff00,
                                          0xff00ff, 0xffff00, 0x00ffff };
        
        if( _idx < sizeof(palette) / sizeof(palette[0]) )
            return Color( palette[_idx++] );
#if defined(Q_OS_WIN)
        return Color( rand() );
#else
        return Color( random() );
#endif
    }

private:
    int _idx;
};

#endif // COLOR_H
