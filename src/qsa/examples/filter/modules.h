#ifndef MODULES_H
#define MODULES_H

#include <qevent.h>
#include <qimage.h>

#include "data.h"

class QSInterpreter;
class QLabel;

class Renderer : public QObject
{
    Q_OBJECT
    Q_OVERRIDE( QCString name SCRIPTABLE false )
public:
    Renderer( QSInterpreter *interp );
    ~Renderer();
public slots:
    void input( const Data &data );
private:
    QImage im;
    QSInterpreter *ip;
    QLabel *widget;
};

class ImgSource : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString file READ file WRITE setFile )
    Q_OVERRIDE( QCString name SCRIPTABLE false )
public:
    ImgSource( QSInterpreter *interp );
    void setFile( const QString &name );
    QString file() const { return fileName; }
public slots:
    void load();
signals:
    void output( const Data &d );
private:
    QString fileName;
    Data last;
    QString lastName;
    QSInterpreter *ip;
};

class Threshold : public QObject
{
    Q_OBJECT
    Q_PROPERTY( int steps READ steps WRITE setSteps )
    Q_OVERRIDE( QCString name SCRIPTABLE false )
public:
    Threshold() : step( 1 ) { }
    int steps() const { return step; }
    void setSteps( int s ) { if( s>0 ) step = s; }
public slots:
    void input( const Data &d );
signals:
    void output( const Data &d );
private:
    int step;
};

class BumpMapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QPoint light READ light WRITE setLight )
    Q_PROPERTY( int smoothness READ smoothness WRITE setSmoothness )
    Q_PROPERTY( int radius READ radius WRITE setRadius )
    Q_OVERRIDE( QCString name SCRIPTABLE false )
public:
    BumpMapper();
    QPoint light() const { return lp; }
    int smoothness() const { return smooth; }
    int radius() const { return rad; }
    void setLight( const QPoint &p ) { lp = p; }
    void setSmoothness( int smo ) { smooth = smo; }
    void setRadius( int ra ) { rad = ra; }
public slots:
    void input( const Data &d );
signals:
    void output( const Data &d );
private:
    QPoint lp;
    int smooth;
    int rad;
};

class Smudger : public QObject
{
    Q_OBJECT
    Q_PROPERTY( int iterations READ iterations WRITE setIterations )
    Q_OVERRIDE( QCString name SCRIPTABLE false )
public:
    Smudger();
    int iterations() const { return iter; }
    void setIterations( int it ) { iter = it; }
public slots:
    void input( const Data &d );
signals:
    void output( const Data &d );
private:
    int iter;
};

#endif


