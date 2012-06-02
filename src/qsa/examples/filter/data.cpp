#include "data.h"

Data::Data( int w, int h ) :
    wid( w ),
    hei( h )
{
    buffer = new DataBuffer( w*h );
    for ( int i=0; i<w*h; i++ )
	buffer->data[i] = 0;
}

Data::~Data()
{
    buffer->deref();
    if( buffer->data==0 )
	delete buffer;
}


int Data::value( int x, int y ) const
{
    if ( x>=0 && x<wid && y>=0 && y<hei )
	return buffer->data[ y*wid + x ];
    qWarning( "Data::value(), Accessing out of bounds" );
    return 0;
}

void Data::setValue( int x, int y, int val )
{
    if ( x>=0 && x<wid && y>=0 && y<hei )
	buffer->data[ y*wid + x ] = val;
    else
	qWarning( "Data::value(), Accessing out of bounds" );
}


Data::Data( const Data &d )
    : wid( d.wid ), hei( d.hei ), buffer( d.buffer )
{
    buffer->ref();
}


Data& Data::operator=( const Data &d )
{
    wid = d.wid;
    hei = d.hei;
    buffer->deref();
    buffer = d.buffer;
    buffer->ref();
    return *this;
}

DataBuffer::DataBuffer( int size )
    : data( new int[size] ), count( 1 )
{
}


void DataBuffer::ref()
{
    Q_ASSERT( data );
    Q_ASSERT( count>0 );
    count++;
}

void DataBuffer::deref()
{
    Q_ASSERT( data );
    Q_ASSERT( count>0 );

    if( --count == 0 ) {
	delete [] data;
	data = 0;
    }
}
