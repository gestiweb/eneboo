#include <qgfxdriverplugin_qws.h>
#include <qgfxshadowfb_qws.h>

class GfxShadowFbDriver : public QGfxDriverPlugin
{
public:
    GfxShadowFbDriver();

    QStringList keys() const;
    QScreen *create( const QString&, int displayId );
};

GfxShadowFbDriver::GfxShadowFbDriver()
: QGfxDriverPlugin()
{
}

QStringList GfxShadowFbDriver::keys() const
{
    QStringList list;
    list << "ShadowFb";
    return list;
}

QScreen* GfxShadowFbDriver::create( const QString& driver, int displayId )
{
    if ( driver.lower() == "shadowfb" )
        return new QShadowFbScreen( displayId );

    return 0;
}

Q_EXPORT_PLUGIN( GfxShadowFbDriver )
