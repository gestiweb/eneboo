#include <qgfxdriverplugin_qws.h>
#include <qgfxvoodoo_qws.h>

class GfxVoodooDriver : public QGfxDriverPlugin
{
public:
    GfxVoodooDriver();

    QStringList keys() const;
    QScreen *create( const QString&, int displayId );
};

GfxVoodooDriver::GfxVoodooDriver()
: QGfxDriverPlugin()
{
}

QStringList GfxVoodooDriver::keys() const
{
    QStringList list;
    list << "Voodoo3";
    return list;
}

QScreen* GfxVoodooDriver::create( const QString& driver, int displayId )
{
    if ( driver.lower() == "voodoo3" )
        return new QVoodooScreen( displayId );

    return 0;
}

Q_EXPORT_PLUGIN( GfxVoodooDriver )
