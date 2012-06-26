#ifndef DATA_H
#define DATA_H

#include <qobject.h>

class DataBuffer
{
    friend class Data;
    DataBuffer( int size = 0 );

    void ref();
    void deref();

private:
    int *data;
    int count;
};

class Data
{
public:
    Data( int w = 0, int h = 0 );
    ~Data();

    int width() const { return wid; }
    int height() const { return hei; }

    Data( const Data &data );
    Data& operator=( const Data &data );

    int value( int x, int y ) const;
    void setValue( int x, int y, int value );

private:
    int wid;
    int hei;
    DataBuffer *buffer;
};

#endif
