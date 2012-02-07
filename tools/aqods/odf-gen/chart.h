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

#ifndef CHART_H
#define CHART_H

#include <iostream>
#include <list>

#include "odstype.h"
#include "celladdress.h"
#include "color.h"
#include "length.h"

class Series 
{
public:
    Series( const CellAddress& name,
            const CellRange& domain,
            const CellRange& values,
            const Color& color )
        : _name( name ),
          _domain( domain ),
          _values( values ),
          _color( color )
    {}

    std::ostream& operator << ( std::ostream& ostream ) const
    {
        ostream << "<series name-address=\"" << _name << "\""
                << "        x-range=\"" << _domain << "\""
                << "        y-range=\"" << _values << "\""
                << "        color=\"" << _color << "\""
                << "/>";
        return ostream;
    }

private:
    CellAddress _name;
    CellRange _domain,
              _values;
    Color _color;
};

inline
std::ostream& operator << ( std::ostream &ostream,
                            const Series& series ) 
{
    return series.operator << (ostream);
}

class Chart 
{
public:
    Chart( const std::string &name,
           const Length &width,
           const Length &height )
        : _name( name ),
          _width( width.str() ),
          _height( height.str() )
    {}

    void add_range( const CellRange &range )
    {
        _range_list.push_back( range );
    }

    void add_series( const Series &series )
    {
        _series_list.push_back( series );
    }
    
    void set_title( const std::string& title )
    {
        _title = title;
    }

    void set_subtitle( const std::string& subtitle )
    {
        _subtitle = subtitle;
    }

    void set_x_axis_label( const std::string& x_axis_label )
    {
        _x_axis_label = x_axis_label;
    }

    void set_y_axis_label( const std::string& y_axis_label )
    {
        _y_axis_label = y_axis_label;
    }

    std::ostream& operator << ( std::ostream& ostream ) const
    {
        ostream << "<chart name=\"" << _name << "\""
                << "       width=\"" << _width << "\""
                << "       height=\"" << _height << "\""
                << "       title=\"" << _title << "\""
                << "       subtitle=\"" << _subtitle << "\""
                << "       x-axis-label=\"" << _x_axis_label << "\""
                << "       y-axis-label=\"" << _y_axis_label << "\""
                << "       range=\"";
        
        for( std::list< CellRange >::const_iterator 
             it  = _range_list.begin();
             it != _range_list.end();
             it++ )
            ostream << *it << ";";
        
        ostream << "\">";
        
        for( std::list< Series >::const_iterator 
             it  = _series_list.begin();
             it != _series_list.end();
             it++ )
            ostream << *it;
        
        ostream << "</chart>";
        
        return ostream;
    }

private:
    std::string _name,
                _width,
                _height,
                _title,
                _subtitle,
                _x_axis_label,
                _y_axis_label;
    std::list< CellRange > _range_list;
    std::list< Series > _series_list;
};

MAP_ODS_TYPE(Chart, object);

inline
std::ostream& operator << ( std::ostream &ostream,
                            const Chart& chart ) 
{
    return chart.operator << (ostream);
}

class AutoChart : public Chart
{
public:
    AutoChart( const std::string &name,
               const Length &width,
               const Length &height,
               const Sheet &sheet )
        : Chart( name, 
                 width, 
                 height ) 
    {
        ColorGenerator generator;

        add_range( CellRange( CellAddress( sheet.get_name(), 1, 1 ),
                              CellAddress( sheet.get_name(),
                                           sheet.get_columns(),
                                           sheet.get_rows() ) ) );

        for( unsigned int i = 2; i <= sheet.get_columns(); i++ ) 
        {
            CellAddress name( sheet.get_name(), i, 1 ),
                        domain_start( sheet.get_name(), 1, 2 ),
                        domain_end( sheet.get_name(), 1, sheet.get_rows() ),
                        values_start( sheet.get_name(), i, 2 ),
                        values_end( sheet.get_name(), i, sheet.get_rows() );
            CellRange domain( domain_start, domain_end ),
                      values( values_start, values_end );
            Color color( generator.next() );

            add_series( Series( name, domain, values, color ) );
        }
    }
};

MAP_ODS_TYPE(AutoChart, object);

#endif // CHART_H
