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

#ifndef LENGTH_H
#define LENGTH_H

#include <iostream>
#include <string>
#include <sstream>

class Length
{
public:
    Length( unsigned int value )
        : _value( value )
    {}
    
    virtual const char* unit() const
    {
        return "";
    }
    
    std::string str() const
    {
        std::stringstream stream;
        stream << _value << this->unit();
        return stream.str();
    }

private:
    unsigned int _value;
};

class Centimeters : public Length
{
public:
    Centimeters( unsigned int value )
        : Length( value )
    {}
    
    const char* unit() const
    {
        return "cm";
    }
};

#endif // LENGTH_H
