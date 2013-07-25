#include <qgfxdriverplugin_qws.h>
#include <qgfxsnap_qws.h>

class GfxSNAPDriver : public QGfxDriverPlugin
{
public:
    GfxSNAPDriver();

    QStringList keys() const;
    QScreen *create( const QString&, int displayId );
};

GfxSNAPDriver::GfxSNAPDriver()
: QGfxDriverPlugin()
{
}

QStringList GfxSNAPDriver::keys() const
{
    QStringList list;
    list << "SNAP";
    return list;
}

QScreen* GfxSNAPDriver::create( const QString& driver, int displayId )
{
    if ( driver.lower() == "snap" )
        return new QSNAPScreen( displayId );

    return 0;
}

Q_EXPORT_PLUGIN( GfxSNAPDriver )
