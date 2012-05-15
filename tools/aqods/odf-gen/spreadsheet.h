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

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <iostream>

#include "element.h"

class Spreadsheet : public Element
{
public:
    Spreadsheet( std::ostream &ostream = std::cout )
        : Element( ostream )
    {
        _ostream << "<?xml version=\"1.0\"?>"
                 << "<spreadsheet>";
    }
    
    virtual void close_()
    {
        _ostream << "</spreadsheet>";
    }
    
    ~Spreadsheet()
    {
        close();
    }
};

#endif // SPREADSHEET_H
