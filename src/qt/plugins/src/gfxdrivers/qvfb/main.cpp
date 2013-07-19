#include <qgfxdriverplugin_qws.h>
#include <qgfxvfb_qws.h>

class GfxVfbDriver : public QGfxDriverPlugin
{
public:
    GfxVfbDriver();

    QStringList keys() const;
    QScreen *create( const QString&, int displayId );
};

GfxVfbDriver::GfxVfbDriver()
: QGfxDriverPlugin()
{
}

QStringList GfxVfbDriver::keys() const
{
    QStringList list;
    list << "QVFb";
    return list;
}

QScreen* GfxVfbDriver::create( const QString& driver, int displayId )
{
    if ( driver.lower() == "qvfb" )
        return new QVFbScreen( displayId );

    return 0;
}

Q_EXPORT_PLUGIN( GfxVfbDriver )
