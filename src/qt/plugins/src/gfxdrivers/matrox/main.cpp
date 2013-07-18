#include <qgfxdriverplugin_qws.h>
#include <qgfxmatrox_qws.h>

class GfxMatroxDriver : public QGfxDriverPlugin
{
public:
    GfxMatroxDriver();

    QStringList keys() const;
    QScreen *create( const QString&, int displayId );
};

GfxMatroxDriver::GfxMatroxDriver()
: QGfxDriverPlugin()
{
}

QStringList GfxMatroxDriver::keys() const
{
    QStringList list;
    list << "Matrox";
    return list;
}

QScreen* GfxMatroxDriver::create( const QString& driver, int displayId )
{
    if ( driver.lower() == "matrox" )
        return new QMatroxScreen( displayId );

    return 0;
}

Q_EXPORT_PLUGIN( GfxMatroxDriver )
