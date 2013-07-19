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

#ifndef ROW_H
#define ROW_H

#include <iostream>

#include "odstype.h"

#include "element.h"
#include "sheet.h"

#include "celladdress.h"
#include "style.h"
#include "span.h"
#include "color.h"

class Row : public Element
{
public:
    Row( Sheet &sheet )
        : Element( sheet ),
          _sheet( sheet ),
          _column( 0 ),
      _row(0),
      _fixPrec(0) {
        _sheet.add_row();
        _row = _sheet.get_rows();
        _ostream << "<row>";
    }
    
    virtual void close_()
    {
        if( !!_style )
            add_cell( "" );

        _ostream << "</row>";
    }

    ~Row()
    {
        close();
    }
    
    template < class T >
    void begin_cell( const Style& style,
                     unsigned int column_span,
                     unsigned int row_span )
    {
        _ostream << "<cell style=\"" << style << "\"" 
                 << "      column-span=\"" << column_span << "\""
                 << "      row-span=\"" << row_span << "\""
                 << "      type=\"" << ODSType< T >::convert() << "\"";

    if (_bgcolor.isValid())
      _ostream << " bgcolor=\"" << _bgcolor << "\"";
    if (_fgcolor.isValid())
      _ostream << " fgcolor=\"" << _fgcolor << "\"";

      _ostream << ">";
    }

    void end_cell() 
    {
        _ostream << "</cell>";
    }

    template < class T >
    void add_value( const T& value )
    {
    if (_fixPrec > 0)
      _ostream << std::fixed << std::setprecision(_fixPrec) << value;
    else
        _ostream << value;
    }

    template < class T >
    CellAddress add_cell( const T& value,
                          unsigned int column_span,
                          unsigned int row_span )
    {
        // add the cell using the preset style
        begin_cell< T >( _style, column_span, row_span );
        add_value( value );
        end_cell();

        // reset style
        _style = Style::NONE;
        
        // increment current column count
        ++_column;
        
        // capture current cell address
        CellAddress address( _sheet.get_name(),
                             _column,
                             _row );

        // adjust (if necessary) the columns count
        _column += column_span ? column_span - 1 : 0;
        
        // if current column count > maximum column count
        // for all the rows in this sheet, update sheet's
        // column count
        if( _column > _sheet.get_columns() )
            _sheet.add_column();

        // return a valid CellAddress
        return address;
    }

    template < class T >
    CellAddress add_cell( const T& value )
    {
        return add_cell( value, 0, 0 );
    }
    
    void add_style( const Style& style )
    {
        _style |= style;
    }

   Row &add_bgcolor(const Color &color) {
    _bgcolor = color;
    return *this;
    }

   Row &add_fgcolor(const Color &color) {
    _fgcolor = color;
    return *this;
    }

    template < class T >
    Row& operator << ( const T& value )
    {
        add_cell( value );
        return *this;
    }

    Row& operator << ( const Style& style )
    {
        add_style( style );
        return *this;
    }
        
    template < class T >
    Row& operator << ( const ColumnSpan< T >& spanned_value )
    {
        add_cell( spanned_value.value(),
                  spanned_value.count(),
                  0 );
        return *this;
    }

    template < class T >
    Row& operator << ( const RowSpan< T >& spanned_value )
    {
        add_cell( spanned_value.value(),
                  0,
                  spanned_value.count() );
        return *this;
    }

    Row& operator << ( const CoveredCell& covered )
    {
        _ostream << "<cell covered=\"true\"/>";
        return *this;
    }
    
    Row &add_coveredcell() {
    _ostream << "<cell covered=\"true\"/>";
    return *this;
  }
  void set_fixed_precision(unsigned int p) {
    _fixPrec = p;
  }

private:
    Sheet &_sheet;

    Style _style;
    Color _bgcolor;
    Color _fgcolor;
  unsigned int _column, _row;
  unsigned int _fixPrec;
};

#endif // ROW_H
