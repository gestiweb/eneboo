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

#ifndef SPAN_H
#define SPAN_H

#include <iostream>

template < class K, class T >
class Span
{
public:
    Span( const T& value, unsigned int count )
        : _value( value ),
          _count( count )
    {}

    const T& value() const { return _value; }
    unsigned int count() const { return _count; }
    
private:
    T _value;
    unsigned int _count;
};

class ColumnK {};
class RowK {};

template < class T >
class ColumnSpan : public Span< ColumnK, T >
{
public:
    ColumnSpan( const T& value, unsigned int count )
        : Span< ColumnK, T >( value, count )
    {}
};

template < class T >
class RowSpan : public Span< RowK, T >
{
public:
    RowSpan( const T& value, unsigned int count )
        : Span< RowK, T >( value, count )
    {}
};

class CoveredCell {};
static const CoveredCell covered_cell = CoveredCell();

template < class T >
inline ColumnSpan< T > column_span( T value, unsigned int count )
{
    return ColumnSpan< T >( value, count );
}

template < class T >
inline RowSpan< T > row_span( T value, unsigned int count )
{
    return RowSpan< T >( value, count );
}

#endif // SPAN_H
