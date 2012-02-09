/*
odf-gen: Simple API to generate OpenDocument documents.
    Copyright (C) 2009  Pablo Jorge, FuDePAN

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

#ifndef CELLADDRESS_H
#define CELLADDRESS_H

#include <iostream>

class CellAddress 
{
public:
    CellAddress( const std::string &sheet,
                 unsigned int column,
                 unsigned int row )
        : _sheet( sheet ),
          _column( column ),
          _row( row )
    {}

    std::ostream& operator << ( std::ostream& ostream ) const
    {
        ostream << "'" << _sheet << "'" 
                << "." << (char)('A' + _column - 1) // XXX
                << _row;
        return ostream;
    }

private:
    std::string _sheet;
    unsigned int _column,
                 _row;
};

inline
std::ostream& operator << ( std::ostream &ostream,
                            const CellAddress& address ) 
{
    return address.operator << (ostream);
}

class CellRange 
{
public:
    CellRange( const CellAddress &start,
               const CellAddress &end )
        : _start( start ),
          _end( end )
    {}

    std::ostream& operator << ( std::ostream& ostream ) const
    {
        ostream << _start << ":" << _end;
        return ostream;
    }

private:
    CellAddress _start,
                _end;
};

inline 
std::ostream& operator << ( std::ostream &ostream,
                            const CellRange& range ) 
{
    return range.operator << (ostream);
}

#endif // CELLADDRESS_H
