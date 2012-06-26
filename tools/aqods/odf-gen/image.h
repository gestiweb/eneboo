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

#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>

#include "odstype.h"
#include "length.h"

class Image 
{
public:
    Image( const std::string &name,
           const Length &width,
           const Length &height,
           const Length &x,
           const Length &y,
           const std::string &link )
        : _name( name ),
          _width( width.str() ),
          _height( height.str() ),
          _x( x.str() ),
          _y( y.str() ),
          _link( link )
    {}

    std::ostream& operator << ( std::ostream& ostream ) const
    {
        ostream << "<image name=\"" << _name << "\""
                << "       width=\"" << _width << "\""
                << "       height=\"" << _height << "\""
                << "       x=\"" << _x << "\""
                << "       y=\"" << _y << "\""
                << "       link=\"" << _link << "\""
                << ">"        
                << "</image>";
        return ostream;
    }

private:
    std::string _name,
                _width,
                _height,
                _x,
                _y,
                _link;
};

MAP_ODS_TYPE(Image, image);

inline
std::ostream& operator << ( std::ostream &ostream,
                            const Image& image ) 
{
    return image.operator << (ostream);
}

#endif // IMAGE_H
